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
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifdef READ_HDF5
#include <hdf5.h>
#endif

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include "types.h"
#include "IrregularQuadGenerator.h"
#include "Mesh_SimplexType.h"
#include "Mesh_SimplexAlgorithms.h"
#include "IrregularTriGenerator.h"

#define OFFSET_TAG 6

/* Textual name of this class */
const Type IrregularTriGenerator_Type = (const Type)"IrregularTriGenerator";

/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/

IrregularTriGenerator* IrregularTriGenerator_New( Name name ) {
	/* Variables set in this function */
	SizeT                                                    _sizeOfSelf = sizeof(IrregularTriGenerator);
	Type                                                            type = IrregularTriGenerator_Type;
	Stg_Class_DeleteFunction*                                    _delete = _IrregularTriGenerator_Delete;
	Stg_Class_PrintFunction*                                      _print = _IrregularTriGenerator_Print;
	Stg_Component_DefaultConstructorFunction*        _defaultConstructor = (void* (*)(Name))_IrregularTriGenerator_New;
	Stg_Component_ConstructFunction*                          _construct = _IrregularTriGenerator_AssignFromXML;
	Stg_Component_BuildFunction*                                  _build = _IrregularTriGenerator_Build;
	Stg_Component_InitialiseFunction*                        _initialise = _IrregularTriGenerator_Initialise;
	Stg_Component_ExecuteFunction*                              _execute = _IrregularTriGenerator_Execute;
	Stg_Component_DestroyFunction*                              _destroy = _IrregularTriGenerator_Destroy;
	AllocationType                                    nameAllocationType = NON_GLOBAL;
	MeshGenerator_SetDimSizeFunc*                         setDimSizeFunc = IrregularTriGenerator_SetDimSize;
	MeshGenerator_GenerateFunc*                             generateFunc = IrregularTriGenerator_Generate;
	IrregularTriGenerator_SetTopologyParamsFunc*  setTopologyParamsFunc = _IrregularTriGenerator_SetTopologyParams;
	IrregularTriGenerator_GenElementTypesFunc*      genElementTypesFunc = _IrregularTriGenerator_GenElementTypes;

	/* The following terms are parameters that have been passed into or defined in this function but are being set before being passed onto the parent */
	Stg_Class_CopyFunction*  _copy = NULL;

   IrregularTriGenerator* self = _IrregularTriGenerator_New(  IRREGULARTRIGENERATOR_PASSARGS  );

   	_MeshGenerator_Init( (MeshGenerator*)self, NULL/*context*/ );
	_IrregularTriGenerator_Init( self );

    return self;
}

IrregularTriGenerator* _IrregularTriGenerator_New(  IRREGULARTRIGENERATOR_DEFARGS  ) {
	IrregularTriGenerator* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(IrregularTriGenerator) );
	self = (IrregularTriGenerator*)_MeshGenerator_New(  MESHGENERATOR_PASSARGS  );

	/* Virtual info */
	self->setTopologyParamsFunc = setTopologyParamsFunc;
	self->genElementTypesFunc = genElementTypesFunc;

	return self;
}

void _IrregularTriGenerator_Init( IrregularTriGenerator* self ) {
	Stream*	stream;

	assert( self && Stg_CheckType( self, IrregularTriGenerator ) );

	stream = Journal_Register( Info_Type, (Name)self->type  );
	Stream_SetPrintingRank( stream, 0 );

	self->comm = NULL;
	self->maxDecompDims = 0;
	self->minDecomp = NULL;
	self->maxDecomp = NULL;
	self->shadowDepth = 1;
}


/*----------------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _IrregularTriGenerator_Delete( void* meshGenerator ) {
	IrregularTriGenerator*	self = (IrregularTriGenerator*)meshGenerator;
	/* Delete the parent. */
	_MeshGenerator_Delete( self );
}

void _IrregularTriGenerator_Print( void* meshGenerator, Stream* stream ) {
	IrregularTriGenerator*	self = (IrregularTriGenerator*)meshGenerator;
	
	/* Set the Journal for printing informations */
	Stream* meshGeneratorStream;
	meshGeneratorStream = Journal_Register( InfoStream_Type, (Name)"IrregularTriGeneratorStream"  );

	assert( self && Stg_CheckType( self, IrregularTriGenerator ) );

	/* Print parent */
	Journal_Printf( stream, "IrregularTriGenerator (ptr): (%p)\n", self );
	_MeshGenerator_Print( self, stream );
}

