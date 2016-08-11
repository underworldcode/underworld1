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
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include "types.h"
#include "FieldTest.h"
#include "FeVariable.h"
#include "OperatorFeVariable.h"
#include "AnalyticFeVariable.h"
#include "ErrorFeVariable.h"
#include "ElementType.h"
#include "FeMesh.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

const Type FieldTest_Type = "FieldTest";

void* _FieldTest_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                             _sizeOfSelf = sizeof ( FieldTest);
   Type                                                     type = FieldTest_Type;
   Stg_Class_DeleteFunction*                             _delete = _FieldTest_Delete;
   Stg_Class_PrintFunction*                               _print = _FieldTest_Print;
   Stg_Class_CopyFunction*                                 _copy = _FieldTest_Copy;
   Stg_Component_DefaultConstructorFunction* _defaultConstructor = _FieldTest_DefaultNew;
   Stg_Component_ConstructFunction*                   _construct = _FieldTest_AssignFromXML;
   Stg_Component_BuildFunction*                           _build = _FieldTest_Build;
   Stg_Component_InitialiseFunction*                 _initialise = _FieldTest_Initialise;
   Stg_Component_ExecuteFunction*                       _execute = _FieldTest_Execute;
   Stg_Component_DestroyFunction*                       _destroy = _FieldTest_Destroy;

   /*
    * Variables that are set to ZERO are variables that will be set either by
    * the current _New function or another parent _New function further up the hierachy 
    */ 
    
   /* default value NON_GLOBAL */
   AllocationType nameAllocationType = NON_GLOBAL;

   return _FieldTest_New( FIELDTEST_PASSARGS );
}

FieldTest* _FieldTest_New( FIELDTEST_DEFARGS ) {
   FieldTest* self;
   
   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof ( FieldTest) );

   /* 
    * The following terms are parameters that have been passed into this function
    * but are being set before being passed onto the parent.
    *
    * This means that any values of these parameters that are passed into this function
    * are not passed onto the parent function and so should be set to ZERO in
    * any children of this class.
    */
   nameAllocationType = NON_GLOBAL;

   /* Construct using parent */
   self = (FieldTest*)_Stg_Component_New( STG_COMPONENT_PASSARGS );

   return self;
}

void _FieldTest_Init(
   void*                   fieldTest,
   DomainContext*          context,
   Dictionary_Entry_Value* fieldMappingsList,
   unsigned                fieldMappingsCount,
   Bool                    referenceSolnFromFile,
   Bool                    highResReferenceSoln, 
   Swarm*                  integrationSwarm,
   FeMesh*                 elementMesh,
   unsigned                swarmVarCount,
   Name                    swarmVarName,
   Name                    referenceSolnPath,
   Bool                    normalise,
   double                  epsilon,
   double                  testTimestep,
   Bool                    appendToAnalysisFile,
   Bool                    testAtRestartStep,
   Name                    expectedFileName,
   Name                    expectedFilePath,
   Name                    dumpExpectedFileName,
   Bool                    expectedPass,
   Dictionary_Entry_Value* swarmVarList,
   void*                   data )
{
   FieldTest*              self = (FieldTest*)fieldTest;
   FieldVariable_Register* fV_Register;
   Index                   swarmVar_I;
   Name                    numericFieldName;
   Name                    analyticFieldName;
   Name                    analyticMagFieldName;
   Name                    errorFieldName;
   Name                    errorMagFieldName;
   Index                   updatedFieldCount;
   Hook*                   generateErrorFields;
   Hook*                   physicsTestHook;
   FieldMapping*           fieldMapping = NULL;
   Dictionary*             fieldDict = NULL;
   Index                   fieldRead_I = 0;
   Index                   fieldInsert_I = 0;

   /* Assign context to this component. */
   self->context = context;
   fV_Register = self->context->fieldVariable_Register;

   self->fieldCount = fieldMappingsCount;

   if( self->fieldCount ) {
      /* Allocate memory for our field mappings. */
      self->fieldMappings = Memory_Alloc_Array( FieldMapping*, self->fieldCount, "Field Mappings" );

      fieldInsert_I = 0;
      updatedFieldCount = self->fieldCount;

      for( fieldRead_I = 0; fieldRead_I < self->fieldCount; fieldRead_I++ ) {
         fieldDict = Dictionary_Entry_Value_AsDictionary(
            Dictionary_Entry_Value_GetElement( fieldMappingsList, fieldRead_I ) );

         /* Allocate memory for local field mapping. */
         fieldMapping = Memory_Alloc_Unnamed( FieldMapping );
         fieldMapping->numericField = Memory_Alloc_Unnamed( FeVariable );
         fieldMapping->analyticField = Memory_Alloc_Unnamed( AnalyticFeVariable );
         fieldMapping->analyticMagField = Memory_Alloc_Unnamed( OperatorFeVariable );
         fieldMapping->errorField = Memory_Alloc_Unnamed( ErrorFeVariable );
         fieldMapping->errorMagField = Memory_Alloc_Unnamed( OperatorFeVariable );

         /* Read in the Fields from the FieldMappings block. */
         numericFieldName = Dictionary_GetString( fieldDict, (Dictionary_Entry_Key)"NumericField" );
         analyticFieldName = Dictionary_GetString( fieldDict, (Dictionary_Entry_Key)"AnalyticField" );
         analyticMagFieldName = Dictionary_GetString( fieldDict, (Dictionary_Entry_Key)"AnalyticMagnitudeField" );
         errorFieldName = Dictionary_GetString( fieldDict, (Dictionary_Entry_Key)"ErrorField" );
         errorMagFieldName = Dictionary_GetString( fieldDict, (Dictionary_Entry_Key)"ErrorMagnitudeField" );

         /* Retrieve the field from the register and store it in a local variable. */
         fieldMapping->numericField = (FeVariable*)FieldVariable_Register_GetByName(
            fV_Register, numericFieldName );
         fieldMapping->analyticField = (AnalyticFeVariable*)FieldVariable_Register_GetByName(
            fV_Register, analyticFieldName );
         fieldMapping->analyticMagField = (OperatorFeVariable*)FieldVariable_Register_GetByName( fV_Register, analyticMagFieldName );
         fieldMapping->errorField = (ErrorFeVariable*)FieldVariable_Register_GetByName(
            fV_Register, errorFieldName ); 
         fieldMapping->errorMagField = (OperatorFeVariable*)FieldVariable_Register_GetByName( fV_Register, errorMagFieldName ); 

         /*
          * If the numericField can't be found in register and can't be constructed
          * through the factory then skip this entry and take one off self->fieldCount 
          * needed for simple regression test.
          */
         if( !fieldMapping->numericField ) {
            fieldMapping->numericField = (FeVariable*)Stg_ComponentFactory_ConstructByName(
               self->context->CF, (Name)numericFieldName, FeVariable, False, data ); 

            if( fieldMapping->numericField == NULL ) { 
               updatedFieldCount--; 
               continue; 
            }
         }

         if( !fieldMapping->analyticField ) 
            fieldMapping->analyticField = (AnalyticFeVariable*)Stg_ComponentFactory_ConstructByName(
               self->context->CF, (Name)analyticFieldName, AnalyticFeVariable, False, data );

         if( !fieldMapping->errorField ) 
            fieldMapping->errorField = (ErrorFeVariable*)Stg_ComponentFactory_ConstructByName(
               self->context->CF, (Name)errorFieldName, ErrorFeVariable, False, data );

         if( !fieldMapping->analyticMagField )
            fieldMapping->analyticMagField = (OperatorFeVariable*)Stg_ComponentFactory_ConstructByName(
               self->context->CF, (Name)analyticMagFieldName, OperatorFeVariable, False, data );

         if( !fieldMapping->errorMagField )
            fieldMapping->errorMagField = (OperatorFeVariable*)Stg_ComponentFactory_ConstructByName(
               self->context->CF, (Name)errorMagFieldName, OperatorFeVariable, False, data );

         /* Save field mapping to the data structure. */
         self->fieldMappings[fieldInsert_I] = fieldMapping;

         /* Know the field was found, so update insert index. */
         fieldInsert_I++;
      }
      /* Assign the updatedFieldCount back to the object's field count. */
      self->fieldCount = updatedFieldCount;
   }

   self->integrationSwarm = integrationSwarm;
   self->elementMesh = elementMesh;
   self->swarmVarCount = swarmVarCount;

   if( self->swarmVarCount ) {
      self->swarmVarNameList = Memory_Alloc_Array( Name, self->swarmVarCount, "numeric swarm variable names" );
   
      for( swarmVar_I = 0; swarmVar_I < self->swarmVarCount; swarmVar_I++ ) {
         self->swarmVarNameList[swarmVar_I] = ( swarmVarList ) ? 
            StG_Strdup(
               Dictionary_Entry_Value_AsString( Dictionary_Entry_Value_GetElement( swarmVarList, swarmVar_I ) ) ):
            StG_Strdup( swarmVarName );
      }   
   }

   self->referenceSolnPath = referenceSolnPath;
   self->appendToAnalysisFile = appendToAnalysisFile;
   self->testTimestep = testTimestep;
   self->testAtRestartStep = testAtRestartStep;

   /* Set attributes for the Physics test. */
   self->expectedFileName = expectedFileName;
   self->expectedFilePath = expectedFilePath;
   self->dumpExpectedFileName = dumpExpectedFileName;
   self->expectedPass = False;

   self->normalise = normalise;
   self->epsilon = epsilon;
   self->referenceSolnFromFile = referenceSolnFromFile;
   self->highResReferenceSoln = highResReferenceSoln;

   /* Set up the entry point for error analysis. */
   generateErrorFields = Hook_New( "Generate error fields hook", FieldTest_GenerateErrFields, self->name );

   if( !self->testAtRestartStep ) 
      _EntryPoint_AppendHook(
         Context_GetEntryPoint( self->context, AbstractContext_EP_FrequentOutput ), generateErrorFields );
   else 
      _EntryPoint_AppendHook(
         Context_GetEntryPoint( self->context, AbstractContext_EP_Dump ), generateErrorFields );
   

   /* Entry point for the Physics test function. */
   if( strlen( self->expectedFileName ) > 1 ) {
      physicsTestHook = Hook_New( "Physics test hook", FieldTest_EvaluatePhysicsTest, self->name );

      _EntryPoint_AppendHook(
         Context_GetEntryPoint( self->context, AbstractContext_EP_FrequentOutput ), physicsTestHook );
   }

   /* Get the Live component register from the context and assign it to the object's register. */
   self->LCRegister = self->context->CF->LCRegister;
}

