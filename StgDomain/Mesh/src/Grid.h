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

#ifndef __StgDomain_Mesh_Grid_h__
#define __StgDomain_Mesh_Grid_h__

	/** Textual name of this class */
	extern const Type Grid_Type;

	/** Virtual function types */

	/** Grid class contents */
	#define __Grid				\
		/* General info */		\
		__Stg_Class			\
						\
		/* Virtual info */		\
						\
		/* Grid info */			\
		unsigned	nDims;		\
		unsigned*	sizes;		\
		unsigned*	basis;		\
		unsigned	nPoints;	\
						\
		HashTable*	map;		\
		HashTable*	invMap;

	struct Grid { __Grid };

	/*--------------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/



	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define GRID_DEFARGS \
                STG_CLASS_DEFARGS

	#define GRID_PASSARGS \
                STG_CLASS_PASSARGS

	Grid* Grid_New();
	Grid* _Grid_New(  GRID_DEFARGS  );
	void _Grid_Init( Grid* self );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Virtual functions
	*/

	void _Grid_Delete( void* grid );
	void _Grid_Print( void* grid, Stream* stream );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Public functions
	*/

	void Grid_SetNumDims( void* grid, unsigned nDims );
	void Grid_SetSizes( void* grid, unsigned* sizes );
	void Grid_SetMapping( void* grid, HashTable* mapping, HashTable* inverse );

	unsigned Grid_GetNumDims( void* grid );
	unsigned* Grid_GetSizes( void* grid );
	unsigned Grid_GetNumPoints( void* grid );
	void Grid_GetMapping( void* grid, HashTable** mapping, HashTable** inverse );

	void Grid_Lift( void* grid, unsigned ind, unsigned* params );
	unsigned Grid_Project( void* grid, unsigned* params );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/

#endif /* __StgDomain_Mesh_Grid_h__ */

