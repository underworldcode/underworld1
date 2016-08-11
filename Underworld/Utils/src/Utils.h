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


#ifndef __Underworld_Utils_Utils_h__
#define __Underworld_Utils_Utils_h__

   #include "types.h"
   #include "UnderworldContext.h"
   #include "StressField.h"
   #include "NodalPressureField.h"
   #include "SmoothVelGradField.h"
   #include "ViscosityField.h"
   #include "DensityField.h"
   #include "PressureTemperatureOutput.h"
   #include "Underworld_SwarmOutput.h"
   #include "RadiogenicHeatingTerm.h"
   #include "XDMFGenerator.h"
   #include "BaseRecoveryFeVar.h"
   #include "SPR_StrainRate.h"
   #include "REP_Algorithm.h"
   #include "RecoveredFeVariable.h"
   #include "GALEDivergenceForce.h"
   #include "GALEMixedStabiliserTerm.h"
   #include "GALEStressBC.h"

   #include "SUPGAdvDiffTermPpc.h"
   #include "AdiabaticHeatingTermPpc.h"
   #include "ViscousHeatingTermPpc.h"
   #include "BuoyancyForceTermPpc.h"

   #include "VectorAssemblyTerm_NA__F.h"
   #include "VectorAssemblyTerm_NA_i__Fi.h"
   #include "MatrixAssemblyTerm_NA_i__NB_i__F.h"
   #include "MatrixAssemblyTerm_NA__NB__F.h"
   #include "VectorSurfaceAssemblyTerm_NA__Fi__ni.h"

   #include "RateFieldTimeIntegrator.h"
   #include "MaterialIndexField.h"

   #include "RBFManager.h"
   #include "RBFFieldVariable.h"
   #include "ASCIIParticleLayout.h"
   #include "BelowHeightField.h"
   #include "BetweenHeightFields.h"
   #include "FieldValueShape.h"
   #include "PpcIntegral.h"

   #include "Init.h"
   #include "Finalise.h"

#endif 