void _FieldTest_Delete( void* fieldTest ) {
   FieldTest* self = (FieldTest*)fieldTest;
   
   /* Stg_Class_Delete parent */
   _Stg_Component_Delete( self );
}

void _FieldTest_Print( void* fieldTest, Stream* stream ) {
   FieldTest* self = (FieldTest*)fieldTest;
   
   /* Print parent */
   _Stg_Component_Print( self, stream );
}

void* _FieldTest_Copy( void* fieldTest, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
   abort();
   return NULL;
}

void _FieldTest_AssignFromXML( void* fieldTest, Stg_ComponentFactory* cf, void* data ) {
   FieldTest*              self = (FieldTest*)fieldTest;
   Dictionary*             dictionary = Dictionary_GetDictionary( cf->componentDict, self->name );
   DomainContext*          context = NULL;
   Swarm*                  integrationSwarm = NULL;
   FeMesh*                 elementMesh = NULL;
   Bool                    normalise;
   Bool                    referenceSolnFromFile;
   Bool                    highResReferenceSoln;
   Bool                    appendToAnalysisFile;
   Bool                    testAtRestartStep;
   Dictionary_Entry_Value* swarmVarList;
   unsigned                swarmVarCount;
   Name                    swarmVarName;
   Name                    referenceSolnPath;
   Name                    expectedFileName;
   Name                    expectedFilePath;
   Name                    dumpExpectedFileName;
   Bool                    expectedPass;
   double                  epsilon;
   double                  testTimestep;
   Dictionary_Entry_Value* fieldMappingsList;
   Index                   fieldMappingsCount;

   context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context",
      DomainContext, False, data );

   if( !context )
      context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", DomainContext, True, data );

   /* Get the integration swarm. */
   integrationSwarm = (Swarm*)LiveComponentRegister_Get( cf->LCRegister,
      Dictionary_GetString( dictionary, (Dictionary_Entry_Key)"IntegrationSwarm" ) );

   /* Get the element mesh. */
   elementMesh = (FeMesh*)LiveComponentRegister_Get( cf->LCRegister,
      Dictionary_GetString( dictionary, (Dictionary_Entry_Key)"ElementMesh" ) );

   /* Get flag for normalisation. */
   normalise = Dictionary_Entry_Value_AsBool(
      Dictionary_Get( dictionary, (Dictionary_Entry_Key)"normaliseByAnalyticSolution" ) );
  
   /* Get flag if using reference solution from file. */
   referenceSolnFromFile = Dictionary_Entry_Value_AsBool(
      Dictionary_Get( dictionary, (Dictionary_Entry_Key)"useReferenceSolutionFromFile" ) );

   /* Get flag if using high-res reference solution from file. */
   highResReferenceSoln = Dictionary_Entry_Value_AsBool(
      Dictionary_Get( dictionary, (Dictionary_Entry_Key)"useHighResReferenceSolutionFromFile" ) );

   /* Get flag if appending to analysis file. */
   appendToAnalysisFile = Dictionary_GetBool_WithDefault(
      dictionary, (Dictionary_Entry_Key)"appendToAnalysisFile", False );

   /* Get flag if testing at restart step. */
   testAtRestartStep = Dictionary_GetBool_WithDefault(
      dictionary, (Dictionary_Entry_Key)"testAtRestartStep", False );

   /* Get the number of swarm variable count. */
   swarmVarList = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"NumericSwarmVariableNames" );
   swarmVarCount = swarmVarList ? Dictionary_Entry_Value_GetCount( swarmVarList ) : 0;
   swarmVarName = Dictionary_GetString( dictionary, (Dictionary_Entry_Key)"SwarmVariable" ); 

   /* Get the path to the reference solution. */
   referenceSolnPath = Dictionary_GetString_WithDefault( dictionary, (Dictionary_Entry_Key)"referenceSolutionFilePath", "" );

   /* For the Physics test. */
   /* Get expected filename. */
   expectedFileName = Dictionary_GetString_WithDefault( dictionary, (Dictionary_Entry_Key)"expectedFileName", "" );

   /* Get the path to the expected file. */
   expectedFilePath = Dictionary_GetString_WithDefault( dictionary, (Dictionary_Entry_Key)"expectedFilePath", "" );
 
   /* Get the expected file name. */
   dumpExpectedFileName = Dictionary_GetString_WithDefault( dictionary, (Dictionary_Entry_Key)"expectedOutputFileName", "" );

   expectedPass = False;

   /* Get value for epsilon. */
   epsilon = Dictionary_Entry_Value_AsDouble(
      Dictionary_Get( dictionary, (Dictionary_Entry_Key)"epsilon" ) );

   /* Get the time step to run the test. */
   testTimestep = Dictionary_GetInt_WithDefault( dictionary, (Dictionary_Entry_Key)"testTimestep", 0 );

   /* Get the list of field mappings. */
   fieldMappingsList = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"FieldMappings" );

   /* Get the number of field mappings. */
   fieldMappingsCount = ( fieldMappingsList ? Dictionary_Entry_Value_GetCount( fieldMappingsList ) : 1 );

   _FieldTest_Init(
      self,
      context,
      fieldMappingsList,
      fieldMappingsCount,
      referenceSolnFromFile,
      highResReferenceSoln,
      integrationSwarm,
      elementMesh,
      swarmVarCount,
      swarmVarName,
      referenceSolnPath,
      normalise,
      epsilon,
      testTimestep,
      appendToAnalysisFile,
      testAtRestartStep,
      expectedFileName,
      expectedFilePath,
      dumpExpectedFileName,
      expectedPass,
      swarmVarList,
      data );
}

