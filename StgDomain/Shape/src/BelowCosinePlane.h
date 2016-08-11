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

#ifndef __StgDomain_Shape_BelowCosinePlaneClass_h__
#define __StgDomain_Shape_BelowCosinePlaneClass_h__

	/* Textual name of this class */
	extern const Type BelowCosinePlane_Type;

	/* BelowCosinePlane information */
	#define __BelowCosinePlane \
		/* General info */ \
		__BelowPlane \
		/* Virtual Info */\
		\
		double                  amplitude;  \
		double                  wavelength; \
		double                  phase;

	struct BelowCosinePlane { __BelowCosinePlane };
	
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define BELOWCOSINEPLANE_DEFARGS \
                BELOWPLANE_DEFARGS

	#define BELOWCOSINEPLANE_PASSARGS \
                BELOWPLANE_PASSARGS

   BelowCosinePlane* BelowCosinePlane_New(
		Name                                  name,
		Dimension_Index                       dim,
		XYZ                                   centre, 
		double                                alpha,
		double                                beta,
		double                                gamma,
		double                                offset,
		XYZ                                   width,
		XYZ                                   minValue,
		XYZ                                   maxValue,
		double                                amplitude,
		double                                wavelength,
		double                                phase );
	BelowCosinePlane* _BelowCosinePlane_New(  BELOWCOSINEPLANE_DEFARGS  );
	
	void _BelowCosinePlane_Init( void* belowPlane, XYZ width, double amplitude, double wavelength, double phase ) ;

	/* Stg_Class_Delete BelowCosinePlane implementation */
	void _BelowCosinePlane_Delete( void* belowPlane );
	void _BelowCosinePlane_Print( void* belowPlane, Stream* stream );
	#define BelowCosinePlane_Copy( self ) \
		(BelowCosinePlane*) Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define BelowCosinePlane_DeepCopy( self ) \
		(BelowCosinePlane*) Stg_Class_Copy( self, NULL, True, NULL, NULL )
	void* _BelowCosinePlane_Copy( void* belowPlane, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	
	void* _BelowCosinePlane_DefaultNew( Name name ) ;
	void _BelowCosinePlane_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) ;
	void _BelowCosinePlane_Build( void* belowPlane, void* data ) ;
	void _BelowCosinePlane_Initialise( void* belowPlane, void* data ) ;
	void _BelowCosinePlane_Execute( void* belowPlane, void* data );
	void _BelowCosinePlane_Destroy( void* belowPlane, void* data ) ;
	
	Bool _BelowCosinePlane_IsCoordInside( void* belowPlane, Coord coord ) ;

	double _BelowCosinePlane_CalculateVolume( void* belowPlane );
	void _BelowCosinePlane_DistanceFromCenterAxis( void* shape, Coord coord, double* disVec );


	/*---------------------------------------------------------------------------------------------------------------------
	** Public member functions
	*/
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/
	
	
#endif 

