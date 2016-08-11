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

#include <math.h>
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include "types.h"
#include "RheologyClass.h"
#include "StrainWeakening.h"
#include "YieldRheology.h"
#include "GALEVonMises.h"
#include "ConstitutiveMatrix.h"

#include <assert.h>

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type GALEVonMises_Type = "GALEVonMises";

/* Public Constructor */
GALEVonMises* GALEVonMises_New(
	Name                  name,
	AbstractContext*      context,
	StrainWeakening*      strainWeakening, 
	MaterialPointsSwarm*  materialPointsSwarm, 
	double                minVisc, 
	FeVariable*           strainRateField,
	SwarmVariable*        swarmStrainRate,
	double                cohesion,
	double                cohesionAfterSoftening,
	Bool                  strainRateSoftening )
{
   GALEVonMises* self = (GALEVonMises*) _GALEVonMises_DefaultNew( name );

   _Rheology_Init( self, (PICelleratorContext*)context );
   _YieldRheology_Init( (YieldRheology*)self, strainWeakening, materialPointsSwarm, minVisc ); 
   _GALEVonMises_Init( (GALEVonMises*)self, strainRateField, swarmStrainRate, cohesion, cohesionAfterSoftening, strainRateSoftening );

   self->isConstructed = True;
   return self;
}


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
GALEVonMises* _GALEVonMises_New(  GALEVONMISES_DEFARGS  ) 
{
	GALEVonMises*					self;

	/* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
	assert( _sizeOfSelf >= sizeof(GALEVonMises) );
	self = (GALEVonMises*) _YieldRheology_New(  YIELDRHEOLOGY_PASSARGS  );
	
	/* Function pointers for this class that are not on the parent class should be set here */
	
	return self;
}

void _GALEVonMises_Init( 
		GALEVonMises*      self, 
		FeVariable*    strainRateField,
		SwarmVariable* swarmStrainRate,
		double         cohesion, 
		double         cohesionAfterSoftening,
		Bool           strainRateSoftening)
{
	self->strainRateField        = strainRateField;
	self->swarmStrainRate        = swarmStrainRate;
	self->cohesion               = cohesion;
	
	/* Strain softening of Cohesion - (linear weakening is assumed) */
	/* needs a softening factor between +0 and 1 and a reference strain > 0 */
	self->cohesionAfterSoftening = cohesionAfterSoftening;
	self->strainRateSoftening    = strainRateSoftening;
}

void* _GALEVonMises_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                     _sizeOfSelf = sizeof(GALEVonMises);
	Type                                                             type = GALEVonMises_Type;
	Stg_Class_DeleteFunction*                                     _delete = _YieldRheology_Delete;
	Stg_Class_PrintFunction*                                       _print = _YieldRheology_Print;
	Stg_Class_CopyFunction*                                         _copy = _YieldRheology_Copy;
	Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _GALEVonMises_DefaultNew;
	Stg_Component_ConstructFunction*                           _construct = _GALEVonMises_AssignFromXML;
	Stg_Component_BuildFunction*                                   _build = _GALEVonMises_Build;
	Stg_Component_InitialiseFunction*                         _initialise = _GALEVonMises_Initialise;
	Stg_Component_ExecuteFunction*                               _execute = _YieldRheology_Execute;
	Stg_Component_DestroyFunction*                               _destroy = _GALEVonMises_Destroy;
	Rheology_ModifyConstitutiveMatrixFunction*  _modifyConstitutiveMatrix = _YieldRheology_ModifyConstitutiveMatrix;
	YieldRheology_GetYieldCriterionFunction*           _getYieldCriterion = _GALEVonMises_GetYieldCriterion;
	YieldRheology_GetYieldIndicatorFunction*           _getYieldIndicator = _GALEVonMises_GetYieldIndicator;
	YieldRheology_HasYieldedFunction*                         _hasYielded = _GALEVonMises_HasYielded;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*) _GALEVonMises_New(  GALEVONMISES_PASSARGS  );
}

void _GALEVonMises_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data ){
	GALEVonMises*          self           = (GALEVonMises*)rheology;
	FeVariable*        strainRateField;
	SwarmVariable*     swarmStrainRate;

	/* Construct Parent */
	_YieldRheology_AssignFromXML( self, cf, data );
	
	strainRateField = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"StrainRateField", FeVariable, False, data  );
	swarmStrainRate = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"swarmStrainRate", SwarmVariable, False, data );
   Journal_Firewall( 
			(strainRateField || self->swarmStrainRate ), 
			Journal_Register( Error_Type, (Name)self->type  ), 
			"\n Error in component type %s, name '%s'.\n Must specify a strainRateField OR a swarmStrainRate, but not both. \n", self->type, self->name ); 

	_GALEVonMises_Init( 
			self, 
			strainRateField,
			swarmStrainRate,
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"cohesion", 0.0  ),
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"cohesionAfterSoftening", 0.0  ),
			Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"strainRateSoftening", False )  );
}

void _GALEVonMises_Destroy( void* rheology, void* data ){
   GALEVonMises* self = (GALEVonMises*) rheology;
   
   if( self->strainRateField ) Stg_Component_Destroy( self->strainRateField, data, False );
   if( self->swarmStrainRate ) Stg_Component_Destroy( self->swarmStrainRate, data, False );
   
   _YieldRheology_Destroy( self, data );
}

