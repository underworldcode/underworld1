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


#ifndef __Underworld_Rheology_StrainWeakening_h__
#define __Underworld_Rheology_StrainWeakening_h__

	/** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
	extern const Type StrainWeakening_Type;

	typedef struct {
		double postFailureWeakening;
		double postFailureWeakeningIncrement;
	}  StrainWeakening_ParticleExt; 

	/* virtual function interface */
	typedef double (StrainWeakening_CalcIncrementFunction) ( 		
		void*                            strainWeakening,
		ConstitutiveMatrix*              constitutiveMatrix,
		MaterialPointsSwarm*             swarm,
		MaterialPoint*                   particle,
		double                           yieldCriterion,
		double                           yieldIndicator );
	
	/** Rheology class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
	#define __StrainWeakening \
		/* Parent info */ \
 		__TimeIntegrand \
		/* Virtual functions go here */ \
		StrainWeakening_CalcIncrementFunction* _calcIncrement;                    \
		/* General Info */\
		SwarmVariable*                         postFailureWeakening;              \
		SwarmVariable*                         postFailureWeakeningIncrement;     \
		ExtensionInfo_Index                    particleExtHandle;                 \
		/* Param passed in */ \
		MaterialPointsSwarm*                   swarm;                             \
		double                                 healingRate;                       \
		double                                 softeningStrain;                   \
		double                                 initialDamageFraction;             \
		double                                 initialDamageWavenumber;           \
		double                                 initialDamageWavenumberSinI;           \
		double                                 initialDamageWavenumberCosI;           \
		double                                 initialDamageWavenumberSinJ;           \
		double                                 initialDamageWavenumberCosJ;           \
		double                                 initialDamageWavenumberSinK;           \
		double                                 initialDamageWavenumberCosK;           \
		double                                 initialDamageFactor;               \
		Stg_Shape*                             initialStrainShape;                \
		long int                               randomSeed;
				
	struct StrainWeakening { __StrainWeakening };

	/** Public Constructor */
	StrainWeakening* StrainWeakening_New(
		Name                                               name,
		MaterialPointsSwarm*                               swarm,
		double                                             healingRate,
		double                                             softeningStrain,
		double                                             initialDamageFraction,
		double                                             initialDamageWavenumber,
		double                                             initialDamageWavenumberSinI,
		double                                             initialDamageWavenumberCosI,
		double                                             initialDamageWavenumberSinJ,
		double                                             initialDamageWavenumberCosJ,
		double                                             initialDamageWavenumberSinK,
		double                                             initialDamageWavenumberCosK,
		double                                             initialDamageFactor,
		long int                                           randomSeed,
		Stg_Shape*                                         initialStrainShape );
 
	/** Private Constructor: This will accept all the virtual functions for this class as arguments. */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define STRAINWEAKENING_DEFARGS \
                TIMEINTEGRAND_DEFARGS, \
                StrainWeakening_CalcIncrementFunction*  _calcIncrement

	#define STRAINWEAKENING_PASSARGS \
                TIMEINTEGRAND_PASSARGS, \
	        _calcIncrement

	StrainWeakening* _StrainWeakening_New(  STRAINWEAKENING_DEFARGS  ) ;

	void _StrainWeakening_Init(
		StrainWeakening*                                   self,
		MaterialPointsSwarm*                               swarm,
		double                                             healingRate,
		double                                             softeningStrain,
		double                                             initialDamageFraction,
		double                                             initialDamageWavenumber,
		double                                             initialDamageWavenumberSinI,
		double                                             initialDamageWavenumberCosI,
		double                                             initialDamageWavenumberSinJ,
		double                                             initialDamageWavenumberCosJ,
		double                                             initialDamageWavenumberSinK,
		double                                             initialDamageWavenumberCosK,
		double                                             initialDamageFactor,
		long int                                           randomSeed,
		Stg_Shape*                                         initialStrainShape  );
	
	/* 'Stg_Component' implementations */
	void* _StrainWeakening_DefaultNew( Name name ) ;
	void _StrainWeakening_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data );
	void _StrainWeakening_Build( void* strainWeakening, void* data ) ;
	void _StrainWeakening_Initialise( void* strainWeakening, void* data ) ;
	
	Bool _StrainWeakening_TimeDerivative( void* _strainWeakening, Index lParticle_I, double* timeDeriv );

	/* Private Functions */
	void _StrainWeakening_MakeValuesPositive( void* timeIntegrator, void* strainWeakening ) ;

	double _StrainWeakening_CalcIncrementIsotropic( 
		void*                            strainWeakening,
		ConstitutiveMatrix*              constitutiveMatrix,
		MaterialPointsSwarm*             swarm,
		MaterialPoint*                   particle,
		double                           yieldCriterion,
		double                           yieldIndicator );

	/* Public Functions */
	double StrainWeakening_CalcRatio( void* strainWeakening, void* particle ) ;

	void StrainWeakening_AssignIncrement( 
		void*                            strainWeakening,
		ConstitutiveMatrix*              constitutiveMatrix,
		MaterialPoint*                   particle,
		double                           yieldCriterion,
		double                           yieldIndicator ) ;

	double StrainWeakening_GetPostFailureWeakening( void* strainWeakening, void* particle ) ;
	double StrainWeakening_GetInitialDamageFraction( void* strainWeakening, void* particle );

#endif

