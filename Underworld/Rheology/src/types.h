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


#ifndef __Underworld_Rheology_types_h__
#define __Underworld_Rheology_types_h__

	typedef struct ConstitutiveMatrix               ConstitutiveMatrix;
	typedef struct ConstitutiveMatrixCartesian      ConstitutiveMatrixCartesian;
	typedef struct Rheology_Register                Rheology_Register;
	typedef struct RheologyMaterial                 RheologyMaterial;
	typedef struct MultiRheologyMaterial            MultiRheologyMaterial;
	typedef struct Compressible                     Compressible;

	typedef struct Rheology                         Rheology;
	typedef struct MaterialViscosity                MaterialViscosity;
	typedef struct Arrhenius                        Arrhenius;
	typedef struct Anisotropic                      Anisotropic;
	typedef struct OrthotropicAligned               OrthotropicAligned;
	typedef struct Orthotropic                      Orthotropic;
	typedef struct FrankKamenetskii                 FrankKamenetskii;
	typedef struct NonNewtonian                     NonNewtonian;
	// typedef struct StrainDependent	                StrainDependent;
	typedef struct GALENonNewtonian                 GALENonNewtonian;
	typedef struct DepthDependentViscosity          DepthDependentViscosity;
	typedef struct LinearViscosity                  LinearViscosity;
	typedef struct ViscosityFieldRheology           ViscosityFieldRheology;
	typedef struct ViscosityLimiter                 ViscosityLimiter;
	
	typedef struct YieldRheology                    YieldRheology;
	typedef struct VonMises                         VonMises;
	typedef struct Byerlee                          Byerlee;
	
	typedef struct DruckerPrager                    DruckerPrager;
	typedef struct DruckerPrager_Extra              DruckerPrager_Extra;
	typedef struct GALEVonMises                     GALEVonMises;
	typedef struct GALEDruckerPrager                GALEDruckerPrager;
	typedef struct Pouliquen_etal                   Pouliquen_etal;
	typedef struct FaultingMoresiMuhlhaus2006       FaultingMoresiMuhlhaus2006;
	typedef struct MohrCoulomb                      MohrCoulomb;
	
	typedef struct StrainWeakening                  StrainWeakening;
	// typedef struct StrainTracking                   StrainTracking;
	typedef struct SwarmAddDouble                   SwarmAddDouble;
	typedef struct BuiterStrainWeakening	     	BuiterStrainWeakening;
	typedef struct Director                         Director;
	typedef struct AlignmentSwarmVariable           AlignmentSwarmVariable;
	
	typedef struct StoreStress                      StoreStress;
	typedef struct StoreVisc                        StoreVisc;
	
	typedef struct ConstitutiveMat_Refactored       ConstitutiveMat_Refactored;
	typedef struct ConstitutiveMatCartesian_Refactored  ConstitutiveMatCartesian_Refactored;

   typedef struct ViscousCreep  ViscousCreep;
   typedef struct PpcRheology  PpcRheology;
   typedef struct MeltViscosity  MeltViscosity;

	/* typedefs for readability */
	typedef Index                                   Rheology_Index;

#endif

