#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include "types.h"
#include "SUPGAdvDiffTermPpc.h"

#include <assert.h>
#include <string.h>

/* Textual name of this class */
const Type SUPGAdvDiffTermPpc_Type = "SUPGAdvDiffTermPpc";

void* _SUPGAdvDiffTermPpc_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                                  _sizeOfSelf = sizeof(SUPGAdvDiffTermPpc);
   Type                                                          type = SUPGAdvDiffTermPpc_Type;
   Stg_Class_DeleteFunction*                                  _delete = _SUPGAdvDiffTermPpc_Delete;
   Stg_Class_PrintFunction*                                    _print = NULL;
   Stg_Class_CopyFunction*                                      _copy = NULL;
   Stg_Component_DefaultConstructorFunction*      _defaultConstructor = _SUPGAdvDiffTermPpc_DefaultNew;
   Stg_Component_ConstructFunction*                        _construct = _SUPGAdvDiffTermPpc_AssignFromXML;
   Stg_Component_BuildFunction*                                _build = _SUPGAdvDiffTermPpc_Build;
   Stg_Component_InitialiseFunction*                      _initialise = _SUPGAdvDiffTermPpc_Initialise;
   Stg_Component_ExecuteFunction*                            _execute = _SUPGAdvDiffTermPpc_Execute;
   Stg_Component_DestroyFunction*                            _destroy = _SUPGAdvDiffTermPpc_Destroy;
   ForceTerm_AssembleElementFunction*                _assembleElement = _SUPGAdvDiffTermPpc_AssembleElement;
   AdvDiffResidualForceTerm_UpwindParamFunction*         _upwindParam = _SUPGAdvDiffTermPpc_UpwindParam;

   /* 
    * Variables that are set to ZERO are variables that will be set either by the 
    * current _New function or another parent _New function further up the hierachy.
    */

   /* default value NON_GLOBAL */
   AllocationType  nameAllocationType = NON_GLOBAL;

   return (void*)_SUPGAdvDiffTermPpc_New( SUPGADVDIFFTERM_PASSARGS );
}

/* Creation implementation / Virtual constructor */
SUPGAdvDiffTermPpc* _SUPGAdvDiffTermPpc_New( SUPGADVDIFFTERM_DEFARGS ) {
   SUPGAdvDiffTermPpc* self;
   
   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(SUPGAdvDiffTermPpc) );

   /* 
    * The following terms are parameters that have been passed into this
    * function but are being set before being passed onto the parent. 
    * This means that any values of these parameters that are passed into this.
    * function are not passed onto the parent function
    * and so should be set to ZERO in any children of this class. 
    */
   nameAllocationType = NON_GLOBAL;

   self = (SUPGAdvDiffTermPpc*) _ForceTerm_New( FORCETERM_PASSARGS );
   
   /* Virtual info */
   self->_upwindParam = _upwindParam;
   
   return self;
}

void _SUPGAdvDiffTermPpc_Init(
   void*                  residual,
   PpcManager*            mgr,
   AdvectionDiffusionSLE* sle,
   FeVariable*            velocityField,
   FeVariable*            phiField,
   char*                  upwindParamFuncName,
   int                    diffusivityLabel )
{
   SUPGAdvDiffTermPpc* self = (SUPGAdvDiffTermPpc*)residual;

   self->mgr = mgr;
   self->sle = sle;
   self->velocityField = velocityField;
   self->phiField = phiField;
   self->diffusivityLabel = diffusivityLabel;
   self->last_maxNodeCount = 0;

   /* error check the upwinding scheme chosen */
   if ( strcasecmp( upwindParamFuncName, "DoublyAsymptoticAssumption" ) == 0 )
      self->upwindParamType = DoublyAsymptoticAssumption;
   else if ( strcasecmp( upwindParamFuncName, "CriticalAssumption" ) == 0 )
      self->upwindParamType = CriticalAssumption;
   else if ( strcasecmp( upwindParamFuncName, "Exact" ) == 0 )
      self->upwindParamType = Exact;
   else {
      Journal_Firewall(
         False,
         Journal_Register( Error_Type, (Name)self->type  ),
         "Cannot understand '%s'\n",
         upwindParamFuncName );
   }
}

void _SUPGAdvDiffTermPpc_Delete( void* residual ) {
   SUPGAdvDiffTermPpc* self = (SUPGAdvDiffTermPpc*)residual;

   _ForceTerm_Delete( self );
}

