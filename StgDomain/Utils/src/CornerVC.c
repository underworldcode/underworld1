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

#include <mpi.h>
#include <StGermain/StGermain.h>

#include <StgDomain/Geometry/Geometry.h>
#include <StgDomain/Shape/Shape.h>
#include <StgDomain/Mesh/Mesh.h>

#include "types.h"
#include "CornerVC.h"
#include "RegularMeshUtils.h"

#include <string.h>
#include <assert.h>



const Type CornerVC_Type = "CornerVC";
const Name defaultCornerVCName = "defaultCornerVCName";
/* List of corner names */
const char* CornerVC_CornerEnumToStr[CornerVC_Corner_Size] = {
   "MinIMinJMaxK_bottomLeftFront",
   "MaxIMinJMaxK_bottomRightFront",
   "MinIMaxJMaxK_topLeftFront",
   "MaxIMaxJMaxK_topRightFront",
   "MinIMinJMinK_bottomLeftBack",
   "MaxIMinJMinK_bottomRightBack",
   "MinIMaxJMinK_topLeftBack",
   "MaxIMaxJMinK_topRightBack" };

/*--------------------------------------------------------------------------------------------------------------------------
** Constructor
*/

VariableCondition* CornerVC_Factory(
	AbstractContext*					context,
   Variable_Register*				variable_Register, 
   ConditionFunction_Register*	conFunc_Register, 
   Dictionary*							dictionary,
   void*									data )
{
   return (VariableCondition*)CornerVC_New( defaultCornerVCName, context, NULL, variable_Register, conFunc_Register, dictionary, (Mesh*)data );
}

CornerVC* CornerVC_New(
   Name									name,
	AbstractContext*					context,
   Name									_dictionaryEntryName, 
   Variable_Register*				variable_Register, 
   ConditionFunction_Register*	conFunc_Register, 
   Dictionary*							dictionary,
   void*									_mesh )
{
   CornerVC* self = _CornerVC_DefaultNew( name );

	self->isConstructed = True;
	_VariableCondition_Init( self, context, variable_Register, conFunc_Register, dictionary );
	_CornerVC_Init( self, _dictionaryEntryName, _mesh );

	return self;
}

CornerVC* _CornerVC_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                               _sizeOfSelf = sizeof(CornerVC);
	Type                                                       type = CornerVC_Type;
	Stg_Class_DeleteFunction*                               _delete = _CornerVC_Delete;
	Stg_Class_PrintFunction*                                 _print = _CornerVC_Print;
	Stg_Class_CopyFunction*                                   _copy = _CornerVC_Copy;
	Stg_Component_DefaultConstructorFunction*   _defaultConstructor = (Stg_Component_DefaultConstructorFunction*)_CornerVC_DefaultNew;
	Stg_Component_ConstructFunction*                     _construct = _CornerVC_AssignFromXML;
	Stg_Component_BuildFunction*                             _build = _CornerVC_Build;
	Stg_Component_InitialiseFunction*                   _initialise = _VariableCondition_Initialise;
	Stg_Component_ExecuteFunction*                         _execute = _VariableCondition_Execute;
	Stg_Component_DestroyFunction*                         _destroy = _CornerVC_Destroy;
	AllocationType                               nameAllocationType = NON_GLOBAL;
	VariableCondition_BuildSelfFunc*                     _buildSelf = _CornerVC_BuildSelf;
	VariableCondition_PrintConciseFunc*               _printConcise = _CornerVC_PrintConcise;
	VariableCondition_ReadDictionaryFunc*           _readDictionary = _CornerVC_ReadDictionary;
	VariableCondition_GetSetFunc*                           _getSet = _CornerVC_GetSet;
	VariableCondition_GetVariableCountFunc*       _getVariableCount = _CornerVC_GetVariableCount;
	VariableCondition_GetVariableIndexFunc*       _getVariableIndex = _CornerVC_GetVariableIndex;
	VariableCondition_GetValueIndexFunc*             _getValueIndex = _CornerVC_GetValueIndex;
	VariableCondition_GetValueCountFunc*             _getValueCount = _CornerVC_GetValueCount;
	VariableCondition_GetValueFunc*                       _getValue = _CornerVC_GetValue;
	VariableCondition_ApplyFunc*                             _apply = _VariableCondition_Apply;

	return _CornerVC_New(  CORNERVC_PASSARGS  );
}

