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
#include "RheologyClass.h"
#include "Arrhenius.h"
#include "ConstitutiveMatrix.h"

#include <assert.h>

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type Arrhenius_Type = "Arrhenius";

void _Arrhenius_Init( Arrhenius* self, int temperature_id, double eta0, double activationEnergy, double activationVolume, double referenceTemp )
{
   self->temperature_id   = temperature_id;
   self->eta0             = eta0;
   self->activationEnergy = activationEnergy;
   self->activationVolume = activationVolume;
   self->referenceTemp    = referenceTemp;
}

/* Public Constructor */
Arrhenius* Arrhenius_New(
	Name             name,
	AbstractContext* context,
	int              temperature_id,
	double           eta0,
	double           activationEnergy,
	double           activationVolume,
	double           referenceTemp)
{
   Arrhenius* self = (Arrhenius*) _Arrhenius_DefaultNew( name );

   _Rheology_Init( self, (PICelleratorContext*)context );
   _Arrhenius_Init( self, temperature_id, eta0, activationEnergy, activationVolume, referenceTemp );
   self->isConstructed = True;
   return self;
}

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Arrhenius* _Arrhenius_New(  ARRHENIUS_DEFARGS  )
{
   Arrhenius*              self;

   assert( _sizeOfSelf >= sizeof(Arrhenius) );
   self = (Arrhenius*) _Rheology_New(  RHEOLOGY_PASSARGS  );

   return self;
}

void* _Arrhenius_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                     _sizeOfSelf = sizeof(Arrhenius);
	Type                                                             type = Arrhenius_Type;
	Stg_Class_DeleteFunction*                                     _delete = _Rheology_Delete;
	Stg_Class_PrintFunction*                                       _print = _Rheology_Print;
	Stg_Class_CopyFunction*                                         _copy = _Rheology_Copy;
	Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _Arrhenius_DefaultNew;
	Stg_Component_ConstructFunction*                           _construct = _Arrhenius_AssignFromXML;
	Stg_Component_BuildFunction*                                   _build = _Rheology_Build;
	Stg_Component_InitialiseFunction*                         _initialise = _Rheology_Initialise;
	Stg_Component_ExecuteFunction*                               _execute = _Rheology_Execute;
	Stg_Component_DestroyFunction*                               _destroy = _Arrhenius_Destroy;
	Rheology_ModifyConstitutiveMatrixFunction*  _modifyConstitutiveMatrix = _Arrhenius_ModifyConstitutiveMatrix;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _Arrhenius_New(  ARRHENIUS_PASSARGS  );
}

void _Arrhenius_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data ){
   Arrhenius*     self = (Arrhenius*)rheology;
   int temperature_id;

   /* Construct Parent */
   _Rheology_AssignFromXML( self, cf, data );

   /* PpcManager_GetField_TestForFeVariable */
   temperature_id = PpcManager_GetField( self->mgr, cf, (Stg_Component*)self, "TemperatureField", False );

   Journal_Firewall(
      (temperature_id!=-1),
      Journal_MyStream( Error_Type, self ),
      "Error In %s - No TemperatureField found for the Arrhenius rheology.\nA temperature field must be provided.",
      __func__ );

   _Arrhenius_Init(
         self,
         temperature_id,
         Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"eta0", 1.0  ),
         Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"activationEnergy", 0.0  ),
         Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"activationVolume", 0.0  ),
         Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"referenceTemperature", 1.0 )  );
}

void _Arrhenius_Destroy( void* _self, void* data ) {
   Arrhenius* self = (Arrhenius*) _self;

   /* Destroy Parent */
   _Rheology_Destroy( self, data );
}

void _Arrhenius_Build( void* _self, void* data ) {
   Arrhenius* self = (Arrhenius*) _self;

   /* Build Parent */
   _Rheology_Build( self, data );
}

void _Arrhenius_Initialise( void* _self, void* data ) {
   Arrhenius* self = (Arrhenius*) _self;

   /* Build Parent */
   _Rheology_Initialise( self, data );
}

void _Arrhenius_ModifyConstitutiveMatrix(
      void*                                              rheology,
      ConstitutiveMatrix*                                constitutiveMatrix,
      MaterialPointsSwarm*                               swarm,
      Element_LocalIndex                                 lElement_I,
      MaterialPoint*                                     materialPoint,
      Coord                                              xi )
{
   Arrhenius*  self = (Arrhenius*) rheology;
   double      temperature;
   double      viscosity;

   /* stupid way to recover the integration point from the materialPoint */
   IntegrationPoint* integrationPoint = (IntegrationPoint*) Swarm_ParticleAt( constitutiveMatrix->integrationSwarm, constitutiveMatrix->currentParticleIndex );
   int err = PpcManager_Get( self->mgr, lElement_I, integrationPoint, self->temperature_id, &temperature );

   Journal_Firewall( !err,
                    NULL,
                    "Error in Arrhenius rheology when trying to obtain result from temperature Ppc with Id %d. Possibly outside domain.", self->temperature_id );

   /* If activationVolume is 0 there is no need to calculate the depth of the particle see viscosity line below. */
   if( self->activationVolume > (0.0 + 1e-12 )  ) {
      Coord    min, max;
      Coord    coord;
      double   depth;
      double   height;
      FeMesh*  mesh = ConstitutiveMatrix_GetMesh( constitutiveMatrix );
      /* Extract geometric extents. */
      Mesh_GetGlobalCoordRange( mesh, min, max );
      /* Calculate Depth */
      height = max[ J_AXIS ];

      /* This rheology assumes particle is an integration points thats can be mapped to a particle
       * that has no meaningful coord. Best to re-calc the global from local */
      FeMesh_CoordLocalToGlobal( mesh, lElement_I, xi, coord );
      depth = height - coord[ J_AXIS ];
      /* Calculate New Viscosity */
      viscosity = self->eta0 * exp(( self->activationEnergy + self->activationVolume * depth)/ (temperature + self->referenceTemp));
   }
   else {
      /* Calculate New Viscosity */
      viscosity = self->eta0 * exp(( self->activationEnergy )/ (temperature + self->referenceTemp));
   }
   ConstitutiveMatrix_SetIsotropicViscosity( constitutiveMatrix, viscosity );
}


