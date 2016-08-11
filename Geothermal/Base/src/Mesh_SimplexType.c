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
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <mpi.h>

#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>

#include "types.h"
#include "Mesh_SimplexType.h"


/* Textual name of this class */
const Type Mesh_SimplexType_Type = (const Type)"Mesh_SimplexType";


/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/

Mesh_SimplexType* Mesh_SimplexType_New( Name name ) {
   /* Variables set in this function */
   SizeT                                                    _sizeOfSelf = sizeof(Mesh_SimplexType);
   Type                                                            type = Mesh_SimplexType_Type;
   Stg_Class_DeleteFunction*                                    _delete = _Mesh_SimplexType_Delete;
   Stg_Class_PrintFunction*                                      _print = _Mesh_SimplexType_Print;
   Stg_Class_CopyFunction*                                        _copy = NULL;
   Mesh_ElementType_UpdateFunc*                              updateFunc = Mesh_SimplexType_Update;
   Mesh_ElementType_ElementHasPointFunc*            elementHasPointFunc = Mesh_SimplexType_ElementHasPoint;
   Mesh_ElementType_GetMinimumSeparationFunc*  getMinimumSeparationFunc = Mesh_SimplexType_GetMinimumSeparation;
   Mesh_ElementType_GetCentroidFunc*                    getCentroidFunc = _Mesh_ElementType_GetCentroid;

   return _Mesh_SimplexType_New(  MESH_HEXTYPE_PASSARGS  );
}

Mesh_SimplexType* _Mesh_SimplexType_New(  MESH_HEXTYPE_DEFARGS  ) {
   Mesh_SimplexType* self;

   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(Mesh_SimplexType) );
   self = (Mesh_SimplexType*)_Mesh_ElementType_New(  MESH_ELEMENTTYPE_PASSARGS  );

   /* Virtual info */

   /* Mesh_SimplexType info */
   _Mesh_SimplexType_Init( self );

   return self;
}

void _Mesh_SimplexType_Init( Mesh_SimplexType* self ) {
   assert( self && Stg_CheckType( self, Mesh_SimplexType ) );

   self->vertMap = AllocArray( unsigned, 4 );
   self->inc = AllocArray( unsigned, 4 );
   Mesh_SimplexType_SetVertexMap( self, NULL );

   self->elementHasPoint = NULL;

   self->incArray = IArray_New();

   self->triInds = AllocArray2D( unsigned, 1, 3 );
   self->triInds[0][0] = 0; self->triInds[0][1] = 1; self->triInds[0][2] = 2;
}


/*----------------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _Mesh_SimplexType_Delete( void* elementType ) {
   Mesh_SimplexType*   self = (Mesh_SimplexType*)elementType;

   FreeArray( self->vertMap );
   FreeArray( self->inc );
   Stg_Class_Delete( self->incArray );
   FreeArray( self->triInds );

   /* Delete the parent. */
   _Mesh_ElementType_Delete( self );
}

void _Mesh_SimplexType_Print( void* elementType, Stream* stream ) {
   Mesh_SimplexType*   self = (Mesh_SimplexType*)elementType;
   Stream*         elementTypeStream;

   elementTypeStream = Journal_Register( InfoStream_Type, (Name)"Mesh_SimplexTypeStream"  );

   /* Print parent */
   Journal_Printf( stream, "Mesh_SimplexType (ptr): (%p)\n", self );
   _Mesh_ElementType_Print( self, stream );
}

void Mesh_SimplexType_Update( void* simplexType ) {
   Mesh_SimplexType*   self = (Mesh_SimplexType*)simplexType;
   unsigned   nDims;
   unsigned   d_i;

   assert( self && Stg_CheckType( self, Mesh_SimplexType ) );

   nDims = Mesh_GetDimSize( self->mesh );
   for( d_i = 0; d_i < nDims; d_i++ ) {
      if( (!Mesh_GetGlobalSize( self->mesh, d_i ) ||
         !Mesh_HasIncidence( self->mesh, nDims, d_i )) ) {
         break;
      }
   }

   if( Mesh_GetDimSize( self->mesh ) == 3 ) {
      printf( "error: have not yet implemented 3d simplex type\n" );
      abort();
   }
   else if( Mesh_GetDimSize( self->mesh ) == 2 ) {
      self->elementHasPoint = (Mesh_ElementType_ElementHasPointFunc*)Mesh_SimplexType_ElementHasPoint2D;
   }
   else {
      printf( "error: have not yet implemented 1d simplex type\n" );
      abort();
   }
}

