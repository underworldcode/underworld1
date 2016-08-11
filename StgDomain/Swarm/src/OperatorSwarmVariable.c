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

#include <string.h>
#include <mpi.h>
#include <StGermain/StGermain.h>

#include <StgDomain/Geometry/Geometry.h>
#include <StgDomain/Shape/Shape.h>
#include <StgDomain/Mesh/Mesh.h>
#include <StgDomain/Utils/Utils.h>

#include "types.h"
#include "SwarmVariable_Register.h"
#include "SwarmVariable.h"
#include "OperatorSwarmVariable.h"
#include "SwarmClass.h"

#include <assert.h>

const Type OperatorSwarmVariable_Type = "OperatorSwarmVariable";
const Name defaultOperatorSwarmVariableName = "defaultOperatorSwarmVariableName";

OperatorSwarmVariable* OperatorSwarmVariable_NewUnary( 
	Name					name,
	AbstractContext*	context,
	void*					_swarmVariable,
	Name					operatorName )
{
	SwarmVariable* swarmVariable = (SwarmVariable*) _swarmVariable;
       	
	return OperatorSwarmVariable_New( 
		name,
		context,
		_OperatorSwarmVariable_UnaryValueAt, 
		operatorName,
		1,
		&swarmVariable );
}

OperatorSwarmVariable* OperatorSwarmVariable_NewBinary( 
	Name					name,
	AbstractContext*	context,
	void*					_swarmVariable1,
	void*					_swarmVariable2,
	Name					operatorName )
{
	SwarmVariable* swarmVariableList[2];
       
	swarmVariableList[0] = (SwarmVariable*) _swarmVariable1;
	swarmVariableList[1] = (SwarmVariable*) _swarmVariable2;
	
	return OperatorSwarmVariable_New( 
		name,
		context,
		_OperatorSwarmVariable_BinaryValueAt, 
		operatorName,
		2, 
		swarmVariableList );
}

OperatorSwarmVariable* OperatorSwarmVariable_New( 
	Name										name,
	AbstractContext*						context,
	SwarmVariable_ValueAtFunction*	_valueAt,
	Name										operatorName,
	Index										swarmVariableCount,
	SwarmVariable**						swarmVariableList )
{
	OperatorSwarmVariable* self = _OperatorSwarmVariable_DefaultNew( name );

	self->isConstructed = True;
	_SwarmVariable_Init( (SwarmVariable*)self, context, swarmVariableList[0]->swarm, NULL, 0, False, False);
	_OperatorSwarmVariable_Init( self, operatorName, swarmVariableCount, swarmVariableList );

	return self;
}

void* _OperatorSwarmVariable_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                 _sizeOfSelf = sizeof(OperatorSwarmVariable);
	Type                                                         type = OperatorSwarmVariable_Type;
	Stg_Class_DeleteFunction*                                 _delete = _OperatorSwarmVariable_Delete;
	Stg_Class_PrintFunction*                                   _print = _OperatorSwarmVariable_Print;
	Stg_Class_CopyFunction*                                     _copy = _OperatorSwarmVariable_Copy;
	Stg_Component_DefaultConstructorFunction*     _defaultConstructor = _OperatorSwarmVariable_DefaultNew;
	Stg_Component_ConstructFunction*                       _construct = _OperatorSwarmVariable_AssignFromXML;
	Stg_Component_BuildFunction*                               _build = _OperatorSwarmVariable_Build;
	Stg_Component_InitialiseFunction*                     _initialise = _OperatorSwarmVariable_Initialise;
	Stg_Component_ExecuteFunction*                           _execute = _OperatorSwarmVariable_Execute;
	Stg_Component_DestroyFunction*                           _destroy = _OperatorSwarmVariable_Destroy;
	AllocationType                                 nameAllocationType = NON_GLOBAL;
	SwarmVariable_ValueAtFunction*                           _valueAt = _OperatorSwarmVariable_ValueAt;
	SwarmVariable_GetGlobalValueFunction*      _getMinGlobalMagnitude = _OperatorSwarmVariable_GetMinGlobalMagnitude;
	SwarmVariable_GetGlobalValueFunction*      _getMaxGlobalMagnitude = _OperatorSwarmVariable_GetMaxGlobalMagnitude;

	return (void*) _OperatorSwarmVariable_New(  OPERATORSWARMVARIABLE_PASSARGS  );
}

OperatorSwarmVariable* _OperatorSwarmVariable_New(  OPERATORSWARMVARIABLE_DEFARGS  ) {
	OperatorSwarmVariable* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(OperatorSwarmVariable) );
	self = (OperatorSwarmVariable*) _SwarmVariable_New(  SWARMVARIABLE_PASSARGS  );
	
	return self;
}

