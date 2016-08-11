#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_Reciprocol.h"


/* Textual name of this class */
const Type Ppc_Reciprocol_Type = "Ppc_Reciprocol";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_Reciprocol* _Ppc_Reciprocol_New(  PPC_LINEAR_DEFARGS  )
{
	Ppc_Reciprocol* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_Reciprocol) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_Reciprocol*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_Reciprocol_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_Reciprocol);
	Type                                                      type = Ppc_Reciprocol_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_Reciprocol_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_Reciprocol_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_Reciprocol_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_Reciprocol_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_Reciprocol_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_Reciprocol_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_Reciprocol_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_Reciprocol_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_Reciprocol_Get;

	return (void*)_Ppc_Reciprocol_New(  PPC_LINEAR_PASSARGS  );
}


void _Ppc_Reciprocol_Init( void* _self, int alphaTag )
{
   Ppc_Reciprocol* self = (Ppc_Reciprocol*)_self;

	/* Sanity check */
	Journal_Firewall( alphaTag >= 0, 
							self->error_stream, "\n\n\n"
							"Error in configuration of Ppc_Reciprocol %s\n"
							"Alpha not set"
							"\n\n\n", self->name );

	self->alphaTag = alphaTag;
}


void _Ppc_Reciprocol_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	Ppc_Reciprocol* self = (Ppc_Reciprocol*)_self;
	Dictionary*	theDictionary;

	/* Construct parent */
	_Ppc_AssignFromXML( self, cf, data );

	/* The dictionary */ 
	theDictionary = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );

   _Ppc_Reciprocol_Init( 
      self, 
      PpcManager_GetPpcFromDict( self->manager, cf, self->name, (Dictionary_Entry_Key)"Alpha", "" ) );
}


void _Ppc_Reciprocol_Build( void* _self, void* data ) {
   Ppc_Reciprocol* self = (Ppc_Reciprocol*)_self;

	/* Build parent */
	_Ppc_Build( self, data );
}

void _Ppc_Reciprocol_Initialise( void* _self, void* data ) {
   Ppc_Reciprocol* self = (Ppc_Reciprocol*)_self;

	/* Initialize parent */
	_Ppc_Initialise( self, data );
}

void _Ppc_Reciprocol_Delete( void* _self ) {
   Ppc_Reciprocol* self = (Ppc_Reciprocol*)_self;

	/* Delete parent */
	_Ppc_Delete( self );
}

void _Ppc_Reciprocol_Print( void* _self, Stream* stream ) {
   Ppc_Reciprocol* self = (Ppc_Reciprocol*)_self;

	/* Print parent */
	_Ppc_Print( self, stream );
}

void _Ppc_Reciprocol_Execute( void* _self, void* data ) {
   Ppc_Reciprocol* self = (Ppc_Reciprocol*)_self;

	/* Execute parent */
	_Ppc_Execute( self, data );
}

void _Ppc_Reciprocol_Destroy( void* _self, void* data ) {
   Ppc_Reciprocol* self = (Ppc_Reciprocol*)_self;

	/* Destroy parent */
	_Ppc_Destroy( self, data );
}


Ppc_Reciprocol* Ppc_Reciprocol_New( Name name, Stg_Component* _self ) {
  Ppc_Reciprocol* self = (Ppc_Reciprocol*) _self;
  return self;
}

/* 
 * Public functions 
 *
 */

int _Ppc_Reciprocol_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
  Ppc_Reciprocol* self = (Ppc_Reciprocol*) _self;
  double alpha;
  int err;

  /* get alpha */
  err = PpcManager_Get( self->manager, lElement_I, particle, self->alphaTag, &alpha );
  assert( !err );

  result[0] = 1./alpha;
  
  return err;
}
