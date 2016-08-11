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
 ** $Id: SpatialTree.h 3904 2006-12-14 00:52:06Z LukeHodkinson $
 **
 **/
 
#ifndef __StgDomain_Mesh_SpatialTree_h__
#define __StgDomain_Mesh_SpatialTree_h__

extern const Type SpatialTree_Type;
        
#define __SpatialTree                           \
    __Stg_Class                                 \
    Mesh* mesh;                                 \
    int nDims;                                  \
    double* min;                                \
    double* max;                                \
    int nChld;                                  \
    void* root;                                 \
    int tol;                                    \
    int nNodes;

struct SpatialTree { __SpatialTree };

#ifndef ZERO
#define ZERO 0
#endif

#define SPATIALTREE_DEFARGS                     \
    STG_CLASS_DEFARGS

#define SPATIALTREE_PASSARGS                    \
    STG_CLASS_PASSARGS

SpatialTree* SpatialTree_New();

SpatialTree* _SpatialTree_New( SPATIALTREE_DEFARGS );

void SpatialTree_Init( SpatialTree* self );

void _SpatialTree_Init( void* self );

void SpatialTree_Destruct( SpatialTree* self );

void _SpatialTree_Delete( void* self );

void SpatialTree_Copy( void* self, const void* op );

void SpatialTree_SetMesh( void* _self, void* mesh );

void SpatialTree_Rebuild( void* _self );

Bool SpatialTree_Search( void* _self, const double* pnt, int* nEls, int** els );

void SpatialTree_Clear( void* _self );

#endif /* __StgDomain_Mesh_SpatialTree_h__ */