void _OperatorSwarmVariable_Delete( void* _swarmVariable ) {
	OperatorSwarmVariable* self = (OperatorSwarmVariable*) _swarmVariable;

	_SwarmVariable_Delete( self );
}

void _OperatorSwarmVariable_Print( void* _swarmVariable, Stream* stream ) {
	OperatorSwarmVariable* self = (OperatorSwarmVariable*) _swarmVariable;
	Index                  swarmVariable_I;

	_SwarmVariable_Print( self, stream );

	Journal_PrintValue( stream, self->swarmVariableCount );
	for ( swarmVariable_I = 0 ; swarmVariable_I < self->swarmVariableCount ; swarmVariable_I++ ) 
		Journal_Printf( stream, "\tSwarmVariable %u - '%s'\n", swarmVariable_I, self->swarmVariableList[ swarmVariable_I ]->name );

}

void _OperatorSwarmVariable_Init( void* _swarmVariable, Name operatorName, Index swarmVariableCount, SwarmVariable** swarmVariableList ) {
	OperatorSwarmVariable*	self = (OperatorSwarmVariable*)_swarmVariable;
	SwarmVariable*				swarmVariable;
	Index							swarmVariable_I;
	Stream*						errorStream = Journal_Register( Error_Type, (Name)self->type  );

	self->_operator = Operator_NewFromName( operatorName, swarmVariableList[0]->dofCount, self->dim );
	self->dofCount = self->_operator->resultDofs; /* reset value */
	self->swarmVariableCount = swarmVariableCount;
   /* variable does not store data, so is not checkpointed */
   self->isCheckpointedAndReloaded = False;

	/* Copy swarm variable list */
	self->swarmVariableList      = Memory_Alloc_Array( SwarmVariable*, swarmVariableCount, "Array of Swarm Variables" );
	memcpy( self->swarmVariableList, swarmVariableList, swarmVariableCount * sizeof( SwarmVariable* ) );

	for ( swarmVariable_I = 0 ; swarmVariable_I < swarmVariableCount ; swarmVariable_I++ ) {
		swarmVariable = swarmVariableList[ swarmVariable_I ];
		Journal_Firewall( swarmVariable != NULL, errorStream, 
			"In func %s: SwarmVariable %u in list is NULL\n", __func__, swarmVariable_I );
		Journal_Firewall( swarmVariable->dofCount <= MAX_DOF, errorStream, 
			"In func %s: Swarm Variable '%s' has too many components.\n", __func__, swarmVariable->name );
	}
}

void* _OperatorSwarmVariable_Copy( void* swarmVariable, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
	OperatorSwarmVariable*	self = (OperatorSwarmVariable*)swarmVariable;
	OperatorSwarmVariable*	newOperatorSwarmVariable;
	
	newOperatorSwarmVariable = _SwarmVariable_Copy( self, dest, deep, nameExt, ptrMap );
	
	newOperatorSwarmVariable->_operator              = self->_operator;
	newOperatorSwarmVariable->swarmVariableCount     = self->swarmVariableCount;
	
	if (deep) {
		newOperatorSwarmVariable->swarmVariableList = Memory_Alloc_Array( SwarmVariable*, self->swarmVariableCount, 
				"Array of Swarm Variables" );
		memcpy( newOperatorSwarmVariable->swarmVariableList, self->swarmVariableList, 
				self->swarmVariableCount * sizeof( SwarmVariable* ) );
	}
	else 
		newOperatorSwarmVariable->swarmVariableList = self->swarmVariableList;
	
	return (void*)newOperatorSwarmVariable;
}

void _OperatorSwarmVariable_AssignFromXML( void* swarmVariable, Stg_ComponentFactory* cf, void* data ) {
	OperatorSwarmVariable*  self       = (OperatorSwarmVariable*) swarmVariable;
	Dictionary*             dictionary = Dictionary_GetDictionary( cf->componentDict, self->name );
	Dictionary_Entry_Value* list;
	Index                   swarmVariableCount = 0;
	Index                   swarmVariable_I;
	Name                    swarmVariableName;
	Name                    operatorName;
	SwarmVariable**         swarmVariableList;
	SwarmVariable_Register* swarmVariable_Register;

	/* Call parent's construct function */
	_SwarmVariable_AssignFromXML( self, cf, data );
	swarmVariable_Register = self->swarm->swarmVariable_Register;

	operatorName = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"Operator", ""  );

	list = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"SwarmVariables" );

	swarmVariableCount = ( list ? Dictionary_Entry_Value_GetCount(list) : 1  );
	swarmVariableList = Memory_Alloc_Array( SwarmVariable*, swarmVariableCount, "SwarmVars" );

	for ( swarmVariable_I = 0 ; swarmVariable_I < swarmVariableCount ; swarmVariable_I++ ) {
		swarmVariableName = (list ? 
				Dictionary_Entry_Value_AsString( Dictionary_Entry_Value_GetElement( list, swarmVariable_I ) ) :
				Dictionary_GetString( dictionary, (Dictionary_Entry_Key)"SwarmVariable" )  );

		/* Check in swarmVariable_Register first before assuming in LiveComponentRegister */
		Journal_PrintfL( cf->infoStream, 2, "Looking for SwarmVariable '%s' in swarmVariable_Register.\n",
				swarmVariableName );
		swarmVariableList[swarmVariable_I] = (SwarmVariable*) 
			SwarmVariable_Register_GetByName( swarmVariable_Register, swarmVariableName );
		
		if ( !swarmVariableList[swarmVariable_I] )
			swarmVariableList[swarmVariable_I] = 
				Stg_ComponentFactory_ConstructByName( cf, (Name)swarmVariableName, SwarmVariable, True, data  );
	}

	_SwarmVariable_AssignFromXML( self, cf, data );
	_OperatorSwarmVariable_Init( self, operatorName, swarmVariableCount, swarmVariableList );

	Memory_Free( swarmVariableList );
}

