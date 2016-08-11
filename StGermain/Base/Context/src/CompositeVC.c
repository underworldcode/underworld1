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

#include <stdarg.h>
#include "Base/Foundation/Foundation.h"
#include "Base/IO/IO.h"
#include "Base/Container/Container.h"
#include "Base/Automation/Automation.h"
#include "Base/Extensibility/Extensibility.h"

#include "types.h"
#include "Variable.h"
#include "Variable_Register.h"
#include "VariableCondition.h"
#include "VariableCondition_Register.h"
#include "ConditionFunction.h"
#include "ConditionFunction_Register.h"
#include "ContextEntryPoint.h"
#include "AbstractContext.h"
#include "CompositeVC.h"

#include <string.h>
#include <assert.h>


const Type CompositeVC_Type = "CompositeVC";
const Name defaultCompositeVCName = "defaultCompositeVCName";

/*--------------------------------------------------------------------------------------------------------------------------
** Constructor
*/

VariableCondition* CompositeVC_Factory(
	AbstractContext*					context,
	Variable_Register*				variable_Register, 
	ConditionFunction_Register*	conFunc_Register,
	Dictionary*							dictionary,
	void*									data )
{
	return (VariableCondition*)CompositeVC_New( defaultCompositeVCName, context, variable_Register, conFunc_Register, dictionary, data );
}

CompositeVC* CompositeVC_New(
	Name									name,
	AbstractContext*					context,
	Variable_Register*				variable_Register, 
	ConditionFunction_Register*	conFunc_Register,
	Dictionary*							dictionary,
	void*									data )
{
	CompositeVC* self = _CompositeVC_DefaultNew( name );

	self->isConstructed = True;
	_VariableCondition_Init( self, context, variable_Register, conFunc_Register, dictionary );
	_CompositeVC_Init( self, data );

	return self;
}

CompositeVC* _CompositeVC_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(CompositeVC);
	Type                                                      type = CompositeVC_Type;
	Stg_Class_DeleteFunction*                              _delete = _CompositeVC_Delete;
	Stg_Class_PrintFunction*                                _print = _CompositeVC_Print;
	Stg_Class_CopyFunction*                                  _copy = _CompositeVC_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = (Stg_Component_DefaultConstructorFunction*)_CompositeVC_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _CompositeVC_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _CompositeVC_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _VariableCondition_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _VariableCondition_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _CompositeVC_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
	VariableCondition_BuildSelfFunc*                    _buildSelf = NULL;
	VariableCondition_PrintConciseFunc*              _printConcise = _CompositeVC_PrintConcise;
	VariableCondition_ReadDictionaryFunc*          _readDictionary = _CompositeVC_ReadDictionary;
	VariableCondition_GetSetFunc*                          _getSet = _CompositeVC_GetSet;
	VariableCondition_GetVariableCountFunc*      _getVariableCount = _CompositeVC_GetVariableCount;
	VariableCondition_GetVariableIndexFunc*      _getVariableIndex = _CompositeVC_GetVariableIndex;
	VariableCondition_GetValueIndexFunc*            _getValueIndex = _CompositeVC_GetValueIndex;
	VariableCondition_GetValueCountFunc*            _getValueCount = _CompositeVC_GetValueCount;
	VariableCondition_GetValueFunc*                      _getValue = _CompositeVC_GetValue;
	VariableCondition_ApplyFunc*                            _apply = _CompositeVC_Apply;

	return (CompositeVC*)_CompositeVC_New(  COMPOSITEVC_PASSARGS  );
}

CompositeVC* _CompositeVC_New(  COMPOSITEVC_DEFARGS  ) {
	CompositeVC* self;
	
	/* Allocate memory/General info */
	assert (_sizeOfSelf >= sizeof(CompositeVC) );
	self = (CompositeVC*)_VariableCondition_New(  VARIABLECONDITION_PASSARGS  );
	
	/* Virtual info */
	
	/* Stg_Class info */
	
	return self;
}


