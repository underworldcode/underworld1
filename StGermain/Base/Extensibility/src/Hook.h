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
*/
/** \file
**  Role:
**   Defines the information that goes with a function pointer.
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StGermain_Base_Extensibility_Hook_h__
#define __StGermain_Base_Extensibility_Hook_h__
   
   /** Textual name of this class */
   extern const Type Hook_Type;

   /** Hook struct- stores a func pointer plus info */
   #define __Hook \
      __Stg_Object \
      \
      Func_Ptr     funcPtr; \
      Hook_AddedBy addedBy; \
   
   struct Hook { __Hook };
   
   /* Create a new Hook */
   Hook* Hook_New( Name name, Func_Ptr funcPtr, char* addedBy );
   
   /* Initialise an Hook */
   void Hook_Init( void* hook, Name name, Func_Ptr funcPtr, char* addedBy );
   
   /* Creation implementation */
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define HOOK_DEFARGS \
      STG_OBJECT_DEFARGS, \
      Func_Ptr  funcPtr, \
      char*     addedBy

   #define HOOK_PASSARGS \
      STG_OBJECT_PASSARGS, \
      funcPtr, \
      addedBy

   Hook* _Hook_New( HOOK_DEFARGS );
   
   /* Initialisation implementation */
   void _Hook_Init( Hook* self, Func_Ptr funcPtr, char* addedBy );
   
   /* Stg_Class_Delete implementation */
   void _Hook_Delete( void* hook );
   
   /* Print implementation */
   void _Hook_Print( void* hook, Stream* stream );
   
   /* Copy */
   #define Hook_Copy( self ) \
      (Hook*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
   #define Hook_DeepCopy( self ) \
      (Hook*)Stg_Class_Copy( self, NULL, True, NULL, NULL )
   
   void* _Hook_Copy( void* hook, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
   
#endif /* __StGermain_Base_Extensibility_Hook_h__ */

