/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University 
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
**       * Redistributions of source code must retain the above copyright notice, 
**          this list of conditions and the following disclaimer.
**       * Redistributions in binary form must reproduce the above copyright 
**         notice, this list of conditions and the following disclaimer in the 
**         documentation and/or other materials provided with the distribution.
**       * Neither the name of the Monash University nor the names of its contributors 
**         may be used to endorse or promote products derived from this software 
**         without specific prior written permission.
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
** Contributors:
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/** \file 
\details Compares calculated FE Variable 
solution against the analytic solution, with the result truncated to the level of accuracy of the reference FeVariable. 
**/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>

#include <string.h>

const Type CompareFeVariableAgainstReferenceSolution_Type = "StgFEM_CompareFeVariableAgainstReferenceSolution";

void CompareFeVariableAgainstReferenceSolution_TestAll( void* compareFeVariable, void* data );
void CompareFeVariableAgainstReferenceSolution_TestVariable(
   void*       compareFeVariable,
   FeVariable* feVarToTest,
   double      tolerance,
   Bool        relativeErrorMeasure );
void _CompareFeVariableAgainstReferenceSolution_Delete( void* compareFeVariable );

typedef struct {
   __Codelet
   Stg_ComponentFactory* cf;
   char*                 referencePath;
   Swarm*                integrationSwarm;
   Stg_ObjectList*       variables;
   Stg_ObjectList*       tolerances;
   Stg_ObjectList*       relativeErrorMeasure;
   Index                 timeStepToCompare;
   char*                 importFormatType;
   char*                 exportFormatType;
   char*                 referenceFeVariableSuffix;
   Bool                  alwaysOutputErrors;
} CompareFeVariableAgainstReferenceSolution;

