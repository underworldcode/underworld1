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
 ** $Id: Decomp.h 3904 2006-12-14 00:52:06Z LukeHodkinson $
 **
 **/
 
#ifndef __StgDomain_Mesh_Decomp_h__
#define __StgDomain_Mesh_Decomp_h__

extern const Type Decomp_Type;
        
#define __Decomp                                \
    __Stg_Class                                 \
    MPI_Comm mpiComm;                          \
    int nGlobals;                              \
    IArray* locals;                            \
    IArray localsObj;                          \
    IMap* inv;                                 \
    IMap invObj;                               \
    int rngBegin;                              \
    int rngEnd;                                \
    IMap* owners;                              \
    IMap ownersObj;

struct Decomp { __Decomp };

#ifndef ZERO
#define ZERO 0
#endif

#define DECOMP_DEFARGS                          \
    STG_CLASS_DEFARGS

#define DECOMP_PASSARGS                         \
    STG_CLASS_PASSARGS

Decomp* Decomp_New();

Decomp* _Decomp_New( DECOMP_DEFARGS );

void Decomp_Init( Decomp* self );

void _Decomp_Init( void* self );

void Decomp_Destruct( Decomp* self );

void _Decomp_Delete( void* self );

void Decomp_Copy( void* self, const void* op );

void Decomp_SetMPIComm( void* _self, MPI_Comm mpiComm );

void Decomp_SetLocals( void* _self, int nLocals, const int* locals );

void Decomp_AddLocals( void* _self, int nLocals, const int* locals );

void Decomp_RemoveLocals( void* _self, int nLocals, const int* locals, IMap* map );

void Decomp_Clear( void* _self );

void Decomp_ClearLocals( void* _self );

MPI_Comm Decomp_GetComm( const void* self );

MPI_Comm Decomp_GetMPIComm( const void* self );

int Decomp_GetNumGlobals( const void* self );

int Decomp_GetNumLocals( const void* self );

void Decomp_GetLocals( const void* self, int* nLocals, const int** locals );

int Decomp_LocalToGlobal( const void* self, int local );

int Decomp_GlobalToLocal( const void* self, int global );

Bool Decomp_TryGlobalToLocal( const void* self, int global, int* local );

void Decomp_FindOwners( const void* _self, int nGlobals, const int* globals, 
                        int* ranks );

void Decomp_Update( Decomp* self );

void Decomp_UpdateOwnerMap( Decomp* self );

#endif /* __StgDomain_Mesh_Decomp_h__ */
