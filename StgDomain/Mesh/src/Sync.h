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
 ** $Id: Sync.h 3904 2006-12-14 00:52:06Z LukeHodkinson $
 **
 **/
 
#ifndef __StgDomain_Mesh_Sync_h__
#define __StgDomain_Mesh_Sync_h__

extern const Type Sync_Type;
        
#define __Sync                                  \
   __Stg_Class                                  \
   Decomp* decomp;                              \
   Comm* comm;                                  \
   int nDomains;                                \
   IArray* remotes;                             \
   IArray remotesObj;                           \
   int* owners;                                 \
   int nShared;                                 \
   int* shared;                                 \
   int* nSharers;                               \
   int** sharers;                               \
   IMap* gr;                                    \
   IMap grObj;                                  \
   IMap* ls;                                    \
   IMap lsObj;                                  \
   int* nSrcs;                                  \
   int** srcs;                                  \
   int* nSnks;                                  \
   int** snks;

struct Sync { __Sync };

#ifndef ZERO
#define ZERO 0
#endif

#define SYNC_DEFARGS                            \
   STG_CLASS_DEFARGS

#define SYNC_PASSARGS                           \
   STG_CLASS_PASSARGS

Sync* Sync_New();

Sync* _Sync_New( SYNC_DEFARGS );

void Sync_Init( Sync* self );

void _Sync_Init( void* self );

void Sync_Destruct( Sync* self );

void _Sync_Delete( void* self );

void Sync_Copy( void* self, const void* op );

void Sync_SetDecomp( void* _self, const Decomp* decomp );

void Sync_FindRemotes( void* _self, int nRemotes, const int* remotes );

void Sync_SetComm( void* _self, const Comm* comm );

void Sync_SetRemotes( void* _self, int nRemotes, const int* remotes );

void Sync_AddRemotes( void* _self, int nRemotes, const int* remotes );

void Sync_RemoveRemotes( void* _self, int nRemotes, const int* remotes, IMap* map );

void Sync_Clear( void* _self );

void Sync_ClearDecomp( void* _self );

void Sync_ClearComm( void* _self );

void Sync_ClearRemotes( void* _self );

const Decomp* Sync_GetDecomp( const void* self );

const Comm* Sync_GetComm( const void* self );

int Sync_GetNumRemotes( const void* self );

int Sync_GetNumDomains( const void* self );

int Sync_GetNumShared( const void* self );

int Sync_GetNumSharers( const void* self, int shared );

void Sync_GetRemotes( const void* self, int* nRemotes, const int** remotes );

int Sync_GetOwner( const void* self, int remote );

void Sync_GetShared( const void* self, int* nShared, const int** shared );

void Sync_GetSharers( const void* self, int shared, int* nSharers, const int** sharers );

int Sync_RemoteToGlobal( const void* self, int remote );

int Sync_GlobalToRemote( const void* self, int global );

Bool Sync_TryGlobalToRemote( const void* self, int global, int* remote );

int Sync_DomainToGlobal( const void* self, int domain );

int Sync_GlobalToDomain( const void* self, int global );

Bool Sync_TryGlobalToDomain( const void* self, int global, int* domain );

int Sync_SharedToLocal( const void* self, int shared );

int Sync_LocalToShared( const void* self, int local );

Bool Sync_TryLocalToShared( const void* self, int local, int* shared );

void Sync_SyncArray( const void* _self, 
		     const void* local, size_t localStride, 
		     const void* remote, size_t remoteStride, 
		     size_t itmSize );

#endif /* __StgDomain_Mesh_Sync_h__ */
