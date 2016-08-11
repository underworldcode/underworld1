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
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdarg.h>
#include "Base/Foundation/Foundation.h"
#include "Base/IO/IO.h"
#include "Base/Container/Container.h"
#include "Base/Automation/Automation.h"

#include "types.h"
#include "shortcuts.h"
#include "ExtensionManager.h"
#include "ExtensionManager_Register.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

/* Textual name of this class */
const Type ExtensionManager_Register_Type = "ExtensionManager_Register";

ExtensionManager_Register* extensionMgr_Register = 0;

ExtensionManager_Register* ExtensionManager_Register_New( void ) {
	/* Variables set in this function */
	SizeT                      _sizeOfSelf = sizeof(ExtensionManager_Register);
	Type                              type = ExtensionManager_Register_Type;
	Stg_Class_DeleteFunction*      _delete = _ExtensionManager_Register_Delete;
	Stg_Class_PrintFunction*        _print = _ExtensionManager_Register_Print;
	Stg_Class_CopyFunction*          _copy = NULL;

	return _ExtensionManager_Register_New(  EXTENSIONMANAGER_REGISTER_PASSARGS  );
}

void ExtensionManager_Register_Init( void* extensionManager_Register ) {
	ExtensionManager_Register* self = (ExtensionManager_Register*)extensionManager_Register;
	
	/* General info */
	self->type = ExtensionManager_Register_Type;
	self->_sizeOfSelf = sizeof(ExtensionManager_Register);
	self->_deleteSelf = False;
	
	/* Virtual info */
	self->_delete = _ExtensionManager_Register_Delete;
	self->_print = _ExtensionManager_Register_Print;
	self->_copy = NULL;
	_Stg_Class_Init( (Stg_Class*)self );
	
	/* ExtensionManager_Register info */
	_ExtensionManager_Register_Init( self );
}

ExtensionManager_Register* _ExtensionManager_Register_New(  EXTENSIONMANAGER_REGISTER_DEFARGS  )
{
	ExtensionManager_Register* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(ExtensionManager_Register) );
	self = (ExtensionManager_Register*)_Stg_Class_New(  STG_CLASS_PASSARGS  );
	
	/* General info */
	
	/* Virtual info */
	
	_ExtensionManager_Register_Init( self );
	
	return self;
}

void _ExtensionManager_Register_Init( void* extensionManager_Register ) {
	ExtensionManager_Register* self = (ExtensionManager_Register*)extensionManager_Register;
	
	/* General and Virtual info should already be set */
	
	/* ExtensionManager_Register info */
	self->extensions = Stg_ObjectList_New();
}

void _ExtensionManager_Register_Delete( void* extensionManager_Register ) {
	ExtensionManager_Register* self = (ExtensionManager_Register*)extensionManager_Register;
	
	/* Assumes ownerships of the element types */
	Stg_Class_Delete( self->extensions );
	
	/* Stg_Class_Delete parent */
	_Stg_Class_Delete( self );
}

void _ExtensionManager_Register_Print( void* extensionManager_Register, Stream* stream ) {
	ExtensionManager_Register* self = (ExtensionManager_Register*)extensionManager_Register;
	
	/* General info */
	Journal_Printf( (void*) stream, "ExtensionManager_Register (ptr): %p\n", self );
	
	/* Print parent */
	_Stg_Class_Print( self, stream );
	
	/* Virtual info */
	
	/* ExtensionManager_Register info */
	Journal_Printf( (void*)stream, "\tcount: %u\n", self->extensions->count );
	
	Journal_Printf( (void*)stream, "\textensions (ptr): %p\n", self->extensions );
	Stg_Class_Print( self->extensions, stream );
}

ExtensionManager_Index ExtensionManager_Register_Add( void* extensionManager_Register, void* extension ) {
	ExtensionManager_Register*	self = (ExtensionManager_Register*)extensionManager_Register;
	
	return Stg_ObjectList_Append( self->extensions, extension );
}

ExtensionManager_Index ExtensionManager_Register_Remove( void* extensionManager_Register, void* extension ) {
	ExtensionManager_Register*	self = (ExtensionManager_Register*)extensionManager_Register;
	
	return Stg_ObjectList_Remove( self->extensions, ((ExtensionManager*)extension)->name, KEEP );
}

ExtensionManager_Index ExtensionManager_Register_GetExtensionHandle( void* extensionManager_Register, Name toGet ) {
	ExtensionManager_Register*	self = (ExtensionManager_Register*)extensionManager_Register;
	
	return Stg_ObjectList_GetIndex( self->extensions, toGet );
}

ExtensionManager* _ExtensionManager_Register_GetExtension( void* extensionManager_Register, ExtensionManager_Index handle ) {
	ExtensionManager_Register*	self = (ExtensionManager_Register*)extensionManager_Register;
	
	return ExtensionManager_Register_GetExtension( self, handle );
}


