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
#include <Underworld/Underworld.h>

#include "types.h"
#include "RateFieldTimeIntegrator.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type RateFieldTimeIntegrator_Type = "RateFieldTimeIntegrator";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
RateFieldTimeIntegrator* _RateFieldTimeIntegrator_New(  RATEFIELDTIMEINTEGRATOR_DEFARGS  )
{
   RateFieldTimeIntegrator* self;

   /* Call private construCtor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(RateFieldTimeIntegrator) );
   self = (RateFieldTimeIntegrator*) _TimeIntegrand_New(  TIMEINTEGRAND_PASSARGS  );

   return self;
}


void* _RateFieldTimeIntegrator_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                               _sizeOfSelf = sizeof(RateFieldTimeIntegrator);
   Type                                                       type = RateFieldTimeIntegrator_Type;
   Stg_Class_DeleteFunction*                               _delete = _TimeIntegrand_Delete;
   Stg_Class_PrintFunction*                                 _print = _TimeIntegrand_Print;
   Stg_Class_CopyFunction*                                   _copy = _TimeIntegrand_Copy;
   Stg_Component_DefaultConstructorFunction*   _defaultConstructor = _RateFieldTimeIntegrator_DefaultNew;
   Stg_Component_ConstructFunction*                     _construct = _RateFieldTimeIntegrator_AssignFromXML;
   Stg_Component_BuildFunction*                             _build = _RateFieldTimeIntegrator_Build;
   Stg_Component_InitialiseFunction*                   _initialise = _RateFieldTimeIntegrator_Initialise;
   Stg_Component_ExecuteFunction*                         _execute = _TimeIntegrand_Execute;
   Stg_Component_DestroyFunction*                         _destroy = _TimeIntegrand_Destroy;
   TimeIntegrand_CalculateTimeDerivFunction*   _calculateTimeDeriv = _RateFieldTimeIntegrator_TimeDerivative;
   TimeIntegrand_IntermediateFunction*               _intermediate = _TimeIntegrand_Intermediate;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _RateFieldTimeIntegrator_New(  RATEFIELDTIMEINTEGRATOR_PASSARGS  );
}

void _RateFieldTimeIntegrator_AssignFromXML( void* rateFieldTimeIntegrator, Stg_ComponentFactory* cf, void* data ){
   RateFieldTimeIntegrator*  self = (RateFieldTimeIntegrator*) rateFieldTimeIntegrator;

   /* Construct Parent */
   _TimeIntegrand_AssignFromXML( self, cf, data );

   /** see Underworld/InputFiles/CumulativeTotalStrain.xml for a usage example */
   /** this provided swarm will have each particle extended to store the cumulative integral information */
   self->swarm      = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"MaterialPointsSwarm", MaterialPointsSwarm, True, data  );
   /** this ratefield defines the rate of change used for the time integration */
   self->rateField  = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"RateField"          ,       FieldVariable, True, data  );

}


void _RateFieldTimeIntegrator_Build( void* rateFieldTimeIntegrator, void* data ) {
   RateFieldTimeIntegrator*  self = (RateFieldTimeIntegrator*) rateFieldTimeIntegrator;
   ExtensionInfo_Index   handle;

   Stg_Component_Build( self->swarm, data, False );
   Stg_Component_Build( self->rateField, data, False );

   handle = ExtensionManager_GetHandle( self->swarm->particleExtensionMgr, (Name)self->name );

   if ( handle == (ExtensionInfo_Index) -1 ) {
      ArithPointer offset;
      handle = ExtensionManager_Add( self->swarm->particleExtensionMgr, (Name)RateFieldTimeIntegrator_Type, self->rateField->fieldComponentCount*sizeof(double)  );

      /* Adding required increment variable */
      offset = (ArithPointer ) ExtensionManager_Get( self->swarm->particleExtensionMgr, NULL, handle );

      /* Add variables for vizualization / analysis purposes */
      self->particleTimeIntegral = Swarm_NewVectorVariable( self->swarm, self->name, offset, Variable_DataType_Double, self->rateField->fieldComponentCount, "STG_AUTONAME"  );
      LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*)self->particleTimeIntegral->variable );
      LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*)self->particleTimeIntegral );

      /* The RateFieldTimeIntegrator class inherits from the TimeIntegrand class - this class needs a 'Variable' to
       * integrate through time. */
      self->variable = self->particleTimeIntegral->variable;
   } else
      Journal_Firewall(False, Journal_Register( Error_Type, (Name)"RateFieldTimeIntegrator"  ),
                       "\n\nError in '%s' - Extension with name %s should not already exists.\n"
                       "An error has occurred.  Please contact developers.", __func__, self->name);


   /* Build parent.  Note that this needs to go last because it assumes that a variable has been set, which only just happens above */
   /* Another example where the build/init/etc phases don't work */

   _TimeIntegrand_Build( self, data );
   Stg_Component_Build( self->particleTimeIntegral, data, False );

}

void _RateFieldTimeIntegrator_Delete( void* _self ) {
   RateFieldTimeIntegrator* self = (RateFieldTimeIntegrator*) _self;

   Journal_DPrintf( self->debug, "In %s for %s '%s'\n", __func__, self->type, self->name );

   /* Delete Class */
   _Stg_Component_Delete( self );

}

void _RateFieldTimeIntegrator_Destroy( void* _self, void* data ) {
   RateFieldTimeIntegrator* self = (RateFieldTimeIntegrator*) _self;

   Stg_Component_Destroy( self->particleTimeIntegral, data, False );
   Stg_Component_Destroy( self->rateField, data, False );
   Stg_Component_Destroy( self->swarm, data, False );

   /* Destroy Parent */
   _TimeIntegrand_Destroy( self, data );

}

void _RateFieldTimeIntegrator_Initialise( void* rateFieldTimeIntegrator, void* data ) {
   RateFieldTimeIntegrator* self  = (RateFieldTimeIntegrator*) rateFieldTimeIntegrator;

   Stg_Component_Initialise( self->swarm          , data, False );
   Stg_Component_Initialise( self->rateField, data, False );

}

Bool _RateFieldTimeIntegrator_TimeDerivative( void* rateFieldTimeIntegrator, Index lParticle_I, double* timeDeriv ) {
   RateFieldTimeIntegrator*    self   = (RateFieldTimeIntegrator*) rateFieldTimeIntegrator;
   MaterialPoint*          materialPoint = (MaterialPoint*) Swarm_ParticleAt( self->swarm, lParticle_I );

   InterpolationResult result = FieldVariable_InterpolateValueAt( self->rateField, materialPoint->coord, timeDeriv );

   if ( (result == OTHER_PROC) || (result == OUTSIDE_GLOBAL) ){
      /* if unsuccessful in obtaining result, quietly set result to zero and continue  */
      int ii;
      for (ii=0; ii<self->rateField->fieldComponentCount; ii++) {
         timeDeriv[ii] = 0.;
      }
   }
   
   return True;
}

