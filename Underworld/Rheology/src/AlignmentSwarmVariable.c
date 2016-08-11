/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
**       * Redistributions of source code must retain the above copyright notice,
**          this list of conditions and the following disclaimer.
**       * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**       * Neither the name of the Monash University nor the names of its contributors
**       may be used to endorse or promote products derived from this software
**       without specific prior written permission.
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
#include "AlignmentSwarmVariable.h"
#include "Director.h"

#include <assert.h>
#include <string.h>

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type AlignmentSwarmVariable_Type = "AlignmentSwarmVariable";

/* Public Constructor */
AlignmentSwarmVariable* AlignmentSwarmVariable_New(
	Name					name,
	AbstractContext*	context,
	Swarm*				swarm,
	Variable*			variable,
	Index					dofCount,
	FeVariable*			velocityField,
	int					CalculateAlignmentSlip,
	double*				slip,
	Director*			director )
{
   AlignmentSwarmVariable* self = (AlignmentSwarmVariable*) _AlignmentSwarmVariable_DefaultNew( name );

   self->isConstructed = True;
   _SwarmVariable_Init( (SwarmVariable*)self, context, swarm, variable, dofCount, False, False);
   _AlignmentSwarmVariable_Init( self, velocityField, CalculateAlignmentSlip, slip, director );

   return self;
}

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
AlignmentSwarmVariable* _AlignmentSwarmVariable_New(  ALIGNMENTSWARMVARIABLE_DEFARGS  ) {
   AlignmentSwarmVariable* self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(AlignmentSwarmVariable) );
   self = (AlignmentSwarmVariable*) _SwarmVariable_New(  SWARMVARIABLE_PASSARGS  );

   /* Function pointers for this class that are not on the parent class should be set here */

   return self;
}

void _AlignmentSwarmVariable_Init( void* druckerPrager, FeVariable* velocityField, int CalculateAlignmentSlip, double* slip, Director* director ) {
   AlignmentSwarmVariable*	self = (AlignmentSwarmVariable*)druckerPrager;
	int i;
	
   self->dofCount = 1;
   self->director = director;
	self->CalculateAlignmentSlip = CalculateAlignmentSlip;
	for (i = 0; i < 3; i++) {
		self->slip[i] = slip[i];
	}
   self->velocityField = velocityField;
   /* variable does not store data, so is not checkpointed */
   self->isCheckpointedAndReloaded = False;
}

void* _AlignmentSwarmVariable_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                 _sizeOfSelf = sizeof(AlignmentSwarmVariable);
	Type                                                         type = AlignmentSwarmVariable_Type;
	Stg_Class_DeleteFunction*                                 _delete = _SwarmVariable_Delete;
	Stg_Class_PrintFunction*                                   _print = _SwarmVariable_Print;
	Stg_Class_CopyFunction*                                     _copy = _SwarmVariable_Copy;
	Stg_Component_DefaultConstructorFunction*     _defaultConstructor = _AlignmentSwarmVariable_DefaultNew;
	Stg_Component_ConstructFunction*                       _construct = _AlignmentSwarmVariable_AssignFromXML;
	Stg_Component_BuildFunction*                               _build = _AlignmentSwarmVariable_Build;
	Stg_Component_InitialiseFunction*                     _initialise = _AlignmentSwarmVariable_Initialise;
	Stg_Component_ExecuteFunction*                           _execute = _SwarmVariable_Execute;
	Stg_Component_DestroyFunction*                           _destroy = _AlignmentSwarmVariable_Destroy;
	AllocationType                                 nameAllocationType = NON_GLOBAL;
	SwarmVariable_ValueAtFunction*                           _valueAt = _AlignmentSwarmVariable_ValueAt;
	SwarmVariable_GetGlobalValueFunction*      _getMinGlobalMagnitude = _AlignmentSwarmVariable_GetMinGlobalMagnitude;
	SwarmVariable_GetGlobalValueFunction*      _getMaxGlobalMagnitude = _AlignmentSwarmVariable_GetMaxGlobalMagnitude;

   return (void*) _AlignmentSwarmVariable_New(  ALIGNMENTSWARMVARIABLE_PASSARGS  );
}

