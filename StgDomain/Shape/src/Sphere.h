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

#ifndef __StgDomain_Shape_SphereClass_h__
#define __StgDomain_Shape_SphereClass_h__

	/* Textual name of this class */
	extern const Type Sphere_Type;

	/* Sphere information */
	#define __Sphere \
		/* General info */ \
		__Stg_Shape \
		/* Virtual Info */\
		\
		double          radius;         \
		double		radiusSquared;  \

	struct Sphere { __Sphere };
	
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/
	Sphere* Sphere_New(
		Name                                  name,
		Dimension_Index                       dim,
		XYZ                                   centre, 
		double                                alpha,
		double                                beta,
		double                                gamma,
		double                                radius );

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define SPHERE_DEFARGS \
                STG_SHAPE_DEFARGS

	#define SPHERE_PASSARGS \
                STG_SHAPE_PASSARGS

	Sphere* _Sphere_New(  SPHERE_DEFARGS  );
	
	void _Sphere_Init( void* sphere, double radius ) ;

	/* Stg_Class_Delete Sphere implementation */
	void _Sphere_Delete( void* sphere );
	void _Sphere_Print( void* sphere, Stream* stream );
	#define Sphere_Copy( self ) \
		(Sphere*) Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define Sphere_DeepCopy( self ) \
		(Sphere*) Stg_Class_Copy( self, NULL, True, NULL, NULL )
	void* _Sphere_Copy( void* sphere, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	
	void* _Sphere_DefaultNew( Name name ) ;
	void _Sphere_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) ;
	void _Sphere_Build( void* sphere, void* data ) ;
	void _Sphere_Initialise( void* sphere, void* data ) ;
	void _Sphere_Execute( void* sphere, void* data );
	void _Sphere_Destroy( void* sphere, void* data ) ;
	
	Bool _Sphere_IsCoordInside( void* sphere, Coord coord ) ;
	void _Sphere_DistanceFromCenterAxis( void* sphere, Coord coord, double* disVec );
	double _Sphere_CalculateVolume( void* sphere );

	/*---------------------------------------------------------------------------------------------------------------------
	** Public member functions
	*/
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/
	
	
#endif 