void _IrregularTriGenerator_AssignFromXML( void* meshGenerator, Stg_ComponentFactory* cf, void* data ) {
	IrregularTriGenerator*	self = (IrregularTriGenerator*)meshGenerator;
	Dictionary*		dict;
	Dictionary_Entry_Value*	tmp;
	char*			rootKey;
	Dictionary_Entry_Value	*minList, *maxList;
	unsigned		maxDecompDims;
	unsigned		*minDecomp, *maxDecomp;
	unsigned		shadowDepth;
	Stream*			stream;
	Stream*			errorStream = Journal_Register( Error_Type, (Name)self->type  );
	unsigned		d_i, i;
	unsigned 		restartTimestep;
	char* 			meshSaveFileName;
	char*			checkpointPath;
	char*			checkpointPrefix;
	Index                   meshStrLen;
	
	assert( self && Stg_CheckType( self, IrregularTriGenerator ) );
	assert( cf );

	/* Call parent construct. */
	_MeshGenerator_AssignFromXML( self, cf, data );

	/* Rip out the components structure as a dictionary. */
	dict = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );

	/* Read decomposition restrictions. */
	maxDecompDims = Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"maxDecomposedDims", 0  );

	minList = Dictionary_Get( dict, (Dictionary_Entry_Key)"minDecompositions" );
	if( minList  ) {
		minDecomp = AllocArray( unsigned, self->nDims );
		for( d_i = 0; d_i < self->nDims; d_i++ ) {
			tmp = Dictionary_Entry_Value_GetElement( minList, d_i );
			rootKey = Dictionary_Entry_Value_AsString( tmp );

			if( !Stg_StringIsNumeric( (char *)rootKey )  )
				tmp = Dictionary_Get( cf->rootDict, (Dictionary_Entry_Key)rootKey );
			minDecomp[d_i] = Dictionary_Entry_Value_AsUnsignedInt( tmp  );
		}
	}
	else
		minDecomp = NULL;

	maxList = Dictionary_Get( dict, (Dictionary_Entry_Key)"maxDecompositions" );
	if( maxList  ) {
		maxDecomp = AllocArray( unsigned, self->nDims );
		for( d_i = 0; d_i < self->nDims; d_i++ ) {
			tmp = Dictionary_Entry_Value_GetElement( maxList, d_i );
			rootKey = Dictionary_Entry_Value_AsString( tmp );

			if( !Stg_StringIsNumeric( (char *)rootKey )  )
				tmp = Dictionary_Get( cf->rootDict, (Dictionary_Entry_Key)rootKey );
			maxDecomp[d_i] = Dictionary_Entry_Value_AsUnsignedInt( tmp  );
		}
	}
	else
		maxDecomp = NULL;

	/* Initial setup. */
	IrregularTriGenerator_SetTopologyParams( self, maxDecompDims, minDecomp, maxDecomp );

        /* Contact stuff. */
        self->contactDepth[0][0] = Stg_ComponentFactory_GetInt( cf, self->name, (Dictionary_Entry_Key)"contactDepth-left", 0  );
        self->contactDepth[0][1] = Stg_ComponentFactory_GetInt( cf, self->name, (Dictionary_Entry_Key)"contactDepth-right", 0  );
        self->contactDepth[1][0] = Stg_ComponentFactory_GetInt( cf, self->name, (Dictionary_Entry_Key)"contactDepth-bottom", 0  );
        self->contactDepth[1][1] = Stg_ComponentFactory_GetInt( cf, self->name, (Dictionary_Entry_Key)"contactDepth-top", 0  );
        self->contactDepth[2][0] = Stg_ComponentFactory_GetInt( cf, self->name, (Dictionary_Entry_Key)"contactDepth-back", 0  );
        self->contactDepth[2][1] = Stg_ComponentFactory_GetInt( cf, self->name, (Dictionary_Entry_Key)"contactDepth-front", 0  );
        self->contactGeom[0] = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"contactGeometry-x", 0.0  );
        self->contactGeom[1] = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"contactGeometry-y", 0.0  );
        self->contactGeom[2] = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"contactGeometry-z", 0.0  );

	/* Read and set shadow depth. */
	shadowDepth = Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"shadowDepth", 1  );
	IrregularTriGenerator_SetShadowDepth( self, shadowDepth );

	/* Read a general dictionary flag for which processor to watch. */
	stream = Journal_Register( Info_Type, (Name)self->type  );
	Stream_SetPrintingRank( stream, Dictionary_GetUnsignedInt_WithDefault( cf->rootDict, "rankToWatch", 0 ) );

	/* file name prefix for the mesh topology files */
	self->filename = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"meshTopologyFilePrefix", ""  );

	/* required for parallel implementation */
	self->referenceMesh = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"ReferenceMesh", FeMesh, False, NULL );

	/* Free stuff. */
	if( minList ) FreeArray( minDecomp );
	if( maxList ) FreeArray( maxDecomp  );
}

void _IrregularTriGenerator_Build( void* meshGenerator, void* data ) {
	IrregularTriGenerator*	self = (IrregularTriGenerator*)meshGenerator;

	_MeshGenerator_Build( meshGenerator, data );

	self->incVerts = IArray_New();
	self->incEls = IArray_New();
}

void _IrregularTriGenerator_Initialise( void* meshGenerator, void* data ) {
}

void _IrregularTriGenerator_Execute( void* meshGenerator, void* data ) {
}

void _IrregularTriGenerator_Destroy( void* meshGenerator, void* data ) {
	IrregularTriGenerator*	self = (IrregularTriGenerator*)meshGenerator;

	IrregularTriGenerator_Destruct( self );
	//Memory_Free( self->filename );
   _MeshGenerator_Destroy( self, data );
}

void IrregularTriGenerator_SetDimSize( void* meshGenerator, unsigned nDims ) {
	IrregularTriGenerator*	self = (IrregularTriGenerator*)meshGenerator;

	assert( self && Stg_CheckType( self, IrregularTriGenerator ) );

	_MeshGenerator_SetDimSize( self, nDims );

	self->minDecomp = MemRearray( self->minDecomp, unsigned, self->nDims, IrregularTriGenerator_Type );
	memset( self->minDecomp, 0, nDims * sizeof(unsigned) );
	self->maxDecomp = MemRearray( self->maxDecomp, unsigned, self->nDims, IrregularTriGenerator_Type );
	memset( self->maxDecomp, 0, nDims * sizeof(unsigned) );
}

void IrregularTriGenerator_Generate( void* meshGenerator, void* _mesh, void* data ) {
	IrregularTriGenerator*	self 	= (IrregularTriGenerator*)meshGenerator;
	Stream*			stream	= Journal_Register( Info_Type, (Name)self->type );
	Mesh*			mesh 	= (Mesh*)_mesh;

	/* Sanity check */
	assert( self );
	assert( mesh  );

	Journal_Printf( stream, "Irregular Mesh Generator: '%s'\n", self->name );
	Stream_Indent( stream );

	/* serial or parallel implementation */	
	if( self->referenceMesh ) {
		self->comm->mpiComm = MPI_COMM_WORLD;
	}
	else {
		self->comm->mpiComm = MPI_COMM_SELF;
	}

	/* Fill topological values. */
	MeshTopology_SetComm( mesh->topo, self->comm );
	MeshTopology_SetNumDims( mesh->topo, 2/*TODO*/ );
	if( self->referenceMesh ) {
		IrregularTriGenerator_GenTopoAndGeom_Parallel( self, mesh, (IGraph*)mesh->topo );
	}
	else {
		IrregularTriGenerator_GenTopoAndGeom( self, mesh, (IGraph*)mesh->topo );
	}
	
	/* Fill element types. */
	IrregularTriGenerator_GenElementTypes( self, mesh );

	/* hack! add a dummy element grid so the context can dump the mesh without problems (as the finite element context assumes a 
         * cartesian topology. at some point will need to generalise the mesh writing routine to account for irregular tris */
	unsigned sizes[2] = { 1, 1 };
	Grid** grid;
	ExtensionManager_Add( mesh->info, (Name)"elementGrid", sizeof(Grid*) );
	grid = (Grid** )ExtensionManager_Get( mesh->info, mesh, ExtensionManager_GetHandle( mesh->info, (Name)"elementGrid" ) );
	*grid = Grid_New( );
	Grid_SetNumDims( *grid, 2 );
	Grid_SetSizes( *grid, sizes );

	Stream_UnIndent( stream );
}

