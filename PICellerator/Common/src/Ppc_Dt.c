#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>
#include <float.h>

#include "types.h"
#include "Ppc_Dt.h"


/* Textual name of this class */
const Type Ppc_Dt_Type = "Ppc_Dt";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_Dt* _Ppc_Dt_New(  PPC_OPERATION_DEFARGS  )
{
	Ppc_Dt* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_Dt) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_Dt*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_Dt_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_Dt);
	Type                                                      type = Ppc_Dt_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_Dt_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_Dt_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_Dt_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_Dt_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_Dt_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_Dt_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_Dt_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_Dt_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_Dt_Get;

	return (void*)_Ppc_Dt_New(  PPC_OPERATION_PASSARGS  );
}


void _Ppc_Dt_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
  Ppc_Dt* self = (Ppc_Dt*)_self;
  
  /* Construct parent */
  _Ppc_AssignFromXML( self, cf, data );
  
}


void _Ppc_Dt_Build( void* _self, void* data ) {
  Ppc_Dt* self = (Ppc_Dt*)_self;

  /* Build parent */
  _Ppc_Build( self, data );
}

void _Ppc_Dt_Initialise( void* _self, void* data ) {
  Ppc_Dt* self = (Ppc_Dt*)_self;

  /* Initialize parent */
  _Ppc_Initialise( self, data );
}

void _Ppc_Dt_Delete( void* _self ) {
  Ppc_Dt* self = (Ppc_Dt*)_self;

  /* Delete parent */
  _Ppc_Delete( self );
}

void _Ppc_Dt_Print( void* _self, Stream* stream ) {
  Ppc_Dt* self = (Ppc_Dt*)_self;

  /* Print parent */
  _Ppc_Print( self, stream );
}

void _Ppc_Dt_Execute( void* _self, void* data ) {
  Ppc_Dt* self = (Ppc_Dt*)_self;

  /* Execute parent */
  _Ppc_Execute( self, data );
}

void _Ppc_Dt_Destroy( void* _self, void* data ) {
  Ppc_Dt* self = (Ppc_Dt*)_self;

  /* Destroy parent */
  _Ppc_Destroy( self, data );
}


/* 
 * Public functions 
 *
 */

int _Ppc_Dt_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
   Ppc_Dt* self = (Ppc_Dt*) _self;
   PpcManager* mgr = self->manager;

   *result = self->context->dt;

   return 0;
}
