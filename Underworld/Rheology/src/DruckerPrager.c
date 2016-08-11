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
#include "RheologyClass.h"
#include "StrainWeakening.h"
#include "YieldRheology.h"
#include "VonMises.h"
#include "DruckerPrager.h"
#include "ConstitutiveMatrix.h"

#include <assert.h>
#include <string.h>

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type DruckerPrager_Type = "DruckerPrager";

/* Public Constructor */
DruckerPrager* DruckerPrager_New(
	Name                  name,
	AbstractContext*      context,
	StrainWeakening*      strainWeakening, 
	MaterialPointsSwarm*  materialPointsSwarm, 
	double                minVisc, 
	int                   strainRateTag,
	double                cohesion,
	double                cohesionAfterSoftening,
	Bool                  strainRateSoftening,
	int                   pressureTag,
	int                   velocityGradientsTag,
	double                minimumYieldStress,
	double                frictionCoefficient,
	double                frictionCoefficientAfterSoftening )
{
   DruckerPrager* self = (DruckerPrager*) _DruckerPrager_DefaultNew( name );

   _Rheology_Init( self, (PICelleratorContext*)context );
   _YieldRheology_Init( (YieldRheology*)self, strainWeakening, materialPointsSwarm, minVisc ); 
   _VonMises_Init( (VonMises*)self, strainRateTag, cohesion, cohesionAfterSoftening, strainRateSoftening );
   _DruckerPrager_Init( self, pressureTag, velocityGradientsTag, minimumYieldStress, frictionCoefficient, frictionCoefficientAfterSoftening );
   self->isConstructed = True;
   return self;
}

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
DruckerPrager* _DruckerPrager_New(  DRUCKERPRAGER_DEFARGS  ) 
{
	DruckerPrager*					self;

	/* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
	assert( _sizeOfSelf >= sizeof(DruckerPrager) );
	self = (DruckerPrager*) _VonMises_New(  VONMISES_PASSARGS  );
	
	/* Function pointers for this class that are not on the parent class should be set here */
	
	return self;
}

void _DruckerPrager_Init(
		DruckerPrager*                                     self,
		int                                                pressure_id,
		int                                                velocityGradients_id,
		double                                             minimumYieldStress,
		double                                             frictionCoefficient,
		double                                             frictionCoefficientAfterSoftening )
{
   MaterialPointsSwarm*      materialPointsSwarm;
	DruckerPrager_Particle*   particleExt;
	StandardParticle          materialPoint;
	Dimension_Index          dim = 0;
	
	/* Assign Pointers */
	self->frictionCoefficient = frictionCoefficient;
	self->minimumYieldStress  = minimumYieldStress;
	
	self->pressureTag         = pressure_id;
	self->velocityGradientsTag = velocityGradients_id;
   materialPointsSwarm = self->mgr->materialSwarm;
   dim = materialPointsSwarm->dim;	
    /* Strain softening of Friction - (linear weakening is assumed ) */
	/* needs a softening factor between +0 and 1 and a reference strain > 0 */
	self->frictionCoefficientAfterSoftening = frictionCoefficientAfterSoftening;

   /* get the particle extension */
   self->particleExtHandle = ExtensionManager_GetHandle( materialPointsSwarm->particleExtensionMgr, (Name)DruckerPrager_Type );

   if( self->particleExtHandle == (unsigned)-1 ) {
      /* if no particles extension add it and add Update hook */
      self->particleExtHandle = ExtensionManager_Add( materialPointsSwarm->particleExtensionMgr, (Name)DruckerPrager_Type, sizeof(DruckerPrager_Particle) );

      particleExt = (double*)ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, &materialPoint, self->particleExtHandle );
      
      /* The tensileFailure variable allows to check whether a materialPoint has failed in tensile mode or not */
      self->tensileFailure = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"DruckerPragerTensileFailure", (ArithPointer) &particleExt->tensileFailure - (ArithPointer) &materialPoint, Variable_DataType_Char );
      LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*)self->tensileFailure->variable );
      LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*)self->tensileFailure );
	   
   } else {
      /* get references to all swarm variables */
      Name var_name = Stg_Object_AppendSuffix( materialPointsSwarm, (Name)"DruckerPragerTensileFailure"  );
      self->tensileFailure = SwarmVariable_Register_GetByName( materialPointsSwarm->swarmVariable_Register, var_name );
      Memory_Free( var_name );
	   
   }

   self->curFrictionCoef = 0.0;

}

