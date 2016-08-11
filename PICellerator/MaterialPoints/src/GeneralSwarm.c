/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**	Melbourne, 3053, Australia.
** Copyright (c) 2005-2010, Monash University, Building 28, Monash University Clayton Campus,
**	Victoria, 3800, Australia
**
** Primary Contributing Organisations:
**	Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**	AuScope - http://www.auscope.org
**	Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**
** Contributors:
**	Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**	Patrick D. Sunter, Software Engineer, VPAC. (patrick@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**	Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
**	Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**	Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**	David Stegman, Postdoctoral Fellow, Monash University. (david.stegman@sci.monash.edu.au)
**	Wendy Sharples, PhD Student, Monash University (wendy.sharples@sci.monash.edu.au)
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

#include <PICellerator/PopulationControl/PopulationControl.h>
#include <PICellerator/Weights/Weights.h>

#include "MaterialPoints.h"
#include <assert.h>
#include <string.h>
#include <math.h>


/* Textual name of this class */
const Type GeneralSwarm_Type = "GeneralSwarm";

GeneralSwarm* _GeneralSwarm_New(  GENERALSWARM_DEFARGS  )
{
   GeneralSwarm* self;

   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(GeneralSwarm) );
   /* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
   /* This means that any values of these parameters that are passed into this function are not passed onto the parent function
      and so should be set to ZERO in any children of this class. */
   ics = NULL;

   self = (GeneralSwarm*)_Swarm_New(  SWARM_PASSARGS  );

   return self;
}


void _GeneralSwarm_Init(
   void*                                 swarm,
   EscapedRoutine*                       escapedRoutine )
{
   GeneralSwarm*    self = (GeneralSwarm*)swarm;
   GlobalParticle          globalParticle;

   self->swarmAdvector      = NULL;		/* If we're using a SwarmAdvector, it will 'attach' itself later on. */
   self->escapedRoutine     = escapedRoutine;

   self->particleCoordVariable = Swarm_NewVectorVariable( self, (Name)"Position", GetOffsetOfMember( globalParticle, coord ),
                                 Variable_DataType_Double,
                                 self->dim,
                                 "PositionX",
                                 "PositionY",
                                 "PositionZ" );
   LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*)self->particleCoordVariable->variable );
   LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*)self->particleCoordVariable );

}