void _CompareFeVariableAgainstReferenceSolution_AssignFromXML( void* compareFeVariable, Stg_ComponentFactory* cf, void* data ) {
   CompareFeVariableAgainstReferenceSolution* self = (CompareFeVariableAgainstReferenceSolution*) compareFeVariable;
   AbstractContext*        context;
   Dictionary*             dictionary;
   char*                   referencePath;
   char*                   integrationSwarmName;
   char*                   varName;
   Dictionary_Entry_Value* varList;
   FeVariable*             feVarToTest;
   Index                   var_I;
   double                  tolerance;
   Bool                    relativeErrorMeasure;
   char*                   tmpName;
   Stream*                 myStream;

   context = (AbstractContext*)Stg_ComponentFactory_ConstructByName( cf, (Name)"context", AbstractContext, True, data ); 
   self->context = context;
   self->cf = cf;

   EP_AppendClassHook( 
      Context_GetEntryPoint( context, AbstractContext_EP_DumpClass ),
      CompareFeVariableAgainstReferenceSolution_TestAll, 
      self );

   self->alwaysOutputErrors = Dictionary_GetBool_WithDefault( cf->rootDict, (Dictionary_Entry_Key)"alwaysOutputErrors", False );

   dictionary = Dictionary_GetDictionary( cf->rootDict, self->name );
   Journal_Firewall(
      dictionary != NULL,
      Journal_MyStream( Error_Type, self ),
      "In func %s - Specify FeVariables to compare in struct %s\n", __func__, self->name );

   referencePath = Dictionary_GetString_WithDefault( dictionary, "referencePath", "./" );
   Journal_Printf(
      Journal_MyStream( Info_Type, self ),
      "%s: Using reference path %s\n", self->name, referencePath );
   if ( referencePath[ strlen(referencePath) ] == (int)'/' ) {
      self->referencePath = StG_Strdup( referencePath );
   } 
   else {   
      Stg_asprintf( &self->referencePath, "%s/", referencePath );
   }
   
   integrationSwarmName = Dictionary_GetString_WithDefault( dictionary, "integrationSwarm", "gaussSwarm" );
   Journal_Printf(
      Journal_MyStream( Info_Type, self ),
      "%s: Using integration swarm %s\n", self->name, integrationSwarmName );
   self->integrationSwarm = Stg_ComponentFactory_ConstructByName( cf, (Name)integrationSwarmName, Swarm, True, data ); 

   self->variables            = Stg_ObjectList_New();
   self->tolerances           = Stg_ObjectList_New();
   self->relativeErrorMeasure = Stg_ObjectList_New( );

   varList = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"variables" );
   Journal_Firewall(
      varList != NULL && Dictionary_Entry_Value_GetCount( varList ) > 0,
      Journal_MyStream( Error_Type, self ),
      "In func %s - Specify FeVariables to compare in list \"variables\" for %s\n", __func__, self->name );

   for ( var_I = 0; var_I < Dictionary_Entry_Value_GetCount( varList ); ++var_I ) {
      varName = Dictionary_Entry_Value_AsString( Dictionary_Entry_Value_GetElement( varList, var_I ) );
      feVarToTest = Stg_ComponentFactory_ConstructByName( cf, (Name)varName, FeVariable, True, data );
      Journal_Printf(
         Journal_MyStream( Info_Type, self ),
         "%s: Comparing FeVariable %s\n", self->name, varName );

      tmpName = Stg_Object_AppendSuffix( feVarToTest, (Name)"tolerance" );
      tolerance = Dictionary_GetDouble_WithDefault( dictionary, (Dictionary_Entry_Key)tmpName, 0.005 );

      tmpName = Stg_Object_AppendSuffix( feVarToTest, (Name)"useRelativeErrorMeasure" );
      relativeErrorMeasure = Dictionary_GetBool_WithDefault( dictionary, (Dictionary_Entry_Key)tmpName, False );
      
      Stg_ObjectList_Append( self->variables, feVarToTest );
      Stg_ObjectList_Append( self->tolerances, Stg_PrimitiveObject_New_Double( tolerance, (Name)varName ) );
      Stg_ObjectList_Append(
         self->relativeErrorMeasure,
         Stg_PrimitiveObject_New_Int( (relativeErrorMeasure)?1:0, (Name)varName ) );
   }

   /* Default is zero which means every time step */
   self->timeStepToCompare = Dictionary_GetUnsignedInt_WithDefault( dictionary, "timeStepToCompare", 0 );
   if ( self->timeStepToCompare == 0 ) {
      Journal_Printf(
         Journal_MyStream( Info_Type, self ),
         "%s: timeStepToCompare is 0 - All time steps will be compared\n",
         self->type );
   }

   self->referenceFeVariableSuffix = StG_Strdup(
      Dictionary_GetString_WithDefault(
         dictionary,
         "referenceFeVariableSuffix", 
         "Reference" ) );

   myStream = Journal_MyStream( Info_Type, self );
   Stg_asprintf( &tmpName, "%s.dat", self->name );
   Stream_RedirectFile_WithPrependedPath( myStream, self->context->outputPath, tmpName );
   Memory_Free( tmpName );
}

void* _CompareFeVariableAgainstReferenceSolution_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof(CompareFeVariableAgainstReferenceSolution);
   Type                                                      type = CompareFeVariableAgainstReferenceSolution_Type;
   Stg_Class_DeleteFunction*                              _delete = _CompareFeVariableAgainstReferenceSolution_Delete;
   Stg_Class_PrintFunction*                                _print = _Codelet_Print;
   Stg_Class_CopyFunction*                                  _copy = _Codelet_Copy;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _CompareFeVariableAgainstReferenceSolution_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _CompareFeVariableAgainstReferenceSolution_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _Codelet_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _Codelet_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _Codelet_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _Codelet_Destroy;

   /* 
    * Variables that are set to ZERO are variables that will be set either by the 
    * current _New function or another parent _New function further up the hierachy.
    */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return _Codelet_New(  CODELET_PASSARGS );
}

Index StgFEM_CompareFeVariableAgainstReferenceSolution_Register( PluginsManager* pluginsManager ) {
   return PluginsManager_Submit( pluginsManager, CompareFeVariableAgainstReferenceSolution_Type, (Name)"0", _CompareFeVariableAgainstReferenceSolution_DefaultNew );
}