void _GALEVonMises_Build( void* rheology, void* data ){
   GALEVonMises* self = (GALEVonMises*) rheology;
   
   /* build parent */
   _YieldRheology_Build( self, data );
   
   if( self->strainRateField ) Stg_Component_Build( self->strainRateField, data, False );
   if( self->swarmStrainRate ) Stg_Component_Build( self->swarmStrainRate, data, False );
   
}

void _GALEVonMises_Initialise( void* rheology, void* data ){
   GALEVonMises* self = (GALEVonMises*) rheology;
   
   /* Initialise parent */
   _YieldRheology_Initialise( self, data );
   
   if( self->strainRateField ) Stg_Component_Initialise( self->strainRateField, data, False );
   if( self->swarmStrainRate ) Stg_Component_Initialise( self->swarmStrainRate, data, False );
   
}

double _GALEVonMises_GetYieldCriterion( 
		void*                            rheology,
		ConstitutiveMatrix*              constitutiveMatrix,
		MaterialPointsSwarm*             materialPointsSwarm,
		Element_LocalIndex               lElement_I,
		MaterialPoint*                   materialPoint,
		Coord                            xi )
{
	GALEVonMises*                         self             = (GALEVonMises*) rheology;
	double                            strainWeakeningRatio;
	double                            effectiveCohesion;
	
	/* Strain softening of yield stress */
	strainWeakeningRatio = StrainWeakening_CalcRatio( self->strainWeakening, materialPoint );
		
	effectiveCohesion =  self->cohesion * (1.0 - strainWeakeningRatio) + 
			self->cohesionAfterSoftening * strainWeakeningRatio;

        /* Store current yield criterion for jacobian evaluation. */
        self->yieldCriterion = effectiveCohesion;

	return effectiveCohesion;
}

double _GALEVonMises_GetYieldIndicator( 
		void*                            rheology,
		ConstitutiveMatrix*              constitutiveMatrix,
		MaterialPointsSwarm*             materialPointsSwarm,
		Element_LocalIndex               lElement_I,
		MaterialPoint*                   materialPoint,
		Coord                            xi )
{
	GALEVonMises*                         self             = (GALEVonMises*) rheology;
	SymmetricTensor                   strainRate;
        int i;
        double stressTrace, strainRateTrace;
	
	/* Get Strain Rate */
	if( self->strainRateField ) {
	   FeVariable_InterpolateWithinElement(
	      self->strainRateField, lElement_I, xi, strainRate );
	}
	else {
	   SwarmVariable_ValueAt( self->swarmStrainRate,
				  constitutiveMatrix->currentParticleIndex,
				  strainRate );
	}

	/* Get Stress */
	ConstitutiveMatrix_CalculateStress( constitutiveMatrix, strainRate,
                                            self->stress );
        SymmetricTensor_GetTrace(self->stress, constitutiveMatrix->dim,
                                 &stressTrace);
        SymmetricTensor_GetTrace(strainRate, constitutiveMatrix->dim,
                                 &strainRateTrace);

        for(i=0;i<constitutiveMatrix->dim;++i)
          {
            strainRate[TensorMapST3D[i][i]]-=
              strainRateTrace/constitutiveMatrix->dim;
            self->stress[TensorMapST3D[i][i]]-=
              stressTrace/constitutiveMatrix->dim;
          }

        /* Save the strain rate invariant */
        self->strainRateSecondInvariant=
          SymmetricTensor_2ndInvariant( strainRate, constitutiveMatrix->dim );

	self->stressInv = 
          SymmetricTensor_2ndInvariant( self->stress, constitutiveMatrix->dim );

        self->yieldIndicator = self->stressInv;

	return self->stressInv;
}

void _GALEVonMises_HasYielded( 
		void*                            rheology,
		ConstitutiveMatrix*              constitutiveMatrix,
		MaterialPointsSwarm*             materialPointsSwarm,
		Element_LocalIndex               lElement_I,
		MaterialPoint*                   materialPoint,
		double                           yieldCriterion,
		double                           yieldIndicator )
{
	GALEVonMises*                 self             = (GALEVonMises*) rheology;
	double                    viscosity        = ConstitutiveMatrix_GetIsotropicViscosity( constitutiveMatrix );

	if ( self->strainRateSoftening ) {
		/* TODO - Put this function in a plugin */
		viscosity = 2.0 * yieldCriterion * yieldCriterion * viscosity / 
				( yieldCriterion * yieldCriterion + yieldIndicator * yieldIndicator );
		ConstitutiveMatrix_SetIsotropicViscosity( constitutiveMatrix, viscosity );
	}
	else {
                double beta = 1.0 - yieldCriterion / yieldIndicator;
                double corr = -viscosity * beta;

		if( (viscosity + corr) < self->minVisc )
		   corr = self->minVisc - viscosity;
		ConstitutiveMatrix_IsotropicCorrection( constitutiveMatrix, corr );
	}

        if( constitutiveMatrix->sle && constitutiveMatrix->sle->nlFormJacobian ) {
           double* derivs = constitutiveMatrix->derivs;
           double coef;

           coef = -viscosity * self->yieldCriterion / (2.0 * self->stressInv * self->stressInv * self->stressInv);
           derivs[0] += coef * 2.0 * viscosity * self->stress[0];
           derivs[4] += coef * 2.0 * viscosity * self->stress[1];
           derivs[1] += coef * 2.0 * viscosity * self->stress[2];
           derivs[3] += coef * 2.0 * viscosity * self->stress[2];

        }

}