Bool Mesh_SimplexType_ElementHasPoint( void* simplexType, unsigned elInd, double* point, MeshTopology_Dim* dim, unsigned* ind ) {
   Mesh_SimplexType*   self = (Mesh_SimplexType*)simplexType;

   assert( self && Stg_CheckType( self, Mesh_SimplexType ) );
   assert( Mesh_GetDimSize( self->mesh ) <= 3 );
   assert( self->elementHasPoint );

   return self->elementHasPoint( self, elInd, point, dim, ind );
}

/* TODO: assuming 2-D mesh for now... also slow due to the sqrt, need to optimise */
double Mesh_SimplexType_GetMinimumSeparation( void* simplexType, unsigned elInd, double* perDim ) {
   Mesh_SimplexType*   self          = (Mesh_SimplexType*)simplexType;
   double         dx, dy, sepSq, minSepSq, mindx, mindy;
   Mesh*         mesh         = self->mesh;
   unsigned      nInc, *inc;

   assert( self );
   assert( elInd < Mesh_GetDomainSize( mesh, Mesh_GetDimSize( mesh ) ) );
   assert( Mesh_GetDimSize( mesh ) == 2 );

   Mesh_GetIncidence( mesh, Mesh_GetDimSize( mesh ), elInd, MT_VERTEX, self->incArray );
   nInc = IArray_GetSize( self->incArray );
   inc = IArray_GetPtr( self->incArray );

   mindx = Mesh_GetVertex( mesh, inc[0] )[0] - Mesh_GetVertex( mesh, inc[1] )[0];
   mindy = Mesh_GetVertex( mesh, inc[0] )[1] - Mesh_GetVertex( mesh, inc[1] )[1];
   minSepSq = mindx*mindx + mindy*mindy;

   dx = Mesh_GetVertex( mesh, inc[1] )[0] - Mesh_GetVertex( mesh, inc[2] )[0];
   dy = Mesh_GetVertex( mesh, inc[1] )[1] - Mesh_GetVertex( mesh, inc[2] )[1];
   sepSq = dx*dx + dy*dy;
   if( sepSq < minSepSq )
      minSepSq = sepSq;
   if( fabs( dx ) < fabs( mindx ) )
      mindx = dx;
   if( fabs( dy ) < fabs( mindy ) )
      mindx = dy;
   
   dx = Mesh_GetVertex( mesh, inc[2] )[0] - Mesh_GetVertex( mesh, inc[0] )[0];
   dy = Mesh_GetVertex( mesh, inc[2] )[1] - Mesh_GetVertex( mesh, inc[0] )[1];
   sepSq = dx*dx + dy*dy;
   if( sepSq < minSepSq )
      minSepSq = sepSq;
   if( fabs( dx ) < fabs( mindx ) )
      mindx = dx;
   if( fabs( dy ) < fabs( mindy ) )
      mindx = dy;

   perDim[0] = fabs( mindx );
   perDim[1] = fabs( mindy );

   return sqrt( minSepSq );
}


/*--------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/

void Mesh_SimplexType_SetVertexMap( void* simplexType, unsigned* map ) {
   Mesh_SimplexType*   self = (Mesh_SimplexType*)simplexType;
   unsigned   v_i;

   assert( self && Stg_CheckType( self, Mesh_SimplexType ) );

   if( map ) {
      self->mapSize = (Mesh_GetDimSize( self->mesh ) == 3) ? 4 : 3;
      memcpy( self->vertMap, map, self->mapSize * sizeof(unsigned) );
   }
   else {
      self->mapSize = 0;
      for( v_i = 0; v_i < 4; v_i++ )
         self->vertMap[v_i] = v_i;
   }
}


/*----------------------------------------------------------------------------------------------------------------------------------
** Private Functions
*/


/* this implementation assumes a 2-D planar mesh within a 2-D space for efficiency. for a 2-D planar mesh within
 * a 3-D space a seperate implementation is required, see http://www.blackpawn.com/texts/pointinpoly/default.html */
