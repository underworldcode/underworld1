#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_IsInsideShape.h"


/* Textual name of this class */
const Type Ppc_IsInsideShape_Type = "Ppc_IsInsideShape";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_IsInsideShape* _Ppc_IsInsideShape_New(  PPC_LINEAR_DEFARGS  )
{
	Ppc_IsInsideShape* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_IsInsideShape) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_IsInsideShape*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_IsInsideShape_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_IsInsideShape);
	Type                                                      type = Ppc_IsInsideShape_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_IsInsideShape_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_IsInsideShape_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_IsInsideShape_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_IsInsideShape_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_IsInsideShape_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_IsInsideShape_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_IsInsideShape_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_IsInsideShape_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_IsInsideShape_Get;

	return (void*)_Ppc_IsInsideShape_New(  PPC_LINEAR_PASSARGS  );
}

void _Ppc_IsInsideShape_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	Ppc_IsInsideShape* self = (Ppc_IsInsideShape*)_self;

	/* Construct parent */
	_Ppc_AssignFromXML( self, cf, data );

   self->shape = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Shape", Stg_Shape, True, data );
}


void _Ppc_IsInsideShape_Build( void* _self, void* data ) {
   Ppc_IsInsideShape* self = (Ppc_IsInsideShape*)_self;

	/* Build parent */
	_Ppc_Build( self, data );
   Stg_Component_Build( self->shape, NULL, False );
}

void _Ppc_IsInsideShape_Initialise( void* _self, void* data ) {
   Ppc_IsInsideShape* self = (Ppc_IsInsideShape*)_self;

	/* Initialize parent */
	_Ppc_Initialise( self, data );

   Stg_Component_Initialise( self->shape, NULL, False );
}

void _Ppc_IsInsideShape_Delete( void* _self ) {
   Ppc_IsInsideShape* self = (Ppc_IsInsideShape*)_self;

	/* Delete parent */
	_Ppc_Delete( self );
}

void _Ppc_IsInsideShape_Print( void* _self, Stream* stream ) {
   Ppc_IsInsideShape* self = (Ppc_IsInsideShape*)_self;

	/* Print parent */
	_Ppc_Print( self, stream );
}

void _Ppc_IsInsideShape_Execute( void* _self, void* data ) {
   Ppc_IsInsideShape* self = (Ppc_IsInsideShape*)_self;

	/* Execute parent */
	_Ppc_Execute( self, data );
}

void _Ppc_IsInsideShape_Destroy( void* _self, void* data ) {
   Ppc_IsInsideShape* self = (Ppc_IsInsideShape*)_self;

	/* Destroy parent */
	_Ppc_Destroy( self, data );
}

/* 
 * Public functions 
 *
 */

int _Ppc_IsInsideShape_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
   /*@
     returns 1 if a particle is found within the specified shape
     @*/

   Ppc_IsInsideShape* self = (Ppc_IsInsideShape*) _self;
   double globalCoord[3];
   int             err = 0;

   // get global coords
   FeMesh_CoordLocalToGlobal( self->manager->integrationSwarm->mesh, lElement_I, particle->xi, globalCoord );

   if( Stg_Shape_IsCoordInside( self->shape, globalCoord ) ) {
      result[0] = 1;
   } else {
      result[0] = 0;
   }

   return err;
}
