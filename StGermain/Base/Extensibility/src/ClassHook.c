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
#include "Hook.h"
#include "ClassHook.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


/* Textual name of this class */
const Type ClassHook_Type = "ClassHook";

/** allocate and initialise a new ClassHook. */
ClassHook* ClassHook_New( Name name, Func_Ptr funcPtr, char* addedBy, void* reference ) {
	/* Variables set in this function */
	SizeT                      _sizeOfSelf = sizeof(ClassHook);
	Type                              type = ClassHook_Type;
	Stg_Class_DeleteFunction*      _delete = _ClassHook_Delete;
	Stg_Class_PrintFunction*        _print = _ClassHook_Print;
	Stg_Class_CopyFunction*          _copy = _ClassHook_Copy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return _ClassHook_New(  CLASSHOOK_PASSARGS  );
}

void ClassHook_Init( void* hook, Name name, Func_Ptr funcPtr, char* addedBy, void* reference ) {
	ClassHook* self = (ClassHook*)hook;

	/* General info */
	self->type = ClassHook_Type;
	self->_sizeOfSelf = sizeof(ClassHook);
	self->_deleteSelf = False;
	
	/* Virtual info */
	self->_delete = _ClassHook_Delete;
	self->_print = _ClassHook_Print;
	self->_copy = _ClassHook_Copy;
	_Stg_Class_Init( (Stg_Class*)self );
	_Stg_Object_Init( (Stg_Object*)self, name, NON_GLOBAL );
	
	_ClassHook_Init( self, funcPtr, addedBy, reference );
}


ClassHook* _ClassHook_New(  CLASSHOOK_DEFARGS  )
{
	ClassHook* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(ClassHook) );
	/* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
	/* This means that any values of these parameters that are passed into this function are not passed onto the parent function
	   and so should be set to ZERO in any children of this class. */
	nameAllocationType = NON_GLOBAL;

	self = (ClassHook*)_Stg_Object_New(  STG_OBJECT_PASSARGS  );
	
	/* General info */
	
	/* Virtual info */
	
	/* ClassHook info */
	_ClassHook_Init( self, funcPtr, addedBy, reference );
	
	return self;
}


void _ClassHook_Init( ClassHook* self, Func_Ptr funcPtr, char* addedBy, void* reference ) {
	/* General and Virtual info should already be set */
	
	/* ClassHook info */
	_Hook_Init( (Hook*)self, funcPtr, addedBy );

	self->reference = reference;
}


void _ClassHook_Delete( void* hook ) {
	ClassHook* self = (ClassHook*)hook;
	#if DEBUG
		assert( self );
	#endif	
	
	_Hook_Delete( self );
}

void _ClassHook_Print( void* hook, Stream* stream ) {
	ClassHook* self = (ClassHook*)hook;
	#if DEBUG
		assert( self );
		assert( stream );
	#endif	
	
	/* General info */
	_Hook_Print( self, stream );
	Journal_Printf( (void*) stream, "\treference(ptr): %p\n", self->reference );
	
}


void* _ClassHook_Copy( void* hook, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
	ClassHook*	self = (ClassHook*)hook;
	ClassHook*	newClassHook;
	
	newClassHook = (ClassHook*)_Hook_Copy( self, dest, deep, nameExt, ptrMap );

	newClassHook->reference = Stg_Class_Copy( self->reference, dest, deep, nameExt, ptrMap );
	
	return (void*)newClassHook;
}



