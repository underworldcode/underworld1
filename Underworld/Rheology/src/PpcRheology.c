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
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include "types.h"
#include "RheologyClass.h"
#include "PpcRheology.h"
#include "ConstitutiveMatrix.h"

#include <assert.h>

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type PpcRheology_Type = "PpcRheology";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
PpcRheology* _PpcRheology_New(  PPCRHEOLOGY_DEFARGS  ) 
{
	PpcRheology*					self;

	assert( _sizeOfSelf >= sizeof(PpcRheology) );
	self = (PpcRheology*) _Rheology_New(  RHEOLOGY_PASSARGS  );
	
	return self;
}

void _PpcRheology_Init( PpcRheology* self, PpcManager* ppcManager, int functionLabel )
{
   self->errorStream   = Journal_Register( Error_Type, (Name)self->name  );
   self->ppcManager    = ppcManager;
   self->functionLabel = functionLabel;
}

void* _PpcRheology_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                     _sizeOfSelf = sizeof(PpcRheology);
	Type                                                             type = PpcRheology_Type;
	Stg_Class_DeleteFunction*                                     _delete = _Rheology_Delete;
	Stg_Class_PrintFunction*                                       _print = _Rheology_Print;
	Stg_Class_CopyFunction*                                         _copy = _Rheology_Copy;
	Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _PpcRheology_DefaultNew;
	Stg_Component_ConstructFunction*                           _construct = _PpcRheology_AssignFromXML;
	Stg_Component_BuildFunction*                                   _build = _PpcRheology_Build;
	Stg_Component_InitialiseFunction*                         _initialise = _PpcRheology_Initialise;
	Stg_Component_ExecuteFunction*                               _execute = _Rheology_Execute;
	Stg_Component_DestroyFunction*                               _destroy = _PpcRheology_Destroy;
	Rheology_ModifyConstitutiveMatrixFunction*  _modifyConstitutiveMatrix = _PpcRheology_ModifyConstitutiveMatrix;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*) _PpcRheology_New(  PPCRHEOLOGY_PASSARGS  );
}

void _PpcRheology_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data ){
   PpcRheology*  self = (PpcRheology*)rheology;
   PpcManager*   ppcManager  = NULL;
   int           functionLabel;

   /* Construct Parent */
	_Rheology_AssignFromXML( self, cf, data );
   
   /* The PpcManager */
   ppcManager = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Manager", PpcManager, False, data );
   if( !ppcManager  )
      ppcManager = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_ppcManager", PpcManager, True, data  );

   functionLabel = PpcManager_GetPpcFromDict( ppcManager, cf, self->name, (Dictionary_Entry_Key)"functionLabel", "functionLabel" );

   _PpcRheology_Init( self, ppcManager, functionLabel );
}

void _PpcRheology_Build( void* rheology, void* data ){
	PpcRheology*     self = (PpcRheology*)rheology;

	_Rheology_Build( self, data );

   Stg_Component_Build( self->ppcManager, data, False );

}

void _PpcRheology_Initialise( void* rheology, void* data ){
	PpcRheology*     self = (PpcRheology*)rheology;

	_Rheology_Initialise( self, data );

   Stg_Component_Initialise( self->ppcManager, data, False );

}

void _PpcRheology_Destroy( void* rheology, void* data ){
	PpcRheology*     self = (PpcRheology*)rheology;

	_Rheology_Destroy( self, data );

   Stg_Component_Destroy( self->ppcManager, data, False );

}

void _PpcRheology_ModifyConstitutiveMatrix( 
      void*                 rheology,
      ConstitutiveMatrix*   constitutiveMatrix,
      MaterialPointsSwarm*  swarm,
      Element_LocalIndex    lElement_I,
      MaterialPoint*        materialPoint,
      Coord                 xi )
{
   PpcRheology*  self = (PpcRheology*) rheology;
   double viscosity;

   IntegrationPoint* particle = (IntegrationPoint*) Swarm_ParticleAt( constitutiveMatrix->integrationSwarm, constitutiveMatrix->currentParticleIndex );

   /* evaluate ppc function */
   PpcManager_Get( self->ppcManager, lElement_I, particle, self->functionLabel, &viscosity );
	ConstitutiveMatrix_SetIsotropicViscosity( constitutiveMatrix, viscosity );
}


