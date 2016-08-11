/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2013, Monash University 
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
**  Steve Quenette - Steve.Quenette@monash.edu
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
#define COORDS_COUNT 3

const Type Geothermal_DepthMap_Type = (const Type)"Geothermal_DepthMap";

typedef struct {
   __Codelet;
   FeVariable* field;
   Swarm*      swarm;
   double      depth;
   double*     nodeValues;
   double**    nodeCoords;
   Name        outputPath;
   Bool        outputAllNodes;
   Bool        outputTopNodes;
   Bool        gocadOutput;
   Index       topNodesCount;
} Geothermal_DepthMap;

void Geothermal_DepthMap_Dump( void* _context );

void _Geothermal_DepthMap_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
   Geothermal_DepthMap* self = (Geothermal_DepthMap*)component;
   Dictionary*          dictionary = Codelet_GetPluginDictionary( component, cf->rootDict );
   Stream*              errorStream = Journal_Register( ErrorStream_Type, Geothermal_DepthMap_Type );
	
   Stream_SetPrintingRank( errorStream, 0 );
   Journal_Printf( errorStream, "WARNING: \"DepthMap\" plugin is depricated, use \"FieldMap\" plugin instead" );

   self->swarm = Stg_ComponentFactory_ConstructByName(
      cf, Dictionary_GetString( dictionary, "Swarm" ), Swarm, True, data );

   self->context = (AbstractContext*)Stg_ComponentFactory_ConstructByName(
      cf, "context", UnderworldContext, True, data ); 

   self->depth = Dictionary_GetDouble_WithDefault( dictionary, (Dictionary_Entry_Key)"DepthFromSurface", 0.0 );
   self->outputPath = Dictionary_GetString_WithDefault( dictionary, (Dictionary_Entry_Key)"outputPath", "./output" );
   self->outputAllNodes = Dictionary_GetBool_WithDefault( dictionary, (Dictionary_Entry_Key)"OutputAllNodes", False );
   self->outputTopNodes = Dictionary_GetBool_WithDefault( dictionary, (Dictionary_Entry_Key)"OutputTopNodes", False );
   self->gocadOutput = Dictionary_GetBool_WithDefault( dictionary, (Dictionary_Entry_Key)"GOCADOutput", True );
   self->outputPath = Dictionary_Entry_Value_AsString( Dictionary_Get( dictionary, "outputPath" ) );

   self->field = Stg_ComponentFactory_ConstructByName(
      cf, Dictionary_GetString( dictionary, "Field" ), FeVariable, True, data );

   ContextEP_Append( self->context, AbstractContext_EP_Dump, Geothermal_DepthMap_Dump );
}

void _Geothermal_DepthMap_Build( void* component, void* data ) {
}

void _Geothermal_DepthMap_Initialise( void* component, void* data ) {
}

void _Geothermal_DepthMap_Destroy( void* component, void* data ) {
   Geothermal_DepthMap* self = (Geothermal_DepthMap*)component;
   
   Memory_Free( self->nodeValues );
   Memory_Free( self->nodeCoords );
}

void* _Geothermal_DepthMap_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                             _sizeOfSelf = sizeof(Geothermal_DepthMap);
   Type                                                     type = Geothermal_DepthMap_Type;
   Stg_Class_DeleteFunction*                             _delete = _Codelet_Delete;
   Stg_Class_PrintFunction*                               _print = _Codelet_Print;
   Stg_Class_CopyFunction*                                 _copy = _Codelet_Copy;
   Stg_Component_DefaultConstructorFunction* _defaultConstructor = _Geothermal_DepthMap_DefaultNew;
   Stg_Component_ConstructFunction*                   _construct = _Geothermal_DepthMap_AssignFromXML;
   Stg_Component_BuildFunction*                           _build = _Geothermal_DepthMap_Build;
   Stg_Component_InitialiseFunction*                 _initialise = _Geothermal_DepthMap_Initialise;
   Stg_Component_ExecuteFunction*                       _execute = _Codelet_Execute;
   Stg_Component_DestroyFunction*                       _destroy = _Geothermal_DepthMap_Destroy;

   /* 
    * Variables that are set to ZERO are variables that will be set either by the current 
    * _New function or another parent _New function further up the hierachy.
    */

   /* default value NON_GLOBAL */
   AllocationType nameAllocationType = NON_GLOBAL;

   return _Codelet_New( CODELET_PASSARGS );
}

Index Geothermal_DepthMap_Register( PluginsManager* pluginsManager ) {
   return PluginsManager_Submit( pluginsManager, Geothermal_DepthMap_Type, (Name)"0", _Geothermal_DepthMap_DefaultNew );
}

