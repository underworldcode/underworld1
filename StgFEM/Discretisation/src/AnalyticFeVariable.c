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

#include <string.h>
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>

#include "types.h"
#include "FeMesh.h"
#include "FeVariable.h"
#include "AnalyticFeVariable.h"

#include <assert.h>

const Type AnalyticFeVariable_Type = "AnalyticFeVariable";

void* AnalyticFeVariable_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                                       _sizeOfSelf = sizeof(AnalyticFeVariable);
   Type                                                               type = AnalyticFeVariable_Type;
   Stg_Class_DeleteFunction*                                       _delete = _AnalyticFeVariable_Delete;
   Stg_Class_PrintFunction*                                         _print = _AnalyticFeVariable_Print;
   Stg_Class_CopyFunction*                                           _copy = _AnalyticFeVariable_Copy;
   Stg_Component_DefaultConstructorFunction*           _defaultConstructor = (Stg_Component_DefaultConstructorFunction*)AnalyticFeVariable_DefaultNew;
   Stg_Component_ConstructFunction*                             _construct = _AnalyticFeVariable_AssignFromXML;
   Stg_Component_BuildFunction*                                     _build = _AnalyticFeVariable_Build;
   Stg_Component_InitialiseFunction*                           _initialise = _AnalyticFeVariable_Initialise;
   Stg_Component_ExecuteFunction*                                 _execute = _AnalyticFeVariable_Execute;
   Stg_Component_DestroyFunction*                                 _destroy = _AnalyticFeVariable_Destroy;
   FieldVariable_InterpolateValueAtFunction*           _interpolateValueAt = _FeVariable_InterpolateValueAt;
   FieldVariable_GetValueFunction*             _getMinGlobalFieldMagnitude = _FeVariable_GetMinGlobalFieldMagnitude;
   FieldVariable_GetValueFunction*             _getMaxGlobalFieldMagnitude = _FeVariable_GetMaxGlobalFieldMagnitude;
   FieldVariable_CacheValuesFunction*     _cacheMinMaxGlobalFieldMagnitude = _FeVariable_CacheMinMaxGlobalFieldMagnitude;
   FieldVariable_GetCoordFunction*                _getMinAndMaxLocalCoords = _FeVariable_GetMinAndMaxLocalCoords;
   FieldVariable_GetCoordFunction*               _getMinAndMaxGlobalCoords = _FeVariable_GetMinAndMaxGlobalCoords;
   FeVariable_InterpolateWithinElementFunction*  _interpolateWithinElement = _FeVariable_InterpolateNodeValuesToElLocalCoord;
   FeVariable_GetValueAtNodeFunction*                      _getValueAtNode = _FeVariable_GetValueAtNode;

   /* 
    * Variables that are set to ZERO are variables that will be set either by the 
    * current _New function or another parent _New function further up the hierachy.
    */
   AllocationType                   nameAllocationType = ZERO;
   FeVariable_SyncShadowValuesFunc* _syncShadowValues = ZERO;

   return (AnalyticFeVariable*) _AnalyticFeVariable_New( ANALYTICFEVARIABLE_PASSARGS );
}         

AnalyticFeVariable* _AnalyticFeVariable_New( ANALYTICFEVARIABLE_DEFARGS ) {
   AnalyticFeVariable* self;
   
   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(AnalyticFeVariable) );

   /* 
    * The following terms are parameters that have been passed into this function 
    * but are being set before being passed onto the parent.
    * This means that any values of these parameters that are passed into this function 
    * are not passed onto the parent function and so should be set to 
    * ZERO in any children of this class. 
    */
   nameAllocationType = NON_GLOBAL;
   _syncShadowValues = _FeVariable_SyncShadowValues;

   self = (AnalyticFeVariable*) _FeVariable_New( FEVARIABLE_PASSARGS );

   return self;
}

void _AnalyticFeVariable_Init( void* analyticFeVariable, Name functionName, FeVariable* numericField ) {
   AnalyticFeVariable* self = (AnalyticFeVariable*) analyticFeVariable;

   self->analyticFunctionName = functionName;
   self->numericField = numericField;
}

void _AnalyticFeVariable_Delete( void* _analyticFeVariable ) {
   AnalyticFeVariable* self = (AnalyticFeVariable*) _analyticFeVariable;

   _FeVariable_Delete( self );
}

void _AnalyticFeVariable_Print( void* _analyticFeVariable, Stream* stream ) {
   AnalyticFeVariable* self = (AnalyticFeVariable*) _analyticFeVariable;

   /* Print parent */
   _FeVariable_Print( self, stream );
}

void* _AnalyticFeVariable_Copy( void* analyticFeVariable, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
   assert(0);
   return (void*)analyticFeVariable;
}