/*------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _GeneralSwarm_Delete( void* swarm )
{
   GeneralSwarm* self = (GeneralSwarm*)swarm;

   _Swarm_Delete( self );
}

void* _GeneralSwarm_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                 _sizeOfSelf = sizeof(GeneralSwarm);
   Type                                                         type = GeneralSwarm_Type;
   Stg_Class_DeleteFunction*                                 _delete = _GeneralSwarm_Delete;
   Stg_Class_PrintFunction*                                   _print = NULL;
   Stg_Class_CopyFunction*                                     _copy = NULL;
   Stg_Component_DefaultConstructorFunction*     _defaultConstructor = _GeneralSwarm_DefaultNew;
   Stg_Component_ConstructFunction*                       _construct = _GeneralSwarm_AssignFromXML;
   Stg_Component_BuildFunction*                               _build = _GeneralSwarm_Build;
   Stg_Component_InitialiseFunction*                     _initialise = _GeneralSwarm_Initialise;
   Stg_Component_ExecuteFunction*                           _execute = _GeneralSwarm_Execute;
   Stg_Component_DestroyFunction*                           _destroy = _GeneralSwarm_Destroy;
   SizeT                                                particleSize = sizeof(GlobalParticle);

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;
   void*                          ics = ZERO;

   return _GeneralSwarm_New(  GENERALSWARM_PASSARGS  );
}


void _GeneralSwarm_AssignFromXML( void* swarm, Stg_ComponentFactory* cf, void* data )
{
   GeneralSwarm*	        self          = (GeneralSwarm*) swarm;
   EscapedRoutine*                 escapedRoutine;
   PICelleratorContext*		context;

   _Swarm_AssignFromXML( self, cf, data );

   escapedRoutine   = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"EscapedRoutine", EscapedRoutine, False, data  );

   context = (PICelleratorContext* )self->context;
   assert( Stg_CheckType( context, PICelleratorContext ) );

   _GeneralSwarm_Init(
      self,
      escapedRoutine );

}

void _GeneralSwarm_Build( void* swarm, void* data )
{
   GeneralSwarm*	self = (GeneralSwarm*) swarm;
   int			commHandler_I;
   Bool                    movementCommHandlerFound = False;
   Stream*                 errorStream = Journal_Register( Error_Type, (Name)self->type  );
   int var_I;

   _Swarm_Build( self, data );

   if( self->escapedRoutine != NULL) Stg_Component_Build( self->escapedRoutine, data , False );

   /* Since this swarm is being set up to advect a PICellerator material, it should make sure
    * at least one ParticleMovementHandler-type ParticleCommHandler has been added to the base
    * Swarm. */
   for( commHandler_I=0; commHandler_I < self->commHandlerList->count; commHandler_I++ )
   {
      ParticleCommHandler *pComm = NULL;

      pComm = (ParticleCommHandler*)(Stg_ObjectList_At( self->commHandlerList, commHandler_I ));
      if( pComm->type == ParticleMovementHandler_Type )
      {
         movementCommHandlerFound = True;
         break;
      }
   }

   Journal_Firewall( (Stg_ObjectList_Count(self->commHandlerList) >= 1) && (movementCommHandlerFound == True),
                     errorStream, "Error: for GeneralSwarm Swarms, at least one ParticleMovementHandler"
                     " commHandler must be registered. Please rectify this in your XML / code.\n" );

   for( var_I = 0 ; var_I < self->nSwarmVars ; var_I++ )
   {
      Stg_Component_Build( self->swarmVars[var_I], data , False );
   }
}
void _GeneralSwarm_Initialise( void* swarm, void* data )
{
   GeneralSwarm*	self 	= (GeneralSwarm*) swarm;
   AbstractContext* 	context = (AbstractContext*)self->context;
   Index            	var_I	= 0;

   _Swarm_Initialise( self, data );

   if( self->escapedRoutine != NULL) Stg_Component_Initialise( self->escapedRoutine, data , False );

   for( var_I = 0 ; var_I < self->nSwarmVars ; var_I++ )
   {
      Stg_Component_Initialise( self->swarmVars[var_I], data , False );
   }


   /** if loading from checkpoint, particle materials etc have already been loaded in Swarm_Build() - */
   /** possibly need to check for empty cells (and populate) if performing a interpolation restart */
   if ( True == context->loadFromCheckPoint )
   {
      if ( (True == self->isSwarmTypeToCheckPointAndReload) && (True == context->interpolateRestart) )
      {
         Particle_InCellIndex cParticle_I         = 0;
         Particle_InCellIndex particle_I          = 0;
         GlobalParticle*      particle            = NULL;
         double               minDistance         = HUGE_VAL;
         double               distanceToParticle;
         Dimension_Index      dim = self->dim;
         Cell_DomainIndex     dCell_I;
         Cell_LocalIndex      lCell_I;
         Cell_DomainIndex     belongsToCell_I = 0;
         GlobalParticle*       matNewParticle;
         GlobalParticle*       matParticleToSplit;
         Particle_Index       matNewParticle_IndexOnCPU;
         Coord                xi;
         Coord                coord;
         unsigned             nEmptyCells = 0;
         Index*				   cellID = NULL;
         Index*				   particleCPUID = NULL;
         unsigned             count;
         unsigned             ii;

         /** first determine how many local cells are empty */
         for( lCell_I = 0 ; lCell_I < self->cellLocalCount ; lCell_I++ )
            if (self->cellParticleCountTbl[lCell_I] == 0) nEmptyCells++;

         /** create arrays which will be later used to populate cells */
         cellID        = Memory_Alloc_Array( Index, nEmptyCells, "Cell ID for cell to be populated" );
         particleCPUID = Memory_Alloc_Array( Index, nEmptyCells, "particle ID for particle to populate cell" );

         count = 0;
         for( lCell_I = 0 ; lCell_I < self->cellLocalCount ; lCell_I++ )
         {
            minDistance = HUGE_VAL;
            if (self->cellParticleCountTbl[lCell_I] == 0)
            {
               /** select the centre of the current cell */
               xi[0] = 0;
               xi[1] = 0;
               xi[2] = 0;

               Journal_Firewall(
                                Stg_Class_IsInstance( self->cellLayout, ElementCellLayout_Type ),
                                NULL,
                                "Error In func %s: When performing interpolation restart, cellLayout must be of type ElementCellLayout.",
                                __func__ );

               /** get global coord */
               FeMesh_CoordLocalToGlobal( ((ElementCellLayout*)self->cellLayout)->mesh, lCell_I, xi, coord );

               for( dCell_I = 0 ; dCell_I < self->cellDomainCount ; dCell_I++ )
               {
                  /** Loop over particles find closest to cell centre */
                  for( cParticle_I = 0 ; cParticle_I < self->cellParticleCountTbl[dCell_I] ; cParticle_I++ )
                  {
                     particle = (GlobalParticle*)Swarm_ParticleInCellAt( self, dCell_I, cParticle_I );

                     /** Calculate distance to particle */
                     distanceToParticle =
                        (particle->coord[ I_AXIS ] - coord[ I_AXIS ]) *
                        (particle->coord[ I_AXIS ] - coord[ I_AXIS ]) +
                        (particle->coord[ J_AXIS ] - coord[ J_AXIS ]) *
                        (particle->coord[ J_AXIS ] - coord[ J_AXIS ]) ;

                     if (dim == 3)
                     {
                        distanceToParticle +=
                           (particle->coord[ K_AXIS ] - coord[ K_AXIS ]) *
                           (particle->coord[ K_AXIS ] - coord[ K_AXIS ]) ;
                     }
                     /** Don't do square root here because it is unnessesary: i.e. a < b <=> sqrt(a) < sqrt(b) */
                     /** Check if this is the closest particle */
                     if (minDistance > distanceToParticle)
                     {
                        particle_I       = cParticle_I;
                        minDistance      = distanceToParticle;
                        belongsToCell_I  = dCell_I;
                     }
                  }
               }

               /** create new particle which will be placed at centre of empty cell */
               matNewParticle      = (GlobalParticle*) Swarm_CreateNewParticle( self, &matNewParticle_IndexOnCPU );
               /** grab closest particle, which we will copy */
               matParticleToSplit  = (GlobalParticle*) Swarm_ParticleInCellAt( self, belongsToCell_I, particle_I );
               /** copy - even though self->particleExtensionMgr->finalSize maybe biger than sizeof(GlobalParticle)
                   the addressing of the copy is the important part
                */
               memcpy( matNewParticle, matParticleToSplit, self->particleExtensionMgr->finalSize );
               /** set the owningCell to the cellDomainCount so that its owningCell will be reinitialised (not sure if necessary) */
               matNewParticle->owningCell = self->cellDomainCount;
               /** Copy new global position (cell centre) to coord on new mat particle */
               memcpy( matNewParticle->coord, coord, sizeof(Coord) );

               /** we now store the required information to populate empty cells */
               /** note that cells are not populated at this point, as this may interfere
                   with the 0th order interpolation we are performing */
               cellID[count]        = lCell_I;
               particleCPUID[count] = matNewParticle_IndexOnCPU;
               count++;

            }
         }
         /** populate empty cells */
         for(ii = 0 ; ii < count ; ii++)
            Swarm_AddParticleToCell( self, cellID[ii], particleCPUID[ii] );
         Memory_Free( cellID );
         Memory_Free( particleCPUID );
      }
      /* TODO: print info / debug message */
   }

}
void _GeneralSwarm_Execute( void* swarm, void* data )
{
   GeneralSwarm*	self = (GeneralSwarm*)swarm;

   _Swarm_Execute( self, data );
}
void _GeneralSwarm_Destroy( void* swarm, void* data )
{
   GeneralSwarm*	self = (GeneralSwarm*)swarm;
   int var_I;

   Stg_Component_Destroy( self->escapedRoutine, data , False );

   for( var_I = 0 ; var_I < self->nSwarmVars ; var_I++ )
   {
      Stg_Component_Destroy( self->swarmVars[var_I], data , False );
   }

   _Swarm_Destroy( self, data );

}

