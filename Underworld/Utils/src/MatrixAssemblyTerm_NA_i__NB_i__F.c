/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**	Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**	Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**	AuScope - http://www.auscope.org
**	Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**	Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**	Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**	Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**	Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
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
#include "MatrixAssemblyTerm_NA_i__NB_i__F.h"

#include <assert.h>
#include <string.h>

/* Textual name of this class */
const Type MatrixAssemblyTerm_NA_i__NB_i__F_Type = "MatrixAssemblyTerm_NA_i__NB_i__F";

/* Creation implementation / Virtual constructor */
MatrixAssemblyTerm_NA_i__NB_i__F* _MatrixAssemblyTerm_NA_i__NB_i__F_New(  MATRIXASSEMBLYTERM_NA_I__NB_I__F_DEFARGS  )
{
   MatrixAssemblyTerm_NA_i__NB_i__F* self;

   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(MatrixAssemblyTerm_NA_i__NB_i__F) );
   self = (MatrixAssemblyTerm_NA_i__NB_i__F*) _StiffnessMatrixTerm_New(  STIFFNESSMATRIXTERM_PASSARGS  );

   /* Virtual info */

   return self;
}

void _MatrixAssemblyTerm_NA_i__NB_i__F_Init( void* matrixTerm, PpcManager* ppcManager, int functionLabel ) {
   MatrixAssemblyTerm_NA_i__NB_i__F* self = (MatrixAssemblyTerm_NA_i__NB_i__F*)matrixTerm;

   self->errorStream = Journal_Register( Error_Type, (Name)self->name  );
   self->ppcManager    = ppcManager;
   self->functionLabel = functionLabel;
}

void _MatrixAssemblyTerm_NA_i__NB_i__F_Delete( void* matrixTerm ) {
   MatrixAssemblyTerm_NA_i__NB_i__F* self = (MatrixAssemblyTerm_NA_i__NB_i__F*)matrixTerm;

   _StiffnessMatrixTerm_Delete( self );
}

void _MatrixAssemblyTerm_NA_i__NB_i__F_Print( void* matrixTerm, Stream* stream ) {
   MatrixAssemblyTerm_NA_i__NB_i__F* self = (MatrixAssemblyTerm_NA_i__NB_i__F*)matrixTerm;

   _StiffnessMatrixTerm_Print( self, stream );

   /* General info */
}

void* _MatrixAssemblyTerm_NA_i__NB_i__F_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                                 _sizeOfSelf = sizeof(MatrixAssemblyTerm_NA_i__NB_i__F);
   Type                                                         type = MatrixAssemblyTerm_NA_i__NB_i__F_Type;
   Stg_Class_DeleteFunction*                                 _delete = _MatrixAssemblyTerm_NA_i__NB_i__F_Delete;
   Stg_Class_PrintFunction*                                   _print = _MatrixAssemblyTerm_NA_i__NB_i__F_Print;
   Stg_Class_CopyFunction*                                     _copy = NULL;
   Stg_Component_DefaultConstructorFunction*     _defaultConstructor = _MatrixAssemblyTerm_NA_i__NB_i__F_DefaultNew;
   Stg_Component_ConstructFunction*                       _construct = _MatrixAssemblyTerm_NA_i__NB_i__F_AssignFromXML;
   Stg_Component_BuildFunction*                               _build = _MatrixAssemblyTerm_NA_i__NB_i__F_Build;
   Stg_Component_InitialiseFunction*                     _initialise = _MatrixAssemblyTerm_NA_i__NB_i__F_Initialise;
   Stg_Component_ExecuteFunction*                           _execute = _MatrixAssemblyTerm_NA_i__NB_i__F_Execute;
   Stg_Component_DestroyFunction*                           _destroy = _MatrixAssemblyTerm_NA_i__NB_i__F_Destroy;
   StiffnessMatrixTerm_AssembleElementFunction*     _assembleElement = _MatrixAssemblyTerm_NA_i__NB_i__F_AssembleElement;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*)_MatrixAssemblyTerm_NA_i__NB_i__F_New(  MATRIXASSEMBLYTERM_NA_I__NB_I__F_PASSARGS  );
}

