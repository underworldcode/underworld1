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
#include "StrainWeakening.h"
#include "YieldRheology.h"
#include "VonMises.h"
#include "Byerlee.h"
#include "ConstitutiveMatrix.h"

#include <assert.h>

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type Byerlee_Type = "Byerlee";


void _Byerlee_Init( Byerlee* self, FeMesh* mesh, int height_id, double depthCoefficient ) {
   self->mesh = mesh;
   self->depthCoefficient = depthCoefficient;
   self->height_id = height_id;
}

/* Public Constructor */
Byerlee* Byerlee_New(
      Name                  name,
      AbstractContext*      context,
      StrainWeakening*      strainWeakening, 
      MaterialPointsSwarm*  materialPointsSwarm, 
      double                minVisc, 
      int                   strainRateTag,
      double                cohesion,
      double                cohesionAfterSoftening,
      Bool                  strainRateSoftening,
      FeMesh*               mesh,
      double                depthCoefficient,
      int height_id )
{
   Byerlee* self = (Byerlee*) _Byerlee_DefaultNew( name );

   _Rheology_Init( self, (PICelleratorContext*)context );
   _YieldRheology_Init( (YieldRheology*)self, strainWeakening, materialPointsSwarm, minVisc ); 
   _VonMises_Init( (VonMises*)self, strainRateTag, cohesion, cohesionAfterSoftening, strainRateSoftening );
   _Byerlee_Init( self, mesh, height_id, depthCoefficient );
   self->isConstructed = True;
   return self;
}

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Byerlee* _Byerlee_New(  BYERLEE_DEFARGS  )
{
   Byerlee*             self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(Byerlee) );
   self = (Byerlee*) _VonMises_New(  VONMISES_PASSARGS  );

   /* Function pointers for this class that are not on the parent class should be set here */

   return self;
}


void* _Byerlee_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                     _sizeOfSelf = sizeof(Byerlee);
	Type                                                             type = Byerlee_Type;
	Stg_Class_DeleteFunction*                                     _delete = _YieldRheology_Delete;
	Stg_Class_PrintFunction*                                       _print = _YieldRheology_Print;
	Stg_Class_CopyFunction*                                         _copy = _YieldRheology_Copy;
	Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _Byerlee_DefaultNew;
	Stg_Component_ConstructFunction*                           _construct = _Byerlee_AssignFromXML;
	Stg_Component_BuildFunction*                                   _build = _YieldRheology_Build;
	Stg_Component_InitialiseFunction*                         _initialise = _YieldRheology_Initialise;
	Stg_Component_ExecuteFunction*                               _execute = _YieldRheology_Execute;
	Stg_Component_DestroyFunction*                               _destroy = _Byerlee_Destroy;
	Rheology_ModifyConstitutiveMatrixFunction*  _modifyConstitutiveMatrix = _YieldRheology_ModifyConstitutiveMatrix;
	YieldRheology_GetYieldCriterionFunction*           _getYieldCriterion = _Byerlee_GetYieldCriterion;
	YieldRheology_GetYieldIndicatorFunction*           _getYieldIndicator = _VonMises_GetYieldIndicator;
	YieldRheology_HasYieldedFunction*                         _hasYielded = _VonMises_HasYielded;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _Byerlee_New(  BYERLEE_PASSARGS  );
}

void _Byerlee_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data ){
   Byerlee*            self           = (Byerlee*)rheology;
   FeMesh*         mesh;
   int height_id;

   /* Construct Parent */
   _VonMises_AssignFromXML( self, cf, data );

   mesh     = Stg_ComponentFactory_ConstructByNameWithKeyFallback( cf, self->name, (Name)"mesh-linear", (Dictionary_Entry_Key)"FeMesh", FeMesh, True, data  );

   height_id = PpcManager_GetPpcFromDict( self->mgr, cf, self->name, (Dictionary_Entry_Key)"HeightReferenceInput", "" );

   if( height_id == -1 ) {
      Journal_Printf( global_info_stream, 
            "\nUsing maximum mesh height in y-axis for a height reference\n"
            "To use a (Ppc) function a the line\n\t<param name=\"HeightReferenceInput\">someInputfunction</param>\n\n"
            );
   }

   _Byerlee_Init(
         self,
         mesh,
         height_id,
         Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"depthCoefficient", 0.0  )  );
}

void _Byerlee_Destroy( void* rheology, void* data ) {
   Byerlee*  self = (Byerlee*) rheology;
   
   Stg_Component_Destroy( self->mesh, data, False );
   
   _VonMises_Destroy( self, data );
   
}


double _Byerlee_GetYieldCriterion(
      void*                            rheology,
      ConstitutiveMatrix*              constitutiveMatrix,
      MaterialPointsSwarm*             materialPointsSwarm,
      Element_LocalIndex               lElement_I,
      MaterialPoint*                   materialPoint,
      Coord                            xi )
{
   Byerlee*                         self             = (Byerlee*) rheology;
   double                           depth = 0.0;
   double                           height;
   int err;
   Coord                            coord;
   /* stupid way to recover the integration point from the materialPoint
    * TODO: FIXCONSTITUTIVE use integration point only */
   IntegrationPoint* integrationPoint = (IntegrationPoint*) Swarm_ParticleAt( constitutiveMatrix->integrationSwarm,
                                        constitutiveMatrix->currentParticleIndex );

   /* Calculate Depth */
   if( self->height_id != -1 ) {
      err = PpcManager_Get( self->mgr, lElement_I, integrationPoint, self->height_id, &height );
      Journal_Firewall( !err, global_error_stream,
            "Error in file %s. Cannot get reference height for '%s'. Ensure you have passed in the correct input with a line like\n\t<param name=\"HeightReferenceInput\">someInputfunction</param>\n", __func__, self->name );
   } else {
      double min[3], max[3];
      Mesh_GetGlobalCoordRange( self->mesh, min, max );
      height = max[ J_AXIS ];
   }

   /* This rheology assumes particle is an integration points thats can be mapped to a particle
    * that has no meaningful coord. Best to re-calc the global from local */
   FeMesh_CoordLocalToGlobal( self->mesh, lElement_I, xi, coord );
   depth = height - coord[ J_AXIS ];

   return self->cohesion + self->depthCoefficient * depth;
}


