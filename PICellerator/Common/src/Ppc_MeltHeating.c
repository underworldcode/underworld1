#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>
#include <float.h>

#include "types.h"
#include "Ppc_MeltHeating.h"


/* Textual name of this class */
const Type Ppc_MeltHeating_Type = "Ppc_MeltHeating";


void _Ppc_MeltHeating_Init( void* _self, 
      int          lhfLabel,
      int          cpLabel,
      int          meltLabel ) {
   Ppc_MeltHeating* self = (Ppc_MeltHeating*)_self;

   self->lhfLabel = lhfLabel;
   self->cpLabel = cpLabel;
   self->meltLabel = meltLabel;
}
   
/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_MeltHeating* _Ppc_MeltHeating_New(  PPC_OPERATION_DEFARGS  )
{
	Ppc_MeltHeating* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_MeltHeating) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_MeltHeating*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_MeltHeating_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_MeltHeating);
	Type                                                      type = Ppc_MeltHeating_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_MeltHeating_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_MeltHeating_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_MeltHeating_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_MeltHeating_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_MeltHeating_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_MeltHeating_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_MeltHeating_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_MeltHeating_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_MeltHeating_Get;

	return (void*)_Ppc_MeltHeating_New(  PPC_OPERATION_PASSARGS  );
}


void _Ppc_MeltHeating_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
  Ppc_MeltHeating* self = (Ppc_MeltHeating*)_self;
  
  /* Construct parent */
  _Ppc_AssignFromXML( self, cf, data );
  
  _Ppc_MeltHeating_Init(
      self,
      PpcManager_GetPpcFromDict( self->manager, cf, self->name, (Dictionary_Entry_Key)"LatentHeatFusionLabel", "LatentHeatFusionLabel" ),
      PpcManager_GetPpcFromDict( self->manager, cf, self->name, (Dictionary_Entry_Key)"CpLabel", "CpLabel" ),
      PpcManager_GetPpcFromDict( self->manager, cf, self->name, (Dictionary_Entry_Key)"MeltFractionLabel", "MeltFractionLabel" ));
}


void _Ppc_MeltHeating_Build( void* _self, void* data ) {
  Ppc_MeltHeating* self = (Ppc_MeltHeating*)_self;

  /* Build parent */
  _Ppc_Build( self, data );
}

void _Ppc_MeltHeating_Initialise( void* _self, void* data ) {
  Ppc_MeltHeating* self = (Ppc_MeltHeating*)_self;

  /* Initialize parent */
  _Ppc_Initialise( self, data );
  self->timeStepTolerance = Scaling_Scale( self->manager->scaling, 3600*24, sTime ); /* secs */
}

void _Ppc_MeltHeating_Delete( void* _self ) {
  Ppc_MeltHeating* self = (Ppc_MeltHeating*)_self;

  /* Delete parent */
  _Ppc_Delete( self );
}

void _Ppc_MeltHeating_Print( void* _self, Stream* stream ) {
  Ppc_MeltHeating* self = (Ppc_MeltHeating*)_self;

  /* Print parent */
  _Ppc_Print( self, stream );
}

void _Ppc_MeltHeating_Execute( void* _self, void* data ) {
  Ppc_MeltHeating* self = (Ppc_MeltHeating*)_self;

  /* Execute parent */
  _Ppc_Execute( self, data );
}

void _Ppc_MeltHeating_Destroy( void* _self, void* data ) {
  Ppc_MeltHeating* self = (Ppc_MeltHeating*)_self;

  /* Destroy parent */
  _Ppc_Destroy( self, data );
}


/* 
 * Public functions 
 *
 */

int _Ppc_MeltHeating_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
   Ppc_MeltHeating* self = (Ppc_MeltHeating*) _self;
   PpcManager* mgr = self->manager;
   double lhf, cp, dt, melt, previousMelt;
   int err;

   dt = self->context->dt;

   if( dt < self->timeStepTolerance ) {
      *result = 0;
      return 0;
   }

   /* Latent heat of fusion */
   err = PpcManager_Get( mgr, lElement_I, particle, self->lhfLabel, &lhf );
   assert(!err);
   /* Cp */
   err = PpcManager_Get( mgr, lElement_I, particle, self->cpLabel, &cp );
   assert(!err);
   /* melt */
   err = PpcManager_Get( mgr, lElement_I, particle, self->meltLabel, &melt );
   assert(!err);

  err = PpcManager_GetPrevious( mgr, lElement_I, particle, self->meltLabel, &previousMelt );
   assert(!err);

   *result = -1.0 * lhf * (melt-previousMelt) / (cp * dt);

   return 0;
}