void _Geothermal_DepthMap_WriteGocadHeaderForField( FeVariable* field, FILE* file, Name name ) {
   /* Write the GOCAD header information. */
   fprintf( file, "GOCAD VSet 1\n" );
   fprintf( file, "HEADER {\n" );
   fprintf( file, "name:%s\n", name );
   fprintf( file, "}\n" );
   fprintf( file, "GOCAD_ORIGINAL_COORDINATE_SYSTEM\n" );
   fprintf( file, "NAME Default\n" );
   fprintf( file, "AXIS_NAME \"X\" \"Y\" \"Z\"\n" );
   fprintf( file, "AXIS_UNIT \"m\" \"m\" \"m\"\n" );
   fprintf( file, "ZPOSITIVE Elevation\n" );
   fprintf( file, "END_ORIGINAL_COORDINATE_SYSTEM\n" );
   fprintf( file, "PROPERTIES %s\n", field->name );
   fprintf( file, "PROP_LEGAL_RANGES **none** **none** **none** **none**\n" );
   fprintf( file, "NO_DATA_VALUES -99999\n" );
   fprintf( file, "PROPERTY_CLASSES %s\n", field->name );
   fprintf( file, "PROPERTY_KINDS unknown\n" );
   fprintf( file, "PROPERTY_SUBCLASSES QUANTITY Float\n" );
   fprintf( file, "ESIZES 1\n" );
   fprintf( file, "UNITS none\n" );
   fprintf( file, "PROPERTY_CLASS_HEADER %s {\n", field->name );
   fprintf( file, "name:%s\n", field->name );
   fprintf( file, "}\n" );
}

