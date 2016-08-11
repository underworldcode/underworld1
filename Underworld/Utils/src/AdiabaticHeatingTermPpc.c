#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include "types.h"
#include "AdiabaticHeatingTermPpc.h"

#include <assert.h>
#include <string.h>

/* Textual name of this class */
const Type AdiabaticHeatingTermPpc_Type = "AdiabaticHeatingTermPpc";


/* Private Constructor */
AdiabaticHeatingTermPpc* _AdiabaticHeatingTermPpc_New(  ADIABATICHEATINGTERMPPC_DEFARGS  )
{
	AdiabaticHeatingTermPpc* self;
	
	assert( _sizeOfSelf >= sizeof(AdiabaticHeatingTermPpc) );
	nameAllocationType = NON_GLOBAL;
	self = (AdiabaticHeatingTermPpc*) _ForceTerm_New(  FORCETERM_PASSARGS  );
	return self;
}

void _AdiabaticHeatingTermPpc_Delete( void* _self ) {
	AdiabaticHeatingTermPpc* self = (AdiabaticHeatingTermPpc*)_self;

	_ForceTerm_Delete( self );
}

void _AdiabaticHeatingTermPpc_Print( void* _self, Stream* stream ) {
  AdiabaticHeatingTermPpc* self = (AdiabaticHeatingTermPpc*)_self;
	
  _ForceTerm_Print( self, stream );
}

void* _AdiabaticHeatingTermPpc_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(AdiabaticHeatingTermPpc);
	Type                                                      type = AdiabaticHeatingTermPpc_Type;
	Stg_Class_DeleteFunction*                              _delete = _AdiabaticHeatingTermPpc_Delete;
	Stg_Class_PrintFunction*                                _print = _AdiabaticHeatingTermPpc_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _AdiabaticHeatingTermPpc_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _AdiabaticHeatingTermPpc_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _AdiabaticHeatingTermPpc_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _AdiabaticHeatingTermPpc_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _AdiabaticHeatingTermPpc_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _AdiabaticHeatingTermPpc_Destroy;
	ForceTerm_AssembleElementFunction*            _assembleElement = _AdiabaticHeatingTermPpc_AssembleElement;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*)_AdiabaticHeatingTermPpc_New(  ADIABATICHEATINGTERMPPC_PASSARGS  );
}

void _AdiabaticHeatingTermPpc_Init( void* _self, int temperatureLabel, int velocity_id, PpcManager* mgr, int cpLabel, int alphaName ) {
	AdiabaticHeatingTermPpc* self = (AdiabaticHeatingTermPpc*)_self;

	self->temperatureLabel = temperatureLabel;
	self->velocity_id = velocity_id;
	self->manager = mgr;	
	self->cpLabel = cpLabel;
	self->alphaLabel = alphaName;
}

void _AdiabaticHeatingTermPpc_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	AdiabaticHeatingTermPpc* self = (AdiabaticHeatingTermPpc*)_self;
	PpcManager* mgr = NULL;
	int vel;

	/* Construct Parent */
	_ForceTerm_AssignFromXML( self, cf, data );

	/* The PpcManager */
	mgr = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Manager", PpcManager, False, data );
	if( !mgr  )
		mgr = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_ppcManager", PpcManager, True, data );

	vel = PpcManager_GetField( mgr, cf, (Stg_Component*)self, "VelocityField", True );

	_AdiabaticHeatingTermPpc_Init(
		self,
		PpcManager_GetField( mgr, cf, (Stg_Component*)self, "TemperatureField", True ),
		vel,
		mgr,
		PpcManager_GetPpcFromDict( mgr, cf, self->name, (Dictionary_Entry_Key)"CpLabel", "CpLabel" ),
		PpcManager_GetPpcFromDict( mgr, cf, self->name, (Dictionary_Entry_Key)"ThermalExpansivityLabel", "ThermalExpansivityLabel" ) );
}

void _AdiabaticHeatingTermPpc_Build( void* _self, void* data ) {
	AdiabaticHeatingTermPpc* self = (AdiabaticHeatingTermPpc*)_self;

	_ForceTerm_Build( self, data );
}

