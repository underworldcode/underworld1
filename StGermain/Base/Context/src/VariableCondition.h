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
*/
/** \file
**  Role:
**	Abstract class for specifying (1 or more) conditions that apply to (1 or more) variables in an array.
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StGermain_Base_Context_VariableCondition_h__
#define __StGermain_Base_Context_VariableCondition_h__
	
	/** Textual name of this class */
	extern const Type VariableCondition_Type;
	
	typedef void (VariableCondition_BuildSelfFunc) ( void* variableCondition, void* data );
	typedef void (VariableCondition_PrintConciseFunc) ( void* variableCondition, Stream* stream );
	typedef void (VariableCondition_ReadDictionaryFunc) ( void* variableCondition, void* dictionary );
	typedef IndexSet* (VariableCondition_GetSetFunc) ( void* variableCondition );
	typedef VariableCondition_VariableIndex (VariableCondition_GetVariableCountFunc)	( void* variableCondition, Index index );
	typedef Variable_Index (VariableCondition_GetVariableIndexFunc) ( void* variableCondition, Index index, VariableCondition_VariableIndex condVar_I );
	typedef VariableCondition_ValueIndex (VariableCondition_GetValueIndexFunc) ( void* variableCondition, Index index, VariableCondition_VariableIndex condVar_I );
	typedef VariableCondition_ValueIndex (VariableCondition_GetValueCountFunc) ( void* variableCondition );
	typedef VariableCondition_Value (VariableCondition_GetValueFunc) ( void* variableCondition, VariableCondition_ValueIndex valIndex );
	typedef void (VariableCondition_ApplyFunc)( void *vc, void* ctx );
	
	typedef struct {
		Index		size;
		double*	array;
	} VariableCondition_Value_Array;
	
	typedef union { 
		double	typeDouble;
		int		typeInt;
		short		typeShort;
		char		typeChar;
		void*		typePtr;
		VariableCondition_Value_Array typeArray;
		ConditionFunction_Index typeCFIndex;
	} VariableCondition_Value_AsType;
	
	#define __VariableCondition_Value \
		VariableCondition_ValueType	type; \
		VariableCondition_Value_AsType	as;
	struct _VariableCondition_Value { __VariableCondition_Value };
	
	
	#define __VariableCondition_Tuple \
		/** The variable index into VariableCondition::variable_Register identifying a variable */ \
		Variable_Index				varIndex; \
		/** The value index into VariableCondition::valueTbl identifying a value to apply to the variable */ \
		VariableCondition_ValueIndex		valIndex; \
	
	/** Used to relate which Condition is applied to a Variable */
	struct _VariableCondition_Tuple { __VariableCondition_Tuple };
	
	#define __VariableCondition \
		/* General info */ \
		__Stg_Component \
		\
		AbstractContext*									context; \
		/* Virtual info */ \
		VariableCondition_BuildSelfFunc*				_buildSelf; \
		VariableCondition_PrintConciseFunc*			_printConcise; \
		VariableCondition_ReadDictionaryFunc*		_readDictionary; \
		VariableCondition_GetSetFunc*					_getSet; \
		VariableCondition_GetVariableCountFunc*	_getVariableCount; \
		VariableCondition_GetVariableIndexFunc*	_getVariableIndex; \
		VariableCondition_GetValueIndexFunc*		_getValueIndex; \
		VariableCondition_GetValueCountFunc*		_getValueCount; \
		VariableCondition_GetValueFunc*				_getValue; \
		VariableCondition_ApplyFunc*					_apply; \
		\
		/* Stg_Class info */ \
		/** Register of Variables that may be operated on. */ \
		Variable_Register*								variable_Register; \
		/** Register of Condition functions that may be applied. */ \
		ConditionFunction_Register*					conFunc_Register; \
		Dictionary*											dictionary; \
		\
		/** Set specifying which indexes in the client structure are being controlled by this VC. */ \
		IndexSet*											_set; \
		/** the number of indexes this VC applies to. */ \
		Index													indexCount; \
		/** Array, of size indexCount, containing the indexes this VC applies to. */ \
		Index*												indexTbl; \
		/** Array containing the number of variables at each index that are controlled by this VC */ \
		VariableCondition_VariableIndex*				vcVarCountTbl; \
		/** 2d Array, which for each index this VC controls, stores the VariableCondition_Tuple that relates
		what condition is being applied to which Variable */ \
		VariableCondition_Tuple**						vcTbl; \
		/** Number of values in the valueTbl which will be applied somewhere using this VC */ \
		VariableCondition_ValueIndex					valueCount; \
		/** Array of values which are available to apply (which index/var pairs they apply to is determined
		by VariableCondition::vcTbl). */ \
		VariableCondition_Value*						valueTbl; \
		\
		UIntMap*												mapping;  \
		Stg_ComponentFactory*							cf;
	
	/** Abstract class for specifying (1 or more) conditions that apply to (1 or more) variables in an array - 
	see VariableCondition.h for details */
	struct _VariableCondition { __VariableCondition };
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** Constructor
	*/
	
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define VARIABLECONDITION_DEFARGS \
                STG_COMPONENT_DEFARGS, \
                VariableCondition_BuildSelfFunc*                _buildSelf, \
                VariableCondition_PrintConciseFunc*          _printConcise, \
                VariableCondition_ReadDictionaryFunc*      _readDictionary, \
                VariableCondition_GetSetFunc*                      _getSet, \
                VariableCondition_GetVariableCountFunc   _getVariableCount, \
                VariableCondition_GetVariableIndexFunc*  _getVariableIndex, \
                VariableCondition_GetValueIndexFunc*        _getValueIndex, \
                VariableCondition_GetValueCountFunc*        _getValueCount, \
                VariableCondition_GetValueFunc*                  _getValue, \
                VariableCondition_ApplyFunc*                        _apply

	#define VARIABLECONDITION_PASSARGS \
 		STG_COMPONENT_PASSARGS, \
	        _buildSelf,        \
	        _printConcise,     \
	        _readDictionary,   \
	        _getSet,           \
	        _getVariableCount, \
	        _getVariableIndex, \
	        _getValueIndex,    \
	        _getValueCount,    \
	        _getValue,         \
	        _apply           

	VariableCondition* _VariableCondition_New(  VARIABLECONDITION_DEFARGS  );
	
	void _VariableCondition_Init(
		void*									variableCondition, 
		AbstractContext*					context,
		Variable_Register*				variable_Register, 
		ConditionFunction_Register*	conFunc_Register,
		Dictionary*							dictionary );
	
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** General virtual functions
	*/
	
	void _VariableCondition_Delete( void* variableCondition );
	
	void _VariableCondition_Print( void* variableCondition );
	
	/* Copy */
	#define VariableCondition_Copy( self ) \
		(VariableCondition*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define VariableCondition_Copy( self ) \
		(VariableCondition*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	
	void* _VariableCondition_Copy( void* variableCondition, void* dest, Bool deep, Name nameExt, struct PtrMap* ptrMap );
	
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** Macros
	*/
	
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** Virtual functions
	*/
	
	void _VariableCondition_Apply( void* variableCondition, void* context );
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** Build functions
	*/
	void _VariableCondition_AssignFromXML( void* variableCondition, Stg_ComponentFactory* cf, void* d );
	void _VariableCondition_Build( void* variableCondition, void* data );
	void _VariableCondition_Initialise( void* variableCondition, void* data );
	void _VariableCondition_Execute( void* variableCondition, void* data );
	void _VariableCondition_Destroy( void* variableCondition, void* data );
	
	#define VariableCondition_BuildSelf( self, data ) \
		if( (self)->_buildSelf ) \
			(self)->_buildSelf( self, data )
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** Functions
	*/
	
	void VariableCondition_Apply( void* variableCondition, void* context );
	
	void VariableCondition_ApplyToVariable(
		void*										variableCondition,
		VariableCondition_VariableIndex	varIndex, 
		void*										context );
	
	void VariableCondition_ApplyToIndex( void* variableCondition, Index globalIndex, void* context );
	
	void VariableCondition_ApplyToIndexVariable(
		void*										variableCondition, 
		Index										globalIndex, 
		VariableCondition_VariableIndex	varIndex,
		void*										context );
	
	/** Checks whether a given (node)/(var at that node) pair is a variable condition */
	Bool VariableCondition_IsCondition(
		void*				variableCondition,
		Index				globalIndex, 
		Variable_Index	varIndex );
	
	VariableCondition_ValueIndex	VariableCondition_GetValueIndex(
		void*				variableCondition, 
		Index				globalIndex, 
		Variable_Index	varIndex );
	
	void VariableCondition_PrintConcise( void* variableCondition, Stream* stream );
	
#endif /* __StGermain_Base_Context_VariableCondition_h__ */