void _IrregularTriGenerator_SetTopologyParams( void* meshGenerator, unsigned maxDecompDims, unsigned* minDecomp, unsigned* maxDecomp ) {
	IrregularTriGenerator*	self = (IrregularTriGenerator*)meshGenerator;
	unsigned		d_i;

	/* Sanity check. */
	assert( self && Stg_CheckType( self, IrregularTriGenerator ) );
	assert( self->nDims <= 3 );

	/* Kill everything we have, topologically. */
	IrregularTriGenerator_DestructTopology( self );

	/* Don't continue if we have nothing. */
	if( !self->nDims )
		return;

	if( minDecomp )
		memcpy( self->minDecomp, minDecomp, self->nDims * sizeof(unsigned) );
	else
		memset( self->minDecomp, 0, self->nDims * sizeof(unsigned) );
	if( maxDecomp )
		memcpy( self->maxDecomp, maxDecomp, self->nDims * sizeof(unsigned) );
	else
		memset( self->maxDecomp, 0, self->nDims * sizeof(unsigned) );
	self->maxDecompDims = maxDecompDims;

	/* As soon as we know the topology, we can decompose. */
	/*IrregularTriGenerator_BuildDecomp( self );*/
	Stg_Class_RemoveRef( self->comm );
	self->comm = Comm_New();
	Stg_Class_AddRef( self->comm );
	Comm_SetMPIComm( self->comm, self->mpiComm );
}

void _IrregularTriGenerator_GenFaces( void* meshGenerator, IGraph* topo ) {
	IrregularTriGenerator*	self = (IrregularTriGenerator*)meshGenerator;

	/* TODO */
}

/* irregular triangular elements */
#define VERTS_PER_EL 3

void _IrregularTriGenerator_GenElementTypes( void* meshGenerator, Mesh* mesh ) {
	IrregularTriGenerator*	self = (IrregularTriGenerator*)meshGenerator;
	Stream*			stream;
	unsigned		nDomainEls;
	unsigned		e_i;

	assert( self && Stg_CheckType( self, IrregularTriGenerator ) );

	stream = Journal_Register( Info_Type, (Name)self->type  );
	Journal_Printf( stream, "Generating element types...\n" );
	Stream_Indent( stream );

	mesh->nElTypes = 1;
	mesh->elTypes = Memory_Alloc_Array( Mesh_ElementType*, mesh->nElTypes, "Mesh::elTypes" );
	//mesh->elTypes[0] = (Mesh_ElementType*)Mesh_HexType_New();
	mesh->elTypes[0] = (Mesh_ElementType*)Mesh_SimplexType_New();
	Mesh_ElementType_SetMesh( mesh->elTypes[0], mesh );
	nDomainEls = Mesh_GetDomainSize( mesh, Mesh_GetDimSize( mesh ) );
	mesh->elTypeMap = Memory_Alloc_Array( unsigned, nDomainEls, "Mesh::elTypeMap" );
	for( e_i = 0; e_i < nDomainEls; e_i++ )
		mesh->elTypeMap[e_i] = 0;

	//Mesh_SetAlgorithms( mesh, Mesh_Algorithms_New( "", NULL ) );
	Mesh_SetAlgorithms( mesh, Mesh_SimplexAlgorithms_New( "", NULL ) );

	MPI_Barrier( self->mpiComm );
	Journal_Printf( stream, "... element types are '%s',\n", mesh->elTypes[0]->type );
	Journal_Printf( stream, "... mesh algorithm type is '%s',\n", mesh->algorithms->type );
	Journal_Printf( stream, "... done.\n" );
	Stream_UnIndent( stream );
}

/*--------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/

void IrregularTriGenerator_SetShadowDepth( void* meshGenerator, unsigned depth ) {
	IrregularTriGenerator*	self = (IrregularTriGenerator*)meshGenerator;

	/* Sanity check. */
	assert( self );

	self->shadowDepth = depth;
}

/*----------------------------------------------------------------------------------------------------------------------------------
** Private Functions
*/

typedef double Coord2D[2];