void _CompositeVC_Init( void* compositeVC, void* data ) {
	CompositeVC*	self = (CompositeVC*)compositeVC;
	
	self->isConstructed = True;
	self->itemCount = 0;
	self->nIndepItems = 0;
	self->indepItems = NULL;
	self->_size = 8;
	self->_delta = 8;
	self->itemTbl = Memory_Alloc_Array( VariableCondition*, self->_size, "CompositeCV->itemTbl" );
	memset(self->itemTbl, 0 , sizeof(VariableCondition*)*self->_size);
	self->iOwnTbl = Memory_Alloc_Array( Bool, self->_size, "CompositeCV->iOwnTbl" );
	memset(self->iOwnTbl, 0, sizeof(Bool)*self->_size);
	self->data = data;
	self->attachedSets = 0;
	self->hasReadDictionary = False;
}


/*--------------------------------------------------------------------------------------------------------------------------
** General virtual functions
*/

void _CompositeVC_ReadDictionary( void* compositeVC, void* dictionary ) {
	CompositeVC*	self = (CompositeVC*)compositeVC;

	/* View the dictionary as a list of variable conditions */
	if( dictionary && !self->hasReadDictionary ) {
		Dictionary_Entry_Value*	vcList;
		
		vcList = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"independentVCList" );

		if( vcList ) {
			Index	count;
			Index	entry_I;
			
			count = Dictionary_Entry_Value_GetCount(vcList);

			for (entry_I = 0; entry_I < count; entry_I++ ) {
				Dictionary_Entry_Value*	vcEntry;
				Type			type;
				Dictionary*		dictionary;
				VariableCondition*	vc;
				
				vcEntry = Dictionary_Entry_Value_GetElement(vcList, entry_I);
				type = Dictionary_Entry_Value_AsString(Dictionary_Entry_Value_GetMember( vcEntry, (Dictionary_Entry_Key)"type"));
				dictionary = Dictionary_Entry_Value_AsDictionary(vcEntry );
				vc = VariableCondition_Register_CreateNew( self->context, variableCondition_Register, self->variable_Register, 
					self->conFunc_Register, type, dictionary, self->data );
				vc->cf = self->cf;
				vc->_readDictionary( vc, dictionary );

				self->nIndepItems++;
				self->indepItems = (VariableCondition**)ReallocArray( self->indepItems, VariableCondition*, self->nIndepItems );
				self->indepItems[self->nIndepItems - 1] = vc;

				/* Don't add so we can modify the matrix later.
				CompositeVC_Add(self, vc, True);
				*/
			}
		}
		vcList = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"vcList" );

		if( vcList ) {
			Index	count;
			Index	entry_I;
			
			count = Dictionary_Entry_Value_GetCount(vcList);

			for (entry_I = 0; entry_I < count; entry_I++ ) {
				Dictionary_Entry_Value*	vcEntry;
				Type			type;
				Dictionary*		dictionary;
				VariableCondition*	vc;
				
				vcEntry = Dictionary_Entry_Value_GetElement(vcList, entry_I);
				type = Dictionary_Entry_Value_AsString(Dictionary_Entry_Value_GetMember( vcEntry, (Dictionary_Entry_Key)"type"));
				dictionary = Dictionary_Entry_Value_AsDictionary(vcEntry );
				vc = VariableCondition_Register_CreateNew( self->context, variableCondition_Register, self->variable_Register, 
					self->conFunc_Register, type, dictionary, self->data );
				vc->_readDictionary( vc, dictionary );
				CompositeVC_Add(self, vc, True);
			}
		}
		self->hasReadDictionary = True;
	}
}

void _CompositeVC_AssignFromXML( void* compositeVC, Stg_ComponentFactory* cf, void* data ) {
	CompositeVC*		self = (CompositeVC*)compositeVC;
	void*       		variableRegister = NULL;
	void*       		conditionFunctionRegister = NULL;
	void*       		initData = NULL;
	Dictionary* 		vcDict = NULL;
	Name       			vcName;
	AbstractContext*	context;
	
	/* Need to store this so we can get at components
		later on when using the fucked up 'ReadDictionary' function. */
	self->cf = cf;

	context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", AbstractContext, False, data );
	if( !context  )
		context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", AbstractContext, True, data );
	
	variableRegister = context->variable_Register;
	assert( variableRegister );
	conditionFunctionRegister = condFunc_Register; 
	assert( conditionFunctionRegister  );
	
	vcName = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"vcName", self->name );

	if ( cf->rootDict  )
		vcDict = Dictionary_GetDictionary( cf->rootDict, vcName );

	initData = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Data", Stg_Component, False, data  );
	
	_VariableCondition_Init( self, context, (Variable_Register*)variableRegister, (ConditionFunction_Register*)conditionFunctionRegister, vcDict );
	_CompositeVC_Init( self, initData );
}

