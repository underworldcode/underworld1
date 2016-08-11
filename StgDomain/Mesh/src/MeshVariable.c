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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>

#include <StGermain/StGermain.h>
#include <StgDomain/Geometry/Geometry.h>

#include "Mesh.h"


/* Textual name of this class */
const Type MeshVariable_Type = "MeshVariable";


/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/

MeshVariable* MeshVariable_New( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(MeshVariable);
	Type                                                      type = MeshVariable_Type;
	Stg_Class_DeleteFunction*                              _delete = _MeshVariable_Delete;
	Stg_Class_PrintFunction*                                _print = _MeshVariable_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = (void* (*)(Name))_MeshVariable_New;
	Stg_Component_ConstructFunction*                    _construct = _MeshVariable_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _MeshVariable_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _MeshVariable_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _MeshVariable_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _MeshVariable_Destroy;
	Index                                                dataCount = 0;
	SizeT*                                             dataOffsets = NULL;
	Variable_DataType*                                   dataTypes = NULL;
	Index*                                          dataTypeCounts = NULL;
	Name*                                                dataNames = NULL;
	SizeT*                                           structSizePtr = NULL;
	Index*                                            arraySizePtr = NULL;
	Variable_ArraySizeFunc*                          arraySizeFunc = NULL;
	void**                                             arrayPtrPtr = NULL;
	Variable_Register*                                          vr = NULL;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	MeshVariable* self = _MeshVariable_New(  MESHVARIABLE_PASSARGS  );

	_MeshVariable_Init( self );

	return self;
}

MeshVariable* _MeshVariable_New(  MESHVARIABLE_DEFARGS  ) {
	MeshVariable* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(MeshVariable) );
	self = (MeshVariable*)_Variable_New(  VARIABLE_PASSARGS  );

	/* Virtual info */

	return self;
}

void _MeshVariable_Init( MeshVariable* self ) {
	self->mesh = NULL;
	self->topoDim = MT_VERTEX;
	self->meshArraySize = 0;
}

/*----------------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _MeshVariable_Delete( void* meshVariable ) {
	MeshVariable*	self = (MeshVariable*)meshVariable;

	/* Delete the parent. */
	_Variable_Delete( self );
}

void _MeshVariable_Print( void* meshVariable, Stream* stream ) {
	MeshVariable*	self = (MeshVariable*)meshVariable;
	
	/* Set the Journal for printing informations */
	Stream* meshVariableStream;
	meshVariableStream = Journal_Register( InfoStream_Type, (Name)"MeshVariableStream"  );

	/* Print parent */
	Journal_Printf( stream, "MeshVariable (ptr): (%p)\n", self );
	_Variable_Print( self, stream );
}

