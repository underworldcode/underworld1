#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_Condition.h"


/* Textual name of this class */
const Type Ppc_Condition_Type = "Ppc_Condition";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_Condition* _Ppc_Condition_New(  PPC_CONDITION_DEFARGS  )
{
	Ppc_Condition* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_Condition) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_Condition*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_Condition_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_Condition);
	Type                                                      type = Ppc_Condition_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_Condition_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_Condition_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_Condition_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_Condition_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_Condition_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_Condition_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_Condition_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_Condition_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_Condition_Get;

	return (void*)_Ppc_Condition_New(  PPC_CONDITION_PASSARGS  );
}


void _Ppc_Condition_Init( void* _self, char* cond, int f, int tag1, int tag2, int tag3 )
{
   Ppc_Condition* self = (Ppc_Condition*)_self;

	/* Sanity check */
	Journal_Firewall( f != -1, 
							self->error_stream, "\n\n\n"
							"Error in configuration of Ppc_Condition %s\n"
							"Field not set"
							"\n\n\n", self->name );

	Journal_Firewall( tag1 != 0, 
							self->error_stream, "\n\n\n"
							"Error in configuration of Ppc_Condition %s\n"
							"ValueToCompare not set"
							"\n\n\n", self->name );

	Journal_Firewall( tag2 != 0, 
							self->error_stream, "\n\n\n"
							"Error in configuration of Ppc_Condition %s\n"
							"ValueIfTrue not set"
							"\n\n\n", self->name );

	Journal_Firewall( tag3 != 0, 
							self->error_stream, "\n\n\n"
							"Error in configuration of Ppc_Condition %s\n"
							"ValueIfFalse not set"
							"\n\n\n", self->name );

	Journal_Firewall( strcmp( cond, "eq" ) == 0 ||
							strcmp( cond, "ne" ) == 0 ||
							strcmp( cond, "l" ) == 0 ||
							strcmp( cond, "g" ) == 0 ||
							strcmp( cond, "le" ) == 0 ||
						   strcmp( cond, "ge" ) == 0, 
							self->error_stream, "\n\n\n"
							"Error in configuration of Ppc_Condition %s\n"
							"invalid condition %s\n"
							"valid options are: eq, ne, l, g, le, ge"
							"\n\n\n", self->name, cond );

	self->condition = cond;
	self->fieldTag = f;
	self->valueTag = tag1;
	self->valueTrueTag = tag2;
	self->valueFalseTag = tag3;
}


void _Ppc_Condition_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	Ppc_Condition* self = (Ppc_Condition*)_self;

	/* Construct parent */
	_Ppc_AssignFromXML( self, cf, data );

   _Ppc_Condition_Init( 
	   self, 
		Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"Condition", "" ), 
		PpcManager_GetPpcFromDict( self->manager, cf, self->name, (Dictionary_Entry_Key)"Field", "" ), 
		PpcManager_GetPpcFromDict( self->manager, cf, self->name, (Dictionary_Entry_Key)"ValueToCompare", "" ),
		PpcManager_GetPpcFromDict( self->manager, cf, self->name, (Dictionary_Entry_Key)"ValueIfTrue", "" ),
		PpcManager_GetPpcFromDict( self->manager, cf, self->name, (Dictionary_Entry_Key)"ValueIfFalse", "" ) );
}


void _Ppc_Condition_Build( void* _self, void* data ) {
   Ppc_Condition* self = (Ppc_Condition*)_self;

	/* Build parent */
	_Ppc_Build( self, data );
}

void _Ppc_Condition_Initialise( void* _self, void* data ) {
   Ppc_Condition* self = (Ppc_Condition*)_self;

	/* Initialize parent */
	_Ppc_Initialise( self, data );
}

void _Ppc_Condition_Delete( void* _self ) {
   Ppc_Condition* self = (Ppc_Condition*)_self;

	/* Delete parent */
	_Ppc_Delete( self );
}

void _Ppc_Condition_Print( void* _self, Stream* stream ) {
   Ppc_Condition* self = (Ppc_Condition*)_self;

	/* Print parent */
	_Ppc_Print( self, stream );
}

void _Ppc_Condition_Execute( void* _self, void* data ) {
   Ppc_Condition* self = (Ppc_Condition*)_self;

	/* Execute parent */
	_Ppc_Execute( self, data );
}

void _Ppc_Condition_Destroy( void* _self, void* data ) {
   Ppc_Condition* self = (Ppc_Condition*)_self;

	/* Destroy parent */
	_Ppc_Destroy( self, data );
}

/* 
 * Public functions 
 *
 */

int _Ppc_Condition_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
  Ppc_Condition* self = (Ppc_Condition*) _self;
  double value, fieldValue;
  char* op = self->condition;
  Bool conditionResult = False;
  int tag;
  int err;

  /* get field value */
  err = PpcManager_Get( self->manager, lElement_I, particle, self->fieldTag, &fieldValue );
  assert( !err );

  /* get value to compare  */
  err = PpcManager_Get( self->manager, lElement_I, particle, self->valueTag, &value );
  assert( !err );

  switch( op[0] ) {
  case 'e':
	 conditionResult = fieldValue == value;
	 break;
  case 'n':
	 conditionResult = fieldValue != value;
	 break;
  case 'l':
	 if( op[1] == 'e' )
		conditionResult = fieldValue <= value;
	 else
		conditionResult = fieldValue < value;
	 break;
  case 'g':
	 if( op[1] == 'e' )
		conditionResult = fieldValue >= value;
	 else
		conditionResult = fieldValue > value;
	 break;
  }

  tag = conditionResult ? self->valueTrueTag : self->valueFalseTag;

  err = PpcManager_Get( self->manager, lElement_I, particle, tag, result );
  
  return err;
}
