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
#include "Stg_ComponentFactory.h"
#include "OneOffComponentFactory.h"
#include "HierarchyTable.h"
#include "Stg_ComponentRegister.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define COMPONENT_TYPE_STRING_LENGTH 30

/* Textual name of this class */
const Type OneOffComponentFactory_Type = "OneOffComponentFactory";

/*--------------------------------------------------------------------------------------------------------------------------
** Constructor
*/
	
OneOffComponentFactory* OneOffComponentFactory_New( ) {
	/* Variables set in this function */
	SizeT                                                                        _sizeOfSelf = sizeof( OneOffComponentFactory );
	Type                                                                                type = OneOffComponentFactory_Type;
	Stg_Class_DeleteFunction*                                                        _delete = _OneOffComponentFactory_Delete;
	Stg_Class_PrintFunction*                                                          _print = _OneOffComponentFactory_Print;
	Stg_ComponentFactory_GetDoubleFunc*                                            getDouble = _OneOffComponentFactory_GetDouble;
	Stg_ComponentFactory_GetIntFunc*                                                  getInt = _OneOffComponentFactory_GetInt;
	Stg_ComponentFactory_GetUnsignedIntFunc*                                  getUnsignedInt = _OneOffComponentFactory_GetUnsignedInt;
	Stg_ComponentFactory_GetBoolFunc*                                                getBool = _OneOffComponentFactory_GetBool;
	Stg_ComponentFactory_GetStringFunc*                                            getString = _OneOffComponentFactory_GetString;
	Stg_ComponentFactory_GetRootDictDoubleFunc*                            getRootDictDouble = _OneOffComponentFactory_GetRootDictDouble;
	Stg_ComponentFactory_GetRootDictIntFunc*                                  getRootDictInt = _OneOffComponentFactory_GetRootDictInt;
	Stg_ComponentFactory_GetRootDictUnsignedIntFunc*                  getRootDictUnsignedInt = _OneOffComponentFactory_GetRootDictUnsignedInt;
	Stg_ComponentFactory_GetRootDictBoolFunc*                                getRootDictBool = _OneOffComponentFactory_GetRootDictBool;
	Stg_ComponentFactory_GetRootDictStringFunc*                            getRootDictString = _OneOffComponentFactory_GetRootDictString;
	Stg_ComponentFactory_ConstructByNameFunc*                                constructByName = _OneOffComponentFactory_ConstructByName;
	Stg_ComponentFactory_ConstructByKeyFunc*                                  constructByKey = _OneOffComponentFactory_ConstructByKey;
	Stg_ComponentFactory_ConstructByNameWithKeyFallbackFunc*  constructByNameWithKeyFallback = _OneOffComponentFactory_ConstructByNameWithKeyFallback;
	Stg_ComponentFactory_ConstructByListFunc*                                constructByList = _OneOffComponentFactory_ConstructByList;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	Dictionary*       rootDict = ZERO;
	Dictionary*  componentDict = ZERO;

	OneOffComponentFactory *self = NULL;

	/* The following terms are parameters that have been passed into or defined in this function but are being set before being passed onto the parent */
	Stg_Class_CopyFunction*  _copy = NULL;

	self = _OneOffComponentFactory_New(  ONEOFFCOMPONENTFACTORY_PASSARGS  );

	return self;
}

/* Creation implementation */
OneOffComponentFactory* _OneOffComponentFactory_New(  ONEOFFCOMPONENTFACTORY_DEFARGS  )
{
	OneOffComponentFactory *self = NULL;

	assert( _sizeOfSelf >= sizeof( OneOffComponentFactory ) );
	/* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
	/* This means that any values of these parameters that are passed into this function are not passed onto the parent function
	   and so should be set to ZERO in any children of this class. */
	rootDict      = NULL;
	componentDict = NULL;

	self = (OneOffComponentFactory*) _Stg_ComponentFactory_New(  STG_COMPONENTFACTORY_PASSARGS  );

	return self;
}

