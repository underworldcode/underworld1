/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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
** Role:
**   Tests the MeshSuite
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdio.h>
#include <stdlib.h>

#include "pcu/pcu.h"
#include <StGermain/StGermain.h> 
#include "StgDomain/Geometry/Geometry.h"
#include "StgDomain/Shape/Shape.h"
#include "StgDomain/Mesh/Mesh.h" 
#include "StgDomain/Utils/Utils.h"
#include "StgDomain/Swarm/Swarm.h"

#include "MeshSuite.h"

typedef struct {
   MPI_Comm comm;
   int      rank;
   int      nProcs;
} MeshSuiteData;

int MeshSuite_findOwner( Mesh* mesh, int vert ) {
   IArray* inc;
   int     lowest, cur;
   int     nDims;
   int     ii;

   inc = IArray_New();

   nDims = Mesh_GetDimSize( mesh );
   Mesh_GetIncidence( mesh, 0, vert, nDims, inc );
   lowest = Mesh_DomainToGlobal( mesh, nDims, IArray_GetPtr( inc )[0] );
   for( ii = 1; ii < IArray_GetSize( inc ); ii++ ) {
      cur = Mesh_DomainToGlobal( mesh, nDims, IArray_GetPtr( inc )[ii] );
      if( cur < lowest )
         lowest = cur;
   }
   Stg_Class_Delete( inc );
   Mesh_GlobalToDomain( mesh, nDims, lowest, &lowest );

   return lowest;
}

void MeshSuite_Setup( MeshSuiteData* data ) {
   Journal_Enable_AllTypedStream( False );

   /* MPI Initializations */
   data->comm = MPI_COMM_WORLD;
   MPI_Comm_rank( data->comm, &data->rank );
   MPI_Comm_size( data->comm, &data->nProcs );
}

void MeshSuite_Teardown( MeshSuiteData* data ) {
   Journal_Enable_AllTypedStream( True );
}

void MeshSuite_TestMeshNearVert1D( MeshSuiteData* data ) {
   CartesianGenerator* gen;
   Mesh*               mesh;
   int                 nDims;
   int                 sizes[3];
   double              minCrd[3];
   double              maxCrd[3];
   int                 nInc, *inc;
   IArray*             incArray;
   double*             vert;
   int                 e_i, inc_i;

   sizes[0] = sizes[1] = sizes[2] = 4 * data->nProcs;
   minCrd[0] = minCrd[1] = minCrd[2] = 0.0;
   maxCrd[0] = maxCrd[1] = maxCrd[2] = (double)data->nProcs;

   nDims = 1;
   gen = CartesianGenerator_New( "", NULL );
   MeshGenerator_SetDimSize( gen, nDims );
   CartesianGenerator_SetShadowDepth( gen, 1 );
   CartesianGenerator_SetTopologyParams( gen, sizes, 0, NULL, NULL );
   CartesianGenerator_SetGeometryParams( gen, minCrd, maxCrd );

   mesh = Mesh_New( "", NULL );
   Mesh_SetGenerator( mesh, gen );
   Stg_Component_Build( mesh, NULL, False );
   incArray = IArray_New();

   for( e_i = 0; e_i < Mesh_GetDomainSize( mesh, nDims ); e_i++ ) {
      Mesh_GetIncidence( mesh, nDims, e_i, MT_VERTEX, incArray );
      nInc = IArray_GetSize( incArray );
      inc = IArray_GetPtr( incArray );
      for( inc_i = 0; inc_i < nInc; inc_i++ ) {
         vert = Mesh_GetVertex( mesh, inc[inc_i] );
         if( !Mesh_NearestVertex( mesh, vert ) == inc[inc_i] ) break;
      }
   }
   pcu_check_true( e_i == Mesh_GetDomainSize( mesh, nDims ) );

   Stg_Class_Delete( incArray );

   FreeObject( gen );
   FreeObject( mesh );
}

void MeshSuite_TestMeshNearVert2D( MeshSuiteData* data ) {
   CartesianGenerator* gen;
   Mesh*               mesh;
   int                 nDims;
   int                 sizes[3];
   double              minCrd[3];
   double              maxCrd[3];
   int                 nInc, *inc;
   IArray*             incArray;
   double*             vert;
   int                 e_i, inc_i;

   sizes[0] = sizes[1] = sizes[2] = 4 * data->nProcs;
   minCrd[0] = minCrd[1] = minCrd[2] = 0.0;
   maxCrd[0] = maxCrd[1] = maxCrd[2] = (double)data->nProcs;

   nDims = 2;
   gen = CartesianGenerator_New( "", NULL );
   MeshGenerator_SetDimSize( gen, nDims );
   CartesianGenerator_SetShadowDepth( gen, 1 );
   CartesianGenerator_SetTopologyParams( gen, sizes, 0, NULL, NULL );
   CartesianGenerator_SetGeometryParams( gen, minCrd, maxCrd );

   mesh = Mesh_New( "", NULL );
   Mesh_SetGenerator( mesh, gen );
   Stg_Component_Build( mesh, NULL, False );
   incArray = IArray_New();

   for( e_i = 0; e_i < Mesh_GetDomainSize( mesh, nDims ); e_i++ ) {
      Mesh_GetIncidence( mesh, nDims, e_i, MT_VERTEX, incArray );
       nInc = IArray_GetSize( incArray );
      inc = IArray_GetPtr( incArray );
      for( inc_i = 0; inc_i < nInc; inc_i++ ) {
         vert = Mesh_GetVertex( mesh, inc[inc_i] );
         if( !Mesh_NearestVertex( mesh, vert ) == inc[inc_i] ) break;
      }
   }
   pcu_check_true( e_i == Mesh_GetDomainSize( mesh, nDims ) );

   Stg_Class_Delete( incArray );

   FreeObject( gen );
   FreeObject( mesh );
}