void _SUPGAdvDiffTermPpc_AssignFromXML( void* residual, Stg_ComponentFactory* cf, void* data ) {
   SUPGAdvDiffTermPpc* self = (SUPGAdvDiffTermPpc*)residual;
   PpcManager*         mgr = NULL;

   /* Construct Parent */
   _ForceTerm_AssignFromXML( self, cf, data );

   /* The PpcManager */
   mgr = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Manager", PpcManager, False, data );

   if ( !mgr  )
      mgr = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_ppcManager", PpcManager, True, data );

   _SUPGAdvDiffTermPpc_Init(
      self,
      mgr,
      Stg_ComponentFactory_ConstructByNameWithKeyFallback(
         cf,
         self->name,
         (Dictionary_Entry_Key)"default_EnergyEqn",
         (Dictionary_Entry_Key)"EnergyEqn",
         AdvectionDiffusionSLE, True, data ), 
      Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"VelocityField", FeVariable, True, data ), 
      Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"PhiField", FeVariable, True, data ), 
      Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"UpwindXiFunction", "Exact" ), 
      PpcManager_GetPpcFromDict( mgr, cf, self->name, (Dictionary_Entry_Key)"DiffusivityLabel", "DiffusivityLabel" ) );
}

void _SUPGAdvDiffTermPpc_Build( void* residual, void* data ) {
   SUPGAdvDiffTermPpc* self = (SUPGAdvDiffTermPpc*)residual;

   _ForceTerm_Build( self, data );

   //Stg_Component_Build( self->sle, data, False );
   Stg_Component_Build( self->velocityField, data, False );
   Stg_Component_Build( self->phiField, data, False );
   Stg_Component_Build( self->mgr, data, False );

}

void _SUPGAdvDiffTermPpc_Initialise( void* residual, void* data ) {
   SUPGAdvDiffTermPpc* self = (SUPGAdvDiffTermPpc*)residual;

   _ForceTerm_Initialise( self, data );

   //Stg_Component_Initialise( self->sle, data, False );
   Stg_Component_Initialise( self->velocityField, data, False );
   Stg_Component_Initialise( self->phiField, data, False );
   Stg_Component_Initialise( self->mgr, data, False );
}

void _SUPGAdvDiffTermPpc_Execute( void* residual, void* data ) {
   SUPGAdvDiffTermPpc* self = (SUPGAdvDiffTermPpc*)residual;

   _ForceTerm_Execute( self, data );
}

void _SUPGAdvDiffTermPpc_Destroy( void* residual, void* data ) {
   SUPGAdvDiffTermPpc* self = (SUPGAdvDiffTermPpc*)residual;

   //Stg_Component_Destroy( self->sle, data, False );
   Stg_Component_Destroy( self->velocityField, data, False );
   Stg_Component_Destroy( self->phiField, data, False );
   Stg_Component_Destroy( self->mgr, data, False );

   self->last_maxNodeCount = 0;
   _ForceTerm_Destroy( self, data );
}

void _SUPGAdvDiffTermPpc_AssembleElement(
   void*              forceTerm,
   ForceVector*       forceVector,
   Element_LocalIndex lElement_I,
   double*            elementResidual )
{
   SUPGAdvDiffTermPpc*    self = Stg_CheckType( forceTerm, SUPGAdvDiffTermPpc );
   AdvectionDiffusionSLE* sle = self->sle;
   Swarm*                 swarm = self->integrationSwarm;
   Particle_Index         lParticle_I;
   Particle_Index         cParticle_I;
   Particle_Index         cellParticleCount;
   Cell_Index             cell_I;    
   IntegrationPoint*      particle;
   FeVariable*            phiField = self->phiField;
   Dimension_Index        dim = forceVector->dim;
   double                 velocity[3];
   double                 phi, phiDot;
   double                 detJac;
   double*                xi;
   double                 totalDerivative, diffusionTerm;
   double                 diffusivity = 0;
   ElementType*           elementType = FeMesh_GetElementType( phiField->feMesh, lElement_I );
   Node_Index             elementNodeCount = elementType->nodeCount;
   Node_Index             node_I;
   double                 factor;
   double**               GNx;
   double*                phiGrad;
   double*                Ni;
   double*                SUPGNi;
   double                 supgfactor;
   double                 udotu, perturbation;
   double                 upwindDiffusivity;
   int                    err;

   GNx = self->GNx;
   phiGrad = self->phiGrad;
   Ni = self->Ni;
   SUPGNi = self->SUPGNi;
   
   upwindDiffusivity = SUPGAdvDiffTermPpc_UpwindDiffusivity( self, sle, swarm, phiField->feMesh, lElement_I, dim );

   /* Determine number of particles in element */
   cell_I = CellLayout_MapElementIdToCellId( swarm->cellLayout, lElement_I );
   cellParticleCount = swarm->cellParticleCountTbl[ cell_I ];
   
   for ( cParticle_I = 0 ; cParticle_I < cellParticleCount ; cParticle_I++ ) {
      particle = (IntegrationPoint*)Swarm_ParticleInCellAt( swarm, cell_I, cParticle_I );
      xi          = particle->xi;
      
      /* Evaluate Shape Functions */
      ElementType_EvaluateShapeFunctionsAt(elementType, xi, Ni);

      /* Calculate Global Shape Function Derivatives */
      ElementType_ShapeFunctionsGlobalDerivs( 
         elementType,
         phiField->feMesh, lElement_I,
         xi, dim, &detJac, GNx );
      
      /* Calculate Velocity */
      FeVariable_InterpolateFromMeshLocalCoord( self->velocityField, phiField->feMesh, lElement_I, xi, velocity );

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

      /* get the diffusivity */
      err = PpcManager_Get( self->mgr, lElement_I, particle, self->diffusivityLabel, &diffusivity );
      if( err ) assert(0);

      /* Add to element residual */
      factor = particle->weight * detJac;
      for( node_I = 0 ; node_I < elementNodeCount ; node_I++ ) {
         /* Calculate Diffusion Term */
         diffusionTerm = diffusivity * ( GNx[0][node_I] * phiGrad[0] + GNx[1][node_I] * phiGrad[1] );
         if(dim == 3)
            diffusionTerm += diffusivity * GNx[2][ node_I ] * phiGrad[2] ;
         
         elementResidual[ node_I ] -=  factor * ( SUPGNi[ node_I ] * totalDerivative + diffusionTerm );
      }
   }
}

