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


#ifndef __Underworld_Rheology_GALEDruckerPrager_h__
#define __Underworld_Rheology_GALEDruckerPrager_h__

   /** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
   extern const Type GALEDruckerPrager_Type;

   typedef struct {
      float          brightness;
      float          opacity;
      float          diameter;
      Particle_Bool  tensileFailure;
   }  GALEDruckerPrager_Particle;

   /** Rheology class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
   #define __GALEDruckerPrager \
      /* Parent info */ \
      __GALEVonMises \
      /* Virtual functions go here */ \
      /* General Info */\
      SwarmVariable*                                      brightness;                            \
      SwarmVariable*                                      opacity;                               \
      SwarmVariable*                                      diameter;                              \
      SwarmVariable*                                      tensileFailure;                        \
      ExtensionInfo_Index                                 particleExtHandle;                     \
      /* Param passed in */\
      FeVariable*                                         pressureField;                         \
      SwarmVariable*                                      swarmPressure;                         \
      double                                              minimumYieldStress;                    \
      double                                              minimumViscosity;                      \
      double                                              frictionCoefficient;                   \
      double                                              frictionCoefficientAfterSoftening;     \
		double                                              boundaryCohesion;                              \
		double                                              boundaryCohesionAfterSoftening;                \
		double                                              boundaryFrictionCoefficient;                   \
		double                                              boundaryFrictionCoefficientAfterSoftening;     \
                Bool                                                boundaryTop; \
                Bool                                                boundaryBottom; \
                Bool                                                boundaryLeft; \
                Bool                                                boundaryRight; \
                Bool                                                boundaryFront; \
                Bool                                                boundaryBack; \
                HydrostaticTerm*                                    hydrostaticTerm; \
      double curFrictionCoef;


   struct GALEDruckerPrager { __GALEDruckerPrager };

   /* Public Constructor */
   GALEDruckerPrager* GALEDruckerPrager_New(
      Name                  name,
      StrainWeakening*      strainWeakening,
      MaterialPointsSwarm*  materialPointsSwarm,
      double                minVisc,
      FeVariable*           strainRateField,
      SwarmVariable*        swarmStrainRate,
      double                cohesion,
      double                cohesionAfterSoftening,
      Bool                  strainRateSoftening,
      FeVariable*           pressureField,
      SwarmVariable*        swarmPressure,
      double                minimumYieldStress,
      double                minimumViscosity,
      double                frictionCoefficient,
      double                frictionCoefficientAfterSoftening,
      double                                              boundaryCohesion,
      double                                              boundaryCohesionAfterSoftening,
      double                                              boundaryFrictionCoefficient,
      double                                              boundaryFrictionCoefficientAfterSoftening,
      Bool                                                boundaryTop,
      Bool                                                boundaryBottom,
      Bool                                                boundaryLeft,
      Bool                                                boundaryRight,
      Bool                                                boundaryFront,
      Bool                                                boundaryBack,
      HydrostaticTerm*                                    hydrostaticTerm);

   /** Private Constructor: This will accept all the virtual functions for this class as arguments. */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define GALEDRUCKERPRAGER_DEFARGS \
                GALEVONMISES_DEFARGS

	#define GALEDRUCKERPRAGER_PASSARGS \
                GALEVONMISES_PASSARGS

   GALEDruckerPrager* _GALEDruckerPrager_New(  GALEDRUCKERPRAGER_DEFARGS  ) ;

   /* 'Stg_Component' implementations */
   void* _GALEDruckerPrager_DefaultNew( Name name ) ;
   void _GALEDruckerPrager_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data );

   void _GALEDruckerPrager_Build( void* rheology, void* data );
   void _GALEDruckerPrager_Initialise( void* rheology, void* data ) ;
   void _GALEDruckerPrager_Destroy( void* rheology, void* data ) ;
   void _GALEDruckerPrager_Init(
		GALEDruckerPrager*                                     self,
		FeVariable*                                        pressureField,
		SwarmVariable*                                     swarmPressure,
		MaterialPointsSwarm*                               materialPointsSwarm,
		double                                             frictionCoefficient,
		double                                             frictionCoefficientAfterSoftening,
                double                    boundaryCohesion,
                double                    boundaryCohesionAfterSoftening,
                double                    boundaryFrictionCoefficient,
                double                    boundaryFrictionCoefficientAfterSoftening,
                Bool                      boundaryBottom,
                Bool                      boundaryTop,
                Bool                      boundaryLeft,
                Bool                      boundaryRight,
                Bool                      boundaryFront,
                Bool                      boundaryBack,
                double                    minimumYieldStress,
                double                    minimumViscosity,
                HydrostaticTerm*          hydrostaticTerm );


   /* 'YieldRheology' implementations */
   double _GALEDruckerPrager_GetYieldCriterion(
         void*                            druckerPrager,
         ConstitutiveMatrix*              constitutiveMatrix,
         MaterialPointsSwarm*             materialPointsSwarm,
         Element_LocalIndex               lElement_I,
         MaterialPoint*                   materialPoint,
         Coord                            xi );

   void _GALEDruckerPrager_HasYielded(
         void*                            rheology,
         ConstitutiveMatrix*              constitutiveMatrix,
         MaterialPointsSwarm*             materialPointsSwarm,
         Element_LocalIndex               lElement_I,
         MaterialPoint*                   materialPoint,
         double                           yieldCriterion,
         double                           yieldIndicator );

   void _GALEDruckerPrager_UpdateDrawParameters( void* rheology ) ;

	double _GALEDruckerPrager_EffectiveCohesion( void* rheology, void* materialPoint, Bool inside_boundary ) ;
	double _GALEDruckerPrager_EffectiveFrictionCoefficient( void* rheology, void* materialPoint, Bool inside_boundary ) ;
	
#endif