void _GeneralSwarm_UpdateHook( void* timeIntegrator, void* swarm )
{
   GeneralSwarm* self = (GeneralSwarm*)swarm;
   Index                cell;
   Index                point_I;
   GlobalParticle*      particle;

   /* Need to check for escaped particles before the next block. */
   if ( self->escapedRoutine )
   {
      Stg_Component_Execute( self->escapedRoutine, self, True );
   }

   /* Check that particles have not exited the box after advection */
   if ( self->swarmAdvector  )
   {
      for ( point_I = 0; point_I < self->particleLocalCount; ++point_I )
      {
         particle = (GlobalParticle*)Swarm_ParticleAt( self, point_I );
         cell = particle->owningCell;
         Journal_Firewall(
            cell < self->cellLocalCount,
            Journal_MyStream( Error_Type, self ),
            "In func %s: GlobalParticle '%d' outside element. Coord = {%g, %g, %g}\n",
            __func__,
            point_I,
            particle->coord[ I_AXIS ],
            particle->coord[ J_AXIS ],
            particle->coord[ K_AXIS ] );
      }
   }

}

void* GeneralSwarm_GetExtensionAt( void* swarm, Index point_I, Index extHandle )
{
   GeneralSwarm* self  = (GeneralSwarm*)swarm;
   GlobalParticle*       point;

   point = (GlobalParticle*)Swarm_ParticleAt( self, point_I );
   return ExtensionManager_Get( self->particleExtensionMgr, point, extHandle );

}

