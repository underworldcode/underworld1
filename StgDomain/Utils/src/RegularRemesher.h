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
 ** $Id: RegularRemesher.h 3904 2006-12-14 00:52:06Z LukeHodkinson $
 **
 **/
 
#ifndef __StgDomain_Utils_RegularRemesher_h__
#define __StgDomain_Utils_RegularRemesher_h__

#ifdef HAVE_PETSC

extern const Type RegularRemesher_Type;
        
#define __RegularRemesher                       \
    __NewRemesher                               \
    int nDims;                                  \
    int contactDepth;                           \
    double contactSize;                         \
    ISet* remeshDims;                           \
    ISet** staticWalls;                         \
    Sync** syncs;                               \
    double** crds;                              \
    int** nWallVerts;                           \
    int*** wallVerts;                           \
    double*** wallCrds;                         \
    double* contactVerts;                       \
    Bool diffuseCorners;                        \
    Bool diffuseSurface;                        \
    double diffusionCoef;                       \
    AbstractContext* ctx;                       \
    int* surfaceIdxs;


struct RegularRemesher { __RegularRemesher };

#ifndef ZERO
#define ZERO 0
#endif

#define REGULARREMESHER_DEFARGS                 \
    NEWREMESHER_DEFARGS

#define REGULARREMESHER_PASSARGS                \
    NEWREMESHER_PASSARGS

RegularRemesher* RegularRemesher_New();

RegularRemesher* _RegularRemesher_New( REGULARREMESHER_DEFARGS );

void RegularRemesher_Init( RegularRemesher* self );

void _RegularRemesher_Init( void* self );

void RegularRemesher_Destruct( RegularRemesher* self );

void _RegularRemesher_Delete( void* self );

void RegularRemesher_Copy( void* self, const void* op );

void _RegularRemesher_Print( const void* _self, Stream* stream );

void RegularRemesher_Remesh( void* _self );

void RegularRemesher_Build( void* _self );

void RegularRemesher_SetRemeshState( void* _self, int dim, Bool state );

void RegularRemesher_SetStaticWall( void* _self, int dim, int wall, Bool state );

#endif

#endif /* __StgDomain_Utils_RegularRemesher_h__ */
