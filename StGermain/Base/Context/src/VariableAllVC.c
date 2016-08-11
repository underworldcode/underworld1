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
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <mpi.h>
   
#include "Base/Foundation/Foundation.h"
#include "Base/IO/IO.h"
#include "Base/Container/Container.h"
#include "Base/Automation/Automation.h"
                                                                                                                                    
#include "types.h"
#include "shortcuts.h"
#include "Variable.h"
#include "Variable_Register.h"
#include "VariableCondition.h"
#include "VariableAllVC.h"
#include "ConditionFunction_Register.h"

#include <string.h>
#include <assert.h>

const Type VariableAllVC_Type = "VariableAllVC";
const Name defaultVariableAllVCName = "defaultVariableAllVCName";


/*--------------------------------------------------------------------------------------------------------------------------
** Constructor
*/

VariableCondition* VariableAllVC_Factory(
   AbstractContext*            context,
   Variable_Register*          variable_Register, 
   ConditionFunction_Register* conFunc_Register, 
   Dictionary*                 dictionary,
   void*                       data )
{
   return (VariableCondition*)VariableAllVC_New(
      defaultVariableAllVCName,
      context,
      NULL,
      variable_Register,
      conFunc_Register,
      dictionary,
      data );
}

VariableAllVC* VariableAllVC_New(
   Name                        name,
   AbstractContext*            context,
   Name                        _dictionaryEntryName, 
   Variable_Register*          variable_Register, 
   ConditionFunction_Register* conFunc_Register,
   Dictionary*                 dictionary,
   void*                       data )
{
   VariableAllVC* self = _VariableAllVC_DefaultNew( name );

   self->isConstructed = True;
   _VariableCondition_Init( self, context, variable_Register, conFunc_Register, dictionary );
   _VariableAllVC_Init( self, _dictionaryEntryName, data );

   return self;
}

VariableAllVC* _VariableAllVC_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                               _sizeOfSelf = sizeof(VariableAllVC);
   Type                                                       type = VariableAllVC_Type;
   Stg_Class_DeleteFunction*                               _delete = _VariableAllVC_Delete;
   Stg_Class_PrintFunction*                                 _print = _VariableAllVC_Print;
   Stg_Class_CopyFunction*                                   _copy = _VariableAllVC_Copy;
   Stg_Component_DefaultConstructorFunction*   _defaultConstructor = (Stg_Component_DefaultConstructorFunction*)_VariableAllVC_DefaultNew;
   Stg_Component_ConstructFunction*                     _construct = _VariableCondition_AssignFromXML;
   Stg_Component_BuildFunction*                             _build = _VariableAllVC_Build;
   Stg_Component_InitialiseFunction*                   _initialise = _VariableCondition_Initialise;
   Stg_Component_ExecuteFunction*                         _execute = _VariableCondition_Execute;
   Stg_Component_DestroyFunction*                         _destroy = _VariableAllVC_Destroy;
   AllocationType                               nameAllocationType = NON_GLOBAL;
   VariableCondition_BuildSelfFunc*                     _buildSelf = _VariableAllVC_BuildSelf;
   VariableCondition_PrintConciseFunc*               _printConcise = _VariableAllVC_PrintConcise;
   VariableCondition_ReadDictionaryFunc*           _readDictionary = _VariableAllVC_ReadDictionary;
   VariableCondition_GetSetFunc*                           _getSet = _VariableAllVC_GetSet;
   VariableCondition_GetVariableCountFunc*       _getVariableCount = _VariableAllVC_GetVariableCount;
   VariableCondition_GetVariableIndexFunc*       _getVariableIndex = _VariableAllVC_GetVariableIndex;
   VariableCondition_GetValueIndexFunc*             _getValueIndex = _VariableAllVC_GetValueIndex;
   VariableCondition_GetValueCountFunc*             _getValueCount = _VariableAllVC_GetValueCount;
   VariableCondition_GetValueFunc*                       _getValue = _VariableAllVC_GetValue;
   VariableCondition_ApplyFunc*                             _apply = _VariableCondition_Apply;

   return (VariableAllVC*)_VariableAllVC_New( VARIABLEALLVC_PASSARGS );
}