void _Geothermal_DepthMap_RetrieveAndWriteAllNodes( void* depthMap ) {
   Geothermal_DepthMap* self = (Geothermal_DepthMap*)depthMap;
   FeVariable*          field = self->field;
   FeMesh*              mesh = field->feMesh;
   FILE*                fp;
   FILE*                gp;
   double*              coord;
   char                 filename[200];
   char                 gocadFilename[200];
   int                  rank;
   int                  nProcs;
   MPI_Comm             comm = self->context->communicator;
   Index                nodeGlobalCount = FeMesh_GetNodeGlobalSize( mesh );
   Node_LocalIndex      nodeLocalCount = FeMesh_GetNodeLocalSize( mesh );
   Node_LocalIndex      node_I;
   Index                gNode_I, proc_I;
   double               value;
   int                  *globalCount, *globalNodeCount, *globalDisplacement;
   double               *localValues, *localCoordsX, *localCoordsY, *localCoordsZ;
   double               *globalValues, *globalCoordsX, *globalCoordsY, *globalCoordsZ;

   MPI_Comm_size( comm, (int*)&nProcs );
   MPI_Comm_rank( comm, (int*)&rank );

   globalNodeCount = Memory_Alloc_Array_Unnamed( int, nProcs );
   (void)MPI_Gather( &nodeLocalCount, 1, MPI_INT, globalNodeCount, 1, MPI_INT, MASTER, comm );
   
   sprintf( filename, "%s/%s-All.dat", self->context->outputPath, field->name );

   if( self->gocadOutput )
      sprintf( gocadFilename, "%s/%s-All.vs", self->context->outputPath, field->name );

   if( rank == MASTER ) {
      /* Allocate memory for global storage. */
      globalCount = Memory_Alloc_Array_Unnamed( int, nProcs );
      globalValues = Memory_Alloc_Array_Unnamed( double, nodeGlobalCount );
      globalCoordsX = Memory_Alloc_Array_Unnamed( double, nodeGlobalCount );
      globalCoordsY = Memory_Alloc_Array_Unnamed( double, nodeGlobalCount );
      globalCoordsZ = Memory_Alloc_Array_Unnamed( double, nodeGlobalCount );
      globalDisplacement = Memory_Alloc_Array_Unnamed( int, nProcs );
     
      /* Compute the displacement of the nodes for each procs. */ 
      globalDisplacement[0] = 0;
      for( proc_I = 1; proc_I < nProcs; proc_I++ )
         globalDisplacement[proc_I] = globalDisplacement[proc_I - 1] + globalNodeCount[proc_I - 1];
      
      fp = fopen( filename, "w" );

      if( self->gocadOutput ) {
         gp = fopen( gocadFilename, "w" );
         _Geothermal_DepthMap_WriteGocadHeaderForField( field, gp, (Name)"All-Nodes" );
      }
   }
   else {
      /* Set global storage to zero if not root process. */
      globalCount = 0;
      globalValues = 0;
      globalCoordsX = 0;
      globalCoordsY = 0;
      globalCoordsZ = 0;
      globalDisplacement = 0;
   }
  
   /* Allocate memory to local storage. */ 
   localValues = Memory_Alloc_Array_Unnamed( double, nodeLocalCount );
   localCoordsX = Memory_Alloc_Array_Unnamed( double, nodeLocalCount );
   localCoordsY = Memory_Alloc_Array_Unnamed( double, nodeLocalCount );
   localCoordsZ = Memory_Alloc_Array_Unnamed( double, nodeLocalCount );
  
   /* Iterate through each local nodes and retrieve its coordinates and values. */ 
   for( node_I = 0; node_I < nodeLocalCount; node_I++ ) {
      gNode_I = FeMesh_NodeDomainToGlobal( mesh, node_I );
      coord = Mesh_GetVertex( mesh, node_I );
      FeVariable_GetValueAtNode( field, node_I, &value );
     
      /* Store the local node coordinates and value into the local storage. */
      localValues[node_I] = value;
      localCoordsX[node_I] = coord[0];
      localCoordsY[node_I] = coord[2];
      localCoordsZ[node_I] = coord[1];
   }

   /* Perform MPI_Gather to get the node coordinates and values from the other procs. */
   (void)MPI_Gather( &node_I, 1, MPI_INT, globalCount, 1, MPI_INT, MASTER, comm );
   (void)MPI_Gatherv( localValues, node_I, MPI_DOUBLE, globalValues, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );
   (void)MPI_Gatherv( localCoordsX, node_I, MPI_DOUBLE, globalCoordsX, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );
   (void)MPI_Gatherv( localCoordsY, node_I, MPI_DOUBLE, globalCoordsY, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );
   (void)MPI_Gatherv( localCoordsZ, node_I, MPI_DOUBLE, globalCoordsZ, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );
  
   /* If root proc, dump the node information into the output file. */ 
   if( rank == MASTER ) {
      for( node_I = 0; node_I < nodeGlobalCount; node_I++ ) {
         fprintf( fp, "%f %f %f %f\n", globalCoordsX[node_I], globalCoordsY[node_I], globalCoordsZ[node_I], globalValues[node_I] );

         if( self->gocadOutput ) {
            fprintf( gp, "PVRTX %u %f %f %f %f\n",
               node_I, globalCoordsX[node_I], globalCoordsY[node_I], globalCoordsZ[node_I], globalValues[node_I] );
         }   
      }
      
      fclose( fp );

      if( self->gocadOutput ) {
         fprintf( gp, "END\n" );
         fclose( gp );
      }
     
      /* Free global storage. */ 
      Memory_Free( globalValues );
      Memory_Free( globalCoordsX );
      Memory_Free( globalCoordsY );
      Memory_Free( globalCoordsZ );
   }

   /* Free local storage. */   
   Memory_Free( localValues );
   Memory_Free( localCoordsX );
   Memory_Free( localCoordsY );
   Memory_Free( localCoordsZ );
}

void _Geothermal_DepthMap_WriteTopNodes( void* depthMap ) {
   Geothermal_DepthMap* self = (Geothermal_DepthMap*)depthMap;
   FILE*                fp;
   FILE*                gp;
   char                 filename[200];
   char                 gocadFilename[200];
   Index                node_I;
   FeVariable*          field;
   int                  rank;
   MPI_Comm             comm = self->context->communicator;
 
   MPI_Comm_rank( comm, (int*)&rank );
   field = self->field;

   /* If root proc, dump the top nodes information to the output file/s. */
   if( rank == MASTER ) {
      sprintf( filename, "%s/%s-Top.dat", self->context->outputPath, field->name );
      fp = fopen( filename, "w" );

      if( self->gocadOutput ) {
         sprintf( gocadFilename, "%s/%s-Top.vs", self->context->outputPath, field->name );
         gp = fopen( gocadFilename, "w" );

         _Geothermal_DepthMap_WriteGocadHeaderForField( field, gp, (Name)"Top-Nodes" );
      }
      
      /* Iterate through the top nodes (I by K size) and dump its information. */
      for( node_I = 0; node_I < self->topNodesCount; node_I++ ) {
         fprintf( fp, "%f %f %f %f\n",
            self->nodeCoords[node_I][0],
            self->nodeCoords[node_I][2],
            self->nodeCoords[node_I][1],
            self->nodeValues[node_I] );

         if( self->gocadOutput ) {
            fprintf( gp, "PVRTX %u %f %f %f %f\n",
               node_I,
               self->nodeCoords[node_I][0],
               self->nodeCoords[node_I][2],
               self->nodeCoords[node_I][1],
               self->nodeValues[node_I] );
         }
      }
      fclose( fp );

      if( self->gocadOutput ) {
         fprintf( gp, "END\n" );
         fclose( gp );
      }
   }
}