void _MatrixAssemblyTerm_NA_i__NB_i__F_AssignFromXML( void* matrixTerm, Stg_ComponentFactory* cf, void* data ) {
   MatrixAssemblyTerm_NA_i__NB_i__F*            self             = (MatrixAssemblyTerm_NA_i__NB_i__F*)matrixTerm;
   PpcManager*                ppcManager  = NULL;
   int                        functionLabel;

   /* Construct Parent */
   _StiffnessMatrixTerm_AssignFromXML( self, cf, data );

   /* The PpcManager */
   ppcManager = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Manager", PpcManager, False, data );
   if( !ppcManager  )
      ppcManager = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_ppcManager", PpcManager, True, data  );

   functionLabel = PpcManager_GetPpcFromDict( ppcManager, cf, self->name, (Dictionary_Entry_Key)"functionLabel", "functionLabel" );

   _MatrixAssemblyTerm_NA_i__NB_i__F_Init( self, ppcManager, functionLabel );
}

void _MatrixAssemblyTerm_NA_i__NB_i__F_Build( void* matrixTerm, void* data ) {
   MatrixAssemblyTerm_NA_i__NB_i__F*             self             = (MatrixAssemblyTerm_NA_i__NB_i__F*)matrixTerm;

    Stg_Component_Build( self->ppcManager, data, False );
   _StiffnessMatrixTerm_Build( self, data );
}

void _MatrixAssemblyTerm_NA_i__NB_i__F_Initialise( void* matrixTerm, void* data ) {
   MatrixAssemblyTerm_NA_i__NB_i__F* self = (MatrixAssemblyTerm_NA_i__NB_i__F*)matrixTerm;

   Stg_Component_Initialise( self->ppcManager, data, False );
   _StiffnessMatrixTerm_Initialise( self, data );
}

void _MatrixAssemblyTerm_NA_i__NB_i__F_Execute( void* matrixTerm, void* data ) {
   _StiffnessMatrixTerm_Execute( matrixTerm, data );
}

void _MatrixAssemblyTerm_NA_i__NB_i__F_Destroy( void* matrixTerm, void* data ) {
   _StiffnessMatrixTerm_Destroy( matrixTerm, data );
}

void _MatrixAssemblyTerm_NA_i__NB_i__F_AssembleElement(
      void*                                              matrixTerm,
      StiffnessMatrix*                                   stiffnessMatrix,
      Element_LocalIndex                                 lElement_I,
      SystemLinearEquations*                             sle,
      FiniteElementContext*                              context,
      double**                                           elStiffMat )
{
   MatrixAssemblyTerm_NA_i__NB_i__F*   self = (MatrixAssemblyTerm_NA_i__NB_i__F*)matrixTerm;
   Swarm*                              swarm        = self->integrationSwarm;
   FeVariable*                         variable1    = stiffnessMatrix->rowVariable;
   Dimension_Index                     dim          = stiffnessMatrix->dim;
   IntegrationPoint*                   currIntegrationPoint;
   double*                             xi;
   double                              weight;
   Particle_InCellIndex                cParticle_I, cellParticleCount;
   Index                               nodesPerEl;
   Index                               A,B;
   Index                               i;
   double**                            GNx;
   double                              detJac;
   double                              F;
   Cell_Index                          cell_I;
   ElementType*                        elementType;

   /* Set the element type */
   elementType = FeMesh_GetElementType( variable1->feMesh, lElement_I );
   nodesPerEl = elementType->nodeCount;

   if( nodesPerEl > self->max_nElNodes ) {
      /* reallocate */
      self->GNx = (double*)ReallocArray2D( self->GNx, double, dim, nodesPerEl );
      self->max_nElNodes = nodesPerEl;
   }
   GNx = self->GNx;

   cell_I = CellLayout_MapElementIdToCellId( swarm->cellLayout, lElement_I );
   cellParticleCount = swarm->cellParticleCountTbl[ cell_I ];

   for( cParticle_I = 0 ; cParticle_I < cellParticleCount ; cParticle_I++ ) {

      currIntegrationPoint = (IntegrationPoint*)Swarm_ParticleInCellAt( swarm, cell_I, cParticle_I );

      xi = currIntegrationPoint->xi;
      weight = currIntegrationPoint->weight;

      ElementType_ShapeFunctionsGlobalDerivs(
         elementType,
         variable1->feMesh, lElement_I,
         xi, dim, &detJac, GNx );

      /* evaluate ppc function */
      PpcManager_Get( self->ppcManager, lElement_I, currIntegrationPoint, self->functionLabel, &F );

      for( A=0; A<nodesPerEl; A++ ) {
         for( B=0; B<nodesPerEl; B++ ) {
            for ( i = 0; i < dim ; i++ ) {
               elStiffMat[A][B] += detJac * weight * GNx[i][A] * GNx[i][B] * F;
            }
         }
      }
   }
}


