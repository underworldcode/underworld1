/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**
**  This library is free software; you can redistribute it and/or
**  modify it under the terms of the GNU Lesser General Public
**  License as published by the Free Software Foundation; either
**  version 2.1 of the License, or (at your option) any later version.
**
**  This library is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public
**  License along with this library; if not, write to the Free Software
**  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdio.h>
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>
#include "Base.h"
#include "Init.h"

Bool Geothermal_Base_Init( int* argc, char** argv[] ) {
    VariableCondition_Register_Add( variableCondition_Register, DiscreetPointsVC_Type, DiscreetPointsVC_Factory );

    Stg_ComponentRegister_Add( Stg_ComponentRegister_Get_ComponentRegister(), ThermalDataShape_Type, (Name)"0", (Stg_Component_DefaultConstructorFunction*)_ThermalDataShape_DefaultNew);
    Stg_ComponentRegister_Add(
	Stg_ComponentRegister_Get_ComponentRegister( ),
	ThermalDataMaterial_Type, "0",
	(Stg_Component_DefaultConstructorFunction*)_ThermalDataMaterial_DefaultNew);
    Stg_ComponentRegister_Add( Stg_ComponentRegister_Get_ComponentRegister(), ConductivityMaterialManager_Type, (Name)"0", (Stg_Component_DefaultConstructorFunction*)_ConductivityMaterialManager_DefaultNew);
    Stg_ComponentRegister_Add(
	Stg_ComponentRegister_Get_ComponentRegister( ),
	IrregularQuadGenerator_Type, "0",
	(Stg_Component_DefaultConstructorFunction*) IrregularQuadGenerator_New);
    Stg_ComponentRegister_Add(
	Stg_ComponentRegister_Get_ComponentRegister( ),
	IrregularTriGenerator_Type, "0",
	(Stg_Component_DefaultConstructorFunction*) IrregularTriGenerator_New);
    Stg_ComponentRegister_Add(
	Stg_ComponentRegister_Get_ComponentRegister( ),
	Mesh_SimplexAlgorithms_Type, "0",
	(Stg_Component_DefaultConstructorFunction*) Mesh_SimplexAlgorithms_New);
    Stg_ComponentRegister_Add( Stg_ComponentRegister_Get_ComponentRegister(), FieldVariableSurfaceAdaptor_Type, (Name)"0", (Stg_Component_DefaultConstructorFunction*) FieldVariableSurfaceAdaptor_New);
    Stg_ComponentRegister_Add( Stg_ComponentRegister_Get_ComponentRegister(), FieldVariableSurfaceAdaptorSimple_Type, (Name)"0", (Stg_Component_DefaultConstructorFunction*) FieldVariableSurfaceAdaptorSimple_New);    
    Stg_ComponentRegister_Add( Stg_ComponentRegister_Get_ComponentRegister(), IrregularMeshGaussLayout_Type, (Name)"0", (Stg_Component_DefaultConstructorFunction*) _IrregularMeshGaussLayout_DefaultNew);
    Stg_ComponentRegister_Add(
	Stg_ComponentRegister_Get_ComponentRegister( ),
	DiscreetPointsVC_Type, "0",
	(Stg_Component_DefaultConstructorFunction*) _DiscreetPointsVC_DefaultNew);

   Stg_ComponentRegister_Add( Stg_ComponentRegister_Get_ComponentRegister( ),	FeVariable_IrregTri_Type, "0", (Stg_Component_DefaultConstructorFunction*) _FeVariable_IrregTri_DefaultNew);
   RegisterParent( FeVariable_IrregTri_Type, FeVariable_Type);

    RegisterParent( ThermalDataShape_Type, 		Stg_Shape_Type 			);
    RegisterParent( ThermalDataMaterial_Type,		Stg_Component_Type 		);
    RegisterParent( ConductivityMaterialManager_Type,	Stg_Component_Type 		);
    RegisterParent( IrregularQuadGenerator_Type,	MeshGenerator_Type 		);
    RegisterParent( IrregularTriGenerator_Type,		MeshGenerator_Type 		);
    RegisterParent( Mesh_SimplexType_Type,		Mesh_ElementType_Type 		);
    RegisterParent( Mesh_SimplexAlgorithms_Type,	Mesh_Algorithms_Type 		);
    RegisterParent( FieldVariableSurfaceAdaptor_Type,	MeshAdaptor_Type		);
    RegisterParent( FieldVariableSurfaceAdaptorSimple_Type,	MeshAdaptor_Type		);
    RegisterParent( DiscreetPointsVC_Type, 		VariableCondition_Type 		);
    RegisterParent( IrregularMeshGaussLayout_Type,	GaussParticleLayout_Type	);
    /*RegisterParent( MaterialLayer_Type,			Stg_Component_Type		);*/

    Stg_ComponentRegister_Add( Stg_ComponentRegister_Get_ComponentRegister(), ConstantField_Type, (Name)"0", _ConstantField_DefaultNew  );
    RegisterParent( ConstantField_Type, FeVariable_Type );
	 Stg_ComponentRegister_Add( Stg_ComponentRegister_Get_ComponentRegister(), ShapedMaterial_Type, (Name)"0", _ShapedMaterial_DefaultNew  );
    RegisterParent(ShapedMaterial_Type, Material_Type);
	 Stg_ComponentRegister_Add( Stg_ComponentRegister_Get_ComponentRegister(), MaterialRegisterShape_Type, (Name)"0", _MaterialRegisterShape_DefaultNew  );
    RegisterParent(MaterialRegisterShape_Type, Stg_Shape_Type);

    return True;
}


