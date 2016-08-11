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

#ifndef __StgDomain_Mesh_MeshGenerator_h__
#define __StgDomain_Mesh_MeshGenerator_h__

	/** Textual name of this class */
	extern const Type MeshGenerator_Type;

	/** Virtual function types */
	typedef void (MeshGenerator_SetDimSizeFunc)( void* meshGenerator, unsigned nDims );
	typedef void (MeshGenerator_GenerateFunc)( void* meshGenerator, void* _mesh, void* data );

	/** MeshGenerator class contents */
	#define __MeshGenerator					\
		/* General info */				\
		__Stg_Component					\
								\
		AbstractContext*		context;	\
		/* Virtual info */				\
		MeshGenerator_SetDimSizeFunc*	setDimSizeFunc;	\
		MeshGenerator_GenerateFunc*	generateFunc;	\
								\
		/* MeshGenerator info */			\
		MPI_Comm		mpiComm;		\
		unsigned		nMeshes;		\
		Mesh**			meshes;			\
								\
		unsigned		nDims;			\
		Bool*			enabledDims;		\
		Bool**			enabledInc;

	struct MeshGenerator { __MeshGenerator };

	/*--------------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/



	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define MESHGENERATOR_DEFARGS \
                STG_COMPONENT_DEFARGS, \
                MeshGenerator_SetDimSizeFunc*  setDimSizeFunc, \
                MeshGenerator_GenerateFunc*      generateFunc

	#define MESHGENERATOR_PASSARGS \
                STG_COMPONENT_PASSARGS, \
	        setDimSizeFunc, \
	        generateFunc  

	MeshGenerator* _MeshGenerator_New(  MESHGENERATOR_DEFARGS  );
	void _MeshGenerator_Init( MeshGenerator* self, AbstractContext* context );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Virtual functions
	*/

	void _MeshGenerator_Delete( void* meshGenerator );
	void _MeshGenerator_Print( void* meshGenerator, Stream* stream );

	#define MeshGenerator_Copy( self ) \
		(Mesh*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define MeshGenerator_DeepCopy( self ) \
		(Mesh*)Stg_Class_Copy( self, NULL, True, NULL, NULL )

	void _MeshGenerator_AssignFromXML( void* meshGenerator, Stg_ComponentFactory* cf, void* data );
	void _MeshGenerator_Build( void* meshGenerator, void* data );
	void _MeshGenerator_Initialise( void* meshGenerator, void* data );
	void _MeshGenerator_Execute( void* meshGenerator, void* data );
	void _MeshGenerator_Destroy( void* meshGenerator, void* data );

	void _MeshGenerator_SetDimSize( void* meshGenerator, unsigned nDims );

	#define MeshGenerator_SetDimSize( self, nDims )			\
		VirtualCall( self, setDimSizeFunc, self, nDims )

	#define MeshGenerator_Generate( self, mesh, data )			\
		VirtualCall( self, generateFunc, self, mesh, data )

	/*--------------------------------------------------------------------------------------------------------------------------
	** Public functions
	*/

	void MeshGenerator_SetMPIComm( void* meshGenerator, MPI_Comm mpiComm );
	void MeshGenerator_AddMesh( void* meshGenerator, void* mesh );
	void MeshGenerator_SetFullIncidence( void* meshGenerator );
	void MeshGenerator_SetDimState( void* meshGenerator, unsigned dim, Bool state );
	void MeshGenerator_ClearIncidenceStates( void* meshGenerator );
	void MeshGenerator_SetIncidenceState( void* meshGenerator, unsigned fromDim, unsigned toDim, Bool state );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/

	void MeshGenerator_Destruct( MeshGenerator* self );

#endif /* __StgDomain_Mesh_MeshGenerator_h__ */