/* generate the topology from a set of triangle files (.node, .ele & .edge) */
void IrregularTriGenerator_GenTopoAndGeom( IrregularTriGenerator* self, Mesh* mesh, IGraph* topo ) {
	Comm*		comm;
	FILE*		fp;
	int		numNodes, numTris, numEdges;
	double		**nodes;
	TriIndex	*tris;
	EdgeIndex	*edges;
	unsigned	*vertInds, *elInds, *edgeInds;
	int		i, j;
	Sync*		sync;
	int 		rank, size;

	MPI_Comm_rank( self->mpiComm, &rank );
	MPI_Comm_size( self->mpiComm, &size );

	/* make sure the processors read the files sequentially */
	for( i = 0; i < size; i++ ) {
		if( i == rank ) {
			nodes 	= ReadNodeFile( self->filename, &numNodes );
			tris	= ReadTriFile( self->filename, &numTris );
			edges	= ReadEdgeFile( self->filename, &numEdges );
		}
		MPI_Barrier( self->mpiComm );
	}
	MPI_Barrier( self->mpiComm );
	
	/* generate the vertices */
	vertInds = Memory_Alloc_Array( unsigned, numNodes, "mesh vertices" );
	for( i = 0; i < numNodes; i++ ) {
		vertInds[i] = i;
	}
	IGraph_SetElements( topo, MT_VERTEX, numNodes, vertInds );
	FreeArray( vertInds );
	MPI_Barrier( self->mpiComm );

	/* generate the elements */
	elInds = Memory_Alloc_Array( unsigned, numTris, "mesh elements" );
	for( i = 0; i < numTris; i++ ) {
		elInds[i] = i;
	}
	IGraph_SetElements( topo, topo->nDims, numTris, elInds ); /* topo->nDims = 2 for surface of 3D volume */
	FreeArray( elInds );
	MPI_Barrier( self->mpiComm );

	/* generate the edges */
	edgeInds = Memory_Alloc_Array( unsigned, numEdges, "mesh edges" );
	for( i = 0; i < numEdges; i++ ) {
		edgeInds[i] = i;
	}
	IGraph_SetElements( topo, MT_EDGE, numEdges, edgeInds );
	FreeArray( edgeInds );
	MPI_Barrier( self->mpiComm );

	/* generate the geometry for the nodes */
	sync = (Sync*)IGraph_GetDomain( topo, 0 );
	mesh->verts = AllocNamedArray2D( double, Sync_GetNumDomains( sync ), topo->nDims, "Mesh::verts" );

	for( i = 0; i < numNodes; i++ ) {
		mesh->verts[i][0] = nodes[i][0];
		mesh->verts[i][1] = nodes[i][1];
	}
	MPI_Barrier( self->mpiComm );

	/* generate the element-vertex incidence relationships */	
	for( i = 0; i < numTris; i++ ) {
		IGraph_SetIncidence( topo, topo->nDims, i, MT_VERTEX, VERTS_PER_EL, tris[i] );
	}
	MPI_Barrier( self->mpiComm );

	/* generate the edge-vertex incidence relationships... first those derived from the triangle edges */
	for( i = 0; i < numEdges; i++ ) {
		IGraph_SetIncidence( topo, MT_EDGE, i, MT_VERTEX, 2, edges[i] );
	}
	MPI_Barrier( self->mpiComm );

	/* Set the shadow depth and correct incidence. */
	comm = MeshTopology_GetComm( topo );
	if( self->shadowDepth && Comm_GetNumNeighbours( comm ) > 0 ) {
		/* Build enough incidence to set shadow depth. */
		IGraph_InvertIncidence( topo, MT_VERTEX, topo->nDims );
		IGraph_ExpandIncidence( topo, topo->nDims );

		/* Set the shadow depth. */
		MeshTopology_SetShadowDepth( topo, self->shadowDepth );

		/* Kill up relations and neighbours. */
		IGraph_RemoveIncidence( topo, topo->nDims, topo->nDims );
		IGraph_RemoveIncidence( topo, 0, topo->nDims );
	}
	MPI_Barrier( self->mpiComm );

	/* inverse of these relationships */
	IGraph_InvertIncidence( topo, MT_VERTEX, MT_EDGE );
	IGraph_InvertIncidence( topo, MT_VERTEX, topo->nDims );
	IGraph_ExpandIncidence( topo, MT_FACE );
	MPI_Barrier( self->mpiComm );

	IrregularTriGenerator_CompleteVertexNeighbours( self, topo );
	MPI_Barrier( self->mpiComm );

	IrregularTriGenerator_GenBndVerts( self, topo );	
	MPI_Barrier( self->mpiComm );

	Memory_Free( nodes );
	Memory_Free( tris );
	Memory_Free( edges );
}

void IrregularTriGenerator_GenBndVerts( IrregularTriGenerator* self, IGraph* topo ) {
	unsigned	numVerts 		= Decomp_GetNumLocals( Sync_GetDecomp( IGraph_GetDomain( topo, MT_VERTEX ) ) );
	IArray		boundaryVertsObj, *boundaryVerts = &boundaryVertsObj;
	const int*	boundaryVertsPtr;
	unsigned	numBoundaryVerts;
	unsigned	numIncEls;
	IArray*		incEls			= IArray_New();
	unsigned*	incElsPtr;
	unsigned	vert_i, el_i, elVertIndex, i, j;
	unsigned**	edgeList;
	IArray*		elVerts			= IArray_New();
	unsigned*	elVertsPtr;
	Bool		isBndVert;

	IArray_Init( boundaryVerts );

	for( vert_i = 0; vert_i < numVerts; vert_i++ ) {
		/* get the (2D) elements which are incident on this vertex */
		numIncEls = IGraph_GetIncidenceSize( topo, MT_VERTEX, vert_i, 2 );
		_IGraph_GetIncidence( topo, MT_VERTEX, vert_i, 2, incEls );
		incElsPtr = IArray_GetPtr( incEls );

		edgeList = Memory_Alloc_2DArray_Unnamed( unsigned, numIncEls, 2 );

		for( el_i = 0; el_i < numIncEls; el_i++ ) {
			/* get the vertices which are incident on the ith element */
			_IGraph_GetIncidence( topo, 2, incElsPtr[el_i], 0, elVerts );
			elVertsPtr = IArray_GetPtr( elVerts );

			/* find the index of the current vertex in the list of element vertices */
			for( elVertIndex = 0; elVertIndex < VERTS_PER_EL; elVertIndex++ ) {
				if( elVertsPtr[elVertIndex] == vert_i )
					break;
			}
			/* assign the vertices which together with the current vertex constitute a valid edge for this element */
			edgeList[el_i][0] = elVertsPtr[(elVertIndex+1)%VERTS_PER_EL];
			edgeList[el_i][1] = elVertsPtr[(elVertIndex+2)%VERTS_PER_EL];
		}

		/* test to see if a given edge is incident on two different elements */
		isBndVert = False;
		for( i = 0; i < numIncEls; i++ ) {
			for( j = 0; j < numIncEls; j++ ) {
				if( j == i )
					continue;

				/* edge is incident on two different elements - is NOT a boundary! */
				if( edgeList[i][0] == edgeList[j][1] )
					break;
			}
			/* no other element shares this edge - is a boundary! */
			if( j == numIncEls ) {
				isBndVert = True;
				break;
			}
		}
	
		if( isBndVert )
			IArray_Append( boundaryVerts, vert_i );	
			
		Memory_Free( edgeList );
	}

	IArray_GetArray( boundaryVerts, &numBoundaryVerts, &boundaryVertsPtr );
	IGraph_SetBoundaryElements( topo, MT_VERTEX, numBoundaryVerts, boundaryVertsPtr );
	IArray_Destruct( boundaryVerts );
}

