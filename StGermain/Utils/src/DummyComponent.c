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

#include <mpi.h>
#include <StGermain/StGermain.h>

#include "types.h"
#include "DummyComponent.h"

const Type DummyComponent_Type = "DummyComponent";

void* _DummyComponent_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                _sizeOfSelf = sizeof(DummyComponent);
	Type                                                        type = DummyComponent_Type;
	Stg_Class_DeleteFunction*                                _delete = _DummyComponent_Delete;
	Stg_Class_PrintFunction*                                  _print = _DummyComponent_Print;
	Stg_Class_CopyFunction*                                    _copy = _DummyComponent_Copy;
	Stg_Component_DefaultConstructorFunction*    _defaultConstructor = _DummyComponent_DefaultNew;
	Stg_Component_ConstructFunction*                      _construct = _DummyComponent_AssignFromXML;
	Stg_Component_BuildFunction*                              _build = _DummyComponent_Build;
	Stg_Component_InitialiseFunction*                    _initialise = _DummyComponent_Initialise;
	Stg_Component_ExecuteFunction*                          _execute = _DummyComponent_Execute;
	Stg_Component_DestroyFunction*                          _destroy = _DummyComponent_Destroy;

	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*)_DummyComponent_New(  DUMMYCOMPONENT_PASSARGS  );
}

DummyComponent* _DummyComponent_New(  DUMMYCOMPONENT_DEFARGS  ) {
	DummyComponent* self;
	
	/* Allocate memory */
	self = (DummyComponent*)_Stg_Component_New(  STG_COMPONENT_PASSARGS  );
	
	/* Virtual info */
	
	/* DummyComponent info */
	
	return self;
}

void _DummyComponent_Delete( void* dummyComponent ) {
	DummyComponent* self = (DummyComponent*)dummyComponent;
	
	/* Stg_Class_Delete parent class */
	_Stg_Component_Delete( self );
}

void _DummyComponent_Print( void* dummyComponent, Stream* stream ){}

void* _DummyComponent_Copy( void* dummyComponent, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ){}

void _DummyComponent_AssignFromXML( void* dummyComponent, Stg_ComponentFactory* cf, void* data ){}
	
void _DummyComponent_Build( void* dummyComponent, void* data ){}
	
void _DummyComponent_Initialise( void* dummyComponent, void* data ){}
	
void _DummyComponent_Execute( void* dummyComponent, void* data ){}
	
void _DummyComponent_Destroy( void* dummyComponent, void* data ){}



