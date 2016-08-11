/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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
** Role:
**   Tests the CompositeVCSuite
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdio.h>
#include <stdlib.h>

#include "pcu/pcu.h"
#include <StGermain/StGermain.h> 
#include "StgDomain/Geometry/Geometry.h"
#include "StgDomain/Shape/Shape.h"
#include "StgDomain/Mesh/Mesh.h" 
#include "StgDomain/Utils/Utils.h"
#include "StgDomain/Swarm/Swarm.h"

#include "CompositeVCSuite.h"

typedef struct {
   MPI_Comm comm;
   int      rank;
   int      nProcs;
} CompositeVCSuiteData;

void CompositeVCSuite_quadratic(Index index, Variable_Index var_I, void* context, void* data, void* result) {
   *(double *)result = 20.0;
}

void CompositeVCSuite_exponential(Index index, Variable_Index var_I, void* context, void* data, void* result) {
   *(double *)result = 30.0;
}


Mesh* CompositeVCSuite_buildMesh( unsigned nDims, unsigned* size, double* minCrds, double* maxCrds, ExtensionManager_Register* emReg ) {
   CartesianGenerator* gen;
   Mesh*               mesh;
   unsigned            maxDecomp[3] = {0, 1, 1};

   gen = CartesianGenerator_New( "", NULL );
   gen->shadowDepth = 0;
   CartesianGenerator_SetDimSize( gen, nDims );
   CartesianGenerator_SetTopologyParams( gen, size, 0, NULL, maxDecomp );
   CartesianGenerator_SetGeometryParams( gen, minCrds, maxCrds );

   mesh = Mesh_New( "", NULL );
   Mesh_SetExtensionManagerRegister( mesh, emReg );
   Mesh_SetGenerator( mesh, gen );

   Stg_Component_Build( mesh, NULL, False );
   Stg_Component_Initialise( mesh, NULL, False );

   FreeObject( mesh->generator );

   return mesh;
}

void CompositeVCSuite_Setup( CompositeVCSuiteData* data ) {
   /* MPI Initializations */
   data->comm = MPI_COMM_WORLD;
   MPI_Comm_rank( data->comm, &data->rank );
   MPI_Comm_size( data->comm, &data->nProcs );
}

void CompositeVCSuite_Teardown( CompositeVCSuiteData* data ) {
}