void _FieldTest_Build( void* fieldTest, void* data ) {
   FieldTest* self = (FieldTest*)fieldTest;

   if( self->fieldCount ) {
      self->gAnalyticSq = Memory_Alloc_2DArray( double, self->fieldCount, 9, (Name)"global reference solution squared" );
      self->gErrorSq = Memory_Alloc_2DArray( double, self->fieldCount, 9, (Name)"global L2 error squared" );
      self->gError = Memory_Alloc_2DArray( double, self->fieldCount, 9, (Name)"global L2 error" );
      self->gErrorNorm = Memory_Alloc_2DArray( double, self->fieldCount, 9, (Name)"global L2 error normalised" );
   }
}

void _FieldTest_Initialise( void* fieldTest, void* data ) {
   FieldTest* self = (FieldTest*)fieldTest;
   Index      field_I;
   FILE*      expected_fp;
   char*      expectedFilename;
   char*      filename;
   int        num_time_steps;
   int        dof_i, dim_i;
   int        expected_i = 0;

   /* Load the reference solution from file if req'd. */
   if( self->referenceSolnFromFile ) {
      char *referenceSolnFileName;

      for( field_I = 0; field_I < self->fieldCount; field_I++ ) {
         /*
          * Create the name of the reference file, the apprendix is handled by
          * FieldTest_LoadReferenceSolutionFromFile NOT FOR THE HR CASE
          */
         referenceSolnFileName = Memory_Alloc_Array_Unnamed( char,
            strlen( (char*)self->referenceSolnPath ) +
            strlen( (char*)self->fieldMappings[field_I]->numericField->name) +
            strlen(".h5x\0") + 7 );

#ifdef READ_HDF5
         sprintf( referenceSolnFileName, "%s.%.5d.h5", self->fieldMappings[field_I]->numericField->name,
            self->testTimestep );
#else
         sprintf( referenceSolnFileName, "%s.%.5d.dat", self->fieldMappings[field_I]->numericField->name,
            self->testTimestep );
#endif

         filename = Memory_Alloc_Array_Unnamed( char,
            strlen ( self->referenceSolnPath) + strlen ( referenceSolnFileName) + 1 + 2 + 1 );

         sprintf( filename, "%s/%s", self->referenceSolnPath, referenceSolnFileName );

         FeVariable_ReadFromFile( self->fieldMappings[field_I]->analyticField, filename );
         Memory_Free( referenceSolnFileName );
      }
   }
   else if( self->highResReferenceSoln ) {
      char *referenceSolnFileName;
      char *meshFileName;
      for( field_I = 0; field_I < self->fieldCount; field_I++ ) {
         /*
          * Create the name of the reference file, the apprendix is handled by
          * FieldTest_LoadReferenceSolutionFromFile NOT FOR THE HR CASE
          */
         referenceSolnFileName = Memory_Alloc_Array_Unnamed( char,
            strlen( (char*)self->referenceSolnPath ) +
            strlen( (char*)self->fieldMappings[field_I]->numericField->name ) +
            strlen(".h5x\0") + 7 );
         
         meshFileName = Memory_Alloc_Array_Unnamed( char,
            strlen((char*)self->referenceSolnPath) + strlen("Mesh.linearMesh.") + strlen(".h5x\0") + 7 );

#ifdef READ_HDF5
         sprintf( referenceSolnFileName, "%s/%s.%.5d.h5", self->referenceSolnPath,
            self->fieldMappings[field_I]->numericField->name, self->testTimestep );

         sprintf( meshFileName, "%s/Mesh.linearMesh.%.5d.h5", self->referenceSolnPath, self->testTimestep );
#else
         sprintf( referenceSolnFileName, "%s/%s.%.5d.dat", self->referenceSolnPath,
            self->fieldMappings[field_I]->numericField->name, self->testTimestep );

         sprintf( meshFileName, "%s/Mesh.linearMesh.%.5d.dat", self->referenceSolnPath, self->testTimestep );
#endif
         self->fieldMappings[field_I]->analyticField = (AnalyticFeVariable*)FeVariable_ReadFromFileWithMesh(
               self->fieldMappings[field_I]->numericField, referenceSolnFileName, meshFileName, True );
         
         Memory_Free( referenceSolnFileName );
         Memory_Free( meshFileName );
      }
   }
   /* Calculate the analytic solutions */
   else {
      for( field_I = 0; field_I < self->fieldCount; field_I++ ) {
         FieldTest_CalculateAnalyticSolutionForField( self, field_I ); 
         FeVariable_ZeroField( self->fieldMappings[field_I]->errorField );
      }
   }

   if( strlen( self->expectedFileName ) > 1 && strcmp( self->expectedFileName, "false" ) ) {
      /* If the self->expectedFileName == False then don't go here */
      expectedFilename = Memory_Alloc_Array_Unnamed( char,
         strlen( self->expectedFilePath ) + strlen( self->expectedFileName ) + 1 );

      sprintf( expectedFilename, "%s%s", self->expectedFilePath, self->expectedFileName );
      expected_fp = fopen( expectedFilename, "r" );
      fscanf( expected_fp, "%d %d", &self->expectedDofs, &num_time_steps );
   
      self->expected = Memory_Alloc_Array_Unnamed( Event, num_time_steps + 1 );
      self->numeric = Memory_Alloc_Array_Unnamed( Event, self->context->maxTimeSteps + 1 );
      self->tolerance = Memory_Alloc_Array_Unnamed( Event, num_time_steps + 1 );

      while( !feof( expected_fp ) ) {
         fscanf( expected_fp, "%lf ", &self->expected[expected_i].time );
         
         for( dim_i = 0; dim_i < self->context->dim; dim_i++ )
            fscanf( expected_fp, "%lf ", &self->expected[expected_i].place[dim_i] );

         for( dof_i = 0; dof_i < self->expectedDofs; dof_i++ )
            fscanf( expected_fp, "%lf ", &self->expected[expected_i].value[dof_i] );

         fscanf( expected_fp, "%lf ", &self->tolerance[expected_i].time );
         
         for( dim_i = 0; dim_i < self->context->dim; dim_i++ )
            fscanf( expected_fp, "%lf ", &self->tolerance[expected_i].place[dim_i] );

         for( dof_i = 0; dof_i < self->expectedDofs; dof_i++ )
            fscanf( expected_fp, "%lf ", &self->tolerance[expected_i].value[dof_i] );

         expected_i++;
      }
      fclose( expected_fp );
      Memory_Free( expectedFilename );
   }
}

