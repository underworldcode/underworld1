#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_LinearDensity.h"


/* Textual name of this class */
const Type Ppc_LinearDensity_Type = "Ppc_LinearDensity";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_LinearDensity* _Ppc_LinearDensity_New(  PPC_LINEARDENSITY_DEFARGS  )
{
	Ppc_LinearDensity* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_LinearDensity) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_LinearDensity*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_LinearDensity_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_LinearDensity);
	Type                                                      type = Ppc_LinearDensity_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_LinearDensity_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_LinearDensity_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_LinearDensity_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_LinearDensity_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_LinearDensity_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_LinearDensity_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_LinearDensity_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_LinearDensity_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_LinearDensity_Get;

	return (void*)_Ppc_LinearDensity_New(  PPC_LINEARDENSITY_PASSARGS  );
}


void _Ppc_LinearDensity_Init( void* _self, int alpha, int beta, int t, int p, double rt, double rp, double rd )
{
   Ppc_LinearDensity* self = (Ppc_LinearDensity*)_self;

	/* Sanity check */
	self->alphaTag = alpha;
	self->betaTag = beta;
	self->temperatureTag = t;
	self->pressureTag = p;
	self->referenceTemperature = rt;
	self->referencePressure = rp;
	self->referenceDensity = rd;
}


void _Ppc_LinearDensity_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	Ppc_LinearDensity* self = (Ppc_LinearDensity*)_self;

	/* Construct parent */
	_Ppc_AssignFromXML( self, cf, data );

   _Ppc_LinearDensity_Init( 
	   self, 
		PpcManager_GetPpcFromDict( self->manager, cf, (Name)self->name, "ThermalExpansivity", "" ),
		PpcManager_GetPpcFromDict( self->manager, cf, (Name)self->name, "Compressibility", "" ),
		PpcManager_GetPpcFromDict( self->manager, cf, (Name)self->name, "Temperature", "" ),
		PpcManager_GetPpcFromDict( self->manager, cf, (Name)self->name, "Pressure", "" ),
		Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ReferenceTemperature", 0.0 ), 
		Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ReferencePressure", 0.0 ), 
		Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ReferenceDensity", 0.0 ) );
}


void _Ppc_LinearDensity_Build( void* _self, void* data ) {
   Ppc_LinearDensity* self = (Ppc_LinearDensity*)_self;

	/* Build parent */
	_Ppc_Build( self, data );
}

void _Ppc_LinearDensity_Initialise( void* _self, void* data ) {
   Ppc_LinearDensity* self = (Ppc_LinearDensity*)_self;

	/* Initialize parent */
	_Ppc_Initialise( self, data );
}

void _Ppc_LinearDensity_Delete( void* _self ) {
   Ppc_LinearDensity* self = (Ppc_LinearDensity*)_self;

	/* Delete parent */
	_Ppc_Delete( self );
}

void _Ppc_LinearDensity_Print( void* _self, Stream* stream ) {
   Ppc_LinearDensity* self = (Ppc_LinearDensity*)_self;

	/* Print parent */
	_Ppc_Print( self, stream );
}

void _Ppc_LinearDensity_Execute( void* _self, void* data ) {
   Ppc_LinearDensity* self = (Ppc_LinearDensity*)_self;

	/* Execute parent */
	_Ppc_Execute( self, data );
}

void _Ppc_LinearDensity_Destroy( void* _self, void* data ) {
   Ppc_LinearDensity* self = (Ppc_LinearDensity*)_self;

	/* Destroy parent */
	_Ppc_Destroy( self, data );
}

/* 
 * Public functions 
 *
 */

int _Ppc_LinearDensity_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
  Ppc_LinearDensity* self = (Ppc_LinearDensity*) _self;
  double density, alpha, beta, T, P, T0, P0;
  double t_term = 0.0;
  double p_term = 0.0; 
  int err;

  /* Reference density */
  density = self->referenceDensity;

  /** Temperature dependence **/
  err = PpcManager_Get( self->manager, lElement_I, particle, self->temperatureTag, &T );
  if( !err ) {
	 T0 = self->referenceTemperature;
	 /* get thermal expansivity */
	 err = PpcManager_Get( self->manager, lElement_I, particle, self->alphaTag, &alpha );
	 if( err )
		alpha = 0;
	 
	 t_term = alpha*(T - T0); 
  }
  
  /** Pressure dependence **/
  err = PpcManager_Get( self->manager, lElement_I, particle, self->pressureTag, &P );
  if( !err ) {
	 P0 = self->referencePressure;
	 /* get compressibility */
	 err = PpcManager_Get( self->manager, lElement_I, particle, self->betaTag, &beta );
	 if( err )
		beta = 0;

	 p_term = beta*(P - P0);
  }
  
  density = density * ( 1 + p_term - t_term );

  result[0] = density;
  
  return 0;
}
