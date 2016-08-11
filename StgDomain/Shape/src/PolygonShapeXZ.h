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

#ifndef __StgDomain_Shape_PolygonShapeXZClass_h__
#define __StgDomain_Shape_PolygonShapeXZClass_h__

	/* Textual name of this class */
	extern const Type PolygonShapeXZ_Type;

	/* PolygonShapeXZ information */
	#define __PolygonShapeXZ \
		/* General info */ \
		__Stg_Shape \
		/* Virtual Info */\
		\
		Coord_List              vertexList;    \
		Index                   vertexCount;   \
		XYZ                     start;        \
		XYZ                     end;          \
		XYZ                     centroid;     \

	struct PolygonShapeXZ { __PolygonShapeXZ };
	
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/
	
	PolygonShapeXZ* PolygonShapeXZ_New(
		Name                                  name,
		Dimension_Index                       dim,
		Coord_List                            vertexList,
		Index                                 vertexCount,
		XYZ                                   start,
		XYZ                                   end );
		
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define POLYGONSHAPEXZ_DEFARGS \
                STG_SHAPE_DEFARGS

	#define POLYGONSHAPEXZ_PASSARGS \
                STG_SHAPE_PASSARGS

	PolygonShapeXZ* _PolygonShapeXZ_New(  POLYGONSHAPEXZ_DEFARGS  );
	
	void _PolygonShapeXZ_Init( void* polygon, Coord_List vertexList, Index vertexCount, XYZ start, XYZ end ) ;
		
	/* Stg_Class_Delete PolygonShapeXZ implementation */
	void _PolygonShapeXZ_Delete( void* polygon );
	void _PolygonShapeXZ_Print( void* polygon, Stream* stream );
	#define PolygonShapeXZ_Copy( self ) \
		(PolygonShapeXZ*) Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define PolygonShapeXZ_DeepCopy( self ) \
		(PolygonShapeXZ*) Stg_Class_Copy( self, NULL, True, NULL, NULL )
	void* _PolygonShapeXZ_Copy( void* polygon, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	
	void* _PolygonShapeXZ_DefaultNew( Name name ) ;
	void _PolygonShapeXZ_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) ;
	void _PolygonShapeXZ_Build( void* polygon, void* data ) ;
	void _PolygonShapeXZ_Initialise( void* polygon, void* data ) ;
	void _PolygonShapeXZ_Execute( void* polygon, void* data );
	void _PolygonShapeXZ_Destroy( void* polygon, void* data ) ;
	
	Bool _PolygonShapeXZ_IsCoordInside( void* polygon, Coord coord ) ;
	double _PolygonShapeXZ_CalculateVolume( void* polygon );
	void _PolygonShapeXZ_DistanceFromCenterAxis( void* shape, Coord coord, double* disVec );

	/*---------------------------------------------------------------------------------------------------------------------
	** Public member functions
	*/
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/
	
	
#endif 

