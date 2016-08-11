/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**	Melbourne, 3053, Australia.
** Copyright (c) 2005-2010, Monash University, Building 28, Monash University Clayton Campus,
**	Victoria, 3800, Australia
**
** Primary Contributing Organisations:
**	Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**	AuScope - http://www.auscope.org
**	Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**
** Contributors:
**	Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**	Patrick D. Sunter, Software Engineer, VPAC. (patrick@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**	Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
**	Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**	Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**	David Stegman, Postdoctoral Fellow, Monash University. (david.stegman@sci.monash.edu.au)
**	Wendy Sharples, PhD Student, Monash University (wendy.sharples@sci.monash.edu.au)
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
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include "types.h"
#include "ShapedMaterial.h"
#include <assert.h>
#include <string.h>

const Type ShapedMaterial_Type = (const Type)"ShapedMaterial";

void* _ShapedMaterial_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(ShapedMaterial);
	Type                                                      type = ShapedMaterial_Type;
	Stg_Class_DeleteFunction*                              _delete = _Material_Delete;
	Stg_Class_PrintFunction*                                _print = _Material_Print;
	Stg_Class_CopyFunction*                                  _copy = _Material_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _ShapedMaterial_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _ShapedMaterial_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Material_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Material_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Material_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Material_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*) _ShapedMaterial_New(  SHAPEDMATERIAL_PASSARGS  );
}


/* Private Constructor */
ShapedMaterial* _ShapedMaterial_New(  SHAPEDMATERIAL_DEFARGS  )
{
	ShapedMaterial* self;
	
	assert( _sizeOfSelf >= sizeof(ShapedMaterial) );
	/* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
	/* This means that any values of these parameters that are passed into this function are not passed onto the parent function
	   and so should be set to ZERO in any children of this class. */
	nameAllocationType = NON_GLOBAL;

	self = (ShapedMaterial*) _Stg_Component_New(  STG_COMPONENT_PASSARGS  );

	return self;
}

void _ShapedMaterial_Init( 
	void*						shapedMaterial,
	PICelleratorContext*	context,
	FieldVariable*			materialIndexField,
	Dictionary*				materialDictionary,
	Materials_Register*	shapedMaterialRegister )
{
	ShapedMaterial* self = (ShapedMaterial*)shapedMaterial;
	char* name;
	Coord centre = {0,0,0};
	
	/* Set Values */
	self->context = context;

	self->dictionary = materialDictionary;

	/* Register shapedMaterial */
	if (shapedMaterialRegister != NULL)	
		self->index = Materials_Register_Add( shapedMaterialRegister, self );	
	else 
		self->index = 0;

	Stg_asprintf( &name, "%s_Shape", self->name );

	/** create a field value shape using the current material's index to define limits for the fieldvalueshape */
	self->shape = FieldValueShape_New( name, 3,centre,0,0,0,materialIndexField,self->index-0.5,self->index+ 0.5 );
	
	Memory_Free( name );

	self->extensionMgr = ExtensionManager_New_OfExistingObject( self->name, self );

}

void _ShapedMaterial_AssignFromXML( void* shapedMaterial, Stg_ComponentFactory* cf, void* data ) {
	ShapedMaterial*				self = (ShapedMaterial*) shapedMaterial;
	Materials_Register*	Materials_Register;
	PICelleratorContext*	context;
	FieldVariable*       materialIndexField;
	Dictionary*				materialDictionary;

	context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", PICelleratorContext, False, data );
	if( !context  ) 
		context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", PICelleratorContext, True, data  );

	materialDictionary = Dictionary_GetDictionary( cf->componentDict, self->name );

	/** user must provide a field which represents the shapes, with field values corresponding to current material index */
	/** (this field should be initialised later, so no circular dependency should result)                                */
	materialIndexField =  Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"MaterialIndexField", FieldVariable, True, data   ) ;
	Materials_Register = context->materials_Register;

	_ShapedMaterial_Init( self, context, materialIndexField, materialDictionary, Materials_Register );
}

