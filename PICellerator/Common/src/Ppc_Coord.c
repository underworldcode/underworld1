#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_Coord.h"


/* Textual name of this class */
const Type Ppc_Coord_Type = "Ppc_Coord";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_Coord* _Ppc_Coord_New(  PPC_CONSTANT_DEFARGS  )
{
   Ppc_Coord* self;

   assert( _sizeOfSelf >= sizeof(Ppc_Coord) );
   nameAllocationType = NON_GLOBAL;
   self = (Ppc_Coord*) _Ppc_New(  PPC_PASSARGS  );
   self->_get = _get;
   return self;
}


void* _Ppc_Coord_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof(Ppc_Coord);
   Type                                                      type = Ppc_Coord_Type;
   Stg_Class_DeleteFunction*                              _delete = _Ppc_Coord_Delete;
   Stg_Class_PrintFunction*                                _print = _Ppc_Coord_Print;
   Stg_Class_CopyFunction*                                  _copy = NULL;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_Coord_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _Ppc_Coord_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _Ppc_Coord_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _Ppc_Coord_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _Ppc_Coord_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _Ppc_Coord_Destroy;
   AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_Coord_Get;

   return (void*)_Ppc_Coord_New(  PPC_CONSTANT_PASSARGS  );
}


void _Ppc_Coord_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data )
{
   Ppc_Coord* self = (Ppc_Coord*)_self;

   /* Construct parent */
   _Ppc_AssignFromXML( self, cf, data );

   self->axis = Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"Axis", 0 );

}


void _Ppc_Coord_Build( void* _self, void* data )
{
   Ppc_Coord* self = (Ppc_Coord*)_self;

   /* Build parent */
   _Ppc_Build( self, data );

}

void _Ppc_Coord_Initialise( void* _self, void* data )
{
   Ppc_Coord* self = (Ppc_Coord*)_self;

   /* Initialize parent */
   _Ppc_Initialise( self, data );

}

void _Ppc_Coord_Delete( void* _self )
{
   Ppc_Coord* self = (Ppc_Coord*)_self;

   /* Delete parent */
   _Ppc_Delete( self );
}

void _Ppc_Coord_Print( void* _self, Stream* stream )
{
   Ppc_Coord* self = (Ppc_Coord*)_self;

   /* Print parent */
   _Ppc_Print( self, stream );
}

void _Ppc_Coord_Execute( void* _self, void* data )
{
   Ppc_Coord* self = (Ppc_Coord*)_self;

   /* Execute parent */
   _Ppc_Execute( self, data );
}

void _Ppc_Coord_Destroy( void* _self, void* data )
{
   Ppc_Coord* self = (Ppc_Coord*)_self;

   /* Destroy parent */
   _Ppc_Destroy( self, data );
}

/*
 * Public functions
 *
 */

int _Ppc_Coord_Get( void* _self, unsigned lElement_I, IntegrationPoint* particle, double* result )
{
   Ppc_Coord* self = (Ppc_Coord*) _self;
   Coord globalCoord;

   FeMesh_CoordLocalToGlobal( self->manager->mesh, lElement_I, particle->xi, globalCoord );

   result[0] =  globalCoord[self->axis];

   return 0;
}
