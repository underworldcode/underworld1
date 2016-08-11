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
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdarg.h>
#include "Base/Foundation/Foundation.h"
#include "Base/IO/IO.h"
#include "Base/Container/Container.h"

#include "types.h"
#include "shortcuts.h"
#include "Stg_Component.h"
#include "LiveComponentRegister.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* Textual name of this class. */
const Type LiveComponentRegister_Type = "LiveComponentRegister";
LiveComponentRegister* stgLiveComponentRegister = NULL;

LiveComponentRegister* _LiveComponentRegister_New( LIVECOMPONENTREGISTER_DEFARGS ) {
   LiveComponentRegister *self = NULL;

   assert( _sizeOfSelf >= sizeof( LiveComponentRegister ) );
   self = (LiveComponentRegister*) _Stg_Class_New( STG_CLASS_PASSARGS );
   
   return self;
}
   
LiveComponentRegister* LiveComponentRegister_New() {
   /* Variables set in this function */
   SizeT                     _sizeOfSelf = sizeof( LiveComponentRegister );
   Type                             type = LiveComponentRegister_Type;
   Stg_Class_DeleteFunction*     _delete = _LiveComponentRegister_Delete;
   Stg_Class_PrintFunction*       _print = _LiveComponentRegister_Print;

   LiveComponentRegister*  self = NULL;
   Stg_Class_CopyFunction* _copy = NULL;

   /* 
    * The following terms are parameters that have been passed into or defined in this
    * function but are being set before being passed onto the parent.
    */
   self = _LiveComponentRegister_New( LIVECOMPONENTREGISTER_PASSARGS );
   LiveComponentRegister_Init( self );

   if( !stgLiveComponentRegister ) 
      stgLiveComponentRegister = self;

   return self;
}

void _LiveComponentRegister_Init( LiveComponentRegister *self ) { }
   
void LiveComponentRegister_Init( LiveComponentRegister *self ) {
   assert( self );

   self->componentList = Stg_ObjectList_New( );
}

void _LiveComponentRegister_Delete( void* liveComponentRegister ) {
   LiveComponentRegister *self = (LiveComponentRegister *) liveComponentRegister;

   assert( self );

   /* TODO: add some debug printing about this... */
   LiveComponentRegister_DeleteAll( self );
   Stg_Class_Delete( self->componentList );

   /* 
    * Note: this has to come after the LCRegister delete all, in case any of the
    * components need to refer to it as part of their delete phase.
    */
   if( stgLiveComponentRegister == self ) 
      stgLiveComponentRegister = NULL;

   _Stg_Class_Delete( self );
}

void _LiveComponentRegister_Print( void* liveComponentRegister, Stream* stream ) {
   LiveComponentRegister *self = (LiveComponentRegister *) liveComponentRegister;

   assert( self );
   
   /* General info */
   Journal_Printf( (void*)stream, "LiveComponentRegister (ptr): %p\n", self );
   
   /* Print parent class */
   _Stg_Class_Print( self, stream );
   
   Journal_Printf( (void*)stream, "componentList (ptr): %p\n", self->componentList );
   Stg_Class_Print( self->componentList, stream );
}

Index LiveComponentRegister_Add( LiveComponentRegister *self, Stg_Component *component ) {
   assert( self );
   // check if component is already there. If so don't append to objectList
   if( Stg_ObjectList_Get( self->componentList, component->name ) ) {
      Index id = Stg_ObjectList_GetIndex( self->componentList, component->name );
      // lets just silence this for now
      //Journal_Printf( global_error_stream, "### Trying to add component '%s'. But it already exist on the LiveComponentRegister at index %d\n", component->name, id );
      return id;
   }
   return Stg_ObjectList_Append( self->componentList, component );
}

Index LiveComponentRegister_IfRegThenAdd( Stg_Component *component ) {
   if( LiveComponentRegister_GetLiveComponentRegister() ) {
      LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), component );
      return 1;
   }
   else
      return 0;
}
   
Stg_Component *LiveComponentRegister_Get( LiveComponentRegister *self, Name name ) {
   assert( self );
   
   return ( Stg_Component* ) Stg_ObjectList_Get( self->componentList, name );
}

Stg_Component *LiveComponentRegister_At( void* liveComponentRegister, Index index ) {
   LiveComponentRegister* self = (LiveComponentRegister*)liveComponentRegister;
   assert( self );

   return ( Stg_Component* ) Stg_ObjectList_At( self->componentList, index );
}

Index LiveComponentRegister_RemoveOneComponentsEntry( void* liveComponentRegister, Name name ) {
   LiveComponentRegister* self = (LiveComponentRegister*)liveComponentRegister;
   assert( self );

   /* 
    * Note: as specified in the header, we don't want to actually delete the component, 
    * just the entry.
    */
   return Stg_ObjectList_Remove( self->componentList, name, KEEP );
}

unsigned int LiveComponentRegister_GetCount( LiveComponentRegister *self ) {
   assert( self );

   return self->componentList->count;
}

void LiveComponentRegister_BuildAll( void* liveComponentRegister, void* data ) {
   LiveComponentRegister* self = (LiveComponentRegister*)liveComponentRegister;
   Stg_Component*         component;
   Index                  index;
   Index                  count = LiveComponentRegister_GetCount( self );
   
   for( index = 0; index < count; index++ ) {
      component = LiveComponentRegister_At( self, index );
      Stg_Component_Build( component, data, False );
   }
}

void LiveComponentRegister_InitialiseAll( void* liveComponentRegister, void* data ) {
   LiveComponentRegister* self = (LiveComponentRegister*)liveComponentRegister;
   Stg_Component*         component;
   Index                  index;
   Index                  count = LiveComponentRegister_GetCount( self );
   
   for( index = 0; index < count; index++ ) {
      component = LiveComponentRegister_At( self, index );
      Stg_Component_Initialise( component, data, False );
   }
}

void LiveComponentRegister_DeleteAll( void* liveComponentRegister ) {
   LiveComponentRegister* self = (LiveComponentRegister*)liveComponentRegister;
   Stg_Component*         component;
   Index                  index;
   
   /* 
    * Note: have to recompute count dynamically each loop in case a component deletes
    * some other components.
    */
   for( index = 0; index < LiveComponentRegister_GetCount( self ); index++ ) {
      component = LiveComponentRegister_At( self, index );
      Stg_Class_Delete( component );
   }
   self->componentList->count = 0;
}

LiveComponentRegister* LiveComponentRegister_GetLiveComponentRegister() {
   return stgLiveComponentRegister;
}

void LiveComponentRegister_PrintSimilar( void* liveComponentRegister, Name name, Stream* stream, unsigned int number ) {
   LiveComponentRegister* self = (LiveComponentRegister*)liveComponentRegister;

   Stg_ObjectList_PrintSimilar( self->componentList, name, stream, number );
}

void LiveComponentRegister_DestroyAll( void* lcReg ) {
   LiveComponentRegister* self = (LiveComponentRegister*)lcReg;
   Stg_Component*         component;
   unsigned               index;
   
   /* 
    * Note, if a Component is already Destroyed the, isDestroyed flag should protect the
    * code from running the _Destroy function again.
    */
   for( index = 0; index < LiveComponentRegister_GetCount( self ); index++ ) {
      component = LiveComponentRegister_At( self, index );
      Stg_Component_Destroy( component, NULL, False );
   }
}

