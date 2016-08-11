/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**   Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**   Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**   AuScope - http://www.auscope.org
**   Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**   Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**   Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**   Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**   Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
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

#include <mpi.h>
#include <stdio.h>

#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include "Discretisation.h"


Stream* StgFEM_Debug = NULL;
Stream* StgFEM_Warning = NULL;
Stream* StgFEM_Discretisation_Debug = NULL;

/* 
 * Initialises the Linear Algebra package, then any init for this package
 * such as streams etc.
 */
Bool StgFEM_Discretisation_Init( int* argc, char** argv[] ) {
   Stg_ComponentRegister* componentRegister = Stg_ComponentRegister_Get_ComponentRegister();
   int                    tmp;

   /* DO NOT CHANGE OR REMOVE */   
   Journal_Printf( Journal_Register( DebugStream_Type, (Name)"Context" ), "In: %s\n", __func__ );
   tmp = Stream_GetPrintingRank( Journal_Register( InfoStream_Type, (Name)"Context" ) );
   Stream_SetPrintingRank( Journal_Register( InfoStream_Type, (Name)"Context" ), 0 );

   Stream_Flush( Journal_Register( InfoStream_Type, (Name)"Context" ) );
   Stream_SetPrintingRank( Journal_Register( InfoStream_Type, (Name)"Context" ), tmp );
   
   /* initialise this level's streams */
   StgFEM_Debug = Journal_Register( DebugStream_Type, (Name)"StgFEM" );
   StgFEM_Discretisation_Debug = Stream_RegisterChild( StgFEM_Debug, "Discretisation" );
   StgFEM_Warning = Journal_Register( ErrorStream_Type, (Name)"StgFEM" );

   elementType_Register = ElementType_Register_New( "elementType_Register"  );
   ElementType_Register_Add( elementType_Register, (ElementType*)ConstantElementType_New( "constantElementType" ) );
   ElementType_Register_Add( elementType_Register, (ElementType*)LinearElementType_New( "linearElementType" ) );
   ElementType_Register_Add( elementType_Register, (ElementType*)BilinearElementType_New( "bilinearElementType" ) );
   ElementType_Register_Add( elementType_Register, (ElementType*)TrilinearElementType_New( "trilinearElementType" ) );
   ElementType_Register_Add( elementType_Register, (ElementType*)LinearTriangleElementType_New( "linearElementType" ) );
   ElementType_Register_Add( elementType_Register, (ElementType*)Biquadratic_New( "biquadraticElementType" ) );
   ElementType_Register_Add( elementType_Register, (ElementType*)Triquadratic_New( "triquadraticElementType" ) );
   
   Stg_ComponentRegister_Add( componentRegister, FeVariable_Type, (Name)"0", _FeVariable_DefaultNew );
   Stg_ComponentRegister_Add( componentRegister, AnalyticFeVariable_Type, (Name)"0", AnalyticFeVariable_DefaultNew );
   Stg_ComponentRegister_Add( componentRegister, ErrorFeVariable_Type, (Name)"0", ErrorFeVariable_DefaultNew );
   Stg_ComponentRegister_Add( componentRegister, LinkedDofInfo_Type, (Name)"0", _LinkedDofInfo_DefaultNew );
   Stg_ComponentRegister_Add( componentRegister, ShapeFeVariable_Type, (Name)"0", ShapeFeVariable_DefaultNew );
   Stg_ComponentRegister_Add( componentRegister, FeSwarmVariable_Type, (Name)"0", _FeSwarmVariable_DefaultNew );
   Stg_ComponentRegister_Add( componentRegister, FeMesh_Type, (Name)"0", _FeMesh_DefaultNew );
   Stg_ComponentRegister_Add( componentRegister, C0Generator_Type, (Name)"0", C0Generator_New );
   Stg_ComponentRegister_Add( componentRegister, C2Generator_Type, (Name)"0", C2Generator_New );
   /* Stg_ComponentRegister_Add( componentRegister, P1Generator_Type, (Name)"0", P1Generator_New ); */
   Stg_ComponentRegister_Add( componentRegister, Inner2DGenerator_Type, (Name)"0", Inner2DGenerator_New );
   Stg_ComponentRegister_Add( componentRegister, OperatorFeVariable_Type, (Name)"0", _OperatorFeVariable_DefaultNew );
   /* Stg_ComponentRegister_Add( componentRegister, AnalyticOperatorFeVariable_Type, (Name)"0", _AnalyticOperatorFeVariable_DefaultNew ); */
   Stg_ComponentRegister_Add( componentRegister, FieldTest_Type, (Name)"0", _FieldTest_DefaultNew );
   
   /* Register Parents for type checking */
   RegisterParent( FeMesh_Algorithms_Type, Mesh_Algorithms_Type );
   RegisterParent( FeMesh_ElementType_Type, Mesh_HexType_Type );
   RegisterParent( ElementType_Type, Stg_Component_Type );
   RegisterParent( LinearElementType_Type, ElementType_Type );
   RegisterParent( BilinearElementType_Type, ElementType_Type );
   RegisterParent( TrilinearElementType_Type, ElementType_Type );
   RegisterParent( Biquadratic_Type, ElementType_Type );
   RegisterParent( Triquadratic_Type, ElementType_Type );
   RegisterParent( P1_Type, ElementType_Type );
   RegisterParent( RegularTrilinear_Type, TrilinearElementType_Type );
   RegisterParent( ConstantElementType_Type, ElementType_Type );
   RegisterParent( LinearTriangleElementType_Type, ElementType_Type );
   RegisterParent( ElementType_Register_Type, Stg_Component_Type );
   RegisterParent( FeEquationNumber_Type, Stg_Component_Type );
   RegisterParent( LinkedDofInfo_Type, Stg_Component_Type );
   RegisterParent( FeMesh_Type, Mesh_Type );
   RegisterParent( C0Generator_Type, MeshGenerator_Type );
   RegisterParent( C2Generator_Type, CartesianGenerator_Type );
   /* RegisterParent( P1Generator_Type, MeshGenerator_Type ); */
   RegisterParent( Inner2DGenerator_Type, MeshGenerator_Type );
   RegisterParent( FeVariable_Type, FieldVariable_Type );
   RegisterParent( OperatorFeVariable_Type, FeVariable_Type );
   //RegisterParent( AnalyticOperatorFeVariable_Type, OperatorFeVariable_Type );
   RegisterParent( ShapeFeVariable_Type, FeVariable_Type );
   RegisterParent( AnalyticFeVariable_Type, FeVariable_Type );
   RegisterParent( ErrorFeVariable_Type, FeVariable_Type );
   RegisterParent( FeSwarmVariable_Type, SwarmVariable_Type );
   RegisterParent( FieldTest_Type, Stg_Component_Type );

   {
      PetscErrorCode ec;
      ec = PetscInitialize( argc, argv, (char*)0, NULL );
      CheckPETScError( ec );
   }

   return True;
}

