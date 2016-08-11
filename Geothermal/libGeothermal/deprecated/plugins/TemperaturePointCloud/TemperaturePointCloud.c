/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University 
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
**       * Redistributions of source code must retain the above copyright notice, 
**          this list of conditions and the following disclaimer.
**       * Redistributions in binary form must reproduce the above copyright 
**         notice, this list of conditions and the following disclaimer in the 
**         documentation and/or other materials provided with the distribution.
**       * Neither the name of the Monash University nor the names of its contributors 
**         may be used to endorse or promote products derived from this software 
**         without specific prior written permission.
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
**  Louis.Moresi - Louis.Moresi@monash.edu
**
** Contributors:
** 
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include <mpi.h>
#include <assert.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>
#include "Geothermal/Base/Base.h"

#define MASTER 0

const Type Geothermal_TemperaturePointCloud_Type = (const Type)"Geothermal_TemperaturePointCloud";

typedef struct {
   __Codelet;
   FeVariable*       field;
   Swarm*            swarm;
   BuoyancyMaterial* materialMgr;
   Name              outputPath;
   Bool              gocadOutput;
   Bool              mapTempToNodes;
} Geothermal_TemperaturePointCloud;

void Geothermal_TemperaturePointCloud_Dump( void* _context );

void _Geothermal_TemperaturePointCloud_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
   Geothermal_TemperaturePointCloud* self = (Geothermal_TemperaturePointCloud*)component;
   Dictionary*                       dictionary = Codelet_GetPluginDictionary( component, cf->rootDict );
   
   self->field = Stg_ComponentFactory_ConstructByName(
      cf, Dictionary_GetString( dictionary, "TemperatureField" ), FeVariable, True, data );

   self->swarm = Stg_ComponentFactory_ConstructByName(
      cf, Dictionary_GetString( dictionary, "Swarm" ), Swarm, True, data );

   self->materialMgr = Stg_ComponentFactory_ConstructByName(
      cf, Dictionary_GetString( dictionary, "MaterialManager" ), BuoyancyMaterial, False, data );

   self->context = (AbstractContext*)Stg_ComponentFactory_ConstructByName(
      cf, "context", UnderworldContext, True, data ); 

   self->gocadOutput = Dictionary_GetBool_WithDefault( dictionary, (Dictionary_Entry_Key)"GOCADOutput", True );
   self->mapTempToNodes = Dictionary_GetBool_WithDefault( dictionary, (Dictionary_Entry_Key)"MapTemperatureToNodes", True );
   self->outputPath = Dictionary_Entry_Value_AsString( Dictionary_Get( dictionary, "outputPath" ) );

   ContextEP_Append( self->context, AbstractContext_EP_Dump, Geothermal_TemperaturePointCloud_Dump );
}

void _Geothermal_TemperaturePointCloud_Build( void* component, void* data ) {
}

void _Geothermal_TemperaturePointCloud_Initialise( void* component, void* data ) {
}

void _Geothermal_TemperaturePointCloud_Destroy( void* component, void* data ) {
}

void* _Geothermal_TemperaturePointCloud_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                             _sizeOfSelf = sizeof(Geothermal_TemperaturePointCloud);
   Type                                                     type = Geothermal_TemperaturePointCloud_Type;
   Stg_Class_DeleteFunction*                             _delete = _Codelet_Delete;
   Stg_Class_PrintFunction*                               _print = _Codelet_Print;
   Stg_Class_CopyFunction*                                 _copy = _Codelet_Copy;
   Stg_Component_DefaultConstructorFunction* _defaultConstructor = _Geothermal_TemperaturePointCloud_DefaultNew;
   Stg_Component_ConstructFunction*                   _construct = _Geothermal_TemperaturePointCloud_AssignFromXML;
   Stg_Component_BuildFunction*                           _build = _Geothermal_TemperaturePointCloud_Build;
   Stg_Component_InitialiseFunction*                 _initialise = _Geothermal_TemperaturePointCloud_Initialise;
   Stg_Component_ExecuteFunction*                       _execute = _Codelet_Execute;
   Stg_Component_DestroyFunction*                       _destroy = _Geothermal_TemperaturePointCloud_Destroy;

   /* 
    * Variables that are set to ZERO are variables that will be set either by the current 
    * _New function or another parent _New function further up the hierachy.
    */

   /* default value NON_GLOBAL */
   AllocationType nameAllocationType = NON_GLOBAL;

   return _Codelet_New( CODELET_PASSARGS );
}

