#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_LithostaticPressure.h"


/* Textual name of this class */
const Type Ppc_LithostaticPressure_Type = "Ppc_LithostaticPressure";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_LithostaticPressure* _Ppc_LithostaticPressure_New(  PPC_LITHOSTATICPRESSURE_DEFARGS  )
{
	Ppc_LithostaticPressure* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_LithostaticPressure) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_LithostaticPressure*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_LithostaticPressure_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_LithostaticPressure);
	Type                                                      type = Ppc_LithostaticPressure_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_LithostaticPressure_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_LithostaticPressure_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_LithostaticPressure_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_LithostaticPressure_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_LithostaticPressure_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_LithostaticPressure_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_LithostaticPressure_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_LithostaticPressure_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_LithostaticPressure_Get;

	return (void*)_Ppc_LithostaticPressure_New(  PPC_LITHOSTATICPRESSURE_PASSARGS  );
}


void _Ppc_LithostaticPressure_Init( void* _self, FeMesh* mesh, double refDepth, int density_id )
{
   Ppc_LithostaticPressure* self = (Ppc_LithostaticPressure*)_self;

	/* Sanity check */
	Journal_Firewall( mesh != NULL, 
							self->error_stream, "\n\n\n"
							"Error in configuration of Ppc_LithostaticPressure %s\n"
							"Mesh not set"
							"\n\n\n", self->name );
	
	self->mesh = mesh;
	self->density_id = density_id;
	self->refDepth = refDepth;

   if( self->ms ) { 
      /* more hack for passiveTraverSwarm as reference height */
      EP_PrependClassHook( Context_GetEntryPoint( self->context, AbstractContext_EP_UpdateClass ),
                           _Ppc_LithostaticPressure_ReferenceHeight,
                           self );
   }

}

void _Ppc_LithostaticPressure_ReferenceHeight( Ppc_LithostaticPressure* self, void* data)
{ 
   /* This function finds the particle's height and communicate to the other procs */
   double lbuffer, gbuffer;
   int ierr; 
   if( self->ms->particleLocalCount == 1 ) {
      GlobalParticle *mp;
      mp = (GlobalParticle*)Swarm_ParticleAt(self->ms, 0 );
      lbuffer = mp->coord[1];
   } else {
      lbuffer = 0.0;
   }

   /* Communicate the height, note MPI_SUM is used */
   ierr = MPI_Allreduce( &lbuffer, &gbuffer, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
   self->refDepth = gbuffer;

   assert( isnan(self->refDepth) == 0 );
}
void _Ppc_LithostaticPressure_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
  Ppc_LithostaticPressure* self = (Ppc_LithostaticPressure*)_self;
  FeMesh* mesh = NULL;

  /* Construct parent */
  _Ppc_AssignFromXML( self, cf, data );

  /* get the mesh */
  mesh = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Mesh", FeMesh, False, data );
  if( !mesh )
	 mesh = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_linearMesh", FeMesh, False, data );
  if( !mesh )  
	 mesh = self->manager->mesh;

   /* hack to get a passive tracer swarm */
  self->ms = (Swarm*)Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"ReferenceSwarm", MaterialPointsSwarm, False, data );

  _Ppc_LithostaticPressure_Init( 
     self, 
	  mesh,
     Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ReferenceDepth", 0.0 ), 
     PpcManager_GetPpcFromDict( self->manager, cf, self->name, (Dictionary_Entry_Key)"ReferenceDensity", "ReferenceDensity" ));
}


void _Ppc_LithostaticPressure_Build( void* _self, void* data ) {
   Ppc_LithostaticPressure* self = (Ppc_LithostaticPressure*)_self;

	/* Build parent */
	_Ppc_Build( self, data );

   if( self->ms ) Stg_Component_Build(self->ms, data, False );
}

void _Ppc_LithostaticPressure_Initialise( void* _self, void* data ) {
   Ppc_LithostaticPressure* self = (Ppc_LithostaticPressure*)_self;

	/* Initialize parent */
	_Ppc_Initialise( self, data );

   if( self->ms ) {
      Stg_Component_Initialise(self->ms, data, False );
      _Ppc_LithostaticPressure_ReferenceHeight( self, NULL );
   }
}

void _Ppc_LithostaticPressure_Delete( void* _self ) {
   Ppc_LithostaticPressure* self = (Ppc_LithostaticPressure*)_self;

	/* Delete parent */
	_Ppc_Delete( self );
}

void _Ppc_LithostaticPressure_Print( void* _self, Stream* stream ) {
   Ppc_LithostaticPressure* self = (Ppc_LithostaticPressure*)_self;

	/* Print parent */
	_Ppc_Print( self, stream );
}

void _Ppc_LithostaticPressure_Execute( void* _self, void* data ) {
   Ppc_LithostaticPressure* self = (Ppc_LithostaticPressure*)_self;

	/* Execute parent */
	_Ppc_Execute( self, data );
}

void _Ppc_LithostaticPressure_Destroy( void* _self, void* data ) {
   Ppc_LithostaticPressure* self = (Ppc_LithostaticPressure*)_self;

	/* Destroy parent */
	_Ppc_Destroy( self, data );
}


/* 
 * Public functions 
 *
 */

int _Ppc_LithostaticPressure_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
  Ppc_LithostaticPressure* self = (Ppc_LithostaticPressure*) _self;
  double g[3], rho, depth, coord[3], min[3], max[3];
  double refHeight = 0;
  int err;

  if( !self->mesh )
	 return -1;

  /* get density */
  err = PpcManager_Get( self->manager, lElement_I, particle, self->density_id, &rho );
  Journal_Firewall( err != -1, self->error_stream,
       "\nError in %s:\nCan't find density\n", __func__ );

  /* get gravity */
  err = PpcManager_GetGravity( self->manager, lElement_I, particle, g );
  Journal_Firewall( err != -1, self->error_stream,
       "\nError in %s:\nCan't find gravity\n", __func__ );
  

  if( self->ms ) {
     refHeight = self->refDepth;
  } else {
     Mesh_GetGlobalCoordRange( self->mesh, min, max );
     refHeight = max[J_AXIS] - self->refDepth;
  }

  /* Depth */
  FeMesh_CoordLocalToGlobal( self->mesh, lElement_I, particle->xi, (double*)&coord );
  depth = fabs( refHeight - coord[J_AXIS] );

  /* Pressure */
  result[0] = rho*g[J_AXIS]*depth;

  return 0;
}
