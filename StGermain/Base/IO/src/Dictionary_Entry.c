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

#include "types.h"
#include "shortcuts.h"
#include "Journal.h"
#include "Dictionary_Entry_Value.h"
#include "Dictionary_Entry.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>


Dictionary_Entry* Dictionary_Entry_New( Dictionary_Entry_Key key, Dictionary_Entry_Value* value )
{
	Dictionary_Entry* self;
	
	self = Memory_Alloc( Dictionary_Entry, "Entry" );
	Dictionary_Entry_Init( self, key, value );
	
	return self;
}

Dictionary_Entry* Dictionary_Entry_NewWithSource( Dictionary_Entry_Key key, Dictionary_Entry_Value* value, Dictionary_Entry_Source source )
{
	Dictionary_Entry* self;
	
	self = Memory_Alloc( Dictionary_Entry, "Entry" );
	assert( key );
	assert( value );
	
	self->key = StG_Strdup( key );
	self->value = value;
	if( source != NULL )
		self->source = StG_Strdup( source );
	else
		self->source = NULL;

	return self;
}

void Dictionary_Entry_Init( Dictionary_Entry* self, Dictionary_Entry_Key key, Dictionary_Entry_Value* value )
{
	assert( key );
	assert( value );

	self->key = StG_Strdup( key );
	self->value = value;
	self->source = NULL;
   self->units = NULL;
}

void Dictionary_Entry_Delete( Dictionary_Entry* self )
{
	Memory_Free( self->key );
	Dictionary_Entry_Value_Delete( self->value );
	if( self->source != NULL )
		Memory_Free( self->source );
	Memory_Free( self );
}

void Dictionary_Entry_Print( Dictionary_Entry* self, Stream* stream )
{
	Journal_Printf( stream, "%s: ", self->key );
	Dictionary_Entry_Value_Print( self->value, stream );
	if( self->source != NULL )
		Journal_Printf( stream, " (original source file: %s)", self->source );
}

Bool Dictionary_Entry_Compare( Dictionary_Entry* self, Dictionary_Entry_Key key )
{
	return (Bool)!strcmp( self->key, key );
}

Bool Dictionary_Entry_CompareWithSource( Dictionary_Entry* self, Dictionary_Entry_Key key, Dictionary_Entry_Source source )
{
	if( self->source == NULL )
	{
		if( source == NULL )
			return (Bool)!strcmp( self->key, key );
		else
			return False;
	}
	else
	{
		if( source == NULL )
			return False;
		else
			return (Bool)( !strcmp( self->key, key ) && !strcmp( self->source, source ) );
	}
}

void Dictionary_Entry_Set( Dictionary_Entry* self, Dictionary_Entry_Value* value )
{
	assert( value );
	Dictionary_Entry_Value_Delete( self->value );
	self->value = value;
}

void Dictionary_Entry_SetWithSource( Dictionary_Entry* self, Dictionary_Entry_Value* value, Dictionary_Entry_Source source )
{
	assert( value );
	Dictionary_Entry_Value_Delete( self->value );
	self->value = value;

	if( self->source != NULL )
		Memory_Free( self->source );
	if( source != NULL )
		self->source = StG_Strdup( source );
}

void Dictionary_Entry_SetSource( Dictionary_Entry* self, Dictionary_Entry_Source source )
{
	if( self->source != NULL )
		Memory_Free( self->source );
	if( source != NULL )
		self->source = StG_Strdup( source );
}

Dictionary_Entry_Value* Dictionary_Entry_Get( Dictionary_Entry* self )
{
	return self->value;
}

Dictionary_Entry_Source Dictionary_Entry_GetSource( Dictionary_Entry* self )
{
	return self->source;
}