void FieldTest_CalculateAnalyticSolutionForField( void* fieldTest, Index field_I ) {
   FieldTest*          self = (FieldTest*)fieldTest;
   AnalyticFeVariable* analyticField = self->fieldMappings[field_I]->analyticField;
   FeMesh*             analyticMesh = NULL;
   AnalyticFunction*   analyticFunction;
   Index               lNode_I;
   Index               lMeshSize;
   double*             coord = NULL;
   double*             value = NULL;

   analyticMesh = analyticField->feMesh;
   analyticFunction = AnalyticFunction_Register_GetByName(
      analyticFunc_Register, analyticField->analyticFunctionName );

   lMeshSize = Mesh_GetLocalSize( analyticMesh, MT_VERTEX );
   
   value = Memory_Alloc_Array_Unnamed( double, analyticField->fieldComponentCount );
   memset( value, 0, analyticField->fieldComponentCount * sizeof( double) );

   for( lNode_I = 0; lNode_I < lMeshSize; lNode_I++ ) {
      coord = Mesh_GetVertex( analyticMesh, lNode_I );
      analyticFunction->apply( self->context, coord, value );
      FeVariable_SetValueAtNode( analyticField, lNode_I, value );
   }

   Memory_Free( value );
}

void _FieldTest_Execute( void* fieldTest, void* data ) {
}

void _FieldTest_Destroy( void* fieldTest, void* data ) {
   FieldTest* self = (FieldTest*)fieldTest;

   if( self->fieldCount ) {
      Memory_Free( self->fieldMappings );
      Memory_Free( self->gAnalyticSq );
      Memory_Free( self->gErrorSq );
      Memory_Free( self->gError );
      Memory_Free( self->gErrorNorm );
   }

   if( strlen( self->expectedFileName ) > 1 ) {
      Memory_Free( self->expected );
      Memory_Free( self->numeric );
      Memory_Free( self->tolerance );
   }

   Stg_Component_Destroy( self, data, False );
}

