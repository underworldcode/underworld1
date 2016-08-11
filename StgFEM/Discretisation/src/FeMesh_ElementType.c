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
#include <float.h>

#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>

#include "Discretisation.h"

/* Textual name of this class */
const Type FeMesh_ElementType_Type = "FeMesh_ElementType";

/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/

FeMesh_ElementType* FeMesh_ElementType_New( Name name ) {
   /* Variables set in this function */
   SizeT                                       _sizeOfSelf = sizeof(FeMesh_ElementType);
   Type                                             type = FeMesh_ElementType_Type;
   Stg_Class_DeleteFunction*                           _delete = _FeMesh_ElementType_Delete;
   Stg_Class_PrintFunction*                            _print = _FeMesh_ElementType_Print;
   Stg_Class_CopyFunction*                              _copy = NULL;
   Mesh_ElementType_UpdateFunc*                      updateFunc = FeMesh_ElementType_Update;
   Mesh_ElementType_ElementHasPointFunc*         elementHasPointFunc = FeMesh_ElementType_ElementHasPoint;
	 Mesh_ElementType_GetMinimumSeparationFunc*  getMinimumSeparationFunc = Mesh_HexType_GetMinimumSeparationGeneral;
 //  Mesh_ElementType_GetMinimumSeparationFunc* getMinimumSeparationFunc = Mesh_HexType_GetMinimumSeparation;
   Mesh_ElementType_GetCentroidFunc*               getCentroidFunc = _Mesh_ElementType_GetCentroid;

   return _FeMesh_ElementType_New( FEMESH_ELEMENTTYPE_PASSARGS );
}

void FeMesh_ElementType_Update( void* hexType ) {
   Mesh_HexType*  self = (Mesh_HexType*)hexType;

   if( self->mesh->isRegular == False ) {
      self->elementHasPointFunc = FeMesh_ElementType_ElementHasPoint_ForIrregular;
   }
}
FeMesh_ElementType* _FeMesh_ElementType_New( FEMESH_ELEMENTTYPE_DEFARGS ) {
   FeMesh_ElementType* self;

   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(FeMesh_ElementType) );
   self = (FeMesh_ElementType*)_Mesh_HexType_New( MESH_HEXTYPE_PASSARGS );

   /* Virtual info */

   /* FeMesh_ElementType info */
   _FeMesh_ElementType_Init( self );

   return self;
}

void _FeMesh_ElementType_Init( FeMesh_ElementType* self ) {
   assert( self && Stg_CheckType( self, FeMesh_ElementType ) );

   _Mesh_HexType_Init( (Mesh_HexType*) self );
}

/*----------------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _FeMesh_ElementType_Delete( void* elementType ) {
   FeMesh_ElementType* self = (FeMesh_ElementType*)elementType;

   /* Delete the parent. */
   _Mesh_HexType_Delete( self );
}

void _FeMesh_ElementType_Print( void* elementType, Stream* stream ) {
   FeMesh_ElementType* self = (FeMesh_ElementType*)elementType;
   Stream*             elementTypeStream;

   elementTypeStream = Journal_Register( InfoStream_Type, (Name)"FeMesh_ElementTypeStream" );

   /* Print parent */
   Journal_Printf( stream, "FeMesh_ElementType (ptr): (%p)\n", self );
   _Mesh_HexType_Print( self, stream );
}