void _CompositeVC_Delete(void* compositeVC) {
	CompositeVC* self = (CompositeVC*)compositeVC;
	
	/* Stg_Class_Delete parent */
	_VariableCondition_Delete(self);
}

void _CompositeVC_Destroy(void* compositeVC, void* data) {
	CompositeVC*	self = (CompositeVC*)compositeVC;
	int				ii;

	if( self->indepItems ) {
		for( ii = 0; ii < self->nIndepItems; ii++ )
         _VariableCondition_Delete( self->indepItems[ii]);
		Memory_Free( self->indepItems );
	}
	
	if (self->itemTbl) {
		VariableCondition_Index	entry_I;
		
		for (entry_I = 0; entry_I < self->itemCount; entry_I++) {
			if (self->iOwnTbl[entry_I] && self->itemTbl[entry_I])
				//Stg_Component_Destroy( self->itemTbl[entry_I], NULL, False );
				_VariableCondition_Delete( self->itemTbl[entry_I]);
		}
		Memory_Free(self->itemTbl);
	}
	
	if( self->attachedSets ) {
		Memory_Free( self->attachedSets );
	}

   Stg_Component_Destroy( self->data, NULL, False );

	_VariableCondition_Destroy( self, data );
}

void _CompositeVC_Print(void* compositeVC, Stream* stream) {
	CompositeVC*	self = (CompositeVC*)compositeVC;
	Index				item_I;
	
	/* Set the Journal for printing informations */
	Stream* info = stream;
	
	/* General info */
	Journal_Printf( info, "CompositeVC (ptr): %p\n", self);
	
	/* Virtual info */
	
	/* Stg_Class info */
	Journal_Printf( info, "\tdictionary (ptr): %p\n", self->dictionary);
	Journal_Printf( info, "\titemCount: %u\n", self->itemCount );
	Journal_Printf( info, "\titemTbl (ptr): %p\n", self->itemTbl);
	if (self->itemTbl)
	{
		for (item_I = 0; item_I < self->itemCount; item_I++)
			Journal_Printf( info, "\t\titemTbl[%u] (ptr): %p\n", item_I, self->itemTbl[item_I]);
	}
	Journal_Printf( info, "\tiOwnTbl (ptr): %p\n", self->iOwnTbl);
	if (self->iOwnTbl)
	{
		for (item_I = 0; item_I < self->itemCount; item_I++)
			Journal_Printf( info, "\t\tiOwnTbl[%u]: %s\n", item_I, self->iOwnTbl[item_I] ? "True" : "False");
	}
	Journal_Printf( info, "\t_size: %lu\n", self->_size);
	Journal_Printf( info, "\t_delta: %lu\n", self->_delta);
	
	/* Print parent */
	_VariableCondition_Print(self);
}


