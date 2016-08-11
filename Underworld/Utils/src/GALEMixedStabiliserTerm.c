/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd,
** 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <string.h>
#include <assert.h>
#include <mpi.h>

#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include "types.h"
#include "GALEMixedStabiliserTerm.h"


const Type GALEMixedStabiliserTerm_Type = "GALEMixedStabiliserTerm";


void* _GALEMixedStabiliserTerm_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                 _sizeOfSelf = sizeof(GALEMixedStabiliserTerm);
	Type                                                         type = GALEMixedStabiliserTerm_Type;
	Stg_Class_DeleteFunction*                                 _delete = _GALEMixedStabiliserTerm_Delete;
	Stg_Class_PrintFunction*                                   _print = _StiffnessMatrixTerm_Print;
	Stg_Class_CopyFunction*                                     _copy = NULL;
	Stg_Component_DefaultConstructorFunction*     _defaultConstructor = _GALEMixedStabiliserTerm_DefaultNew;
	Stg_Component_ConstructFunction*                       _construct = _GALEMixedStabiliserTerm_AssignFromXML;
	Stg_Component_BuildFunction*                               _build = _GALEMixedStabiliserTerm_Build;
	Stg_Component_InitialiseFunction*                     _initialise = _GALEMixedStabiliserTerm_Initialise;
	Stg_Component_ExecuteFunction*                           _execute = _GALEMixedStabiliserTerm_Execute;
	Stg_Component_DestroyFunction*                           _destroy = _GALEMixedStabiliserTerm_Destroy;
	StiffnessMatrixTerm_AssembleElementFunction*     _assembleElement = _GALEMixedStabiliserTerm_AssembleElement;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*)_GALEMixedStabiliserTerm_New(  MIXEDSTABILISERTERM_PASSARGS  );
}

/* Creation implementation / Virtual constructor */
GALEMixedStabiliserTerm* _GALEMixedStabiliserTerm_New(   MIXEDSTABILISERTERM_DEFARGS  )
{
	GALEMixedStabiliserTerm* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(GALEMixedStabiliserTerm) );
	self = (GALEMixedStabiliserTerm*) _StiffnessMatrixTerm_New(  STIFFNESSMATRIXTERM_PASSARGS  );

   return self;
}

void _GALEMixedStabiliserTerm_Init( GALEMixedStabiliserTerm* self ) {
   self->picSwarm = NULL;
   self->storeVisc = NULL;
   self->Ni = NULL;
   self->elStiffMat = NULL;
}

void _GALEMixedStabiliserTerm_Delete( void* _self ) {
   GALEMixedStabiliserTerm* self = (GALEMixedStabiliserTerm*)_self;

   if( self->Ni )
      Memory_Free( self->Ni );
   if( self->elStiffMat )
      Memory_Free( self->elStiffMat );

   _StiffnessMatrixTerm_Delete( self );
}

void _GALEMixedStabiliserTerm_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
   GALEMixedStabiliserTerm* self = (GALEMixedStabiliserTerm*)_self;

   _StiffnessMatrixTerm_AssignFromXML( self, cf, data );
   _GALEMixedStabiliserTerm_Init( self );

   self->picSwarm = Stg_ComponentFactory_ConstructByKey( cf, self->name, "picSwarm", IntegrationPointsSwarm, True, data );
   self->storeVisc = Stg_ComponentFactory_ConstructByKey( cf, self->name, "storeVisc", StoreVisc, True, data );
}

void _GALEMixedStabiliserTerm_Build( void* _self, void* data ) {
   GALEMixedStabiliserTerm* self = (GALEMixedStabiliserTerm*)_self;

   _StiffnessMatrixTerm_Build( self, data );
}

void _GALEMixedStabiliserTerm_Initialise( void* _self, void* data ) {
   GALEMixedStabiliserTerm* self = (GALEMixedStabiliserTerm*)_self;
   _StiffnessMatrixTerm_Initialise( self, data );
}

void _GALEMixedStabiliserTerm_Execute( void* _self, void* data ) {
   _StiffnessMatrixTerm_Execute( _self, data );
}

void _GALEMixedStabiliserTerm_Destroy( void* _self, void* data ) {
   _StiffnessMatrixTerm_Destroy( _self, data );
}

