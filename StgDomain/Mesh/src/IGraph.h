/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 
** 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/** \file
 ** <b>Role:</b>
 **	Abstract class faciliting how class inheritance is done.
 **
 ** <b>Assumptions:</b>
 **	None
 **
 ** <b>Comments:</b>
 **	None
 **
 ** $Id: IGraph.h 3904 2006-12-14 00:52:06Z LukeHodkinson $
 **
 **/
 
#ifndef __StgDomain_Mesh_IGraph_h__
#define __StgDomain_Mesh_IGraph_h__

extern const Type IGraph_Type;
        
#define __IGraph                                \
    __MeshTopology                              \
    Decomp** locals;                            \
    Sync** remotes;                             \
    int* nBndEls;                               \
    int** bndEls;                               \
    int*** nIncEls;                             \
    int**** incEls;

struct IGraph { __IGraph };

#ifndef ZERO
#define ZERO 0
#endif

#define IGRAPH_DEFARGS                          \
    MESHTOPOLOGY_DEFARGS

#define IGRAPH_PASSARGS                         \
    MESHTOPOLOGY_PASSARGS

IGraph* IGraph_New();

IGraph* _IGraph_New( IGRAPH_DEFARGS );

void IGraph_Init( IGraph* self );

void _IGraph_Init( void* self );

void IGraph_Destruct( IGraph* self );

void _IGraph_Delete( void* self );

void _IGraph_SetNumDims( void* _self, int nDims );

void IGraph_SetComm( void* _self, const Comm* comm );

void IGraph_SetDomain( void* _self, int dim, Sync* sync );

void IGraph_SetElements( void* _self, int dim, int nEls, const int* globals );

void IGraph_SetLocalElements( void* _self, int dim, int nEls, const int* globals );

void IGraph_AddLocalElements( void* _self, int dim, int nEls, const int* globals );

void IGraph_RemoveLocalElements( void* _self, int dim, int nEls, const int* globals, IMap* map );

void IGraph_SetRemoteElements( void* _self, int dim, int nEls, const int* globals );

void IGraph_AddRemoteElements( void* _self, int dim, int nEls, const int* globals );

void IGraph_RemoveRemoteElements( void* _self, int dim, int nEls, const int* globals, IMap* map );

void IGraph_SetBoundaryElements( void* _self, int dim, int nEls, const int* els );

void IGraph_SetIncidence( void* _self, int fromDim, int fromEl, int toDim, int nIncEls, const int* incEls  );

void IGraph_RemoveIncidence( void* _self, int fromDim, int toDim );

void IGraph_InvertIncidence( void* _self, int fromDim, int toDim );

void IGraph_ExpandIncidence( void* _self, int dim );

void _IGraph_SetShadowDepth( void* _self, int depth );

void IGraph_Clear( void* self );

void IGraph_ClearDims( void* _self );

void IGraph_ClearElements( void* _self );

void IGraph_ClearIncidence( void* _self );

int IGraph_GetNumDims( const void* self );

const Comm* IGraph_GetComm( const void* self );

Bool IGraph_HasDomain( const void* self, int dim );

const Sync* IGraph_GetDomain( const void* self, int dim );

void IGraph_GetBoundaryElements( const void* self, int dim, int* nEls, const int** els );

Bool IGraph_HasIncidence( const void* self, int fromDim, int toDim );

int IGraph_GetIncidenceSize( const void* self, int fromDim, int fromEl, int toDim );

void _IGraph_GetIncidence( void* self, int fromDim, int fromEl, int toDim, IArray* inc );

void IGraph_PrintIncidence( const void* _self, int fromDim, int toDim );

#endif /* __StgDomain_Mesh_IGraph_h__ */
