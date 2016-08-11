#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_PressureCalibrate.h"


/* Textual name of this class */
const Type Ppc_PressureCalibrate_Type = "Ppc_PressureCalibrate";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_PressureCalibrate* _Ppc_PressureCalibrate_New(  PPC_LITHOSTATICPRESSURE_DEFARGS  )
{
	Ppc_PressureCalibrate* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_PressureCalibrate) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_PressureCalibrate*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_PressureCalibrate_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_PressureCalibrate);
	Type                                                      type = Ppc_PressureCalibrate_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_PressureCalibrate_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_PressureCalibrate_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_PressureCalibrate_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_PressureCalibrate_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_PressureCalibrate_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_PressureCalibrate_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_PressureCalibrate_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_PressureCalibrate_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_PressureCalibrate_Get;

	return (void*)_Ppc_PressureCalibrate_New(  PPC_LITHOSTATICPRESSURE_PASSARGS  );
}


void _Ppc_PressureCalibrate_Init( void* _self, Swarm* swarm, FeVariable* pressureField, int blp_tag )
{
   Ppc_PressureCalibrate* self = (Ppc_PressureCalibrate*)_self;

	/* Sanity check */
	Journal_Firewall( pressureField != NULL || swarm != NULL, 
							self->error_stream, "\n\n\n"
							"Error in configuration of Ppc_PressureCalibrate %s\n"
							"Mesh not set"
							"\n\n\n", self->name );
	
	self->pressureFeVariable = pressureField;
   self->ms = swarm;
   self->blp_tag = blp_tag;

   /* sooooo dodgy */
   self->stokes_solver = (SLE_Solver*)LiveComponentRegister_Get( LiveComponentRegister_GetLiveComponentRegister(), "uzawa"  );

   if( self->ms ) { 
      /* more hack for passiveTraverSwarm as reference height */
      EP_PrependClassHook( Context_GetEntryPoint( self->context, AbstractContext_EP_UpdateClass ),
                           _Ppc_PressureCalibrate_GetAndStorePressure,
                           self );
   }

}

void _Ppc_PressureCalibrate_GetAndStorePressure( Ppc_PressureCalibrate* self, void* data)
{ 
   /* This function finds the particle's pressure and communicate to the other procs */
   double lbuffer, gbuffer;
   int ierr; 
   if( self->ms->particleLocalCount == 1 ) {
      GlobalParticle *mp;
      mp = (GlobalParticle*)Swarm_ParticleAt(self->ms, 0 );
      _FeVariable_InterpolateValueAt( self->pressureFeVariable, mp->coord, &lbuffer );
   } else {
      lbuffer = 0.0;
   }

   /* Communicate the height, note MPI_SUM is used */
   ierr = MPI_Allreduce( &lbuffer, &gbuffer, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
   self->offset = gbuffer;

   assert( isnan(self->offset) == 0 );
}
void _Ppc_PressureCalibrate_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
  Ppc_PressureCalibrate* self = (Ppc_PressureCalibrate*)_self;

  /* Construct parent */
  _Ppc_AssignFromXML( self, cf, data );

  _Ppc_PressureCalibrate_Init( 
     self, 
     (Swarm*)Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"ReferenceSwarm", MaterialPointsSwarm, False, data ),
     Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"PressureFeVariable", FeVariable, False, data ),
		PpcManager_GetPpcFromDict( self->manager, cf, (Name)self->name, "LithostaticPressureTag", "" ) );
}


void _Ppc_PressureCalibrate_Build( void* _self, void* data ) {
   Ppc_PressureCalibrate* self = (Ppc_PressureCalibrate*)_self;

	/* Build parent */
	_Ppc_Build( self, data );

   if( self->ms ) Stg_Component_Build(self->ms, data, False );
}

void _Ppc_PressureCalibrate_Initialise( void* _self, void* data ) {
   Ppc_PressureCalibrate* self = (Ppc_PressureCalibrate*)_self;

	/* Initialize parent */
	_Ppc_Initialise( self, data );

   if( self->ms ) {
      Stg_Component_Initialise(self->ms, data, False ); 
      _Ppc_PressureCalibrate_GetAndStorePressure( self, NULL );
   }
}

void _Ppc_PressureCalibrate_Delete( void* _self ) {
   Ppc_PressureCalibrate* self = (Ppc_PressureCalibrate*)_self;

	/* Delete parent */
	_Ppc_Delete( self );
}

void _Ppc_PressureCalibrate_Print( void* _self, Stream* stream ) {
   Ppc_PressureCalibrate* self = (Ppc_PressureCalibrate*)_self;

	/* Print parent */
	_Ppc_Print( self, stream );
}

void _Ppc_PressureCalibrate_Execute( void* _self, void* data ) {
   Ppc_PressureCalibrate* self = (Ppc_PressureCalibrate*)_self;

	/* Execute parent */
	_Ppc_Execute( self, data );
}

void _Ppc_PressureCalibrate_Destroy( void* _self, void* data ) {
   Ppc_PressureCalibrate* self = (Ppc_PressureCalibrate*)_self;

	/* Destroy parent */
	_Ppc_Destroy( self, data );
}


/* 
 * Public functions 
 *
 */

int _Ppc_PressureCalibrate_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
  Ppc_PressureCalibrate* self = (Ppc_PressureCalibrate*) _self;
  double originalPressure;
  int err;

  /* if there is no  pressure solution yet we need to assume the background lithostatic pressure
   * is good enough */
   if( 
         (self->context->timeStepSinceJobRestart == 0 && self->context->loadFromCheckPoint) 
         || 
         self->stokes_solver->hasSolved == False 
     ) 
   {
      err = PpcManager_Get( self->manager, lElement_I, particle, self->blp_tag, result );
      assert(err == 0);
      return 0;
   }

  /* get pressure at current location */
  FeVariable_InterpolateFromMeshLocalCoord( self->pressureFeVariable, self->pressureFeVariable->feMesh, lElement_I, particle->xi, &originalPressure );

  /* Adjust pressure with offset */
  result[0] = originalPressure - self->offset;

  return 0;
}
