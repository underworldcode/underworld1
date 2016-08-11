#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include "types.h"
#include "ViscousHeatingTermPpc.h"

#include <assert.h>
#include <string.h>

const Type ViscousHeatingTermPpc_Type = "ViscousHeatingTermPpc";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
ViscousHeatingTermPpc* _ViscousHeatingTermPpc_New(  VISCOUSHEATINGTERMTP_DEFARGS  )
{
   ViscousHeatingTermPpc* self;

   /* Call private constructor of parent */
   assert( _sizeOfSelf >= sizeof(ViscousHeatingTermPpc) );
   /* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
   /* This means that any values of these parameters that are passed into this function are not passed onto the parent function
      and so should be set to ZERO in any children of this class. */
   nameAllocationType = NON_GLOBAL;

   self = (ViscousHeatingTermPpc*) _ForceTerm_New(  FORCETERM_PASSARGS  );

   return self;
}

void _ViscousHeatingTermPpc_Init(
   void*          forceTerm,
   PpcManager*    mgr,
   FeVariable*    stressField,
   FeVariable*    strainRateField,
   int          densityLabel,
   int          cpLabel )
{
   ViscousHeatingTermPpc* self = (ViscousHeatingTermPpc*)forceTerm;

   self->mgr = mgr;
   self->stressField = stressField;
   self->strainRateField = strainRateField;
   self->densityLabel = densityLabel;
   self->cpLabel = cpLabel;
}

void _ViscousHeatingTermPpc_Delete( void* forceTerm )
{
   ViscousHeatingTermPpc* self = (ViscousHeatingTermPpc*)forceTerm;

   _ForceTerm_Delete( self );
}

void* _ViscousHeatingTermPpc_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof(ViscousHeatingTermPpc);
   Type                                                      type = ViscousHeatingTermPpc_Type;
   Stg_Class_DeleteFunction*                              _delete = _ViscousHeatingTermPpc_Delete;
   Stg_Class_PrintFunction*                                _print = NULL;
   Stg_Class_CopyFunction*                                  _copy = NULL;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _ViscousHeatingTermPpc_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _ViscousHeatingTermPpc_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _ViscousHeatingTermPpc_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _ViscousHeatingTermPpc_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _ViscousHeatingTermPpc_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _ViscousHeatingTermPpc_Destroy;
   ForceTerm_AssembleElementFunction*            _assembleElement = _ViscousHeatingTermPpc_AssembleElement;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*)_ViscousHeatingTermPpc_New(  VISCOUSHEATINGTERMTP_PASSARGS  );
}

void _ViscousHeatingTermPpc_AssignFromXML( void* forceTerm, Stg_ComponentFactory* cf, void* data )
{
   ViscousHeatingTermPpc* self = (ViscousHeatingTermPpc*)forceTerm;
   PpcManager* mgr = NULL;

   /* Construct Parent */
   _ForceTerm_AssignFromXML( self, cf, data );

   /* The PpcManager */
   mgr = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Manager", PpcManager, False, data );
   if ( !mgr  )
      mgr = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_ppcManager", PpcManager, True, data  );


   _ViscousHeatingTermPpc_Init(
      self, mgr,
      Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"DeviatoricStressField", FeVariable, True, data  ),
      Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"StrainRateField", FeVariable, True, data  ),
      PpcManager_GetPpcFromDict( mgr, cf, self->name, (Dictionary_Entry_Key)"DensityLabel", "DensityLabel" ),
      PpcManager_GetPpcFromDict( mgr, cf, self->name, (Dictionary_Entry_Key)"CpLabel", "CpLabel" )
   );

}

void _ViscousHeatingTermPpc_Build( void* forceTerm, void* data )
{
   ViscousHeatingTermPpc* self = (ViscousHeatingTermPpc*)forceTerm;

   _ForceTerm_Build( self, data );

   Stg_Component_Build( self->mgr, data, False );
   Stg_Component_Build( self->stressField, data, False );
   Stg_Component_Build( self->strainRateField, data, False );
}

