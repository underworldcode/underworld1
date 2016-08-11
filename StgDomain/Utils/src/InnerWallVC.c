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
#include "InnerWallVC.h"
#include "RegularMeshUtils.h"

#include <string.h>
#include <assert.h>


const Type InnerWallVC_Type = "InnerWallVC";
const Name defaultInnerWallVCName = "defaultInnerWallVCName";

const char* InnerWallVC_InnerWallEnumToStr[InnerWallVC_InnerWall_Size] = {
	"MinK_back",
	"MinI_left",
	"MinJ_bottom",
	"MaxI_right",
	"MaxJ_top",
	"MaxK_front" };

/*--------------------------------------------------------------------------------------------------------------------------
** Constructor
*/

VariableCondition* InnerWallVC_Factory(
	AbstractContext*					context,
	Variable_Register*				variable_Register, 
	ConditionFunction_Register*	conFunc_Register, 
	Dictionary*							dictionary,
	void*									data )
{
	return (VariableCondition*)InnerWallVC_New( defaultInnerWallVCName, context, NULL, variable_Register, conFunc_Register, dictionary, (Mesh*)data );
}

InnerWallVC* InnerWallVC_New(
	Name									name,
	AbstractContext*					context,
	Name									_dictionaryEntryName, 
	Variable_Register*				variable_Register, 
	ConditionFunction_Register*	conFunc_Register, 
	Dictionary*							dictionary,
	void*									_mesh )
{
	InnerWallVC* self = _InnerWallVC_DefaultNew( name );

	self->isConstructed = True;
	_VariableCondition_Init( (VariableCondition*)self, context, variable_Register, conFunc_Register, dictionary );
	_InnerWallVC_Init( self, _dictionaryEntryName, _mesh );

	return self;
}
InnerWallVC* _InnerWallVC_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                               _sizeOfSelf = sizeof(InnerWallVC);
	Type                                                       type = InnerWallVC_Type;
	Stg_Class_DeleteFunction*                               _delete = _InnerWallVC_Delete;
	Stg_Class_PrintFunction*                                 _print = _InnerWallVC_Print;
	Stg_Class_CopyFunction*                                   _copy = _InnerWallVC_Copy;
	Stg_Component_DefaultConstructorFunction*   _defaultConstructor = (Stg_Component_DefaultConstructorFunction*)_InnerWallVC_DefaultNew;
	Stg_Component_ConstructFunction*                     _construct = _InnerWallVC_AssignFromXML;
	Stg_Component_BuildFunction*                             _build = _InnerWallVC_Build;
	Stg_Component_InitialiseFunction*                   _initialise = _VariableCondition_Initialise;
	Stg_Component_ExecuteFunction*                         _execute = _VariableCondition_Execute;
	Stg_Component_DestroyFunction*                         _destroy = _InnerWallVC_Destroy;
	AllocationType                               nameAllocationType = NON_GLOBAL;
	VariableCondition_BuildSelfFunc*                     _buildSelf = _InnerWallVC_BuildSelf;
	VariableCondition_PrintConciseFunc*               _printConcise = _InnerWallVC_PrintConcise;
	VariableCondition_ReadDictionaryFunc*           _readDictionary = _InnerWallVC_ReadDictionary;
	VariableCondition_GetSetFunc*                           _getSet = _InnerWallVC_GetSet;
	VariableCondition_GetVariableCountFunc*       _getVariableCount = _InnerWallVC_GetVariableCount;
	VariableCondition_GetVariableIndexFunc*       _getVariableIndex = _InnerWallVC_GetVariableIndex;
	VariableCondition_GetValueIndexFunc*             _getValueIndex = _InnerWallVC_GetValueIndex;
	VariableCondition_GetValueCountFunc*             _getValueCount = _InnerWallVC_GetValueCount;
	VariableCondition_GetValueFunc*                       _getValue = _InnerWallVC_GetValue;
	VariableCondition_ApplyFunc*                             _apply = _VariableCondition_Apply;

	return _InnerWallVC_New(  INNERWALLVC_PASSARGS  );
}

InnerWallVC* _InnerWallVC_New(  INNERWALLVC_DEFARGS  ) {
	InnerWallVC* self;
	
	/* Allocate memory/General info */
	assert( _sizeOfSelf >= sizeof(InnerWallVC) );
	self = (InnerWallVC*)_VariableCondition_New(  VARIABLECONDITION_PASSARGS  );
	
	/* Virtual info */
	
	/* Stg_Class info */
	
	return self;
}


