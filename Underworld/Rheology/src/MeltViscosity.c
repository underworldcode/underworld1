#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include "types.h"
#include "MeltViscosity.h"
#include <assert.h>


/* Textual name of this class */
const Type MeltViscosity_Type = "MeltViscosity";

/* Private Constructor */
MeltViscosity* _MeltViscosity_New(  MELTDEPENDENCE_DEFARGS  )
{
   MeltViscosity*self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(MeltViscosity) );
   self = (MeltViscosity*) _Rheology_New(  RHEOLOGY_PASSARGS  );

   return self;
}

void* _MeltViscosity_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof(MeltViscosity);
   Type                                                             type = MeltViscosity_Type;
   Stg_Class_DeleteFunction*                                     _delete = _Rheology_Delete;
   Stg_Class_PrintFunction*                                       _print = _Rheology_Print;
   Stg_Class_CopyFunction*                                         _copy = _Rheology_Copy;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _MeltViscosity_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _MeltViscosity_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _Rheology_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _Rheology_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _Rheology_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _Rheology_Destroy;
   Rheology_ModifyConstitutiveMatrixFunction*  _modifyConstitutiveMatrix = _MeltViscosity_ModifyConstitutiveMatrix;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _MeltViscosity_New(  MELTDEPENDENCE_PASSARGS  );
}

void _MeltViscosity_Init( void* _self, ParticleMelting *particleMelting, double lowerlimit, double upperlimit,  double minVisFac, PpcManager* mgr, int meltTag )
{
   MeltViscosity*  self = (MeltViscosity*)_self;

   self->particleMelting = particleMelting;
   self->lowerlimit = lowerlimit;
   self->upperlimit = upperlimit;
   self->minVisFac = minVisFac;
   self->ppcMgr = mgr;
   self->meltTag = meltTag;
}

void _MeltViscosity_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data )
{
   MeltViscosity*  self = (MeltViscosity*)_self;
   ParticleMelting *particleMelting = NULL;
   double lowerlimit, upperlimit, minVisFac;
   PpcManager* mgr = NULL;
   int meltTag = -1;

   /* Construct Parent */
   _Rheology_AssignFromXML( self, cf, data );

   particleMelting = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"ParticleMelting", ParticleMelting, False, data );

   /* ParticleMelting was the old implementation, if that is not found
    * look for the PpcManager and the MeltFractionLabel */
   if( particleMelting == NULL )
   {
      /* The PpcManager */
      mgr = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Manager", PpcManager, False, data );
      if( !mgr  )
         mgr = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_ppcManager", PpcManager, True, data  );

      meltTag = PpcManager_GetPpcFromDict( mgr, cf, (Name)self->name, "MeltFraction", "MeltFractionLabel" );
   }

   lowerlimit = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"MeltFraction_LowerLimit", 0.2 );
   upperlimit = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"MeltFraction_UpperLimit", 0.4 );
   minVisFac = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"max_linear_decrease", 0.0 );

   _MeltViscosity_Init( self, particleMelting, lowerlimit, upperlimit, minVisFac, mgr, meltTag );
}

void _MeltViscosity_ModifyConstitutiveMatrix( void* _self,
      ConstitutiveMatrix*    constitutiveMatrix,
      MaterialPointsSwarm*   swarm,
      Element_LocalIndex     lElement_I,
      MaterialPoint*         materialPoint,
      Coord                  xi )
{
   MeltViscosity* self = (MeltViscosity*) _self;
   ParticleMelting_ParExt *parExt = NULL;
   double viscosity, aux, init_viscosity, F;

   if( self->particleMelting != NULL )
   {
      /* get the F viao ParticleMelting's particle extension */
      parExt = ExtensionManager_Get( swarm->particleExtensionMgr, materialPoint, self->particleMelting->particleExtHandle );
      F = parExt->F;
   }
   else
   {
      int err;
      /***** SCARY stuff to get an integration point *****/
      IntegrationPointsSwarm* i_swarm = (IntegrationPointsSwarm*)constitutiveMatrix->integrationSwarm;
      /*
      unsigned cell_I = CellLayout_MapElementIdToCellId( i_swarm->cellLayout, lElement_I );
         Swarm_ParticleInCellAt( i_swarm, cell_I, constitutiveMatrix );
         */
      IntegrationPoint* ip = (IntegrationPoint*)Swarm_ParticleAt( i_swarm, constitutiveMatrix->currentParticleIndex );
      /***** END SCARY stuff to get an integration point *****/
      /* get the F via a ppc */
      err = PpcManager_Get( self->ppcMgr, lElement_I, ip, self->meltTag, &F );
      assert( err != -1 );
   }

   viscosity = ConstitutiveMatrix_GetIsotropicViscosity( constitutiveMatrix );
   init_viscosity = viscosity;

   if( F > self->upperlimit )
   {
      viscosity = (self->minVisFac)*viscosity;
   }
   else if ( (F > self->lowerlimit) && (F < self->upperlimit ) )
   {
      /* aux is the interpolation coordinate between lowerlimit and upperlimit in F */
      aux = 1.0 + (F - self->lowerlimit) / (self->lowerlimit - self->upperlimit);
      /* linear interpolation */
      viscosity = viscosity * (1 * aux + self->minVisFac * (1 - aux) );
   }

   assert( self->minVisFac* 0.99 * init_viscosity <= viscosity );

   ConstitutiveMatrix_SetIsotropicViscosity( constitutiveMatrix, viscosity );
}