void* _DruckerPrager_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                     _sizeOfSelf = sizeof(DruckerPrager);
	Type                                                             type = DruckerPrager_Type;
	Stg_Class_DeleteFunction*                                     _delete = _YieldRheology_Delete;
	Stg_Class_PrintFunction*                                       _print = _YieldRheology_Print;
	Stg_Class_CopyFunction*                                         _copy = _YieldRheology_Copy;
	Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _DruckerPrager_DefaultNew;
	Stg_Component_ConstructFunction*                           _construct = _DruckerPrager_AssignFromXML;
	Stg_Component_BuildFunction*                                   _build = _DruckerPrager_Build;
	Stg_Component_InitialiseFunction*                         _initialise = _DruckerPrager_Initialise;
	Stg_Component_ExecuteFunction*                               _execute = _YieldRheology_Execute;
	Stg_Component_DestroyFunction*                               _destroy = _DruckerPrager_Destroy;
	Rheology_ModifyConstitutiveMatrixFunction*  _modifyConstitutiveMatrix = _YieldRheology_ModifyConstitutiveMatrix;
	YieldRheology_GetYieldCriterionFunction*           _getYieldCriterion = _DruckerPrager_GetYieldCriterion;
	YieldRheology_GetYieldIndicatorFunction*           _getYieldIndicator = _VonMises_GetYieldIndicator;
	YieldRheology_HasYieldedFunction*                         _hasYielded = _DruckerPrager_HasYielded;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*) _DruckerPrager_New(  DRUCKERPRAGER_PASSARGS   );
}

void _DruckerPrager_AssignFromXML( void* druckerPrager, Stg_ComponentFactory* cf, void* data ){
	DruckerPrager*          self           = (DruckerPrager*)druckerPrager;
   int pressure_id, velocityGradientsField_id;

	/* Construct Parent */
	_VonMises_AssignFromXML( self, cf, data );
	
   pressure_id = PpcManager_GetField( self->mgr, cf, (Stg_Component*)self, "PressureField", True );
   velocityGradientsField_id = PpcManager_GetField( self->mgr, cf, (Stg_Component*)self, "VelocityGradientsField", False );
			
	_DruckerPrager_Init( self, 
			pressure_id,
			velocityGradientsField_id,
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"minimumYieldStress", 0.0  ),
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"frictionCoefficient", 0.0  ),
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"frictionCoefficientAfterSoftening", 0.0 )  );
}

void _DruckerPrager_Build( void* rheology, void* data ) {
	DruckerPrager*          self               = (DruckerPrager*) rheology;

	/* Build parent */
	_VonMises_Build( self, data );

	Stg_Component_Build( self->tensileFailure, data, False );

}

void _DruckerPrager_Destroy( void* rheology, void* data ) {
	DruckerPrager*          self               = (DruckerPrager*) rheology;

	Stg_Component_Destroy( self->tensileFailure, data, False );
	
	/* Destroy parent */
	_VonMises_Destroy( self, data );

}


void _DruckerPrager_Initialise( void* rheology, void* data ) {
	DruckerPrager*                  self                  = (DruckerPrager*) rheology;
	Particle_Index                  lParticle_I;
	Particle_Index                  particleLocalCount;
	_VonMises_Initialise( self, data );

	/* We should only set initial conditions if in regular non-restart mode. If in restart mode, then
	the particle-based variables will be set correcty when we re-load the Swarm. */
	if ( self->context->loadSwarmsFromCheckpoint == False ) {
      Stg_Component_Initialise( self->tensileFailure, data, False );
		
		/* We don't need to Initialise hasYieldedVariable because it's a parent variable and _YieldRheology_Initialise
		 * has already been called */
		particleLocalCount = self->hasYieldedVariable->variable->arraySize;

		for ( lParticle_I = 0 ; lParticle_I < particleLocalCount ; lParticle_I++ ) { 
				
			Variable_SetValueChar( self->hasYieldedVariable->variable, lParticle_I, False );
			Variable_SetValueChar( self->tensileFailure->variable,lParticle_I, False );
			
		}
	}	
}

