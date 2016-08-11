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
#include "PpcIntegral.h"

#include <assert.h>
#include <string.h>

/* Textual name of this class */
const Type PpcIntegral_Type = "PpcIntegral";

/* Creation implementation / Virtual constructor */
PpcIntegral* _PpcIntegral_New(  PPCINTEGRAL_DEFARGS  )
{
   PpcIntegral* self;

   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(PpcIntegral) );
   /* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
   /* This means that any values of these parameters that are passed into this function are not passed onto the parent function
      and so should be set to ZERO in any children of this class. */
   nameAllocationType = NON_GLOBAL;

   self = (PpcIntegral*)_Stg_Component_New(  STG_COMPONENT_PASSARGS  );

   /* Virtual info */

   self->ppcManager = NULL;

   return self;
}

void* _PpcIntegral_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof(PpcIntegral);
   Type                                                      type = PpcIntegral_Type;
   Stg_Class_DeleteFunction*                              _delete = _Stg_Component_Delete;
   Stg_Class_PrintFunction*                                _print = _Stg_Component_Print;
   Stg_Class_CopyFunction*                                  _copy = NULL;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _PpcIntegral_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _PpcIntegral_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _PpcIntegral_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _PpcIntegral_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _PpcIntegral_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _PpcIntegral_Destroy;

   /* Variables that are set to ZERO are variables that will be set eis by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*)_PpcIntegral_New(  PPCINTEGRAL_PASSARGS  );
}

void _PpcIntegral_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
   PpcIntegral*  self = (PpcIntegral*) _self;

   self->errorStream = Journal_Register( Error_Type, (Name)self->name  );

   self->context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", PICelleratorContext, False, data );
   if( !self->context  )
      self->context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", PICelleratorContext, True, data  );

   self->ppcManager = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Manager", PpcManager, False, data );
   if( !self->ppcManager )
      self->ppcManager = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_ppcManager", PpcManager, True, data  );

   Journal_Firewall( self->ppcManager, self->errorStream, "Error in PpcIntegral '%s' - No Ppc Manager found in dictionary", self->name );

   self->functionLabel = PpcManager_GetPpcFromDict( self->ppcManager, cf, self->name, (Dictionary_Entry_Key)"functionLabel", "functionLabel" );

}

void _PpcIntegral_Build( void* _self, void* data ) {
   PpcIntegral* self = (PpcIntegral*)_self;

   Stg_Component_Build( self->ppcManager, data, False );
}

void _PpcIntegral_Initialise( void* _self, void* data ) {
   PpcIntegral* self = (PpcIntegral*)_self;

   Stg_Component_Initialise( self->ppcManager, data, False );
}

void _PpcIntegral_Destroy( void* _self, void* data ) {
}

void _PpcIntegral_Execute( void* _self, void* data ) {
}

double PpcIntegral_Integrate( void* ppcIntegral ) {
   PpcIntegral*  self = Stg_CheckType( ppcIntegral, PpcIntegral );
   Swarm*                     swarm = self->ppcManager->integrationSwarm;
   Dimension_Index            dim = self->context->dim;
   IntegrationPoint*          particle;
   FeMesh*                    mesh;
   double*                    xi;
   Particle_InCellIndex       cParticle_I;
   Particle_InCellIndex       cellParticleCount;
   ElementType*               elementType;
   Dof_Index                  fieldDofs, i;
   Cell_Index                 cell_I;
   double                     detJac;
   double                     factor;
   double                     N[27];
   double                     F[3];
   int                        err;
   int                        nElements;
   int                        lElement_I;
   double                     lIntegral=0;

   /* get the mesh from the PpcManager */
   mesh             = self->ppcManager->mesh;

   /* hardcode this guy for now */
   fieldDofs = 1;

   nElements = FeMesh_GetElementLocalSize( mesh );
   // printf("n_elements = %d \n", n_elements );

   for( lElement_I=0; lElement_I<nElements; lElement_I++ ) {

      cell_I = CellLayout_MapElementIdToCellId( swarm->cellLayout, lElement_I );
      cellParticleCount = swarm->cellParticleCountTbl[ cell_I ];
      elementType = FeMesh_GetElementType( mesh, lElement_I );

      for ( cParticle_I = 0 ; cParticle_I < cellParticleCount ; cParticle_I++ ) {
         particle = (IntegrationPoint*) Swarm_ParticleInCellAt( swarm, cell_I, cParticle_I );
         xi       = particle->xi;

         /* Calculate Determinant of Jacobian and Shape Functions */
         detJac = ElementType_JacobianDeterminant( elementType, mesh, lElement_I, xi, dim );
         ElementType_EvaluateShapeFunctionsAt( elementType, xi, N );

         /* evaluate ppc function */
         err = PpcManager_Get( self->ppcManager, lElement_I, particle, self->functionLabel, &F[0] );

         Journal_Firewall( !err,
            self->errorStream,
            "%s not found at the PpcManager\n", self->functionLabel );

         factor = detJac * particle->weight;
         for( i = 0 ; i < fieldDofs ; i++ )
               lIntegral += factor * F[i] ;

      }
   }

   (void)MPI_Allreduce ( &lIntegral, &self->gIntegral, 1, MPI_DOUBLE, MPI_SUM, self->context->communicator );
   
   return self->gIntegral;
}