void FieldTest_LoadReferenceSolutionFromFile(
   FeVariable*    feVariable,
   Name           referenceSolnName,
   Name           referenceSolnPath,
   DomainContext* context )
{
   FeMesh*  feMesh = feVariable->feMesh;
   char*    filename;
   unsigned nx = 0, ny = 0, nz = 0, total;
   unsigned lineNum = 0;
   double   resolution[3];
   double*  coord;
   Index    node_I, dim_I;
   unsigned increments[3];
   double   value[3];
   unsigned lineNum0;
   unsigned dofAtEachNodeCount, dof_I;
   unsigned meshSize = Mesh_GetLocalSize( feMesh, MT_VERTEX );
   unsigned nDims = Mesh_GetDimSize( feMesh );
   double   vertex0[3], coordPrime[3];
   double   Ni[27], values[27][3];
   unsigned shapeFunc_I;
   double   *posx, *posy, *posz;
   double   **variables;
   unsigned numShapeFuncs = ( nDims == 3 ) ? 27 : 9;
   double   xi, eta, zeta;
   double   a0, b0, c0;
   double   a1, b1, c1;
   double   a2, b2, c2;
   double   m0, m1, m2, m3, m4, m5, m6;
#ifdef READ_HDF5
   hid_t    inputFile;
   hid_t    dataSet, memSpace, dataSpace;
   hsize_t  start[2], count[2], hSize;
#endif
   int      sizes[3];
   double*  data;
   int      dataPos = 0;
   double   nodeDummy;

   Stg_Component_Initialise( feMesh, context, False );
   Stg_Component_Initialise( feVariable, context, False );

   dofAtEachNodeCount = feVariable->fieldComponentCount;
   /* .  h5  \0 */
   filename = Memory_Alloc_Array_Unnamed( char,
      strlen ( referenceSolnPath) + strlen ( referenceSolnName) + 4 );

   sprintf( filename, "%s/%s", referenceSolnPath, referenceSolnName );
#ifdef READ_HDF5
   inputFile = H5Fopen( filename, H5F_ACC_RDONLY, H5P_DEFAULT );
#if (H5_VERS_MAJOR == 1 && H5_VERS_MINOR < 8) || H5Dopen_vers == 1
   dataSet = H5Dopen( inputFile, "/size" );
#else
   dataSet = H5Dopen( inputFile, "/size", H5P_DEFAULT );
#endif
   H5Dread( dataSet, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, sizes );

   nx = sizes[0];
   ny = sizes[1];
   total = nx * ny;

   if( nDims == 3 ) {
      nz = sizes[2];
      total *= nz;
   }
   H5Dclose( dataSet );

   posx = Memory_Alloc_Array_Unnamed( double, total );
   posy = Memory_Alloc_Array_Unnamed( double, total );

   if( nDims == 3 )
      posz = Memory_Alloc_Array_Unnamed( double, total );

   variables = Memory_Alloc_2DArray_Unnamed( double, total, dofAtEachNodeCount );
   data = Memory_Alloc_Array_Unnamed( double, nDims + dofAtEachNodeCount );

   hSize = nDims + dofAtEachNodeCount;
   memSpace = H5Screate_simple( 1, &hSize, NULL );
   H5Sselect_all( memSpace );
#if (H5_VERS_MAJOR == 1 && H5_VERS_MINOR < 8) || H5Dopen_vers == 1
   dataSet = H5Dopen( inputFile, "/data" );
#else
   dataSet = H5Dopen( inputFile, "/data", H5P_DEFAULT );
#endif
   dataSpace = H5Dget_space( dataSet );
   start[0] = 0;
   start[1] = 0;
   count[0] = 1;
   count[1] = nDims + dofAtEachNodeCount;
   H5Sselect_hyperslab( dataSpace, H5S_SELECT_SET, start, NULL, count, NULL );

   for( lineNum = 0; lineNum < total; lineNum++ ) {
      start[0] = lineNum;
      H5Sselect_hyperslab( dataSpace, H5S_SELECT_SET, start, NULL, count, NULL );
      H5Dread( dataSet, H5T_NATIVE_DOUBLE, memSpace, dataSpace, H5P_DEFAULT, data );

      dataPos  = 0;
      nodeDummy = data[dataPos++];
      posx[lineNum] = data[dataPos++];
      posy[lineNum] = data[dataPos++];

      if( nDims == 3 )
         posz[lineNum] = data[dataPos++];

      for( dof_I = 0; dof_I < dofAtEachNodeCount; dof_I++ )
         variables[lineNum][dof_I] = data[dataPos++];
   }

   H5Sclose( memSpace );
   H5Sclose( dataSpace );
   H5Dclose( dataSet );
#endif
   Memory_Free( data );

   resolution[0] = posx[1] - posx[0];
   resolution[1] = posy[nx] - posy[0];

   if( nDims == 3 )
      resolution[2] = posz[nx*ny] - posz[0];

   for( node_I = 0; node_I < meshSize; node_I++ ) {
      coord = Mesh_GetVertex( feMesh, node_I );
      increments[0] = (unsigned)( ( coord[0] - posx[0] ) / resolution[0] );
      increments[1] = (unsigned)( ( coord[1] - posy[0] ) / resolution[1] );

      if( nDims == 3 )increments[2] = (unsigned)( ( coord[2] - posz[0] ) / resolution[2] );
         
      for( dim_I = 0; dim_I < nDims; dim_I++ ) 
         if( increments[dim_I] % 2 == 1 )
            increments[dim_I]--;

      if( increments[0] >= nx - 2 )
         increments[0] = nx - 3;
      if( increments[1] >= ny - 2 )
         increments[1] = ny - 3;
      if( nDims == 3 && increments[2] >= nz - 2 )
         increments[2] = nz - 3;

      lineNum0 = increments[0] + nx * increments[1];

      if( nDims == 3 )
         lineNum0 += nx * ny * increments[2];

      if( lineNum0 >= total )  
         Journal_Printf( context->info,
            "[FieldTest] Interpolation error: node value: %d resolution size: %d\n",
            lineNum0,
            total );
         
      vertex0[0] = posx[lineNum0];
      vertex0[1] = posy[lineNum0];

      if( nDims == 3 )
         vertex0[2] = posz[lineNum0];
      
      /* for quadratic elements the resolution is twice the distance between the nodes */
      for( dim_I = 0; dim_I < nDims; dim_I++ )
         coordPrime[dim_I] = ( coord[dim_I] - vertex0[dim_I] ) / resolution[dim_I] - 1.0;

      /* assign the shape functions & interpolate quadratically */
      if( nDims == 2 ) {
         xi = coordPrime[0]; eta = coordPrime[1];
         a0 = xi - 1.0; b0 = eta - 1.0;
         a1 = 1.0 - xi * xi; b1 = 1.0 - eta * eta;
         a2 = xi + 1.0; b2 = eta + 1.0;
         m0 = 0.5 * xi; m1 = 0.5 * eta; m2 = 0.25 * xi * eta;

         Ni[0] = m2 * a0 * b0; Ni[1] = m1 * a1 * b0; Ni[2] = m2 * a2 * b0;
         Ni[3] = m0 * a0 * b1; Ni[4] = a1 * b1;      Ni[5] = m0 * a2 * b1;
         Ni[6] = m2 * a0 * b2; Ni[7] = m1 * a1 * b2; Ni[8] = m2 * a2 * b2;

         for( dof_I = 0; dof_I < dofAtEachNodeCount; dof_I++ ) {
            values[0][dof_I] = variables[lineNum0][dof_I];
            values[1][dof_I] = variables[lineNum0+1][dof_I];
            values[2][dof_I] = variables[lineNum0+2][dof_I];
            values[3][dof_I] = variables[lineNum0+nx][dof_I];
            values[4][dof_I] = variables[lineNum0+nx+1][dof_I];
            values[5][dof_I] = variables[lineNum0+nx+2][dof_I];
            values[6][dof_I] = variables[lineNum0+(2*nx)][dof_I];
            values[7][dof_I] = variables[lineNum0+(2*nx)+1][dof_I];
            values[8][dof_I] = variables[lineNum0+(2*nx)+2][dof_I];
               
            value[dof_I] = 0.0;

            for( shapeFunc_I = 0; shapeFunc_I < numShapeFuncs; shapeFunc_I++ )
               value[dof_I] += Ni[shapeFunc_I] * values[shapeFunc_I][dof_I];
         }
      }
      else {
         xi = coordPrime[0]; eta = coordPrime[1]; zeta = coordPrime[2];
         a0 = xi - 1.0; b0 = eta - 1.0; c0 = zeta - 1.0;
         a1 = 1.0 - xi * xi; b1 = 1.0 - eta * eta; c1 = 1.0 - zeta * zeta;
         a2 = xi + 1.0; b2 = eta + 1.0; c2 = zeta + 1.0;
         m0 = 0.5 * xi; m1 = 0.5 * eta; m2 = 0.5 * zeta;
         m3 = 0.25 * xi * eta; m4 = 0.25 * xi * zeta; m5 = 0.25 * eta * zeta;
         m6 = 0.125 * xi * eta * zeta;

         Ni[0] = m6 * a0 * b0 * c0;  Ni[1] = m5 * a1 * b0 * c0;  Ni[2] = m6 * a2 * b0 * c0;
         Ni[3] = m4 * a0 * b1 * c0;  Ni[4] = m2 * a1 * b1 * c0;  Ni[5] = m4 * a2 * b1 * c0;
         Ni[6] = m6 * a0 * b2 * c0;  Ni[7] = m5 * a1 * b2 * c0;  Ni[8] = m6 * a2 * b2 * c0;

         Ni[9] = m3 * a0 * b0 * c1;  Ni[10] = m1 * a1 * b0 * c1; Ni[11] = m3 * a2 * b0 * c1;
         Ni[12] = m0 * a0 * b1 * c1; Ni[13] = a1 * b1 * c1;      Ni[14] = m0 * a2 * b1 * c1;
         Ni[15] = m3 * a0 * b2 * c1; Ni[16] = m1 * a1 * b2 * c1; Ni[17] = m3 * a2 * b2 * c1;

         Ni[18] = m6 * a0 * b0 * c2; Ni[19] = m5 * a1 * b0 * c2; Ni[20] = m6 * a2 * b0 * c2;
         Ni[21] = m4 * a0 * b1 * c2; Ni[22] = m2 * a1 * b1 * c2; Ni[23] = m4 * a2 * b1 * c2;
         Ni[24] = m6 * a0 * b2 * c2; Ni[25] = m5 * a1 * b2 * c2; Ni[26] = m6 * a2 * b2 * c2;
            
         for( dof_I = 0; dof_I < dofAtEachNodeCount; dof_I++ ) {
            values[0][dof_I] = variables[lineNum0][dof_I];
            values[1][dof_I] = variables[lineNum0+1][dof_I];
            values[2][dof_I] = variables[lineNum0+2][dof_I];
            values[3][dof_I] = variables[lineNum0+nx][dof_I];
            values[4][dof_I] = variables[lineNum0+nx+1][dof_I];
            values[5][dof_I] = variables[lineNum0+nx+2][dof_I];
            values[6][dof_I] = variables[lineNum0+(2*nx)][dof_I];
            values[7][dof_I] = variables[lineNum0+(2*nx)+1][dof_I];
            values[8][dof_I] = variables[lineNum0+(2*nx)+2][dof_I];

            values[9][dof_I]  = variables[lineNum0+(nx*ny)][dof_I];
            values[10][dof_I] = variables[lineNum0+(nx*ny)+1][dof_I];
            values[11][dof_I] = variables[lineNum0+(nx*ny)+2][dof_I];
            values[12][dof_I] = variables[lineNum0+(nx*ny)+nx][dof_I];
            values[13][dof_I] = variables[lineNum0+(nx*ny)+nx+1][dof_I];
            values[14][dof_I] = variables[lineNum0+(nx*ny)+nx+2][dof_I];
            values[15][dof_I] = variables[lineNum0+(nx*ny)+(2*nx)][dof_I];
            values[16][dof_I] = variables[lineNum0+(nx*ny)+(2*nx)+1][dof_I];
            values[17][dof_I] = variables[lineNum0+(nx*ny)+(2*nx)+2][dof_I];

            values[18][dof_I] = variables[lineNum0+(2*nx*ny)][dof_I];
            values[19][dof_I] = variables[lineNum0+(2*nx*ny)+1][dof_I];
            values[20][dof_I] = variables[lineNum0+(2*nx*ny)+2][dof_I];
            values[21][dof_I] = variables[lineNum0+(2*nx*ny)+nx][dof_I];
            values[22][dof_I] = variables[lineNum0+(2*nx*ny)+nx+1][dof_I];
            values[23][dof_I] = variables[lineNum0+(2*nx*ny)+nx+2][dof_I];
            values[24][dof_I] = variables[lineNum0+(2*nx*ny)+(2*nx)][dof_I];
            values[25][dof_I] = variables[lineNum0+(2*nx*ny)+(2*nx)+1][dof_I];
            values[26][dof_I] = variables[lineNum0+(2*nx*ny)+(2*nx)+2][dof_I];

            value[dof_I] = 0.0;

            for( shapeFunc_I = 0; shapeFunc_I < numShapeFuncs; shapeFunc_I++ )
               value[dof_I] += Ni[shapeFunc_I] * values[shapeFunc_I][dof_I];
         }
      }
      FeVariable_SetValueAtNode( feVariable, node_I, value );
   }

   Memory_Free( filename );
   Memory_Free( posx );
   Memory_Free( posy );

   if( nDims == 3 )
      Memory_Free( posz );

   Memory_Free( variables );

#ifdef READ_HDF5
   H5Fclose( inputFile );
#endif
}

