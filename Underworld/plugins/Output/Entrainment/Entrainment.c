/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University 
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 		* Redistributions of source code must retain the above copyright notice, 
** 			this list of conditions and the following disclaimer.
** 		* Redistributions in binary form must reproduce the above copyright 
**			notice, this list of conditions and the following disclaimer in the 
**			documentation and/or other materials provided with the distribution.
** 		* Neither the name of the Monash University nor the names of its contributors 
**			may be used to endorse or promote products derived from this software 
**			without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
** THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
** PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS 
** BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
** OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**
** Contact:
*%  Louis.Moresi - Louis.Moresi@monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
** 
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/** \file 
\details Entrainment of particles 
**/
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>
#include <StgFEM/FrequentOutput/FrequentOutput.h>

#include "Entrainment.h"

const Type Underworld_Entrainment_Type = "Underworld_Entrainment";

void _Underworld_Entrainment_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
	Underworld_Entrainment*		self = (Underworld_Entrainment*)component;

	self->context = (AbstractContext*)Stg_ComponentFactory_PluginConstructByKey( cf, self, (Dictionary_Entry_Key)"Context", UnderworldContext, True, data  );
	self->matSwarm = Stg_ComponentFactory_PluginConstructByKey( cf, self, (Dictionary_Entry_Key)"MaterialPointsSwarm", MaterialPointsSwarm, True, data  );
	/* TODO: get other params */
	self->entrainedMaterial = Stg_ComponentFactory_PluginConstructByKey( cf, self, (Dictionary_Entry_Key)"EntrainedMaterial", Material, True, data );

	self->entrainmentY = Dictionary_GetDouble_WithDefault( self->context->dictionary,
		"EntrainmentY", 0 );
	self->entrainmentDirPositive = Dictionary_GetBool_WithDefault( self->context->dictionary,
		"EntrainmentDirPositive", True );
	self->entrainmentRatio = 0;

	/* Add functions to entry points */
	ContextEP_Append( self->context, AbstractContext_EP_FrequentOutput, Underworld_Entrainment_Output );

	StgFEM_FrequentOutput_PrintString( self->context, "Entrainment" );
}

void _Underworld_Entrainment_Build( void* component, void* data ) {
	Underworld_Entrainment*	self = (Underworld_Entrainment*)component;

   Stg_Component_Build( self->matSwarm, data, False );
   
   _Codelet_Build( component, data );
}

void _Underworld_Entrainment_Initialise( void* component, void* data ) {
	Underworld_Entrainment*	self = (Underworld_Entrainment*)component;

   Stg_Component_Initialise( self->matSwarm, data, False );
   
   _Codelet_Initialise( component, data );
}

void _Underworld_Entrainment_Destroy( void* component, void* data ) {
	Underworld_Entrainment*	self = (Underworld_Entrainment*)component;

   Stg_Component_Destroy( self->matSwarm, data, False );
   
   _Codelet_Destroy( component, data );
}

void* _Underworld_Entrainment_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Underworld_Entrainment);
	Type                                                      type = Underworld_Entrainment_Type;
	Stg_Class_DeleteFunction*                              _delete = _Codelet_Delete;
	Stg_Class_PrintFunction*                                _print = _Codelet_Print;
	Stg_Class_CopyFunction*                                  _copy = _Codelet_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Underworld_Entrainment_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Underworld_Entrainment_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Underworld_Entrainment_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Underworld_Entrainment_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Codelet_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Underworld_Entrainment_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return _Codelet_New(  CODELET_PASSARGS  );
}

Index Underworld_Entrainment_Register( PluginsManager* pluginsManager ) {
	return PluginsManager_Submit( pluginsManager, Underworld_Entrainment_Type, (Name)"0", _Underworld_Entrainment_DefaultNew );
}

void Underworld_Entrainment_Output( UnderworldContext* context ) {
	Underworld_Entrainment* self;
	Particle_Index          plI = 0;
	Material_Index          mI = 0;
	GlobalParticle*         p = NULL;
	double                  pY = 0.0;
	Particle_Index          pMatCountLocal = 0;
	Particle_Index          pEntCountLocal = 0;
	Particle_Index          pMatCountGlobal = 0;
	Particle_Index          pEntCountGlobal = 0;
	Material_Index          entrainedMatIndex = 0;

	self = (Underworld_Entrainment* )LiveComponentRegister_Get( context->CF->LCRegister, (Name)Underworld_Entrainment_Type  );

	entrainedMatIndex = Materials_Register_GetIndex(self->matSwarm->materials_Register,
		self->entrainedMaterial->name);
	/* Calculate Entrainment as ratio of # particles of given material
	 * above entrainment depth */
	for (plI=0; plI < self->matSwarm->particleLocalCount; plI++) {
		mI = MaterialPointsSwarm_GetMaterialIndexAt( self->matSwarm, plI );
		if (mI == entrainedMatIndex) {
			pMatCountLocal++;
			p = (GlobalParticle*)Swarm_ParticleAt(self->matSwarm, plI);
			pY = p->coord[J_AXIS];
			if (self->entrainmentDirPositive && (pY > self->entrainmentY)) {
				pEntCountLocal++;
			}
			else if (!self->entrainmentDirPositive && (pY < self->entrainmentY)) {
				pEntCountLocal++;
			}
			/* Otherwise, particle hasn't been entrained */
		}
	}
	(void)MPI_Allreduce(&pMatCountLocal, &pMatCountGlobal, 1, MPI_INT, MPI_SUM, self->matSwarm->comm);
	(void)MPI_Allreduce(&pEntCountLocal, &pEntCountGlobal, 1, MPI_INT, MPI_SUM, self->matSwarm->comm);
	self->entrainmentRatio = ((double)pEntCountGlobal) / pMatCountGlobal;
	StgFEM_FrequentOutput_PrintValue( context, self->entrainmentRatio );
}
