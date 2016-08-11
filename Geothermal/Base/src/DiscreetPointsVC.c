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
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>

#include "types.h"
#include "DiscreetPointsVC.h"

#include <assert.h>
#include <string.h>

const Type DiscreetPointsVC_Type = (const Type)"DiscreetPointsVC";
const Name defaultDiscreetPointsVCName = (const Name)"defaultDiscreetPointsVCName";

/*-----------------------------------------------------------------------------------------------------------------
** Constructor
*/
VariableCondition* DiscreetPointsVC_Factory(
	AbstractContext*					context,
	Variable_Register*				variable_Register, 
	ConditionFunction_Register*	conFunc_Register, 
	Dictionary*							dictionary,
	void*									data )
{
	return (VariableCondition*) DiscreetPointsVC_New( defaultDiscreetPointsVCName, context, NULL, variable_Register, conFunc_Register, dictionary, (Mesh*)data );
}

DiscreetPointsVC* DiscreetPointsVC_New(
	Name									name,
	AbstractContext*					context,
	Name									_dictionaryEntryName, 
	Variable_Register*				variable_Register, 
	ConditionFunction_Register*	conFunc_Register, 
	Dictionary*							dictionary,
	void*									_mesh )
{
	DiscreetPointsVC* self = (DiscreetPointsVC*)_DiscreetPointsVC_DefaultNew( name );

	self->isConstructed = True;
	_VariableCondition_Init( self, context, variable_Register, conFunc_Register, dictionary );
	_DiscreetPointsVC_Init( self, _dictionaryEntryName, _mesh );

	return self;
}

void* _DiscreetPointsVC_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                               _sizeOfSelf = sizeof(DiscreetPointsVC);
	Type                                                       type = DiscreetPointsVC_Type;
	Stg_Class_DeleteFunction*                               _delete = _DiscreetPointsVC_Delete;
	Stg_Class_PrintFunction*                                 _print = _DiscreetPointsVC_Print;
	Stg_Class_CopyFunction*                                   _copy = _DiscreetPointsVC_Copy;
	Stg_Component_DefaultConstructorFunction*   _defaultConstructor = _DiscreetPointsVC_DefaultNew;
	Stg_Component_ConstructFunction*                     _construct = _DiscreetPointsVC_AssignFromXML;
	Stg_Component_BuildFunction*                             _build = _DiscreetPointsVC_Build;
	Stg_Component_InitialiseFunction*                   _initialise = _VariableCondition_Initialise;
	Stg_Component_ExecuteFunction*                         _execute = _VariableCondition_Execute;
	Stg_Component_DestroyFunction*                         _destroy = _DiscreetPointsVC_Destroy;
	AllocationType                               nameAllocationType = NON_GLOBAL;
	VariableCondition_BuildSelfFunc*                     _buildSelf = _DiscreetPointsVC_BuildSelf;
	VariableCondition_PrintConciseFunc*               _printConcise = VariableCondition_PrintConcise;
	VariableCondition_ReadDictionaryFunc*           _readDictionary = _DiscreetPointsVC_ReadDictionary;
	VariableCondition_GetSetFunc*                           _getSet = _DiscreetPointsVC_GetSet;
	VariableCondition_GetVariableCountFunc*       _getVariableCount = _DiscreetPointsVC_GetVariableCount;
	VariableCondition_GetVariableIndexFunc*       _getVariableIndex = _DiscreetPointsVC_GetVariableIndex;
	VariableCondition_GetValueIndexFunc*             _getValueIndex = _DiscreetPointsVC_GetValueIndex;
	VariableCondition_GetValueCountFunc*             _getValueCount = _DiscreetPointsVC_GetValueCount;
	VariableCondition_GetValueFunc*                       _getValue = _DiscreetPointsVC_GetValue;
	VariableCondition_ApplyFunc*                             _apply = _DiscreetPointsVC_Apply;

	return (void*) _DiscreetPointsVC_New(  DISCREETPOINTSVC_PASSARGS  );
}

DiscreetPointsVC* _DiscreetPointsVC_New(  DISCREETPOINTSVC_DEFARGS  ) {
	DiscreetPointsVC*	self;
	
	/* Allocate memory/General info */
	assert( _sizeOfSelf >= sizeof(DiscreetPointsVC) );
	self = (DiscreetPointsVC*)_VariableCondition_New(  VARIABLECONDITION_PASSARGS  );
	
	/* Virtual info */
	
	return self;
}

