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
#ifndef __PICellerator_Common_types_h__
#define __PICellerator_Common_types_h__
	
typedef struct RockProperty 			RockProperty;
typedef struct MineralPhase 			MineralPhase;
typedef struct LookupTable 			LookupTable;
typedef struct PhysicalPropertyConfiguration 	PhysicalPropertyConfiguration;
typedef struct ParticleMelting      ParticleMelting;

/* The ppc suite*/
typedef struct PpcManager           PpcManager;
typedef struct PpcFeVariable        PpcFeVariable;
typedef struct Ppc_Variable         Ppc_Variable;
typedef struct Ppc                  Ppc;
typedef struct Ppc_Operation        Ppc_Operation;
typedef struct Ppc_Condition        Ppc_Condition;
typedef struct Ppc_a_Constant       Ppc_a_Constant;
typedef struct Ppc_a_Vector         Ppc_a_Vector;
typedef struct Ppc_Switch           Ppc_Switch;
typedef struct Ppc_Constant         Ppc_Constant;
typedef struct Ppc_Depth            Ppc_Depth;
typedef struct Ppc_Coord            Ppc_Coord;
typedef struct Ppc_LinearDensity    Ppc_LinearDensity;
typedef struct Ppc_LinearDensityMelt    Ppc_LinearDensityMelt;
typedef struct Ppc_Linear           Ppc_Linear;
typedef struct Ppc_Table            Ppc_Table;
typedef struct Ppc_MineralPhase     Ppc_MineralPhase;
typedef struct Ppc_Polynomial       Ppc_Polynomial;
typedef struct Ppc_PartialMelt      Ppc_PartialMelt;
typedef struct Ppc_Reciprocol       Ppc_Reciprocol;
typedef struct Ppc_LithostaticPressure  Ppc_LithostaticPressure;
typedef struct Ppc_TempDependentDiffusivity Ppc_TempDependentDiffusivity;
typedef struct Ppc_TempDependentDiffusivityAlt Ppc_TempDependentDiffusivityAlt;
typedef struct Ppc_MeltHeating Ppc_MeltHeating;
typedef struct Ppc_Dt Ppc_Dt;
typedef struct Ppc_PressureCalibrate Ppc_PressureCalibrate;
typedef struct Ppc_JacobianCondition Ppc_JacobianCondition;
typedef struct Ppc_IsMaterial Ppc_IsMaterial;
typedef struct Ppc_IsInsideShape Ppc_IsInsideShape;

/* can delete _ParticleMeltingProps because Ppc takes care of melt now, 28Mar11 */
typedef enum   _ParticleMeltingProps { MELT_LATENTHEAT_FUSION, MELT_HEATCAPACITY, MELT_PREVDEPLETION, MELT_DEPLETION, MELT_FRACTION, MELT_ON, MELT_SOLIDUS_POLY, MELT_LIQUIDUS_POLY } ParticleMeltingProps;

#endif 
