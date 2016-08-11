/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd,
** 110 Victoria Street, Melbourne, 3053, Australia.
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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StGermain_Utils_DummyComponent_h__
#define __StGermain_Utils_DummyComponent_h__

#define __DummyComponent				\
   __Stg_Component

	struct DummyComponent { __DummyComponent };

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define DUMMYCOMPONENT_DEFARGS \
                STG_COMPONENT_DEFARGS

	#define DUMMYCOMPONENT_PASSARGS \
                STG_COMPONENT_PASSARGS

extern const Type DummyComponent_Type;

DummyComponent* _DummyComponent_New(  DUMMYCOMPONENT_DEFARGS  );
void* _DummyComponent_DefaultNew( Name name );
void _DummyComponent_Delete( void* dummyComponent );
void _DummyComponent_Print( void* dummyComponent, Stream* stream );
void* _DummyComponent_Copy( void* dummyComponent, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
void _DummyComponent_AssignFromXML( void* dummyComponent, Stg_ComponentFactory* cf, void* data );
void _DummyComponent_Build( void* dummyComponent, void* data );
void _DummyComponent_Initialise( void* dummyComponent, void* data );
void _DummyComponent_Execute( void* dummyComponent, void* data );
void _DummyComponent_Destroy( void* dummyComponent, void* data );

#endif /* __StGermain_Utils_DummyCompnonent_h__ */

