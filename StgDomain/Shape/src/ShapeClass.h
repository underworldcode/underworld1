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

#ifndef __StgDomain_Shape_ShapeClass_h__
#define __StgDomain_Shape_ShapeClass_h__

	typedef Bool (Stg_Shape_IsCoordInsideFunction) ( void* shape, Coord coord );

	typedef void (Stg_Shape_DistanceFromCenterAxisFunction) ( void* shape, Coord coord, double* disVec );

	typedef double (Stg_Shape_CalculateVolumeFunction) ( void* shape );


	/* Textual name of this class */
	extern const Type Stg_Shape_Type;

   /* Stg_Shape information */
#define __Stg_Shape \
   /* General info */ \
   __Stg_Component \
   AbstractContext*                      context;             \
   /* Virtual Info */                                         \
   Stg_Shape_IsCoordInsideFunction*      _isCoordInside;      \
   Stg_Shape_CalculateVolumeFunction*    _calculateVolume;    \
   Stg_Shape_DistanceFromCenterAxisFunction* _distanceFromCenterAxis; \
   /* Other info */                                           \
   Dimension_Index                       dim;                 \
   XYZ                                   centre;              \
   Bool                                  invert;              \
   double                                rotationMatrix[3][3]; \
   double                                rotations[3]; 		/* store the rotations */ \

   struct Stg_Shape { __Stg_Shape };
	
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/



	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define STG_SHAPE_DEFARGS \
                STG_COMPONENT_DEFARGS, \
                Stg_Shape_IsCoordInsideFunction*                    _isCoordInside, \
                Stg_Shape_CalculateVolumeFunction*                _calculateVolume, \
                Stg_Shape_DistanceFromCenterAxisFunction*  _distanceFromCenterAxis

	#define STG_SHAPE_PASSARGS \
                STG_COMPONENT_PASSARGS, \
	        _isCoordInside,          \
	        _calculateVolume,        \
	        _distanceFromCenterAxis

Stg_Shape* _Stg_Shape_New(  STG_SHAPE_DEFARGS  );
	
	void _Stg_Shape_Init( void* shape, Dimension_Index dim, Coord centre, Bool invert, double alpha, double beta, double gamma ) ;
	void Stg_Shape_InitAll( void* shape, Dimension_Index dim, Coord centre, double alpha, double beta, double gamma );

	void _Stg_Shape_Delete( void* shape );
	void _Stg_Shape_Print( void* shape, Stream* stream );
	#define Stg_Shape_Copy( self ) \
		(Stg_Shape*) Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define Stg_Shape_DeepCopy( self ) \
		(Stg_Shape*) Stg_Class_Copy( self, NULL, True, NULL, NULL )
	void* _Stg_Shape_Copy( void* shape, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );

	/* 'Stg_Component' Virtual Function Implementations */
	void _Stg_Shape_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) ;
	void _Stg_Shape_Build( void* shape, void* data ) ;
	void _Stg_Shape_Initialise( void* shape, void* data ) ;
	void _Stg_Shape_Execute( void* shape, void* data ) ;
	void _Stg_Shape_Destroy( void* shape, void* data ) ;

	
	/*---------------------------------------------------------------------------------------------------------------------
	** Virtual Function Implementation 
	*/
	Bool Stg_Shape_IsCoordInside( void* shape, Coord coord ) ;

	double Stg_Shape_CalculateVolume( void* shape );
	void Stg_Shape_DistanceFromCenterAxis( void* shape, Coord coord, double* disVec );
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Public member functions
	*/
	void Stg_Shape_TransformCoord( void* shape, Coord coord, Coord transformedCoord ) ;
	void Stg_Shape_TranslateCoord( void* shape, Coord coord, Coord translatedCoord ) ;
	
#endif 

