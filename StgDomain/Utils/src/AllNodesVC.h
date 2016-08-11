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

#ifndef __StgDomain_Utils_AllNodesVC_h__
#define __StgDomain_Utils_AllNodesVC_h__
	
	
	extern const Type AllNodesVC_Type;
	
	#define __AllNodesVC_Entry \
		Name							varName; \
		VariableCondition_Value	value; \
		
	struct _AllNodesVC_Entry { __AllNodesVC_Entry };
	
	#define __AllNodesVC \
		/* General info */ \
		__VariableCondition \
		\
		/* Virtual info */ \
		\
		/* Stg_Class info */ \
		Name							_dictionaryEntryName; \
		AllNodesVC_Entry_Index	_entryCount; \
		AllNodesVC_Entry*			_entryTbl; \
		Mesh*							mesh;

	struct AllNodesVC { __AllNodesVC };

	/*--------------------------------------------------------------------------------------------------------------------------
	** Constructor
	*/
	
	VariableCondition* AllNodesVC_Factory(
		AbstractContext*					context,
		Variable_Register*				variable_Register, 
		ConditionFunction_Register*	conFunc_Register, 
		Dictionary*							dictionary,
		void*									data );
	
	AllNodesVC* _AllNodesVC_DefaultNew( Name name );

	AllNodesVC* AllNodesVC_New(
		Name									name,
		AbstractContext*					context,
		Name									_dictionaryEntryName, 
		Variable_Register*				variable_Register, 
		ConditionFunction_Register*	conFunc_Register,
		Dictionary*							dictionary,
		void*									mesh );
	
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define ALLNODESVC_DEFARGS \
                VARIABLECONDITION_DEFARGS

	#define ALLNODESVC_PASSARGS \
                VARIABLECONDITION_PASSARGS

	AllNodesVC* _AllNodesVC_New(  ALLNODESVC_DEFARGS  );
	
	void _AllNodesVC_Init( void* allNodesVC, Name _dictionaryEntryName, void* mesh );
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** General virtual functions
	*/
	
	void _AllNodesVC_Delete( void* allNodesVC );
	
	void _AllNodesVC_Print( void* allNodesVC, Stream* stream );
	
	void _AllNodesVC_Destroy( void* allNodesVC, void* data );

	/* Copy */
	#define AllNodesVC_Copy( self ) \
		(VariableCondition*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define AllNodesVC_Copy( self ) \
		(VariableCondition*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	
	void* _AllNodesVC_Copy( void* allNodesVC, void* dest, Bool deep, Name nameExt, struct PtrMap* ptrMap );
	
	void _AllNodesVC_Build( void* allNodesVC, void* data );
	
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** Macros
	*/
	
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** Virtual functions
	*/
	
	void _AllNodesVC_AssignFromXML( void* allNodesVC, Stg_ComponentFactory* cf, void* data ) ;
	
	void _AllNodesVC_BuildSelf( void* allNodesVC, void* data );
	
	void _AllNodesVC_ReadDictionary( void* variableCondition, void* dictionary );
	
	IndexSet* _AllNodesVC_GetSet( void* variableCondition );
	
	VariableCondition_VariableIndex	_AllNodesVC_GetVariableCount( void* variableCondition, Index globalIndex );
	
	Variable_Index _AllNodesVC_GetVariableIndex( void* variableCondition, Index globalIndex, VariableCondition_VariableIndex	varIndex );
						
	VariableCondition_ValueIndex _AllNodesVC_GetValueIndex( void* variableCondition, Index globalIndex, VariableCondition_VariableIndex	varIndex );
						
	VariableCondition_ValueIndex _AllNodesVC_GetValueCount( void* variableCondition );
	
	VariableCondition_Value _AllNodesVC_GetValue( void* variableCondition, VariableCondition_ValueIndex valIndex );
	
	void _AllNodesVC_PrintConcise( void* variableCondition, Stream* stream );
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** Build functions
	*/
	
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** Functions
	*/

	
#endif /* __StgDomain_Utils_AllNodesVC_h__ */