void _AdiabaticHeatingTermPpc_Initialise( void* _self, void* data ) {
	AdiabaticHeatingTermPpc* self = (AdiabaticHeatingTermPpc*)_self;

	_ForceTerm_Initialise( self, data );
}

void _AdiabaticHeatingTermPpc_Execute( void* _self, void* data ) {
	AdiabaticHeatingTermPpc* self = (AdiabaticHeatingTermPpc*)_self;

	_ForceTerm_Execute( self, data );
}

void _AdiabaticHeatingTermPpc_Destroy( void* _self, void* data ) {
	AdiabaticHeatingTermPpc* self = (AdiabaticHeatingTermPpc*)_self;

	_ForceTerm_Destroy( self, data );
}

void _AdiabaticHeatingTermPpc_AssembleElement( void* _self, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elForceVec ) {
  AdiabaticHeatingTermPpc* self = (AdiabaticHeatingTermPpc*) _self;
  IntegrationPointsSwarm* swarm =  (IntegrationPointsSwarm*) self->integrationSwarm;
  FeMesh* mesh = forceVector->feVariable->feMesh;
  IntegrationPoint* particle = NULL;
  double* xi = NULL;
  Particle_Index lParticle_I, cParticle_I, cellParticleCount;
  Cell_Index cell_I;    
  Dimension_Index dim = forceVector->dim;
  ElementType* elementType = FeMesh_GetElementType( mesh, lElement_I );
  Node_Index elementNodeCount = elementType->nodeCount;
  Node_Index node_I;
  double factor, detJac;
  double Ni[8];
  double velocity[3];
  double temperature = 0.0;
  double g[3], gravityNorm, cp, thermalExpansivity, adiabaticHeating, verticalVelocity;
  int err = 0;

  /* Determine number of particles in element */
  cell_I = CellLayout_MapElementIdToCellId( swarm->cellLayout, lElement_I );
  cellParticleCount = swarm->cellParticleCountTbl[ cell_I ];
	
  for ( cParticle_I = 0 ; cParticle_I < cellParticleCount ; cParticle_I++ ) {
	 lParticle_I = swarm->cellParticleTbl[cell_I][cParticle_I];

	 particle = (IntegrationPoint*) Swarm_ParticleAt( swarm, lParticle_I );
	 xi = particle->xi;

	 /* Calculate Shape Functions */
	 detJac = ElementType_JacobianDeterminant( elementType, mesh, lElement_I, xi, dim );
	 ElementType_EvaluateShapeFunctionsAt( elementType, xi, Ni );

	 /* get temperature */
	 err = PpcManager_Get( self->manager, lElement_I, particle, self->temperatureLabel, &temperature );
	 assert( !err );

	 /* Vertical velocity */
	 PpcManager_Get( self->manager, lElement_I, particle, self->velocity_id, velocity );
	 /* FIXME: cartessian grid assumed */
	 verticalVelocity = velocity[J_AXIS];
	  	 
	 /* get thermal expansivity */
	 err = PpcManager_Get( self->manager, lElement_I, particle, self->alphaLabel, &thermalExpansivity );
	 assert( !err );

	 /* get cp */
	 err = PpcManager_Get( self->manager, lElement_I, particle, self->cpLabel, &cp );
	 assert( !err );

	 /* get gravity */
	 err = PpcManager_GetGravity( self->manager, lElement_I, particle, g );
	 assert( !err );
  	 gravityNorm = sqrt( g[0]*g[0] + g[1]*g[1] + g[2]*g[2] );

	 
	 /* Energy equation writen in terms of the THERMAL DIFFUSIVITY! */
	 adiabaticHeating = -1.0 * temperature * thermalExpansivity * verticalVelocity * gravityNorm / cp;
	  
	 /* Add to element residual */
	 factor = particle->weight * detJac * adiabaticHeating;
	 for ( node_I = 0 ; node_I < elementNodeCount ; node_I++ ) {
		elForceVec[ node_I ] += factor * Ni[ node_I ]; 
	 }
  }
}