void _FieldTest_DumpToAnalysisFile( FieldTest* self, Stream* analysisStream ) {
   int              field_I, dim, dof_I;
   Index            numDofs;
   ErrorFeVariable* errorField;
   /* double        error; */

   for( field_I = 0; field_I < self->fieldCount; field_I++ ) {
      /* Should be using MT_VOLUME for the reference field mesh, but seems to have a bug. */
      errorField = self->fieldMappings[field_I]->errorField;
      numDofs = self->fieldMappings[field_I]->numericField->fieldComponentCount;
      dim = self->fieldMappings[field_I]->numericField->dim;

#if 0
      /* Fancy error measurements of magnitudes and 2ndInvars, not needed
       * but I'm leaving it in incase */
      if( dim == numDofs ) {
         /* It's a vector */
         error = StGermain_VectorMagnitude( self->gErrorNorm[field_I], dim );
      }
      else if ( numDofs > self->fieldMappings[field_I]->numericField->dim ) {
         /* Assume it's a symmetric tensor */
         error = SymmetricTensor_2ndInvariant( self->gErrorNorm[field_I], dim );   
      }
      else {
         /* It's a scalar */
         error = self->gErrorNorm[field_I][0];
      }
      Journal_Printf( analysisStream, "%.8e ", error );
#endif

      for( dof_I = 0; dof_I < numDofs; dof_I++ ) {
         if( self->normalise)
            Journal_RPrintf( analysisStream, "%.8e ", self->gErrorNorm[field_I][dof_I] );
         else
            Journal_RPrintf( analysisStream, "%.8e ", self->gErrorSq[field_I][dof_I] );
      }
   }
}

/*
 * By default, success of the Physics test is set to false.
 * This is reset if the test passes.
 */
