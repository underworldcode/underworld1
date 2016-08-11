#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc.h"
#include "Ppc_LinearDensity.h"
#include "Ppc_LinearDensityMelt.h"


/* Textual name of this class */
const Type Ppc_LinearDensityMelt_Type = "Ppc_LinearDensityMelt";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_LinearDensityMelt* _Ppc_LinearDensityMelt_New(  PPC_LINEARDENSITYMELT_DEFARGS  )
{
	Ppc_LinearDensityMelt* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_LinearDensityMelt) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_LinearDensityMelt*) _Ppc_LinearDensity_New(  PPC_LINEARDENSITY_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_LinearDensityMelt_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_LinearDensityMelt);
	Type                                                      type = Ppc_LinearDensityMelt_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_LinearDensity_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_LinearDensity_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_LinearDensityMelt_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_LinearDensityMelt_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_LinearDensity_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_LinearDensity_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_LinearDensity_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_LinearDensity_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_LinearDensityMelt_Get;

	return (void*)_Ppc_LinearDensityMelt_New(  PPC_LINEARDENSITY_PASSARGS  );
}


void _Ppc_LinearDensityMelt_Init( void* _self, int m, int mdc )
{
   Ppc_LinearDensityMelt* self = (Ppc_LinearDensityMelt*)_self;

	self->meltFracTag = m;
	self->meltDensityCoeffTag = mdc;
}


void _Ppc_LinearDensityMelt_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	Ppc_LinearDensityMelt* self = (Ppc_LinearDensityMelt*)_self;

	/* Construct parent */
	_Ppc_LinearDensity_AssignFromXML( self, cf, data );

   _Ppc_LinearDensityMelt_Init( 
	   self, 
		PpcManager_GetPpcFromDict( self->manager, cf, (Name)self->name, "MeltFraction", "" ),
		PpcManager_GetPpcFromDict( self->manager, cf, (Name)self->name, "MeltDensityChange", "" )
      ); 
}

/* 
 * Public functions 
 *
 */

int _Ppc_LinearDensityMelt_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
  Ppc_LinearDensityMelt* self = (Ppc_LinearDensityMelt*) _self;
  double density, alpha, beta, T, P, T0, P0, melt, mc;
  double t_term = 0.0;
  double p_term = 0.0; 
  double m_term = 0.0; 
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

  /** Melt dependence **/
  err = PpcManager_Get( self->manager, lElement_I, particle, self->meltFracTag, &melt );
  if( !err ) {
     err = PpcManager_Get( self->manager, lElement_I, particle, self->meltDensityCoeffTag, &mc ); 
     if( err )
        mc = 0;

     m_term = mc * melt; 
  }
  
  density = density * ( 1 + p_term - t_term - m_term );

  result[0] = density;
  
  return 0;
}