void _DiscreetPointsVC_Init( void* variableCondition, Name _dictionaryEntryName, void* _mesh ) {
	DiscreetPointsVC* self = (DiscreetPointsVC*) variableCondition;

	self->_dictionaryEntryName = _dictionaryEntryName;
	self->_mesh                = (Mesh*)_mesh;

	assert( _mesh && Stg_Class_IsInstance( _mesh, Mesh_Type ) );
}

/*--------------------------------------------------------------------------------------------------------------------------
** General virtual functions
*/


void _DiscreetPointsVC_Delete( void* variableCondition ) {
	DiscreetPointsVC*	self = (DiscreetPointsVC*)variableCondition;

	/* Stg_Class_Delete parent */
	_VariableCondition_Delete(self);
}

void _DiscreetPointsVC_Destroy( void* variableCondition, void* data ) {
	DiscreetPointsVC*	self = (DiscreetPointsVC*)variableCondition;

	Memory_Free( self->_entryTbl );
	
	/* Stg_Class_Delete parent */
	_VariableCondition_Destroy( self, data );
}

void _DiscreetPointsVC_Print(void* variableCondition, Stream* stream) {
	DiscreetPointsVC*      	self 	= (DiscreetPointsVC*)variableCondition;
	Index                   array_I;
	
	/* General info */
	Journal_Printf( stream, "DiscreetPointsVC (ptr): %p\n", self);
	
	/* Virtual info */
	
	/* Stg_Class info */
	Journal_Printf( stream, "\tdictionary (ptr): %p\n", self->dictionary);
	Journal_Printf( stream, "\t_dictionaryEntryName (ptr): %p\n", self->_dictionaryEntryName);
	if (self->_dictionaryEntryName)
		Journal_Printf( stream, "\t\t_dictionaryEntryName: %s\n", self->_dictionaryEntryName);
	
	Journal_Printf( stream, "\t_entryTbl (ptr): %p\n", self->_entryTbl);
	Journal_Printf( stream, "\t\tvarName (ptr): %p\n", self->_entryTbl[0].varName);
	if (self->_entryTbl[0].varName)
		Journal_Printf( stream, "\t\t\t\tvarName: %s\n", self->_entryTbl[0].varName);
	Journal_Printf( stream, "\t\tvalue:\n");
	Journal_Printf( stream, "\t\t\ttype: VC_ValueType_CFIndex\n");
	Journal_Printf( stream, "\t\t\tasCFIndex: %u\n", self->_entryTbl[0].value.as.typeCFIndex);
	Journal_Printf( stream, "\t_mesh (ptr): %p\n", self->_mesh);
	Journal_Printf( stream, "\tfeVariable (ptr): %p\n", self->feVariable);
	
	/* Print parent */
	_VariableCondition_Print( self );
}


void* _DiscreetPointsVC_Copy( void* variableCondition, void* dest, Bool deep, Name nameExt, struct PtrMap* ptrMap ) {
	DiscreetPointsVC*        self           = (DiscreetPointsVC*)variableCondition;
	DiscreetPointsVC*        newDiscreetPointsVC;
	PtrMap*         map            = ptrMap;
	Bool            ownMap         = False;
	
	if( !map ) {
		map = PtrMap_New( 10 );
		ownMap = True;
	}
	
	newDiscreetPointsVC = (DiscreetPointsVC*)_VariableCondition_Copy( self, dest, deep, nameExt, map );
	
	newDiscreetPointsVC->_dictionaryEntryName = self->_dictionaryEntryName;
	
	if( deep ) {
		newDiscreetPointsVC->_mesh = (Mesh*)Stg_Class_Copy( self->_mesh, NULL, deep, nameExt, map );
		
		if( (newDiscreetPointsVC->_entryTbl = PtrMap_Find( map, self->_entryTbl )) == NULL && self->_entryTbl ) {
			newDiscreetPointsVC->_entryTbl = Memory_Alloc_Array( DiscreetPointsVC_Entry, 1, "DiscreetPointsVC->_entryTbl");
			memcpy( newDiscreetPointsVC->_entryTbl, self->_entryTbl, sizeof(DiscreetPointsVC_Entry) );
			PtrMap_Append( map, newDiscreetPointsVC->_entryTbl, self->_entryTbl );
		}
	}
	else {
		newDiscreetPointsVC->_mesh = self->_mesh;
		newDiscreetPointsVC->_entryTbl = self->_entryTbl;
	}
	
	if( ownMap ) {
		Stg_Class_Delete( map );
	}
	
	return (void*)newDiscreetPointsVC;
}
	
