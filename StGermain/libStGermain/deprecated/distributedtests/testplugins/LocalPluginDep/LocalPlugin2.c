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
#include "Base/Foundation/Foundation.h"
#include "Base/IO/IO.h"
#include "Base/Container/Container.h"
#include "Base/Automation/Automation.h"
#include "Base/Extensibility/Extensibility.h"

#include "../../MockContext.h"

const Type LocalPlugin2_Type = "LocalPlugin2";
typedef struct { __Codelet } LocalPlugin2;

void LocalPlugin2_Function() {
	printf( "%s\n", __func__ );
}

void _LocalPlugin2_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
	MockContext* context = (MockContext*)Stg_ComponentFactory_ConstructByName(cf, "context", Stg_Component, True, data );
	int* ext;

	ExtensionManager_Add( context->em, "LocalPlugin2", sizeof(int) );

	ext = ExtensionManager_Get(
		context->em, 
		context,
		ExtensionManager_GetHandle( context->em, "LocalPlugin2" ) );
	*ext = 2;
	
	EP_Append( context->ep, LocalPlugin2_Function );
}

void* _LocalPlugin2_DefaultNew( Name name ) {
	return Codelet_New(
		LocalPlugin2_Type,
		_LocalPlugin2_DefaultNew,
		_LocalPlugin2_AssignFromXML,
		_Codelet_Build,
		_Codelet_Initialise,
		_Codelet_Execute,
		_Codelet_Destroy,
		name );
}

Index LocalPlugin2_Register( PluginsManager* pluginsManager ) {
	return ModulesManager_Submit( pluginsManager, LocalPlugin2_Type, "0", _LocalPlugin2_DefaultNew );
}


