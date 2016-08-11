
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include "types.h"
#include "BuoyancyForceTermPpc.h"

#include <assert.h>
#include <string.h>
#include <math.h>

/* Textual name of this class */
const Type BuoyancyForceTermPpc_Type = "BuoyancyForceTermPpc";

/* Private Constructor */
void* _BuoyancyForceTermPpc_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(BuoyancyForceTermPpc);
	Type                                                      type = BuoyancyForceTermPpc_Type;
	Stg_Class_DeleteFunction*                              _delete = _BuoyancyForceTermPpc_Delete;
	Stg_Class_PrintFunction*                                _print = _BuoyancyForceTermPpc_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _BuoyancyForceTermPpc_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _BuoyancyForceTermPpc_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _BuoyancyForceTermPpc_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _BuoyancyForceTermPpc_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _BuoyancyForceTermPpc_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _BuoyancyForceTermPpc_Destroy;
	ForceTerm_AssembleElementFunction*            _assembleElement = _BuoyancyForceTermPpc_AssembleElement;
	AllocationType                              nameAllocationType = NON_GLOBAL;

	return (void*)_BuoyancyForceTermPpc_New(  BUOYANCYFORCETERMPPC_PASSARGS  );
}

BuoyancyForceTermPpc* _BuoyancyForceTermPpc_New(  BUOYANCYFORCETERMPPC_DEFARGS  ) {
	BuoyancyForceTermPpc* self;
	assert( _sizeOfSelf >= sizeof(BuoyancyForceTermPpc) );
	self = (BuoyancyForceTermPpc*) _ForceTerm_New(  FORCETERM_PASSARGS  );
	return self;
}

void _BuoyancyForceTermPpc_Init( void* _self, PpcManager* mgr ) {
	BuoyancyForceTermPpc* self = (BuoyancyForceTermPpc*)_self;

	self->manager = mgr;
}

void _BuoyancyForceTermPpc_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	BuoyancyForceTermPpc* self = (BuoyancyForceTermPpc*)_self;
	PpcManager* mgr = NULL;

	/* Construct Parent */
	_ForceTerm_AssignFromXML( self, cf, data );

	/* The PpcManager */
	mgr = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Manager", PpcManager, False, data );
	if( !mgr  )
		mgr = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_ppcManager", PpcManager, True, data  );

	self->density_id = PpcManager_GetPpcFromDict( mgr, cf, self->name, (Dictionary_Entry_Key)"DensityLabel", "DensityLabel" );

   _BuoyancyForceTermPpc_Init( self, mgr );

}

void _BuoyancyForceTermPpc_Build( void* _self, void* data ) {
	BuoyancyForceTermPpc* self = (BuoyancyForceTermPpc*)_self;

	/* Build parent */	
	_ForceTerm_Build( self, data );
}

void _BuoyancyForceTermPpc_Initialise( void* _self, void* data ) {
	BuoyancyForceTermPpc* self = (BuoyancyForceTermPpc*)_self;

	/* Initialize parent */
	_ForceTerm_Initialise( self, data );
}

void _BuoyancyForceTermPpc_Delete( void* _self ) {
	BuoyancyForceTermPpc* self = (BuoyancyForceTermPpc*)_self;

	_ForceTerm_Delete( self );
}

void _BuoyancyForceTermPpc_Print( void* _self, Stream* stream ) {
	BuoyancyForceTermPpc* self = (BuoyancyForceTermPpc*)_self;
	
	_ForceTerm_Print( self, stream );
}

void _BuoyancyForceTermPpc_Execute( void* _self, void* data ) {
	BuoyancyForceTermPpc* self = (BuoyancyForceTermPpc*)_self;

	_ForceTerm_Execute( self, data );
}

void _BuoyancyForceTermPpc_Destroy( void* _self, void* data ) {
	BuoyancyForceTermPpc* self = (BuoyancyForceTermPpc*)_self;

	_ForceTerm_Destroy( self, data );
}

void _BuoyancyForceTermPpc_AssembleElement( void* _self, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elForceVec ) {
	BuoyancyForceTermPpc* self = (BuoyancyForceTermPpc*) _self;
	IntegrationPoint* particle;
	Particle_InCellIndex cParticle_I;
	Particle_InCellIndex cellParticleCount;
	Element_NodeIndex elementNodeCount;
	Dimension_Index dim = forceVector->dim;
	IntegrationPointsSwarm* swarm = (IntegrationPointsSwarm*)self->integrationSwarm;
	FeMesh* mesh = forceVector->feVariable->feMesh;
	Node_ElementLocalIndex eNode_I;
	unsigned int dim_I;
	Cell_Index cell_I;
	ElementType* elementType;
	Dof_Index nodeDofCount;
	double gravity[3], factor, density;
	double detJac = 0.0;
	double Ni[27];
	double* xi;
	int err;

	elementType = FeMesh_GetElementType( mesh, lElement_I );
	elementNodeCount = elementType->nodeCount;
	nodeDofCount = dim;
	cell_I = CellLayout_MapElementIdToCellId( swarm->cellLayout, lElement_I );
	cellParticleCount = swarm->cellParticleCountTbl[cell_I];

	for( cParticle_I = 0 ; cParticle_I < cellParticleCount ; cParticle_I++ ) {
		particle = (IntegrationPoint*) Swarm_ParticleInCellAt( swarm, cell_I, cParticle_I );
		xi = particle->xi;

		detJac = ElementType_JacobianDeterminant( elementType, mesh, lElement_I, xi, dim );
		ElementType_EvaluateShapeFunctionsAt( elementType, xi, Ni );

		/* Density */
		err = PpcManager_Get( self->manager, lElement_I, particle, self->density_id, &density );

		Journal_Firewall( !err, 
			Journal_Register( Error_Type, (Name)BuoyancyForceTermPpc_Type  ), 
			"%d not found at the PpcManager\n", self->density_id );

		Journal_Firewall( !isnan( density ), 
			Journal_Register( Error_Type, (Name)BuoyancyForceTermPpc_Type  ), 
			"Density at integration point %i of element %i is nan\n", cParticle_I, lElement_I );

		/* Gravity */
		PpcManager_GetGravity( self->manager, lElement_I, particle, gravity );
		
		/* Apply force in the correct direction */
		for( eNode_I = 0 ; eNode_I < elementNodeCount; eNode_I++ ) {
		  for( dim_I = 0 ; dim_I < dim ; dim_I++ ){
			 factor = detJac * particle->weight * density * gravity[dim_I];
			 elForceVec[ eNode_I * nodeDofCount + dim_I ] += -1.0 * factor * Ni[ eNode_I ] ;
		  }
		}
	}
}