void FieldTest_EvaluatePhysicsTest( void* _context, void* data ) {
   DomainContext*                context = (DomainContext*)_context;
   FieldTest*                    self = (FieldTest*)LiveComponentRegister_Get( context->CF->LCRegister, (Name)FieldTest_Type );
   FieldTest_ExpectedResultFunc* expectedFunc = self->expectedFunc;
   FILE*                         dumpExpectedFilePtr;
   char*                         dumpExpectedFileName;
   int                           dim_i, dof_i;

   if( expectedFunc( self->expectedData, context, self->expected, self->numeric, self->tolerance ) )
      self->expectedPass = True;

   if( strlen ( self->dumpExpectedFileName) > 1 ) {
      dumpExpectedFileName = Memory_Alloc_Array_Unnamed( char,
         strlen ( self->expectedFilePath) + strlen ( self->dumpExpectedFileName) + 5 );

      sprintf( dumpExpectedFileName, "%s%s.out", self->expectedFilePath, self->dumpExpectedFileName );
      dumpExpectedFilePtr = fopen( dumpExpectedFileName, "a" );
      fprintf( dumpExpectedFilePtr, "%.8e ", self->numeric[context->timeStep].time );

      for( dim_i = 0; dim_i < context->dim; dim_i++ )
         fprintf( dumpExpectedFilePtr, "%.8e ", self->numeric[context->timeStep].place[dim_i] );

      for( dof_i = 0; dof_i < self->expectedDofs; dof_i++ )
         fprintf( dumpExpectedFilePtr, "%.8e ", self->numeric[context->timeStep].value[dof_i] );

      fprintf( dumpExpectedFilePtr, "\n" );

      if( context->timeStep == context->maxTimeSteps ) {
         if( self->expectedPass )
            fprintf( dumpExpectedFilePtr, "[FieldTest] Test Result: PASS\n" );
         else
            fprintf( dumpExpectedFilePtr, "[FieldTest] Test Result: FAIL\n" );
      }

      Memory_Free( dumpExpectedFileName );
      fclose( dumpExpectedFilePtr );
   }
}

void FieldTest_GenerateErrFields( void* _context, void* data ) {
   DomainContext*   context = (DomainContext*)_context;
   FieldTest*       self = (FieldTest*)LiveComponentRegister_Get( context->CF->LCRegister, (Name)FieldTest_Type );
   ErrorFeVariable* errorField;
   Index            lMeshSize, lElement_I;
   double           elErrorSq[9], elNormSq[9], elError[9];
   double           lAnalyticSq[9], gAnalyticSq[9];
   double           lErrorSq[9], gErrorSq[9];
   Bool             normalise = self->normalise;
   Index            numDofs, dof_I, fieldCount;
   Index            field_I;
   Stream*          analysisStream;
   /* double        eps = self->epsilon; */
   
   /*
    * If testTimestep is NOT initialise and NOT equal to the current timestep
    * we skip this function.
    */
   if( self->testTimestep != context->timeStep && self->testTimestep != 0 )
      return;

   fieldCount = self->fieldCount;

   if(!self->highResReferenceSoln && !self->referenceSolnFromFile){
      for( field_I = 0; field_I < self->fieldCount; field_I++ ) {
         FieldTest_CalculateAnalyticSolutionForField( self, field_I );
         FeVariable_ZeroField( self->fieldMappings[field_I]->errorField );
      }
   }

   if( self->appendToAnalysisFile ) {
      printf("[FieldTest] Appending analysis results to file...\n");
      /* Append (or create if not found) a file to report results. */
      double length, elementResI;
      char*  filename;

      analysisStream = Journal_Register( Info_Type, (Name)self->type );
      Stg_asprintf( &filename, "%s-analysis.cvg", self->name );
      Stream_AppendFile( analysisStream, filename );
      Memory_Free( filename );

      /* Write heading names in file */
      Journal_RPrintf( analysisStream, "#Res ");

      for( field_I = 0; field_I < fieldCount; field_I++ ) {
         numDofs = self->fieldMappings[field_I]->numericField->fieldComponentCount;

         for( dof_I = 0; dof_I < numDofs; dof_I++ ) 
            Journal_RPrintf( analysisStream, "%s%d ", (self->fieldMappings[field_I]->numericField)->name, dof_I+1 );
      }

      length = Dictionary_GetDouble( context->CF->rootDict, "maxX" ) -
         Dictionary_GetDouble( context->CF->rootDict, "minX" );

      elementResI = Dictionary_GetInt( context->CF->rootDict, (Dictionary_Entry_Key)"elementResI" );

      /* Assume square resolution */
      Journal_RPrintf( analysisStream, "\n%e ", length/elementResI );
   }
   
   for( field_I = 0; field_I < fieldCount; field_I++ ) {
      /* Should be using MT_VOLUME for the reference field mesh, but seems to have a bug */
      errorField = self->fieldMappings[field_I]->errorField;
      lMeshSize = Mesh_GetLocalSize( errorField->constantMesh, MT_VERTEX );
      numDofs = (self->fieldMappings[field_I]->numericField)->fieldComponentCount;

      for( dof_I = 0; dof_I < numDofs; dof_I++ ) {
         lAnalyticSq[dof_I] = 0.0;
         lErrorSq[dof_I] = 0.0;
      }
   
      for( lElement_I = 0; lElement_I < lMeshSize; lElement_I++ ) {
         for( dof_I = 0; dof_I < numDofs; dof_I++ ) {
            elErrorSq[dof_I] = 0.0;
            elNormSq[dof_I] = 0.0;
         }

         if( self->referenceSolnFromFile ) 
            FieldTest_EvaluateErrorFromField( self, field_I, lElement_I, elErrorSq, elNormSq, REFERENCE_SOLUTION );
         else if( self->highResReferenceSoln ) 
            FieldTest_EvaluateErrorFromField( self, field_I, lElement_I, elErrorSq, elNormSq, HIGHRES_REFERENCE_SOLUTION );
         else 
            FieldTest_EvaluateErrorFromField( self, field_I, lElement_I, elErrorSq, elNormSq, ANALYTIC_SOLUTION );

         for( dof_I = 0; dof_I < numDofs; dof_I++ ) {
            lAnalyticSq[dof_I] += elNormSq[dof_I];
            lErrorSq[dof_I] += elErrorSq[dof_I];
            /* elError[dof_I] = normalise ? sqrt( elErrorSq[dof_I] /
               ( elNormSq[dof_I] + eps ) ) : sqrt( elErrorSq[dof_I] );*/
            elError[dof_I] = normalise ? sqrt( elErrorSq[dof_I] / ( elNormSq[dof_I] ) ) : sqrt( elErrorSq[dof_I] );
         }
         /* Constant mesh, so node and element indices map 1:1 */
         FeVariable_SetValueAtNode( errorField, lElement_I, elError );
      }
  
      /*
       * TODO: These MPI calls should be updated to point to the correct
       * communicator once the fields are properly re-mapped.
       */ 
      (void)MPI_Allreduce( lAnalyticSq, gAnalyticSq, numDofs, MPI_DOUBLE, MPI_SUM,
         self->fieldMappings[field_I]->analyticField->communicator );

      (void)MPI_Allreduce( lErrorSq, gErrorSq, numDofs, MPI_DOUBLE, MPI_SUM,
         self->fieldMappings[field_I]->analyticField->communicator );

      for( dof_I = 0; dof_I < numDofs; dof_I++ ) {
         self->gAnalyticSq[field_I][dof_I] = gAnalyticSq[dof_I];
         self->gErrorSq[field_I][dof_I] = gErrorSq[dof_I];
         self->gErrorNorm[field_I][dof_I] = sqrt( gErrorSq[dof_I] / gAnalyticSq[dof_I] );

         if( normalise ) {
            Journal_RPrintf( context->info, "%s - dof %d normalised global error: %.8e\n", 
               self->fieldMappings[field_I]->numericField->name, dof_I,
               self->gErrorNorm[field_I][dof_I] );
         }
         else {
            Journal_RPrintf( context->info, "%s - dof %d global error: %.8e\n",
               self->fieldMappings[field_I]->numericField->name, dof_I,
               sqrt( self->gErrorSq[field_I][dof_I] ) );
         }
      }
   }

   if( self->appendToAnalysisFile ) {
      _FieldTest_DumpToAnalysisFile( self, analysisStream );
      Journal_RPrintf( analysisStream, "\n" );
      Stream_CloseAndFreeFile( analysisStream );
   }
   if( self->testAtRestartStep ) {
      char* readPathString = Context_GetCheckPointReadPrefixString((AbstractContext*)context);

      if( self->referenceSolnFromFile ) {
         Journal_Printf( context->info, "\n[FieldTest] Testing of checkpoint data located at %s\n"
            "against reference solution located at %s\ncomplete.\n\n",
            readPathString, self->referenceSolnPath );
      }
      else {
         Journal_Printf( context->info,
            "\n[FieldTest] Testing of checkpoint data located at %s\nagainst analytic solution complete.\n\n",
            readPathString );
      }
      Memory_Free( readPathString );
      context->gracefulQuit = True;
   }
}