Bool FeMesh_ElementType_ElementHasPoint(
   void*             hexType,
   unsigned          elInd,
   double*           point, 
   MeshTopology_Dim* dim,
   unsigned*         ind )
{
   FeMesh_ElementType* self = (FeMesh_ElementType*)hexType;
   FeMesh              *mesh=NULL;
   const double        epsilon = 1e-7;
   int                 nDims, ii;
   Bool                border = False;

   IArray              *inc=NULL;
   ElementType         *elType=NULL;

   assert( self && Stg_CheckType( self, FeMesh_ElementType ) );

   mesh = (FeMesh*)self->mesh;

   elType = FeMesh_GetElementType( mesh, elInd );
   nDims = Mesh_GetDimSize( mesh );
   assert( nDims <= 3 );

   // generate possible local coord
   FeMesh_CoordGlobalToLocal( mesh, elInd, point, self->local );

   for( ii = 0; ii < nDims; ii++ ) {
      if( self->local[ii] < -1.0 - epsilon ||
         self->local[ii] > 1.0 + epsilon ) {
         return False;
      }

      if( self->local[ii] < -1.0 + epsilon ||
         self->local[ii] > 1.0 - epsilon ) {
         border = True;
      }
   }

   if( border ) {
      // Find the lowest indexed owning element.
      inc = IArray_New();
      double local[3];
      int    myGlobal, otherGlobal;
      int    jj, kk;

      myGlobal = FeMesh_ElementDomainToGlobal( mesh, elInd );

      Mesh_GetIncidence( mesh, nDims, elInd, nDims, inc );
      for( jj = 0; jj < inc->size; jj++ ) {
         FeMesh_CoordGlobalToLocal( mesh, inc->ptr[jj], point, local );
         for( kk = 0; kk < nDims; kk++ ) {
            if( local[kk] < -1.0 - epsilon ||
               local[kk] > 1.0 + epsilon ) {
               // This guy does not want the point.
               break;
            }
         }
         if( kk == nDims ) {
            // This guy wants the point too. Lowest index
            // wins.
            otherGlobal = FeMesh_ElementDomainToGlobal( mesh, inc->ptr[jj] );
            if( otherGlobal < myGlobal ) {
               Stg_Class_Delete( inc );
               return False;
            }
         }
      }
      Stg_Class_Delete( inc );
   }

   *dim = nDims;
   *ind = elInd;
   return True;
}

Bool FeMesh_ElementType_ElementHasPoint_ForIrregular(
   void*             hexType,
   unsigned          elInd,
   double*           point, 
   MeshTopology_Dim* dim,
   unsigned*         ind )
{
   FeMesh_ElementType* self = (FeMesh_ElementType*)hexType;
   FeMesh              *mesh=NULL;
   const double        epsilon = 1e-7;
   int                 nDims;

   IArray              *inc=NULL;
   ElementType         *elType=NULL;

   int n_i, dim_i;
   int *nodes, nNodes;
   double shapeFuncs[27], coord[3], *nodeCoord, lengthScale;

   assert( self && Stg_CheckType( self, FeMesh_ElementType ) );

   mesh = (FeMesh*)self->mesh;

   elType = FeMesh_GetElementType( mesh, elInd );
   nDims = Mesh_GetDimSize( mesh );
   assert( nDims <= 3 );

   // first test if point belongs in element elInd
   if ( FeMesh_ElementType_ElementHasPoint( hexType, elInd, point, dim, ind ) == False ) return False;

   /* sanity test if original global coords agree with evaluated position for local coord interpolation
      i.e. global_coords = \sum _i N_i\bar{x_i}(\zeta) */
   inc = IArray_New();
   Mesh_GetIncidence( mesh, Mesh_GetDimSize(mesh), elInd, MT_VERTEX, inc );
   //Mesh_GetIncidence( mesh, nDims, elInd, 0, inc );
   nNodes = IArray_GetSize( inc );
   nodes = IArray_GetPtr( inc );

   memset( coord, 0, sizeof(double)*3);
   ElementType_EvaluateShapeFunctionsAt( elType, self->local, shapeFuncs );
   for( n_i=0; n_i<nNodes; n_i++ ) {
      nodeCoord = Mesh_GetVertex(mesh,nodes[n_i]); // get node coords

      // global_coords = \sum _i N_i\bar{x_i}(\zeta)
      for( dim_i=0; dim_i<nDims; dim_i++ ) {
	 coord[dim_i] = coord[dim_i] + shapeFuncs[n_i]*nodeCoord[dim_i];
      }
   }

   // calc a length scale for error measure, simple implementation - using the 1st and 4th node in element.
   lengthScale = fabs( (Mesh_GetVertex(mesh, nodes[0])[0]) - (Mesh_GetVertex(mesh, nodes[3])[0]) ) ;
   assert( lengthScale > DBL_MIN );

   for( dim_i = 0; dim_i<nDims; dim_i++ ) {
      if( fabs(coord[dim_i] - point[dim_i]) > epsilon*lengthScale ) {
	 Stg_Class_Delete( inc );
	 return False;
      }	
   }

   Stg_Class_Delete( inc );
	
   *dim = nDims;
   *ind = elInd;
   return True;
}

/*--------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/

void FeMesh_ElementTypeGetLocal( FeMesh_ElementType* self, double* xi ) { memcpy( xi, self->local, 3*sizeof(double) ); }

/*----------------------------------------------------------------------------------------------------------------------------------
** Private Functions
*/