/* find the neighbouring vertices for a given vertex and store them in a topology */
void IrregularTriGenerator_CompleteVertexNeighbours( IrregularTriGenerator* self, IGraph* topo ) {
	unsigned	numVerts 		= Decomp_GetNumLocals( Sync_GetDecomp( IGraph_GetDomain( topo, MT_VERTEX ) ) );
	IArray*		incEdges		= IArray_New();
	IArray*		incVerts		= IArray_New();
	unsigned	*incEdgesPtr, *incVertsPtr;
	unsigned	*verts;
	unsigned	numIncEdges;
	unsigned	vert_i, incEdge_j;

	for( vert_i = 0; vert_i < numVerts; vert_i++ ) {
		/* get the edges incident on a the ith node */
		numIncEdges = IGraph_GetIncidenceSize( topo, MT_VERTEX, vert_i, MT_EDGE );
		verts = Memory_Alloc_Array_Unnamed( unsigned, numIncEdges );
		_IGraph_GetIncidence( topo, MT_VERTEX, vert_i, MT_EDGE, incEdges );
		incEdgesPtr = IArray_GetPtr( incEdges );
		for( incEdge_j = 0; incEdge_j < numIncEdges; incEdge_j++ ) {
			/* get the vertices incident on the jth edge */
			_IGraph_GetIncidence( topo, MT_EDGE, incEdgesPtr[incEdge_j], MT_VERTEX, incVerts );
			incVertsPtr = IArray_GetPtr( incVerts );
			verts[incEdge_j] = ( incVertsPtr[0] == vert_i ) ? incVertsPtr[1] : incVertsPtr[0];
		}

		IGraph_SetIncidence( topo, MT_VERTEX, vert_i, MT_VERTEX, numIncEdges, verts );
		Memory_Free( verts );
	}

	Stg_Class_Delete( incEdges );
	Stg_Class_Delete( incVerts );

	MPI_Barrier( self->mpiComm );
}

void IrregularTriGenerator_Destruct( IrregularTriGenerator* self ) {
	assert( self );

	IrregularTriGenerator_DestructTopology( self );
	IrregularTriGenerator_DestructGeometry( self );
	KillArray( self->minDecomp );
	KillArray( self->maxDecomp );
	Stg_Class_RemoveRef( self->comm );

	Stg_Class_Delete( self->incVerts );
	Stg_Class_Delete( self->incEls );
}

void IrregularTriGenerator_DestructTopology( IrregularTriGenerator* self ) {
	assert( self );

	self->maxDecompDims = 0;
	memset( self->minDecomp, 0, self->nDims * sizeof(unsigned) );
	memset( self->maxDecomp, 0, self->nDims * sizeof(unsigned) );
}

void IrregularTriGenerator_DestructGeometry( IrregularTriGenerator* self ) {
	assert( self );
}

