#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_IsMaterial.h"


/* Textual name of this class */
const Type Ppc_IsMaterial_Type = "Ppc_IsMaterial";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_IsMaterial* _Ppc_IsMaterial_New(  PPC_LINEAR_DEFARGS  )
{
	Ppc_IsMaterial* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_IsMaterial) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_IsMaterial*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_IsMaterial_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_IsMaterial);
	Type                                                      type = Ppc_IsMaterial_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_IsMaterial_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_IsMaterial_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_IsMaterial_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_IsMaterial_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_IsMaterial_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_IsMaterial_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_IsMaterial_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_IsMaterial_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_IsMaterial_Get;

	return (void*)_Ppc_IsMaterial_New(  PPC_LINEAR_PASSARGS  );
}

void _Ppc_IsMaterial_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	Ppc_IsMaterial* self = (Ppc_IsMaterial*)_self;

	/* Construct parent */
	_Ppc_AssignFromXML( self, cf, data );

   self->materialName = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"MaterialName", ""  );

	/* Sanity check */
	Journal_Firewall( strcmp("",self->materialName), 
							self->error_stream,
							"Error in configuration of Ppc_IsMaterial %s.\n"
							"No material name provided.",
							self->name );

}


void _Ppc_IsMaterial_Build( void* _self, void* data ) {
   Ppc_IsMaterial* self = (Ppc_IsMaterial*)_self;

	/* Build parent */
	_Ppc_Build( self, data );
}

void _Ppc_IsMaterial_Initialise( void* _self, void* data ) {
   Ppc_IsMaterial* self = (Ppc_IsMaterial*)_self;
   Material*       material;

	/* Initialize parent */
	_Ppc_Initialise( self, data );

   /* now get the material index */
   material = Materials_Register_GetByName( self->manager->materialSwarm->materials_Register, self->materialName );
	
   Journal_Firewall( material, 
							self->error_stream,
							"Error in configuration of Ppc_IsMaterial %s.\n"
							"No material with name %s found.",
							self->name, self->materialName);
   self->materialIndex = material->index; 

}

void _Ppc_IsMaterial_Delete( void* _self ) {
   Ppc_IsMaterial* self = (Ppc_IsMaterial*)_self;

	/* Delete parent */
   Memory_Free( self->materialName );
	_Ppc_Delete( self );
}

void _Ppc_IsMaterial_Print( void* _self, Stream* stream ) {
   Ppc_IsMaterial* self = (Ppc_IsMaterial*)_self;

	/* Print parent */
	_Ppc_Print( self, stream );
}

void _Ppc_IsMaterial_Execute( void* _self, void* data ) {
   Ppc_IsMaterial* self = (Ppc_IsMaterial*)_self;

	/* Execute parent */
	_Ppc_Execute( self, data );
}

void _Ppc_IsMaterial_Destroy( void* _self, void* data ) {
   Ppc_IsMaterial* self = (Ppc_IsMaterial*)_self;

	/* Destroy parent */
	_Ppc_Destroy( self, data );
}

/* 
 * Public functions 
 *
 */

int _Ppc_IsMaterial_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
   Ppc_IsMaterial* self = (Ppc_IsMaterial*) _self;
   Material*       material;
   int             err = 0;

   /* Get the material */
   material = IntegrationPointsSwarm_GetMaterialOn( (IntegrationPointsSwarm*) self->manager->integrationSwarm, particle );

   result[0] = 0;
   if( material->index == self->materialIndex ) 
      result[0] = 1;
   return err;
}
