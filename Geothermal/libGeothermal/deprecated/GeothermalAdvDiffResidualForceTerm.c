/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
** Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
** Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
** AuScope - http://www.auscope.org
** Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
** Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
** Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
** Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
** Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
** David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
** Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
** Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
** Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
** Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
** Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
** Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
**
**  This library is free software; you can redistribute it and/or
**  modify it under the terms of the GNU Lesser General Public
**  License as published by the Free Software Foundation; either
**  version 2.1 of the License, or (at your option) any later version.
**
**  This library is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public
**  License along with this library; if not, write to the Free Software
**  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include "types.h"
#include "GeothermalAdvDiffResidualForceTerm.h"

#include <assert.h>
#include <string.h>

/* Textual name of this class */
const Type GeothermalAdvDiffResidualForceTerm_Type = (const Type)"GeothermalAdvDiffResidualForceTerm";

void* _GeothermalAdvDiffResidualForceTerm_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                                  _sizeOfSelf = sizeof(GeothermalAdvDiffResidualForceTerm);
   Type                                                          type = GeothermalAdvDiffResidualForceTerm_Type;
   Stg_Class_DeleteFunction*                                  _delete = _AdvDiffResidualForceTerm_Delete;
   Stg_Class_PrintFunction*                                    _print = _AdvDiffResidualForceTerm_Print;
   Stg_Class_CopyFunction*                                      _copy = NULL;
   Stg_Component_DefaultConstructorFunction*      _defaultConstructor = _GeothermalAdvDiffResidualForceTerm_DefaultNew;
   Stg_Component_ConstructFunction*                        _construct = _GeothermalAdvDiffResidualForceTerm_AssignFromXML;
   Stg_Component_BuildFunction*                                _build = _GeothermalAdvDiffResidualForceTerm_Build;
   Stg_Component_InitialiseFunction*                      _initialise = _GeothermalAdvDiffResidualForceTerm_Initialise;
   Stg_Component_ExecuteFunction*                            _execute = _GeothermalAdvDiffResidualForceTerm_Execute;
   Stg_Component_DestroyFunction*                            _destroy = _GeothermalAdvDiffResidualForceTerm_Destroy;
   ForceTerm_AssembleElementFunction*                _assembleElement = _GeothermalAdvDiffResidualForceTerm_AssembleElement;
   AdvDiffResidualForceTerm_UpwindParamFunction*         _upwindParam = _AdvDiffResidualForceTerm_UpwindParam;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*)_GeothermalAdvDiffResidualForceTerm_New(  GEOTHERMALADVDIFFRESIDUALFORCETERM_PASSARGS  );
}

/* Creation implementation / Virtual constructor */
GeothermalAdvDiffResidualForceTerm* _GeothermalAdvDiffResidualForceTerm_New(  GEOTHERMALADVDIFFRESIDUALFORCETERM_DEFARGS  )
{
   GeothermalAdvDiffResidualForceTerm* self;

   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(GeothermalAdvDiffResidualForceTerm) );
   /* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
   /* This means that any values of these parameters that are passed into this function are not passed onto the parent function
      and so should be set to ZERO in any children of this class. */
   nameAllocationType = NON_GLOBAL;

   self = (GeothermalAdvDiffResidualForceTerm*) _ForceTerm_New(  FORCETERM_PASSARGS  );

   /* Virtual info */
   self->_upwindParam = _upwindParam;

   return self;
}

