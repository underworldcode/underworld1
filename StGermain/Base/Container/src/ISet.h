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
 ** $Id: ISet.h 3904 2006-12-14 00:52:06Z LukeHodkinson $
 **
 **/
 
#ifndef __StGermain_Base_Container_ISet_h__
#define __StGermain_Base_Container_ISet_h__

extern const double ISet_TableFactor;

extern const Type ISet_Type;
        
#define __ISet                                  \
   __Stg_Class                                  \
   int maxSize;                                 \
   int curSize;                                 \
   int tblSize;                                 \
   ISetItem* tbl;                               \
   Bool* used;

struct ISet { __ISet };

#ifndef ZERO
#define ZERO 0
#endif

#define ISET_DEFARGS                            \
   STG_CLASS_DEFARGS

#define ISET_PASSARGS                           \
   STG_CLASS_PASSARGS

ISet* ISet_New();

ISet* _ISet_New( ISET_DEFARGS );

void ISet_Init( ISet* self );

void _ISet_Init( void* self );

void ISet_Destruct( ISet* self );

void _ISet_Delete( void* self );

void ISet_Copy( void* self, const void* op );

void ISet_UseArray( void* _self, int size, const int* array );

void ISet_SetMaxSize( void* _self, int maxSize );

void ISet_Insert( void* _self, int key );

Bool ISet_TryInsert( void* _self, int key );

void ISet_Remove( void* _self, int key );

Bool ISet_TryRemove( void* _self, int key );

void ISet_Clear( void* _self );

void ISet_Union( void* _self, const void* _op );

void ISet_Isect( void* _self, const void* _op );

void ISet_Subtr( void* _self, const void* _op );

int ISet_GetMaxSize( const void* self );

int ISet_GetSize( const void* self );

void ISet_GetArray( const void* _self, int* keys );

Bool ISet_Has( const void* _self, int key );

int ISet_Hash( const void* self, int key );

void ISet_First( const void* _self, ISetIter* iter );

#endif /* __StGermain_Base_Container_ISet_h__ */