/* parallel implementation of the topology and geometry generator - only reads in nodes which are part of the local domain */
void IrregularTriGenerator_GenTopoAndGeom_Parallel( IrregularTriGenerator* self, Mesh* mesh, IGraph* topo ) {
	Comm*		comm;
	FILE*		fp;
	double		**nodes;
	TriIndex	*tris;
	EdgeIndex	*edges;
	unsigned	*vertInds, *elInds, *edgeInds;
	unsigned	elVerts[VERTS_PER_EL], edgeVerts[2];
	int		i, j;
	Sync*		sync;
	int 		rank, size;
	double		min[3], max[3];
	char*		filename;
	unsigned	numNodesGlobal, numNodesLocal;
	unsigned	numTrisGlobal, numTrisLocal;
	unsigned	numEdgesGlobal, numEdgesLocal;
	int*		nodeMap;
	unsigned	localNode_i, localTri_i, localEdge_i;
	int		dummy_int[3];
	double		dummy_double;
	double		coord[2];
	unsigned	tri[3];
	unsigned	edge[2];
	unsigned	*numNodesPerProc, localOffset, *numEdgesPerProc, *numElsPerProc;

	MPI_Comm_rank( self->mpiComm, &rank );
	MPI_Comm_size( self->mpiComm, &size );

	Mesh_GetDomainCoordRange( self->referenceMesh, min, max );

	/* read in the nodal values for the local domain */
	filename = Memory_Alloc_Array( char, strlen( self->filename ) + 6, "triangle node file" );
	sprintf( filename, "%s.node", self->filename );
	fp = fopen( filename, "r" );
	fscanf( fp, "%u  %d  %d  %d", &numNodesGlobal, dummy_int, dummy_int + 1, dummy_int + 2, dummy_int + 3 );
	nodeMap = Memory_Alloc_Array( int, numNodesGlobal, "global to local node map" );
	memset( nodeMap, -1, numNodesGlobal*sizeof( int ) );
	numNodesLocal = 0;
	for( i = 0; i < numNodesGlobal; i++ ) {
		fscanf( fp, "%d %lf %lf %lf %d", dummy_int, coord, coord + 1, &dummy_double, dummy_int + 1 );
		if( coord[0] < min[I_AXIS] || coord[0] > max[I_AXIS] || coord[1] < min[K_AXIS] || coord[1] > max[K_AXIS] )
			continue;

		numNodesLocal++;
	}

	nodes = Memory_Alloc_2DArray( double, numNodesLocal, 2, (Name)"triangle nodes" );
	rewind( fp  );
	fscanf( fp, "%u  %d  %d  %d", &numNodesGlobal, dummy_int, dummy_int + 1, dummy_int + 2, dummy_int + 3 );
	localNode_i = 0;
	for( i = 0; i < numNodesGlobal; i++ ) {
		fscanf( fp, "%d %lf %lf %lf %d", dummy_int, coord, coord + 1, &dummy_double, dummy_int + 1 );
		if( coord[0] < min[I_AXIS] || coord[0] > max[I_AXIS] || coord[1] < min[K_AXIS] || coord[1] > max[K_AXIS] )
			continue;

		nodes[localNode_i][0] = coord[0];
		nodes[localNode_i][1] = coord[1];
		nodeMap[i] = localNode_i++;
	}
	fclose( fp );
	Memory_Free( filename );

	/* read in the edge topology for the local domain */
	filename = Memory_Alloc_Array( char, strlen( self->filename ) + 6, "triangle edges file" );
	sprintf( filename, "%s.edge", self->filename );
	fp = fopen( filename, "r" );
	fscanf( fp, "%u  %d", &numEdgesGlobal, dummy_int );
	numEdgesLocal = 0;
	for( i = 0; i < numEdgesGlobal; i++ ) {
		fscanf( fp, "%d  %u  %u  %d", dummy_int, edge, edge + 1, dummy_int + 1 );
		if( nodeMap[edge[0]] != -1 && nodeMap[edge[1]] != -1 )
			numEdgesLocal++;
	}

	edges = Memory_Alloc_Array( EdgeIndex, numEdgesLocal, "triangle edges" );
	rewind( fp );
	localEdge_i = 0;
	fscanf( fp, "%u  %d", &numEdgesGlobal, dummy_int );
	for( i = 0; i < numEdgesGlobal; i++ ) {
		fscanf( fp, "%d  %u  %u  %d", dummy_int, edge, edge + 1, dummy_int + 1 );
		if( nodeMap[edge[0]] != -1 && nodeMap[edge[1]] != -1 ) {
			edges[localEdge_i][0] = nodeMap[edge[0]];
			edges[localEdge_i][1] = nodeMap[edge[1]];
			localEdge_i++;
		}
	}
	fclose( fp );
	Memory_Free( filename );

	/* read the triangle topology for the local domain */
	filename = Memory_Alloc_Array( char, strlen( self->filename ) + 5, "triangle element file" );
	sprintf( filename, "%s.ele", self->filename );
	fp = fopen( filename, "r" );
	fscanf( fp, "%u  %d  %d", &numTrisGlobal, dummy_int, dummy_int + 1 );
	numTrisLocal = 0;
	for( i = 0; i < numTrisGlobal; i++ ) {
		fscanf( fp, "%d  %u  %u  %u", dummy_int, tri, tri + 1, tri + 2 );
		if( nodeMap[tri[0]] != -1 && nodeMap[tri[1]] != -1 && nodeMap[tri[2]] != -1 )
			numTrisLocal++;
	}

	tris = Memory_Alloc_Array( TriIndex, numTrisLocal, "triangle elements" );
	rewind( fp );
	localTri_i = 0;
	fscanf( fp, "%u  %d  %d", &numTrisGlobal, dummy_int, dummy_int + 1 );
	for( i = 0; i < numTrisGlobal; i++ ) {
		fscanf( fp, "%d  %u  %u  %u", dummy_int, tri, tri + 1, tri + 2 );
		if( nodeMap[tri[0]] != -1 && nodeMap[tri[1]] != -1 && nodeMap[tri[2]] != -1 ) {
			tris[localTri_i][0] = nodeMap[tri[0]];
			tris[localTri_i][1] = nodeMap[tri[1]];
			tris[localTri_i][2] = nodeMap[tri[2]];
			localTri_i++;
		}
	}
	fclose( fp );
	Memory_Free( filename );

	MPI_Barrier( self->mpiComm );
	
	/* generate the vertices - for parallel implementation this indices must correspond to the GLOBAL node indices */
	numNodesPerProc = Memory_Alloc_Array( unsigned, size, "number of nodes local to each processor" );
	numNodesPerProc[rank] = numNodesLocal;
	for( i = 0; i < size; i++ ) {
		MPI_Bcast( &numNodesPerProc[i], 1, MPI_UNSIGNED, i, self->mpiComm );
	}
	vertInds = Memory_Alloc_Array( unsigned, numNodesLocal, "mesh vertices" );
	localOffset = 0;
	for( i = 0; i < rank; i++ ) {
		localOffset += numNodesPerProc[i];
	}
	for( i = 0; i < numNodesLocal; i++ ) {
		vertInds[i] = i + localOffset;
	}
	IGraph_SetElements( topo, MT_VERTEX, numNodesLocal, vertInds );
	FreeArray( vertInds );
	FreeArray( numNodesPerProc );
	MPI_Barrier( self->mpiComm );

	/* generate the elements */
	numElsPerProc = Memory_Alloc_Array( unsigned, size, "number of elements local to each processor" );
	numElsPerProc[rank] = numTrisLocal;
	for( i = 0; i < size; i++ ) {
		MPI_Bcast( &numElsPerProc[i], 1, MPI_UNSIGNED, i, self->mpiComm );
	}
	elInds = Memory_Alloc_Array( unsigned, numTrisLocal, "mesh elements" );
	localOffset = 0;
	for( i = 0; i < rank; i++ ) {
		localOffset += numElsPerProc[i];
	}
	for( i = 0; i < numTrisLocal; i++ ) {
		elInds[i] = i + localOffset;
	}
	IGraph_SetElements( topo, topo->nDims, numTrisLocal, elInds ); /* topo->nDims = 2 for surface of 3D volume */
	FreeArray( elInds );
	FreeArray( numElsPerProc );
	MPI_Barrier( self->mpiComm );

	/* generate the edges */
	numEdgesPerProc = Memory_Alloc_Array( unsigned, size, "number of edges local to each processor" );
	numEdgesPerProc[rank] = numEdgesLocal;
	for( i = 0; i < size; i++ ) {
		MPI_Bcast( &numEdgesPerProc[i], 1, MPI_UNSIGNED, i, self->mpiComm );
	}
	edgeInds = Memory_Alloc_Array( unsigned, numEdgesLocal, "mesh edges" );
	localOffset = 0;
	for( i = 0; i < rank; i++ ) {
		localOffset += numEdgesPerProc[i];
	}
	for( i = 0; i < numEdgesLocal; i++ ) {
		edgeInds[i] = i + localOffset;
	}
	IGraph_SetElements( topo, MT_EDGE, numEdgesLocal, edgeInds );
	FreeArray( edgeInds );
	FreeArray( numEdgesPerProc );
	MPI_Barrier( self->mpiComm );

	/* generate the geometry for the nodes */
	sync = (Sync*)IGraph_GetDomain( topo, 0 );
	mesh->verts = AllocNamedArray2D( double, Sync_GetNumDomains( sync ), topo->nDims, "Mesh::verts" );

	for( i = 0; i < numNodesLocal; i++ ) {
		mesh->verts[i][0] = nodes[i][0];
		mesh->verts[i][1] = nodes[i][1];
	}
	MPI_Barrier( self->mpiComm );

	/* generate the element-vertex incidence relationships */	
	for( i = 0; i < numTrisLocal; i++ ) {
		IGraph_SetIncidence( topo, topo->nDims, i, MT_VERTEX, VERTS_PER_EL, tris[i] );
	}
	MPI_Barrier( self->mpiComm );

	/* generate the edge-vertex incidence relationships... first those derived from the triangle edges */
	for( i = 0; i < numEdgesLocal; i++ ) {
		IGraph_SetIncidence( topo, MT_EDGE, i, MT_VERTEX, 2, edges[i] );
	}
	MPI_Barrier( self->mpiComm );

	/* Set the shadow depth and correct incidence. */
	comm = MeshTopology_GetComm( topo );
	if( self->shadowDepth && Comm_GetNumNeighbours( comm ) > 0 ) {
		/* Build enough incidence to set shadow depth. */
		IGraph_InvertIncidence( topo, MT_VERTEX, topo->nDims );
		IGraph_ExpandIncidence( topo, topo->nDims );

		/* Set the shadow depth. */
		MeshTopology_SetShadowDepth( topo, self->shadowDepth );

		/* Kill up relations and neighbours. */
		IGraph_RemoveIncidence( topo, topo->nDims, topo->nDims );
		IGraph_RemoveIncidence( topo, 0, topo->nDims );
	}
	MPI_Barrier( self->mpiComm );

	/* inverse of these relationships */
	IGraph_InvertIncidence( topo, MT_VERTEX, MT_EDGE );
	IGraph_InvertIncidence( topo, MT_VERTEX, topo->nDims );
	MPI_Barrier( self->mpiComm );

	IrregularTriGenerator_CompleteVertexNeighbours( self, topo );
	MPI_Barrier( self->mpiComm );

	IrregularTriGenerator_GenBndVerts( self, topo );	
	MPI_Barrier( self->mpiComm );

	Memory_Free( nodes );
	Memory_Free( tris );
	Memory_Free( edges );
	Memory_Free( nodeMap );
}