void CompositeVCSuite_TestCompositeVC( CompositeVCSuiteData* data ) {
   Stream*                     stream;
   Dictionary*                 dictionary;
   Dictionary*                 sources;
   XML_IO_Handler*             io_handler;
   unsigned                    nDims = 3;
   unsigned                    meshSize[3] = {3, 3, 3};
   unsigned                    procToWatch;
   double                      minCrds[3] = {0.0, 0.0, 0.0};
   double                      maxCrds[3] = {1.0, 1.0, 1.0};
   Mesh*                       mesh;
   Variable*                   var[7];
   Variable_Register*          variable_Register;
   WallVC*                     vc;
   CompositeVC*                cvc;
   ConditionFunction*          quadCF;
   ConditionFunction*          expCF;
   ConditionFunction_Register* conFunc_Register;
   ExtensionManager_Register*  extensionMgr_Register;
   double*                     array[7];
   char*                       vcKey[] = {"WallVC_Front", "WallVC_Back", "WallVC_Left", "WallVC_Right", "WallVC_Top", "WallVC_Bottom"};
   char*                       vcKeyName[] = {"WallVC_FrontName", "WallVC_BackName", "WallVC_LeftName", "WallVC_RightName", "WallVC_TopName", "WallVC_BottomName"};
   char*                       varName[] = {"x", "y", "z", "vx", "vy", "vz", "temp"};
   char                        input_file[PCU_PATH_MAX];
   char                        expected_file[PCU_PATH_MAX];
   unsigned                    nDomains;
   Index                       i, j, k;

   procToWatch = data->nProcs >=2 ? 1 : 0;

   io_handler = XML_IO_Handler_New();

    stream = Journal_Register( Info_Type, (Name)"CompositeVCStream" );
   Stream_RedirectFile( stream, "testCompositeVC.dat" );

   dictionary = Dictionary_New();
   sources = Dictionary_New();
   Dictionary_Add( dictionary, (Dictionary_Entry_Key)"outputPath", Dictionary_Entry_Value_FromString("./output") );

   /* Input file */
   pcu_filename_input( "wallVC.xml", input_file );
   IO_Handler_ReadAllFromFile( io_handler, input_file, dictionary, sources );
   fflush( stdout );

   extensionMgr_Register = ExtensionManager_Register_New(); 

    /* Create a mesh. */
   mesh = (Mesh*) CompositeVCSuite_buildMesh( nDims, meshSize, minCrds, maxCrds, extensionMgr_Register );
   nDomains = Mesh_GetDomainSize( mesh, MT_VERTEX );

   /* Create CF stuff */
   quadCF = ConditionFunction_New( CompositeVCSuite_quadratic, (Name)"quadratic", NULL );
   expCF = ConditionFunction_New( CompositeVCSuite_exponential, (Name)"exponential", NULL);
   conFunc_Register = ConditionFunction_Register_New( );
   ConditionFunction_Register_Add(conFunc_Register, quadCF);
   ConditionFunction_Register_Add(conFunc_Register, expCF);

   /* Create variable register */
   variable_Register = Variable_Register_New();

   /* Create variables */
   for (i = 0; i < 6; i++) {
      array[i] = Memory_Alloc_Array( double, nDomains, "array[i]" );
      var[i] = Variable_NewScalar( varName[i], NULL, Variable_DataType_Double, (Index*)&nDomains, NULL, (void**)&array[i], 0 );
      Variable_Register_Add(variable_Register, var[i]);
   }
   array[6] = Memory_Alloc_Array( double, nDomains*5, "array[6]" );
   var[6] = Variable_NewVector( varName[6], NULL, Variable_DataType_Double, 5, &nDomains, NULL, (void**)&array[6], 0 );
   Variable_Register_Add(variable_Register, var[6]);
   Variable_Register_BuildAll(variable_Register);

   /* Create CompositeVC */
   cvc = CompositeVC_New( "CompositeVC", NULL, variable_Register, conFunc_Register, dictionary, mesh );

   for (i = 0; i < 6; i++) {
      vc = WallVC_New( vcKeyName[i], NULL, vcKey[i], variable_Register, conFunc_Register, dictionary, mesh );
      Stg_Component_Build( vc, 0, False );
      CompositeVC_Add(cvc, vc, True);
   }

   Stg_Component_Build( cvc, 0, False );

   for (j = 0; j < 6; j++)
      memset(array[j], 0, sizeof(double)*nDomains);
   memset(array[6], 0, sizeof(double)*nDomains*5);
   VariableCondition_Apply(cvc, NULL);

   if (data->rank == procToWatch) {
      for (j = 0; j < 6; j++) {
         Journal_Printf( stream, "\nvar[%u]: %.2lf", j, array[j][0]);
         for (k = 1; k < nDomains; k++)
            Journal_Printf( stream, ", %.2lf", array[j][k]);
      } Journal_Printf( stream, "\nvar[7]: %.2lf", array[6][0]);

      for (j = 1; j < nDomains*5; j++)
         Journal_Printf( stream, ", %.2lf", array[6][j]);
      Journal_Printf( stream, "\n\n");

      for (j = 0; j < 7; j++) {
         for (k = 0; k < nDomains; k++)
            Journal_Printf( stream, "%s ", VariableCondition_IsCondition(cvc, k, j) ? "True " : "False");
         Journal_Printf( stream, "\n");
      } Journal_Printf( stream, "\n");

      for (j = 0; j < 7; j++) {
         for (k = 0; k < nDomains; k++) {
            VariableCondition_ValueIndex  valIndex;

            valIndex = VariableCondition_GetValueIndex(cvc, k, j);
            if (valIndex != (unsigned)-1)
               Journal_Printf( stream, "%03u ", valIndex);
            else
               Journal_Printf( stream, "XXX ");
         } Journal_Printf( stream, "\n");
      } Journal_Printf( stream, "\n");

      pcu_filename_expected( "testCompositeVC.expected", expected_file );
      pcu_check_fileEq( "testCompositeVC.dat", expected_file );
      remove( "testCompositeVC.dat" );
   }

   _Stg_Component_Delete(cvc);
   Stg_Class_Delete(variable_Register);
   for (i = 0; i < 7; i++) {
      _Stg_Component_Delete(var[i]);
      if (array[i]) Memory_Free(array[i]);
   }
   Stg_Class_Delete(extensionMgr_Register);
   Stg_Class_Delete(io_handler);
   Stg_Class_Delete(conFunc_Register);
   Stg_Class_Delete(quadCF);
   Stg_Class_Delete(expCF);
   Stg_Class_Delete(dictionary);
   Stg_Class_Delete(sources);
   FreeObject( mesh );
}

