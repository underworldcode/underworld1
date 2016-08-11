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

#ifndef __Underworld_Utils_FieldValueShapeClass_h__
#define __Underworld_Utils_FieldValueShapeClass_h__

	/* Textual name of this class */
	extern const Type FieldValueShape_Type;

	/* FieldValueShape information */
	#define __FieldValueShape                  \
		/* General info */                      \
		__Stg_Shape                             \
		/* Virtual Info */                      \
		                                        \
		FieldVariable*       valueField;        \
		double               lowerLimit;        \
		double               upperLimit;

	struct FieldValueShape { __FieldValueShape };
	
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define FIELDVALUESHAPE_DEFARGS \
                STG_SHAPE_DEFARGS

	#define FIELDVALUESHAPE_PASSARGS \
                STG_SHAPE_PASSARGS

	FieldValueShape* _FieldValueShape_New(  FIELDVALUESHAPE_DEFARGS  );
	
   void _FieldValueShape_Init( void* fieldValueShape, FieldVariable* valueField, double lowerLimit, double upperLimit  );

	/* Stg_Class_Delete FieldValueShape implementation */
	void _FieldValueShape_Delete( void* fieldValueShape );
	void _FieldValueShape_Print( void* fieldValueShape, Stream* stream );
	#define FieldValueShape_Copy( self ) \
		(FieldValueShape*) Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define FieldValueShape_DeepCopy( self ) \
		(FieldValueShape*) Stg_Class_Copy( self, NULL, True, NULL, NULL )
	void* _FieldValueShape_Copy( void* fieldValueShape, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	
	void* _FieldValueShape_DefaultNew( Name name ) ;
	void _FieldValueShape_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) ;
	void _FieldValueShape_Build( void* fieldValueShape, void* data ) ;
	void _FieldValueShape_Initialise( void* fieldValueShape, void* data ) ;
	void _FieldValueShape_Execute( void* fieldValueShape, void* data );
	void _FieldValueShape_Destroy( void* fieldValueShape, void* data ) ;
	
	Bool _FieldValueShape_IsCoordInside( void* fieldValueShape, Coord coord ) ;
   FieldValueShape* FieldValueShape_New(
         Name               name,
         Dimension_Index    dim,
         XYZ                centre, 
         double             alpha,                                                                              
         double             beta,
         double             gamma,
         FieldVariable*     valueField,
         double             lowerLimit, 
         double             upperLimit );

	double _FieldValueShape_CalculateVolume( void* fieldValueShape );
	void _FieldValueShape_DistanceFromCenterAxis( void* shape, Coord coord, double* disVec );

	/*---------------------------------------------------------------------------------------------------------------------
	** Public member functions
	*/
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/
	
	
#endif 