void _AlignmentSwarmVariable_AssignFromXML( void* druckerPrager, Stg_ComponentFactory* cf, void* data ){
   AlignmentSwarmVariable*	self = (AlignmentSwarmVariable*)druckerPrager;
   FeVariable*					velocityField;
	int							CalculateAlignmentSlip;
	double						slip[3];
   Director*					director;

   /* AssignFromXML Parent */
   _SwarmVariable_AssignFromXML( self, cf, data );

   /* AssignFromXML 'AlignmentSwarmVariable' stuff */
   velocityField = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"VelocityField", FeVariable, True, data  ) ;
	/* If we don't want to set up a director and calculate via a specified slip vector this is set to 1 and slip vector is specified */
	CalculateAlignmentSlip = Stg_ComponentFactory_GetInt( cf, self->name, (Dictionary_Entry_Key)"CalculateAlignmentSlip",  0 );
	slip[0] = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"slip1", 0  );
	slip[1] = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"slip2", 0  );
	slip[2] = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"slip3", 0  );	
	/* If we want to calculate via a specified director- this is an essential component */
	director = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Director", Director, True, data  ) ;

   _AlignmentSwarmVariable_Init( self, velocityField, CalculateAlignmentSlip, slip, director );
}

void _AlignmentSwarmVariable_Build( void* alignment, void* data ) {
   AlignmentSwarmVariable* self = (AlignmentSwarmVariable*) alignment;

   /* Build parent */
   _SwarmVariable_Build( self, data );

   Stg_Component_Build( self->director, data, False );
   Stg_Component_Build( self->velocityField, data, False );
}

void _AlignmentSwarmVariable_Initialise( void* alignment, void* data ) {
   AlignmentSwarmVariable* self = (AlignmentSwarmVariable*) alignment;

   /* Initialise Parent */
   _SwarmVariable_Initialise( self, data );

   Stg_Component_Initialise( self->director, data, False );
   Stg_Component_Initialise( self->velocityField, data, False );
}

void _AlignmentSwarmVariable_Destroy( void* alignment, void* data ) {
   AlignmentSwarmVariable* self = (AlignmentSwarmVariable*) alignment;

   Stg_Component_Destroy( self->director, data, False );
   Stg_Component_Destroy( self->velocityField, data, False );

   /* Destroy Parent */
   _SwarmVariable_Destroy( self, data );
}

void _AlignmentSwarmVariable_ValueAt( void* alignment, Particle_Index lParticle_I, double* value ) {
   AlignmentSwarmVariable* self = (AlignmentSwarmVariable*) alignment;
   GlobalParticle*         particle;
   XYZ                     normal;
   double                  velocity[3];
   Dimension_Index         dim = self->swarm->dim;

   particle = (GlobalParticle*)Swarm_ParticleAt( self->swarm, lParticle_I );

   FieldVariable_InterpolateValueAt( self->velocityField, particle->coord, velocity );
   StGermain_VectorNormalise( velocity, self->swarm->dim );
	if (self->CalculateAlignmentSlip == 1) {
		*value = 1.0 - fabs( StGermain_VectorDotProduct( velocity, self->slip, dim ) );
	}
	else{
		Director_GetNormal( self->director, particle, normal );
		*value = 1.0 - fabs( StGermain_VectorDotProduct( velocity, normal, dim ) );
	}
}

double _AlignmentSwarmVariable_GetMinGlobalMagnitude( void* alignment ) {
   return 0.0;
}
double _AlignmentSwarmVariable_GetMaxGlobalMagnitude( void* alignment ) {
   return 1.0;
}