void _GeothermalAdvDiffResidualForceTerm_Init(
   void*   _self,
   double  densityFluid,
   double  densitySolid,
   double  specificHeatFluid,
   double  specificHeatSolid,
   double  thermalConductivityFluid,
   double  thermalConductivitySolid,
   double  porosity,
   BuoyancyMaterial* diffMaterial,
   Bool    specificVelocity)
{
   GeothermalAdvDiffResidualForceTerm* self = (GeothermalAdvDiffResidualForceTerm*)_self;
   double                              weightedVolumetricHeat;
   double                              velocityCorrection;
   FieldVariable*                      velCorrectionConstField;
   FeVariable*                         correctedVelocityField;

   self->densityFluid              = densityFluid;
   self->densitySolid              = densitySolid;
   self->specificHeatFluid         = specificHeatFluid;
   self->specificHeatSolid         = specificHeatSolid;
   self->thermalConductivityFluid  = thermalConductivityFluid;
   self->thermalConductivitySolid  = thermalConductivitySolid;
   self->porosity                  = porosity;
   self->diffMaterial              = diffMaterial;
   self->specificVelocity          = specificVelocity;

   /** now need to hijack some parent component settings.  perhaps this is a little hacky, but... */

   /** for now need to bomb out if attempting to use disable variable thermal conductivity as it wont work correctly. */
   Journal_Firewall( !self->diffusivityVariable, Journal_Register( Error_Type, (Name)self->type  ), "Cannot currently use variable diffusivity with the geothermal heat flow component" );

   weightedVolumetricHeat = (1. - self->porosity)*self->densitySolid*self->specificHeatSolid + self->porosity*self->densityFluid*self->specificHeatFluid;

   /** define velocity correction constant */
   if(self->specificVelocity)
      velocityCorrection = self->specificHeatFluid / weightedVolumetricHeat;
   else
      velocityCorrection = self->porosity*self->specificHeatFluid / weightedVolumetricHeat;

   /** create constant field which will be multiplied by velocity field */
   velCorrectionConstField = ConstantField_New( "velocityCorrection_ConstantField", 1, &velocityCorrection, self->velocityField->feMesh ) ;

   /** created corrected velocity field */
   correctedVelocityField  = OperatorFeVariable_NewBinary(  "correctedVelocityField", self->context, velCorrectionConstField, self->velocityField, "ScalarByVectorMultiplication" );

   /** replace velocityField */
   self->velocityField     = correctedVelocityField;

   /** set diffusion value... for now use algebraic mean */

   self->defaultDiffusivity = ( (1. - self->porosity)*self->thermalConductivitySolid + self->porosity*self->thermalConductivityFluid ) / weightedVolumetricHeat;

}


void _GeothermalAdvDiffResidualForceTerm_AssignFromXML( void* residual, Stg_ComponentFactory* cf, void* data ) {
   GeothermalAdvDiffResidualForceTerm*  self = (GeothermalAdvDiffResidualForceTerm*)residual;
   double                               densityFluid;
   double                               densitySolid;
   double                               specificHeatFluid;
   double                               specificHeatSolid;
   double                               thermalConductivityFluid;
   double                               thermalConductivitySolid;
   double                               porosity;
   Bool                                 specificVelocity;
   BuoyancyMaterial*                    diffMaterial;

   /* Construct Parent */
   _AdvDiffResidualForceTerm_AssignFromXML( self, cf, data );

   densityFluid              = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"densityFluid", 1.0  );
   densitySolid              = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"densitySolid", 1.0  );
   specificHeatFluid         = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"specificHeatFluid", 1.0  );
   specificHeatSolid         = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"specificHeatSolid", 1.0  );
   thermalConductivityFluid  = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"thermalConductivityFluid" , 1.0  );
   thermalConductivitySolid  = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"thermalConductivitySolid" , 1.0  );
   porosity                  = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"porosity"         , 1.0  );
   /* the specific velocity flag determines whether a specific velocity is provided by the user */
   specificVelocity          = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"specificVelocity"   , True );
   diffMaterial              = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"diffusionMaterial", BuoyancyMaterial, False, data  );
   _GeothermalAdvDiffResidualForceTerm_Init( self, densityFluid, densitySolid, specificHeatFluid, specificHeatSolid, thermalConductivityFluid, thermalConductivitySolid, porosity, diffMaterial, specificVelocity );
}

void _GeothermalAdvDiffResidualForceTerm_Build( void* residual, void* data ) {
   GeothermalAdvDiffResidualForceTerm* self = (GeothermalAdvDiffResidualForceTerm*)residual;

   _ForceTerm_Build( self, data );

   Stg_Component_Build( self->velocityField, data, False );

   if ( self->diffusivityVariable )
      Stg_Component_Build( self->diffusivityVariable, data, False );
}

void _GeothermalAdvDiffResidualForceTerm_Initialise( void* residual, void* data ) {
   GeothermalAdvDiffResidualForceTerm* self = (GeothermalAdvDiffResidualForceTerm*)residual;

   _ForceTerm_Initialise( self, data );

   Stg_Component_Initialise( self->velocityField, data, False );

   if ( self->diffusivityVariable )
      Stg_Component_Initialise( self->diffusivityVariable, data, False );
}