void CompareFeVariableAgainstReferenceSolution_TestAll( void* compareFeVariable, void* data ) {
   CompareFeVariableAgainstReferenceSolution* self = (CompareFeVariableAgainstReferenceSolution*) compareFeVariable;
   FeVariable* feVarToTest;
   double      tolerance;
   int         relativeErrorMeasure;
   Index       var_I;

   /* No need to test initial conditions */
   if ( self->context->timeStep < 1 ) {
      return;
   }

   if ( self->timeStepToCompare != 0 && self->context->timeStep != self->timeStepToCompare ) {
      /* Only compare timesteps that has been selected iff timeStepToCompare is non-zero */
      return;
   }

   for ( var_I = 0; var_I < self->variables->count; ++var_I ) {
      feVarToTest = (FeVariable*)Stg_ObjectList_At( self->variables, var_I );
      tolerance = ((Stg_PrimitiveObject*)Stg_ObjectList_At( self->tolerances, var_I ))->value.asDouble;
      relativeErrorMeasure = ((Stg_PrimitiveObject*)Stg_ObjectList_At( self->relativeErrorMeasure, var_I ))->value.asInt;
      CompareFeVariableAgainstReferenceSolution_TestVariable( self, feVarToTest, tolerance, relativeErrorMeasure );
   }
}

