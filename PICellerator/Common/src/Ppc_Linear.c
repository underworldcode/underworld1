#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_Linear.h"


/* Textual name of this class */
const Type Ppc_Linear_Type = "Ppc_Linear";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_Linear* _Ppc_Linear_New(  PPC_LINEAR_DEFARGS  )
{
	Ppc_Linear* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_Linear) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_Linear*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_Linear_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_Linear);
	Type                                                      type = Ppc_Linear_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_Linear_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_Linear_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_Linear_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_Linear_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_Linear_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_Linear_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_Linear_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_Linear_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_Linear_Get;

	return (void*)_Ppc_Linear_New(  PPC_LINEAR_PASSARGS  );
}


void _Ppc_Linear_Init( void* _self, int alphaTag, int fieldTag, double rv )
{
   Ppc_Linear* self = (Ppc_Linear*)_self;

	/* Sanity check */
	Journal_Firewall( alphaTag != 0, 
							self->error_stream, "\n\n\n"
							"Error in configuration of Ppc_Linear %s\n"
							"Alpha not set"
							"\n\n\n", self->name );

	Journal_Firewall( fieldTag != 0, 
							self->error_stream, "\n\n\n"
							"Error in configuration of Ppc_Linear %s\n"
							"Field not set"
							"\n\n\n", self->name );
	
	self->alphaTag = alphaTag;
	self->fieldTag = fieldTag;
	self->referenceValue = rv;
}


void _Ppc_Linear_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	Ppc_Linear* self = (Ppc_Linear*)_self;
	Dictionary*	theDictionary;

	/* Construct parent */
	_Ppc_AssignFromXML( self, cf, data );

	/* The dictionary */ 
	theDictionary = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );

   _Ppc_Linear_Init( 
	   self, 
		PpcManager_GetPpcFromDict( self->manager, cf, self->name, (Dictionary_Entry_Key)"Alpha", "" ), 
		PpcManager_GetPpcFromDict( self->manager, cf, self->name, (Dictionary_Entry_Key)"Field", "" ), 
		Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ReferenceValue", 0.0 ) );							
}


void _Ppc_Linear_Build( void* _self, void* data ) {
   Ppc_Linear* self = (Ppc_Linear*)_self;

	/* Build parent */
	_Ppc_Build( self, data );
}

void _Ppc_Linear_Initialise( void* _self, void* data ) {
   Ppc_Linear* self = (Ppc_Linear*)_self;

	/* Initialize parent */
	_Ppc_Initialise( self, data );
}

void _Ppc_Linear_Delete( void* _self ) {
   Ppc_Linear* self = (Ppc_Linear*)_self;

	/* Delete parent */
	_Ppc_Delete( self );
}

void _Ppc_Linear_Print( void* _self, Stream* stream ) {
   Ppc_Linear* self = (Ppc_Linear*)_self;

	/* Print parent */
	_Ppc_Print( self, stream );
}

void _Ppc_Linear_Execute( void* _self, void* data ) {
   Ppc_Linear* self = (Ppc_Linear*)_self;

	/* Execute parent */
	_Ppc_Execute( self, data );
}

void _Ppc_Linear_Destroy( void* _self, void* data ) {
   Ppc_Linear* self = (Ppc_Linear*)_self;

	/* Destroy parent */
	_Ppc_Destroy( self, data );
}

/* 
 * Public functions 
 *
 */

int _Ppc_Linear_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
  Ppc_Linear* self = (Ppc_Linear*) _self;
  double alpha, referenceValue, fieldValue;
  int err;

  /* get reference value */
  referenceValue = self->referenceValue;

  /* get field value */
  err = PpcManager_Get( self->manager, lElement_I, particle, self->fieldTag, &fieldValue );
  assert( !err );

  /* get alpha */
  err = PpcManager_Get( self->manager, lElement_I, particle, self->alphaTag, &alpha );
  assert( !err );

  result[0] = alpha*(fieldValue - referenceValue);
  
  return err;
}
