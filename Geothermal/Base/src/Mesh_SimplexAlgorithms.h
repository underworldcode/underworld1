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

#ifndef __Geothermal_Base_SimplexAlgorithms_h__
#define __Geothermal_Base_SimplexAlgorithms_h__

	/** Textual name of this class */
	extern const Type Mesh_SimplexAlgorithms_Type;

	/** Virtual function types */

	/** Class contents */
	#define __Mesh_SimplexAlgorithms		\
		/* General info */			\
		__Mesh_Algorithms			\
							\
		/* Virtual info */			\
							\
		/* Mesh_SimplexAlgorithms info */

	struct Mesh_SimplexAlgorithms { __Mesh_SimplexAlgorithms };

	/*--------------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/



	Mesh_SimplexAlgorithms* Mesh_SimplexAlgorithms_New( Name name, AbstractContext* context );

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define MESH_SIMPLEXALGORITHMS_DEFARGS \
                MESH_ALGORITHMS_DEFARGS

	#define MESH_SIMPLEXALGORITHMS_PASSARGS \
                MESH_ALGORITHMS_PASSARGS

	Mesh_SimplexAlgorithms* _Mesh_SimplexAlgorithms_New(  MESH_SIMPLEXALGORITHMS_DEFARGS  );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Virtual functions
	*/

	void _Mesh_SimplexAlgorithms_Init( void* simplexAlgorithms );

	void _Mesh_SimplexAlgorithms_Delete( void* simplexAlgorithms );

	void _Mesh_SimplexAlgorithms_Print( void* simplexAlgorithms, Stream* stream );

	void _Mesh_SimplexAlgorithms_AssignFromXML( void* simplexAlgorithms, Stg_ComponentFactory* cf, void* data );

	void _Mesh_SimplexAlgorithms_Build( void* simplexAlgorithms, void* data );

	void _Mesh_SimplexAlgorithms_Initialise( void* simplexAlgorithms, void* data );

	void _Mesh_SimplexAlgorithms_Execute( void* simplexAlgorithms, void* data );

	void _Mesh_SimplexAlgorithms_Destroy( void* simplexAlgorithms, void* data );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Public functions
	*/

	/*--------------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/

	Bool _Mesh_SimplexAlgorithms_SearchElements( void* simplexAlgorithms, double* point, unsigned* elInd );

#endif /* __Geothermal_Base_SimplexAlgorithms_h__ */

