/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 
** 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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
 **   Abstract class faciliting how class inheritance is done.
 **
 ** <b>Assumptions:</b>
 **   None
 **
 ** <b>Comments:</b>
 **   None
 **
 ** $Id: Iter.h 3904 2006-12-14 00:52:06Z LukeHodkinson $
 **
 **/
 
#ifndef __StGermain_Base_Container_Iter_h__
#define __StGermain_Base_Container_Iter_h__

   extern const Type Iter_Type;

   typedef void (Iter_NextFunc)( void* self );
        
   #define __Iter \
      __Stg_Class \
      Iter_NextFunc* nextFunc; \
      Bool           valid;

   struct Iter { __Iter };

   #ifndef ZERO
   #define ZERO 0
   #endif

   #define ITER_DEFARGS \
      STG_CLASS_DEFARGS, \
         Iter_NextFunc* nextFunc

   #define ITER_PASSARGS \
      STG_CLASS_PASSARGS, \
         nextFunc

   Iter* _Iter_New( ITER_DEFARGS );

   void _Iter_Init( void* self );

   void _Iter_Delete( void* self );

   void Iter_Copy( void* self, const void* op );

   void Iter_Next( void* self );

   Bool Iter_IsValid( void* self );

#endif /* __StGermain_Base_Container_Iter_h__ */
