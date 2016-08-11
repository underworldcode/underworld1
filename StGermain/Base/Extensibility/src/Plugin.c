/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
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

#include "types.h"
#include "shortcuts.h"
#include "Module.h"
#include "Plugin.h"

#include <string.h>


const Type Plugin_Type = "Plugin";

static const char* PLUGIN_REGISTER_SUFFIX = "_Register";
#ifdef MEMORY_STATS
	static const char* PLUGIN_MANGLEDNAME = "mangledName";
#endif


Plugin* Plugin_New( Name name, Stg_ObjectList* directories ) {
	/* Variables set in this function */
	SizeT                       _sizeOfSelf = sizeof(Plugin);
	Type                               type = Plugin_Type;
	Stg_Class_DeleteFunction*       _delete = _Plugin_Delete;
	Stg_Class_PrintFunction*         _print = _Plugin_Print;
	Stg_Class_CopyFunction*           _copy = NULL;
	Module_MangleNameFunction*   MangleName = _Plugin_MangleName;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return _Plugin_New(  PLUGIN_PASSARGS  );
}

Module* Plugin_Factory( Name name, Stg_ObjectList* directories ) {
	return (Module*)Plugin_New( name, directories );
}
	
Plugin* _Plugin_New(  PLUGIN_DEFARGS  )
{
	Plugin* self;

	assert( _sizeOfSelf >= sizeof(Plugin) );

	self = (Plugin*)_Module_New(  MODULE_PASSARGS  );
	
	_Plugin_Init( self );

	return self;
}
	
void _Plugin_Init( Plugin* self ) {
	Stream* debug = Journal_Register( Debug_Type, self->type );

	if( self->dllPtr != NULL ) {
		self->Register = (Plugin_RegisterFunction*)Module_LoadSymbol( self, PLUGIN_REGISTER_SUFFIX );
		
		/* If the register function is not found, then unload the module... it's not a plugin. */
		if( self->Register == NULL ) {
			Journal_Printf( debug, "Plugin %s is not a plugin, unloading.\n", self->name );
			Module_UnLoad( self );
		}
	}
}
	
void _Plugin_Delete( void* plugin ) {
	Plugin* self = (Plugin*)plugin;

	/* Delete parent */
	_Module_Delete( self );
}
	
void _Plugin_Print( void* plugin, Stream* stream ) {
	Plugin* self = (Plugin*)plugin;

	Journal_Printf( stream, "Plugin: %s\n", self->name );
	Stream_Indent( stream );
	
	/* Print parent */
	_Module_Print( self, stream );
	
	Stream_UnIndent( stream );
}

char* _Plugin_MangleName( char* name ) {
	char* mangledName = Memory_Alloc_Array( char, strlen( name ) + 1, PLUGIN_MANGLEDNAME );
	sprintf( mangledName, "%s", name );
	return mangledName;
}


Plugin_RegisterFunction* Plugin_GetRegisterFunc( void* plugin ) {
	Plugin* self = (Plugin*)plugin;

	return self->Register;
}


