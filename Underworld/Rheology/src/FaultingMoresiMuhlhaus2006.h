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


#ifndef __Underworld_Rheology_FaultingMoresiMuhlhaus2006_h__
#define __Underworld_Rheology_FaultingMoresiMuhlhaus2006_h__

	/** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
	extern const Type FaultingMoresiMuhlhaus2006_Type;
	
	typedef struct {
		double             slipRate;
		XYZ                slip;
		float              brightness;
		float              opacity;
		float              length;
		float              thickness;
		Particle_Bool      tensileFailure;
		Particle_Bool      fullySoftened;
	}  FaultingMoresiMuhlhaus2006_Particle; 

	
	/** Rheology class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
	#define __FaultingMoresiMuhlhaus2006 \
		/* Parent info */ \
 		__YieldRheology \
		/* Virtual functions go here */ \
		/* Material Parameters */\
		SwarmVariable*                                      slipRate;                              \
		SwarmVariable*                                      slip;                                  \
		SwarmVariable*                                      brightness;                            \
		SwarmVariable*                                      opacity;                               \
		SwarmVariable*                                      length;                                \
		SwarmVariable*                                      thickness;                             \
		SwarmVariable*                                      tensileFailure;                        \
		SwarmVariable*                                      fullySoftened;                        \
		ExtensionInfo_Index                                 particleExtHandle;                     \
		/* Param passed in */ \
		MaterialPointsSwarm*                                materialPointsSwarm;                                 \
		FeVariable*                                         pressureField;                         \
		FeVariable*                                         velocityGradientsField;                \
		/* Director component is used to update the normal */\
		Director*                                           director;                              \
		double                                              cohesion;                              \
		double                                              cohesionAfterSoftening;                \
		double                                              frictionCoefficient;                   \
		double                                              frictionCoefficientAfterSoftening;     \
		double                                              minimumYieldStress;                    \
		Bool                                                ignoreOldOrientation;	                \
		Bool                                                updateOrientationAtMaxSoftness;        \
                Bool                                                updateOrientations;                    \
                Bool                                                isotropicCorrection;                   \
		/* Stored values that are calculated once for each particle */ \
		Eigenvector                                         currentEigenvectorList[3];             \
		TensorArray                                         currentVelocityGradient;               \
		SymmetricTensor                                     currentStress;                         \
		double                                              currentPressure;                       \
		double                                              tau_nn;                                \
		double                                              storedSlipRateValue;                   \
		/* Flags set to tell functions what's going on */ \
		Bool                                                tryingOldOrientation;			\

	struct FaultingMoresiMuhlhaus2006 { __FaultingMoresiMuhlhaus2006 };

	/** Public Constructor */
   FaultingMoresiMuhlhaus2006* FaultingMoresiMuhlhaus2006_New(
      Name                  name,
      AbstractContext*      context,
      StrainWeakening*      strainWeakening, 
      MaterialPointsSwarm*  materialPointsSwarm, 
      double                minVisc, 
		FeVariable*           pressureField,
		FeVariable*           velocityGradientsField,
		Director*             director,
		double                cohesion,
		double                cohesionAfterSoftening,
		double                frictionCoefficient,
		double                frictionCoefficientAfterSoftening,
		double                minimumYieldStress,
		Bool                  ignoreOldOrientation,
		Bool                  updateOrientationAtMaxSoftness,
		Bool                  updateOrientations,
		Bool                  isotropicCorrection );
	
	/** Private Constructor: This will accept all the virtual functions for this class as arguments. */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define FAULTINGMORESIMUHLHAUS2006_DEFARGS \
                YIELDRHEOLOGY_DEFARGS

	#define FAULTINGMORESIMUHLHAUS2006_PASSARGS \
                YIELDRHEOLOGY_PASSARGS

	FaultingMoresiMuhlhaus2006* _FaultingMoresiMuhlhaus2006_New(  FAULTINGMORESIMUHLHAUS2006_DEFARGS  ) ;
	
	/* 'Stg_Component' implementations */
	void* _FaultingMoresiMuhlhaus2006_DefaultNew( Name name ) ;
	void _FaultingMoresiMuhlhaus2006_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data );

	void _FaultingMoresiMuhlhaus2006_Build( void* rheology, void* data );
	void _FaultingMoresiMuhlhaus2006_Initialise( void* rheology, void* data );
   void _FaultingMoresiMuhlhaus2006_Destroy( void* rheology, void* data );
   void _FaultingMoresiMuhlhaus2006_Init(
		FaultingMoresiMuhlhaus2006*                        self,
		FeVariable*                                        pressureField,
		FeVariable*                                        velocityGradientsField,
		MaterialPointsSwarm*                               materialPointsSwarm,
		FiniteElementContext*                              context,
		Director*                                          director,
		double                                             cohesion,
		double                                             cohesionAfterSoftening,
		double                                             frictionCoefficient,
		double                                             frictionCoefficientAfterSoftening,
		double                                             minimumYieldStress,
		Bool                                               ignoreOldOrientation,
		Bool                                               updateOrientationAtMaxSoftness,
		Bool                                               updateOrientations,
		Bool                                               isotropicCorrection) ; 
   
	/* 'YieldRheology' implementations */
	void _FaultingMoresiMuhlhaus2006_ModifyConstitutiveMatrix( 
		void*                                              rheology, 
		ConstitutiveMatrix*                                constitutiveMatrix,
		MaterialPointsSwarm*                               materialPointsSwarm,
		Element_LocalIndex                                 lElement_I,
		MaterialPoint*                                     materialPoint,
		Coord                                              xi );
		
	double _FaultingMoresiMuhlhaus2006_GetYieldCriterion( 
		void*                                              rheology,
		ConstitutiveMatrix*                                constitutiveMatrix,
		MaterialPointsSwarm*                               materialPointsSwarm,
		Element_LocalIndex                                 lElement_I,
		MaterialPoint*                                     materialPoint,
		Coord                                              xi );
	
	double _FaultingMoresiMuhlhaus2006_GetYieldIndicator( 
		void*                                              rheology,
		ConstitutiveMatrix*                                constitutiveMatrix,
		MaterialPointsSwarm*                               materialPointsSwarm,
		Element_LocalIndex                                 lElement_I,
		MaterialPoint*                                     materialPoint,
		Coord                                              xi );
	
	void _FaultingMoresiMuhlhaus2006_HasYielded( 
		void*                                              rheology,
		ConstitutiveMatrix*                                constitutiveMatrix,
		MaterialPointsSwarm*                               materialPointsSwarm,
		Element_LocalIndex                                 lElement_I,
		MaterialPoint*                                     materialPoint,
		double                                             yieldCriterion,
		double                                             yieldIndicator );
	
	Bool _FaultingMoresiMuhlhaus2006_OldOrientationStillSoftening( void* rheology, MaterialPointsSwarm* materialPointsSwarm, void* materialPoint, Dimension_Index dim ) ;
	double* _FaultingMoresiMuhlhaus2006_UpdateNormalDirection( void* rheology, MaterialPointsSwarm* materialPointsSwarm, void* materialPoint, Dimension_Index dim ) ;

	double _FaultingMoresiMuhlhaus2006_EffectiveCohesion( void* rheology, void* materialPoint, void* particleExt ) ;
	double _FaultingMoresiMuhlhaus2006_EffectiveFrictionCoefficient( void* rheology, void* materialPoint ) ;
	double _FaultingMoresiMuhlhaus2006_Sigma_nn( void* rheology, void* materialPoint, Dimension_Index dim ) ;
	void  _FaultingMoresiMuhlhaus2006_UpdateNormalAtMaxSoft( void* rheology, void* materialPoint, Dimension_Index dim, SymmetricTensor strainRate, void* particleExtIn ) ;
	void _FaultingMoresiMuhlhaus2006_StoreCurrentParameters( 
		void*                                              rheology,
		ConstitutiveMatrix*                                constitutiveMatrix, 
		MaterialPointsSwarm*                               materialPointsSwarm,
		Element_LocalIndex                                 lElement_I, 
		MaterialPoint*                                     materialPoint,
		Coord                                              xi ) ;
	
	void _FaultingMoresiMuhlhaus2006_UpdateDrawParameters( void* rheology ) ;

#endif

