/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 		* Redistributions of source code must retain the above copyright notice,
** 			this list of conditions and the following disclaimer.
** 		* Redistributions in binary form must reproduce the above copyright
**			notice, this list of conditions and the following disclaimer in the
**			documentation and/or other materials provided with the distribution.
** 		* Neither the name of the Monash University nor the names of its contributors
**			may be used to endorse or promote products derived from this software
**			without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
** THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
** BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
** OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**
** Contact:
*%  Owen Kaluza - Owen.Kaluza(at)monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>

#include <gLucifer/Base/Base.h>

#include "types.h"
#include <gLucifer/Base/DrawingObject.h>
#include "HistoricalSwarmTrajectory.h"

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type lucHistoricalSwarmTrajectory_Type = "lucHistoricalSwarmTrajectory";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
lucHistoricalSwarmTrajectory* _lucHistoricalSwarmTrajectory_New(  LUCHISTORICALSWARMTRAJECTORY_DEFARGS  )
{
   lucHistoricalSwarmTrajectory*					self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucHistoricalSwarmTrajectory) );
   self = (lucHistoricalSwarmTrajectory*) _lucDrawingObject_New(  LUCDRAWINGOBJECT_PASSARGS  );

   return self;
}

void _lucHistoricalSwarmTrajectory_Init(
   lucHistoricalSwarmTrajectory*       self,
   Swarm*                              swarm,
   Bool                                flat,
   double                              lineScaling,
   double                              arrowHead,
   unsigned int 						      historySteps,
   double							         historyTime )
{
   self->swarm             = swarm;
   self->flat              = flat;
   self->scaling           = lineScaling;
   self->arrowHead         = arrowHead;
   self->steps	            = historySteps;
   self->time	            = historyTime;

   /* Append to property string */
   lucDrawingObject_AppendProps(self, "scaling=%g\nflat=%d\narrowhead=%g\nsteps=%d\ntime=%g\n", lineScaling, flat, arrowHead, historySteps, historyTime); 

   /* Create the particle extension */
   /* in this case we must assign a global id to each particle in the swarm; so we add an extension to the particles */
   self->particleIdExtHandle = ExtensionManager_Add( swarm->particleExtensionMgr, "particleID", sizeof(unsigned int) );
}

void _lucHistoricalSwarmTrajectory_Delete( void* drawingObject )
{
   lucHistoricalSwarmTrajectory*  self = (lucHistoricalSwarmTrajectory*)drawingObject;

   _lucDrawingObject_Delete( self );
}

void _lucHistoricalSwarmTrajectory_Print( void* drawingObject, Stream* stream )
{
   lucHistoricalSwarmTrajectory*  self = (lucHistoricalSwarmTrajectory*)drawingObject;

   _lucDrawingObject_Print( self, stream );
}

void* _lucHistoricalSwarmTrajectory_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof(lucHistoricalSwarmTrajectory);
   Type                                                             type = lucHistoricalSwarmTrajectory_Type;
   Stg_Class_DeleteFunction*                                     _delete = _lucHistoricalSwarmTrajectory_Delete;
   Stg_Class_PrintFunction*                                       _print = _lucHistoricalSwarmTrajectory_Print;
   Stg_Class_CopyFunction*                                         _copy = NULL;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _lucHistoricalSwarmTrajectory_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _lucHistoricalSwarmTrajectory_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _lucHistoricalSwarmTrajectory_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _lucHistoricalSwarmTrajectory_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _lucHistoricalSwarmTrajectory_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _lucHistoricalSwarmTrajectory_Destroy;
   lucDrawingObject_SetupFunction*                                _setup = lucDrawingObject_Setup;
   lucDrawingObject_DrawFunction*                                  _draw = _lucHistoricalSwarmTrajectory_Draw;
   lucDrawingObject_CleanUpFunction*                            _cleanUp = lucDrawingObject_CleanUp;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _lucHistoricalSwarmTrajectory_New(  LUCHISTORICALSWARMTRAJECTORY_PASSARGS  );
}

