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

#ifdef HAVE_PYTHON
#ifndef __StgDomain_Shape_PythonShapeClass_h__
#define __StgDomain_Shape_PythonShapeClass_h__

	/* Textual name of this class */
	extern const Type PythonShape_Type;

	/* PythonShape information */
	#define __PythonShape \
		/* General info */ \
		__Stg_Shape \
		/* Virtual Info */\
		\
		char*                                 testCondition;           \
		void*                                 pythonDictionary;        \

	struct PythonShape { __PythonShape };
	
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/
	PythonShape* PythonShape_New(
		Name                                  name,
		Dimension_Index                       dim,
		XYZ                                   centre, 
		double                                alpha,
		double                                beta,
		double                                gamma,
		char*                                 conditionFunction );

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PYTHONSHAPE_DEFARGS \
                STG_SHAPE_DEFARGS

	#define PYTHONSHAPE_PASSARGS \
                STG_SHAPE_PASSARGS

	PythonShape* _PythonShape_New(  PYTHONSHAPE_DEFARGS  );
	
	void _PythonShape_Init( void* pythonShape, char* conditionFunction ) ;
	void PythonShape_InitAll( 
		void*                                 pythonShape, 
		Dimension_Index                       dim, 
		Coord                                 centre,
		double                                alpha,
		double                                beta,
		double                                gamma,
		char*                                 conditionFunction );

	/* Stg_Class_Delete PythonShape implementation */
	void _PythonShape_Delete( void* pythonShape );
	void _PythonShape_Print( void* pythonShape, Stream* stream );
	#define PythonShape_Copy( self ) \
		(PythonShape*) Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define PythonShape_DeepCopy( self ) \
		(PythonShape*) Stg_Class_Copy( self, NULL, True, NULL, NULL )
	void* _PythonShape_Copy( void* pythonShape, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	
	void* _PythonShape_DefaultNew( Name name ) ;
	void _PythonShape_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) ;
	void _PythonShape_Build( void* pythonShape, void* data ) ;
	void _PythonShape_Initialise( void* pythonShape, void* data ) ;
	void _PythonShape_Execute( void* pythonShape, void* data );
	void _PythonShape_Destroy( void* pythonShape, void* data ) ;
	
	Bool _PythonShape_IsCoordInside( void* pythonShape, Coord coord ) ;
	double _PythonShape_CalculateVolume( void* pythonShape );
	void _PythonShape_DistanceFromCenterAxis( void* shape, Coord coord, double* disVec );

	/*---------------------------------------------------------------------------------------------------------------------
	** Public member functions
	*/
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/
	
	
#endif 
#endif /* HAVE_PYTHON */

