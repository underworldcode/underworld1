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
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdarg.h>
#include "Base/Foundation/Foundation.h"
#include "Base/IO/IO.h"
#include "Base/Container/Container.h"
#include "Base/Automation/Automation.h"

#include "types.h"
#include "shortcuts.h"
#include "Hook.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


/* Textual name of this class */
const Type Hook_Type = "Hook";

/** allocate and initialise a new Hook. */
Hook* Hook_New( Name name, Func_Ptr funcPtr, char* addedBy ) {
   /* Variables set in this function */
   SizeT                     _sizeOfSelf = sizeof(Hook);
   Type                             type = Hook_Type;
   Stg_Class_DeleteFunction*     _delete = _Hook_Delete;
   Stg_Class_PrintFunction*       _print = _Hook_Print;
   Stg_Class_CopyFunction*         _copy = _Hook_Copy;

   /* 
    * Variables that are set to ZERO are variables that will be set either
    * by the current _New function or another parent _New function further up the hierachy.
    */

   /* default value NON_GLOBAL */
   AllocationType  nameAllocationType = NON_GLOBAL;

   return _Hook_New( HOOK_PASSARGS );
}

void Hook_Init( void* hook, Name name, Func_Ptr funcPtr, char* addedBy ) {
   Hook* self = (Hook*)hook;

   /* General info */
   self->type = Hook_Type;
   self->_sizeOfSelf = sizeof(Hook);
   self->_deleteSelf = False;
   
   /* Virtual info */
   self->_delete = _Hook_Delete;
   self->_print = _Hook_Print;
   self->_copy = _Hook_Copy;
   _Stg_Class_Init( (Stg_Class*)self );
   _Stg_Object_Init( (Stg_Object*)self, name, NON_GLOBAL );
   
   /* Hook info */
   _Hook_Init( self, funcPtr, addedBy );
}

Hook* _Hook_New( HOOK_DEFARGS ) {
   Hook* self;
   
   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(Hook) );

   /* 
    * The following terms are parameters that have been passed into this function 
    * but are being set before being passed onto the parent.
    * This means that any values of these parameters that are passed into this function
    * are not passed onto the parent function and so should be set to ZERO in any children of this class. 
    */
   nameAllocationType = NON_GLOBAL;

   self = (Hook*)_Stg_Object_New( STG_OBJECT_PASSARGS );
   
   /* General info */
   
   /* Virtual info */
   
   /* Hook info */
   _Hook_Init( self, funcPtr, addedBy );
   
   return self;
}

void _Hook_Init( Hook* self, Func_Ptr funcPtr, char* addedBy ) {
   /* General and Virtual info should already be set */
   
   /* Hook info */
   self->funcPtr = funcPtr;
   self->addedBy = StG_Strdup( addedBy );
}

void _Hook_Delete( void* hook ) {
   Hook* self = (Hook*)hook;
   #if DEBUG
      assert( self );
   #endif   
   
   Memory_Free( self->addedBy );
   /* Stg_Class_Delete parent */
   _Stg_Object_Delete( self );
}

void _Hook_Print( void* hook, Stream* stream ) {
   Hook* self = (Hook*)hook;
   #if DEBUG
      assert( self );
      assert( stream );
   #endif   
   
   /* General info */
   Journal_Printf( (void*) stream, "Hook (ptr) - %p:\n", self );
   _Stg_Object_Print( self, stream );
   Journal_Printf( (void*) stream, "\tfuncPtr (func ptr): %p\n", self->funcPtr );
   Journal_Printf( (void*) stream, "\taddedBy: %s\n", self->addedBy );
   
}

void* _Hook_Copy( void* hook, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
   Hook* self = (Hook*)hook;
   Hook* newHook;
   
   newHook = (Hook*)_Stg_Object_Copy( self, dest, deep, nameExt, ptrMap );
   
   newHook->funcPtr = self->funcPtr;
   
   if( self->addedBy ) {
      if( nameExt ) {
         Name tmpName;
         
         tmpName = Memory_Alloc_Array( char, strlen( self->addedBy ) + strlen( nameExt ) + 1, "newHook->addedBy" );
         strcpy( tmpName, self->addedBy );
         strcpy( &tmpName[strlen( self->addedBy )], nameExt );
         newHook->addedBy = tmpName;
      }
      else {
         newHook->addedBy = StG_Strdup( self->addedBy );
      }
   }
   else {
      newHook->addedBy = NULL;
   }
   
   return (void*)newHook;
}


