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


#ifndef __Underworld_Rheology_Pouliquen_etal_h__
#define __Underworld_Rheology_Pouliquen_etal_h__

	/** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
	extern const Type Pouliquen_etal_Type;

	typedef struct {
		float          brightness;
		float          opacity;
		float          diameter;
		Particle_Bool  tensileFailure;
		double         pressure;
		double         strainRateInv;
	}  Pouliquen_etal_Particle; 
	
	/** Rheology class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
	#define __Pouliquen_etal \
		/* Parent info */ \
 		__VonMises \
		/* Virtual functions go here */ \
		/* General Info */\
		SwarmVariable*                                      brightness;                            \
		SwarmVariable*                                      opacity;                               \
		SwarmVariable*                                      diameter;                              \
		SwarmVariable*                                      tensileFailure;                        \
		ExtensionInfo_Index                                 particleExtHandle;                     \
		/* Param passed in */\
		FeVariable*                                         pressureField;                         \
		FeVariable*                                         strainRateInvField;                    \
		double                                              grainDiameter;                         \
		double                                              Io;                                    \
		double                                              rho_s;                                 \
		double                                              mu_2;                                  \
		double                                              mu_s;                                  \
		double                                              mu_2_afterSoftening;                   \
		double                                              mu_s_afterSoftening;                   \
		double                                              maxViscosity;                          \
		double                                              minViscosity;                          \
		double                                              minimumYieldStress;                    \
		double                                              frictionCoefficient;                   \
		double                                              frictionCoefficientAfterSoftening;
				
	struct Pouliquen_etal { __Pouliquen_etal };
	
	/** Private Constructor: This will accept all the virtual functions for this class as arguments. */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define POULIQUEN_ETAL_DEFARGS \
                VONMISES_DEFARGS

	#define POULIQUEN_ETAL_PASSARGS \
                VONMISES_PASSARGS

	Pouliquen_etal* _Pouliquen_etal_New(  POULIQUEN_ETAL_DEFARGS  ) ;
	
	/* 'Stg_Component' implementations */
	void* _Pouliquen_etal_DefaultNew( Name name ) ;
	void _Pouliquen_etal_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data );

	void _Pouliquen_etal_Build( void* rheology, void* data );
	void _Pouliquen_etal_Initialise( void* rheology, void* data ) ;
	void _Pouliquen_etal_Destroy( void* rheology, void* data ) ;
   void _Pouliquen_etal_Init(
		Pouliquen_etal*                                     self,
		FeVariable*                                        pressureField,
		FeVariable*                                        strainRateInvField,
		MaterialPointsSwarm*                               materialPointsSwarm,
		double                                             minimumYieldStress,
		double                                             frictionCoefficient,
		double                                             frictionCoefficientAfterSoftening,
		double                                             grainDiameter,
		double                                             Io,
		double                                             rho_s,
		double                                             mu_2,
		double                                             mu_s,
		double                                             mu_2_afterSoftening,
		double                                             mu_s_afterSoftening,
		double                                             maxViscosity,
		double                                             minViscosity );
	
	/* 'YieldRheology' implementations */
	double _Pouliquen_etal_GetYieldCriterion( 
			void*                            pouliquen_etal,
			ConstitutiveMatrix*              constitutiveMatrix,
			MaterialPointsSwarm*             materialPointsSwarm,
			Element_LocalIndex               lElement_I,
			MaterialPoint*                   materialPoint,
			Coord                            xi );

	void _Pouliquen_etal_UpdateDrawParameters( void* rheology ) ;

	void _Pouliquen_etal_HasYielded( 
		void*                            pouliquen_etal,
		ConstitutiveMatrix*              constitutiveMatrix,
		MaterialPointsSwarm*             materialPointsSwarm,
		Element_LocalIndex               lElement_I,
		MaterialPoint*                   materialPoint,
		double                           yieldCriterion,
		double                           yieldIndicator );

	
#endif