/* Virtual Function Implementations */
double _SUPGAdvDiffTermPpc_UpwindParam( void* residual, double pecletNumber ) {
   SUPGAdvDiffTermPpc* self = (SUPGAdvDiffTermPpc*)residual;

   switch ( self->upwindParamType ) {
      case Exact:
         self->_upwindParam = SUPGAdvDiffTermPpc_UpwindXiExact; break;
      case DoublyAsymptoticAssumption:
         self->_upwindParam = SUPGAdvDiffTermPpc_UpwindXiDoublyAsymptoticAssumption; break;
      case CriticalAssumption:
         self->_upwindParam = SUPGAdvDiffTermPpc_UpwindXiCriticalAssumption; break;
   }
   return self->_upwindParam( self, pecletNumber );
}

#define ISQRT15 0.25819888974716112567

/** AdvectionDiffusion_UpwindDiffusivity - See Brooks, Hughes 1982 Section 3.3 
 * All equations refer to this paper if not otherwise indicated */
double SUPGAdvDiffTermPpc_UpwindDiffusivity( 
   SUPGAdvDiffTermPpc*    self, 
   AdvectionDiffusionSLE* sle, 
   Swarm*                 swarm, 
   FeMesh*                mesh, 
   Element_LocalIndex     lElement_I, 
   Dimension_Index        dim )
{
   FeVariable*          velocityField = self->velocityField;
   Coord                xiElementCentre = {0.0,0.0,0.0};
   double               xiUpwind;
   double               velocityCentre[3];
   double               pecletNumber;
   double               lengthScale;
   double               upwindDiffusivity;
   Dimension_Index      dim_I;
   double*              leastCoord;
   double*              greatestCoord;
   Node_LocalIndex      nodeIndex_LeastValues, nodeIndex_GreatestValues;
   unsigned             nInc, *inc;
   IArray*              incArray;
   Cell_Index           cell_I;
   IntegrationPoint*    particle;
   Particle_Index       lParticle_I;
   double               averageDiffusivity;
   Particle_InCellIndex cParticle_I;
   Particle_InCellIndex particleCount;
   int                  err;
   double               diffusivity;

   
   /* Compute the average diffusivity */
   /* Find Number of Particles in Element */
   cell_I = CellLayout_MapElementIdToCellId( swarm->cellLayout, lElement_I );
   particleCount = swarm->cellParticleCountTbl[ cell_I ];

   /* Average diffusivity for element */
   averageDiffusivity = 0.0;
   for ( cParticle_I = 0 ; cParticle_I < particleCount ; cParticle_I++ ) {
      particle = (IntegrationPoint*)Swarm_ParticleInCellAt( swarm, cell_I, cParticle_I );
      err = PpcManager_Get( self->mgr, lElement_I, particle, self->diffusivityLabel, &diffusivity );
      assert(!err);
      averageDiffusivity += diffusivity;
   }
   averageDiffusivity /= (double)particleCount;
   
   if (sle->maxDiffusivity < averageDiffusivity)
      sle->maxDiffusivity = averageDiffusivity;
   
   /* Change Diffusivity if it is too small */
   if ( averageDiffusivity < SUPG_MIN_DIFFUSIVITY ) 
      averageDiffusivity = SUPG_MIN_DIFFUSIVITY;
   
   /* Calculate Velocity At Middle of Element - See Eq. 3.3.6 */
   FeVariable_InterpolateFromMeshLocalCoord( velocityField, mesh, lElement_I, xiElementCentre, velocityCentre );
   
   /* Calculate Length Scales - See Fig 3.4 - ASSUMES BOX MESH TODO - fix */
   incArray = self->incarray;
   FeMesh_GetElementNodes( mesh, lElement_I, incArray );
   nInc = IArray_GetSize( incArray );
   inc = IArray_GetPtr( incArray );
   
   nodeIndex_LeastValues = inc[0];
   nodeIndex_GreatestValues = (dim == 2) ? inc[3] : (dim == 3) ? inc[7] : inc[1];
   leastCoord = Mesh_GetVertex( mesh, nodeIndex_LeastValues );
   greatestCoord = Mesh_GetVertex( mesh, nodeIndex_GreatestValues );
   
   upwindDiffusivity = 0.0;
   for ( dim_I = 0 ; dim_I < dim ; dim_I++ ) {
      lengthScale = greatestCoord[ dim_I ] - leastCoord[ dim_I ];
      
      /* Calculate Peclet Number (alpha) - See Eq. 3.3.5 */
      pecletNumber = velocityCentre[ dim_I ] * lengthScale / (2.0 * averageDiffusivity);
      
      /* Calculate Upwind Local Coordinate - See Eq. 3.3.4 and (2.4.2, 3.3.1 and 3.3.2) */
      xiUpwind = self->_upwindParam( self, pecletNumber );
      
      /* Calculate Upwind Thermal Diffusivity - See Eq. 3.3.3  */
      upwindDiffusivity += xiUpwind * velocityCentre[ dim_I ] * lengthScale;
   }
   upwindDiffusivity *= ISQRT15; /* See Eq. 3.3.11 */
   
   return upwindDiffusivity;
}

