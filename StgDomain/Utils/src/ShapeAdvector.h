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
*/
/** \file
**  Role:
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgDomain_Utils_ShapeAdvector_h__
#define __StgDomain_Utils_ShapeAdvector_h__

	/* Textual name of this class */
	extern const Type ShapeAdvector_Type;

	/* ShapeAdvector information */
	#define __ShapeAdvector \
		/* General info */ \
		__Stg_Component \
		DomainContext*		context; \
		/* Virtual Info */\
		/* Other Info */\
		Stg_Shape*			shape; \
		FieldVariable*		velocityField; \
		TimeIntegrand*	timeIntegrand; \
		Variable*			shapeCentreVariable;  \
		Index					shapeCount; \
		void*					shapeCentrePtr;

	struct ShapeAdvector { __ShapeAdvector };
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/
	ShapeAdvector* ShapeAdvector_New(
		Name					name,
		DomainContext*		context,
		TimeIntegrator*	timeIntegrator,
		FieldVariable*		velocityField,
		Stg_Shape*			shape,
		Bool					allowFallbackToFirstOrder );

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define SHAPEADVECTOR_DEFARGS \
                STG_COMPONENT_DEFARGS

	#define SHAPEADVECTOR_PASSARGS \
                STG_COMPONENT_PASSARGS

	ShapeAdvector* _ShapeAdvector_New(  SHAPEADVECTOR_DEFARGS  );

	void _ShapeAdvector_Init( 
		ShapeAdvector*		self,
		DomainContext*		context,
		TimeIntegrator*	timeIntegrator,
		FieldVariable*		velocityField,
		Stg_Shape*			shape,
		Bool					allowFallbackToFirstOrder );

	void _ShapeAdvector_Delete( void* materialPoints );

	void _ShapeAdvector_Print( void* materialPoints, Stream* stream );

	#define ShapeAdvector_Copy( self ) \
		(ShapeAdvector*) Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define ShapeAdvector_DeepCopy( self ) \
		(ShapeAdvector*) Stg_Class_Copy( self, NULL, True, NULL, NULL )

	void* _ShapeAdvector_Copy( void* materialPoints, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	
	void* _ShapeAdvector_DefaultNew( Name name );

	void _ShapeAdvector_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data );

	void _ShapeAdvector_Build( void* materialPoints, void* data );

	void _ShapeAdvector_Initialise( void* materialPoints, void* data );

	void _ShapeAdvector_Execute( void* materialPoints, void* data );

	void _ShapeAdvector_Destroy( void* materialPoints, void* data );

	/*---------------------------------------------------------------------------------------------------------------------
	** Private functions
	*/
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Entry Point Hooks
	*/
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Public functions
	*/

#endif 