void _OperatorSwarmVariable_Build( void* swarmVariable, void* data ) {
	OperatorSwarmVariable* self = (OperatorSwarmVariable*) swarmVariable;
	Index                  swarmVariable_I;

	for ( swarmVariable_I = 0 ; swarmVariable_I < self->swarmVariableCount ; swarmVariable_I++ ) 
		Stg_Component_Build( self->swarmVariableList[ swarmVariable_I ] , data, False );
}

void _OperatorSwarmVariable_Execute( void* swarmVariable, void* data ) {}

void _OperatorSwarmVariable_Destroy( void* swarmVariable, void* data ) {
	OperatorSwarmVariable* self = (OperatorSwarmVariable*)swarmVariable;

	Memory_Free( self->swarmVariableList );

	_SwarmVariable_Destroy( self, data );
}

void _OperatorSwarmVariable_Initialise( void* swarmVariable, void* data ) {
	OperatorSwarmVariable* self = (OperatorSwarmVariable*) swarmVariable;
	Index                  swarmVariable_I;

	for ( swarmVariable_I = 0 ; swarmVariable_I < self->swarmVariableCount ; swarmVariable_I++ ) 
		Stg_Component_Initialise( self->swarmVariableList[ swarmVariable_I ] , data, False );
}

double _OperatorSwarmVariable_GetMinGlobalMagnitude( void* swarmVariable ) { 
	/* Just use particle function */
	return _SwarmVariable_GetMinGlobalMagnitude( swarmVariable); 
}
double _OperatorSwarmVariable_GetMaxGlobalMagnitude( void* swarmVariable ) {
	/* Just use particle function */
	return _SwarmVariable_GetMaxGlobalMagnitude( swarmVariable); 
}

void _OperatorSwarmVariable_ValueAt( void* swarmVariable, Particle_Index lParticle_I, double* value ) {
	OperatorSwarmVariable* self            = (OperatorSwarmVariable*) swarmVariable;

	switch ( self->swarmVariableCount ) {
		case 1:
			self->_valueAt = _OperatorSwarmVariable_UnaryValueAt; break;
		case 2:
			self->_valueAt = _OperatorSwarmVariable_BinaryValueAt; break;
		default:
			Journal_Firewall( False, Journal_Register( Error_Type, (Name)self->type  ),
					"Can't use func '%s' with swarmVariableCount = %d\n", __func__, self->swarmVariableCount );
	}

	SwarmVariable_ValueAt( self, lParticle_I, value );
}

void _OperatorSwarmVariable_UnaryValueAt( void* swarmVariable, Particle_Index lParticle_I, double* value ) {
	OperatorSwarmVariable* self            = (OperatorSwarmVariable*) swarmVariable;
	SwarmVariable*         swarm0          = self->swarmVariableList[0];
	double                 swarmValue[ MAX_DOF ]; 

	SwarmVariable_ValueAt( swarm0, lParticle_I, swarmValue );
	Operator_CarryOutUnaryOperation( self->_operator, swarmValue, value );
}

void _OperatorSwarmVariable_BinaryValueAt( void* swarmVariable, Particle_Index lParticle_I, double* value ) {
	OperatorSwarmVariable* self            = (OperatorSwarmVariable*) swarmVariable;
	SwarmVariable*         swarm0          = self->swarmVariableList[0];
	SwarmVariable*         swarm1          = self->swarmVariableList[1];
	double                 swarmValue0[ MAX_DOF ]; 
	double                 swarmValue1[ MAX_DOF ]; 

	SwarmVariable_ValueAt( swarm0, lParticle_I, swarmValue0 );
	SwarmVariable_ValueAt( swarm1, lParticle_I, swarmValue1 );

	Operator_CarryOutBinaryOperation( self->_operator, swarmValue0, swarmValue1, value ); 
}