/** AdvectionDiffusion_UpwindXiExact - Brooks, Hughes 1982 equation 2.4.2
 *\f$ \bar \xi = coth( \alpha ) - \frac{1}{\alpha} \f$ */
double SUPGAdvDiffTermPpc_UpwindXiExact( void* residual, double pecletNumber ) {
   if (fabs(pecletNumber) < 1.0e-8 )
      return 0.33333333333333 * pecletNumber;
   else if (pecletNumber < -20.0)
      return -1.0 - 1.0/pecletNumber;
   else if (pecletNumber > 20.0)
      return +1.0 - 1.0/pecletNumber;
      
   return cosh( pecletNumber )/sinh( pecletNumber ) - 1.0/pecletNumber;
}

/** AdvectionDiffusion_UpwindXiDoublyAsymptoticAssumption - Brooks, Hughes 1982 equation 3.3.1
 * Simplification of \f$ \bar \xi = coth( \alpha ) - \frac{1}{\alpha} \f$ from Brooks, Hughes 1982 equation 2.4.2
 * \f[
   \bar \xi \sim \left\{ \begin{array}{rl} 
   -1                 &for \quad \alpha <= -3 \\
   \frac{\alpha}{3}   &for \quad -3 < \alpha <= 3 \\
   +1                 &for \quad \alpha > +3
   \end{array} \right.  
           
   \f]*/
double SUPGAdvDiffTermPpc_UpwindXiDoublyAsymptoticAssumption( void* residual, double pecletNumber ) {
   if (pecletNumber <= -3.0)
      return -1;
   else if (pecletNumber <= 3.0)
      return 0.33333333333333 * pecletNumber;
   else
      return 1.0;
}
   
/** AdvectionDiffusion_UpwindXiCriticalAssumption - Brooks, Hughes 1982 equation 3.3.2
 * Simplification of \f$ \bar \xi = coth( \alpha ) - \frac{1}{\alpha} \f$ from Brooks, Hughes 1982 equation 2.4.2
 * \f[
   \bar \xi \sim \left\{ \begin{array}{rl}
   -1 - \frac{1}{\alpha}   &for \quad \alpha <= -1 \\
   0                      &for \quad -1 < \alpha <= +1 \\
   +1 - \frac{1}{\alpha}   &for \quad \alpha > +1          
              \end{array} \right.              
   \f] */

double SUPGAdvDiffTermPpc_UpwindXiCriticalAssumption( void* residual, double pecletNumber ) {
   if (pecletNumber <= -1.0)
      return -1.0 - 1.0/pecletNumber;
   else if (pecletNumber <= 1.0)
      return 0.0;
   else
      return 1.0 - 1.0/pecletNumber;
}