#define MAX_ITERATIONS 100
#define TOLERANCE 0.0001

double N_0( double coord[2] ) { return coord[0]; };
double N_1( double coord[2] ) { return coord[1]; };
double N_2( double coord[2] ) { return 1.0 - coord[0] - coord[1]; }
double GN_x0( double coord[2] ) { return 1.0; };
double GN_y0( double coord[2] ) { return 0.0; };
double GN_x1( double coord[2] ) { return 0.0; };
double GN_y1( double coord[2] ) { return 1.0; };
double GN_x2( double coord[2] ) { return -1.0; };
double GN_y2( double coord[2] ) { return -1.0; };

double IrregularTriGenerator_InterpolateValueAt( void* generator, FeVariable* feVariable, double* gCoord ) {
	IrregularTriGenerator*	self 		= (IrregularTriGenerator*)generator;
	unsigned 		element;
	unsigned		nearestVertex;
	double			height;

	if( Mesh_SearchElements( feVariable->feMesh, gCoord, &element ) ) {
        	height = IrregularTriGenerator_InterpolateWithinElement( feVariable->feMesh->generator, feVariable, element, gCoord );
	}
	else {
		nearestVertex = Mesh_NearestVertex( feVariable->feMesh, gCoord );
		FeVariable_GetValueAtNode( feVariable, nearestVertex, &height );
	}

	return height;
}

