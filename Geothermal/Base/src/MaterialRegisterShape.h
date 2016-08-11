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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __Geothermal_Base_MaterialRegisterShapeClass_h__
#define __Geothermal_Base_MaterialRegisterShapeClass_h__

	/* Textual name of this class */
	extern const Type MaterialRegisterShape_Type;

	/* MaterialRegisterShape information */
	#define __MaterialRegisterShape            \
		/* General info */                      \
		__Stg_Shape                             \
		/* Virtual Info */                      \
		                                        \
		FieldVariable*       valueField;        \
		double               lowerLimit;        \
		double               upperLimit;        \
		Name                 materialName;

	struct MaterialRegisterShape { __MaterialRegisterShape };
	
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define MATERIALREGISTERSHAPE_DEFARGS \
                STG_SHAPE_DEFARGS

	#define MATERIALREGISTERSHAPE_PASSARGS \
                STG_SHAPE_PASSARGS

	MaterialRegisterShape* _MaterialRegisterShape_New(  MATERIALREGISTERSHAPE_DEFARGS  );
	
   void _MaterialRegisterShape_Init( void* materialRegisterShape, FieldVariable* valueField, AbstractContext*	context, Name materialName  );

	/* Stg_Class_Delete MaterialRegisterShape implementation */
	void _MaterialRegisterShape_Delete( void* materialRegisterShape );
	#define MaterialRegisterShape_Copy( self ) \
		(MaterialRegisterShape*) Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define MaterialRegisterShape_DeepCopy( self ) \
		(MaterialRegisterShape*) Stg_Class_Copy( self, NULL, True, NULL, NULL )
	
	void* _MaterialRegisterShape_DefaultNew( Name name ) ;
	void _MaterialRegisterShape_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) ;
	void _MaterialRegisterShape_Build( void* materialRegisterShape, void* data ) ;
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Public member functions
	*/
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/
	
	
#endif 

