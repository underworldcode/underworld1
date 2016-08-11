#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_TempDependentDiffusivityAlt.h"


/* Textual name of this class */
const Type Ppc_TempDependentDiffusivityAlt_Type = "Ppc_TempDependentDiffusivityAlt";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_TempDependentDiffusivityAlt* _Ppc_TempDependentDiffusivityAlt_New(  PPC_TEMPDEPENDENTDIFFUSIVITYALT_DEFARGS  ){
   Ppc_TempDependentDiffusivityAlt* self;

   assert( _sizeOfSelf >= sizeof(Ppc_TempDependentDiffusivityAlt) );
   nameAllocationType = NON_GLOBAL;
   self = (Ppc_TempDependentDiffusivityAlt*) _Ppc_New(  PPC_PASSARGS  );
   self->_get = _get;
   return self;
}


void* _Ppc_TempDependentDiffusivityAlt_DefaultNew( Name name ){
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof(Ppc_TempDependentDiffusivityAlt);
   Type                                                      type = Ppc_TempDependentDiffusivityAlt_Type;
   Stg_Class_DeleteFunction*                              _delete = _Ppc_TempDependentDiffusivityAlt_Delete;
   Stg_Class_PrintFunction*                                _print = _Ppc_TempDependentDiffusivityAlt_Print;
   Stg_Class_CopyFunction*                                  _copy = NULL;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_TempDependentDiffusivityAlt_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _Ppc_TempDependentDiffusivityAlt_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _Ppc_TempDependentDiffusivityAlt_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _Ppc_TempDependentDiffusivityAlt_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _Ppc_TempDependentDiffusivityAlt_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _Ppc_TempDependentDiffusivityAlt_Destroy;
   AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_TempDependentDiffusivityAlt_Get;

   return (void*)_Ppc_TempDependentDiffusivityAlt_New(  PPC_TEMPDEPENDENTDIFFUSIVITYALT_PASSARGS  );
}


void _Ppc_TempDependentDiffusivityAlt_Init( void* _self, FeVariable* temperatureField, double T0, double TCrit, double K0, double KCrit ){
   Ppc_TempDependentDiffusivityAlt* self = (Ppc_TempDependentDiffusivityAlt*)_self;

	self->temperatureField = temperatureField;	
	self->T0    = T0;
	self->TCrit = TCrit;
	self->K0    = K0;
	self->KCrit = KCrit;
}


void _Ppc_TempDependentDiffusivityAlt_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ){
   Ppc_TempDependentDiffusivityAlt* self = (Ppc_TempDependentDiffusivityAlt*)_self;
	FeVariable* temperatureField;
	double T0, TCrit;
	double K0, KCrit;

   /* Construct parent */
   _Ppc_AssignFromXML( self, cf, data );

   temperatureField = Stg_ComponentFactory_ConstructByNameWithKeyFallback( cf, self->name, (Name)"TemperatureField", (Dictionary_Entry_Key)"TemperatureField", FeVariable, True, data  );
	
	T0    = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"T0"   , 1. );
	TCrit = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"TCrit", 1. );
	K0    = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"K0"   , 1. );
	KCrit = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"KCrit", 1. );

   _Ppc_TempDependentDiffusivityAlt_Init( self, temperatureField, T0, TCrit, K0, KCrit );
}


void _Ppc_TempDependentDiffusivityAlt_Build( void* _self, void* data ){
   Ppc_TempDependentDiffusivityAlt* self = (Ppc_TempDependentDiffusivityAlt*)_self;

   Stg_Component_Build( self->temperatureField, data, False );

   /* Build parent */
   _Ppc_Build( self, data );
}

void _Ppc_TempDependentDiffusivityAlt_Initialise( void* _self, void* data ){
   Ppc_TempDependentDiffusivityAlt* self = (Ppc_TempDependentDiffusivityAlt*)_self;

   Stg_Component_Initialise( self->temperatureField, data, False );

   /* Initialize parent */
   _Ppc_Initialise( self, data );
}

void _Ppc_TempDependentDiffusivityAlt_Delete( void* _self ){
   Ppc_TempDependentDiffusivityAlt* self = (Ppc_TempDependentDiffusivityAlt*)_self;

   /* Delete parent */
   _Ppc_Delete( self );
}

void _Ppc_TempDependentDiffusivityAlt_Print( void* _self, Stream* stream ){
   Ppc_TempDependentDiffusivityAlt* self = (Ppc_TempDependentDiffusivityAlt*)_self;

   /* Print parent */
   _Ppc_Print( self, stream );
}

void _Ppc_TempDependentDiffusivityAlt_Execute( void* _self, void* data ){
   Ppc_TempDependentDiffusivityAlt* self = (Ppc_TempDependentDiffusivityAlt*)_self;

   /* Execute parent */
   _Ppc_Execute( self, data );
}

void _Ppc_TempDependentDiffusivityAlt_Destroy( void* _self, void* data ){
   Ppc_TempDependentDiffusivityAlt* self = (Ppc_TempDependentDiffusivityAlt*)_self;

   /* Destroy parent */
   _Ppc_Destroy( self, data );
}


/*
 * Public functions
 *
 */

int _Ppc_TempDependentDiffusivityAlt_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ){
   Ppc_TempDependentDiffusivityAlt* self = (Ppc_TempDependentDiffusivityAlt*) _self;
   double  temperature;
   double  K0    = self->K0;
   double  KCrit = self->KCrit;
   double  T0    = self->T0;
   double  TCrit = self->TCrit;

   FeVariable_InterpolateWithinElement( self->temperatureField, lElement_I, particle->xi, &temperature );

   result[0] = ((T0*TCrit)/(TCrit-T0))*(K0-KCrit)*(1/temperature - 1/TCrit) + KCrit;

   if (result[0] < 0 ){
      double globalCoord[3];
      FeMesh_CoordLocalToGlobal( self->temperatureField->feMesh, lElement_I, particle->xi, &globalCoord );
      Journal_Firewall( result[0]>0, NULL, "Error in %s for %s '%s' - Conductivity value (%f) is negative at coord (%f,%f,%f).", __func__, self->type, self->name, result[0], globalCoord[0], globalCoord[1], globalCoord[2] );
   }

   return 0;

}
