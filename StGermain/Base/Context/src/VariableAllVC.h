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
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StGermain_Base_Context_VariableAllVC_h__
#define __StGermain_Base_Context_VariableAllVC_h__
	

	extern const Type VariableAllVC_Type;
	
	
	#define __VariableAllVC_Entry \
		Name				varName; \
		VariableCondition_Value		value; \
		
	struct _VariableAllVC_Entry { __VariableAllVC_Entry };
	
	
	#define __VariableAllVC \
		/* General info */ \
		__VariableCondition \
		\
		/* Virtual info */ \
		\
		/* Stg_Class info */ \
		Name								_dictionaryEntryName; \
		VariableAllVC_Entry_Index	_entryCount; \
		VariableAllVC_Entry*			_entryTbl; \
		void*								data;

	struct _VariableAllVC { __VariableAllVC };
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** Constructor
	*/
	
	VariableCondition* VariableAllVC_Factory(
		AbstractContext*					context,
		Variable_Register*				variable_Register, 
		ConditionFunction_Register*	conFunc_Register, 
		Dictionary*							dictionary,
		void*									data );
	
	VariableAllVC* VariableAllVC_New(
		Name									name,
		AbstractContext*					context,
		Name									_dictionaryEntryName, 
		Variable_Register*				variable_Register, 
		ConditionFunction_Register*	conFunc_Register,
		Dictionary*							dictionary,
		void*									mesh );
	
	VariableAllVC* _VariableAllVC_DefaultNew( Name name );
	
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define VARIABLEALLVC_DEFARGS \
                VARIABLECONDITION_DEFARGS

	#define VARIABLEALLVC_PASSARGS \
                VARIABLECONDITION_PASSARGS

	VariableAllVC* _VariableAllVC_New(  VARIABLEALLVC_DEFARGS  );
	
	void _VariableAllVC_Init( void* allElementsVC, Name _dictionaryEntryName, void* mesh );
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** General virtual functions
	*/
	
	void _VariableAllVC_Delete( void* allElementsVC );
	
	void _VariableAllVC_Print( void* allElementsVC, Stream* stream );
	
	void _VariableAllVC_Destroy( void* allElementsVC, void* data );

	/* Copy */
	#define VariableAllVC_Copy( self ) \
		(VariableCondition*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define VariableAllVC_Copy( self ) \
		(VariableCondition*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	
	void* _VariableAllVC_Copy( void* allElementsVC, void* dest, Bool deep, Name nameExt, struct PtrMap* ptrMap );
	
	void _VariableAllVC_Build( void* allElementsVC, void* data );
	
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** Macros
	*/
	
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** Virtual functions
	*/
	
	void _VariableAllVC_BuildSelf( void* allElementsVC, void* data );
	
	void _VariableAllVC_ReadDictionary( void* variableCondition, void* dictionary );
	
	IndexSet* _VariableAllVC_GetSet( void* variableCondition );
	
	VariableCondition_VariableIndex	_VariableAllVC_GetVariableCount( void* variableCondition, Index globalIndex );
	
	Variable_Index _VariableAllVC_GetVariableIndex( void* variableCondition, Index globalIndex, VariableCondition_VariableIndex varIndex );
						
	VariableCondition_ValueIndex	_VariableAllVC_GetValueIndex( void* variableCondition, Index globalIndex, VariableCondition_VariableIndex varIndex );
						
	VariableCondition_ValueIndex	_VariableAllVC_GetValueCount( void* variableCondition );
	
	VariableCondition_Value _VariableAllVC_GetValue( void* variableCondition, VariableCondition_ValueIndex valIndex );
	
	void _VariableAllVC_PrintConcise( void* variableCondition, Stream* stream );
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** Build functions
	*/
	
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** Functions
	*/

	
#endif /* __StGermain_Base_Context_VariableAllVC_h__ */