void _ViscousHeatingTermPpc_Initialise( void* forceTerm, void* data )
{
   ViscousHeatingTermPpc* self = (ViscousHeatingTermPpc*)forceTerm;

   _ForceTerm_Initialise( self, data );

   Stg_Component_Initialise( self->mgr, data, False );
   Stg_Component_Initialise( self->stressField, data, False );
   Stg_Component_Initialise( self->strainRateField, data, False );
}

void _ViscousHeatingTermPpc_Execute( void* forceTerm, void* data )
{
   ViscousHeatingTermPpc* self = (ViscousHeatingTermPpc*)forceTerm;

   _ForceTerm_Execute( self, data );
}

void _ViscousHeatingTermPpc_Destroy( void* forceTerm, void* data )
{
   ViscousHeatingTermPpc* self = (ViscousHeatingTermPpc*)forceTerm;

   _ForceTerm_Destroy( self, data );

   Stg_Component_Destroy( self->mgr, data, False );
   Stg_Component_Destroy( self->stressField, data, False );
   Stg_Component_Destroy( self->strainRateField, data, False );
}

void _ViscousHeatingTermPpc_AssembleElement( void* forceTerm, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elForceVec )
{
   ViscousHeatingTermPpc*     self                = (ViscousHeatingTermPpc*) forceTerm;
   IntegrationPoint*          particle;
   IntegrationPointsSwarm*    swarm               =  (IntegrationPointsSwarm*) self->integrationSwarm;
   Particle_Index             lParticle_I;
   Particle_Index             cParticle_I;
   Particle_Index             cellParticleCount;
   Cell_Index                 cell_I;
   double                     detJac;
   double*                    xi;
   FeMesh*                    mesh                = forceVector->feVariable->feMesh;
   Dimension_Index            dim                 = forceVector->dim;
   ElementType*               elementType         = FeMesh_GetElementType( mesh, lElement_I );
   Node_Index                 elementNodeCount    = elementType->nodeCount;
   Node_Index                 node_I;
   double                     factor;
   double                     viscousHeating;
   SymmetricTensor            strainRate;
   SymmetricTensor            stress;
   double                     Ni[8];
   double                     cp, density;
   int                        err;

   /* Determine number of particles in element */
   cell_I = CellLayout_MapElementIdToCellId( swarm->cellLayout, lElement_I );
   cellParticleCount = swarm->cellParticleCountTbl[ cell_I ];

   for ( cParticle_I = 0 ; cParticle_I < cellParticleCount ; cParticle_I++ )
   {
      lParticle_I     = swarm->cellParticleTbl[cell_I][cParticle_I];

      particle        = (IntegrationPoint*) Swarm_ParticleAt( swarm, lParticle_I );
      xi              = particle->xi;


      FeVariable_InterpolateFromMeshLocalCoord( self->strainRateField, mesh, lElement_I, xi, strainRate );
      FeVariable_InterpolateFromMeshLocalCoord( self->stressField, mesh, lElement_I, xi, stress );

      /* Calculate Viscous Heating */
      if ( dim == 2 )
      {
         viscousHeating = strainRate[0] * stress[0] +
                          strainRate[1] * stress[1] +
                          2.0 * strainRate[2] * stress[2];
      }
      else
      {
         viscousHeating = strainRate[0] * stress[0] +
                          strainRate[1] * stress[1] +
                          strainRate[2] * stress[2] +
                          2.0 * (strainRate[3] * stress[3] +
                                 strainRate[4] * stress[4] +
                                 strainRate[5] * stress[5] );
      }

      /* Density */
      err = PpcManager_Get( self->mgr, lElement_I, particle, self->densityLabel, &density );
      assert(!err);
      /* Cp */
      err = PpcManager_Get( self->mgr, lElement_I, particle, self->cpLabel, &cp );
      assert(!err);

      /* This correction is needed if the heat equation is used in terms of thermal diffusivity */
      viscousHeating /= (density*cp);

      /* Calculate Shape Functions */
      ElementType_EvaluateShapeFunctionsAt( elementType, xi, Ni );
      detJac = ElementType_JacobianDeterminant( elementType, mesh, lElement_I, xi, dim );

      /* Add to element residual */
      factor = particle->weight * detJac * viscousHeating;
      for ( node_I = 0 ; node_I < elementNodeCount ; node_I++ )
      {
         elForceVec[ node_I ] += factor * Ni[ node_I ];
      }
   }
}