void MeshSuite_TestMeshNearVert3D( MeshSuiteData* data ) {
   CartesianGenerator* gen;
   Mesh*               mesh;
   int                 nDims;
   int                 sizes[3];
   double              minCrd[3];
   double              maxCrd[3];
   int                 nInc, *inc;
   IArray*             incArray;
   double*             vert;
   int                 e_i, inc_i;

   sizes[0] = sizes[1] = sizes[2] = 4 * data->nProcs;
   minCrd[0] = minCrd[1] = minCrd[2] = 0.0;
   maxCrd[0] = maxCrd[1] = maxCrd[2] = (double)data->nProcs;

   nDims = 3;
   gen = CartesianGenerator_New( "", NULL );
   MeshGenerator_SetDimSize( gen, nDims );
   CartesianGenerator_SetShadowDepth( gen, 1 );
   CartesianGenerator_SetTopologyParams( gen, sizes, 0, NULL, NULL );
   CartesianGenerator_SetGeometryParams( gen, minCrd, maxCrd );
   
   mesh = Mesh_New( "", NULL );
   Mesh_SetGenerator( mesh, gen );
   Stg_Component_Build( mesh, NULL, False );
   incArray = IArray_New();

   for( e_i = 0; e_i < Mesh_GetDomainSize( mesh, nDims ); e_i++ ) {
      Mesh_GetIncidence( mesh, nDims, e_i, MT_VERTEX, incArray );
      nInc = IArray_GetSize( incArray );
      inc = IArray_GetPtr( incArray );
      for( inc_i = 0; inc_i < nInc; inc_i++ ) {
         vert = Mesh_GetVertex( mesh, inc[inc_i] );
         if( !Mesh_NearestVertex( mesh, vert ) == inc[inc_i] ) break;
      }
   }
   pcu_check_true( e_i == Mesh_GetDomainSize( mesh, nDims ) );

   Stg_Class_Delete( incArray );

   FreeObject( gen );
   FreeObject( mesh );
}

void MeshSuite_TestMeshSearch( MeshSuiteData* data ) {
   CartesianGenerator* gen;
   Mesh*               mesh;
   int                 nDims;
   int                 sizes[3];
   double              minCrd[3];
   double              maxCrd[3];
   int                 el;
   int                 ii;

   sizes[0] = sizes[1] = sizes[2] = 2 * data->nProcs;
   minCrd[0] = minCrd[1] = minCrd[2] = 0.0;
   maxCrd[0] = maxCrd[1] = maxCrd[2] = (double)data->nProcs;

   nDims = 3;
   gen = CartesianGenerator_New( "", NULL );
   MeshGenerator_SetDimSize( gen, nDims );
   CartesianGenerator_SetShadowDepth( gen, 1 );
   CartesianGenerator_SetTopologyParams( gen, sizes, 0, NULL, NULL );
   CartesianGenerator_SetGeometryParams( gen, minCrd, maxCrd );
   mesh = Mesh_New( "", NULL );
   Mesh_SetGenerator( mesh, gen );
   Stg_Component_Build( mesh, NULL, False );

   for( ii = 0; ii < Mesh_GetLocalSize( mesh, 0 ); ii++ ) {
      if( !Mesh_SearchElements( mesh, Mesh_GetVertex( mesh, ii ), &el ) )
         break;
      if( el != MeshSuite_findOwner( mesh, ii ) )
         break;
   }
   pcu_check_true( ii == Mesh_GetLocalSize( mesh, 0 ) );
}

void MeshSuite( pcu_suite_t* suite ) {
   pcu_suite_setData( suite, MeshSuiteData );
   pcu_suite_setFixtures( suite, MeshSuite_Setup, MeshSuite_Teardown );
   pcu_suite_addTest( suite, MeshSuite_TestMeshNearVert1D );
   pcu_suite_addTest( suite, MeshSuite_TestMeshNearVert2D );
   pcu_suite_addTest( suite, MeshSuite_TestMeshNearVert3D );
   pcu_suite_addTest( suite, MeshSuite_TestMeshSearch );
}


