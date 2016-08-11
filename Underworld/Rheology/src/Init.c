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

#include "Rheology.h"

Bool Underworld_Rheology_Init( int* argc, char** argv[] ) {
	Stg_ComponentRegister* componentRegister = Stg_ComponentRegister_Get_ComponentRegister();

	Journal_Printf( Journal_Register( DebugStream_Type, (Name)"Context"  ), "In: %s\n", __func__ ); /* DO NOT CHANGE OR REMOVE */

	Stg_ComponentRegister_Add( componentRegister, ConstitutiveMatrixCartesian_Type, (Name)"0", _ConstitutiveMatrixCartesian_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, MaterialViscosity_Type, (Name)"0", _MaterialViscosity_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, RheologyMaterial_Type, (Name)"0", _RheologyMaterial_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, MultiRheologyMaterial_Type, (Name)"0", _MultiRheologyMaterial_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Compressible_Type, (Name)"0", _Compressible_DefaultNew  );

	Stg_ComponentRegister_Add( componentRegister, Arrhenius_Type, (Name)"0", _Arrhenius_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, FrankKamenetskii_Type, (Name)"0", _FrankKamenetskii_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, NonNewtonian_Type, (Name)"0", _NonNewtonian_DefaultNew  );
	// Stg_ComponentRegister_Add( componentRegister, StrainDependent_Type, (Name)"0", _StrainDependent_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, GALENonNewtonian_Type, (Name)"0", _GALENonNewtonian_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, LinearViscosity_Type, (Name)"0", _LinearViscosity_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, DepthDependentViscosity_Type, (Name)"0", _DepthDependentViscosity_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Anisotropic_Type, (Name)"0", _Anisotropic_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, OrthotropicAligned_Type, (Name)"0", _OrthotropicAligned_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Orthotropic_Type, (Name)"0", _Orthotropic_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, ViscosityLimiter_Type, (Name)"0", _ViscosityLimiter_DefaultNew  );
	
	Stg_ComponentRegister_Add( componentRegister, VonMises_Type, (Name)"0", _VonMises_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Byerlee_Type, (Name)"0", _Byerlee_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, DruckerPrager_Type, (Name)"0", _DruckerPrager_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, DruckerPrager_Extra_Type, (Name)"0", _DruckerPrager_Extra_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, GALEVonMises_Type, (Name)"0", _GALEVonMises_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, GALEDruckerPrager_Type, (Name)"0", _GALEDruckerPrager_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, FaultingMoresiMuhlhaus2006_Type, (Name)"0", _FaultingMoresiMuhlhaus2006_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, MohrCoulomb_Type, (Name)"0", _MohrCoulomb_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Pouliquen_etal_Type, (Name)"0", _Pouliquen_etal_DefaultNew  );

	Stg_ComponentRegister_Add( componentRegister, StrainWeakening_Type, (Name)"0", _StrainWeakening_DefaultNew  );
	// Stg_ComponentRegister_Add( componentRegister, StrainTracking_Type, (Name)"0", _StrainTracking_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, SwarmAddDouble_Type, (Name)"0", _SwarmAddDouble_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, BuiterStrainWeakening_Type, (Name)"0", _BuiterStrainWeakening_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, Director_Type, (Name)"0", _Director_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, AlignmentSwarmVariable_Type, (Name)"0", _AlignmentSwarmVariable_DefaultNew  );
	
	Stg_ComponentRegister_Add( componentRegister, ViscosityFieldRheology_Type, (Name)"0", _ViscosityFieldRheology_DefaultNew  );

	Stg_ComponentRegister_Add( componentRegister, StoreStress_Type, (Name)"0", _StoreStress_DefaultNew  );
	Stg_ComponentRegister_Add( componentRegister, StoreVisc_Type, (Name)"0", _StoreVisc_DefaultNew  );
	
	Stg_ComponentRegister_Add( componentRegister, ConstitutiveMatCartesian_Refactored_Type, (Name)"0", _ConstitutiveMatCartesian_Refactored_DefaultNew  );

	/* Register Parents for type checking */
	RegisterParent( Rheology_Type,                    Stg_Component_Type );
	RegisterParent( Arrhenius_Type,                   Rheology_Type );
	RegisterParent( ViscosityLimiter_Type,            Rheology_Type );
	RegisterParent( FrankKamenetskii_Type,            Rheology_Type );
	RegisterParent( MaterialViscosity_Type,           Rheology_Type );
	RegisterParent( NonNewtonian_Type,                Rheology_Type );
	// RegisterParent( StrainDependent_Type,             Rheology_Type );
	RegisterParent( GALENonNewtonian_Type,            Rheology_Type );
	RegisterParent( DepthDependentViscosity_Type,     Rheology_Type );
	RegisterParent( LinearViscosity_Type,             Rheology_Type );
	RegisterParent( Anisotropic_Type,                 Rheology_Type );
	RegisterParent( OrthotropicAligned_Type,          Rheology_Type );
	RegisterParent( Orthotropic_Type,                 Rheology_Type );
                                                      
	RegisterParent( StoreStress_Type,                 Rheology_Type );
	RegisterParent( StoreVisc_Type,                   Rheology_Type );
	                                                  
	RegisterParent( ViscosityFieldRheology_Type,      Rheology_Type );
	RegisterParent( YieldRheology_Type,               Rheology_Type );
	RegisterParent( VonMises_Type,                    YieldRheology_Type );
	RegisterParent( Byerlee_Type,                     VonMises_Type );
	RegisterParent( DruckerPrager_Type,               VonMises_Type );
	RegisterParent( DruckerPrager_Extra_Type,         VonMises_Type );
	RegisterParent( GALEDruckerPrager_Type,           VonMises_Type );
	RegisterParent( GALEVonMises_Type,                YieldRheology_Type );
	RegisterParent( FaultingMoresiMuhlhaus2006_Type,  YieldRheology_Type );
	RegisterParent( MohrCoulomb_Type,                 YieldRheology_Type );
	RegisterParent( Pouliquen_etal_Type,              VonMises_Type );
	                                                  
	RegisterParent( StrainWeakening_Type,             TimeIntegrand_Type );
	// RegisterParent( StrainTracking_Type,              TimeIntegrand_Type );
	RegisterParent( SwarmAddDouble_Type,              TimeIntegrand_Type );
	RegisterParent( BuiterStrainWeakening_Type,       StrainWeakening_Type );
	RegisterParent( Director_Type,                    TimeIntegrand_Type );
	RegisterParent( AlignmentSwarmVariable_Type,      SwarmVariable_Type );

	RegisterParent( ConstitutiveMatrix_Type,          StiffnessMatrixTerm_Type );
	RegisterParent( ConstitutiveMatrixCartesian_Type, ConstitutiveMatrix_Type );
	RegisterParent( RheologyMaterial_Type,            Material_Type );
	RegisterParent( MultiRheologyMaterial_Type,       RheologyMaterial_Type );
	RegisterParent( Compressible_Type,                StiffnessMatrixTerm_Type );

	RegisterParent( ConstitutiveMat_Refactored_Type,          Stg_Component_Type );
	RegisterParent( ConstitutiveMatCartesian_Refactored_Type, ConstitutiveMat_Refactored_Type );

   Stg_ComponentRegister_Add( componentRegister, ViscousCreep_Type, (Name)"0", _ViscousCreep_DefaultNew  );
   RegisterParent( ViscousCreep_Type,     Rheology_Type );

   Stg_ComponentRegister_Add( componentRegister, MeltViscosity_Type, (Name)"0", _MeltViscosity_DefaultNew  );
   RegisterParent( MeltViscosity_Type, Rheology_Type );

   Stg_ComponentRegister_Add( componentRegister, PpcRheology_Type, (Name)"0", _PpcRheology_DefaultNew  );
   RegisterParent( PpcRheology_Type,     Rheology_Type );

	return True;
}


