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
 ** $Id: Comm.h 3904 2006-12-14 00:52:06Z LukeHodkinson $
 **
 **/
 
#ifndef __StGermain_Base_Automation_Comm_h__
#define __StGermain_Base_Automation_Comm_h__

extern const Type Comm_Type;
        
#define __Comm                                  \
   __Stg_Class                                  \
   MPI_Comm mpiComm;                            \
   IArray nbrs;                                 \
   IMap inv;                                    \
   MPI_Request* recvs;                          \
   MPI_Request* sends;                          \
   MPI_Status* stats;                           \
   int* recvSizes;                              \
   int itmSize;                                 \
   int srcSize;                                 \
   int* srcSizes;

struct Comm { __Comm };

#ifndef ZERO
#define ZERO 0
#endif

#define COMM_DEFARGS                            \
   STG_CLASS_DEFARGS

#define COMM_PASSARGS                           \
   STG_CLASS_PASSARGS

Comm* Comm_New();

Comm* _Comm_New( COMM_DEFARGS );

void Comm_Init( Comm* self );

void _Comm_Init( void* self );

void Comm_Destruct( Comm* self );

void _Comm_Delete( void* self );

void Comm_Copy( void* self, const void* op );

void Comm_SetMPIComm( void* _self, MPI_Comm mpiComm );

void Comm_SetNeighbours( void* _self, int nNbrs, const int* nbrs );

void Comm_AddNeighbours( void* _self, int nNbrs, const int* nbrs );

void Comm_RemoveNeighbours( void* _self, int nNbrs, const int* nbrLocalIndices, IMap* map );

MPI_Comm Comm_GetMPIComm( const void* self );

int Comm_GetNumNeighbours( const void* self );

void Comm_GetNeighbours( const void* self, int* nNbrs, const int** nbrs );

int Comm_RankLocalToGlobal( const void* self, int local );

Bool Comm_RankGlobalToLocal( const void* self, int global, int* local );

void Comm_AllgatherInit( const void* _self, int srcSize, 
			 int* dstSizes, int itmSize );

void Comm_AllgatherBegin( const void* _self, const void* srcArray, 
			  void** dstArrays );

void Comm_AllgatherEnd( const void* _self );

void Comm_AlltoallInit( const void* _self, const int* srcSizes, 
			int* dstSizes, int itmSize );

void Comm_AlltoallBegin( const void* _self, const void** srcArrays, 
			 void** dstArrays );

void Comm_AlltoallEnd( const void* _self );

#endif /* __StGermain_Base_Automation_Comm_h__ */
