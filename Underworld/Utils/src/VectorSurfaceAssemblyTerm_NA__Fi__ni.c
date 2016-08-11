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
#include "VectorSurfaceAssemblyTerm_NA__Fi__ni.h"

#include <assert.h>
#include <string.h>

/* Textual name of this class */
const Type VectorSurfaceAssemblyTerm_NA__Fi__ni_Type = "VectorSurfaceAssemblyTerm_NA__Fi__ni";

void* _VectorSurfaceAssemblyTerm_NA__Fi__ni_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof(VectorSurfaceAssemblyTerm_NA__Fi__ni);
   Type                                                      type = VectorSurfaceAssemblyTerm_NA__Fi__ni_Type;
   Stg_Class_DeleteFunction*                              _delete = _VectorSurfaceAssemblyTerm_NA__Fi__ni_Delete;
   Stg_Class_PrintFunction*                                _print = _VectorSurfaceAssemblyTerm_NA__Fi__ni_Print;
   Stg_Class_CopyFunction*                                  _copy = NULL;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _VectorSurfaceAssemblyTerm_NA__Fi__ni_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _VectorSurfaceAssemblyTerm_NA__Fi__ni_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _VectorSurfaceAssemblyTerm_NA__Fi__ni_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _VectorSurfaceAssemblyTerm_NA__Fi__ni_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _VectorSurfaceAssemblyTerm_NA__Fi__ni_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _VectorSurfaceAssemblyTerm_NA__Fi__ni_Destroy;
   ForceTerm_AssembleElementFunction*            _assembleElement = _VectorSurfaceAssemblyTerm_NA__Fi__ni_AssembleElement;

   /* Variables that are set to ZERO are variables that will be set eis by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*)_VectorSurfaceAssemblyTerm_NA__Fi__ni_New(  VECTORSURFACEASSEMBLYTERM_NA__Fi__ni_PASSARGS  );
}

/* Creation implementation / Virtual constructor */
void* _VectorSurfaceAssemblyTerm_NA__Fi__ni_New(  VECTORSURFACEASSEMBLYTERM_NA__Fi__ni_DEFARGS  )
{
   VectorSurfaceAssemblyTerm_NA__Fi__ni* self;

   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(VectorSurfaceAssemblyTerm_NA__Fi__ni) );
   /* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
   /* This means that any values of these parameters that are passed into this function are not passed onto the parent function
      and so should be set to ZERO in any children of this class. */
   nameAllocationType = NON_GLOBAL;

   self = (VectorSurfaceAssemblyTerm_NA__Fi__ni*) _ForceTerm_New(  FORCETERM_PASSARGS  );

   /* Virtual info */

   return self;
}

void _VectorSurfaceAssemblyTerm_NA__Fi__ni_Delete( void* forceTerm ) {
   VectorSurfaceAssemblyTerm_NA__Fi__ni* self = (VectorSurfaceAssemblyTerm_NA__Fi__ni*)forceTerm;

   Memory_Free(self->surface);

   _ForceTerm_Delete( self );
    
}

void _VectorSurfaceAssemblyTerm_NA__Fi__ni_Print( void* forceTerm, Stream* stream ) {}

void _VectorSurfaceAssemblyTerm_NA__Fi__ni_AssignFromXML( void* forceTerm, Stg_ComponentFactory* cf, void* data ) {
   VectorSurfaceAssemblyTerm_NA__Fi__ni*  self = (VectorSurfaceAssemblyTerm_NA__Fi__ni*)forceTerm;

   /* Construct Parent */
   _ForceTerm_AssignFromXML( self, cf, data );

   /* The PpcManager */
   self->ppcManager = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Manager", PpcManager, False, data );
   if( !self->ppcManager  )
      self->ppcManager = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_ppcManager", PpcManager, True, data  );

   self->functionLabel = PpcManager_GetPpcFromDict( self->ppcManager, cf, self->name, (Dictionary_Entry_Key)"functionLabel", "functionLabel" );

   self->surface = StG_Strdup( Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"Surface", "MinJ" ));

   self->errorStream = Journal_Register( Error_Type, (Name)self->name  );

   /* ensure that the correct particle layout is being provided */
   Journal_Firewall( !strcasecmp( self->integrationSwarm->particleLayout->type, GaussBorderParticleLayout_Type ),
         self->errorStream,
         "Integration swarm particle layout is of type %s which is probably incorrect for this component.\n\  
          Integration swarm particle layout should be of type \'GaussBorderParticleLayout\'.", self->integrationSwarm->particleLayout->type );

   /* lets also determine which face we need to integrate over */
   if (     !strcasecmp(self->surface, "back")   || !strcasecmp(self->surface, "MinK"))
      self->requiredFace = 4;
   else if (!strcasecmp(self->surface, "left")   || !strcasecmp(self->surface, "MinI"))
      self->requiredFace = 2;
   else if (!strcasecmp(self->surface, "bottom") || !strcasecmp(self->surface, "MinJ"))
      self->requiredFace = 0;
   else if (!strcasecmp(self->surface, "right")  || !strcasecmp(self->surface, "MaxI"))
      self->requiredFace = 3;
   else if (!strcasecmp(self->surface, "top")    || !strcasecmp(self->surface, "MaxJ"))
      self->requiredFace = 1;
   else if (!strcasecmp(self->surface, "front")  || !strcasecmp(self->surface, "MaxK"))
      self->requiredFace = 5;
}