void _AnalyticFeVariable_AssignFromXML( void* analyticFeVariable, Stg_ComponentFactory* cf, void* data ) {
   AnalyticFeVariable* self = (AnalyticFeVariable*)analyticFeVariable;
   Dictionary*         dictionary = Dictionary_GetDictionary( cf->componentDict, self->name );
   Name                numericFieldName;
   Name                functionName;
   FeVariable*         numericField = NULL;

   /* 
    * NOTE:
    * This component is a special case where it calls its ancestor's *_AssignFromXML
    * instead of its immediate parent's. This is because we don't want to re-declare 
    * the required FeVariable parameters in this component's XML block.
    */
   _FieldVariable_AssignFromXML( self, cf, data );

   numericFieldName = Dictionary_GetString( dictionary, (Dictionary_Entry_Key)"NumericField" );
   numericField = (FeVariable*)LiveComponentRegister_Get( cf->LCRegister, numericFieldName );

   if( !numericField ) 
      numericField = Stg_ComponentFactory_ConstructByName( cf, (Name)numericFieldName,
         FeVariable, True, data );

   functionName = Dictionary_GetString( dictionary, (Dictionary_Entry_Key)"Function" );

   /* 
    * NOTE:
    * The parent's *_Init function needs to be called to initialise
    * FeVariable-specific attributes.
    */
   _FeVariable_Init( (FeVariable*)self, numericField->feMesh, NULL,
      NULL, False, NULL, NULL, NULL, True, False );

   _AnalyticFeVariable_Init( self, functionName, numericField );
}

void _AnalyticFeVariable_Build( void* analyticFeVariable, void* data ) {
   AnalyticFeVariable* self = (AnalyticFeVariable*)analyticFeVariable;
   DomainContext*      context = self->context;
   DofLayout*          dofLayout = NULL;
   Name                name, varName[9];
   Variable*           variable = NULL;
   Variable*           baseVariable = NULL;
   FeMesh*             referenceMesh = NULL;
   unsigned            nDomainVerts;
   Index               var_I, node_I;
   double*             arrayPtr;
   Dof_Index           componentsCount;
   Variable_Register*  variable_Register = NULL;;

   _FeVariable_Build( self, data );

   context = self->context;
   variable_Register = context->variable_Register;
   referenceMesh = self->numericField->feMesh;
   componentsCount = self->numericField->fieldComponentCount;
   name = Stg_Object_AppendSuffix( self->numericField, (Name)"AnalyticVariable" );
   nDomainVerts = Mesh_GetDomainSize( referenceMesh, MT_VERTEX );
   
   if( componentsCount == 1 ) {
      arrayPtr = Memory_Alloc_Array_Unnamed( double, nDomainVerts );
      baseVariable = Variable_NewScalar( name,
         (AbstractContext*)context, Variable_DataType_Double, (Index*)&nDomainVerts,
         NULL, (void**)&arrayPtr, variable_Register );
   }
   else {
      for( var_I = 0; var_I < componentsCount; var_I++  )
         Stg_asprintf( &varName[var_I], "%s-Component-%d", name, var_I );

      arrayPtr = Memory_Alloc_Array_Unnamed( double, nDomainVerts * componentsCount );
      baseVariable = Variable_NewVector( name,
         (AbstractContext*)context, Variable_DataType_Double, componentsCount,
         &nDomainVerts, NULL, (void**)&arrayPtr, variable_Register,
         varName[0], varName[1], varName[2],
         varName[3], varName[4], varName[5],
         varName[6], varName[7], varName[8] );
   }
   dofLayout = DofLayout_New( "AnalyticDofLayout",
      context, variable_Register, Mesh_GetDomainSize( referenceMesh, MT_VERTEX ), referenceMesh );

   if( componentsCount == 1 )
      DofLayout_AddAllFromVariableArray( dofLayout, 1, &baseVariable );
   else {
      for( var_I = 0; var_I < componentsCount; var_I++ ) {
         variable = Variable_Register_GetByName( variable_Register, varName[var_I] );
         variable->arrayPtrPtr = &baseVariable->arrayPtr;
   
         for( node_I = 0; node_I < Mesh_GetDomainSize( referenceMesh, MT_VERTEX ); node_I++ )
            DofLayout_AddDof_ByVarName( dofLayout, varName[var_I], node_I );

         Memory_Free( varName[var_I] );
      }
   }
   Stg_Component_Build( dofLayout, NULL, False );
   Stg_Component_Initialise( dofLayout, NULL, False );
   LiveComponentRegister_Add( context->CF->LCRegister, (Stg_Component*)baseVariable );
   LiveComponentRegister_Add( context->CF->LCRegister, (Stg_Component*)dofLayout );

   self->dofLayout = dofLayout;
   self->fieldComponentCount = self->numericField->fieldComponentCount;
}

void _AnalyticFeVariable_Initialise( void* analyticFeVariable, void* data ) {
}

void _AnalyticFeVariable_Execute( void* analyticFeVariable, void* data ) {
}

void _AnalyticFeVariable_Destroy( void* analyticFeVariable, void* data ) {
   AnalyticFeVariable* self = (AnalyticFeVariable*) analyticFeVariable;
   
   _FeVariable_Destroy( self, data );
}