/****************** Stg_Component Virtual Functions ******************/
void _DiscreetPointsVC_AssignFromXML( void* variableCondition, Stg_ComponentFactory* cf, void* data ) {
}

void _DiscreetPointsVC_Build(  void* variableCondition, void* data ) {
	DiscreetPointsVC*		self 	= (DiscreetPointsVC*)variableCondition;

	_DiscreetPointsVC_BuildSelf( self, data );
	_VariableCondition_Build( self, data );
}
	
/****************** VariableCondition Virtual Functions ******************/
void _DiscreetPointsVC_BuildSelf(  void* variableCondition, void* data /* for build phase */ ) {
	DiscreetPointsVC*       self    	= (DiscreetPointsVC*)variableCondition;
	AbstractContext* 	context 	= (AbstractContext*) data;
	ConditionFunction*	condFunc;
	Index			cfIndex;
	Name			bcsVariableName;

	assert( context && Stg_Class_IsInstance( context, AbstractContext_Type ) );
	assert( self->_mesh );

	Stg_Component_Build( self->_mesh, data, False );

	bcsVariableName = Dictionary_GetString_WithDefault( context->dictionary, "DiscreetVC_FieldVariable", "" );
	self->feVariable = Stg_ComponentFactory_ConstructByName( context->CF, (Name)bcsVariableName, FeVariable, True, data  ); //???
	Stg_Component_Build( self->feVariable, data, False );

	condFunc = ConditionFunction_New( DiscreetPointsVC_FromFeVariable, (Name)"discreetBCs", NULL  );
	ConditionFunction_Register_Add( condFunc_Register, condFunc );
	cfIndex = ConditionFunction_Register_GetIndex( self->conFunc_Register, "discreetBCs" );
	self->_entryTbl[0].value.as.typeCFIndex = cfIndex;
}

void _DiscreetPointsVC_ReadDictionary( void* variableCondition, void* dictionary ) {
	DiscreetPointsVC*               self 		= (DiscreetPointsVC*)variableCondition;
	Dictionary_Entry_Value*   	vcDictVal;
	Dictionary_Entry_Value    	_vcDictVal;
	Dictionary_Entry_Value*   	varsVal;
	Dictionary_Entry_Value*   	coordsList;
	unsigned			dim		= Mesh_GetDimSize( self->_mesh );
	unsigned			listSize;
	unsigned			list_i;
	
	/* Find dictionary entry */
	if (self->_dictionaryEntryName)
		vcDictVal = Dictionary_Get( dictionary, (Dictionary_Entry_Key)self->_dictionaryEntryName );
	else {
		vcDictVal = &_vcDictVal;
		Dictionary_Entry_Value_InitFromStruct(vcDictVal, dictionary);
	}
	
	if (vcDictVal) {
		/* read in the BC coords and values from the XML */
		coordsList = Dictionary_Get( Dictionary_Entry_Value_AsDictionary( vcDictVal ), (Dictionary_Entry_Key)"discreetBCs" );
		listSize = Dictionary_Entry_Value_GetCount( coordsList ) / (dim + 1 );

		self->coords = Memory_Alloc_2DArray( double, listSize, dim, (Name)"DiscreetPointsVC-coords"  );
		self->values = Memory_Alloc_Array( double, listSize, "DiscreetPointsVC->values" );

		for( list_i = 0; list_i < listSize; list_i++ )	{
			self->coords[list_i][0] = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetElement( coordsList, (dim+1)*list_i + 0 ) );
			self->coords[list_i][1] = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetElement( coordsList, (dim+1)*list_i + 1 ) );
			if( dim == 3 )
				self->coords[list_i][2] = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetElement( coordsList, (dim+1)*list_i + 2 ) );
			self->values[list_i] = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetElement( coordsList, (dim+1)*list_i + dim ) );
		}

		self->listSize = listSize;

		/* and set the variableCondition type */
		self->_entryTbl = Memory_Alloc( DiscreetPointsVC_Entry, "discreetPointsVC_entry" );
		self->_entryTbl[0].value.type = VC_ValueType_CFIndex;
		self->_entryTbl[0].varName = Dictionary_Entry_Value_AsString( ( Dictionary_Entry_Value_GetMember( vcDictVal, (Dictionary_Entry_Key)"name" ) ) );
	}
}