Index Geothermal_TemperaturePointCloud_Register( PluginsManager* pluginsManager ) {
   return PluginsManager_Submit( pluginsManager, Geothermal_TemperaturePointCloud_Type, (Name)"0", _Geothermal_TemperaturePointCloud_DefaultNew );
}

void _Geothermal_TemperaturePointCloud_WriteFieldValuesFile( void* pointCloud ) {
   Geothermal_TemperaturePointCloud* self = (Geothermal_TemperaturePointCloud*)pointCloud;
   FeVariable*                       field = self->field;
   FeMesh*                           mesh = self->field->feMesh;
   FILE                              *fp, *gp;
   double*                           coord;
   double                            T;
   double                            *localT, *localCoordsX, *localCoordsY, *localCoordsZ;
   double                            *globalT, *globalCoordsX, *globalCoordsY, *globalCoordsZ;
   char                              filename[200], gocadFilename[200];
   int                               *globalCount, *globalNodeCount, *globalDisplacement;
   int                               rank, nProcs;
   MPI_Comm                          comm = self->context->communicator;
   Index                             nodeLocalCount = FeMesh_GetNodeLocalSize( mesh );
   Index                             nodeGlobalCount = FeMesh_GetNodeGlobalSize( mesh );
   Index                             node_I, gNode_I, proc_I;

   MPI_Comm_size( comm, (int*)&nProcs );
   MPI_Comm_rank( comm, (int*)&rank );

   /* Get the number of nodes for each procs. */
   globalNodeCount = Memory_Alloc_Array_Unnamed( int, nProcs );
   (void)MPI_Gather( &nodeLocalCount, 1, MPI_INT, globalNodeCount, 1, MPI_INT, MASTER, comm );

   sprintf( filename, "%s/%s.dat", self->context->outputPath, field->name );

   if( self->gocadOutput )
      sprintf( gocadFilename, "%s/%s.vs", self->context->outputPath, field->name );

   if( rank == MASTER ) {
      /* Allocate memory for global storage. */
      globalCount = Memory_Alloc_Array_Unnamed( int, nProcs );
      globalT = Memory_Alloc_Array_Unnamed( double, nodeGlobalCount );
      globalCoordsX = Memory_Alloc_Array_Unnamed( double, nodeGlobalCount );
      globalCoordsY = Memory_Alloc_Array_Unnamed( double, nodeGlobalCount );
      globalCoordsZ = Memory_Alloc_Array_Unnamed( double, nodeGlobalCount );
      globalDisplacement = Memory_Alloc_Array_Unnamed( int, nProcs );

      /* Compute the displacement based on the number of nodes for each procs. */
      globalDisplacement[0] = 0;
      for( proc_I = 1; proc_I < nProcs; proc_I++ )
         globalDisplacement[proc_I] = globalDisplacement[proc_I - 1] + globalNodeCount[proc_I - 1];

      fp = fopen( filename, "w" );

      if( self->gocadOutput ) {
         /* Write the GOCAD header information. */
         gp = fopen( gocadFilename, "w" );
         fprintf( gp, "GOCAD VSet 1\n" );
         fprintf( gp, "HEADER {\n" );
         fprintf( gp, "name:temperature\n" );
         fprintf( gp, "}\n" );
         fprintf( gp, "GOCAD_ORIGINAL_COORDINATE_SYSTEM\n" );
         fprintf( gp, "NAME Default\n" );
         fprintf( gp, "AXIS_NAME \"X\" \"Y\" \"Z\"\n" );
         fprintf( gp, "AXIS_UNIT \"m\" \"m\" \"m\"\n" );
         fprintf( gp, "ZPOSITIVE Elevation\n" );
         fprintf( gp, "END_ORIGINAL_COORDINATE_SYSTEM\n" );
         fprintf( gp, "PROPERTIES temperature\n" );
         fprintf( gp, "NO_DATA_VALUES -99999\n" );
         fprintf( gp, "PROPERTY_CLASSES temperature\n" );
         fprintf( gp, "PROPERTY_KINDS unknown\n" );
         fprintf( gp, "PROPERTY_SUBCLASSES QUANTITY Float\n" );
         fprintf( gp, "ESIZES 1\n" );
         fprintf( gp, "UNITS none\n" );
         fprintf( gp, "PROPERTY_CLASS_HEADER temperature {\n" );
         fprintf( gp, "name:temperature\n" );
         fprintf( gp, "}\n" );
      }
   }
   else {
      /* If not root proc, set global storage to zero. */
      globalCount = 0;
      globalT = 0;
      globalCoordsX = 0;
      globalCoordsY = 0;
      globalCoordsZ = 0;
      globalDisplacement = 0;
   }

   /* Allocate memory for local storage. */
   localT = Memory_Alloc_Array_Unnamed( double, nodeLocalCount );
   localCoordsX = Memory_Alloc_Array_Unnamed( double, nodeLocalCount );
   localCoordsY = Memory_Alloc_Array_Unnamed( double, nodeLocalCount );
   localCoordsZ = Memory_Alloc_Array_Unnamed( double, nodeLocalCount );

   /* Iterate though each local node and store its coordinates and values in the local storage. */
   for( node_I = 0; node_I < nodeLocalCount; node_I++ ) {
      gNode_I = FeMesh_NodeDomainToGlobal( mesh, node_I );
      coord = Mesh_GetVertex( mesh, node_I );
      FeVariable_GetValueAtNode( field, node_I, &T );

      localCoordsX[node_I] = coord[0];
      localCoordsY[node_I] = coord[2];
      localCoordsZ[node_I] = coord[1];
      localT[node_I] = T;
   }

   /* Perform MPI_Gather to retrieve all node data from the other procs. */
   (void)MPI_Gather( &node_I, 1, MPI_INT, globalCount, 1, MPI_INT, MASTER, comm );
   (void)MPI_Gatherv( localT, node_I, MPI_DOUBLE, globalT, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );
   (void)MPI_Gatherv( localCoordsX, node_I, MPI_DOUBLE, globalCoordsX, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );
   (void)MPI_Gatherv( localCoordsY, node_I, MPI_DOUBLE, globalCoordsY, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );
   (void)MPI_Gatherv( localCoordsZ, node_I, MPI_DOUBLE, globalCoordsZ, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );

   /* Dump node information to the output file/s if rank is root .*/
   if( rank == MASTER ) {
      for( node_I = 0; node_I < nodeGlobalCount; node_I++ ) {
         fprintf( fp, "%f %f %f %f\n", globalCoordsX[node_I], globalCoordsY[node_I], globalCoordsZ[node_I], globalT[node_I] );

         if( self->gocadOutput ) {
            fprintf( gp, "PVRTX %u %f %f %f %f\n",
               node_I, globalCoordsX[node_I], globalCoordsY[node_I], globalCoordsZ[node_I], globalT[node_I] );
         }
      }

      fclose( fp );

      if( self->gocadOutput ) {
         fprintf( gp, "END\n" );
         fclose( gp );
      }

      /* Free global storage. */
      Memory_Free( globalCount );   
      Memory_Free( globalT );
      Memory_Free( globalCoordsX );
      Memory_Free( globalCoordsY );
      Memory_Free( globalCoordsZ );
   }

   /* Free local storage. */
   Memory_Free( localT );
   Memory_Free( localCoordsX );
   Memory_Free( localCoordsY );
   Memory_Free( localCoordsZ );
}

