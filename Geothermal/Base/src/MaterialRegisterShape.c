/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org) )
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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include "types.h"
#include "MaterialRegisterShape.h"

#include <assert.h>
#include <string.h>
#include <math.h>


/* Textual name of this class */
const Type MaterialRegisterShape_Type = (const Type)"MaterialRegisterShape";

/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/

MaterialRegisterShape* _MaterialRegisterShape_New(  MATERIALREGISTERSHAPE_DEFARGS  )
{
	MaterialRegisterShape* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(MaterialRegisterShape) );
	self = (MaterialRegisterShape*)_Stg_Shape_New(  STG_SHAPE_PASSARGS  );
	
	/* General info */
	return self;
}

/*------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/
void _MaterialRegisterShape_Delete( void* materialRegisterShape ) {
	MaterialRegisterShape* self = (MaterialRegisterShape*)materialRegisterShape;
	
	/* Delete parent */
	Memory_Free( self->materialName );
	_Stg_Shape_Delete( self );
}

void* _MaterialRegisterShape_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                  _sizeOfSelf = sizeof(MaterialRegisterShape);
	Type                                                          type = MaterialRegisterShape_Type;
	Stg_Class_DeleteFunction*                                  _delete = _MaterialRegisterShape_Delete;
	Stg_Class_PrintFunction*                                    _print = _FieldValueShape_Print;
	Stg_Class_CopyFunction*                                      _copy = _FieldValueShape_Copy;
	Stg_Component_DefaultConstructorFunction*      _defaultConstructor = _MaterialRegisterShape_DefaultNew;
	Stg_Component_ConstructFunction*                        _construct = _MaterialRegisterShape_AssignFromXML;
	Stg_Component_BuildFunction*                                _build = _MaterialRegisterShape_Build;
	Stg_Component_InitialiseFunction*                      _initialise = _FieldValueShape_Initialise;
	Stg_Component_ExecuteFunction*                            _execute = _FieldValueShape_Execute;
	Stg_Component_DestroyFunction*                            _destroy = _FieldValueShape_Destroy;
	Stg_Shape_IsCoordInsideFunction*                    _isCoordInside = _FieldValueShape_IsCoordInside;
	Stg_Shape_CalculateVolumeFunction*                _calculateVolume = _FieldValueShape_CalculateVolume;
	Stg_Shape_DistanceFromCenterAxisFunction*  _distanceFromCenterAxis = _FieldValueShape_DistanceFromCenterAxis;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*) _MaterialRegisterShape_New(  MATERIALREGISTERSHAPE_PASSARGS  );
}


void _MaterialRegisterShape_AssignFromXML( void* materialRegisterShape, Stg_ComponentFactory* cf, void* data ) {
	MaterialRegisterShape*  self          = (MaterialRegisterShape*) materialRegisterShape;
	FieldVariable*    valueField;
	AbstractContext*	context;
	Name materialName;

	_Stg_Shape_AssignFromXML( self, cf, data );

	context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", AbstractContext, False, data );
	if( !context  ) 
		context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", AbstractContext, True, data  );

	/** fieldVariable which determines values which must lie between limits to be considered inside shape */
   valueField = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"ValueField", FieldVariable, True, data  );

	/** value must be greater than this limit */
	materialName = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"MaterialName", ""  );
	
   Journal_Firewall( !(strcmp(materialName, "")==0),
     Journal_MyStream( Error_Type, self ),
     "Error in %s for %s '%s' - No material name was specified (use the parameter name MaterialName). \n\n ",
     __func__,
     self->type,
     self->name);
   
	_MaterialRegisterShape_Init( self, valueField, context, materialName );
}

void _MaterialRegisterShape_Init( void* materialRegisterShape, FieldVariable* valueField, AbstractContext*	context, Name materialName ) {
	MaterialRegisterShape* self = (MaterialRegisterShape*)materialRegisterShape;

	self->valueField = valueField;
	self->context    = context;
	Stg_asprintf( &self->materialName, "%s", materialName );
}

void _MaterialRegisterShape_Build( void* materialRegisterShape, void* data ) {
	MaterialRegisterShape*	self = (MaterialRegisterShape*)materialRegisterShape;
   Material* namedMaterial;
	
	_Stg_Shape_Build( self, data );

	Stg_Component_Build( self->valueField, NULL, False );
	/** go ahead and initialise this as well, as it is required before the standard init phase */
	Stg_Component_Initialise( self->valueField, NULL, False );	
	
   namedMaterial    = Materials_Register_GetByName( ((PICelleratorContext*)self->context)->materials_Register, self->materialName );

   Journal_Firewall( (namedMaterial != NULL),
     global_error_stream,
     "Error in %s for %s '%s' - No material with name %s found within the material register.",
     __func__,
     self->type,
     self->name,
     self->materialName);

   self->lowerLimit = (double)namedMaterial->index - 0.5;
   self->upperLimit = (double)namedMaterial->index + 0.5;
}

