#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_Constant.h"


/* Textual name of this class */
const Type Ppc_Constant_Type = "Ppc_Constant";


void _Ppc_Constant_Init( Ppc_Constant* self, double value )
{
  Journal_Firewall( value != HUGE_VAL, 
						  self->error_stream, "\n\n\n"
						  "Error in configuration of Ppc_Constant <%s>\n"
						  "value not set"
						  "\n\n\n", self->name );

	self->value = value;
}


/** Public constructor, simple interface */
Ppc_Constant* Ppc_Constant_New(  Name name,
                                 PpcManager* mgr,
                                 double value ) {

   Ppc_Constant* self = _Ppc_Constant_DefaultNew( name );

   self->isConstructed = True;
   _Ppc_Init( self, mgr, HUGE_VAL, HUGE_VAL, 0 );
   _Ppc_Constant_Init( self, value );

   LiveComponentRegister_IfRegThenAdd( (Stg_Component*)self );

   return self;
}


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_Constant* _Ppc_Constant_New(  PPC_CONSTANT_DEFARGS  )
{
	Ppc_Constant* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_Constant) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_Constant*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_Constant_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_Constant);
	Type                                                      type = Ppc_Constant_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_Constant_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_Constant_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_Constant_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_Constant_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_Constant_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_Constant_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_Constant_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_Constant_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_Constant_Get;

	return (void*)_Ppc_Constant_New(  PPC_CONSTANT_PASSARGS  );
}


void _Ppc_Constant_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	Ppc_Constant* self = (Ppc_Constant*)_self;
	Dictionary*	theDictionary;

	/* Construct parent */
	_Ppc_AssignFromXML( self, cf, data );

	/* The dictionary */ 
	theDictionary = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );
							
	/* Init */
	_Ppc_Constant_Init( 
      self, 
		Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"Value", HUGE_VAL ) );
}


void _Ppc_Constant_Build( void* _self, void* data ) {
   Ppc_Constant* self = (Ppc_Constant*)_self;

	/* Build parent */
	_Ppc_Build( self, data );
}

void _Ppc_Constant_Initialise( void* _self, void* data ) {
   Ppc_Constant* self = (Ppc_Constant*)_self;

	/* Initialize parent */
	_Ppc_Initialise( self, data );
}

void _Ppc_Constant_Delete( void* _self ) {
   Ppc_Constant* self = (Ppc_Constant*)_self;

	/* Delete parent */
	_Ppc_Delete( self );
}

void _Ppc_Constant_Print( void* _self, Stream* stream ) {
   Ppc_Constant* self = (Ppc_Constant*)_self;

	/* Print parent */
	_Ppc_Print( self, stream );
}

void _Ppc_Constant_Execute( void* _self, void* data ) {
   Ppc_Constant* self = (Ppc_Constant*)_self;

	/* Execute parent */
	_Ppc_Execute( self, data );
}

void _Ppc_Constant_Destroy( void* _self, void* data ) {
   Ppc_Constant* self = (Ppc_Constant*)_self;

	/* Destroy parent */
	_Ppc_Destroy( self, data );
}


/* 
 * Public functions 
 *
 */

int _Ppc_Constant_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
	Ppc_Constant* self = (Ppc_Constant*) _self;

	result[0] =  self->value;

	return 0;
}
