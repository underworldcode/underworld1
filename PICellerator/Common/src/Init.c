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


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>

#include <PICellerator/PICellerator.h>
#include "Common.h"

#include <stdio.h>

Bool PICellerator_Common_Init( int* argc, char** argv[] ) {
	Stg_ComponentRegister* componentRegister = Stg_ComponentRegister_Get_ComponentRegister();

	Journal_Printf( Journal_Register( DebugStream_Type, (Name)"Context"  ), "In: %s\n", __func__ ); /* DO NOT CHANGE OR REMOVE */
	
	Stg_ComponentRegister_Add( componentRegister, RockProperty_Type , (Name)"0", _RockProperty_DefaultNew  );

	/* The Ppc suite */
	Stg_ComponentRegister_Add( componentRegister, PpcManager_Type,        (Name)"0", _PpcManager_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Ppc_Type,               (Name)"0", _Ppc_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Ppc_Constant_Type ,     (Name)"0", _Ppc_Constant_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Ppc_LinearDensity_Type, (Name)"0", _Ppc_LinearDensity_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Ppc_Linear_Type,        (Name)"0", _Ppc_Linear_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Ppc_Table_Type,         (Name)"0", _Ppc_Table_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Ppc_Operation_Type,     (Name)"0", _Ppc_Operation_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Ppc_Condition_Type,     (Name)"0", _Ppc_Condition_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Ppc_a_Constant_Type,    (Name)"0", _Ppc_a_Constant_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Ppc_a_Vector_Type,      (Name)"0", _Ppc_a_Vector_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Ppc_Switch_Type,        (Name)"0", _Ppc_Switch_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Ppc_MineralPhase_Type,  (Name)"0", _Ppc_MineralPhase_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Ppc_Polynomial_Type,    (Name)"0", _Ppc_Polynomial_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Ppc_LithostaticPressure_Type,     (Name)"0", _Ppc_LithostaticPressure_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Ppc_TempDependentDiffusivity_Type,(Name)"0", _Ppc_TempDependentDiffusivity_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Ppc_TempDependentDiffusivityAlt_Type,(Name)"0", _Ppc_TempDependentDiffusivityAlt_DefaultNew  );


	Stg_ComponentRegister_Add( componentRegister, MineralPhase_Type , (Name)"0", _MineralPhase_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, LookupTable_Type , (Name)"0", _LookupTable_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, PhysicalPropertyConfiguration_Type , (Name)"0", _PhysicalPropertyConfiguration_DefaultNew  );

	RegisterParent( RockProperty_Type,                  Stg_Component_Type );

	/* The Ppc suite */
	RegisterParent( PpcManager_Type,                    Stg_Component_Type );
	RegisterParent( Ppc_Type,                           Stg_Component_Type );
	RegisterParent( Ppc_Constant_Type,                  Ppc_Type );
	RegisterParent( Ppc_LinearDensity_Type,             Ppc_Type );
	RegisterParent( Ppc_Linear_Type,                    Ppc_Type );
	RegisterParent( Ppc_Table_Type,                     Ppc_Type );
	RegisterParent( Ppc_Operation_Type,                 Ppc_Type );
	RegisterParent( Ppc_Condition_Type,                 Ppc_Type );
	RegisterParent( Ppc_a_Constant_Type,                Ppc_Type );
	RegisterParent( Ppc_a_Vector_Type,                  Ppc_Type );
	RegisterParent( Ppc_Switch_Type,                    Ppc_Type );
	RegisterParent( Ppc_MineralPhase_Type,              Ppc_Type );
 	RegisterParent( Ppc_Polynomial_Type,                Ppc_Type );
	RegisterParent( Ppc_LithostaticPressure_Type,       Ppc_Type );
	RegisterParent( Ppc_TempDependentDiffusivity_Type,  Ppc_Type );
	RegisterParent( Ppc_TempDependentDiffusivityAlt_Type,  Ppc_Type );

	RegisterParent( MineralPhase_Type,                  Stg_Component_Type );
	RegisterParent( LookupTable_Type,                   Stg_Component_Type );
	RegisterParent( PhysicalPropertyConfiguration_Type, Stg_Component_Type );

	/* Register the Ppc_LinearDensityMelt component */
	Stg_ComponentRegister_Add( componentRegister, Ppc_LinearDensityMelt_Type, (Name)"0", _Ppc_LinearDensityMelt_DefaultNew  );
	RegisterParent( Ppc_LinearDensityMelt_Type,             Ppc_LinearDensity_Type );

	/* Register the ParticleMelting component */
	Stg_ComponentRegister_Add( componentRegister, ParticleMelting_Type, (Name)"0", _ParticleMelting_DefaultNew  );
	RegisterParent( ParticleMelting_Type, Stg_Component_Type );

   /* Register the PpcFeVariable component */
   Stg_ComponentRegister_Add( componentRegister, PpcFeVariable_Type, (Name)"0", _PpcFeVariable_DefaultNew  );
	RegisterParent( PpcFeVariable_Type, ParticleFeVariable_Type );

   /* Register the Ppc_PartialMelt component */
	Stg_ComponentRegister_Add( componentRegister, Ppc_PartialMelt_Type, (Name)"0", _Ppc_PartialMelt_DefaultNew  );
	RegisterParent( Ppc_PartialMelt_Type, Ppc_Type );

   /* Register the Ppc_Reciprocol component */
	Stg_ComponentRegister_Add( componentRegister, Ppc_Reciprocol_Type, (Name)"0", _Ppc_Reciprocol_DefaultNew  );
	RegisterParent( Ppc_Reciprocol_Type, Ppc_Type );

   /* Register the Ppc_MeltHeating component */
	Stg_ComponentRegister_Add( componentRegister, Ppc_MeltHeating_Type, (Name)"0", _Ppc_MeltHeating_DefaultNew  );
	RegisterParent( Ppc_MeltHeating_Type, Ppc_Type );

  /* Register the Ppc_MeltHeating component */
   Stg_ComponentRegister_Add( componentRegister, Ppc_Dt_Type, (Name)"0", _Ppc_Dt_DefaultNew  );
	RegisterParent( Ppc_Dt_Type, Ppc_Type );

   /* Register the Ppc_Variable component */
	Stg_ComponentRegister_Add( componentRegister, Ppc_Variable_Type, (Name)"0", _Ppc_Variable_DefaultNew  );
	RegisterParent( Ppc_Variable_Type, Ppc_Type );

   /* Register the Ppc_Depth component */
	Stg_ComponentRegister_Add( componentRegister, Ppc_Depth_Type, (Name)"0", _Ppc_Depth_DefaultNew  );
	RegisterParent( Ppc_Depth_Type, Ppc_Type );
 
	Stg_ComponentRegister_Add( componentRegister, Ppc_Coord_Type, (Name)"0", _Ppc_Coord_DefaultNew  );
	RegisterParent( Ppc_Coord_Type, Ppc_Type );
 
   /* Register the Ppc_PressureCalibrate component */
	Stg_ComponentRegister_Add( componentRegister, Ppc_PressureCalibrate_Type, (Name)"0", _Ppc_PressureCalibrate_DefaultNew  );
	RegisterParent( Ppc_PressureCalibrate_Type, Ppc_Type );

	Stg_ComponentRegister_Add( componentRegister, Ppc_JacobianCondition_Type,     (Name)"0", _Ppc_JacobianCondition_DefaultNew  );
	RegisterParent( Ppc_JacobianCondition_Type, Ppc_Type );

	Stg_ComponentRegister_Add( componentRegister, Ppc_IsMaterial_Type,     (Name)"0", _Ppc_IsMaterial_DefaultNew  );
	RegisterParent( Ppc_IsMaterial_Type, Ppc_Type );

	Stg_ComponentRegister_Add( componentRegister, Ppc_IsInsideShape_Type,     (Name)"0", _Ppc_IsInsideShape_DefaultNew  );
	RegisterParent( Ppc_IsInsideShape_Type, Ppc_Type );

	return True;
}





