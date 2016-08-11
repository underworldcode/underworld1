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
** Invariants:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __Geothermal_Base_SimplexType_h__
#define __Geothermal_Base_SimplexType_h__

	/** Textual name of this class */
	extern const Type Mesh_SimplexType_Type;

	/** Virtual function types */

	/** Class contents */
	#define __Mesh_SimplexType						\
		/* General info */						\
		__Mesh_ElementType						\
										\
		/* Virtual info */						\
										\
		/* Mesh_SimplexType info */					\
		unsigned				mapSize;		\
		unsigned*				vertMap;		\
		unsigned*				inc;			\
		Mesh_ElementType_ElementHasPointFunc*	elementHasPoint;	\
		IArray*					incArray;		\
		unsigned**				triInds;		\

	struct Mesh_SimplexType { __Mesh_SimplexType };

	/*--------------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/



	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define MESH_SIMPLEXTYPE_DEFARGS \
                MESH_ELEMENTTYPE_DEFARGS

	#define MESH_SIMPLEXTYPE_PASSARGS \
                MESH_ELEMENTTYPE_PASSARGS

	Mesh_SimplexType* Mesh_SimplexType_New();
	Mesh_SimplexType* _Mesh_SimplexType_New(  MESH_SIMPLEXTYPE_DEFARGS  );
	void _Mesh_SimplexType_Init( Mesh_SimplexType* self );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Virtual functions
	*/

	void _Mesh_SimplexType_Delete( void* hexType );
	void _Mesh_SimplexType_Print( void* hexType, Stream* stream );

	void Mesh_SimplexType_Update( void* hexType );
	Bool Mesh_SimplexType_ElementHasPoint( void* hexType, unsigned elInd, double* point, 
					   MeshTopology_Dim* dim, unsigned* ind );
	double Mesh_SimplexType_GetMinimumSeparation( void* hexType, unsigned elInd, double* perDim );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Public functions
	*/

	void Mesh_SimplexType_SetVertexMap( void* hexType, unsigned* map );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/

	Bool Mesh_SimplexType_ElementHasPoint2D( Mesh_SimplexType* self, unsigned elInd, double* point, 
						    MeshTopology_Dim* dim, unsigned* ind );
	void Mesh_SimplexType_TetBarycenter( double** verts, unsigned* inc, unsigned* inds, double* point, double* bc );
	void Mesh_SimplexType_TriBarycenter( double** verts, unsigned* inc, unsigned* inds, double* point, double* bc );

#endif /* __Geothermal_Base_SimplexType_h__ */