CornerVC* _CornerVC_New(  CORNERVC_DEFARGS  ) {
   CornerVC* self;
	
   /* Allocate memory/General info */
   assert(_sizeOfSelf >= sizeof(CornerVC));
   self = (CornerVC*)_VariableCondition_New(  VARIABLECONDITION_PASSARGS  );
	
   /* Virtual info */
	
   /* Stg_Class info */
	
   return self;
}


void _CornerVC_Init( void*	cornerVC, Name	_dictionaryEntryName, void* _mesh ) {
   CornerVC* self = (CornerVC*)cornerVC;

   self->_dictionaryEntryName =  _dictionaryEntryName;
   self->_mesh =                 (Mesh*)_mesh;
   self->_corner =               CornerVC_Corner_Size;
   self->_entryTbl =             0;
   self->_entryCount =           0;
}


/*--------------------------------------------------------------------------------------------------------------------------
** General virtual functions
*/

void _CornerVC_ReadDictionary( void* variableCondition, void* dictionary ) {
   CornerVC*                 self = (CornerVC*)variableCondition;
   Dictionary_Entry_Value*   vcDictVal;
   Dictionary_Entry_Value    _vcDictVal;
   Dictionary_Entry_Value*   varsVal;
   CornerVC_Entry_Index      entry_I;
	
   /* Find dictionary entry */
   if (self->_dictionaryEntryName) {
      vcDictVal = Dictionary_Get( dictionary, (Dictionary_Entry_Key)self->_dictionaryEntryName );
   }
   else {
      vcDictVal = &_vcDictVal;
      Dictionary_Entry_Value_InitFromStruct(vcDictVal, dictionary);
   }

   if (vcDictVal) {
		char*	cornerStr;
	
		/* Obtain which corner */
		cornerStr = Dictionary_Entry_Value_AsString(Dictionary_Entry_Value_GetMember( vcDictVal, (Dictionary_Entry_Key)"corner" ) );

		if (!strcasecmp(cornerStr, "bottomLeftFront") || !strcasecmp(cornerStr, "MinIMinJMaxK")) 
			self->_corner = CornerVC_Corner_MinIMinJMaxK;
		else if (!strcasecmp(cornerStr, "bottomRightFront") || !strcasecmp(cornerStr, "MaxIMinJMaxK"))
			self->_corner = CornerVC_Corner_MaxIMinJMaxK;
		else if (!strcasecmp(cornerStr, "topLeftFront") || !strcasecmp(cornerStr, "MinIMaxJMaxK"))
			self->_corner = CornerVC_Corner_MinIMaxJMaxK;
		else if (!strcasecmp(cornerStr, "topRightFront") || !strcasecmp(cornerStr, "MaxIMaxJMaxK"))
			self->_corner = CornerVC_Corner_MaxIMaxJMaxK;
		else if (!strcasecmp(cornerStr, "bottomLeftBack") || !strcasecmp(cornerStr, "MinIMinJMinK"))
			self->_corner = CornerVC_Corner_MinIMinJMinK;
		else if (!strcasecmp(cornerStr, "bottomRightBack") || !strcasecmp(cornerStr, "MaxIMinJMinK"))
			self->_corner = CornerVC_Corner_MaxIMinJMinK;
		else if (!strcasecmp(cornerStr, "topLeftBack") || !strcasecmp(cornerStr, "MinIMaxJMinK"))
			self->_corner = CornerVC_Corner_MinIMaxJMinK;
		else if (!strcasecmp(cornerStr, "topRightBack") || !strcasecmp(cornerStr, "MaxIMaxJMinK"))
			self->_corner = CornerVC_Corner_MaxIMaxJMinK;
      else {
			assert( 0 );
			self->_corner = CornerVC_Corner_Size; /* invalid entry */
      }
		
      /* Obtain the variable entries */
      self->_entryCount = Dictionary_Entry_Value_GetCount(Dictionary_Entry_Value_GetMember( vcDictVal, (Dictionary_Entry_Key)"variables") );
      self->_entryTbl = Memory_Alloc_Array( CornerVC_Entry, self->_entryCount, "CornerVC->_entryTbl" );
      varsVal = Dictionary_Entry_Value_GetMember( vcDictVal, (Dictionary_Entry_Key)"variables");
		
		for (entry_I = 0; entry_I < self->_entryCount; entry_I++ ) {
			char*							valType;
			Dictionary_Entry_Value*	valueEntry;
			Dictionary_Entry_Value*	varDictListVal;
			
			varDictListVal = Dictionary_Entry_Value_GetElement(varsVal, entry_I);
			valueEntry = Dictionary_Entry_Value_GetMember( varDictListVal, (Dictionary_Entry_Key)"value" );
			
			self->_entryTbl[entry_I].varName = Dictionary_Entry_Value_AsString( Dictionary_Entry_Value_GetMember( varDictListVal, (Dictionary_Entry_Key)"name")  );
				
			valType = Dictionary_Entry_Value_AsString(Dictionary_Entry_Value_GetMember( varDictListVal, (Dictionary_Entry_Key)"type") );

			if (0 == strcasecmp(valType, "func")) {
				char*	funcName = Dictionary_Entry_Value_AsString(valueEntry);
				Index	cfIndex;
				
				self->_entryTbl[entry_I].value.type = VC_ValueType_CFIndex;
				cfIndex = ConditionFunction_Register_GetIndex( self->conFunc_Register, funcName);

				if ( cfIndex == (unsigned)-1 ) {	
					Stream*	errorStr = Journal_Register( Error_Type, (Name)self->type  );

					Journal_Printf( errorStr, "Error- in %s: While parsing "
						"definition of cornerVC \"%s\" (applies to corner \"%s\"), the cond. func. applied to "
						"variable \"%s\" - \"%s\" - wasn't found in the c.f. register.\n",
						__func__, self->_dictionaryEntryName, CornerVC_CornerEnumToStr[self->_corner],
					self->_entryTbl[entry_I].varName, funcName );
					Journal_Printf( errorStr, "(Available functions in the C.F. register are: ");	
					ConditionFunction_Register_PrintNameOfEachFunc( self->conFunc_Register, errorStr );
					Journal_Printf( errorStr, ")\n");	
					assert(0);
				}	
				self->_entryTbl[entry_I].value.as.typeCFIndex = cfIndex;
			}
			else if (0 == strcasecmp(valType, "array")) {
				Dictionary_Entry_Value*	valueElement;
				Index							i;

				self->_entryTbl[entry_I].value.type = VC_ValueType_DoubleArray;
				self->_entryTbl[entry_I].value.as.typeArray.size = Dictionary_Entry_Value_GetCount(valueEntry);
				self->_entryTbl[entry_I].value.as.typeArray.array = Memory_Alloc_Array( double,
					self->_entryTbl[entry_I].value.as.typeArray.size, "CornerVC->_entryTbl[].value.as.typeArray.array" );
					
				for (i = 0; i < self->_entryTbl[entry_I].value.as.typeArray.size; i++) {
					valueElement = Dictionary_Entry_Value_GetElement(valueEntry, i);
					self->_entryTbl[entry_I].value.as.typeArray.array[i] = Dictionary_Entry_Value_AsDouble(valueElement);
				}
			}
			else if( 0 == strcasecmp( valType, "double" ) || 0 == strcasecmp( valType, "d" ) ||
				0 == strcasecmp( valType, "float" ) || 0 == strcasecmp( valType, "f" ) )
			{
				self->_entryTbl[entry_I].value.type = VC_ValueType_Double;
				self->_entryTbl[entry_I].value.as.typeDouble = Dictionary_Entry_Value_AsDouble( valueEntry );
			}
			else if( 0 == strcasecmp( valType, "integer" ) || 0 == strcasecmp( valType, "int" ) || 0 == strcasecmp( valType, "i" ) ) {
				self->_entryTbl[entry_I].value.type = VC_ValueType_Int;
				self->_entryTbl[entry_I].value.as.typeInt = Dictionary_Entry_Value_AsUnsignedInt( valueEntry );
			}
			else if( 0 == strcasecmp( valType, "short" ) || 0 == strcasecmp( valType, "s" ) ) {
				self->_entryTbl[entry_I].value.type = VC_ValueType_Short;
				self->_entryTbl[entry_I].value.as.typeShort = Dictionary_Entry_Value_AsUnsignedInt( valueEntry );
			}
			else if( 0 == strcasecmp( valType, "char" ) || 0 == strcasecmp( valType, "c" ) ) {
				self->_entryTbl[entry_I].value.type = VC_ValueType_Char;
				self->_entryTbl[entry_I].value.as.typeChar = Dictionary_Entry_Value_AsUnsignedInt( valueEntry );
			}
			else if( 0 == strcasecmp( valType, "pointer" ) || 0 == strcasecmp( valType, "ptr" ) || 0 == strcasecmp( valType, "p" ) ) {
				self->_entryTbl[entry_I].value.type = VC_ValueType_Ptr;
				self->_entryTbl[entry_I].value.as.typePtr = (void*) ( (ArithPointer)Dictionary_Entry_Value_AsUnsignedInt( valueEntry ));
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
		self->_corner = CornerVC_Corner_Size;
		self->_entryCount = 0;
		self->_entryTbl = NULL;
   }
}


void _CornerVC_Delete(void* cornerVC) {
   CornerVC* self = (CornerVC*)cornerVC;
	
   /* Stg_Class_Delete parent */
   _VariableCondition_Delete(self);
}

void _CornerVC_Destroy(void* cornerVC, void* data) {
   CornerVC* self = (CornerVC*)cornerVC;

   if (self->_entryTbl) Memory_Free(self->_entryTbl);

   _VariableCondition_Destroy(self, data);
}

void _CornerVC_Print(void* cornerVC, Stream* stream) {
   CornerVC*				self = (CornerVC*)cornerVC;
   CornerVC_Entry_Index	entry_I;
   Index						i;
	
   /* Set the Journal for printing informations */
   Stream* info = stream;
	
   /* General info */
   Journal_Printf( info, "CornerVC (ptr): %p\n", self);
	
   /* Virtual info */
	
   /* Stg_Class info */
   Journal_Printf( info, "\tdictionary (ptr): %p\n", self->dictionary);
   Journal_Printf( info, "\t_dictionaryEntryName (ptr): %p\n", self->_dictionaryEntryName);
   if (self->_dictionaryEntryName)
      Journal_Printf( info, "\t\t_dictionaryEntryName: %s\n", self->_dictionaryEntryName);
	
   Journal_Printf( info, "\t_corner: %s\n", self->_corner == CornerVC_Corner_MinIMinJMaxK ? "MinIMinJMaxK" :
		   self->_corner == CornerVC_Corner_MaxIMinJMaxK ? "MaxIMinJMaxK" : self->_corner == CornerVC_Corner_MinIMaxJMaxK ? "MinIMaxJMaxK" :
		   self->_corner == CornerVC_Corner_MaxIMaxJMaxK ? "MaxIMaxJMaxK" : self->_corner == CornerVC_Corner_MinIMinJMinK ? "MinIMinJMinK" :
		   self->_corner == CornerVC_Corner_MaxIMinJMinK ? "MaxIMinJMinK" : self->_corner == CornerVC_Corner_MinIMaxJMinK ? "MinIMaxJMinK" :
		   self->_corner == CornerVC_Corner_MaxIMaxJMinK ? "MaxIMaxJMinK" : "None");
   Journal_Printf( info, "\t_entryCount: %u\n", self->_entryCount);
   Journal_Printf( info, "\t_entryTbl (ptr): %p\n", self->_entryTbl);
   if (self->_entryTbl)
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
   Journal_Printf( info, "\t_mesh (ptr): %p\n", self->_mesh);
	
   /* Print parent */
   _VariableCondition_Print(self);
}


void* _CornerVC_Copy( void* cornerVC, void* dest, Bool deep, Name nameExt, struct PtrMap* ptrMap ) {
   CornerVC*       self = (CornerVC*)cornerVC;
   CornerVC*       newCornerVC;
   PtrMap*         map = ptrMap;
   Bool            ownMap = False;
	
   if( !map ) {
      map = PtrMap_New( 10 );
      ownMap = True;
   }
	
   newCornerVC = (CornerVC*)_VariableCondition_Copy( self, dest, deep, nameExt, map );
	
   newCornerVC->_dictionaryEntryName = self->_dictionaryEntryName;
   newCornerVC->_corner = self->_corner;
   newCornerVC->_entryCount = self->_entryCount;
	
   if( deep ) {
      newCornerVC->_mesh = (Mesh*)Stg_Class_Copy( self->_mesh, NULL, deep, nameExt, map );
		
      if( (newCornerVC->_entryTbl = PtrMap_Find( map, self->_entryTbl )) == NULL && self->_entryTbl ) {
	 newCornerVC->_entryTbl = Memory_Alloc_Array( CornerVC_Entry, newCornerVC->_entryCount, "CornerVC->_entryTbl");
	 memcpy( newCornerVC->_entryTbl, self->_entryTbl, sizeof(CornerVC_Entry) * newCornerVC->_entryCount );
	 PtrMap_Append( map, newCornerVC->_entryTbl, self->_entryTbl );
      }
   }
   else {
      newCornerVC->_mesh = self->_mesh;
      newCornerVC->_entryTbl = self->_entryTbl;
   }
	
   if( ownMap ) {
      Stg_Class_Delete( map );
   }
	
   return (void*)newCornerVC;
}


void _CornerVC_Build(  void* cornerVC, void* data ) {
   CornerVC*         self = (CornerVC*)cornerVC;
	
   _CornerVC_BuildSelf( self, data );
	
   _VariableCondition_Build( self, data );
}
	

/*--------------------------------------------------------------------------------------------------------------------------
** Macros
*/


/*--------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _CornerVC_AssignFromXML( void* cornerVC, Stg_ComponentFactory* cf, void* data ) { 
}

void _CornerVC_BuildSelf(  void* cornerVC, void* data ) {
   CornerVC*        self = (CornerVC*)cornerVC;
	
   if( self->_mesh ) {
      Stg_Component_Build( self->_mesh, data, False );
   }
}

IndexSet* _CornerVC_GetSet(void* variableCondition)
{
   CornerVC*   self = (CornerVC*)variableCondition;
   IndexSet    *set = NULL;
   Stream*     warningStr = Journal_Register( Error_Type, (Name)self->type );
   unsigned	nDims;
   Grid*	vertGrid;

   nDims = Mesh_GetDimSize( self->_mesh );
	vertGrid = *(Grid** )ExtensionManager_Get( self->_mesh->info, self->_mesh, self->_mesh->vertGridId );

   switch (self->_corner) {
      case CornerVC_Corner_MinIMinJMaxK:
	 if ( nDims < 3 || !vertGrid->sizes[2]  ) {
	    Journal_Printf( warningStr, "Warning - in %s: Can't build a %s corner VC "
			    "when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
			    CornerVC_CornerEnumToStr[self->_corner], "K" );
	    set = IndexSet_New( Mesh_GetDomainSize( self->_mesh, MT_VERTEX ) );
	 }
	 else {
	    set = RegularMeshUtils_CreateGlobalMinIMinJMaxKSet(self->_mesh);
	 }
	 break;
			
      case CornerVC_Corner_MaxIMinJMaxK:
	 if ( !vertGrid->sizes[0] ) {
	    Journal_Printf( warningStr, "Warning - in %s: Can't build a %s corner VC "
			    "when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
			    CornerVC_CornerEnumToStr[self->_corner], "I" );
	    set = IndexSet_New( Mesh_GetDomainSize( self->_mesh, MT_VERTEX ) );	
	 }
	 else if ( nDims < 3 || !vertGrid->sizes[2] ) {
	    Journal_Printf( warningStr, "Warning - in %s: Can't build a %s corner VC "
			    "when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
			    CornerVC_CornerEnumToStr[self->_corner], "K" );
	    set = IndexSet_New( Mesh_GetDomainSize( self->_mesh, MT_VERTEX ) );	
	 }
	 else {
	    set = RegularMeshUtils_CreateGlobalMaxIMinJMaxKSet(self->_mesh);
	 }	
	 break;
			
      case CornerVC_Corner_MinIMaxJMaxK:
	 if ( nDims < 2 || !vertGrid->sizes[1] ) {
	    Journal_Printf( warningStr, "Warning - in %s: Can't build a %s corner VC "
			    "when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
			    CornerVC_CornerEnumToStr[self->_corner], "J" );
	    set = IndexSet_New( Mesh_GetDomainSize( self->_mesh, MT_VERTEX ));	
	 }
	 else if ( nDims < 3 || !vertGrid->sizes[2] ) {
	    Journal_Printf( warningStr, "Warning - in %s: Can't build a %s corner VC "
			    "when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
			    CornerVC_CornerEnumToStr[self->_corner], "K" );
	    set = IndexSet_New( Mesh_GetDomainSize( self->_mesh, MT_VERTEX ) );	
	 }
	 else {
	    set = RegularMeshUtils_CreateGlobalMinIMaxJMaxKSet(self->_mesh);
	 }	
	 break;
			
      case CornerVC_Corner_MaxIMaxJMaxK:
	 if ( nDims < 2 || !vertGrid->sizes[1] ) {
	    Journal_Printf( warningStr, "Warning - in %s: Can't build a %s corner VC "
			    "when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
			    CornerVC_CornerEnumToStr[self->_corner], "J" );
	    set = IndexSet_New( Mesh_GetDomainSize( self->_mesh, MT_VERTEX ) );	
	 }
	 else if ( nDims < 3 || !vertGrid->sizes[2] ) {
	    Journal_Printf( warningStr, "Warning - in %s: Can't build a %s corner VC "
			    "when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
			    CornerVC_CornerEnumToStr[self->_corner], "K" );
	    set = IndexSet_New( Mesh_GetDomainSize( self->_mesh, MT_VERTEX ) );	
	 }
	 else if ( !vertGrid->sizes[0] ) {
	    Journal_Printf( warningStr, "Warning - in %s: Can't build a %s corner VC "
			    "when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
			    CornerVC_CornerEnumToStr[self->_corner], "I" );
	    set = IndexSet_New( Mesh_GetDomainSize( self->_mesh, MT_VERTEX ) );	
	 }
	 else {
	    set = RegularMeshUtils_CreateGlobalMaxIMaxJMaxKSet(self->_mesh);
	 }	
	 break;
			
      case CornerVC_Corner_MinIMinJMinK:
	 set = RegularMeshUtils_CreateGlobalMinIMinJMinKSet(self->_mesh);
	 break;
			
      case CornerVC_Corner_MaxIMinJMinK:
	 if ( !vertGrid->sizes[0] ) {
	    Journal_Printf( warningStr, "Warning - in %s: Can't build a %s corner VC "
			    "when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
			    CornerVC_CornerEnumToStr[self->_corner], "I" );
	    set = IndexSet_New( Mesh_GetDomainSize( self->_mesh, MT_VERTEX ) );	
	 }
	 else {
	    set = RegularMeshUtils_CreateGlobalMaxIMinJMinKSet(self->_mesh);
	 }
	 break;
			
      case CornerVC_Corner_MinIMaxJMinK:
	 if ( nDims < 2 || !vertGrid->sizes[1] ) {
	    Journal_Printf( warningStr, "Warning - in %s: Can't build a %s corner VC "
			    "when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
			    CornerVC_CornerEnumToStr[self->_corner], "J" );
	    set = IndexSet_New( Mesh_GetDomainSize( self->_mesh, MT_VERTEX ) );	
	 }
	 else {
	    set = RegularMeshUtils_CreateGlobalMinIMaxJMinKSet(self->_mesh);
	 }	
	 break;
			
      case CornerVC_Corner_MaxIMaxJMinK:
	 if ( !vertGrid->sizes[0] ) {
	    Journal_Printf( warningStr, "Warning - in %s: Can't build a %s corner VC "
			    "when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
			    CornerVC_CornerEnumToStr[self->_corner], "I" );
	    set = IndexSet_New(Mesh_GetDomainSize( self->_mesh, MT_VERTEX ) );	
	 }
	 else if ( nDims < 2 || !vertGrid->sizes[1] ) {
	    Journal_Printf( warningStr, "Warning - in %s: Can't build a %s corner VC "
			    "when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
			    CornerVC_CornerEnumToStr[self->_corner], "J" );
	    set = IndexSet_New(Mesh_GetDomainSize( self->_mesh, MT_VERTEX ));
	 }
	 else {
	    set = RegularMeshUtils_CreateGlobalMaxIMaxJMinKSet(self->_mesh);
	 }
	 break;
		
      case CornerVC_Corner_Size:
      default:
	 assert(0);
	 break;
   }

   return set;
}


VariableCondition_VariableIndex _CornerVC_GetVariableCount(void* variableCondition, Index globalIndex)
{
   CornerVC*   self = (CornerVC*)variableCondition;
	
   return self->_entryCount;
}


Variable_Index _CornerVC_GetVariableIndex(void* variableCondition, Index globalIndex, VariableCondition_VariableIndex varIndex)
{
   CornerVC*       self = (CornerVC*)variableCondition;
   Variable_Index  searchedIndex = 0;
   Stream*         errorStr = Journal_Register( Error_Type, (Name)self->type  );
   Name            varName;
	
   varName = self->_entryTbl[varIndex].varName;
   searchedIndex = Variable_Register_GetIndex(self->variable_Register, varName );
	
   Journal_Firewall( ( searchedIndex < self->variable_Register->count ), errorStr, "Error- in %s: searching for index of "
		     "varIndex %u (\"%s\") at global node number %u failed - register returned index %u, greater than "
		     "count %u.\n", __func__, varIndex, varName, globalIndex, searchedIndex, self->variable_Register->count );

   return searchedIndex; 
}


VariableCondition_ValueIndex _CornerVC_GetValueIndex(
   void*				variableCondition, 
   Index				globalIndex, 
   VariableCondition_VariableIndex	varIndex)
{
   return varIndex;
}


VariableCondition_ValueIndex _CornerVC_GetValueCount(void* variableCondition)
{
   CornerVC*	self = (CornerVC*)variableCondition;
	
   return self->_entryCount;
}


VariableCondition_Value _CornerVC_GetValue(void* variableCondition, VariableCondition_ValueIndex valIndex)
{
   CornerVC*	self = (CornerVC*)variableCondition;

   return self->_entryTbl[valIndex].value;
}

void _CornerVC_PrintConcise( void* variableCondition, Stream* stream ) {
   CornerVC*	self = (CornerVC*)variableCondition;
	
   Journal_Printf( stream, "\ttype: %s, set: ", self->type );
   Journal_Printf( stream, "%s\n", 
		   self->_corner == CornerVC_Corner_MinIMinJMaxK ? "MinIMinJMaxK" :
		   self->_corner == CornerVC_Corner_MaxIMinJMaxK ? "MaxIMinJMaxK" : 
		   self->_corner == CornerVC_Corner_MinIMaxJMaxK ? "MinIMaxJMaxK" :
		   self->_corner == CornerVC_Corner_MaxIMaxJMaxK ? "MaxIMaxJMaxK" : 
		   self->_corner == CornerVC_Corner_MinIMinJMinK ? "MinIMinJMinK" :
		   self->_corner == CornerVC_Corner_MaxIMinJMinK ? "MaxIMinJMinK" : 
		   self->_corner == CornerVC_Corner_MinIMaxJMinK ? "MinIMaxJMinK" :
		   self->_corner == CornerVC_Corner_MaxIMaxJMinK ? "MaxIMaxJMinK" : "None" );
}

/*--------------------------------------------------------------------------------------------------------------------------
** Build functions
*/


/*--------------------------------------------------------------------------------------------------------------------------
** Functions
*/


