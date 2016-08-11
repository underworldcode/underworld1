#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_Polynomial.h"


/* Textual name of this class */
const Type Ppc_Polynomial_Type = "Ppc_Polynomial";


Bool isInteger( double x ) {
  return x == (double)floor( x );
}

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_Polynomial* _Ppc_Polynomial_New(  PPC_POLYNOMIAL_DEFARGS  )
{
	Ppc_Polynomial* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_Polynomial) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_Polynomial*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_Polynomial_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_Polynomial);
	Type                                                      type = Ppc_Polynomial_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_Polynomial_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_Polynomial_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_Polynomial_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_Polynomial_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_Polynomial_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_Polynomial_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_Polynomial_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_Polynomial_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_Polynomial_Get;

	return (void*)_Ppc_Polynomial_New(  PPC_POLYNOMIAL_PASSARGS  );
}


void _Ppc_Polynomial_Init( void* _self, int f, double rv ) {
   Ppc_Polynomial* self = (Ppc_Polynomial*)_self;

	Journal_Firewall( f != -1, 
							self->error_stream, "\n\n\n"
							"Error in configuration of Ppc_Polynomial %s\n"
							"Field not set"
							"\n\n\n", self->name );

	self->fieldTag = f;
	self->referenceValue = rv;
}


void _Ppc_Polynomial_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
  Ppc_Polynomial* self = (Ppc_Polynomial*)_self;
  Dictionary*	dictionary;
  Dictionary_Entry_Value* termList = NULL;
  Dictionary_Entry_Value* term = NULL;
  Index term_I;

  /* Construct parent */
  _Ppc_AssignFromXML( self, cf, data );

  /* The dictionary */ 
  dictionary = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );

  /* get coeff & powers */
  termList = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"Terms" );

  Journal_Firewall( (Bool) termList, 
						  self->error_stream, "\n\n\n"
						  "Error in the configuration of Ppc_Polynomial %s\n"
						  "You must define the coefficients and power table."
						  "\n\n\n", self->name );

  self->termsCount = Dictionary_Entry_Value_GetCount( termList );
  /* get mem */
  self->coefficient = Memory_Alloc_Array( double, self->termsCount, "coeff table" );
  self->power = Memory_Alloc_Array( double, self->termsCount, "power table" );
  memset( self->coefficient, 0, self->termsCount * sizeof(double) );
  memset( self->power, 0, self->termsCount * sizeof(double) );
  
  for ( term_I = 0 ; term_I < self->termsCount ; term_I++) { 
	 term = Dictionary_Entry_Value_GetElement( termList, term_I );
	 self->coefficient[term_I] = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( term, (Dictionary_Entry_Key)"Coefficient" ) );
	 self->power[term_I] = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( term, (Dictionary_Entry_Key)"Power" ) );
  }

  _Ppc_Polynomial_Init( 
     self, 
	  PpcManager_GetPpcFromDict( self->manager, cf, (Name)self->name, "Field", "" ),
	  Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ReferenceValue", 0.0 ) );							
}


void _Ppc_Polynomial_Build( void* _self, void* data ) {
   Ppc_Polynomial* self = (Ppc_Polynomial*)_self;

	/* Build parent */
	_Ppc_Build( self, data );
}

void _Ppc_Polynomial_Initialise( void* _self, void* data ) {
   Ppc_Polynomial* self = (Ppc_Polynomial*)_self;

	/* Initialize parent */
	_Ppc_Initialise( self, data );
}

void _Ppc_Polynomial_Delete( void* _self ) {
   Ppc_Polynomial* self = (Ppc_Polynomial*)_self;

	/* Delete parent */
	_Ppc_Delete( self );
}

void _Ppc_Polynomial_Print( void* _self, Stream* stream ) {
   Ppc_Polynomial* self = (Ppc_Polynomial*)_self;

	/* Print parent */
	_Ppc_Print( self, stream );
}

void _Ppc_Polynomial_Execute( void* _self, void* data ) {
   Ppc_Polynomial* self = (Ppc_Polynomial*)_self;

	/* Execute parent */
	_Ppc_Execute( self, data );
}

void _Ppc_Polynomial_Destroy( void* _self, void* data ) {
   Ppc_Polynomial* self = (Ppc_Polynomial*)_self;

	/* Destroy parent */
	_Ppc_Destroy( self, data );
}

/* 
 * Public functions 
 *
 */

int _Ppc_Polynomial_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
  Ppc_Polynomial* self = (Ppc_Polynomial*) _self;
  double referenceValue, fieldValue, coeff, power, x;
  int I, err;

  /* get reference value */
  referenceValue = self->referenceValue;

  /* get field value */
  err = PpcManager_Get( self->manager, lElement_I, particle, self->fieldTag, &fieldValue );
  assert( !err );

  /* compute result */
  result[0] = 0;
  for( I = 0 ; I < self->termsCount ; I++ ) {
	 coeff = self->coefficient[I];
	 power = self->power[I];
	 x = fieldValue - referenceValue;

	 Journal_Firewall( !(x < 0 && !isInteger( power )) , 
							 self->error_stream, "\n\n\n"
							 "Error in configuration of Ppc_Polynomial %s\n"
							 "The term number %i is invalid: %g * %g^%g\n"
							 "You cannot raise a negative number to an non-integer number."
							 "\n\n\n", self->name, I, coeff, x, power );

	 if( x != 0 ) {
		if( x == 1 )
		  result[0] += coeff;
		else
		  result[0] += coeff * pow( x, power );
	 }
  }

  return 0;
}