void CompositeVCSuite_TestCompositeVCDictionary( CompositeVCSuiteData* data ) {
   Stream*                     stream;
   Dictionary*                 dictionary;
   Dictionary*                 sources;
   XML_IO_Handler*             io_handler;
   unsigned                    nDims = 3;
   unsigned                    meshSize[3] = {2, 2, 2};
   unsigned                    procToWatch;
   double                      minCrds[3] = {0.0, 0.0, 0.0};
   double                      maxCrds[3] = {1.0, 1.0, 1.0};
   Mesh*                       mesh;
   Variable*                   var[7];
   Variable_Register*          variable_Register;
   CompositeVC*                cvc;
   ConditionFunction*          quadCF;
   ConditionFunction*          expCF;
   ConditionFunction_Register* conFunc_Register;
   ExtensionManager_Register*  extensionMgr_Register;
   double*                     array[7];
   char*                       varName[] = {"x", "y", "z", "vx", "vy", "vz", "temp"};
   char                        input_file[PCU_PATH_MAX];
   char                        expected_file[PCU_PATH_MAX];
   unsigned                    nDomains;
   Index                       i, j, k;

   procToWatch = data->nProcs >=2 ? 1 : 0;

   io_handler = XML_IO_Handler_New();

    stream = Journal_Register( Info_Type, (Name)"CompositeVCDictionaryStream" );
   Stream_RedirectFile( stream, "testCompositeVCDictionary.dat" );

   dictionary = Dictionary_New();
   sources = Dictionary_New();
   Dictionary_Add( dictionary, (Dictionary_Entry_Key)"outputPath", Dictionary_Entry_Value_FromString("./output") );
   
   /* Input file */
   pcu_filename_input( "compositeVC.xml", input_file );
   IO_Handler_ReadAllFromFile( io_handler, input_file, dictionary, sources );
   fflush( stdout );

   extensionMgr_Register = ExtensionManager_Register_New();

   /* Create a mesh. */
   mesh = (Mesh*) CompositeVCSuite_buildMesh( nDims, meshSize, minCrds, maxCrds, extensionMgr_Register );
   nDomains = Mesh_GetDomainSize( mesh, MT_VERTEX );

   /* Create CF stuff */
   quadCF = ConditionFunction_New( CompositeVCSuite_quadratic, (Name)"quadratic", NULL );
   expCF = ConditionFunction_New( CompositeVCSuite_exponential, (Name)"exponential", NULL);
   conFunc_Register = ConditionFunction_Register_New( );
   ConditionFunction_Register_Add(conFunc_Register, quadCF);
   ConditionFunction_Register_Add(conFunc_Register, expCF);

   /* Create variable register */
   variable_Register = Variable_Register_New();

   /* Create variables */
   for (i = 0; i < 6; i++) {
      array[i] = Memory_Alloc_Array( double, 3*3*3, "array[i]" );
      var[i] = Variable_NewScalar( varName[i], NULL, Variable_DataType_Double, (Index*)&nDomains, NULL, (void**)&array[i], 0 );
       Variable_Register_Add(variable_Register, var[i]);
   }
   array[6] = Memory_Alloc_Array( double, 3*3*3*5, "array[6]" );
   var[6] = Variable_NewVector( varName[6], NULL, Variable_DataType_Double, 5, &nDomains, NULL, (void**)&array[6], 0 );
   Variable_Register_Add(variable_Register, var[6]);
   Variable_Register_BuildAll(variable_Register);

   /* Create CompositeVC */
   cvc = CompositeVC_New( "CompositeVC", NULL, variable_Register, conFunc_Register, dictionary, mesh );
   Stg_Component_Build( cvc, 0, False );

   for (j = 0; j < 6; j++)
      memset(array[j], 0, sizeof(double)*3*3*3);
   memset(array[6], 0, sizeof(double)*3*3*3*5);
   VariableCondition_Apply(cvc, NULL);

   if (data->rank == procToWatch) {
      for (j = 0; j < 6; j++) {
         Journal_Printf( stream, "\nvar[%u]: %.2lf", j, array[j][0] );
         for (k = 1; k < 3*3*3; k++)
            Journal_Printf( stream, ", %.2lf", array[j][k] );
      }

      Journal_Printf( stream, "\nvar[7]: %.2lf", array[6][0] );
      for (j = 1; j < 3*3*3*5; j++)
         Journal_Printf( stream, ", %.2lf", array[6][j] );
      Journal_Printf( stream, "\n\n" );

      for (j = 0; j < 7; j++) {
         for (k = 0; k < 27; k++)
            Journal_Printf( stream, "%s ", VariableCondition_IsCondition(cvc, k, j) ? "True " : "False" );
         Journal_Printf( stream, "\n" );
      } Journal_Printf( stream, "\n" );

      for (j = 0; j < 7; j++) {
         for (k = 0; k < 27; k++) {
            VariableCondition_ValueIndex  valIndex;
            valIndex = VariableCondition_GetValueIndex(cvc, k, j);
            if (valIndex != (unsigned)-1)
               Journal_Printf( stream, "%03u ", valIndex );
            else
               Journal_Printf( stream, "XXX " );
         } Journal_Printf( stream, "\n" );
      } Journal_Printf( stream, "\n" );

      pcu_filename_expected( "testCompositeVC-dictionary.expected", expected_file );
      pcu_check_fileEq( "testCompositeVCDictionary.dat", expected_file );
      remove( "testCompositeVCDictionary.dat" );
   }

   _Stg_Component_Delete(cvc);
   Stg_Class_Delete(variable_Register);
   for (i = 0; i < 7; i++) {
      _Stg_Component_Delete(var[i]);
       if (array[i]) Memory_Free(array[i]);
    }
   Stg_Class_Delete(conFunc_Register);
   Stg_Class_Delete(quadCF);
   Stg_Class_Delete(expCF);
   Stg_Class_Delete(dictionary);
   Stg_Class_Delete(sources);
   FreeObject( mesh );
}

void CompositeVCSuite( pcu_suite_t* suite ) {
   pcu_suite_setData( suite, CompositeVCSuiteData );
   pcu_suite_setFixtures( suite, CompositeVCSuite_Setup, CompositeVCSuite_Teardown );
   pcu_suite_addTest( suite, CompositeVCSuite_TestCompositeVC );
   pcu_suite_addTest( suite, CompositeVCSuite_TestCompositeVCDictionary );
}