void _MeshVariable_AssignFromXML( void* meshVariable, Stg_ComponentFactory* cf, void* data ) {
	MeshVariable*		self = (MeshVariable*)meshVariable;
	SizeT					dataOffsets[] = { 0 };
	Variable_DataType	dataTypes[] = { 0 };		/* Init value later */
	Index					dataTypeCounts[] = { 1 };
	Dictionary*			componentDict = NULL;
	Dictionary*			thisComponentDict = NULL;
	Name					dataTypeName = NULL;
	Name					rankName = NULL;
	void*					variableRegister = NULL;
	void*					pointerRegister = NULL;
	Name*					names = NULL;
	Stream*				error = Journal_Register( Error_Type, (Name)self->type );
	Mesh*					mesh;
	AbstractContext*	context;
	
	assert( self );

	componentDict = cf->componentDict;
	assert( componentDict  );
	thisComponentDict = Dictionary_GetDictionary( componentDict, self->name );
	assert( thisComponentDict );

	context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", AbstractContext, False, data );
	if( !context  )
		context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", AbstractContext, True, data );
	
	/* Grab Registers */
	variableRegister = context->variable_Register;
	assert( variableRegister );
	pointerRegister = context->pointer_Register;
	assert( pointerRegister  );

	/* Construct the mesh. */
	mesh = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"mesh", Mesh, True, data  );
	MeshVariable_SetMesh( self, mesh );

	/* Get the topological element we're intereseted in. */
	self->topoDim = Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"topologicalDim", 0  );
			
	/* Get Type of Variable */
	dataTypeName = Dictionary_GetString( thisComponentDict, (Dictionary_Entry_Key)"DataType"  );
	if ( !strcasecmp( dataTypeName, "Double" ) )
		dataTypes[0] = Variable_DataType_Double;
	else if ( !strcasecmp( dataTypeName, "Float" ) )
		dataTypes[0] = Variable_DataType_Float;
	else if ( !strcasecmp( dataTypeName, "Int" ) )
		dataTypes[0] = Variable_DataType_Int;
	else if ( !strcasecmp( dataTypeName, "Char" ) )
		dataTypes[0] = Variable_DataType_Char;
	else if ( !strcasecmp( dataTypeName, "Short" ) )
		dataTypes[0] = Variable_DataType_Short;
	else 
		Journal_Firewall( False, error, "Variable '%s' cannot understand data type '%s'\n", self->name, dataTypeName );

	/* Get Rank of Variable - i.e. Scalar or Vector */
	rankName = Dictionary_GetString( thisComponentDict, (Dictionary_Entry_Key)"Rank"  );
	if( !strcasecmp( rankName, "Scalar" ) ){
		dataTypeCounts[0] = 1;
	}
	else if ( !strcasecmp( rankName, "Vector" ) ){
		Dictionary_Entry_Value* list;
		Index                   nameCount = 0;

		/* Get Names from list */
		if (( list = Dictionary_Get( thisComponentDict, (Dictionary_Entry_Key)"names" ) )) {
			Index entry_I;

			nameCount = Dictionary_Entry_Value_GetCount( list  );
			names = Memory_Alloc_Array( Name, nameCount, "Variable Names" );

			for ( entry_I = 0 ; entry_I < nameCount ; entry_I++ )
				names[ entry_I ] = Dictionary_Entry_Value_AsString( Dictionary_Entry_Value_GetElement(list, entry_I ) );
		}
		dataTypeCounts[0] = Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"VectorComponentCount", nameCount  );

		Journal_Firewall( nameCount >= dataTypeCounts[0], error, "Variable '%s' has too few names in list for %d vector components.\n", self->name, dataTypeCounts[0] );
	}
	else
		Journal_Firewall( False, error, "Variable '%s' cannot understand rank '%s'\n", self->name, rankName );

	_Variable_Init(
                  (Variable*)self,
                           context,
                                 1,
                       dataOffsets,
                         dataTypes,
                    dataTypeCounts,
                             names,
                                 0,
                              NULL,
    _MeshVariable_GetMeshArraySize,
           (void**)&self->arrayPtr,
                              True,
                  variableRegister );

	/* Clean Up */
	if (names)
		Memory_Free(names);
}

void _MeshVariable_Build( void* meshVariable, void* data ) {
	MeshVariable*	self = (MeshVariable*)meshVariable;

	assert( self );

	/* We need to make sure the Mesh is built first, since this Variable and it's sub-components are going
	 *  to ask for the Mesh Size etc. */	
	Stg_Component_Build( self->mesh, data, False );

	_Variable_Build( self, data );
}

void _MeshVariable_Initialise( void* meshVariable, void* data ) {
	MeshVariable*	self = (MeshVariable*)meshVariable;

    Stg_Component_Initialise( self->mesh, data, False );
}

void _MeshVariable_Execute( void* meshVariable, void* data ) {
}

void _MeshVariable_Destroy( void* meshVariable, void* data ) {
   MeshVariable*	self = (MeshVariable*)meshVariable;

   Stg_Component_Destroy( self->mesh, data, False );

   _Variable_Destroy( self, data );
}

/*--------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/

void MeshVariable_SetMesh( void* meshVariable, void* _mesh ) {
	MeshVariable*	self = (MeshVariable*)meshVariable;
	Mesh*				mesh = (Mesh*)_mesh;

	assert( self );

	self->mesh = mesh;
	if( mesh )
		List_Append( mesh->vars, self );
}


/*----------------------------------------------------------------------------------------------------------------------------------
** Private Functions
*/

Index _MeshVariable_GetMeshArraySize( void* meshVariable ) {
	MeshVariable* self = (MeshVariable*)meshVariable;

	return Mesh_GetDomainSize( self->mesh, self->topoDim );
}



