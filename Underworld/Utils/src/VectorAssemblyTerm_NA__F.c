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
#include "VectorAssemblyTerm_NA__F.h"

#include <assert.h>
#include <string.h>

/* Textual name of this class */
const Type VectorAssemblyTerm_NA__F_Type = "VectorAssemblyTerm_NA__F";

/* Creation implementation / Virtual constructor */
VectorAssemblyTerm_NA__F* _VectorAssemblyTerm_NA__F_New(  FORCEASSEMBLYTERM_NA__FN_DEFARGS  )
{
   VectorAssemblyTerm_NA__F* self;

   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(VectorAssemblyTerm_NA__F) );
   /* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
   /* This means that any values of these parameters that are passed into this function are not passed onto the parent function
      and so should be set to ZERO in any children of this class. */
   nameAllocationType = NON_GLOBAL;

   self = (VectorAssemblyTerm_NA__F*) _ForceTerm_New(  FORCETERM_PASSARGS  );

   /* Virtual info */

   return self;
}

void _VectorAssemblyTerm_NA__F_Init( void* forceTerm, PpcManager* ppcManager, int functionLabel ) {
   VectorAssemblyTerm_NA__F* self = (VectorAssemblyTerm_NA__F*)forceTerm;

   self->errorStream = Journal_Register( Error_Type, (Name)self->name  );
   self->ppcManager    = ppcManager;
   self->functionLabel = functionLabel;
}

void _VectorAssemblyTerm_NA__F_Delete( void* forceTerm ) {
   VectorAssemblyTerm_NA__F* self = (VectorAssemblyTerm_NA__F*)forceTerm;

   _ForceTerm_Delete( self );
}

void _VectorAssemblyTerm_NA__F_Print( void* forceTerm, Stream* stream ) {}

void* _VectorAssemblyTerm_NA__F_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof(VectorAssemblyTerm_NA__F);
   Type                                                      type = VectorAssemblyTerm_NA__F_Type;
   Stg_Class_DeleteFunction*                              _delete = _VectorAssemblyTerm_NA__F_Delete;
   Stg_Class_PrintFunction*                                _print = _VectorAssemblyTerm_NA__F_Print;
   Stg_Class_CopyFunction*                                  _copy = NULL;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _VectorAssemblyTerm_NA__F_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _VectorAssemblyTerm_NA__F_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _VectorAssemblyTerm_NA__F_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _VectorAssemblyTerm_NA__F_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _VectorAssemblyTerm_NA__F_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _VectorAssemblyTerm_NA__F_Destroy;
   ForceTerm_AssembleElementFunction*            _assembleElement = _VectorAssemblyTerm_NA__F_AssembleElement;

   /* Variables that are set to ZERO are variables that will be set eis by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*)_VectorAssemblyTerm_NA__F_New(  FORCEASSEMBLYTERM_NA__FN_PASSARGS  );
}

void _VectorAssemblyTerm_NA__F_AssignFromXML( void* forceTerm, Stg_ComponentFactory* cf, void* data ) {
   VectorAssemblyTerm_NA__F*  self = (VectorAssemblyTerm_NA__F*)forceTerm;
   PpcManager*                ppcManager  = NULL;
   int                        functionLabel;

   /* Construct Parent */
   _ForceTerm_AssignFromXML( self, cf, data );

   /* The PpcManager */
   ppcManager = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Manager", PpcManager, False, data );
   if( !ppcManager  )
      ppcManager = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_ppcManager", PpcManager, True, data  );

   functionLabel = PpcManager_GetPpcFromDict( ppcManager, cf, self->name, (Dictionary_Entry_Key)"functionLabel", "functionLabel" );

   _VectorAssemblyTerm_NA__F_Init( self, ppcManager, functionLabel );
}

void _VectorAssemblyTerm_NA__F_Build( void* forceTerm, void* data ) {
   VectorAssemblyTerm_NA__F* self = (VectorAssemblyTerm_NA__F*)forceTerm;

   Stg_Component_Build( self->ppcManager, data, False );
   _ForceTerm_Build( self, data );
}

void _VectorAssemblyTerm_NA__F_Initialise( void* forceTerm, void* data ) {
   VectorAssemblyTerm_NA__F* self = (VectorAssemblyTerm_NA__F*)forceTerm;

   Stg_Component_Initialise( self->ppcManager, data, False );
   _ForceTerm_Initialise( self, data );
}

void _VectorAssemblyTerm_NA__F_Execute( void* forceTerm, void* data ) {
   VectorAssemblyTerm_NA__F* self = (VectorAssemblyTerm_NA__F*)forceTerm;

   _ForceTerm_Execute( self, data );
}

void _VectorAssemblyTerm_NA__F_Destroy( void* forceTerm, void* data ) {
   VectorAssemblyTerm_NA__F* self = (VectorAssemblyTerm_NA__F*)forceTerm;

   _ForceTerm_Destroy( self, data );
}


void _VectorAssemblyTerm_NA__F_AssembleElement( void* forceTerm, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elForceVec ) {
   VectorAssemblyTerm_NA__F*  self = Stg_CheckType( forceTerm, VectorAssemblyTerm_NA__F );
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
   double                     detJac;
   double                     factor;
   double                     N[27];
   double                     F[3];
   int                        err;

   /* Since we are integrating over the velocity mesh - we want the velocity mesh here and not the temperature mesh */
   mesh             = forceVector->feVariable->feMesh;

   /* Set the element type */
   elementType      = FeMesh_GetElementType( mesh, lElement_I );
   nodesPerEl = elementType->nodeCount;

   /* assumes constant number of dofs per element */
   dofsPerNode = forceVector->feVariable->fieldComponentCount;

   cell_I = CellLayout_MapElementIdToCellId( swarm->cellLayout, lElement_I );
   cellParticleCount = swarm->cellParticleCountTbl[ cell_I ];

   for ( cParticle_I = 0 ; cParticle_I < cellParticleCount ; cParticle_I++ ) {
      particle = (IntegrationPoint*) Swarm_ParticleInCellAt( swarm, cell_I, cParticle_I );
      xi       = particle->xi;

      /* Calculate Determinant of Jacobian and Shape Functions */
      detJac = ElementType_JacobianDeterminant( elementType, mesh, lElement_I, xi, dim );
      ElementType_EvaluateShapeFunctionsAt( elementType, xi, N );

      /* evaluate ppc function */
      F[0]=F[1]=F[2]=0; // MUST INITIALISE
      err = PpcManager_Get( self->ppcManager, lElement_I, particle, self->functionLabel, &F[0] );

      Journal_Firewall( !err,
         self->errorStream,
         "%s not found at the PpcManager\n", self->functionLabel );

      factor = detJac * particle->weight;
      for( A = 0 ; A < nodesPerEl ; A++ )
         for( i = 0 ; i < dofsPerNode ; i++ )
            elForceVec[A * dofsPerNode + i ] += factor * F[i] * N[A] ;

   }
}