void GeneralSwarm_AddParticlesFromCoordArray( void* swarm, Index count, Index dim, double* array )
{
    GeneralSwarm* self  = (GeneralSwarm*)swarm;
    unsigned* cellArray = Memory_Alloc_Array( unsigned, count, "GeneralSwarm_AddParticlesFromCoordArray_CellArray" );
    GlobalParticle localParticle;
    GlobalParticle* particle = &localParticle;
    int cellLocalCount  = self->cellLocalCount;
    int oldParticleCount = self->particleLocalCount;
    int ii;
    int totsLocalParticles=0;
    
    // find which particles are local.  we do this to avoid swarm reallocs.
    for (ii=0; ii<count; ii++) {
        memcpy(&(particle->coord), array + dim*ii, dim*sizeof(double));
        cellArray[ii] = CellLayout_CellOf( self->cellLayout, particle );
        if( cellArray[ii] < cellLocalCount )
            totsLocalParticles++;

    }
    // ok, lets add them to the swarm, now that we know how many are required
    self->particleLocalCount += totsLocalParticles;
    Swarm_Realloc( self );
    int newPartIndex = oldParticleCount;
    for (ii=0; ii<count; ii++) {
        if( cellArray[ii] < cellLocalCount ){
            particle = (GlobalParticle*)Swarm_ParticleAt( self, newPartIndex );
            memcpy(&(particle->coord), array + dim*ii, dim*sizeof(double));
            Swarm_AddParticleToCell( swarm, cellArray[ii], newPartIndex );
            newPartIndex++;
        }
    }

    Memory_Free( cellArray );
    
}

/* This returns a signed value - is there an accepted return value for out-of-bounds ? */

int GeneralSwarm_AddParticle( void* swarm, Index dim, double xI, double xJ, double xK )
{
    GeneralSwarm* self  = (GeneralSwarm*)swarm;
    GlobalParticle localParticle;
    GlobalParticle* particle = &localParticle;
    unsigned cell;
    int newPartIndex; 


   /* If the location is not local, return immediately with index value -1 */
      
   particle->coord[ I_AXIS ] = xI;
   particle->coord[ J_AXIS ] = xJ;
   if(dim == 3)
      particle->coord[ K_AXIS ] = xK;

   cell = CellLayout_CellOf( self->cellLayout, particle );
   if( cell >= self->cellLocalCount )
      return (-1);

   /* Otherwise we allocate the particle to the Swarm and return the new index */

   newPartIndex = self->particleLocalCount;

   self->particleLocalCount ++;
   Swarm_Realloc( self );

   particle = (GlobalParticle*)Swarm_ParticleAt( self, newPartIndex );

   particle->coord[ I_AXIS ] = xI;
   particle->coord[ J_AXIS ] = xJ;
   if(dim==3)
      particle->coord[ K_AXIS ] = xK;

   Swarm_AddParticleToCell( self, cell, newPartIndex );

   return(newPartIndex);
    
}





