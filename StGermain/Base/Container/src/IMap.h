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
 ** $Id: IMap.h 3904 2006-12-14 00:52:06Z LukeHodkinson $
 **
 **/
 
#ifndef __StGermain_Base_Container_IMap_h__
#define __StGermain_Base_Container_IMap_h__

extern const double IMap_TableFactor;

extern const Type IMap_Type;
        
#define __IMap                                  \
    __Stg_Class                                 \
    int maxSize;                                \
    int curSize;                                \
    int tblSize;                                \
    IMapItem* tbl;                              \
    Bool* used;

struct IMap { __IMap };

#ifndef ZERO
#define ZERO 0
#endif

#define IMAP_DEFARGS                            \
    STG_CLASS_DEFARGS

#define IMAP_PASSARGS                           \
    STG_CLASS_PASSARGS

IMap* IMap_New();

IMap* _IMap_New( IMAP_DEFARGS );

void IMap_Init( IMap* self );

void _IMap_Init( void* self );

void IMap_Destruct( IMap* self );

void _IMap_Delete( void* self );

void IMap_Copy( void* self, const void* op );

void IMap_SetMaxSize( void* _self, int maxSize );

void IMap_Insert( void* _self, int key, int val );

void IMap_SetValue( void* _self, int key, int val );

void IMap_Remove( void* _self, int key );

void IMap_Clear( void* _self );

int IMap_GetSize( const void* self );

int IMap_GetMaxSize( const void* self );

int IMap_Map( const void* _self, int key );

Bool IMap_TryMap( const void* _self, int key, int* val );

Bool IMap_Has( const void* _self, int key );

int IMap_Hash( const void* self, int key );

void IMap_First( const void* _self, IMapIter* iter );

#endif /* __StGermain_Base_Container_IMap_h__ */
