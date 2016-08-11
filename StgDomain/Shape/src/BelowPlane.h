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

#ifndef __StgDomain_Shape_BelowPlaneClass_h__
#define __StgDomain_Shape_BelowPlaneClass_h__

	/* Textual name of this class */
	extern const Type BelowPlane_Type;

	/* BelowPlane information */
	#define __BelowPlane \
		/* General info */ \
		__Stg_Shape \
		/* Virtual Info */\
		\
		double                  offset; \
		XYZ                     width; /* The dimesions of a cubiod mesh, for calc volume */ \
		XYZ                     minValue; \
		XYZ                     maxValue; \

	struct BelowPlane { __BelowPlane };
	
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define BELOWPLANE_DEFARGS \
                STG_SHAPE_DEFARGS

	#define BELOWPLANE_PASSARGS \
                STG_SHAPE_PASSARGS

	BelowPlane* _BelowPlane_New(  BELOWPLANE_DEFARGS  );
	
	void _BelowPlane_Init( void* belowPlane, double offset, XYZ width, XYZ minValue, XYZ maxValue ) ;

	/* Stg_Class_Delete BelowPlane implementation */
	void _BelowPlane_Delete( void* belowPlane );
	void _BelowPlane_Print( void* belowPlane, Stream* stream );
	#define BelowPlane_Copy( self ) \
		(BelowPlane*) Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define BelowPlane_DeepCopy( self ) \
		(BelowPlane*) Stg_Class_Copy( self, NULL, True, NULL, NULL )
	void* _BelowPlane_Copy( void* belowPlane, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	
	void* _BelowPlane_DefaultNew( Name name ) ;
	void _BelowPlane_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) ;
	void _BelowPlane_Build( void* belowPlane, void* data ) ;
	void _BelowPlane_Initialise( void* belowPlane, void* data ) ;
	void _BelowPlane_Execute( void* belowPlane, void* data );
	void _BelowPlane_Destroy( void* belowPlane, void* data ) ;
	
	Bool _BelowPlane_IsCoordInside( void* belowPlane, Coord coord ) ;

	double _BelowPlane_CalculateVolume( void* belowPlane );
	void _BelowPlane_DistanceFromCenterAxis( void* shape, Coord coord, double* disVec );

	/*---------------------------------------------------------------------------------------------------------------------
	** Public member functions
	*/
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/
	
	
#endif 