VariableAllVC* _VariableAllVC_New( VARIABLEALLVC_DEFARGS ) {
   VariableAllVC*   self;
   
   /* Allocate memory/General info */
   assert( _sizeOfSelf >= sizeof(VariableAllVC) );
   self = (VariableAllVC*)_VariableCondition_New( VARIABLECONDITION_PASSARGS );
   
   /* Virtual info */
   
   /* Stg_Class info */
   
   return self;
}

void _VariableAllVC_Init(
   void* allElementsVC,
   Name  _dictionaryEntryName, 
   void* data )
{
   VariableAllVC* self = (VariableAllVC*)allElementsVC;

   self->isConstructed = True;
   self->_dictionaryEntryName = _dictionaryEntryName;
   self->data = data;
}


/*--------------------------------------------------------------------------------------------------------------------------
** General virtual functions
*/

void _VariableAllVC_ReadDictionary( void* variableCondition, void* dictionary ) {
   VariableAllVC*            self = (VariableAllVC*)variableCondition;
   Dictionary_Entry_Value*   vcDictVal;
   Dictionary_Entry_Value    _vcDictVal;
   Dictionary_Entry_Value*   varsVal;
   VariableAllVC_Entry_Index entry_I;
   
   /* Find dictionary entry */
   if (self->_dictionaryEntryName)
      vcDictVal = Dictionary_Get( dictionary, (Dictionary_Entry_Key)self->_dictionaryEntryName  );
   else {
      vcDictVal = &_vcDictVal;
      Dictionary_Entry_Value_InitFromStruct( vcDictVal, dictionary );
   }
   
   if (vcDictVal) {
      /* Obtain the variable entries */
      self->_entryCount = Dictionary_Entry_Value_GetCount(
         Dictionary_Entry_Value_GetMember( vcDictVal, (Dictionary_Entry_Key)"variables") );

      self->_entryTbl = Memory_Alloc_Array(
         VariableAllVC_Entry,
         self->_entryCount,
         "VariableAllVC->_entryTbl" );

      varsVal = Dictionary_Entry_Value_GetMember( vcDictVal, (Dictionary_Entry_Key)"variables");
      
      for (entry_I = 0; entry_I < self->_entryCount; entry_I++ ) {
         char*                   valType;
         Dictionary_Entry_Value* valueEntry;
         Dictionary_Entry_Value* varDictListVal;
         
         varDictListVal = Dictionary_Entry_Value_GetElement(varsVal, entry_I);

         valueEntry = Dictionary_Entry_Value_GetMember(
            varDictListVal,
            (Dictionary_Entry_Key)"value" );
         
         self->_entryTbl[entry_I].varName = Dictionary_Entry_Value_AsString(
            Dictionary_Entry_Value_GetMember( varDictListVal, (Dictionary_Entry_Key)"name") );
            
         valType = Dictionary_Entry_Value_AsString(
            Dictionary_Entry_Value_GetMember( varDictListVal, (Dictionary_Entry_Key)"type") );

         if (!strcasecmp(valType, "func")) {
            char* funcName = Dictionary_Entry_Value_AsString(valueEntry);
            
            self->_entryTbl[entry_I].value.type = VC_ValueType_CFIndex;
            self->_entryTbl[entry_I].value.as.typeCFIndex = ConditionFunction_Register_GetIndex(
               self->conFunc_Register,
            funcName);
         }
         else if (!strcasecmp(valType, "array")) {
            Dictionary_Entry_Value* valueElement;
            Index                   i;

            self->_entryTbl[entry_I].value.type = VC_ValueType_DoubleArray;
            self->_entryTbl[entry_I].value.as.typeArray.size = Dictionary_Entry_Value_GetCount(valueEntry);
            self->_entryTbl[entry_I].value.as.typeArray.array = Memory_Alloc_Array(
               double,
               self->_entryTbl[entry_I].value.as.typeArray.size,
               "VariableAllVC->_entryTbl[].value.as.typeArray.array" );
               
            for (i = 0; i < self->_entryTbl[entry_I].value.as.typeArray.size; i++) {
               valueElement = Dictionary_Entry_Value_GetElement(valueEntry, i);
               self->_entryTbl[entry_I].value.as.typeArray.array[i] = 
                  Dictionary_Entry_Value_AsDouble(valueElement);
            }
         }
         else if( !strcasecmp( valType, "double" ) ||
            !strcasecmp( valType, "d" ) ||
            !strcasecmp( valType, "float" ) ||
            !strcasecmp( valType, "f" ) ) {
            self->_entryTbl[entry_I].value.type = VC_ValueType_Double;
            self->_entryTbl[entry_I].value.as.typeDouble = Dictionary_Entry_Value_AsDouble( valueEntry );
         }
         else if( !strcasecmp( valType, "integer" ) ||
            !strcasecmp( valType, "int" ) ||
            !strcasecmp( valType, "i" ) ) {
            self->_entryTbl[entry_I].value.type = VC_ValueType_Int;
            self->_entryTbl[entry_I].value.as.typeInt = Dictionary_Entry_Value_AsUnsignedInt( valueEntry );
         }
         else if( !strcasecmp( valType, "short" ) || !strcasecmp( valType, "s" ) ) {
            self->_entryTbl[entry_I].value.type = VC_ValueType_Short;
            self->_entryTbl[entry_I].value.as.typeShort = Dictionary_Entry_Value_AsUnsignedInt( valueEntry );
         }
         else if( !strcasecmp( valType, "char" ) || !strcasecmp( valType, "c" ) ) {
            self->_entryTbl[entry_I].value.type = VC_ValueType_Char;
            self->_entryTbl[entry_I].value.as.typeChar = Dictionary_Entry_Value_AsUnsignedInt( valueEntry );
         }
         else if( !strcasecmp( valType, "pointer" ) || !strcasecmp( valType, "ptr" ) || !strcasecmp( valType, "p" ) ) {
            self->_entryTbl[entry_I].value.type = VC_ValueType_Ptr;
            self->_entryTbl[entry_I].value.as.typePtr =
               (void*) ( (ArithPointer) Dictionary_Entry_Value_AsUnsignedInt( valueEntry ));
         }
         else {
            /* Assume double */
            Journal_DPrintf(
               Journal_Register( InfoStream_Type, (Name)"myStream"  ),
               "Type to variable on variable condition not given, assuming double\n" );

            self->_entryTbl[entry_I].value.type = VC_ValueType_Double;
            self->_entryTbl[entry_I].value.as.typeDouble = Dictionary_Entry_Value_AsDouble( valueEntry );
         }
      }
   }
   else {
      self->_entryCount = 0;
      self->_entryTbl = NULL;
   }
}

