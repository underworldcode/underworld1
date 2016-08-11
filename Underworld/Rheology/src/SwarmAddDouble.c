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


#include <stdlib.h>
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include "types.h"
#include "SwarmAddDouble.h"

// #include "RheologyClass.h"
// #include "YieldRheology.h"
// #include "VonMises.h"
// #include "ConstitutiveMatrix.h"

#include <assert.h>

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type SwarmAddDouble_Type = "SwarmAddDouble";

/* Public Constructor */

// SwarmAddDouble* SwarmAddDouble_New(
//       	Name                      			name,
// 		PICelleratorContext* 				context,
// 		MaterialPointsSwarm*             	swarm,
// 		SwarmVariable*                 		AddedVariable,              
// 		Name								AddedVariableName		
// 		)
// {
//    SwarmAddDouble* self = (SwarmAddDouble*) _SwarmAddDouble_DefaultNew( name );
// 
//    _SwarmAddDouble_Init(
// 	    self,
// 		context,
// 	    swarm,
// 		AddedVariableName
//  		);
// 
//    self->isConstructed = True;
//    return self;
// }

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */

SwarmAddDouble* _SwarmAddDouble_New(  SWARMADDDOUBLE_DEFARGS  ) 
{
	SwarmAddDouble*					self;

	/* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */

	assert( _sizeOfSelf >= sizeof(SwarmAddDouble) );	
	self = (SwarmAddDouble*) _Stg_Component_New(  STG_COMPONENT_PASSARGS  );
	
	// /* Function pointers for this class that are not on the parent class should be set here */
	// self->_calcIncrement = _calcIncrement;
	
	return self;
}

void _SwarmAddDouble_Init(
		SwarmAddDouble*					self,
		PICelleratorContext* 			context,
		MaterialPointsSwarm*			swarm,
		Name							AddedVariableName		
		)
{
	/* Assign Values */
	self->swarm                       = swarm;
	self->AddedVariableName           = AddedVariableName;
	self->context = context;
	self->debug = Journal_Register( Debug_Type, (Name)self->type ); /* TODO make child of Underworld_Debug */
	
	
	/****** Setup Variables *****/

	/* First check to see if a particle extension has already been created for this swarm */
		
	self->particleExtHandle = ExtensionManager_GetHandle( swarm->particleExtensionMgr, (Name) self->name );

	/* If there isn't one then create the particle extension - otherwise just use the one already there*/
	
	if ( self->particleExtHandle == (ExtensionInfo_Index) -1  ) {
		StandardParticle             particle;
		SwarmAddDouble_ParticleExt*  particleExt;

		/* Add particle extension */
		self->particleExtHandle = 
			ExtensionManager_Add( swarm->particleExtensionMgr, (Name) self->name, sizeof(SwarmAddDouble_ParticleExt)  );	

		particleExt = (SwarmAddDouble_ParticleExt*)ExtensionManager_Get( swarm->particleExtensionMgr, &particle, self->particleExtHandle );

		/*Add variables for vizualization / analysis purposes */
				
		self->AddedVariable = Swarm_NewScalarVariable( swarm, self->AddedVariableName, (ArithPointer) &particleExt->AddedVariable - 
																	   (ArithPointer) &particle, Variable_DataType_Double  );
	}
	else {

		Name variableName;

		/* Get Variables already created */
		variableName = Stg_Object_AppendSuffix( swarm, self->AddedVariableName  );
		self->AddedVariable = SwarmVariable_Register_GetByName( swarm->swarmVariable_Register, variableName );
		assert( self->AddedVariable );
		Memory_Free( variableName );
	}
			
}

void* _SwarmAddDouble_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                               _sizeOfSelf = sizeof(SwarmAddDouble);
	Type                                                       type = SwarmAddDouble_Type;
	Stg_Class_DeleteFunction*                               _delete = _SwarmAddDouble_Delete;
	Stg_Class_PrintFunction*                                 _print = NULL;
	Stg_Class_CopyFunction*                                   _copy = NULL;
	Stg_Component_DefaultConstructorFunction*   _defaultConstructor = _SwarmAddDouble_DefaultNew;
	Stg_Component_ConstructFunction*                     _construct = _SwarmAddDouble_AssignFromXML;
	Stg_Component_BuildFunction*                             _build = _SwarmAddDouble_Build;
	Stg_Component_InitialiseFunction*                   _initialise = _SwarmAddDouble_Initialise;
	Stg_Component_ExecuteFunction*                         _execute = Stg_Component_Execute;
	Stg_Component_DestroyFunction*                         _destroy = _SwarmAddDouble_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */

	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */ ;

	return (void*) _SwarmAddDouble_New(  SWARMADDDOUBLE_PASSARGS  );
}

void _SwarmAddDouble_AssignFromXML( void* swarmAddDouble, Stg_ComponentFactory* cf, void* data ){
	SwarmAddDouble*         self = (SwarmAddDouble*) swarmAddDouble;
	PICelleratorContext* context;
	MaterialPointsSwarm*    materialPointsSwarm;
	Name					AddedVariableName;		

	/* Construct Parent */
	// _TimeIntegrand_AssignFromXML( self, cf, data );
	
	context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", PICelleratorContext, False, data );
   	if( !context  ) 
      context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", PICelleratorContext, True, data  );
   
	
	materialPointsSwarm = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"MaterialPointsSwarm", MaterialPointsSwarm, True, data  );
    AddedVariableName   = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"SwarmNewDoubleVariableName", "NewDoubleVariable"  );

	_SwarmAddDouble_Init(
			self, 
			context,
			materialPointsSwarm, 
			AddedVariableName
			);
}

void _SwarmAddDouble_Build( void* swarmAddDouble, void* data ) {
   SwarmAddDouble*                       self               = (SwarmAddDouble*) swarmAddDouble;

   /* Build parent */
   _TimeIntegrand_Build( self, data );

	Stg_Component_Build( self->AddedVariable, data, False );
	Stg_Component_Build( self->swarm, data, False );   

}

void _SwarmAddDouble_Delete( void* _self ) {
   SwarmAddDouble* self = (SwarmAddDouble*) _self;

   // Journal_DPrintf( self->debug, "In %s for %s '%s'\n", __func__, self->type, self->name );

   /* Delete Class */
   _Stg_Component_Delete( self );   

}
	
void _SwarmAddDouble_Destroy( void* _self, void* data ) {
   SwarmAddDouble* self = (SwarmAddDouble*) _self;

   Stg_Component_Destroy( self->AddedVariable, data, False );
 
   /* Destroy Parent */
   // _TimeIntegrand_Destroy( self, data );   

}

void _SwarmAddDouble_Initialise( void* swarmAddDouble, void* data ) {
   SwarmAddDouble*                        self  = (SwarmAddDouble*) swarmAddDouble;
   Particle_Index                         particleLocalCount;
   Variable*                              positionVariable   = self->swarm->particleCoordVariable->variable;

   Stg_Component_Initialise( self->swarm, data, False );

   /* We should only set initial conditions if in regular non-restart mode. 
	If in restart mode, then the particle-based variables will be set correcty when we re-load the Swarm. */
	
	if ( self->context->loadSwarmsFromCheckpoint == False ) {

      /* Initialise Parent */
      // _TimeIntegrand_Initialise( self, data );

      particleLocalCount = self->AddedVariable->variable->arraySize;

      // Stg_Component_Initialise( self->AddedVariable, data, False );

      /* Update variables */

      Variable_Update( positionVariable );
      Variable_Update( self->AddedVariable->variable );
	}	
}




