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

#ifndef __Geothermal_Base_DiscreetPointsVC_h__
#define __Geothermal_Base_DiscreetPointsVC_h__
	
	extern const Type DiscreetPointsVC_Type;
	
	#define __DiscreetPointsVC_Entry \
		Name							varName; \
		VariableCondition_Value	value; \
	
	typedef struct { __DiscreetPointsVC_Entry } DiscreetPointsVC_Entry;
	
	#define __DiscreetPointsVC \
		/* General info */ \
		__VariableCondition \
		\
		/* Virtual info */ \
		\
		/* Stg_Class info */ \
		Name							_dictionaryEntryName; \
		DiscreetPointsVC_Entry*	_entryTbl; \
		Mesh*							_mesh; \
		FeVariable*					feVariable; \
		double**						coords; \
		double*						values; \
		unsigned						listSize; \

	struct DiscreetPointsVC { __DiscreetPointsVC };



	/*-----------------------------------------------------------------------------------------------------------------
	** Constructor
	*/
	
	VariableCondition* DiscreetPointsVC_Factory(
		AbstractContext*					context,
		Variable_Register*				variable_Register, 
		ConditionFunction_Register*	conFunc_Register, 
		Dictionary*							dictionary,
		void*									data );

	DiscreetPointsVC* DiscreetPointsVC_New(
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

	#define DISCREETPOINTSVC_DEFARGS \
                VARIABLECONDITION_DEFARGS

	#define DISCREETPOINTSVC_PASSARGS \
                VARIABLECONDITION_PASSARGS

	DiscreetPointsVC* _DiscreetPointsVC_New(  DISCREETPOINTSVC_DEFARGS  ); 

	void _DiscreetPointsVC_Init( void* pointsVC, Name _dictionaryEntryName, void* _mesh );
	
	/* Stg_Class Virtual Functions */
	void _DiscreetPointsVC_Delete( void* pointsVC );

	void _DiscreetPointsVC_Print( void* pointsVC, Stream* stream );

	void* _DiscreetPointsVC_Copy( void* pointsVC, void* dest, Bool deep, Name nameExt, struct PtrMap* ptrMap );

	void _DiscreetPointsVC_Destroy( void* pointsVC, void* data );

	#define DiscreetPointsVC_Copy( self ) \
		(VariableCondition*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define DiscreetPointsVC_Copy( self ) \
		(VariableCondition*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	
	/* Stg_Component Virtual Functions */
	void* _DiscreetPointsVC_DefaultNew( Name name );

	void _DiscreetPointsVC_Build(  void* pointsVC, void* data );

	void _DiscreetPointsVC_AssignFromXML( void* pointsVC, Stg_ComponentFactory* cf, void* data );
	
	/* VariableCondition Virtual Functions */
	void _DiscreetPointsVC_BuildSelf( void* variableCondition, void* data );

	void _DiscreetPointsVC_ReadDictionary( void* variableCondition, void* dictionary );

	IndexSet* _DiscreetPointsVC_GetSet( void* variableCondition );

	void _DiscreetPointsVC_Apply( void* variableCondition, void* data );

	VariableCondition_VariableIndex	_DiscreetPointsVC_GetVariableCount( void* variableCondition, Index globalIndex );

	Variable_Index _DiscreetPointsVC_GetVariableIndex( void* variableCondition, Index globalIndex, VariableCondition_VariableIndex varIndex );
						
	VariableCondition_ValueIndex _DiscreetPointsVC_GetValueIndex( void* variableCondition, Index globalIndex, VariableCondition_VariableIndex varIndex );
						
	VariableCondition_ValueIndex _DiscreetPointsVC_GetValueCount( void* variableCondition );
	
	VariableCondition_Value	_DiscreetPointsVC_GetValue( void* variableCondition, VariableCondition_ValueIndex valIndex );
	
	void DiscreetPointsVC_FromFeVariable( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result );	
	
#endif

