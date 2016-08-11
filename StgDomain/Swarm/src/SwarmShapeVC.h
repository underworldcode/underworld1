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

#ifndef __StgDomain_Swarm_SwarmShapeVC_h__
#define __StgDomain_Swarm_SwarmShapeVC_h__
	
	extern const Type SwarmShapeVC_Type;
	
	#define __SwarmShapeVC_Entry \
		Name							varName; \
		VariableCondition_Value	value; \
	
	typedef struct { __SwarmShapeVC_Entry } SwarmShapeVC_Entry;
	
	#define __SwarmShapeVC \
		/* General info */ \
		__VariableCondition \
		\
		/* Virtual info */ \
		\
		/* Stg_Class info */ \
		Name								_dictionaryEntryName; \
		SwarmShapeVC_Entry_Index	_entryCount; \
		SwarmShapeVC_Entry*			_entryTbl; \
		Swarm*							_swarm; \
		Stg_Shape*						_shape; \
		Name								shapeName; \

	struct SwarmShapeVC { __SwarmShapeVC };
	
	/*-----------------------------------------------------------------------------------------------------------------
	** Constructor
	*/
	
	VariableCondition* SwarmShapeVC_Factory(
		AbstractContext*					context,
		Variable_Register*				variable_Register, 
		ConditionFunction_Register*	conFunc_Register, 
		Dictionary*							dictionary,
		void*									data );
	
	SwarmShapeVC* SwarmShapeVC_New(
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

	#define SWARMSHAPEVC_DEFARGS \
                VARIABLECONDITION_DEFARGS

	#define SWARMSHAPEVC_PASSARGS \
                VARIABLECONDITION_PASSARGS

	SwarmShapeVC* _SwarmShapeVC_New(  SWARMSHAPEVC_DEFARGS  );
	
	void _SwarmShapeVC_Init( void* shapeVC, Name _dictionaryEntryName, void* _mesh );
	
	/* Stg_Class Virtual Functions */
	void _SwarmShapeVC_Delete( void* shapeVC );

	void _SwarmShapeVC_Print( void* shapeVC, Stream* stream );

	void* _SwarmShapeVC_Copy( void* shapeVC, void* dest, Bool deep, Name nameExt, struct PtrMap* ptrMap );

	void _SwarmShapeVC_Destroy( void* shapeVC, void* data );

	#define SwarmShapeVC_Copy( self ) \
		(VariableCondition*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define SwarmShapeVC_Copy( self ) \
		(VariableCondition*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	
	/* Stg_Component Virtual Functions */
	void* _SwarmShapeVC_DefaultNew( Name name );

	/* added to call porosity distribution functions standard condition functions plugin */
	void _SwarmShapeVC_Initialise(  void* shapeVC, void* data );

	void _SwarmShapeVC_Build(  void* shapeVC, void* data );

	void _SwarmShapeVC_AssignFromXML( void* shapeVC, Stg_ComponentFactory* cf, void* data );
	
	/* VariableCondition Virtual Functions */
	void _SwarmShapeVC_BuildSelf( void* shapeVC, void* data );

	void _SwarmShapeVC_PrintConcise( void* variableCondition, Stream* stream );

	void _SwarmShapeVC_ReadDictionary( void* variableCondition, void* dictionary );

	IndexSet* _SwarmShapeVC_GetSet( void* variableCondition );

	VariableCondition_VariableIndex	_SwarmShapeVC_GetVariableCount( void* variableCondition, Index globalIndex );

	Variable_Index _SwarmShapeVC_GetVariableIndex( void* variableCondition, Index globalIndex, VariableCondition_VariableIndex varIndex );
						
	VariableCondition_ValueIndex _SwarmShapeVC_GetValueIndex( void* variableCondition, Index globalIndex, VariableCondition_VariableIndex varIndex );
						
	VariableCondition_ValueIndex _SwarmShapeVC_GetValueCount( void* variableCondition );
	
	VariableCondition_Value	_SwarmShapeVC_GetValue( void* variableCondition, VariableCondition_ValueIndex valIndex );
	
	
#endif