void _Geothermal_DepthMap_RetrieveTopNodes( void* depthMap ) {
   Geothermal_DepthMap* self = (Geothermal_DepthMap*)depthMap;
   Index                gNode_I, node_I = 0;
   FeVariable*          field = NULL;
   FeMesh*              mesh = NULL;
   Grid*                grid = NULL;
   Index                i, k, ijk[3];
   int                  rank;
   MPI_Comm             comm = self->context->communicator;

   MPI_Comm_rank( comm, (int*)&rank );

   field = self->field;
   mesh = field->feMesh;
   grid = *(Grid**)ExtensionManager_Get( mesh->info, mesh, ExtensionManager_GetHandle( mesh->info, (Name)"vertexGrid" ) );
   /* Get the top nodes count, I by K. */
   self->topNodesCount = grid->sizes[0] * grid->sizes[2];

   /* Allocate memory for the node coordinates and values. */
   self->nodeValues = Memory_Alloc_Array_Unnamed( double, self->topNodesCount );
   self->nodeCoords = Memory_Alloc_2DArray_Unnamed( double, self->topNodesCount, COORDS_COUNT );
    
   /* Iterate through the top nodes and store its information. */  
   for( i = 0; i < grid->sizes[0]; i++ ) {
      for( k = 0; k < grid->sizes[2]; k ++ ) {
         ijk[0] = i;
         ijk[1] = grid->sizes[1] - 1;
         ijk[2] = k;

         gNode_I = Grid_Project( grid, ijk );
         FeVariable_GetValueAtNodeGlobal( field, gNode_I, &self->nodeValues[node_I] );
         FeVariable_GetCoordAtNodeGlobal( field, gNode_I, self->nodeCoords[node_I] );
         node_I++;
      }
   }
}