double IrregularTriGenerator_InterpolateWithinElement( void* generator, FeVariable* feVariable, unsigned element, double* gCoord ) {
	IrregularTriGenerator*	self 		= (IrregularTriGenerator*)generator;
	FeMesh*			mesh		= feVariable->feMesh; /* this must be the same mesh as that for the generator! */
	double			lCoord[2]	= { 0.33333333, 0.33333333 }; /* initial guess at centre of element */
	unsigned		nInc, *inc;
	unsigned		dim		= Mesh_GetDimSize( mesh );
	unsigned		iter_i, node_j;
	unsigned		k, l;
	double			J[2][2];
	double			rhs[2];
	double			N_i[3], GN_xi[2][3];
	double*			nCoord;
	double			detJinv;
	double			deltax[2], delta_max;
	double			phi, phi_j;

	/* first, convert the global coord to an element local one */
	Mesh_GetIncidence( mesh, dim, element, MT_VERTEX, self->incVerts );
	nInc = IArray_GetSize( self->incVerts );
	inc = IArray_GetPtr( self->incVerts );
	if( nInc != 3 || dim != 2 ) {
		printf( "ERROR in Irregular Triangular Mesh interpolation.\n" );
		abort();
	}

	for( iter_i = 0; iter_i < MAX_ITERATIONS; iter_i++ ) {
		/* evaluate the shape functions for the solution for the local coord at the ith iterate */
		N_i[0] = N_0( lCoord );	N_i[1] = N_1( lCoord );	N_i[2] = N_2( lCoord );

		/* ...and their gradients */
		GN_xi[0][0] = GN_x0( lCoord );	GN_xi[1][0] = GN_y0( lCoord );
		GN_xi[0][1] = GN_x1( lCoord );	GN_xi[1][1] = GN_y1( lCoord );
		GN_xi[0][2] = GN_x2( lCoord );	GN_xi[1][2] = GN_y2( lCoord );

		/* 1: evaluate the jacobian, J_ij = X_iA*(\del N_A(x)/\del x_j); where X is the global coord and x the local coord */
		memset( J, 0, dim*dim*sizeof(double) );
		for( node_j = 0; node_j < nInc; node_j++ ) {
			nCoord = mesh->verts[inc[node_j]];
			J[0][0] += nCoord[0]*GN_xi[0][node_j];
			J[0][1] += nCoord[0]*GN_xi[1][node_j];
			J[1][0] += nCoord[1]*GN_xi[0][node_j];
			J[1][1] += nCoord[1]*GN_xi[1][node_j];
		}

		/* 2: evaluate the residual, F_i(x) = X_iA*N_A(x) - X_i */
		memset( rhs, 0, dim*sizeof(double) );
		for( node_j = 0; node_j < nInc; node_j++ ) {
			nCoord = mesh->verts[inc[node_j]];
			rhs[0] -= nCoord[0]*N_i[node_j];
			rhs[1] -= nCoord[1]*N_i[node_j];
		}
		rhs[0] += gCoord[0];
		rhs[1] += gCoord[1];

		/* 3: evaluate the estimation for the local coordinate at the next iterate, \delta x = [J_ij(x_n)^-1]*F_i(x^n) */
		detJinv = 1.0/(J[0][0]*J[1][1] - J[0][1]*J[1][0]);
		deltax[0] = detJinv*( rhs[0]*J[1][1] - rhs[1]*J[0][1] );
		deltax[1] = detJinv*( rhs[1]*J[0][0] - rhs[0]*J[1][0] );

		/* 4: update the estimate of x^n */
		lCoord[0] += deltax[0];
		lCoord[1] += deltax[1];

		/* 5: termination condition */
		delta_max = ( fabs(deltax[0]) > fabs(deltax[1]) ) ? fabs( deltax[0] ) : fabs( deltax[1] );
		if( delta_max < TOLERANCE )
			break;
	}

double test[2] = {0.0,0.0};
N_i[0] = N_0( lCoord );	N_i[1] = N_1( lCoord );	N_i[2] = N_2( lCoord );
for( node_j = 0; node_j < nInc; node_j++ ) {
  nCoord = mesh->verts[inc[node_j]];
  test[0] += N_i[node_j]*nCoord[0];
  test[1] += N_i[node_j]*nCoord[1];
}
if( fabs( gCoord[0] - test[0] ) > 0.001 || fabs( gCoord[1] - test[1] ) > 0.001 )
  printf( "local coordinate error\n" );

	/* now that we've got the local coordinate, interpolate the height here using the shape functions */
	phi = 0.0;
	N_i[0] = N_0( lCoord );	N_i[1] = N_1( lCoord );	N_i[2] = N_2( lCoord );
	for( node_j = 0; node_j < nInc; node_j++ ) {
		FeVariable_GetValueAtNode( feVariable, inc[node_j], &phi_j );
		phi += phi_j*N_i[node_j];
	}

	return phi;
}

/* this implementation assumes a 2-D planar mesh within a 2-D space for efficiency. for a 2-D planar mesh within
 * a 3-D space a seperate implementation is required, see http://www.blackpawn.com/texts/pointinpoly/default.html */
Bool IrregularTriGenerator_ElementHasPoint( void* generator, Mesh* mesh, unsigned element, double* point ) {
	IrregularTriGenerator*	self 		= (IrregularTriGenerator*)generator;
	unsigned		nInc, *inc;
	double			v_ab[2], v_bc[2], v_ca[2];
	double			v_ap[2], v_bp[2], v_cp[2];

	Mesh_GetIncidence( mesh, MT_FACE, element, MT_VERTEX, self->incVerts );
	nInc = IArray_GetSize( self->incVerts );
	inc = IArray_GetPtr( self->incVerts );

	v_ab[0] = mesh->verts[inc[1]][0] - mesh->verts[inc[0]][0];
	v_ab[1] = mesh->verts[inc[1]][1] - mesh->verts[inc[0]][1];
	v_bc[0] = mesh->verts[inc[2]][0] - mesh->verts[inc[1]][0];
	v_bc[1] = mesh->verts[inc[2]][1] - mesh->verts[inc[1]][1];
	v_ca[0] = mesh->verts[inc[0]][0] - mesh->verts[inc[2]][0];
	v_ca[1] = mesh->verts[inc[0]][1] - mesh->verts[inc[2]][1];

	v_ap[0] = point[0] - mesh->verts[inc[0]][0];
	v_ap[1] = point[1] - mesh->verts[inc[0]][1];
	v_bp[0] = point[0] - mesh->verts[inc[1]][0];
	v_bp[1] = point[1] - mesh->verts[inc[1]][1];
	v_cp[0] = point[0] - mesh->verts[inc[2]][0];
	v_cp[1] = point[1] - mesh->verts[inc[2]][1];

	if( ( v_ab[0]*v_ap[1] - v_ab[1]*v_ap[0] )*( v_ab[1]*v_ca[0] - v_ab[0]*v_ca[1] ) < 0 )
		return False;

	if( ( v_bc[0]*v_bp[1] - v_bc[1]*v_bp[0] )*( v_bc[1]*v_ab[0] - v_bc[0]*v_ab[1] ) < 0 )
		return False;

	if( ( v_ca[0]*v_cp[1] - v_ca[1]*v_cp[0] )*( v_ca[1]*v_bc[0] - v_ca[0]*v_bc[1] ) < 0 )
		return False;

	return True;
}

Bool IrregularTriGenerator_SearchElements( void* generator, Mesh* mesh, double* point, unsigned* element ) {
	IrregularTriGenerator*	self 		= (IrregularTriGenerator*)generator;
	unsigned		nInc, *inc;
	unsigned		nearestVertex;
	unsigned		el_i;
	unsigned		dim;

	nearestVertex = Mesh_NearestVertex( mesh, point );
	Mesh_GetIncidence( mesh, MT_VERTEX, nearestVertex, MT_FACE, self->incEls ); /* assumption!!! */
	nInc = IArray_GetSize( self->incEls );
	inc = IArray_GetPtr( self->incEls );

	*element = (unsigned)-1;

	for( el_i = 0; el_i < nInc; el_i++ ) {
		//if( IrregularTriGenerator_ElementHasPoint( self, mesh, inc[el_i], point ) ) {
		if( Mesh_SimplexType_ElementHasPoint( mesh->elTypes[0], inc[el_i], point, &dim, element ) ) {
			//*element = inc[el_i];
			return True;
		}
	}

	return False; /* point is outside the domain */
}