void _InnerWallVC_Init(
	void*	innerWallVC, 
	Name	_dictionaryEntryName, 
	void*	_mesh )
{
	InnerWallVC* self = (InnerWallVC*)innerWallVC;

	self->isConstructed = True;
	self->_dictionaryEntryName = _dictionaryEntryName;
	self->_mesh = (Mesh*)_mesh;
	self->_innerWall = InnerWallVC_InnerWall_Size;
	self->_entryTbl = 0;
	self->_entryCount = 0;
}


/*--------------------------------------------------------------------------------------------------------------------------
** General virtual functions
*/

void _InnerWallVC_ReadDictionary( void* variableCondition, void* dictionary ) {
	InnerWallVC*			self = (InnerWallVC*)variableCondition;
	Dictionary_Entry_Value*	vcDictVal;
	Dictionary_Entry_Value	_vcDictVal;
	Dictionary_Entry_Value*	varsVal;
	InnerWallVC_Entry_Index	entry_I;
	
	/* Find dictionary entry */
	if (self->_dictionaryEntryName)
		vcDictVal = Dictionary_Get( dictionary, (Dictionary_Entry_Key)self->_dictionaryEntryName );
	else
	{
		vcDictVal = &_vcDictVal;
		Dictionary_Entry_Value_InitFromStruct(vcDictVal, dictionary);
	}
	
	if (vcDictVal)
	{
		char*	innerWallStr;
		
		/* Obtain which innerWall */
		innerWallStr = Dictionary_Entry_Value_AsString(Dictionary_Entry_Value_GetMember( vcDictVal, (Dictionary_Entry_Key)"innerWall" ) );
		if (     !strcasecmp(innerWallStr, "back")   || !strcasecmp(innerWallStr, "MinK"))
			self->_innerWall = InnerWallVC_InnerWall_MinK;
		else if (!strcasecmp(innerWallStr, "left")   || !strcasecmp(innerWallStr, "MinI"))
			self->_innerWall = InnerWallVC_InnerWall_MinI;
		else if (!strcasecmp(innerWallStr, "bottom") || !strcasecmp(innerWallStr, "MinJ"))
			self->_innerWall = InnerWallVC_InnerWall_MinJ;
		else if (!strcasecmp(innerWallStr, "right")  || !strcasecmp(innerWallStr, "MaxI"))
			self->_innerWall = InnerWallVC_InnerWall_MaxI;
		else if (!strcasecmp(innerWallStr, "top")    || !strcasecmp(innerWallStr, "MaxJ"))
			self->_innerWall = InnerWallVC_InnerWall_MaxJ;
		else if (!strcasecmp(innerWallStr, "front")  || !strcasecmp(innerWallStr, "MaxK"))
			self->_innerWall = InnerWallVC_InnerWall_MaxK;
		else {
			assert( 0 );
			self->_innerWall = InnerWallVC_InnerWall_Size; /* invalid entry */
		}
		
		/* Obtain the variable entries */
		self->_entryCount = Dictionary_Entry_Value_GetCount(Dictionary_Entry_Value_GetMember( vcDictVal, (Dictionary_Entry_Key)"variables") );
		self->_entryTbl = Memory_Alloc_Array( InnerWallVC_Entry, self->_entryCount, "InnerWallVC->_entryTbl" );
		varsVal = Dictionary_Entry_Value_GetMember( vcDictVal, (Dictionary_Entry_Key)"variables");
		
		for (entry_I = 0; entry_I < self->_entryCount; entry_I++ )
		{
			char*			valType;
			Dictionary_Entry_Value*	valueEntry;
			Dictionary_Entry_Value*	varDictListVal;
			
			varDictListVal = Dictionary_Entry_Value_GetElement(varsVal, entry_I);
			valueEntry = Dictionary_Entry_Value_GetMember( varDictListVal, (Dictionary_Entry_Key)"value" );
			
			self->_entryTbl[entry_I].varName = Dictionary_Entry_Value_AsString(
				Dictionary_Entry_Value_GetMember( varDictListVal, (Dictionary_Entry_Key)"name") );
				
			valType = Dictionary_Entry_Value_AsString(Dictionary_Entry_Value_GetMember( varDictListVal, (Dictionary_Entry_Key)"type") );
			if (0 == strcasecmp(valType, "func"))
			{
				char*	funcName = Dictionary_Entry_Value_AsString(valueEntry);
				Index	cfIndex;
				
				self->_entryTbl[entry_I].value.type = VC_ValueType_CFIndex;
				cfIndex = ConditionFunction_Register_GetIndex( self->conFunc_Register, funcName);
				if ( cfIndex == (unsigned)-1 ) {	
					Stream*	errorStr = Journal_Register( Error_Type, (Name)self->type  );

					Journal_Printf( errorStr, "Error- in %s: While parsing "
							"definition of innerWallVC \"%s\" (applies to innerWall \"%s\"), the cond. func. applied to "
							"variable \"%s\" - \"%s\" - wasn't found in the c.f. register.\n",
							__func__, self->_dictionaryEntryName, InnerWallVC_InnerWallEnumToStr[self->_innerWall],
							self->_entryTbl[entry_I].varName, funcName );
					Journal_Printf( errorStr, "(Available functions in the C.F. register are: ");	
					ConditionFunction_Register_PrintNameOfEachFunc( self->conFunc_Register, errorStr );
					Journal_Printf( errorStr, ")\n");	
					assert(0);
				}	
				self->_entryTbl[entry_I].value.as.typeCFIndex = cfIndex;
			}
			else if (0 == strcasecmp(valType, "array"))
			{
				Dictionary_Entry_Value*	valueElement;
				Index			i;

				self->_entryTbl[entry_I].value.type = VC_ValueType_DoubleArray;
				self->_entryTbl[entry_I].value.as.typeArray.size = Dictionary_Entry_Value_GetCount(valueEntry);
				self->_entryTbl[entry_I].value.as.typeArray.array = Memory_Alloc_Array( double,
													self->_entryTbl[entry_I].value.as.typeArray.size, "InnerWallVC->_entryTbl[].value.as.typeArray.array" );
					
				for (i = 0; i < self->_entryTbl[entry_I].value.as.typeArray.size; i++)
				{
					valueElement = Dictionary_Entry_Value_GetElement(valueEntry, i);
					self->_entryTbl[entry_I].value.as.typeArray.array[i] = 
						Dictionary_Entry_Value_AsDouble(valueElement);
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
	else
	{
		self->_innerWall = InnerWallVC_InnerWall_Size;
		self->_entryCount = 0;
		self->_entryTbl = NULL;
	}
}


void _InnerWallVC_Delete( void* innerWallVC ) {
	InnerWallVC* self = (InnerWallVC*)innerWallVC;
	
	/* Stg_Class_Delete parent */
	_VariableCondition_Delete( self );
}

void _InnerWallVC_Destroy( void* innerWallVC, void* data ) {
	InnerWallVC* self = (InnerWallVC*)innerWallVC;
	
	if (self->_entryTbl) Memory_Free(self->_entryTbl);
	
	/* Stg_Class_Delete parent */
	_VariableCondition_Destroy( self, data );
}

void _InnerWallVC_Print(void* innerWallVC, Stream* stream)
{
	InnerWallVC*				self = (InnerWallVC*)innerWallVC;
	InnerWallVC_Entry_Index		entry_I;
	Index				i;
	
	/* Set the Journal for printing informations */
	Stream* info = stream;
	
	/* General info */
	Journal_Printf( info, "InnerWallVC (ptr): %p\n", self);
	
	/* Virtual info */
	
	/* Stg_Class info */
	Journal_Printf( info, "\tdictionary (ptr): %p\n", self->dictionary);
	Journal_Printf( info, "\t_dictionaryEntryName (ptr): %p\n", self->_dictionaryEntryName);
	if (self->_dictionaryEntryName)
		Journal_Printf( info, "\t\t_dictionaryEntryName: %s\n", self->_dictionaryEntryName);
	Journal_Printf( info, "\t_innerWall: %s\n", self->_innerWall == InnerWallVC_InnerWall_MaxK ? "MaxK" :
			self->_innerWall == InnerWallVC_InnerWall_MinK ? "MinK" : self->_innerWall == InnerWallVC_InnerWall_MinI ? "MinI" :
			self->_innerWall == InnerWallVC_InnerWall_MaxI ? "MaxI" : self->_innerWall == InnerWallVC_InnerWall_MaxJ ? "MaxJ" :
			self->_innerWall == InnerWallVC_InnerWall_MinJ ? "MinJ" : "None");
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


void* _InnerWallVC_Copy( void* innerWallVC, void* dest, Bool deep, Name nameExt, struct PtrMap* ptrMap ) {
	InnerWallVC*		self = (InnerWallVC*)innerWallVC;
	InnerWallVC*		newInnerWallVC;
	PtrMap*		map = ptrMap;
	Bool		ownMap = False;
	
	if( !map ) {
		map = PtrMap_New( 10 );
		ownMap = True;
	}
	
	newInnerWallVC = (InnerWallVC*)_VariableCondition_Copy( self, dest, deep, nameExt, map );
	
	newInnerWallVC->_dictionaryEntryName = self->_dictionaryEntryName;
	newInnerWallVC->_innerWall = self->_innerWall;
	newInnerWallVC->_entryCount = self->_entryCount;
	
	if( deep ) {
		newInnerWallVC->_mesh = (Mesh*)Stg_Class_Copy( self->_mesh, NULL, deep, nameExt, map );
		
		if( (newInnerWallVC->_entryTbl = PtrMap_Find( map, self->_entryTbl )) == NULL && self->_entryTbl ) {
			newInnerWallVC->_entryTbl = Memory_Alloc_Array( InnerWallVC_Entry, newInnerWallVC->_entryCount, "InnerWallVC->_entryTbl");
			memcpy( newInnerWallVC->_entryTbl, self->_entryTbl, sizeof(InnerWallVC_Entry) * newInnerWallVC->_entryCount );
			PtrMap_Append( map, newInnerWallVC->_entryTbl, self->_entryTbl );
		}
	}
	else {
		newInnerWallVC->_mesh = self->_mesh;
		newInnerWallVC->_entryTbl = self->_entryTbl;
	}
	
	if( ownMap ) {
		Stg_Class_Delete( map );
	}
	
	return (void*)newInnerWallVC;
}


void _InnerWallVC_Build(  void* innerWallVC, void* data ) {
	InnerWallVC*			self = (InnerWallVC*)innerWallVC;
	
	_InnerWallVC_BuildSelf( self, data );
	
	_VariableCondition_Build( self, data );
}
	

/*--------------------------------------------------------------------------------------------------------------------------
** Macros
*/


/*--------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _InnerWallVC_AssignFromXML( void* innerWallVC, Stg_ComponentFactory* cf, void* data ) { 
}

void _InnerWallVC_BuildSelf(  void* innerWallVC, void* data ) {
	InnerWallVC* self = (InnerWallVC*)innerWallVC;
	
	if( self->_mesh ) {
		Stg_Component_Build( self->_mesh, data, False );
	}
}


IndexSet* _InnerWallVC_GetSet(void* variableCondition) {
	InnerWallVC*	self = (InnerWallVC*)variableCondition;
	IndexSet			*set = NULL;
	Stream*			warningStr = Journal_Register( Error_Type, (Name)self->type );
	unsigned			nDims;
	Grid*				vertGrid;

	nDims = Mesh_GetDimSize( self->_mesh );
	vertGrid = *(Grid** )ExtensionManager_Get( self->_mesh->info, self->_mesh,self->_mesh->vertGridId );
	
	switch (self->_innerWall) {
	case InnerWallVC_InnerWall_MaxK:
		if ( nDims < 3 || !vertGrid->sizes[2]  ) {
			Journal_Printf( warningStr, "Warning - in %s: Can't build a %s wall VC "
					"when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
					InnerWallVC_InnerWallEnumToStr[self->_innerWall], "K" );
			set = IndexSet_New( Mesh_GetDomainSize( self->_mesh, MT_VERTEX ) );
		}
		else {
			set = RegularMeshUtils_CreateGlobalInnerMaxKSet( self->_mesh );
		}
		break;
			
	case InnerWallVC_InnerWall_MinK:
		if ( nDims < 3 || !vertGrid->sizes[2] ) {
			Journal_Printf( warningStr, "Warning - in %s: Can't build a %s wall VC "
					"when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
					InnerWallVC_InnerWallEnumToStr[self->_innerWall], "K" );
			set = IndexSet_New( Mesh_GetDomainSize( self->_mesh, MT_VERTEX ) );
		}
		else {
			set = RegularMeshUtils_CreateGlobalInnerMinKSet( self->_mesh );
		}	
		break;
			
	case InnerWallVC_InnerWall_MaxJ:
		if ( nDims < 2 || !vertGrid->sizes[1] ) {
			Journal_Printf( warningStr, "Warning - in %s: Can't build a %s wall VC "
					"when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
					InnerWallVC_InnerWallEnumToStr[self->_innerWall], "J" );
			set = IndexSet_New( Mesh_GetDomainSize( self->_mesh, MT_VERTEX ) );
		}
		else {
			set = RegularMeshUtils_CreateGlobalInnerMaxJSet(self->_mesh);
		}	
		break;
			
	case InnerWallVC_InnerWall_MinJ:
		if ( nDims < 2 || !vertGrid->sizes[1] ) {
			Journal_Printf( warningStr, "Warning - in %s: Can't build a %s wall VC "
					"when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
					InnerWallVC_InnerWallEnumToStr[self->_innerWall], "J" );
			set = IndexSet_New( Mesh_GetDomainSize( self->_mesh, MT_VERTEX ) );
		}
		else {
			set = RegularMeshUtils_CreateGlobalInnerMinJSet(self->_mesh);
		}	
		break;
			
	case InnerWallVC_InnerWall_MinI:
		if ( nDims < 1 ) {
			Journal_Printf( warningStr, "Warning - in %s: Can't build a %s wall VC "
					"when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
					InnerWallVC_InnerWallEnumToStr[self->_innerWall], "I" );
			set = IndexSet_New( Mesh_GetDomainSize( self->_mesh, MT_VERTEX ) );
		}
		else {
			set = RegularMeshUtils_CreateGlobalInnerMinISet(self->_mesh);
		}	
		break;
			
	case InnerWallVC_InnerWall_MaxI:
		if( nDims < 1 ) {
			Journal_Printf( warningStr, "Warning - in %s: Can't build a %s wall VC "
					"when mesh has no elements in the %s axis. Returning an empty set.\n", __func__,
					InnerWallVC_InnerWallEnumToStr[self->_innerWall], "I" );
			set = IndexSet_New( Mesh_GetDomainSize( self->_mesh, MT_VERTEX ) );
		}
		else {
			set = RegularMeshUtils_CreateGlobalInnerMaxISet(self->_mesh);
		}
		break;
			
	case InnerWallVC_InnerWall_Size:
	default:
		assert(0);
		break;
	}
	
	return set;
}


VariableCondition_VariableIndex _InnerWallVC_GetVariableCount(void* variableCondition, Index globalIndex)
{
	InnerWallVC*	self = (InnerWallVC*)variableCondition;
	
	return self->_entryCount;
}


Variable_Index _InnerWallVC_GetVariableIndex(void* variableCondition, Index globalIndex, VariableCondition_VariableIndex varIndex)
{
	InnerWallVC*		self = (InnerWallVC*)variableCondition;
	Variable_Index	searchedIndex = 0;
	Stream*		errorStr = Journal_Register( Error_Type, (Name)self->type  );
	Name		varName;
	
	varName = self->_entryTbl[varIndex].varName;
	searchedIndex = Variable_Register_GetIndex(self->variable_Register, varName );
	
	Journal_Firewall( ( searchedIndex < self->variable_Register->count ), errorStr, "Error- in %s: searching for index of "
			  "varIndex %u (\"%s\") at global node number %u failed - register returned index %u, greater than "
			  "count %u.\n", __func__, varIndex, varName, globalIndex, searchedIndex, self->variable_Register->count );

	return searchedIndex; 
}


VariableCondition_ValueIndex _InnerWallVC_GetValueIndex(
	void*				variableCondition, 
	Index				globalIndex, 
	VariableCondition_VariableIndex	varIndex)
{
	return varIndex;
}


VariableCondition_ValueIndex _InnerWallVC_GetValueCount(void* variableCondition)
{
	InnerWallVC*	self = (InnerWallVC*)variableCondition;
	
	return self->_entryCount;
}


VariableCondition_Value _InnerWallVC_GetValue(void* variableCondition, VariableCondition_ValueIndex valIndex)
{
	InnerWallVC*	self = (InnerWallVC*)variableCondition;

	return self->_entryTbl[valIndex].value;
}

void _InnerWallVC_PrintConcise( void* variableCondition, Stream* stream ) {
	InnerWallVC*		self = (InnerWallVC*)variableCondition;
	
	Journal_Printf( stream, "\ttype: %s, set: ", self->type );
	Journal_Printf( stream, "%s\n", 
			self->_innerWall == InnerWallVC_InnerWall_MaxK ? "MaxK" :
			self->_innerWall == InnerWallVC_InnerWall_MinK ? "MinK" : 
			self->_innerWall == InnerWallVC_InnerWall_MinI ? "MinI" :
			self->_innerWall == InnerWallVC_InnerWall_MaxI ? "MaxI" : 
			self->_innerWall == InnerWallVC_InnerWall_MaxJ ? "MaxJ" :
			self->_innerWall == InnerWallVC_InnerWall_MinJ ? "MinJ" : "None" );
}

/*--------------------------------------------------------------------------------------------------------------------------
** Build functions
*/


/*--------------------------------------------------------------------------------------------------------------------------
** Functions
*/


