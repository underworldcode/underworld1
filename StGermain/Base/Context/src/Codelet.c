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
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdarg.h>
#include "Base/Foundation/Foundation.h"
#include "Base/IO/IO.h"
#include "Base/Container/Container.h"
#include "Base/Automation/Automation.h"
#include "Base/Extensibility/Extensibility.h"

#include "types.h"
#include "shortcuts.h"
#include "AbstractContext.h"
#include "Codelet.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/*  
 * The following terms are parameters that have been passed into this function 
 * but are being set before being passed onto the parent.
 * This means that any values of these parameters that are passed into this f
 * function are not passed onto the parent function and so should be set to 
 * ZERO in any children of this class. 
 */
AllocationType nameAllocationType = NON_GLOBAL;

const Type Codelet_Type = "Codelet";

void* Codelet_New(
   Type                                      type,
   Stg_Component_DefaultConstructorFunction* _defaultConstructor,
   Stg_Component_ConstructFunction*          _construct,
   Stg_Component_BuildFunction*              _build,
   Stg_Component_InitialiseFunction*         _initialise,
   Stg_Component_ExecuteFunction*            _execute,
   Stg_Component_DestroyFunction*            _destroy,
   Name                                      name )
{
   /* Variables set in this function */
   SizeT                      _sizeOfSelf = sizeof( Codelet );
   Stg_Class_DeleteFunction*      _delete = _Codelet_Delete;
   Stg_Class_PrintFunction*        _print = _Codelet_Print;
   Stg_Class_CopyFunction*          _copy = _Codelet_Copy;

   /* 
    * Variables that are set to ZERO are variables that will be set either by the 
    * current _New function or another parent _New function further up the hierachy.
    */

   /* default value NON_GLOBAL */
   AllocationType nameAllocationType = NON_GLOBAL;

   return _Codelet_New( CODELET_PASSARGS );
}

void* _Codelet_New( CODELET_DEFARGS ) {
   /* 
    * The following terms are parameters that have been passed into this function
    * but are being set before being passed onto the parent. 
    * This means that any values of these parameters that are passed into this 
    * function are not passed onto the parent function and so should be set to 
    * ZERO in any children of this class. 
    */
   nameAllocationType = NON_GLOBAL;

   return _Stg_Component_New( STG_COMPONENT_PASSARGS );
}

void  _Codelet_Delete( void* codelet ) {
   _Stg_Component_Delete( codelet );
}

void  _Codelet_Print( void* codelet, Stream* stream ) {
   Codelet* self = (Codelet*)codelet;
   
   Journal_Printf( stream, "Codelet: %s, Type %s\n", self->name, self->type );
   _Stg_Component_Print( self, stream );
}

void* _Codelet_Copy( void* codelet, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
   return _Stg_Component_Copy( codelet, dest, deep, nameExt, ptrMap );
}

void  _Codelet_Build( void* codelet, void* data ) {

}
void  _Codelet_Initialise( void* codelet, void* data ) {

}
void  _Codelet_Execute( void* codelet, void* data ) {

}
void  _Codelet_Destroy( void* codelet, void* data ) {

}

Dictionary* Codelet_GetPluginDictionary( void* codelet, Dictionary* rootDict ) {
   Codelet*                self = (Codelet*)codelet;
   Dictionary_Entry_Value* pluginsDEV = Dictionary_Get( rootDict, "plugins" );
   Dictionary*             pluginDict;
   unsigned                pluginIndex;
   Name                    pluginType;
   
   for( pluginIndex = 0; pluginIndex < Dictionary_Entry_Value_GetCount( pluginsDEV ); pluginIndex++ ) {
      pluginDict = Dictionary_Entry_Value_AsDictionary(
         Dictionary_Entry_Value_GetElement( pluginsDEV, pluginIndex ) );

      pluginType = Dictionary_GetString( pluginDict, "Type" );

      if( !strcmp( self->type, pluginType ) )
         return pluginDict;   
   }

   return NULL;
}