/*--------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _OneOffComponentFactory_Delete( void* componentFactory ) {
	OneOffComponentFactory *self = (OneOffComponentFactory *) componentFactory;

	_Stg_ComponentFactory_Delete( self );
}
	
void _OneOffComponentFactory_Print( void* componentFactory, Stream* stream ) {
	OneOffComponentFactory *self = (OneOffComponentFactory*) componentFactory;

	assert( self );
	
	/* General info */
	Journal_Printf( (void*) stream, "OneOffComponentFactory (ptr): %p\n", self );
	
	/* Print parent class */
	_Stg_ComponentFactory_Print( self, stream );
}

/*--------------------------------------------------------------------------------------------------------------------------
** Functions
*/
double _OneOffComponentFactory_GetDouble( void* cf, Name componentName, Dictionary_Entry_Key key, const double Default ) {
	OneOffComponentFactory* self = (OneOffComponentFactory*) cf;
	Journal_Printf( self->infoStream, "Parameter - Key '%s' - Type 'double' - ", key );
	Journal_PrintValue( self->infoStream, Default );
	return Default;
}

int _OneOffComponentFactory_GetInt( void* cf, Name componentName, Dictionary_Entry_Key key, const int Default ) {
	OneOffComponentFactory* self = (OneOffComponentFactory*) cf;
	Journal_Printf( self->infoStream, "Parameter - Key '%s' - Type 'int' - ", key );
	Journal_PrintValue( self->infoStream, Default );
	return Default;
}	

unsigned int _OneOffComponentFactory_GetUnsignedInt( void* cf, Name componentName, Dictionary_Entry_Key key, const unsigned int Default ) {
	OneOffComponentFactory* self = (OneOffComponentFactory*) cf;
	Journal_Printf( self->infoStream, "Parameter - Key '%s' - Type 'unsigned int' - ", key );
	Journal_PrintValue( self->infoStream, Default );
	return Default;
}	

Bool _OneOffComponentFactory_GetBool( void* cf, Name componentName, Dictionary_Entry_Key key, const Bool Default ) {
	OneOffComponentFactory* self = (OneOffComponentFactory*) cf;
	Journal_Printf( self->infoStream, "Parameter - Key '%s' - Type 'Bool' - ", key );
	Journal_PrintBool( self->infoStream, Default );
	return Default;
}	

char* _OneOffComponentFactory_GetString( void* cf, Name componentName, Dictionary_Entry_Key key, const char* Default ) {
	OneOffComponentFactory* self = (OneOffComponentFactory*) cf;
	Journal_Printf( self->infoStream, "Parameter - Key '%s' - Type 'string' - ", key );
	Journal_PrintString( self->infoStream, Default );
	return (char*)Default;
}

double _OneOffComponentFactory_GetRootDictDouble( void* cf, Dictionary_Entry_Key key, const double Default ) {
	OneOffComponentFactory* self = (OneOffComponentFactory*) cf;
	Journal_Printf( self->infoStream, "Root Dictionary Parameter - Key '%s' - Type 'double' - ", key );
	Journal_PrintValue( self->infoStream, Default );
	return Default;
}

int _OneOffComponentFactory_GetRootDictInt( void* cf, Dictionary_Entry_Key key, const int Default ) {
	OneOffComponentFactory* self = (OneOffComponentFactory*) cf;
	Journal_Printf( self->infoStream, "Root Dictionary Parameter - Key '%s' - Type 'int' - ", key );
	Journal_PrintValue( self->infoStream, Default );
	return Default;
}	

unsigned int _OneOffComponentFactory_GetRootDictUnsignedInt( void* cf, Dictionary_Entry_Key key, const unsigned int Default ) {
	OneOffComponentFactory* self = (OneOffComponentFactory*) cf;
	Journal_Printf( self->infoStream, "Root Dictionary Parameter - Key '%s' - Type 'unsigned int' - ", key );
	Journal_PrintValue( self->infoStream, Default );
	return Default;
}	