void _Geothermal_TemperaturePointCloud_WritePropertiesFile( void* pointCloud ) {
   Geothermal_TemperaturePointCloud* self = (Geothermal_TemperaturePointCloud*)pointCloud;
   FeVariable*                       field = self->field;
   FeMesh*                           mesh = field->feMesh;
   Swarm*                            swarm = self->swarm;
   BuoyancyMaterial*                 matMgr = self->materialMgr;
   IntegrationPoint*                 particle;
   RheologyMaterial*                 mat;
   BuoyancyMaterial_MaterialExt*     matExt;
   MPI_Comm                          comm = self->context->communicator;
   InterpolationResult               result;
   FILE                              *fp, *gp;
   double                            k, Q, T, gCoord[3];
   double                            *localk, *localQ, *localT;
   double                            *localCoordsX, *localCoordsY, *localCoordsZ;
   double                            *globalk, *globalQ, *globalT;
   double                            *globalCoordsX, *globalCoordsY, *globalCoordsZ;
   unsigned                          el_i, cell_i, part_i;
   char                              filename[200], gocadFilename[200];
   int                               *globalCount, *globalParticleCount, *globalDisplacement;
   int                               totalParticleCount = 0, pCount;
   int                               rank, nProcs;
   Index                             proc_I, particle_I;

   MPI_Comm_size( comm, (int*)&nProcs );
   MPI_Comm_rank( comm, (int*)&rank );

   /* Get the particle count for each procs. */
   globalParticleCount = Memory_Alloc_Array_Unnamed( int, nProcs );
   (void)MPI_Gather( &swarm->particleLocalCount, 1, MPI_INT, globalParticleCount, 1, MPI_INT, MASTER, comm );

   if( self->mapTempToNodes )
      sprintf( filename, "%s/Material-Properties.dat", self->context->outputPath );
   else   
      sprintf( filename, "%s/Temperature-and-Material-Properties.dat", self->context->outputPath );

   if( self->gocadOutput ) {
      if( self->mapTempToNodes )
         sprintf( gocadFilename, "%s/Material-Properties.vs", self->context->outputPath );
      else
         sprintf( gocadFilename, "%s/Temperature-and-Material-Properties.vs", self->context->outputPath );
   } 

   /* Get the total particle count. */
   for( proc_I = 0; proc_I < nProcs; proc_I++ )
      totalParticleCount += globalParticleCount[proc_I];
      
   if( rank == MASTER ) {
      /* Allocate memory for global storage. */
      globalCount = Memory_Alloc_Array_Unnamed( int, nProcs );
      globalk = Memory_Alloc_Array_Unnamed( double, totalParticleCount );
      globalQ = Memory_Alloc_Array_Unnamed( double, totalParticleCount );
      globalT = Memory_Alloc_Array_Unnamed( double, totalParticleCount );
      globalCoordsX = Memory_Alloc_Array_Unnamed( double, totalParticleCount );
      globalCoordsY = Memory_Alloc_Array_Unnamed( double, totalParticleCount );
      globalCoordsZ = Memory_Alloc_Array_Unnamed( double, totalParticleCount );
      globalDisplacement = Memory_Alloc_Array_Unnamed( int, nProcs );

      /* Compute the displacement based on the particle count on each of the procs. */
      globalDisplacement[0] = 0;
      for( proc_I = 1; proc_I < nProcs; proc_I++ )
         globalDisplacement[proc_I] = globalDisplacement[proc_I - 1] +  globalParticleCount[proc_I - 1]; 

      fp = fopen( filename, "w" );

      /* Create GOCAD file header. */
      if( self->gocadOutput ) {
         gp = fopen( gocadFilename, "w" );
         fprintf( gp, "GOCAD VSet 1\n" );
         fprintf( gp, "HEADER {\n" );
         fprintf( gp, "name:material_properties\n" );
         fprintf( gp, "}\n" );
         fprintf( gp, "GOCAD_ORIGINAL_COORDINATE_SYSTEM\n" );
         fprintf( gp, "NAME Default\n" );
         fprintf( gp, "AXIS_NAME \"X\" \"Y\" \"Z\"\n" );
         fprintf( gp, "AXIS_UNIT \"m\" \"m\" \"m\"\n" );
         fprintf( gp, "ZPOSITIVE Elevation\n" );
         fprintf( gp, "END_ORIGINAL_COORDINATE_SYSTEM\n" );
      
         /* Adjust header info depending on the number of properties. */   
         if( self->mapTempToNodes ) {
            fprintf( gp, "PROPERTIES k Q\n" );
            fprintf( gp, "NO_DATA_VALUES -99999 -99999\n" );
            fprintf( gp, "PROPERTY_CLASSES k Q\n" );
            fprintf( gp, "PROPERTY_KINDS unknown unknown\n" );
            fprintf( gp, "PROPERTY_SUBCLASSES QUANTITY Float QUANTITY Float\n" );
            fprintf( gp, "ESIZES 1 1\n" );
            fprintf( gp, "UNITS none none\n" );
            fprintf( gp, "PROPERTY_CLASS_HEADER k {\n" );
            fprintf( gp, "name:k\n" );
            fprintf( gp, "}\n" );
            fprintf( gp, "PROPERTY_CLASS_HEADER Q {\n" );
            fprintf( gp, "name:Q\n" );
            fprintf( gp, "}\n" );
         }
         else {
            fprintf( gp, "PROPERTIES k Q T\n" );
            fprintf( gp, "NO_DATA_VALUES -99999 -99999 -99999\n" );
            fprintf( gp, "PROPERTY_CLASSES k Q T\n" );
            fprintf( gp, "PROPERTY_KINDS unknown unknown unknown\n" );
            fprintf( gp, "PROPERTY_SUBCLASSES QUANTITY Float QUANTITY Float QUANTITY Float\n" );
            fprintf( gp, "ESIZES 1 1 1\n" );
            fprintf( gp, "UNITS none none none\n" );
            fprintf( gp, "PROPERTY_CLASS_HEADER k {\n" );
            fprintf( gp, "name:k\n" );
            fprintf( gp, "}\n" );
            fprintf( gp, "PROPERTY_CLASS_HEADER Q {\n" );
            fprintf( gp, "name:Q\n" );
            fprintf( gp, "}\n" );
            fprintf( gp, "PROPERTY_CLASS_HEADER T {\n" );
            fprintf( gp, "name:T\n" );
            fprintf( gp, "}\n" );
         }   
      }
   }
   else {
      /* Set the global storage to zero of rank is not root. */
      fp = 0;
      gp = 0;
      globalCount = 0;
      globalk = 0;
      globalQ = 0;
      globalT = 0;
      globalCoordsX = 0;
      globalCoordsY = 0;
      globalCoordsZ = 0;
      globalDisplacement = 0;
   }

   /* Allocate memory for local storage. */
   localk = Memory_Alloc_Array_Unnamed( double, swarm->particleLocalCount );
   localQ = Memory_Alloc_Array_Unnamed( double, swarm->particleLocalCount );
   localT = Memory_Alloc_Array_Unnamed( double, swarm->particleLocalCount );
   localCoordsX = Memory_Alloc_Array_Unnamed( double, swarm->particleLocalCount );
   localCoordsY = Memory_Alloc_Array_Unnamed( double, swarm->particleLocalCount );
   localCoordsZ = Memory_Alloc_Array_Unnamed( double, swarm->particleLocalCount );

   pCount = 0;
   /* Write the material properties (thermal conductivity and heat production). */
   for( el_i = 0; el_i < Mesh_GetLocalSize( mesh, MT_VOLUME ); el_i++ ) {
      cell_i = CellLayout_MapElementIdToCellId( swarm->cellLayout, el_i );
      
      for( part_i = 0; part_i < swarm->cellParticleCountTbl[cell_i]; part_i++ ) {
         particle = (IntegrationPoint*)Swarm_ParticleInCellAt( swarm, cell_i, part_i );
         mat = (RheologyMaterial*)IntegrationPointsSwarm_GetMaterialOn( (IntegrationPointsSwarm*)swarm, particle );
         matExt = ExtensionManager_Get( mat->extensionMgr, mat, matMgr->materialExtHandle );
         k = matExt->extensions[0];
         Q = matExt->extensions[1];

         FeMesh_CoordLocalToGlobal( mesh, el_i, particle->xi, gCoord );
         result = FieldVariable_InterpolateValueAt( field, gCoord, &T );
         
         /* Save calculated material properties and values onto the local storage. */
         localk[pCount] = k;
         localQ[pCount] = Q;
         localT[pCount] = T;
         localCoordsX[pCount] = gCoord[0];
         localCoordsY[pCount] = gCoord[2];
         localCoordsZ[pCount] = gCoord[1];
         pCount++;
      }
   }

   /* Massive gather on the particle coordinates, material properties and values. */
   (void)MPI_Gather( &pCount, 1, MPI_INT, globalCount, 1, MPI_INT, MASTER, comm );
   (void)MPI_Gatherv( localk, pCount, MPI_DOUBLE, globalk, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );
   (void)MPI_Gatherv( localQ, pCount, MPI_DOUBLE, globalQ, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );
   (void)MPI_Gatherv( localT, pCount, MPI_DOUBLE, globalT, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );
   (void)MPI_Gatherv( localCoordsX, pCount, MPI_DOUBLE, globalCoordsX, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );
   (void)MPI_Gatherv( localCoordsY, pCount, MPI_DOUBLE, globalCoordsY, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );
   (void)MPI_Gatherv( localCoordsZ, pCount, MPI_DOUBLE, globalCoordsZ, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );

   if( rank == MASTER ) {
      /* For each particles, print out the coords, material properties and values if set. */
      for( particle_I = 0; particle_I < totalParticleCount; particle_I++ ) {
         if( self->mapTempToNodes ) {
            fprintf( fp, "%f %f %f %f %f\n",
               globalCoordsX[particle_I], globalCoordsY[particle_I], globalCoordsZ[particle_I],
               globalk[particle_I], globalQ[particle_I] );
         }
         else {
            fprintf( fp, "%f %f %f %f %f %f\n",
               globalCoordsX[particle_I], globalCoordsY[particle_I], globalCoordsZ[particle_I],
               globalk[particle_I], globalQ[particle_I], globalT[particle_I] );
         }

         /* Also print the information on the GOCAD file if enabled. */
         if( self->gocadOutput ) {
            if( self->mapTempToNodes ) {
               fprintf( gp, "PVRTX %u %f %f %f %f %f\n",
                  particle_I, globalCoordsX[particle_I], globalCoordsY[particle_I], globalCoordsZ[particle_I],
                  globalk[particle_I], globalQ[particle_I] );
            }
            else {
               fprintf( gp, "PVRTX %u %f %f %f %f %f %f\n",
                  particle_I, globalCoordsX[particle_I], globalCoordsY[particle_I], globalCoordsZ[particle_I],
                  globalk[particle_I], globalQ[particle_I], globalT[particle_I] );
            }
         }
      }
      fclose( fp );

      /* Close gocad file. */
      if( self->gocadOutput ) {
         fprintf( gp, "END\n" );
         fclose( gp );
      }
    
      /* Free global storage memory. */ 
      Memory_Free( globalCount ); 
      Memory_Free( globalk );
      Memory_Free( globalQ );
      Memory_Free( globalT );
      Memory_Free( globalCoordsX );
      Memory_Free( globalCoordsY );
      Memory_Free( globalCoordsZ );
      Memory_Free( globalDisplacement );
   }

   /* Free local storage memory. */
   Memory_Free( localk );
   Memory_Free( localQ );
   Memory_Free( localT );
   Memory_Free( localCoordsX );
   Memory_Free( localCoordsY );
   Memory_Free( localCoordsZ );
}

void Geothermal_TemperaturePointCloud_Dump( void* _context ) {
   UnderworldContext*                context = (UnderworldContext*)_context;
   Geothermal_TemperaturePointCloud* self;

   self = (Geothermal_TemperaturePointCloud*)LiveComponentRegister_Get( context->CF->LCRegister, (Name)Geothermal_TemperaturePointCloud_Type );
   
   if( self->mapTempToNodes )
      _Geothermal_TemperaturePointCloud_WriteFieldValuesFile( self );

   if( self->materialMgr )
      _Geothermal_TemperaturePointCloud_WritePropertiesFile( self );
}
