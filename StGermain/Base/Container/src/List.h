/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
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
**
*/
/** \file
**  Role:
**
** Assumptions:
**
** Invariants:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StGermain_Base_Container_List_h__
#define __StGermain_Base_Container_List_h__

   /** Textual name of this class */
   extern const Type List_Type;

   /** Virtual function types */

   /** Class contents */
   #define __List \
      /* General info */ \
      __Stg_Class \
      \
      /* Virtual info */ \
      \
      /* List info */ \
      unsigned  nItems; \
      Stg_Byte* items; \
      unsigned  itemSize; \
      unsigned  maxItems; \
      unsigned  delta;

   struct List { __List };

   /*--------------------------------------------------------------------------------------------------------------------------
   ** Constructors
   */
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define LIST_DEFARGS \
      STG_CLASS_DEFARGS

   #define LIST_PASSARGS \
      STG_CLASS_PASSARGS

   List* List_New();
   List* _List_New( LIST_DEFARGS );
   void _List_Init( List* self );

   /*--------------------------------------------------------------------------------------------------------------------------
   ** Virtual functions
   */

   void _List_Delete( void* list );
   void _List_Print( void* list, Stream* stream );

   /*--------------------------------------------------------------------------------------------------------------------------
   ** Public functions
   */

   void List_SetDelta( void* list, unsigned delta );
   void List_SetItemSize( void* list, unsigned itemSize );
   void List_Clear( void* list );

   void List_Insert( void* list, unsigned index, void* data );
   void List_Append( void* list, void* data );
   void List_Prepend( void* list, void* data );
   void List_Remove( void* list, void* data );

   void* List_GetItem( void* list, unsigned index );
   unsigned List_GetSize( void* list );
   Bool List_Exists( void* list, void* data );

   #define List_Get( list, index, type )      \
      ((type*)List_GetItem( list, index ))

   /*--------------------------------------------------------------------------------------------------------------------------
   ** Private Member functions
   */

   void List_Expand( List* self );
   void List_Contract( List* self );
   void List_Destruct( List* self );

#endif /* __StGermain_Base_Container_List_h__ */