Bool _OneOffComponentFactory_GetRootDictBool( void* cf, Dictionary_Entry_Key key, const Bool Default ) {
	OneOffComponentFactory* self = (OneOffComponentFactory*) cf;
	Journal_Printf( self->infoStream, "Root Dictionary Parameter - Key '%s' - Type 'Bool' - ", key );
	Journal_PrintBool( self->infoStream, Default );
	return Default;
}	

char* _OneOffComponentFactory_GetRootDictString( void* cf, Dictionary_Entry_Key key, const char* const Default ) {
	OneOffComponentFactory* self = (OneOffComponentFactory*) cf;
	Journal_Printf( self->infoStream, "Root Dictionary Parameter - Key '%s' - Type 'string' - ", key );
	Journal_PrintString( self->infoStream, Default );
	return (char*)Default;
}


Stg_Component* _OneOffComponentFactory_ConstructByName( 
		void*			cf, 
		Name			componentName, 
		Type			type, 
		Bool			isEssential, 
		void*			data )
{
	OneOffComponentFactory*    self              = (OneOffComponentFactory*) cf;

	Journal_Printf( self->infoStream, "%sEssential Component - Type '%s' - Name = '%s'\n", 
			( isEssential ? "" : "Non-" ), type, componentName );

	return NULL;
}

Stg_Component* _OneOffComponentFactory_ConstructByKey( 
		void*			cf, 
		Name			parentComponentName, 
		Dictionary_Entry_Key	componentKey, 
		Type			type, 
		Bool			isEssential,
		void*			data ) 
{
	OneOffComponentFactory* self = (OneOffComponentFactory*) cf;
	
	Journal_Printf( self->infoStream, "%sEssential Component - Type '%s' - Key = '%s'\n", 
			( isEssential ? "" : "Non-" ), type, componentKey );

	return NULL;
}

Stg_Component* _OneOffComponentFactory_ConstructByNameWithKeyFallback( 
		void*			cf, 
		Name			parentComponentName, 
		Name			componentTrialName, 
		Dictionary_Entry_Key	fallbackKey, 
		Type			type, 
		Bool			isEssential,
		void*			data ) 
{
	OneOffComponentFactory* self = (OneOffComponentFactory*) cf;
	
	Journal_Printf( self->infoStream, "%sEssential Component - Type '%s' - Trial Name = '%s' - Fall back key = '%s'\n", 
			( isEssential ? "" : "Non-" ), type, componentTrialName, fallbackKey );

	return NULL;
}

Stg_Component** _OneOffComponentFactory_ConstructByList( 
		void*			componentFactory, 
		Name			parentComponentName, 
		Name			listName, 
		unsigned int		maxComponents,
		Type			type,
		Bool			isEssential,
		unsigned int*		componentCount,
		void*			data )
{
	OneOffComponentFactory* self = (OneOffComponentFactory*)componentFactory;

	Journal_Printf( self->infoStream, "%sEssential Components of type '%s' in list named '%s'\n", 
			( isEssential ? "" : "Non-" ), type, listName );

	return NULL;
}

void OneOffComponentFactory_DocumentType( void* cf, Name typeName ) {
	OneOffComponentFactory*             self              = (OneOffComponentFactory*) cf;
	Stg_Component_DefaultConstructorFunction*  defaultConstructor;
	Stg_Component*                             component;
	Stream*                                    stream            = self->infoStream;
	Type                                       type;

	type = HierarchyTable_GetTypePtrFromName( HierarchyTable_GetHierarchyTable(), typeName );
			
	Stream_Enable( stream, False );

	defaultConstructor = Stg_ComponentRegister_AssertGet( 
			Stg_ComponentRegister_Get_ComponentRegister(), type, "0" );

	component = (Stg_Component*) defaultConstructor( "componentName" );

	Stream_Enable( stream, True );
	Journal_Printf( stream, "Class '%s'\n", type );
	Stream_Indent( stream );

	Journal_Printf( stream, "Parent - %s\n", GetParent( type ) );

	Stg_Component_AssignFromXML( component, (Stg_ComponentFactory*)self, 0 /* dummy */, True );

	Stg_Class_Delete( component );
	Stream_UnIndent( self->infoStream );
}