void* _CompositeVC_Copy( void* compositeVC, void* dest, Bool deep, Name nameExt, struct PtrMap* ptrMap ) {
	CompositeVC*	self = (CompositeVC*)compositeVC;
	CompositeVC*	newCompositeVC;
	PtrMap*		map = ptrMap;
	Bool		ownMap = False;
	
	if( !map ) {
		map = PtrMap_New( 10 );
		ownMap = True;
	}
	
	newCompositeVC = (CompositeVC*)_VariableCondition_Copy( self, dest, deep, nameExt, map );
	
	newCompositeVC->itemCount = self->itemCount;
	newCompositeVC->_size = self->_size;
	newCompositeVC->_delta = self->_delta;
	newCompositeVC->hasReadDictionary = self->hasReadDictionary;
	newCompositeVC->data = self->data;
	
	if( deep ) {
		if( (newCompositeVC->itemTbl = (VariableCondition**)PtrMap_Find( map, self->itemTbl )) == NULL && self->itemTbl ) {
			Index	item_I;
			
			newCompositeVC->itemTbl = Memory_Alloc_Array( VariableCondition*, newCompositeVC->_size, "CompositeCV->itemTbl" );
			memset( newCompositeVC->itemTbl, 0, sizeof(VariableCondition*) * newCompositeVC->_size );
			for( item_I = 0; item_I < self->itemCount; item_I++ ) {
				newCompositeVC->itemTbl[item_I] = (VariableCondition*)Stg_Class_Copy( self->itemTbl[item_I], NULL, deep, nameExt, map );
			}
			PtrMap_Append( map, newCompositeVC->itemTbl, self->itemTbl );
		}
		
		if( (newCompositeVC->iOwnTbl = (Bool*)PtrMap_Find( map, self->iOwnTbl )) == NULL && self->iOwnTbl ) {
			newCompositeVC->iOwnTbl = Memory_Alloc_Array( Bool, newCompositeVC->_size, "CompositeCV->iOwnTbl" );
			memcpy( newCompositeVC->iOwnTbl, self->iOwnTbl, sizeof(Bool) * newCompositeVC->_size );
			PtrMap_Append( map, newCompositeVC->iOwnTbl, self->iOwnTbl );
		}
		
		if( (newCompositeVC->attachedSets = (IndexSet**)PtrMap_Find( map, self->attachedSets )) == NULL && self->attachedSets ) {
			Index	item_I;
			
			self->attachedSets = Memory_Alloc_Array( IndexSet*, newCompositeVC->itemCount, "CompositeCV->attachedSets" );
			for( item_I = 0; item_I < self->itemCount; item_I++ ) {
				newCompositeVC->attachedSets[item_I] = (IndexSet*)Stg_Class_Copy( self->attachedSets[item_I], NULL, deep, nameExt, map );
			}
			PtrMap_Append( map, newCompositeVC->attachedSets, self->attachedSets );
		}
	}
	else {
		newCompositeVC->itemTbl = self->itemTbl;
		newCompositeVC->iOwnTbl = self->iOwnTbl;
		newCompositeVC->attachedSets = self->attachedSets;
	}
	
	if( ownMap ) {
		Stg_Class_Delete( map );
	}
	
	return (void*)newCompositeVC;
}


/*--------------------------------------------------------------------------------------------------------------------------
** Macros
*/


/*--------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _CompositeVC_Build( void* compositeVC, void* data ) {
	CompositeVC*	self = (CompositeVC*)compositeVC;
	Index		index;
        int ii;
	
	/* Read the dictionary... we have to do this early to get self->itemCount filled in. Hence we keep a flag to ensure we
		dont read twice */
	self->_readDictionary( self, self->dictionary );
	
	/* Build a temporary array to store the sets of attached VCs for the build process and clear it afterward */
	if( self->itemCount ) {
		Index			item_I;
		
		self->attachedSets = Memory_Alloc_Array( IndexSet*, self->itemCount, "CompositeCV->attachedSets" );
		memset( self->attachedSets, 0, sizeof(IndexSet*) * self->itemCount );
		
		/* This guy is here so that a VC that was created by the composite VC can do any
		   build stuff it needs, without calling _VariableCondition_Build for itself. */
		for( item_I = 0; item_I < self->itemCount; item_I++ ) {
			VariableCondition_BuildSelf( self->itemTbl[item_I], data );
		}
		
		_VariableCondition_Build( self, data );
		for( index = 0; index < self->itemCount; index++ ) {
			Stg_Class_Delete( self->attachedSets[index] );
		}
		Memory_Free( self->attachedSets );
		self->attachedSets = 0;
	}

        for( ii = 0; ii < self->nIndepItems; ii++ )
           Stg_Component_Build( self->indepItems[ii], data, False );
}