void _VariableAllVC_Delete( void* allElementsVC ) {
   VariableAllVC* self = (VariableAllVC*)allElementsVC;
   
   /* Stg_Class_Delete parent */
   _VariableCondition_Delete( self );
}

void _VariableAllVC_Destroy( void* allElementsVC, void* data ) {
   VariableAllVC* self = (VariableAllVC*)allElementsVC;

   if (self->_entryTbl) Memory_Free(self->_entryTbl);

   _VariableCondition_Destroy( self, data );   
}

void _VariableAllVC_Print( void* allElementsVC, Stream* stream ) {
   VariableAllVC*               self = (VariableAllVC*)allElementsVC;
   VariableAllVC_Entry_Index   entry_I;
   Index                        i;
   
   /* Set the Journal for printing informations */
   Stream* info = stream;
   
   /* General info */
   Journal_Printf( info, "VariableAllVC (ptr): %p\n", self);
   
   /* Print parent */
   _VariableCondition_Print(self);

   /* Virtual info */
   
   /* Stg_Class info */
   Journal_Printf( info, "\tdictionary (ptr): %p\n", self->dictionary);
   Journal_Printf( info, "\t_dictionaryEntryName (ptr): %p\n", self->_dictionaryEntryName);
   if (self->_dictionaryEntryName)
      Journal_Printf( info, "\t\t_dictionaryEntryName: %s\n", self->_dictionaryEntryName);
   Journal_Printf( info, "\t_entryCount: %u\n", self->_entryCount);
   Journal_Printf( info, "\t_entryTbl (ptr): %p\n", self->_entryTbl);
   if( self->_entryTbl ) {
      for (entry_I = 0; entry_I < self->_entryCount; entry_I++)
      {
         Journal_Printf( info, "\t\t_entryTbl[%u]:\n", entry_I);
         Journal_Printf( info, "\t\t\tvarName (ptr): %p\n", self->_entryTbl[entry_I].varName);
         if (self->_entryTbl[entry_I].varName)
            Journal_Printf( info, "\t\t\t\tvarName: %s\n", self->_entryTbl[entry_I].varName);
         Journal_Printf( info, "\t\t\tvalue:\n");
         switch (self->_entryTbl[entry_I].value.type)
         {
            case VC_ValueType_Double:
               Journal_Printf( info, "\t\t\t\ttype: VC_ValueType_Double\n" );
               Journal_Printf( info, "\t\t\t\tasDouble: %g\n", self->_entryTbl[entry_I].value.as.typeDouble );
               break;
               
            case VC_ValueType_Int:
               Journal_Printf( info, "\t\t\t\ttype: VC_ValueType_Int\n" );
               Journal_Printf( info, "\t\t\t\tasInt: %i\n", self->_entryTbl[entry_I].value.as.typeInt );
               break;
               
            case VC_ValueType_Short:
               Journal_Printf( info, "\t\t\t\ttype: VC_ValueType_Short\n" );
               Journal_Printf( info, "\t\t\t\tasShort: %i\n", self->_entryTbl[entry_I].value.as.typeShort );
               break;
               
            case VC_ValueType_Char:
               Journal_Printf( info, "\t\t\t\ttype: VC_ValueType_Char\n");
               Journal_Printf( info, "\t\t\t\tasChar: %c\n", self->_entryTbl[entry_I].value.as.typeChar );
               break;
               
            case VC_ValueType_Ptr:
               Journal_Printf( info, "\t\t\t\ttype: VC_ValueType_Ptr\n");
               Journal_Printf( info, "\t\t\t\tasPtr: %g\n", self->_entryTbl[entry_I].value.as.typePtr );
               break;
               
            case VC_ValueType_DoubleArray:
               Journal_Printf( info, "\t\t\t\ttype: VC_ValueType_DoubleArray\n");
               Journal_Printf( info, "\t\t\t\tarraySize: %u\n", self->_entryTbl[entry_I].value.as.typeArray.size);
               Journal_Printf( info, "\t\t\t\tasDoubleArray (ptr): %p\n", 
                  self->_entryTbl[entry_I].value.as.typeArray.array);
               if (self->_entryTbl[entry_I].value.as.typeArray.array)
                  for (i = 0; i < self->_entryTbl[entry_I].value.as.typeArray.size; i++)
                     Journal_Printf( info, "\t\t\t\tasDoubleArray[%u]: %g\n", i,
                        self->_entryTbl[entry_I].value.as.typeArray.array[i]);
               break;
               
            case VC_ValueType_CFIndex:
               Journal_Printf( info, "\t\t\t\ttype: VC_ValueType_CFIndex\n");
               Journal_Printf( info, "\t\t\t\tasCFIndex: %u\n", self->_entryTbl[entry_I].value.as.typeCFIndex);
               break;
         }
      }
   }
}


