/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 
** 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**      Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**      Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**      Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**      Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**      Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**      Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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
 **     Abstract class faciliting how class inheritance is done.
 **
 ** <b>Assumptions:</b>
 **     None
 **
 ** <b>Comments:</b>
 **     None
 **
 ** $Id: IArray.h 3904 2006-12-14 00:52:06Z LukeHodkinson $
 **
 **/
 
#ifndef __StGermain_Base_Container_IArray_h__
#define __StGermain_Base_Container_IArray_h__

extern const Type IArray_Type;
        
#define __IArray                                \
    __Stg_Class                                 \
    int delta;                                  \
    int maxSize;                                \
    int size;                                   \
    int* ptr;                                   \
    Bool own;

struct IArray { __IArray };

#ifndef ZERO
#define ZERO 0
#endif

#define IARRAY_DEFARGS                          \
    STG_CLASS_DEFARGS

#define IARRAY_PASSARGS                         \
    STG_CLASS_PASSARGS

IArray* IArray_New();

IArray* _IArray_New( IARRAY_DEFARGS );

void IArray_Init( IArray* self );

void _IArray_Init( void* self );

void IArray_Destruct( IArray* self );

void _IArray_Delete( void* self );

void IArray_Copy( void* self, const void* op );

void IArray_SetDelta(void* self, int delta);

void IArray_Resize(void* self, int size);

void IArray_SoftResize(void* self, int size);

void IArray_SetProxy(void* self, int size, int* ptr);

void IArray_Set(void* self, int nItms, const int* itms);

void IArray_Add(void* self, int nItms, const int* itms);

void IArray_Remove(void* self, int nItms, const int* locals, IMap* map);

void IArray_Append(void* self, int itm);

void IArray_Push(void* self, int itm);

void IArray_Clear(void* self);

int IArray_GetSize(const void* self);

int* IArray_GetPtr(const void* self);

void IArray_GetArray(const void* self, int* size, const int** ptr);

#endif /* __StGermain_Base_Container_IArray_h__ */
