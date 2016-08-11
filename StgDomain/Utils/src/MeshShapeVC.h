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

#ifndef __StgDomain_Utils_MeshShapeVC_h__
#define __StgDomain_Utils_MeshShapeVC_h__
	
	extern const Type MeshShapeVC_Type;
	
	#define __MeshShapeVC_Entry \
		Name							varName; \
		VariableCondition_Value	value; \
	
	typedef struct { __MeshShapeVC_Entry } MeshShapeVC_Entry;
	
	#define __MeshShapeVC \
		/* General info */ \
		__VariableCondition \
		\
		/* Virtual info */ \
		\
		/* Stg_Class info */ \
		Name							_dictionaryEntryName; \
		MeshShapeVC_Entry_Index	_entryCount;          \
		MeshShapeVC_Entry*		_entryTbl;            \
		Mesh*							_mesh;                \
		Stg_Shape*					_shape;               \
		Name							shapeName;

	struct MeshShapeVC { __MeshShapeVC };

	/*-----------------------------------------------------------------------------------------------------------------
	** Constructor
	*/
	
	VariableCondition* MeshShapeVC_Factory(
		AbstractContext*					context,
		Variable_Register*				variable_Register, 
		ConditionFunction_Register*	conFunc_Register, 
		Dictionary*							dictionary,
		void*									data );
	
	MeshShapeVC* MeshShapeVC_New(
		Name									name,
		AbstractContext*					context,
		Name									_dictionaryEntryName, 
		Variable_Register*				variable_Register, 
		ConditionFunction_Register*	conFunc_Register, 
		Dictionary*							dictionary,
		void*									_mesh );
	
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define MESHSHAPEVC_DEFARGS \
                VARIABLECONDITION_DEFARGS

	#define MESHSHAPEVC_PASSARGS \
                VARIABLECONDITION_PASSARGS

	MeshShapeVC* _MeshShapeVC_New(  MESHSHAPEVC_DEFARGS  );
	
	void _MeshShapeVC_Init( void* shapeVC, Name _dictionaryEntryName, void* _mesh );
	
	/* Stg_Class Virtual Functions */
	void _MeshShapeVC_Delete( void* shapeVC );

	void _MeshShapeVC_Print( void* shapeVC, Stream* stream );

	void* _MeshShapeVC_Copy( void* shapeVC, void* dest, Bool deep, Name nameExt, struct PtrMap* ptrMap );

	void _MeshShapeVC_Destroy( void* shapeVC, void* data);

	#define MeshShapeVC_Copy( self ) \
		(VariableCondition*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define MeshShapeVC_Copy( self ) \
		(VariableCondition*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	
	/* Stg_Component Virtual Functions */
	void* _MeshShapeVC_DefaultNew( Name name );

	void _MeshShapeVC_Build(  void* shapeVC, void* data );

	void _MeshShapeVC_AssignFromXML( void* shapeVC, Stg_ComponentFactory* cf, void* data );
	
	/* VariableCondition Virtual Functions */
	void _MeshShapeVC_BuildSelf( void* shapeVC, void* data );

	void _MeshShapeVC_PrintConcise( void* variableCondition, Stream* stream );

	void _MeshShapeVC_ReadDictionary( void* variableCondition, void* dictionary );

	IndexSet* _MeshShapeVC_GetSet( void* variableCondition );

	VariableCondition_VariableIndex	_MeshShapeVC_GetVariableCount( void* variableCondition, Index globalIndex );

	Variable_Index _MeshShapeVC_GetVariableIndex( void* variableCondition, Index globalIndex, VariableCondition_VariableIndex varIndex );
						
	VariableCondition_ValueIndex _MeshShapeVC_GetValueIndex( void* variableCondition, Index globalIndex, VariableCondition_VariableIndex varIndex );
						
	VariableCondition_ValueIndex _MeshShapeVC_GetValueCount( void* variableCondition );
	
	VariableCondition_Value	_MeshShapeVC_GetValue( void* variableCondition, VariableCondition_ValueIndex valIndex );
	
	
#endif

