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


#ifndef __Underworld_Rheology_DruckerPrager_Extra_h__
#define __Underworld_Rheology_DruckerPrager_Extra_h__

   /** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
   extern const Type DruckerPrager_Extra_Type;

   typedef struct {
      Particle_Bool  tensileFailure;
	   double		 slipRate;
	   XYZ			 slip;
	   int			 fullySoftened;
	   double		 plasticStrainRate;
	   double		 backgroundStrainRate;
   }  DruckerPrager_Extra_Particle;

   /** Rheology class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
   #define __DruckerPrager_Extra \
      /* Parent info */ \
      __VonMises \
      /* Virtual functions go here */ \
      /* General Info */\
      SwarmVariable*                                      tensileFailure;                        \
	  SwarmVariable*                                      slipRate;                        \
	  SwarmVariable*                                      slip;                        \
	  SwarmVariable*                                      fullySoftened;                        \
	  SwarmVariable*                                      plasticStrainRate;                        \
	  SwarmVariable*                                      backgroundStrainRate;              \
      ExtensionInfo_Index                                 particleExtHandle;                     \
      /* Param passed in */\
      int                                                 pressureTag;                           \
      int                                                 velocityGradientsTag;                           \
      double                                              minimumYieldStress;                    \
      double                                              frictionCoefficient;                   \
      double                                              frictionCoefficientAfterSoftening;     \
                                                                                                 \
      double curFrictionCoef;


   struct DruckerPrager_Extra { __DruckerPrager_Extra };

   /* Public Constructor */
   DruckerPrager_Extra* DruckerPrager_Extra_New(
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
      double                frictionCoefficientAfterSoftening );

   /** Private Constructor: This will accept all the virtual functions for this class as arguments. */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define DRUCKERPRAGER_EXTRA_DEFARGS \
                VONMISES_DEFARGS

	#define DRUCKERPRAGER_EXTRA_PASSARGS \
                VONMISES_PASSARGS

   DruckerPrager_Extra* _DruckerPrager_Extra_New(  DRUCKERPRAGER_EXTRA_DEFARGS  ) ;

   /* 'Stg_Component' implementations */
   void* _DruckerPrager_Extra_DefaultNew( Name name ) ;
   void _DruckerPrager_Extra_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data );

   void _DruckerPrager_Extra_Build( void* rheology, void* data );
   void _DruckerPrager_Extra_Initialise( void* rheology, void* data ) ;
   void _DruckerPrager_Extra_Destroy( void* rheology, void* data ) ;
   void _DruckerPrager_Extra_Init(
		DruckerPrager_Extra*                                     self,
		int                                                pressure_id,
		int                                                velocityGradients_id,
		double                                             minimumYieldStress,
		double                                             frictionCoefficient,
		double                                             frictionCoefficientAfterSoftening );

double _DruckerPrager_Extra_GetYieldCriterion( 
										void*                            druckerPrager_Extra,
										ConstitutiveMatrix*              constitutiveMatrix,
										MaterialPointsSwarm*             materialPointsSwarm,
										Element_LocalIndex               lElement_I,
										MaterialPoint*                   materialPoint,
										Coord                            xi );

void _DruckerPrager_Extra_HasYielded(
         void*                            rheology,
         ConstitutiveMatrix*              constitutiveMatrix,
         MaterialPointsSwarm*             materialPointsSwarm,
         Element_LocalIndex               lElement_I,
         MaterialPoint*                   materialPoint,
         double                           yieldCriterion,
         double                           yieldIndicator );

void DruckerPrager_Extra_GetSlipVector(void* druckerPrager_Extra, 
									   ConstitutiveMatrix*              constitutiveMatrix,
									   MaterialPointsSwarm*             materialPointsSwarm,
									   Element_LocalIndex               lElement_I,
									   MaterialPoint*                   materialPoint,
									   Coord                            xi, 
									   double dpFrictionCoefficient);

#endif

