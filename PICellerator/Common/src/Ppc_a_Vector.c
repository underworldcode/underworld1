#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_a_Vector.h"


/* Textual name of this class */
const Type Ppc_a_Vector_Type = "Ppc_a_Vector";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_a_Vector* _Ppc_a_Vector_New(  PPC_LINEAR_DEFARGS  )
{
	Ppc_a_Vector* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_a_Vector) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_a_Vector*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_a_Vector_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_a_Vector);
	Type                                                      type = Ppc_a_Vector_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_a_Vector_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_a_Vector_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_a_Vector_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_a_Vector_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_a_Vector_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_a_Vector_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_a_Vector_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_a_Vector_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_a_Vector_Get;

	return (void*)_Ppc_a_Vector_New(  PPC_LINEAR_PASSARGS  );
}


void _Ppc_a_Vector_Init( void* _self, int alphaTag, int vectorTag, double vi, double vj, double vk )
{
   Ppc_a_Vector* self = (Ppc_a_Vector*)_self;

	/* Sanity check */
	Journal_Firewall( alphaTag >= 0, 
							self->error_stream, "\n\n\n"
							"Error in configuration of Ppc_a_Vector %s\n"
							"Alpha not set"
							"\n\n\n", self->name );

	self->alphaTag = alphaTag;
	self->vectorTag = vectorTag;
	self->vector[0] = vi;
	self->vector[1] = vj;
	self->vector[2] = vk;
}


void _Ppc_a_Vector_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	Ppc_a_Vector* self = (Ppc_a_Vector*)_self;
	Dictionary*	theDictionary;

	/* Construct parent */
	_Ppc_AssignFromXML( self, cf, data );

	/* The dictionary */ 
	theDictionary = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );

   _Ppc_a_Vector_Init( 
      self, 
      PpcManager_GetPpcFromDict( self->manager, cf, self->name, (Dictionary_Entry_Key)"Alpha", "" ), 
      PpcManager_GetPpcFromDict( self->manager, cf, self->name, (Dictionary_Entry_Key)"Vector", "" ), 
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"vi", 0.0 ),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"vj", 1.0 ),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"vk", 0.0 ) );
}


void _Ppc_a_Vector_Build( void* _self, void* data ) {
   Ppc_a_Vector* self = (Ppc_a_Vector*)_self;

	/* Build parent */
	_Ppc_Build( self, data );
}

void _Ppc_a_Vector_Initialise( void* _self, void* data ) {
   Ppc_a_Vector* self = (Ppc_a_Vector*)_self;

	/* Initialize parent */
	_Ppc_Initialise( self, data );
}

void _Ppc_a_Vector_Delete( void* _self ) {
   Ppc_a_Vector* self = (Ppc_a_Vector*)_self;

	/* Delete parent */
	_Ppc_Delete( self );
}

void _Ppc_a_Vector_Print( void* _self, Stream* stream ) {
   Ppc_a_Vector* self = (Ppc_a_Vector*)_self;

	/* Print parent */
	_Ppc_Print( self, stream );
}

void _Ppc_a_Vector_Execute( void* _self, void* data ) {
   Ppc_a_Vector* self = (Ppc_a_Vector*)_self;

	/* Execute parent */
	_Ppc_Execute( self, data );
}

void _Ppc_a_Vector_Destroy( void* _self, void* data ) {
   Ppc_a_Vector* self = (Ppc_a_Vector*)_self;

	/* Destroy parent */
	_Ppc_Destroy( self, data );
}


/* 
 * Public functions 
 *
 */

int _Ppc_a_Vector_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
  Ppc_a_Vector* self = (Ppc_a_Vector*) _self;
  double alpha, *vptr;
  double vector[3];
  int err, haveVec;

  vptr = &(self->vector[0]);

  /* get alpha */
  err = PpcManager_Get( self->manager, lElement_I, particle, self->alphaTag, &alpha );
  assert( !err );
  /* get vector if provided */
  haveVec = PpcManager_Get( self->manager, lElement_I, particle, self->vectorTag, &vector[0] );
 
  if( haveVec == -1){
     result[0] = alpha*(vptr[0]);
     result[1] = alpha*(vptr[1]);
     result[2] = alpha*(vptr[2]);
  } else {
     result[0] = alpha*vector[0];
     result[1] = alpha*vector[1];
     result[2] = alpha*vector[2];
  }
  
  return err;
}