void _VectorSurfaceAssemblyTerm_NA__Fi__ni_Build( void* forceTerm, void* data ) {
   VectorSurfaceAssemblyTerm_NA__Fi__ni* self = (VectorSurfaceAssemblyTerm_NA__Fi__ni*)forceTerm;

   _ForceTerm_Build( self, data );

   Stg_Component_Build( self->ppcManager, data, False );
}

void _VectorSurfaceAssemblyTerm_NA__Fi__ni_Initialise( void* forceTerm, void* data ) {
   VectorSurfaceAssemblyTerm_NA__Fi__ni* self = (VectorSurfaceAssemblyTerm_NA__Fi__ni*)forceTerm;
   MeshTopology_Dim topo;

   _ForceTerm_Initialise( self, data );

   Stg_Component_Initialise( self->ppcManager, data, False );

   /* now, lets get the required set */
   topo = (self->forceVector->dim == 2) ? MT_FACE : MT_VOLUME;
   self->surfaceElementSet =  RegularMeshUtils_CreateBoundaryFeatureSet( self->forceVector->feVariable->feMesh, topo, self->surface );
}

void _VectorSurfaceAssemblyTerm_NA__Fi__ni_Execute( void* forceTerm, void* data ) {
   VectorSurfaceAssemblyTerm_NA__Fi__ni* self = (VectorSurfaceAssemblyTerm_NA__Fi__ni*)forceTerm;

   _ForceTerm_Execute( self, data );
}

void _VectorSurfaceAssemblyTerm_NA__Fi__ni_Destroy( void* forceTerm, void* data ) {
   VectorSurfaceAssemblyTerm_NA__Fi__ni* self = (VectorSurfaceAssemblyTerm_NA__Fi__ni*)forceTerm;

   Stg_Class_Delete( self->surfaceElementSet );

   _ForceTerm_Destroy( self, data );
}


void _VectorSurfaceAssemblyTerm_NA__Fi__ni_AssembleElement( void* forceTerm, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elForceVec ) {
   VectorSurfaceAssemblyTerm_NA__Fi__ni*  self = Stg_CheckType( forceTerm, VectorSurfaceAssemblyTerm_NA__Fi__ni );
   Swarm*                     swarm = self->integrationSwarm;
   Dimension_Index            dim = forceVector->dim;
   IntegrationPoint*          particle;
   FeMesh*                    mesh;
   double*                    xi;
   Particle_InCellIndex       cParticle_I;
   Particle_InCellIndex       cellParticleCount;
   Element_NodeIndex          nodesPerEl;
   Node_ElementLocalIndex     A;
   ElementType*               elementType;
   Dof_Index                  dofsPerNode, i;
   Cell_Index                 cell_I;
   double                     factor;
   double                     N[27];
   double                     F[3];
   int                        err;
   double                     normal[3];
   Dimension_Index            faceIndex;
   double                     surfaceJacobian;
   double                     dotprod;
   int                        ii;

   /* if this element isn't along the boundary, nothing to do so return */
   if( !IndexSet_IsMember( self->surfaceElementSet, lElement_I ) )
      return;

   /* Since we are integrating over the velocity mesh - we want the velocity mesh here and not the temperature mesh */
   mesh = forceVector->feVariable->feMesh;

   /* Set the element type */
   elementType = FeMesh_GetElementType( mesh, lElement_I );
   nodesPerEl = elementType->nodeCount;

   /* assumes constant number of dofs per element */
   dofsPerNode = forceVector->feVariable->fieldComponentCount;

   cell_I = CellLayout_MapElementIdToCellId( swarm->cellLayout, lElement_I );
   cellParticleCount = swarm->cellParticleCountTbl[ cell_I ];

   for ( cParticle_I = 0 ; cParticle_I < cellParticleCount ; cParticle_I++ ) {
      particle = (IntegrationPoint*) Swarm_ParticleInCellAt( swarm, cell_I, cParticle_I );
      xi       = particle->xi;

      /* just use the row element type to get the surface normal as default for now */
      faceIndex = ElementType_SurfaceNormal( elementType, lElement_I, dim, xi, normal );

      /* if not required face, continue */
      if( faceIndex != self->requiredFace)
         continue;

      /* Calculate Shape Functions */
      ElementType_EvaluateShapeFunctionsAt( elementType, xi, N );

      surfaceJacobian = ElementType_JacobianDeterminantSurface( elementType, mesh, lElement_I, xi, faceIndex,
                                                                GaussBorderParticleLayout_GetFaceAxis( NULL, faceIndex, 2 ) );

      /* evaluate ppc function */
      err = PpcManager_Get( self->ppcManager, lElement_I, particle, self->functionLabel, &F[0] );

      Journal_Firewall( !err,
         self->errorStream,
         "%s not found at the PpcManager\n", self->functionLabel );

      dotprod = 0.;
      for(ii=0; ii<dim; ii++)
         dotprod += normal[ii]*F[ii];

      factor = - surfaceJacobian * particle->weight;
      for( A = 0 ; A < nodesPerEl ; A++ )
         for( i = 0 ; i < dofsPerNode ; i++ ) 
            elForceVec[A * dofsPerNode + i ] += factor * dotprod * N[A] ;
   }
}