void FieldTest_EvaluateErrorFromField(
   void*            fieldTest,
   Index            field_I,
   Index            lElement_I,
   double*          elErrorSq,
   double*          elNormSq, 
   EvaluateSolution type )
{
   FieldTest*          self = (FieldTest*)fieldTest;
   AnalyticFeVariable* analyticField = self->fieldMappings[field_I]->analyticField;
   FeVariable*         numericField = self->fieldMappings[field_I]->numericField;
   ErrorFeVariable*    errorField = self->fieldMappings[field_I]->errorField;
   FeMesh*             referenceMesh = analyticField->feMesh;
   FeMesh*             elementMesh = self->elementMesh;
   Index               constantElNode = lElement_I;
   double*             coord = Mesh_GetVertex( errorField->constantMesh, constantElNode );
   unsigned            nDims;
   Index               el_I, elementMeshElem;
   ElementType*        elType;
   Swarm*              intSwarm = self->integrationSwarm;
   Index               cell_I;
   unsigned            cellParticleCount;
   Index               cParticle_I;
   IntegrationPoint*   cParticle;
   double              *xi, weight;
   double              globalCoord[3];
   double              detJac;
   double              solution[9], numeric[9];
   Index               numDofs = numericField->fieldComponentCount;
   Index               dof_I;
   Index               solution_I;
   AnalyticFunction*   analyticFunction = NULL;

   for( solution_I = 0; solution_I < 9; solution_I ++ ) {
      solution[solution_I] = 0.0;
      numeric[solution_I] = 0.0;
   }

   /* Check what mode are we using. */
   switch( type ) {
      case ANALYTIC_SOLUTION:
         nDims = Mesh_GetDimSize( elementMesh );
         /* Corresponding analytic solution function for the FeVariable (assigned by the plugin). */
         analyticFunction = AnalyticFunction_Register_GetByName( analyticFunc_Register,
            analyticField->analyticFunctionName );
         break;
      case REFERENCE_SOLUTION:
      case HIGHRES_REFERENCE_SOLUTION:
         nDims = Mesh_GetDimSize( referenceMesh );
         /* Not assuming that constant error field mesh and reference field field mesh maps 1:1. */
         Mesh_SearchElements( referenceMesh, coord, &el_I );
         break;
   }

   Mesh_SearchElements( elementMesh, coord, &elementMeshElem );
   elType = FeMesh_GetElementType( elementMesh, elementMeshElem );

   cell_I = CellLayout_MapElementIdToCellId( intSwarm->cellLayout, elementMeshElem );
   cellParticleCount = intSwarm->cellParticleCountTbl[cell_I];

   for( cParticle_I = 0; cParticle_I < cellParticleCount; cParticle_I++ ) {
      cParticle = (IntegrationPoint*)Swarm_ParticleInCellAt( intSwarm, cell_I, cParticle_I );
      xi = cParticle->xi;
      weight = cParticle->weight;
      detJac = ElementType_JacobianDeterminant( elType, elementMesh, elementMeshElem, xi, nDims );

      switch( type ) {
         case ANALYTIC_SOLUTION:
            FeMesh_CoordLocalToGlobal( elementMesh, elementMeshElem, xi, globalCoord );
            analyticFunction->apply( self->context, globalCoord, solution );
            break;
         case REFERENCE_SOLUTION:
         case HIGHRES_REFERENCE_SOLUTION:
            FeMesh_CoordLocalToGlobal( referenceMesh, el_I, xi, globalCoord );
            FieldVariable_InterpolateValueAt( analyticField, globalCoord, solution );
            break;
      }
      FieldVariable_InterpolateValueAt( numericField, globalCoord, numeric );

      for( dof_I = 0; dof_I < numDofs; dof_I++ ) {
         elErrorSq[dof_I] += ( numeric[dof_I] - solution[dof_I] ) *
            ( numeric[dof_I] - solution[dof_I] ) * weight * detJac;

         elNormSq[dof_I] += solution[dof_I] * solution[dof_I] * weight * detJac;
      }
   }
}

void FieldTest_ElementErrAnalyticFromSwarm(
   void*   fieldTest,
   Index   var_I,
   Index   lElement_I,
   double* elErrorSq,
   double* elNormSq ) {
}

void FieldTest_ElementErrReferenceFromSwarm(
   void*   fieldTest,
   Index   var_I,
   Index   lElement_I,
   double* elErrorSq,
   double* elNormSq ) {
}