IndexSet* _CompositeVC_GetSet( void* compositeVC ) {
	CompositeVC*	self = (CompositeVC*)compositeVC;
	IndexSet*	set = NULL;
	Index		i;
	
	/* Use the first attachment to obtain the size, etc. Be effecient by duplicating if for the my own set */
	if( self->itemCount ) {
		self->attachedSets[0] = self->itemTbl[0]->_getSet( self->itemTbl[0] );
		set = IndexSet_Duplicate( self->attachedSets[0] );
	}
	
	/* For the remainder of the attachments, OR in their sets */
	for( i = 1; i < self->itemCount; i++ ) {
		self->attachedSets[i] = self->itemTbl[i]->_getSet( self->itemTbl[i] );
		IndexSet_Merge_OR( set, self->attachedSets[i] );
	}
	
	return set;
}

	
Variable_Index _CompositeVC_GetVariableCount( void* compositeVC, Index globalIndex ) {
	CompositeVC*	self = (CompositeVC*)compositeVC;
	Variable_Index*	varIndices;
	Variable_Index	varIndexCount = 0;
	Index		i;
	
	varIndices = Memory_Alloc_Array( Variable_Index, self->variable_Register->count, "varIndices" );
		
	for( i = 0; i < self->itemCount; i++ ) {
		if( IndexSet_IsMember( self->attachedSets[i], globalIndex ) ) {
			Index	varCount = self->itemTbl[i]->_getVariableCount( self->itemTbl[i], globalIndex );
			Index	j;
		
			for(j = 0; j < varCount; j++ ) {
				Index	newVarIndex = self->itemTbl[i]->_getVariableIndex(self->itemTbl[i], globalIndex, j);
				Index	k;
			
				for (k = 0; k < varIndexCount; k++)
					if (varIndices[k] == newVarIndex)
						break;
				if (k < varIndexCount)
					continue;
				
				varIndices[varIndexCount++] = newVarIndex;
			}
		}
	}
	
	if (varIndices) Memory_Free(varIndices);
	
	return varIndexCount;
}

	
Variable_Index _CompositeVC_GetVariableIndex(
		void*				compositeVC, 
		Index				globalIndex, 
		VariableCondition_VariableIndex	varIndex)
{
	CompositeVC*	self = (CompositeVC*)compositeVC;
	Variable_Index*	varIndices;
	Variable_Index	varIndexCount = 0;
	Index		i;
	
	varIndices = Memory_Alloc_Array( Variable_Index, self->variable_Register->count, "varIndices" );
		
	for( i = 0; i < self->itemCount; i++ ) {
		if( IndexSet_IsMember( self->attachedSets[i], globalIndex ) ) {
			Index	varCount = self->itemTbl[i]->_getVariableCount(self->itemTbl[i], globalIndex);
			Index	j;
		
			for (j = 0; j < varCount; j++)
			{
				Index	newVarIndex = self->itemTbl[i]->_getVariableIndex(self->itemTbl[i], globalIndex, j);
				Index	k;
			
				for (k = 0; k < varIndexCount; k++)
					if (varIndices[k] == newVarIndex)
						break;
				if (k < varIndexCount)
					continue;
			
				if (varIndexCount == varIndex)
				{
					if (varIndices) Memory_Free(varIndices);
					return newVarIndex;
				}
			
				varIndices[varIndexCount++] = newVarIndex;
			}
		}
	}
	
	if (varIndices) Memory_Free(varIndices);
	
	return (Variable_Index)-1;
}