void _Geothermal_DepthMap_TranslateTopNodes( void* depthMap ) {
   Geothermal_DepthMap* self = (Geothermal_DepthMap*)depthMap;
   FILE*                fp;
   FILE*                gp;
   char                 filename[200];
   char                 gocadFilename[200];
   char                 gName[100];
   int                  rank, nProcs;
   double               value;
   double               newCoord[3];
   FeMesh*              mesh = NULL;
   FeVariable*          field;
   InterpolationResult  result;
   int                  *globalCount, *globalDisplacement;
   double               *localCoordsX, *localCoordsY, *localCoordsZ;
   double               *localValues;
   double               *globalCoordsX, *globalCoordsY, *globalCoordsZ;
   double               *globalValues;
   Index                localCount, resize_I, node_I = 0, proc_I;
   MPI_Comm             comm = self->context->communicator;
   
   MPI_Comm_size( comm, (int*)&nProcs );
   MPI_Comm_rank( comm, (int*)&rank );

   MPI_Barrier( comm );

   field = self->field;
   mesh = field->feMesh;

   sprintf( filename, "%s/%s-%f-Depth.dat", self->context->outputPath, field->name, self->depth );

   if( self->gocadOutput )
      sprintf( gocadFilename, "%s/%s-%f-Depth.vs", self->context->outputPath, field->name, self->depth );
   
   if( rank == MASTER ) {
      /* Allocate memory for global storage. */
      globalCount = Memory_Alloc_Array_Unnamed( int, nProcs );
      globalValues = Memory_Alloc_Array_Unnamed( double, self->topNodesCount );
      globalCoordsX = Memory_Alloc_Array_Unnamed( double, self->topNodesCount );
      globalCoordsY = Memory_Alloc_Array_Unnamed( double, self->topNodesCount );
      globalCoordsZ = Memory_Alloc_Array_Unnamed( double, self->topNodesCount );
      globalDisplacement = Memory_Alloc_Array_Unnamed( int, nProcs );

      fp = fopen( filename, "w" );

      if( self->gocadOutput ) {
         gp = fopen( gocadFilename, "w" );
         sprintf( gName, "%f_Depth-Nodes", self->depth );
         _Geothermal_DepthMap_WriteGocadHeaderForField( field, gp, (Name)gName );
      }
   }
   else {
      /* Set global storage to zero if not root process. */
      globalCount = 0;
      globalValues = 0;
      globalCoordsX = 0;
      globalCoordsY = 0;
      globalCoordsZ = 0;
      globalDisplacement = 0;
   }

   /* Allocate memory for local storage. */
   localValues = Memory_Alloc_Array_Unnamed( double, self->topNodesCount );
   localCoordsX = Memory_Alloc_Array_Unnamed( double, self->topNodesCount );
   localCoordsY = Memory_Alloc_Array_Unnamed( double, self->topNodesCount );
   localCoordsZ = Memory_Alloc_Array_Unnamed( double, self->topNodesCount );
   localCount = 0;
   resize_I = 1;

   /* 
    * Iterate though the top nodes and translate it N meters downwards.
    * Then perform a check on the owner of the translated node and retrieve its information
    * to be saved in the local storage.
    */
   for( node_I = 0; node_I < self->topNodesCount; node_I++ ) {
      newCoord[0] = self->nodeCoords[node_I][0];
      newCoord[1] = self->nodeCoords[node_I][1] - self->depth;
      newCoord[2] = self->nodeCoords[node_I][2];

      result = FieldVariable_InterpolateValueAt( field, newCoord, &value );

      switch( result ) {
         case LOCAL:
            localValues[localCount] = value;    
            localCoordsX[localCount] = newCoord[0];
            localCoordsY[localCount] = newCoord[2];
            localCoordsZ[localCount] = newCoord[1];
            localCount++;
            break;
         case OTHER_PROC:
         case SHADOW:
         case OUTSIDE_GLOBAL:
         default:
            break;
      }
   }

   (void)MPI_Gather( &localCount, 1, MPI_INT, globalCount, 1, MPI_INT, MASTER, comm );

   /* Compute the displacement by computing the local node count of each procs. */
   if( rank == MASTER ) {
      globalDisplacement[0] = 0;
      for( proc_I = 1; proc_I < nProcs; proc_I++ )
         globalDisplacement[proc_I] = globalDisplacement[proc_I - 1] + globalCount[proc_I - 1];
   }
 
   /* Perform MPI_Gather to get the node coordinates and values from the other procs. */
   (void)MPI_Gatherv( localValues, localCount, MPI_DOUBLE, globalValues, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );
   (void)MPI_Gatherv( localCoordsX, localCount, MPI_DOUBLE, globalCoordsX, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );
   (void)MPI_Gatherv( localCoordsY, localCount, MPI_DOUBLE, globalCoordsY, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );
   (void)MPI_Gatherv( localCoordsZ, localCount, MPI_DOUBLE, globalCoordsZ, globalCount, globalDisplacement, MPI_DOUBLE, MASTER, comm );
 
   /* If root proc, dump the translated node information into the output file/s. */ 
   if( rank == MASTER ) {
      for( node_I = 0; node_I < self->topNodesCount; node_I++ ) {
         fprintf( fp, "%f %f %f %f\n", globalCoordsX[node_I], globalCoordsY[node_I], globalCoordsZ[node_I], globalValues[node_I] );

         if( self->gocadOutput ) {
            fprintf( gp, "PVRTX %u %f %f %f %f\n",
               node_I, globalCoordsX[node_I], globalCoordsY[node_I], globalCoordsZ[node_I], globalValues[node_I] );
         }
      }     

      fclose( fp );

      if( self->gocadOutput ) {
         fprintf( gp, "END\n" );
         fclose( gp );
      }

      /* Free global storage. */
      Memory_Free( globalCount );
      Memory_Free( globalValues );
      Memory_Free( globalCoordsX );
      Memory_Free( globalCoordsY );
      Memory_Free( globalCoordsZ );
      Memory_Free( globalDisplacement );
   }

   /* Free local storage. */
   Memory_Free( localValues );
   Memory_Free( localCoordsX );
   Memory_Free( localCoordsY );
   Memory_Free( localCoordsZ );
}

void Geothermal_DepthMap_Dump( void* _context ) {
   UnderworldContext*   context = (UnderworldContext*)_context;
   Geothermal_DepthMap* self;

   self = (Geothermal_DepthMap*)LiveComponentRegister_Get( context->CF->LCRegister, (Name)Geothermal_DepthMap_Type );

   if( self->outputAllNodes )
      _Geothermal_DepthMap_RetrieveAndWriteAllNodes( self );

   _Geothermal_DepthMap_RetrieveTopNodes( self );

   if( self->outputTopNodes )
      _Geothermal_DepthMap_WriteTopNodes( self );

   _Geothermal_DepthMap_TranslateTopNodes( self );
}