void* _VariableAllVC_Copy( void* allElementsVC, void* dest, Bool deep, Name nameExt, struct PtrMap* ptrMap ) {
   VariableAllVC*      self = (VariableAllVC*)allElementsVC;
   VariableAllVC*      newVariableAllVC;
   PtrMap*         map = ptrMap;
   Bool         ownMap = False;
   
   if( !map ) {
      map = PtrMap_New( 10 );
      ownMap = True;
   }
   
   newVariableAllVC = (VariableAllVC*)_VariableCondition_Copy( self, dest, deep, nameExt, map );
   
   newVariableAllVC->_dictionaryEntryName = self->_dictionaryEntryName;
   newVariableAllVC->_entryCount = self->_entryCount;
   
   if( deep ) {
      newVariableAllVC->data = Stg_Class_Copy( self->data, NULL, deep, nameExt, map );
      
      if( (newVariableAllVC->_entryTbl = (VariableAllVC_Entry*)PtrMap_Find( map, self->_entryTbl )) == NULL && self->_entryTbl ) {
         newVariableAllVC->_entryTbl = Memory_Alloc_Array( VariableAllVC_Entry, newVariableAllVC->_entryCount, "VariableAllVC->_entryTbl");
         memcpy( newVariableAllVC->_entryTbl, self->_entryTbl, sizeof(VariableAllVC_Entry) * newVariableAllVC->_entryCount );
         PtrMap_Append( map, newVariableAllVC->_entryTbl, self->_entryTbl );
      }
   }
   else {
      newVariableAllVC->data = self->data;
      newVariableAllVC->_entryTbl = self->_entryTbl;
   }
   
   if( ownMap ) {
      Stg_Class_Delete( map );
   }
   
   return (void*)newVariableAllVC;
}