void _GALEMixedStabiliserTerm_AssembleElement( void* _self,
					  StiffnessMatrix* stiffMat,
					  Element_LocalIndex elementIndex,
					  SystemLinearEquations* _sle,
					  FiniteElementContext* ctx,
					  double** elStiffMat )
{
   GALEMixedStabiliserTerm* self = (GALEMixedStabiliserTerm*)_self;
   Stokes_SLE* sle = Stg_DCheckType( _sle, Stokes_SLE );
   StiffnessMatrix* gMatrix = sle->gStiffMat;
   FeVariable* colVar  = gMatrix->columnVariable;
   FeMesh* mesh = colVar->feMesh;
   int nDims = Mesh_GetDimSize( mesh );
   double *xi, weight, jacDet, weightJacDet, cellArea = 0.0;
   double *Ni, **GNx, viscFac;
   int nParticles, nElNodes, cellIndex;
   ElementType* elementType;
   IntegrationPointsSwarm* swarm;
   IntegrationPoint* integrationPoint;
   double** localElStiffMat;
   int ii, jj, kk;
   double sumVisc = 0.0, visc;

   /* Get the current element's type and cache the number
      of nodes. */
   elementType = FeMesh_GetElementType( mesh, elementIndex );
   nElNodes = elementType->nodeCount;

   /* If we don't already have arrays, allocate them now. */
   if( !self->Ni && !self->GNx && !self->elStiffMat ) {
      self->Ni = Memory_Alloc_Array( double, nElNodes, GALEMixedStabiliserTerm_Type );
      self->GNx = Memory_Alloc_2DArray( double, nDims, nElNodes, GALEMixedStabiliserTerm_Type );
      self->elStiffMat = Memory_Alloc_2DArray( double, nElNodes, nElNodes, GALEMixedStabiliserTerm_Type );
   }

   /* Cache array pointers stored on the mesh. */
   Ni = self->Ni;
   GNx = self->GNx;
   localElStiffMat = self->elStiffMat;

   /* Zero the local element stiffness matrix. */
   for( ii = 0; ii < nElNodes; ii++ )
      memset( localElStiffMat[ii], 0, nElNodes * sizeof(double) );

   /* Assemble the mass matrix portion. */
   swarm = (IntegrationPointsSwarm*)(self->integrationSwarm);
   cellIndex = CellLayout_MapElementIdToCellId( swarm->cellLayout,
                                                elementIndex );
   nParticles = swarm->cellParticleCountTbl[cellIndex];
   for( ii = 0; ii < nParticles; ii++ ) {

      /* Cache information from the current integration point. */
      integrationPoint = (IntegrationPoint*)Swarm_ParticleInCellAt(
         swarm, cellIndex, ii );
      xi = integrationPoint->xi;
      weight = integrationPoint->weight;
      ElementType_EvaluateShapeFunctionsAt( elementType, xi, Ni );
      ElementType_ShapeFunctionsGlobalDerivs(
         elementType, mesh, elementIndex, xi, nDims, &jacDet, GNx );
      weightJacDet = weight * jacDet;

      /* Loop over element nodes. */
      for( jj = 0 ; jj < nElNodes; jj++ ) {
         for ( kk = 0 ; kk < nElNodes ; kk++ ) {
            localElStiffMat[jj][kk] += weightJacDet * Ni[jj] * Ni[kk];
         }
      }
   }

   /* Calculate the cell's area and viscosity. */
   swarm = self->picSwarm;
   cellIndex = CellLayout_MapElementIdToCellId( swarm->cellLayout, elementIndex );
   nParticles = swarm->cellParticleCountTbl[cellIndex];
   for( ii = 0; ii < nParticles; ii++ ) {
     StoreVisc_ParticleExt*            particleExt;
     MaterialPointsSwarm*    mSwarm;
     MaterialPoint*          materialparticle;

      /* Cache information from the current integration point. */
      integrationPoint = (IntegrationPoint*)Swarm_ParticleInCellAt(
         swarm, cellIndex, ii );
      ElementType_ShapeFunctionsGlobalDerivs(
         elementType, mesh, elementIndex, integrationPoint->xi,
         nDims, &jacDet, GNx );

      /* Add this particle's value to the area. */
      cellArea += integrationPoint->weight * jacDet;

      materialparticle =
        OneToOneMapper_GetMaterialPoint( swarm->mapper,
                                         integrationPoint, &mSwarm );
      particleExt=
        (StoreVisc_ParticleExt*)ExtensionManager_Get( mSwarm->particleExtensionMgr,
                              materialparticle,
                              self->storeVisc->particleExtHandle );

      visc=particleExt->effVisc;
      sumVisc += visc*integrationPoint->weight * jacDet;
   }

   /* Normalize the viscosity factor by dividing by cell area. */
   viscFac = cellArea / sumVisc;

   /* Adjust the calculated mass matrix by the 'special operator'. The
      nElNodes term comes from an averaging operator applied twice. */

   for( ii = 0; ii < nElNodes; ii++ )
      for( jj = 0; jj < nElNodes; jj++ )
        localElStiffMat[ii][jj] -= cellArea/(nElNodes*nElNodes);

   /* Apply the viscosity factor and negate the calculated value. */
   for( ii = 0; ii < nElNodes; ii++ ) {
      for( jj = 0; jj < nElNodes; jj++ )
         elStiffMat[ii][jj] += -localElStiffMat[ii][jj] * viscFac;
   }
}