void CompareFeVariableAgainstReferenceSolution_TestVariable(
   void*       compareFeVariable,
   FeVariable* feVarToTest,
   double      tolerance,
   Bool        relativeErrorMeasure )
{
   CompareFeVariableAgainstReferenceSolution* self = (CompareFeVariableAgainstReferenceSolution*) compareFeVariable;
   Variable_Register* variable_Register;
   Variable*          referenceDataVariable;
   Variable*          roundedDataVariable;
   Name               referenceVariableName[9];
   Name               roundedVariableName[9];
   Variable_Index     variable_I;
   DofLayout*         referenceDofLayout;
   DofLayout*         roundedDofLayout;
   Node_DomainIndex   dNode_I;
   Dof_Index          dofCountAtNode;
   Dof_Index          dofCountAtPrevNode = 0;
   Dof_Index          dof_I;
   double*            nodalValues = NULL;
   FeVariable*        referenceFeVar;
   OperatorFeVariable*        refMagnitudeField;
   FeVariable*        roundedFeVar;
   OperatorFeVariable*        errorField;
   OperatorFeVariable*        errorMagnitudeField, *relativeErrorMagnitudeField;
   OperatorFeVariable*        feVarOutput;
   char*              tmpName;
   char*              tmpName2;
   char*              filename;
   Bool               scalar;
   Dof_Index          componentsCount;
   /* TODO: hardcode for now - should be read in constructor, or read from the reference */
   /* feVar type, or file reader or something */
   unsigned int       numSigFigsInReferenceFeVar = 15;
   char*              refName = NULL;
   double             result;
   
   variable_Register = self->context->variable_Register;

   componentsCount = feVarToTest->fieldComponentCount;
   scalar = componentsCount == 1;

   /* Ok:- here, we know that the reference, or benchmark, FeVariable that we are
   comparing against may have been rounded off already, and we don't want to give
   a spurious error result just because the solution just calculated has accuracy
   beyond what the rounded benchmark is giving. Thus, we truncate the result to the
   level of the reference FeVariable */

   /* Create a DataVariable for the Reference. This serves as the memory object is is linked to */
   /* Likewise, for the rounded-off version of the "live" FeVar we are testing */
   tmpName = Stg_Object_AppendSuffix( feVarToTest, (Name)"Reference-DataVariable" );
   tmpName2 = Stg_Object_AppendSuffix( feVarToTest, (Name)"Rounded-DataVariable" );

   if( scalar == 1 ) {
      referenceDataVariable = Variable_NewScalar(
         tmpName,
         self->context,
         Variable_DataType_Double,
         (Index*)&((IGraph*)feVarToTest->feMesh->topo)->remotes[MT_VERTEX]->nDomains,
         NULL,
         (void**)NULL,
         variable_Register );

      roundedDataVariable = Variable_NewScalar(
         tmpName2,
         self->context,
         Variable_DataType_Double,
         (Index*)&((IGraph*)feVarToTest->feMesh->topo)->remotes[MT_VERTEX]->nDomains,
         NULL,
         (void**)NULL,
         variable_Register );
   }
   else {
      Journal_Firewall( 
         componentsCount <= 9,
         Journal_MyStream( Error_Type, self ),
         "In func %s - Cannot create a variable with more than 9 components (%s)\n",
         __func__,
         feVarToTest->name );
      for( variable_I = 0 ; variable_I < componentsCount; variable_I++ ) {
         Stg_asprintf( 
            &referenceVariableName[ variable_I ], 
            "%s-Reference-ComponentVariable%d", 
            feVarToTest->name, 
            variable_I );
         Stg_asprintf( 
            &roundedVariableName[ variable_I ], 
            "%s-Rounded-ComponentVariable%d", 
            feVarToTest->name, 
            variable_I );
      }
      referenceDataVariable = Variable_NewVector(
         tmpName,
         self->context,
         Variable_DataType_Double,
         componentsCount,
         &((IGraph*)feVarToTest->feMesh->topo)->remotes[MT_VERTEX]->nDomains, 
         NULL,
         (void**)NULL,
         variable_Register,
         referenceVariableName[0],
         referenceVariableName[1],
         referenceVariableName[2],
         referenceVariableName[3],
         referenceVariableName[4],
         referenceVariableName[5],
         referenceVariableName[6],
         referenceVariableName[7],
         referenceVariableName[8] );

      roundedDataVariable = Variable_NewVector(
         tmpName2,
         self->context,
         Variable_DataType_Double,
         componentsCount,
         &((IGraph*)feVarToTest->feMesh->topo)->remotes[MT_VERTEX]->nDomains, 
         NULL,
         (void**)NULL,
         variable_Register,
         roundedVariableName[0],
         roundedVariableName[1],
         roundedVariableName[2],
         roundedVariableName[3],
         roundedVariableName[4],
         roundedVariableName[5],
         roundedVariableName[6],
         roundedVariableName[7],
         roundedVariableName[8] );
   }
   Memory_Free( tmpName );
   Memory_Free( tmpName2 );
   
   referenceDataVariable->allocateSelf = True;
   roundedDataVariable->allocateSelf = True;

   Stg_Component_Build( referenceDataVariable, NULL, False );
   Stg_Component_Initialise( referenceDataVariable, NULL, False );
   Stg_Component_Build( roundedDataVariable, NULL, False );
   Stg_Component_Initialise( roundedDataVariable, NULL, False );

   /* Create Dof layout for this variable based on its own DataVariable */
   tmpName = Stg_Object_AppendSuffix( feVarToTest, (Name)"Reference-DofLayout" );
   tmpName2 = Stg_Object_AppendSuffix( feVarToTest, (Name)"Rounded-DofLayout" );
   referenceDofLayout = DofLayout_New(
      tmpName,
      (DomainContext*)self->context,
      variable_Register,
      Mesh_GetDomainSize( feVarToTest->feMesh, MT_VERTEX ),
      NULL );
   roundedDofLayout = DofLayout_New(
      tmpName2,
      (DomainContext*)self->context,
      variable_Register,
      Mesh_GetDomainSize( feVarToTest->feMesh, MT_VERTEX ),
      NULL );

   if ( scalar ) {
      DofLayout_AddAllFromVariableArray( referenceDofLayout, 1, &referenceDataVariable );
      DofLayout_AddAllFromVariableArray( roundedDofLayout, 1, &roundedDataVariable );
   }
   else {
      for ( variable_I = 0 ; variable_I < componentsCount ; variable_I++ ) {
         /* Assign variable to each node */
         for( dNode_I = 0; dNode_I < Mesh_GetDomainSize( feVarToTest->feMesh, MT_VERTEX ); dNode_I++ ) {
            DofLayout_AddDof_ByVarName( referenceDofLayout, referenceVariableName[variable_I], dNode_I );
            DofLayout_AddDof_ByVarName( roundedDofLayout, roundedVariableName[variable_I], dNode_I );
         }
         /* Free Name */
         Memory_Free( referenceVariableName[ variable_I ] );
         Memory_Free( roundedVariableName[ variable_I ] );
      }
   }
   Memory_Free( tmpName );
   Memory_Free( tmpName2 );
   
   Stg_Component_Build( referenceDofLayout, NULL, False );
   Stg_Component_Initialise( referenceDofLayout, NULL, False );
   Stg_Component_Build( roundedDofLayout, NULL, False );
   Stg_Component_Initialise( roundedDofLayout, NULL, False );

   if ( strlen( self->referenceFeVariableSuffix ) > 0 ) {
      refName = Stg_Object_AppendSuffix( feVarToTest, (Name)self->referenceFeVariableSuffix );
   }
   else {
      /* refName = Stg_Object_AppendSuffix( feVarToTest, (Name)"Reference" ); */
      /* We actually need the referenceFeVar initially to be called the same as feVarToTest,
       * so it reads the correct values - PatrickSunter, 9 Jun 2007 */ 
      refName = StG_Strdup( feVarToTest->name );
   }

   referenceFeVar = FeVariable_New_FromTemplate( 
      refName, 
      (DomainContext*)self->context,
      feVarToTest,
      referenceDofLayout, 
      NULL, 
      True,  /* isReference = True */
      False, /* Don't set the "test every timestep var", since we re-create this guy each timestep anyway*/
      feVarToTest->fieldVariable_Register );

   tmpName = Stg_Object_AppendSuffix( feVarToTest, (Name)"Rounded" );
   roundedFeVar = FeVariable_New_FromTemplate( 
      tmpName, 
      (DomainContext*)self->context,
      feVarToTest, 
      roundedDofLayout, 
      NULL, 
      False,
      False,
      feVarToTest->fieldVariable_Register );

   Memory_Free( tmpName );

   Stg_Component_Build( referenceFeVar, self->context, False );
   Stg_Component_Build( roundedFeVar, self->context, False );
   /* Note we _don't_ pass in the context to the vars below to disable checkpoint-restart,
    * since we want manual control over loading these 2 */
   Stg_Component_Initialise( referenceFeVar, NULL, False );
   Stg_Component_Initialise( roundedFeVar, NULL, False );

   filename = Memory_Alloc_Array_Unnamed( char, strlen(self->referencePath) + strlen(self->name) + 1 + 5 + 1 + 3 + 1 );
#ifdef READ_HDF5
      sprintf( filename, "%s/%s.%.5u.h5", self->referencePath, self->name, self->context->timeStep );
#else
      sprintf( filename, "%s/%s.%.5u.h5", self->referencePath, self->name, self->context->timeStep );
#endif
   FeVariable_ReadFromFile( referenceFeVar, filename );

   /* Note this is a bit inelegant, but kind of necessary unless we rewrite the FeVariable
    * checkpointing-reading code again to be more general - see my comment above when
    * refName is allocated */
   if ( 0 == strcmp( feVarToTest->name, referenceFeVar->name ) ) {
      Memory_Free( referenceFeVar->name );
      referenceFeVar->name = Stg_Object_AppendSuffix( feVarToTest, (Name)"Reference" );
   }
   tmpName = Stg_Object_AppendSuffix( feVarToTest, (Name)"MagnitudeField" );
   refMagnitudeField = OperatorFeVariable_NewUnary(
      tmpName,
      (DomainContext*)self->context,
      referenceFeVar,
      "Magnitude" );
   Memory_Free( tmpName );

   /* now we need to round off the feVar we are testing, and copy the result to the roundedFeVar */
   for( dNode_I = 0; dNode_I < Mesh_GetDomainSize( feVarToTest->feMesh, MT_VERTEX ); dNode_I++ ) {
      dofCountAtNode = feVarToTest->dofLayout->dofCounts[dNode_I];
      
      if ( dofCountAtNode != dofCountAtPrevNode ) {
         nodalValues = Memory_Realloc_Array( nodalValues, double, dofCountAtNode ); 
      }
      FeVariable_GetValueAtNode( feVarToTest, dNode_I, nodalValues );

      for ( dof_I=0; dof_I < dofCountAtNode; dof_I++ ) {
         nodalValues[dof_I] = StG_RoundDoubleToNSigFigs( nodalValues[dof_I], numSigFigsInReferenceFeVar );
      }
      FeVariable_SetValueAtNode( roundedFeVar, dNode_I, nodalValues );
      dofCountAtPrevNode = dofCountAtNode;
   }
   Memory_Free( nodalValues );

   tmpName = Stg_Object_AppendSuffix( feVarToTest, (Name)"ErrorField" );
   errorField = OperatorFeVariable_NewBinary(
      tmpName,
      (DomainContext*)self->context,
      roundedFeVar,
      referenceFeVar,
      "Subtraction" );
   Memory_Free( tmpName );

   tmpName = Stg_Object_AppendSuffix( feVarToTest, (Name)"ErrorMagnitudeField" );
   errorMagnitudeField = OperatorFeVariable_NewUnary(
      tmpName,
      (DomainContext*)self->context,
      errorField,
      "Magnitude" );
   Memory_Free( tmpName );

   tmpName = Stg_Object_AppendSuffix( feVarToTest, (Name)"RelativeErrorMagnitudeField" );
   relativeErrorMagnitudeField = OperatorFeVariable_NewBinary(
      tmpName,
      (DomainContext*)self->context,
      errorMagnitudeField,
      refMagnitudeField,
      "ScalarDivision" );
   Memory_Free( tmpName );

   /* Build and Initialise the newly-created OperatorFeVariables - else we can't use them */
   Stg_Component_Build( errorField, self->context, False );
   Stg_Component_Build( errorMagnitudeField, self->context, False );
   Stg_Component_Build( refMagnitudeField, self->context, False );
   Stg_Component_Build( relativeErrorMagnitudeField, self->context, False );
   Stg_Component_Initialise( errorField, self->context, False );
   Stg_Component_Initialise( errorMagnitudeField, self->context, False );
   Stg_Component_Initialise( refMagnitudeField, self->context, False );
   Stg_Component_Initialise( relativeErrorMagnitudeField, self->context, False );

   /* If the relativeErrorMeasure flag was used, then change the calculations and output to be relative */
   if( relativeErrorMeasure ) 
      feVarOutput = relativeErrorMagnitudeField;
   else
      feVarOutput = errorMagnitudeField;

   result = FeVariable_Integrate( feVarOutput, self->integrationSwarm );

   Journal_Printf( 
      Journal_MyStream( Info_Type, self ), 
      "Timestep %u: Total integrated value of '%s' is %s a tolerance %.5g.\n",
      self->context->timeStep, 
      feVarOutput->name, 
      result <= tolerance ? "within" : "outside", 
      tolerance );

   if( ( result > tolerance ) || (True == self->alwaysOutputErrors) ) {
      if( relativeErrorMeasure ) {
         Journal_Printf( 
            Journal_MyStream( Info_Type, self ), 
            "\t(Integrated total relative error was %g)\n",
            result );
      }
      else {
         Journal_Printf( 
            Journal_MyStream( Info_Type, self ), 
            "\t(Integrated total absolute error was %g)\n",
            result );
      }
   }   

   /* TODO: 
    * The lines below were commented out. Why? This is actually a bad memory leak!
    * PatrickSunter, 9 Jul 2007 */
   /*
   Stg_Class_Delete( referenceDataVariable );
   Stg_Class_Delete( referenceDofLayout );
   Stg_Class_Delete( referenceFeVar );
   Stg_Class_Delete( roundedDataVariable );
   Stg_Class_Delete( roundedDofLayout );
   Stg_Class_Delete( roundedFeVar );
   Stg_Class_Delete( errorField );
   Stg_Class_Delete( errorMagnitudeField );
   Stg_Class_Delete( relativeErrorMagnitudeField );
   */

   Memory_Free( refName );
}
   
void _CompareFeVariableAgainstReferenceSolution_Delete( void* compareFeVariable ) {
   CompareFeVariableAgainstReferenceSolution* self = (CompareFeVariableAgainstReferenceSolution*) compareFeVariable;

   Memory_Free( self->referencePath );
   Memory_Free( self->referenceFeVariableSuffix );
   Memory_Free( self->importFormatType );
   Memory_Free( self->exportFormatType );
}   


