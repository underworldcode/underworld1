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
 ** $Id: IMapIter.h 3904 2006-12-14 00:52:06Z LukeHodkinson $
 **
 **/
 
#ifndef __StGermain_Base_Container_IMapIter_h__
#define __StGermain_Base_Container_IMapIter_h__

extern const Type IMapIter_Type;
        
#define __IMapIter                              \
    __Iter                                      \
    IMap* imap;                                 \
    int tblInd;                                 \
    IMapItem* cur;

struct IMapIter { __IMapIter };

#ifndef ZERO
#define ZERO 0
#endif

#define IMAPITER_DEFARGS                        \
    ITER_DEFARGS

#define IMAPITER_PASSARGS                       \
    ITER_PASSARGS

IMapIter* IMapIter_New();

IMapIter* _IMapIter_New( IMAPITER_DEFARGS );

void IMapIter_Init( IMapIter* self );

void _IMapIter_Init( void* self );

void IMapIter_Destruct( IMapIter* self );

void _IMapIter_Delete( void* self );

void IMapIter_Copy( void* self, const void* op );

void IMapIter_Next( void* self );

int IMapIter_GetKey( const void* self );

int IMapIter_GetValue( const void* self );

#endif /* __StGermain_Base_Container_IMapIter_h__ */