void _GeothermalAdvDiffResidualForceTerm_Execute( void* residual, void* data ) {
   GeothermalAdvDiffResidualForceTerm* self = (GeothermalAdvDiffResidualForceTerm*)residual;

   _ForceTerm_Execute( self, data );
}

void _GeothermalAdvDiffResidualForceTerm_Destroy( void* residual, void* data ) {
   GeothermalAdvDiffResidualForceTerm* self = (GeothermalAdvDiffResidualForceTerm*)residual;

   _ForceTerm_Destroy( self, data );
}

void _GeothermalAdvDiffResidualForceTerm_AssembleElement( void* forceTerm, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elementResidual ) {
	GeothermalAdvDiffResidualForceTerm*  self = Stg_CheckType( forceTerm, AdvDiffResidualForceTerm );
	AdvectionDiffusionSLE*     sle                = Stg_CheckType( self->extraInfo, AdvectionDiffusionSLE );
	Swarm*                     swarm              = self->integrationSwarm;
	Particle_Index             lParticle_I;
	Particle_Index             cParticle_I;
	Particle_Index             cellParticleCount;
	Cell_Index                 cell_I;    
	IntegrationPoint*          particle;
	FeVariable*                phiField           = sle->phiField;
	Dimension_Index            dim                = forceVector->dim;
	double                     velocity[3];
	double                     phi, phiDot;
	double                     detJac;
	double*                    xi;
	double                     totalDerivative, diffusionTerm;
	double                     diffusivity         = self->defaultDiffusivity;
	double                     heating=0.;
	Variable*                  diffusivityVariable = self->diffusivityVariable;
	ElementType*               elementType         = FeMesh_GetElementType( phiField->feMesh, lElement_I );
	Node_Index                 elementNodeCount    = elementType->nodeCount;
	Node_Index                 node_I;
	double                     factor;

	double**                   GNx;
	double*                    phiGrad;
	double*                    Ni;
	double*                    SUPGNi;
	double                     supgfactor;
	double                     udotu, perturbation;
	double                     upwindDiffusivity;
   Coord                      globalCoord;
	GNx     = self->GNx;
	phiGrad = self->phiGrad;
	Ni = self->Ni;
	SUPGNi = self->SUPGNi;
	
	upwindDiffusivity  = _GeothermalAdvDiffResidualForceTerm_UpwindDiffusivity( self, sle, swarm, phiField->feMesh, lElement_I, dim );

	/* Determine number of particles in element */
	cell_I = CellLayout_MapElementIdToCellId( swarm->cellLayout, lElement_I );
	cellParticleCount = swarm->cellParticleCountTbl[ cell_I ];
	
	for ( cParticle_I = 0 ; cParticle_I < cellParticleCount ; cParticle_I++ ) {
		lParticle_I     = swarm->cellParticleTbl[cell_I][cParticle_I];

		particle        = (IntegrationPoint*) Swarm_ParticleAt( swarm, lParticle_I );
		xi              = particle->xi;
		
		/* Evaluate Shape Functions */
		ElementType_EvaluateShapeFunctionsAt(elementType, xi, Ni);

		/* Calculate Global Shape Function Derivatives */
		ElementType_ShapeFunctionsGlobalDerivs( 
			elementType,
			phiField->feMesh, lElement_I,
			xi, dim, &detJac, GNx );

		FeMesh_CoordLocalToGlobal( phiField->feMesh, lElement_I, xi, globalCoord );
		
		/* Calculate Velocity */
      memset( velocity, 0, sizeof( double ) * 3 );
      FieldVariable_InterpolateValueAt( self->velocityField, globalCoord, velocity );
		/* Build the SUPG shape functions */
		udotu = velocity[I_AXIS]*velocity[I_AXIS] + velocity[J_AXIS]*velocity[J_AXIS];
		if(dim == 3) udotu += velocity[ K_AXIS ] * velocity[ K_AXIS ];

		supgfactor = upwindDiffusivity / udotu;
		for ( node_I = 0 ; node_I < elementNodeCount ; node_I++ ) {
			/* In the case of per diffusion - just build regular shape functions */
			if ( fabs(upwindDiffusivity) < SUPG_MIN_DIFFUSIVITY ) {
				SUPGNi[node_I] = Ni[node_I];
				continue;
			}
			
			perturbation = velocity[ I_AXIS ] * GNx[ I_AXIS ][ node_I ] + velocity[ J_AXIS ] * GNx[ J_AXIS ][ node_I ];
			if (dim == 3)
					perturbation = perturbation + velocity[ K_AXIS ] * GNx[ K_AXIS ][ node_I ];
			
			/* p = \frac{\bar \kappa \hat u_j w_j }{ ||u|| } -  Eq. 3.2.25 */
			perturbation = supgfactor * perturbation;
			
			SUPGNi[node_I] = Ni[node_I] + perturbation;
		}  
		
		/* Calculate phi on particle */
		_FeVariable_InterpolateNodeValuesToElLocalCoord( phiField, lElement_I, xi, &phi );

		/* Calculate Gradients of Phi */
		FeVariable_InterpolateDerivatives_WithGNx( phiField, lElement_I, GNx, phiGrad );

		/* Calculate time derivative of phi */
		_FeVariable_InterpolateNodeValuesToElLocalCoord( sle->phiDotField, lElement_I, xi, &phiDot );
		
		/* Calculate total derivative (i.e. Dphi/Dt = \dot \phi + u . \grad \phi) */
		totalDerivative = phiDot + StGermain_VectorDotProduct( velocity, phiGrad, dim );

		/* Get Diffusivity */
		/* diffusivityVariable will only be NOT NULL if:
		 * 1) The MaterialDiffusivityPlugin is used. It's in Underworld/Plugins
		 * 2) A special user defined DiffusivityVariable is given during the Construction phase
		 */  
		if ( diffusivityVariable != NULL )
			diffusivity = self->_getDiffusivityFromIntPoint( self, particle );
		if( self->diffMaterial ) {
         RheologyMaterial* material;
         BuoyancyMaterial_MaterialExt*	    materialExt;         
			material = (RheologyMaterial*) IntegrationPointsSwarm_GetMaterialOn( (IntegrationPointsSwarm*)swarm, particle );
			materialExt = ExtensionManager_Get( material->extensionMgr, material, self->diffMaterial->materialExtHandle );
			diffusivity = materialExt->extensions[0];
			heating     = materialExt->extensions[1];
		}

		/* Add to element residual */
		factor = particle->weight * detJac;
		for ( node_I = 0 ; node_I < elementNodeCount ; node_I++ ) {
			/* Calculate Diffusion Term */
			diffusionTerm = diffusivity * ( GNx[0][node_I] * phiGrad[0] + GNx[1][node_I] * phiGrad[1] );
			if (dim == 3)
				diffusionTerm += diffusivity * GNx[2][ node_I ] * phiGrad[2] ;
			
			elementResidual[ node_I ] -=  factor * ( SUPGNi[ node_I ] * ( totalDerivative - heating ) + diffusionTerm );
		}
	}
	
}