Bool Mesh_SimplexType_ElementHasPoint2D( Mesh_SimplexType* self, unsigned elInd, double* point, 
                   MeshTopology_Dim* dim, unsigned* ind )
{
   Mesh*      mesh;
   unsigned   nInc;
   const int*   inc;
   double      v_ab[2], v_bc[2], v_ca[2];
   double      v_ap[2], v_bp[2], v_cp[2];

   assert( self && Stg_CheckType( self, Mesh_SimplexType ) );
   assert( Mesh_GetDimSize( self->mesh ) == 2 );
   assert( elInd < Mesh_GetDomainSize( self->mesh, Mesh_GetDimSize( self->mesh ) ) );
   assert( point );
   assert( dim );
   assert( ind );

   /* Shortcuts. */
   mesh = self->mesh;

   /* Get element to vertex incidence. */
   Mesh_GetIncidence( mesh, MT_FACE, elInd, MT_VERTEX, self->incArray );
   nInc = IArray_GetSize( self->incArray );
   inc = IArray_GetPtr( self->incArray );

   v_ab[0] = mesh->verts[inc[1]][0] - mesh->verts[inc[0]][0];
   v_ab[1] = mesh->verts[inc[1]][1] - mesh->verts[inc[0]][1];
   v_bc[0] = mesh->verts[inc[2]][0] - mesh->verts[inc[1]][0];
   v_bc[1] = mesh->verts[inc[2]][1] - mesh->verts[inc[1]][1];
   v_ca[0] = mesh->verts[inc[0]][0] - mesh->verts[inc[2]][0];
   v_ca[1] = mesh->verts[inc[0]][1] - mesh->verts[inc[2]][1];

   v_ap[0] = point[0] - mesh->verts[inc[0]][0];
   v_ap[1] = point[1] - mesh->verts[inc[0]][1];
   v_bp[0] = point[0] - mesh->verts[inc[1]][0];
   v_bp[1] = point[1] - mesh->verts[inc[1]][1];
   v_cp[0] = point[0] - mesh->verts[inc[2]][0];
   v_cp[1] = point[1] - mesh->verts[inc[2]][1];

   if( ( v_ab[0]*v_ap[1] - v_ab[1]*v_ap[0] )*( v_ab[1]*v_ca[0] - v_ab[0]*v_ca[1] ) < 0.0 )
      return False;

   if( ( v_bc[0]*v_bp[1] - v_bc[1]*v_bp[0] )*( v_bc[1]*v_ab[0] - v_bc[0]*v_ab[1] ) < 0.0 )
      return False;

   if( ( v_ca[0]*v_cp[1] - v_ca[1]*v_cp[0] )*( v_ca[1]*v_bc[0] - v_ca[0]*v_bc[1] ) < 0.0 )
      return False;

   *ind = elInd;
   *dim = MT_FACE;

   return True;
}

#if 0
Bool Mesh_SimplexType_ElementHasPoint2D( Mesh_SimplexType* self, unsigned elInd, double* point, MeshTopology_Dim* dim, unsigned* ind ) {
   Mesh*      mesh;
   unsigned   nInc;
   const int*   inc;
   Bool      found;
   double      bc[3];
   unsigned   inside;

   assert( self && Stg_CheckType( self, Mesh_SimplexType ) );
   assert( Mesh_GetDimSize( self->mesh ) == 2 );
   assert( elInd < Mesh_GetDomainSize( self->mesh, Mesh_GetDimSize( self->mesh ) ) );
   assert( point );
   assert( dim );
   assert( ind );

   /* Shortcuts. */
   mesh = self->mesh;

   /* Get element to vertex incidence. */
   Mesh_GetIncidence( mesh, MT_FACE, elInd, MT_VERTEX, self->incArray );
   nInc = IArray_GetSize( self->incArray );
   inc = IArray_GetPtr( self->incArray );

   found = Simplex_Search2D( mesh->verts, (unsigned*)inc, 1, self->triInds, point, bc, &inside );
   if( found ) {
      *dim = MT_FACE;
      *ind = elInd;
      return True;
   }

   return False;
}
#endif