void _VariableAllVC_Build( void* allElementsVC, void* data ) {
   VariableAllVC* self = (VariableAllVC*)allElementsVC;
   
   _VariableAllVC_BuildSelf( self, data );
   
   _VariableCondition_Build( self, data );
}


/*--------------------------------------------------------------------------------------------------------------------------
** Macros
*/


/*--------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _VariableAllVC_BuildSelf( void* allElementsVC, void* data ) {
   VariableAllVC* self = (VariableAllVC*)allElementsVC;
   
   if( self->data ) {
      Stg_Component_Build( self->data, data, False );
   }
}

IndexSet* _VariableAllVC_GetSet( void* variableCondition ) {
   VariableAllVC* self = (VariableAllVC*)variableCondition;
   Variable*      var = self->variable_Register->_variable[0];
   IndexSet*      set = IndexSet_New( var->arraySize );
   
   IndexSet_AddAll( set );
   
   return set;
}

VariableCondition_VariableIndex _VariableAllVC_GetVariableCount( void* variableCondition, Index globalIndex ) {
   VariableAllVC* self = (VariableAllVC*)variableCondition;
   
   return self->_entryCount;
}

Variable_Index _VariableAllVC_GetVariableIndex(
   void*                           variableCondition, 
   Index                           globalIndex, 
   VariableCondition_VariableIndex varIndex) 
{
   VariableAllVC*   self = (VariableAllVC*)variableCondition;
   
   return Variable_Register_GetIndex(self->variable_Register, self->_entryTbl[varIndex].varName);
}

VariableCondition_ValueIndex _VariableAllVC_GetValueIndex(
   void*                           variableCondition, 
   Index                           globalIndex, 
   VariableCondition_VariableIndex varIndex)
{
   return varIndex;
}

VariableCondition_ValueIndex _VariableAllVC_GetValueCount( void* variableCondition ) {
   VariableAllVC*   self = (VariableAllVC*)variableCondition;
   
   return self->_entryCount;
}

VariableCondition_Value _VariableAllVC_GetValue( void* variableCondition, VariableCondition_ValueIndex valIndex ) {
   VariableAllVC* self = (VariableAllVC*)variableCondition;

   return self->_entryTbl[valIndex].value;
}

void _VariableAllVC_PrintConcise( void* variableCondition, Stream* stream ) {
   VariableAllVC* self = (VariableAllVC*)variableCondition;
   
   Journal_Printf( stream, "\ttype: %s, set: all\n", self->type );
}

/*--------------------------------------------------------------------------------------------------------------------------
** Build functions
*/


/*--------------------------------------------------------------------------------------------------------------------------
** Functions
*/


