#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_Depth.h"


/* Textual name of this class */
const Type Ppc_Depth_Type = "Ppc_Depth";

void _Ppc_Depth_ReferenceHeight( Ppc_Depth* self, void* data)
{
   /* This function finds the particle's height and communicate to the other procs */
   double lbuffer, gbuffer;
   int ierr;
   if ( self->ms->particleLocalCount == 1 )
   {
      GlobalParticle *mp;
      mp = (GlobalParticle*)Swarm_ParticleAt(self->ms, 0 );
      lbuffer = mp->coord[1];
   }
   else
   {
      lbuffer = 0.0;
   }

   /* Communicate the height, note MPI_SUM is used */
   ierr = MPI_Allreduce( &lbuffer, &gbuffer, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
   self->refHeight = gbuffer;

   assert( isnan(self->refHeight) == 0 );
}

void _Ppc_Depth_Init( Ppc_Depth* self, FeMesh *mesh, Swarm *swarm, double refHeight )
{
   self->mesh = mesh;
   self->ms = swarm;

   if ( self->ms )
   {
      /* for passiveTraverSwarm as reference height */
      EP_PrependClassHook( Context_GetEntryPoint( self->context, AbstractContext_EP_UpdateClass ),
                           _Ppc_Depth_ReferenceHeight,
                           self );
   }
   else
   {
      self->refHeight = refHeight;
   }
}


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_Depth* _Ppc_Depth_New(  PPC_CONSTANT_DEFARGS  )
{
   Ppc_Depth* self;

   assert( _sizeOfSelf >= sizeof(Ppc_Depth) );
   nameAllocationType = NON_GLOBAL;
   self = (Ppc_Depth*) _Ppc_New(  PPC_PASSARGS  );
   self->_get = _get;
   return self;
}


void* _Ppc_Depth_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof(Ppc_Depth);
   Type                                                      type = Ppc_Depth_Type;
   Stg_Class_DeleteFunction*                              _delete = _Ppc_Depth_Delete;
   Stg_Class_PrintFunction*                                _print = _Ppc_Depth_Print;
   Stg_Class_CopyFunction*                                  _copy = NULL;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_Depth_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _Ppc_Depth_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _Ppc_Depth_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _Ppc_Depth_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _Ppc_Depth_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _Ppc_Depth_Destroy;
   AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_Depth_Get;

   return (void*)_Ppc_Depth_New(  PPC_CONSTANT_PASSARGS  );
}


void _Ppc_Depth_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data )
{
   Ppc_Depth* self = (Ppc_Depth*)_self;
   FeMesh* mesh = NULL;
   Swarm* ms = NULL;
   double refHeight = 0;;
   Dictionary*	theDictionary;

   /* Construct parent */
   _Ppc_AssignFromXML( self, cf, data );

   /* The dictionary */
   theDictionary = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );
   mesh = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Mesh", FeMesh, False, data );
   if ( !mesh )
      mesh = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_linearMesh", FeMesh, False, data );
   if ( !mesh )
      mesh = self->manager->mesh;


   ms = (Swarm*)Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"ReferenceSwarm", MaterialPointsSwarm, False, data );

   if ( ms == NULL )
   {
      Bool existRefHeight = Stg_ComponentFactory_TryDouble( cf, self->name, (Dictionary_Entry_Key)"ReferenceHeight", &refHeight );
      if ( existRefHeight == False )
      {
         Journal_Firewall(0, self->error_stream,
                          "\n\nError in %s, need <param name=\"ReferenceHeight\"></param> explicitly defined in %s\n\n",
                          __func__, self->name );
      }
   }

   /* Init */
   _Ppc_Depth_Init(
      self,
      mesh,
      ms,
      refHeight);
}


void _Ppc_Depth_Build( void* _self, void* data )
{
   Ppc_Depth* self = (Ppc_Depth*)_self;

   /* Build parent */
   _Ppc_Build( self, data );

   if ( self->ms ) Stg_Component_Build(self->ms, data, False );
}

void _Ppc_Depth_Initialise( void* _self, void* data )
{
   Ppc_Depth* self = (Ppc_Depth*)_self;

   /* Initialize parent */
   _Ppc_Initialise( self, data );

   if ( self->ms )
   {
      Stg_Component_Initialise(self->ms, data, False );
      _Ppc_Depth_ReferenceHeight( self, NULL );
   }
}

void _Ppc_Depth_Delete( void* _self )
{
   Ppc_Depth* self = (Ppc_Depth*)_self;

   /* Delete parent */
   _Ppc_Delete( self );
}

void _Ppc_Depth_Print( void* _self, Stream* stream )
{
   Ppc_Depth* self = (Ppc_Depth*)_self;

   /* Print parent */
   _Ppc_Print( self, stream );
}

void _Ppc_Depth_Execute( void* _self, void* data )
{
   Ppc_Depth* self = (Ppc_Depth*)_self;

   /* Execute parent */
   _Ppc_Execute( self, data );
}

void _Ppc_Depth_Destroy( void* _self, void* data )
{
   Ppc_Depth* self = (Ppc_Depth*)_self;

   /* Destroy parent */
   _Ppc_Destroy( self, data );
}

/*
 * Public functions
 *
 */

int _Ppc_Depth_Get( void* _self, unsigned lElement_I, IntegrationPoint* particle, double* result )
{
   Ppc_Depth* self = (Ppc_Depth*) _self;
   MaterialPoint *mp = NULL;
   MaterialPointsSwarm *ms=NULL;

   if( self->ms ) {
      mp = OneToOneMapper_GetMaterialPoint( self->manager->integrationSwarm->mapper, particle, &ms );
      result[0] =  self->refHeight - mp->coord[1];
   } else {
      double xyz[3];
      FeMesh_CoordLocalToGlobal( self->manager->mesh, lElement_I, particle->xi, xyz );
      result[0] = self->refHeight - xyz[1];
   }
   return 0;
}
