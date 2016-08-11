#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_a_Constant.h"


/* Textual name of this class */
const Type Ppc_a_Constant_Type = "Ppc_a_Constant";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_a_Constant* _Ppc_a_Constant_New(  PPC_LINEAR_DEFARGS  )
{
	Ppc_a_Constant* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_a_Constant) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_a_Constant*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_a_Constant_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_a_Constant);
	Type                                                      type = Ppc_a_Constant_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_a_Constant_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_a_Constant_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_a_Constant_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_a_Constant_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_a_Constant_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_a_Constant_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_a_Constant_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_a_Constant_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_a_Constant_Get;

	return (void*)_Ppc_a_Constant_New(  PPC_LINEAR_PASSARGS  );
}


void _Ppc_a_Constant_Init( void* _self, int alphaTag, double constant )
{
   Ppc_a_Constant* self = (Ppc_a_Constant*)_self;

	/* Sanity check */
	Journal_Firewall( alphaTag >= 0, 
							self->error_stream, "\n\n\n"
							"Error in configuration of Ppc_a_Constant %s\n"
							"Alpha not set"
							"\n\n\n", self->name );

	self->alphaTag = alphaTag;
	self->constant = constant;
}


void _Ppc_a_Constant_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	Ppc_a_Constant* self = (Ppc_a_Constant*)_self;
	Dictionary*	theDictionary;

	/* Construct parent */
	_Ppc_AssignFromXML( self, cf, data );

	/* The dictionary */ 
	theDictionary = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );

   _Ppc_a_Constant_Init( 
      self, 
      PpcManager_GetPpcFromDict( self->manager, cf, self->name, (Dictionary_Entry_Key)"Alpha", "" ), 
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"constant", 1.0 ) );
}


void _Ppc_a_Constant_Build( void* _self, void* data ) {
   Ppc_a_Constant* self = (Ppc_a_Constant*)_self;

	/* Build parent */
	_Ppc_Build( self, data );
}

void _Ppc_a_Constant_Initialise( void* _self, void* data ) {
   Ppc_a_Constant* self = (Ppc_a_Constant*)_self;

	/* Initialize parent */
	_Ppc_Initialise( self, data );
}

void _Ppc_a_Constant_Delete( void* _self ) {
   Ppc_a_Constant* self = (Ppc_a_Constant*)_self;

	/* Delete parent */
	_Ppc_Delete( self );
}

void _Ppc_a_Constant_Print( void* _self, Stream* stream ) {
   Ppc_a_Constant* self = (Ppc_a_Constant*)_self;

	/* Print parent */
	_Ppc_Print( self, stream );
}

void _Ppc_a_Constant_Execute( void* _self, void* data ) {
   Ppc_a_Constant* self = (Ppc_a_Constant*)_self;

	/* Execute parent */
	_Ppc_Execute( self, data );
}

void _Ppc_a_Constant_Destroy( void* _self, void* data ) {
   Ppc_a_Constant* self = (Ppc_a_Constant*)_self;

	/* Destroy parent */
	_Ppc_Destroy( self, data );
}


/* 
 * Public functions 
 *
 */

int _Ppc_a_Constant_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
  Ppc_a_Constant* self = (Ppc_a_Constant*) _self;
  double alpha;
  int err;

  /* get alpha */
  err = PpcManager_Get( self->manager, lElement_I, particle, self->alphaTag, &alpha );
  assert( !err );

  result[0] = alpha*self->constant;
  
  return err;
}