double _DruckerPrager_GetYieldCriterion( 
			void*                            druckerPrager,
			ConstitutiveMatrix*              constitutiveMatrix,
			MaterialPointsSwarm*             materialPointsSwarm,
			Element_LocalIndex               lElement_I,
			MaterialPoint*                   materialPoint,
			Coord                            xi )
{
	DruckerPrager*                    self             = (DruckerPrager*) druckerPrager;
	double                            cohesion;
	double                            cohesionAfterSoftening;
	double                            frictionCoefficient;
	double                            frictionCoefficientAfterSoftening;
	double                            minimumYieldStress;
	double                            strainWeakeningRatio;
	double                            effectiveCohesion;
	double                            effectiveFrictionCoefficient;
	double                            phi;
	double                            sinphi;
	double                            oneOverDenominator;
	double                            dpFrictionCoefficient;
	double                            dpCohesion;
	double                            frictionalStrength;
	double                            pressure;
   int                               dim, err;
	DruckerPrager_Particle*           particleExt;
	
	/* Get Parameters From Rheology */
   dim                                = constitutiveMatrix->dim;
	cohesion                           = self->cohesion;
	cohesionAfterSoftening             = self->cohesionAfterSoftening;
	frictionCoefficient                = self->frictionCoefficient;
	frictionCoefficientAfterSoftening  = self->frictionCoefficientAfterSoftening;
	minimumYieldStress                 = self->minimumYieldStress;
   /* stupid way to recover the integration point from the materialPoint
    * TODO: FIXCONSTITUTIVE use integration point only */
  IntegrationPoint* integrationPoint = (IntegrationPoint*) Swarm_ParticleAt( constitutiveMatrix->integrationSwarm,
																									  constitutiveMatrix->currentParticleIndex );
	
	particleExt = (DruckerPrager_Particle*)ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, materialPoint, self->particleExtHandle );

   /* get pressure */
   err = PpcManager_Get( self->mgr, lElement_I, integrationPoint, self->pressureTag, &pressure );
   if( err ) assert(0);
	
	/* Strain softening of yield stress - if the strain weakening is not defined then this function returns 
	 * a 0 value */
	strainWeakeningRatio = StrainWeakening_CalcRatio( self->strainWeakening, materialPoint );

	
	effectiveCohesion =  cohesion * (1.0 - strainWeakeningRatio) + 
			cohesionAfterSoftening * strainWeakeningRatio;
		
	effectiveFrictionCoefficient = frictionCoefficient * (1.0 - strainWeakeningRatio) +
			frictionCoefficientAfterSoftening * strainWeakeningRatio;
	
   if( dim == 3 ) {
      /* DruckerPrager is calibrated to conincide with the
       * triaxial compression Morh-Coulomb */
      phi = atan(effectiveFrictionCoefficient);
      sinphi = sin(phi);
      oneOverDenominator = 1.0 / (sqrt(3.0) * (3.0 - sinphi));

      dpFrictionCoefficient = 6.0 * sinphi * oneOverDenominator;
      dpCohesion = 6.0 * effectiveCohesion * cos(phi) * oneOverDenominator;
   }
   else {
      /* plane strain */
      phi = atan(effectiveFrictionCoefficient);
      dpCohesion = effectiveCohesion * cos(phi);
      dpFrictionCoefficient = sin(phi);
   }


	frictionalStrength = dpFrictionCoefficient * pressure + dpCohesion ;

	particleExt->tensileFailure = (frictionalStrength <= 0.0);

	if ( frictionalStrength < minimumYieldStress) 
		frictionalStrength = minimumYieldStress;

        self->yieldCriterion = frictionalStrength;
        self->curFrictionCoef = dpFrictionCoefficient;

	return frictionalStrength;
}

void _DruckerPrager_HasYielded( 
		void*                            rheology,
		ConstitutiveMatrix*              constitutiveMatrix,
		MaterialPointsSwarm*             materialPointsSwarm,
		Element_LocalIndex               lElement_I,
		MaterialPoint*                   materialPoint,
		double                           yieldCriterion,
		double                           yieldIndicator )
{
   DruckerPrager* self = (DruckerPrager*)rheology;
   double viscosity = ConstitutiveMatrix_GetIsotropicViscosity( constitutiveMatrix );

   _VonMises_HasYielded(
      self, constitutiveMatrix, materialPointsSwarm, lElement_I, materialPoint,
      yieldCriterion, yieldIndicator );

   if( constitutiveMatrix->sle && constitutiveMatrix->sle->nlFormJacobian ) {

      constitutiveMatrix->derivs[8] += viscosity * self->curFrictionCoef / yieldIndicator;

   }

}