void _lucHistoricalSwarmTrajectory_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data )
{
   lucHistoricalSwarmTrajectory*   self	= (lucHistoricalSwarmTrajectory*)drawingObject;
   Swarm*            swarm;
   unsigned int      historySteps;
   double            historyTime;
   unsigned int      defaultSteps = 100;

   /* Construct Parent */
   _lucDrawingObject_AssignFromXML( self, cf, data );
   unsigned int endstep = self->context->loadFromCheckPoint ? self->context->restartTimestep + self->context->maxTimeSteps : self->context->maxTimeSteps;
   if (endstep < defaultSteps) defaultSteps = endstep;

   swarm         =  Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Swarm", Swarm, True, data  );

   historySteps  =  Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"historySteps", defaultSteps);
   historyTime   =  Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"historyTime", 0  );

   Journal_Firewall(
      swarm->particleLayout->coordSystem == GlobalCoordSystem,
      Journal_MyStream( Error_Type, self ),
      "In func %s, unable to visualise swarm %s because it uses a local coord system layout %s of type %s.\n",
      __func__,
      swarm->name,
      swarm->particleLayout->name,
      swarm->particleLayout->type );

   _lucHistoricalSwarmTrajectory_Init(
      self,
      swarm,
      (Bool) Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"flat", False  ),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"lineScaling", 0.05  ),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"arrowHead", 2.0  ),
      historySteps,
      historyTime );

   /* Disable lighting when flat option specified */
   self->lit = self->flat;
}

void _lucHistoricalSwarmTrajectory_Build( void* drawingObject, void* data ) {}

void _lucHistoricalSwarmTrajectory_Initialise( void* drawingObject, void* data )
{
   lucHistoricalSwarmTrajectory*  self = (lucHistoricalSwarmTrajectory*)drawingObject;
   Swarm*                        swarm = self->swarm;
   int p;
   unsigned int offset = 0;
   unsigned int particle_id;
   unsigned int* counts = Memory_Alloc_Array(unsigned int, self->context->nproc, "particle counts");

   Stg_Component_Initialise( self->swarm, data, False );

   /* Get the count on each proc */
   (void)MPI_Allgather(&swarm->particleLocalCount, 1, MPI_UNSIGNED, counts, 1, MPI_UNSIGNED, self->context->communicator);

   /* Assign a global ID to each particle */
   for (p=0; p < self->context->nproc; p++)
   {
      /* Sum to get offset */
      if (p > 0) offset += counts[p-1];
      if (self->context->rank == p)
      {
         for( particle_id = 0 ; particle_id < swarm->particleLocalCount ; particle_id++ )
         {
            GlobalParticle *g_point = (GlobalParticle*)Swarm_ParticleAt( swarm, particle_id );
            unsigned int *part_ext = ExtensionManager_Get( swarm->particleExtensionMgr, g_point, self->particleIdExtHandle );
            *part_ext = offset + particle_id;
         }
      }
   }
   Memory_Free(counts);
}

void _lucHistoricalSwarmTrajectory_Execute( void* drawingObject, void* data ) {}

void _lucHistoricalSwarmTrajectory_Destroy( void* drawingObject, void* data ) {}

void _lucHistoricalSwarmTrajectory_Draw( void* drawingObject, lucDatabase* database, void* _context )
{
   lucHistoricalSwarmTrajectory* self           = (lucHistoricalSwarmTrajectory*)drawingObject;
   Swarm*                        swarm          = self->swarm;
   Particle_Index                lParticle_I;
   GlobalParticle*               particle;

   /* Loop over all particles and draw lines according to where each one has been */
   for ( lParticle_I = 0 ; lParticle_I < swarm->particleLocalCount ; lParticle_I++ )
   {
      particle = (GlobalParticle*)Swarm_ParticleAt( swarm, lParticle_I );
      unsigned int* particle_id = ExtensionManager_Get( swarm->particleExtensionMgr, particle, self->particleIdExtHandle );

      /* Export particle position */
      if (database)
      {
         float coordf[3] = {particle->coord[0], particle->coord[1], particle->coord[2]};
         lucDatabase_AddVerticesWidth(database, 1, lucTracerType, swarm->particleLocalCount, coordf);
         lucDatabase_AddIndex(database, lucTracerType, *particle_id);

         /* Export particle colour value - not required when using same value for every particle at timestep /
         if (colourMap)
         {
            lucDatabase_AddValues(database, 1, lucTracerType, lucColourValueData, colourMap, &value);
         }*/
      }
   }
}


