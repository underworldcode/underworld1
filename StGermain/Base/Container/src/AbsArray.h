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
 ** $Id: AbsArray.h 3904 2006-12-14 00:52:06Z LukeHodkinson $
 **
 **/
 
#ifndef __StGermain_Base_Container_AbsArray_h__
#define __StGermain_Base_Container_AbsArray_h__

extern const Type AbsArray_Type;
        
#define __AbsArray                              \
    __Stg_Class                                 \
    int itmSize;                                \
    int delta;                                  \
    int maxSize;                                \
    int size;                                   \
    void* ptr;                                  \
    Bool own;

struct AbsArray { __AbsArray };

#ifndef ZERO
#define ZERO 0
#endif

#define ABSARRAY_DEFARGS                        \
    STG_CLASS_DEFARGS

#define ABSARRAY_PASSARGS                       \
    STG_CLASS_PASSARGS

AbsArray* AbsArray_New();

AbsArray* _AbsArray_New( ABSARRAY_DEFARGS );

void AbsArray_Init( AbsArray* self );

void _AbsArray_Init( void* self );

void AbsArray_Destruct( AbsArray* self );

void _AbsArray_Delete( void* self );

void AbsArray_Copy( void* self, const void* op );

void AbsArray_SetItemSize( void* _self, int itmSize );

void AbsArray_SetDelta( void* _self, int delta );

void AbsArray_Resize( void* _self, int size );

void AbsArray_SoftResize( void* _self, int size );

void AbsArray_Offset( void* _self, int delta );

void AbsArray_Push( void* _self, void* itm );

void AbsArray_Set( void* _self, void* itm );

void AbsArray_Clear( void* _self );

int AbsArray_GetSize( const void* _self );

const int* AbsArray_GetPtr( const void* _self );

#endif /* __StGermain_Base_Container_AbsArray_h__ */