IndexSet* _DiscreetPointsVC_GetSet(void* variableCondition) {
	DiscreetPointsVC*	self 	= (DiscreetPointsVC* )variableCondition;
	Mesh*			mesh 	= self->_mesh;
	IndexSet*		set;
	unsigned		coord_i;
	unsigned		el_i, node_i;

	Stg_Component_Initialise( mesh, NULL, False );

	set = IndexSet_New( Mesh_GetDomainSize( mesh, MT_VERTEX ) );

	for( coord_i = 0; coord_i < self->listSize; coord_i++ ) {
		if( Mesh_SearchElements( mesh, self->coords[coord_i], &el_i ) ) {
			if( el_i >= Mesh_GetLocalSize( mesh, Mesh_GetDimSize( mesh ) ) )
				continue;

			node_i = Mesh_NearestVertex( mesh, self->coords[coord_i] );
			if( node_i >= Mesh_GetLocalSize( mesh, MT_VERTEX ) )
				continue;

			IndexSet_Add( set, node_i );

			FeVariable_SetValueAtNode( self->feVariable, node_i, &self->values[coord_i] );
		}
	}

	Memory_Free( self->coords );
	Memory_Free( self->values );

	return set;
}

void DiscreetPointsVC_FromFeVariable( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ) {
	FiniteElementContext*	context		= (FiniteElementContext*)_context;
	FeVariable*		feVariable	= (FeVariable*)FieldVariable_Register_GetByName( context->fieldVariable_Register, "TemperatureBCsField" );
	double*			result		= (double*)_result;
	
	FeVariable_GetValueAtNode( feVariable, node_lI, result );
}

void _DiscreetPointsVC_Apply( void* variableCondition, void* data ) { }

VariableCondition_VariableIndex _DiscreetPointsVC_GetVariableCount(void* variableCondition, Index globalIndex) {
	DiscreetPointsVC*	self = (DiscreetPointsVC*)variableCondition;

	return 1;
}

Variable_Index _DiscreetPointsVC_GetVariableIndex(void* variableCondition, Index globalIndex, VariableCondition_VariableIndex varIndex) {
	DiscreetPointsVC*        self          = (DiscreetPointsVC*)variableCondition;
	Variable_Index  searchedIndex = 0;
	Stream*         errorStr      = Journal_Register( Error_Type, (Name)self->type  );
	Name            varName;
	
	varName = self->_entryTbl[varIndex].varName;
	searchedIndex = Variable_Register_GetIndex(self->variable_Register, varName );
	
	Journal_Firewall( 
			( searchedIndex < self->variable_Register->count ),
			errorStr,
			"Error- in %s: searching for index of varIndex %u (\"%s\") at global node number %u failed"
			" - register returned index %u, greater than count %u.\n",
			__func__, varIndex, varName, globalIndex, searchedIndex, self->variable_Register->count );

	return searchedIndex; 
}

VariableCondition_ValueIndex _DiscreetPointsVC_GetValueIndex(
		void*                                       variableCondition, 
		Index                                       globalIndex, 
		VariableCondition_VariableIndex             varIndex )
{
	return varIndex;
}


VariableCondition_ValueIndex _DiscreetPointsVC_GetValueCount(void* variableCondition) {
	DiscreetPointsVC*	self = (DiscreetPointsVC*)variableCondition;
	
	return 1;
}


VariableCondition_Value _DiscreetPointsVC_GetValue(void* variableCondition, VariableCondition_ValueIndex valIndex) {
	DiscreetPointsVC*	self = (DiscreetPointsVC*)variableCondition;

	return self->_entryTbl[valIndex].value;
}