/** AdvectionDiffusion_UpwindDiffusivity - See Brooks, Hughes 1982 Section 3.3 
 * All equations refer to this paper if not otherwise indicated */
double _GeothermalAdvDiffResidualForceTerm_UpwindDiffusivity( 
		GeothermalAdvDiffResidualForceTerm* self, 
		AdvectionDiffusionSLE* sle, 
		Swarm* swarm, 
		FeMesh* mesh, 
		Element_LocalIndex lElement_I, 
		Dimension_Index dim )
{
	FeVariable*                velocityField   = self->velocityField;
	Coord                      xiElementCentre = {0.0,0.0,0.0};
	double                     xiUpwind;
	double                     velocityCentre[3];
	double                     pecletNumber;
	double                     lengthScale;
	double                     upwindDiffusivity;
	Dimension_Index            dim_I;
	double*                    leastCoord;
	double*                    greatestCoord;
	Node_LocalIndex            nodeIndex_LeastValues, nodeIndex_GreatestValues;
	unsigned                   nInc, *inc;
	IArray*		 incArray;
	
	Cell_Index                 cell_I;
	IntegrationPoint*          particle;
	Variable*                  diffusivityVariable = self->diffusivityVariable;
	Particle_Index             lParticle_I;
	double                     averageDiffusivity;
	Particle_InCellIndex       cParticle_I;
	Particle_InCellIndex       particleCount;
   Coord                      globalCoord;

	
	/* Compute the average diffusivity */
	/* Find Number of Particles in Element */
	cell_I = CellLayout_MapElementIdToCellId( swarm->cellLayout, lElement_I );
	particleCount = swarm->cellParticleCountTbl[ cell_I ];

	/* Average diffusivity for element */
	if ( diffusivityVariable ) {
		averageDiffusivity = 0.0;
		for ( cParticle_I = 0 ; cParticle_I < particleCount ; cParticle_I++ ) {
			lParticle_I = swarm->cellParticleTbl[lElement_I][cParticle_I];
			particle    = (IntegrationPoint*)Swarm_ParticleInCellAt( swarm, cell_I, cParticle_I );
			averageDiffusivity += self->_getDiffusivityFromIntPoint( self, particle );
		}
		averageDiffusivity /= (double)particleCount;
	} else if ( self->diffMaterial ) {
      RheologyMaterial*             material;
      BuoyancyMaterial_MaterialExt* materialExt;         
		averageDiffusivity = 0.0;
		for ( cParticle_I = 0 ; cParticle_I < particleCount ; cParticle_I++ ) {
			particle    = (IntegrationPoint*)Swarm_ParticleInCellAt( swarm, cell_I, cParticle_I );
			material    = (RheologyMaterial*) IntegrationPointsSwarm_GetMaterialOn( (IntegrationPointsSwarm*)swarm, particle );
			materialExt = ExtensionManager_Get( material->extensionMgr, material, self->diffMaterial->materialExtHandle );
			averageDiffusivity += materialExt->extensions[0];;
		}
      averageDiffusivity /= (double)particleCount;
	} else {
		averageDiffusivity = self->defaultDiffusivity;
	}
	
	if (sle->maxDiffusivity < averageDiffusivity)
		sle->maxDiffusivity = averageDiffusivity;
		
	/* Change Diffusivity if it is too small */
	if ( averageDiffusivity < SUPG_MIN_DIFFUSIVITY ) 
		averageDiffusivity = SUPG_MIN_DIFFUSIVITY;
	
   FeMesh_CoordLocalToGlobal( mesh, lElement_I, xiElementCentre, globalCoord );
   
	/* Calculate Velocity At Middle of Element - See Eq. 3.3.6 */
	memset( velocityCentre, 0, sizeof( double ) * 3 );
   FieldVariable_InterpolateValueAt( velocityField, globalCoord, velocityCentre );
	
	/* Calculate Length Scales - See Fig 3.4 - ASSUMES BOX MESH TODO - fix */
	incArray = self->incarray;
	FeMesh_GetElementNodes( mesh, lElement_I, incArray );
	nInc = IArray_GetSize( incArray );
	inc = IArray_GetPtr( incArray );
	
	
	nodeIndex_LeastValues = inc[0];
	nodeIndex_GreatestValues = (dim == 2) ? inc[3] : (dim == 3) ? inc[7] : inc[1];
	leastCoord    = Mesh_GetVertex( mesh, nodeIndex_LeastValues );
	greatestCoord = Mesh_GetVertex( mesh, nodeIndex_GreatestValues );
	
	upwindDiffusivity = 0.0;
	for ( dim_I = 0 ; dim_I < dim ; dim_I++ ) {
		lengthScale = greatestCoord[ dim_I ] - leastCoord[ dim_I ];
		
		/* Calculate Peclet Number (alpha) - See Eq. 3.3.5 */
		pecletNumber = velocityCentre[ dim_I ] * lengthScale / (2.0 * averageDiffusivity);
		
		/* Calculate Upwind Local Coordinate - See Eq. 3.3.4 and (2.4.2, 3.3.1 and 3.3.2) */
		xiUpwind = AdvDiffResidualForceTerm_UpwindParam( self, pecletNumber );
		
		/* Calculate Upwind Thermal Diffusivity - See Eq. 3.3.3  */
		upwindDiffusivity += xiUpwind * velocityCentre[ dim_I ] * lengthScale;
	}
	upwindDiffusivity *= ISQRT15;         /* See Eq. 3.3.11 */	
	
	return upwindDiffusivity;
}