VariableCondition_ValueIndex _CompositeVC_GetValueIndex(
				void*				compositeVC, 
				Index				globalIndex, 
				VariableCondition_VariableIndex	varIndex)
{
	CompositeVC*			self = (CompositeVC*)compositeVC;
	VariableCondition_ValueIndex	valIndex = (VariableCondition_ValueIndex)-1;
	CompositeVC_ItemIndex		valItem = 0;
	Variable_Index			varRegIndex = self->_getVariableIndex(self, globalIndex, varIndex);
	Index				i;
	
	for( i = self->itemCount; i > 0; i-- ) {
		if( IndexSet_IsMember( self->attachedSets[i-1], globalIndex ) ) {
			Index	j;
			
			for (j = 0; j < self->itemTbl[i - 1]->_getVariableCount(self->itemTbl[i - 1], globalIndex); j++)
				if (self->itemTbl[i - 1]->_getVariableIndex(self->itemTbl[i - 1], globalIndex, j) == 
					varRegIndex)
					break;
			if (j == self->itemTbl[i - 1]->_getVariableCount(self->itemTbl[i - 1], globalIndex))
				continue;
			
			valItem = i - 1;
			valIndex = self->itemTbl[valItem]->_getValueIndex(self->itemTbl[valItem], globalIndex, j);
			break;
		}
	}
	
	for (i = 0; i < valItem; i++)
		valIndex += self->itemTbl[i]->_getValueCount(self->itemTbl[i]);
	
	return valIndex;
}

	
VariableCondition_ValueIndex _CompositeVC_GetValueCount(void* compositeVC)
{
	CompositeVC*			self = (CompositeVC*)compositeVC;
	VariableCondition_ValueIndex	valCount = 0;
	Index				i;
	
	for (i = 0; i < self->itemCount; i++)
		valCount += self->itemTbl[i]->_getValueCount(self->itemTbl[i]);
		
	return valCount;
}

	
VariableCondition_Value _CompositeVC_GetValue(void* compositeVC, VariableCondition_ValueIndex valIndex)
{
	CompositeVC*			self = (CompositeVC*)compositeVC;
	VariableCondition_ValueIndex	valPos = valIndex;
	Index				i = 0;
	
	while (valPos >= self->itemTbl[i]->_getValueCount(self->itemTbl[i]))
	{
		valPos -= self->itemTbl[i]->_getValueCount(self->itemTbl[i]);
		i++;
/*		if (++i >= self->itemCount)	*/
			/* Then what? */
	}
	
	return self->itemTbl[i]->_getValue(self->itemTbl[i], valPos);
}


void _CompositeVC_PrintConcise( void* compositeVC, Stream* stream ) {
	CompositeVC*	self = (CompositeVC*)compositeVC;
	Index		item_I;
	
	Journal_Printf( stream, "\ttype: %s, {\n", self->type );
	if( self->itemTbl ) {
		for( item_I = 0; item_I < self->itemCount; item_I++ ) {
			VariableCondition_PrintConcise( self->itemTbl[item_I], stream );
		}
	}
	Journal_Printf( stream, "}\n" );
}

/*--------------------------------------------------------------------------------------------------------------------------
** Build functions
*/

CompositeVC_ItemIndex CompositeVC_Add(void* compositeVC, void* variableCondition, Bool iOwn)
{
	CompositeVC*		self = (CompositeVC*)compositeVC;
	CompositeVC_ItemIndex	handle;
	
	if (self->itemCount >= self->_size)
	{
		SizeT currentSize = self->_size;
		
		self->_size += self->_delta;

		self->itemTbl = Memory_Realloc_Array( self->itemTbl, VariableCondition*, self->_size );
		memset( (Pointer)((ArithPointer)self->itemTbl + (sizeof(VariableCondition*) * currentSize) ),
			0, sizeof(VariableCondition*) * (self->_size - currentSize) );

		self->iOwnTbl = Memory_Realloc_Array( self->iOwnTbl, Bool, self->_size );
		memset( (Pointer)((ArithPointer)self->iOwnTbl + (sizeof(Bool) * currentSize )),
			0, sizeof(Bool) * (self->_size - currentSize ) );
	}
	
	handle = self->itemCount++;
	self->itemTbl[handle] = (VariableCondition*) variableCondition;
	self->iOwnTbl[handle] = iOwn;
	
	return handle;
}


/*--------------------------------------------------------------------------------------------------------------------------
** Functions
*/

void _CompositeVC_Apply( void* _self, void* _ctx ) {
   CompositeVC* self = (CompositeVC*)_self;
   int ii;

   _VariableCondition_Apply( self, _ctx );

   for( ii = 0; ii < self->nIndepItems; ii++ )
      VariableCondition_Apply( self->indepItems[ii], _ctx );
}


