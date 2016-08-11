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

#define OFFSET_TAG 6

/* Textual name of this class */
const Type IrregularQuadGenerator_Type = (const Type)"IrregularQuadGenerator";

/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/

IrregularQuadGenerator* IrregularQuadGenerator_New( Name name ) {
	/* Variables set in this function */
	SizeT                                                    _sizeOfSelf = sizeof(IrregularQuadGenerator);
	Type                                                            type = IrregularQuadGenerator_Type;
	Stg_Class_DeleteFunction*                                    _delete = _IrregularQuadGenerator_Delete;
	Stg_Class_PrintFunction*                                      _print = _IrregularQuadGenerator_Print;
	Stg_Component_DefaultConstructorFunction*        _defaultConstructor = (void* (*)(Name))_IrregularQuadGenerator_New;
	Stg_Component_ConstructFunction*                          _construct = _IrregularQuadGenerator_AssignFromXML;
	Stg_Component_BuildFunction*                                  _build = _IrregularQuadGenerator_Build;
	Stg_Component_InitialiseFunction*                        _initialise = _IrregularQuadGenerator_Initialise;
	Stg_Component_ExecuteFunction*                              _execute = _IrregularQuadGenerator_Execute;
	Stg_Component_DestroyFunction*                              _destroy = _IrregularQuadGenerator_Destroy;
	AllocationType                                    nameAllocationType = NON_GLOBAL;
	MeshGenerator_SetDimSizeFunc*                         setDimSizeFunc = IrregularQuadGenerator_SetDimSize;
	MeshGenerator_GenerateFunc*                             generateFunc = IrregularQuadGenerator_Generate;
	IrregularQuadGenerator_SetTopologyParamsFunc*  setTopologyParamsFunc = _IrregularQuadGenerator_SetTopologyParams;
	IrregularQuadGenerator_GenElementTypesFunc*      genElementTypesFunc = _IrregularQuadGenerator_GenElementTypes;

	/* The following terms are parameters that have been passed into or defined in this function but are being set before being passed onto the parent */
	Stg_Class_CopyFunction*  _copy = NULL;

   IrregularQuadGenerator* self = _IrregularQuadGenerator_New(  IRREGULARQUADGENERATOR_PASSARGS  );

   	_MeshGenerator_Init( (MeshGenerator*)self, NULL/*context*/ );
	_IrregularQuadGenerator_Init( self );

    return self;
}

IrregularQuadGenerator* _IrregularQuadGenerator_New(  IRREGULARQUADGENERATOR_DEFARGS  ) {
	IrregularQuadGenerator* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(IrregularQuadGenerator) );
	self = (IrregularQuadGenerator*)_MeshGenerator_New(  MESHGENERATOR_PASSARGS  );

	/* Virtual info */
	self->setTopologyParamsFunc = setTopologyParamsFunc;
	self->genElementTypesFunc = genElementTypesFunc;

	return self;
}

void _IrregularQuadGenerator_Init( IrregularQuadGenerator* self ) {
	Stream*	stream;

	assert( self && Stg_CheckType( self, IrregularQuadGenerator ) );

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

void _IrregularQuadGenerator_Delete( void* meshGenerator ) {
	IrregularQuadGenerator*	self = (IrregularQuadGenerator*)meshGenerator;
	/* Delete the parent. */
	_MeshGenerator_Delete( self );
}

void _IrregularQuadGenerator_Print( void* meshGenerator, Stream* stream ) {
	IrregularQuadGenerator*	self = (IrregularQuadGenerator*)meshGenerator;
	
	/* Set the Journal for printing informations */
	Stream* meshGeneratorStream;
	meshGeneratorStream = Journal_Register( InfoStream_Type, (Name)"IrregularQuadGeneratorStream"  );

	assert( self && Stg_CheckType( self, IrregularQuadGenerator ) );

	/* Print parent */
	Journal_Printf( stream, "IrregularQuadGenerator (ptr): (%p)\n", self );
	_MeshGenerator_Print( self, stream );
}

void _IrregularQuadGenerator_AssignFromXML( void* meshGenerator, Stg_ComponentFactory* cf, void* data ) {
	IrregularQuadGenerator*	self = (IrregularQuadGenerator*)meshGenerator;
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
	
	assert( self && Stg_CheckType( self, IrregularQuadGenerator ) );
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
	IrregularQuadGenerator_SetTopologyParams( self, maxDecompDims, minDecomp, maxDecomp );

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
	IrregularQuadGenerator_SetShadowDepth( self, shadowDepth );

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

void _IrregularQuadGenerator_Build( void* meshGenerator, void* data ) {
	_MeshGenerator_Build( meshGenerator, data );
}

void _IrregularQuadGenerator_Initialise( void* meshGenerator, void* data ) {
}

void _IrregularQuadGenerator_Execute( void* meshGenerator, void* data ) {
}

void _IrregularQuadGenerator_Destroy( void* meshGenerator, void* data ) {
	IrregularQuadGenerator*	self = (IrregularQuadGenerator*)meshGenerator;

	IrregularQuadGenerator_Destruct( self );
	//Memory_Free( self->filename );
   _MeshGenerator_Destroy( self, data );
}

void IrregularQuadGenerator_SetDimSize( void* meshGenerator, unsigned nDims ) {
	IrregularQuadGenerator*	self = (IrregularQuadGenerator*)meshGenerator;

	assert( self && Stg_CheckType( self, IrregularQuadGenerator ) );

	_MeshGenerator_SetDimSize( self, nDims );

	self->minDecomp = MemRearray( self->minDecomp, unsigned, self->nDims, IrregularQuadGenerator_Type );
	memset( self->minDecomp, 0, nDims * sizeof(unsigned) );
	self->maxDecomp = MemRearray( self->maxDecomp, unsigned, self->nDims, IrregularQuadGenerator_Type );
	memset( self->maxDecomp, 0, nDims * sizeof(unsigned) );
}

void IrregularQuadGenerator_Generate( void* meshGenerator, void* _mesh, void* data ) {
	IrregularQuadGenerator*	self 	= (IrregularQuadGenerator*)meshGenerator;
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
		IrregularQuadGenerator_GenTopoAndGeom_Parallel( self, mesh, (IGraph*)mesh->topo );
	}
	else {
		IrregularQuadGenerator_GenTopoAndGeom( self, mesh, (IGraph*)mesh->topo );
	}
	
	/* Fill element types. */
	IrregularQuadGenerator_GenElementTypes( self, mesh );

	Stream_UnIndent( stream );
}

void _IrregularQuadGenerator_SetTopologyParams( void* meshGenerator, unsigned maxDecompDims, unsigned* minDecomp, unsigned* maxDecomp ) {
	IrregularQuadGenerator*	self = (IrregularQuadGenerator*)meshGenerator;
	unsigned		d_i;

	/* Sanity check. */
	assert( self && Stg_CheckType( self, IrregularQuadGenerator ) );
	assert( self->nDims <= 3 );

	/* Kill everything we have, topologically. */
	IrregularQuadGenerator_DestructTopology( self );

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
	/*IrregularQuadGenerator_BuildDecomp( self );*/
	Stg_Class_RemoveRef( self->comm );
	self->comm = Comm_New();
	Stg_Class_AddRef( self->comm );
	Comm_SetMPIComm( self->comm, self->mpiComm );
}

void _IrregularQuadGenerator_GenFaces( void* meshGenerator, IGraph* topo ) {
	IrregularQuadGenerator*	self = (IrregularQuadGenerator*)meshGenerator;

	/* TODO */
}

/* assuming bilinear element type, so 4 nodes per element */
#define VERTS_PER_EL 4

void _IrregularQuadGenerator_GenElementTypes( void* meshGenerator, Mesh* mesh ) {
	IrregularQuadGenerator*	self = (IrregularQuadGenerator*)meshGenerator;
	Stream*			stream;
	unsigned		nDomainEls;
	unsigned		e_i;

	assert( self && Stg_CheckType( self, IrregularQuadGenerator ) );

	stream = Journal_Register( Info_Type, (Name)self->type  );
	Journal_Printf( stream, "Generating element types...\n" );
	Stream_Indent( stream );

	mesh->nElTypes = 1;
	mesh->elTypes = Memory_Alloc_Array( Mesh_ElementType*, mesh->nElTypes, "Mesh::elTypes" );
	mesh->elTypes[0] = (Mesh_ElementType*)Mesh_HexType_New();
	Mesh_ElementType_SetMesh( mesh->elTypes[0], mesh );
	nDomainEls = Mesh_GetDomainSize( mesh, Mesh_GetDimSize( mesh ) );
	mesh->elTypeMap = Memory_Alloc_Array( unsigned, nDomainEls, "Mesh::elTypeMap" );
	for( e_i = 0; e_i < nDomainEls; e_i++ )
		mesh->elTypeMap[e_i] = 0;

	Mesh_SetAlgorithms( mesh, Mesh_Algorithms_New( "", NULL ) );

	MPI_Barrier( self->mpiComm );
	Journal_Printf( stream, "... element types are '%s',\n", mesh->elTypes[0]->type );
	Journal_Printf( stream, "... mesh algorithm type is '%s',\n", mesh->algorithms->type );
	Journal_Printf( stream, "... done.\n" );
	Stream_UnIndent( stream );
}

/*--------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/

void IrregularQuadGenerator_SetShadowDepth( void* meshGenerator, unsigned depth ) {
	IrregularQuadGenerator*	self = (IrregularQuadGenerator*)meshGenerator;

	/* Sanity check. */
	assert( self );

	self->shadowDepth = depth;
}

/*----------------------------------------------------------------------------------------------------------------------------------
** Private Functions
*/

typedef double Coord2D[2];

/* funcs to read the triangle output files */
double** ReadNodeFile( char* file_prefix, int* num_nodes ) {
	double**	nodes;
	char*		filename	= Memory_Alloc_Array_Unnamed( char, strlen( file_prefix ) + 6 );
	File*		file;
	double		dummy_double;
	int		dummy_int[3];
	int		i;

	sprintf( filename, "%s.node", file_prefix );
	file = CFile_NewRead( filename );
	Journal_Firewall(
		(long)file,
		Journal_Register( IrregularQuadGenerator_Type, Error_Type ),
		"File \"%s\" not found!\n",
		filename );
	fscanf( CFile_Ptr( file ), "%d  %d  %d  %d", num_nodes, dummy_int, dummy_int + 1, dummy_int + 2, dummy_int + 3 );	
	nodes = Memory_Alloc_2DArray_Unnamed( double, *num_nodes, 2 );

	for( i=0; i<*num_nodes; i++ )
		fscanf( CFile_Ptr( file ), "%d %lf %lf %lf %d", dummy_int, nodes[i], nodes[i] + 1, &dummy_double, dummy_int + 1 );

	Stg_Class_Delete( file );
	Memory_Free( filename );

	return nodes;
}

TriIndex* ReadTriFile( char* file_prefix, int* num_tris ) {
	TriIndex*	tris;
	char*		filename	= Memory_Alloc_Array_Unnamed( char, strlen( file_prefix ) + 5 );
	File*		file;
	int		dummy_int[2];
	int		i;

	sprintf( filename, "%s.ele", file_prefix );
	file = CFile_NewRead( filename );
	Journal_Firewall(
		(long)file,
		Journal_Register( IrregularQuadGenerator_Type, Error_Type ),
		"File \"%s\" not found!\n",
		filename );
	fscanf( CFile_Ptr( file ), "%d  %d  %d", num_tris, dummy_int, dummy_int + 1 );
	
	tris = Memory_Alloc_Array_Unnamed( TriIndex, *num_tris );
	for( i=0; i<*num_tris; i++ )
		fscanf( CFile_Ptr( file ), "%d  %d  %d  %d", dummy_int, tris[i], tris[i] + 1, tris[i] + 2 );

	Stg_Class_Delete( file );
	Memory_Free( filename );

	return tris;
}

EdgeIndex* ReadEdgeFile( char* file_prefix, int* num_edges ) {
	EdgeIndex*	edges;
	char*		filename	= Memory_Alloc_Array_Unnamed( char, strlen( file_prefix ) + 6 );
	File*		file;
	int		dummy_int[2];
	int 		i;

	sprintf( filename, "%s.edge", file_prefix );
	file = CFile_NewRead( filename );
	Journal_Firewall(
		(long)file,
		Journal_Register( IrregularQuadGenerator_Type, Error_Type ),
		"File \"%s\" not found!\n",
		filename );
	fscanf( CFile_Ptr( file ), "%d  %d", num_edges, dummy_int );

	edges = Memory_Alloc_Array_Unnamed( EdgeIndex, *num_edges );
	for( i=0; i<*num_edges; i++ )
		fscanf( CFile_Ptr( file ), "%d  %d  %d  %d", dummy_int, edges[i], edges[i] + 1, dummy_int + 1 );

	Stg_Class_Delete( file );
	Memory_Free( filename );

	return edges;
}

/* calculate the edge node coords as being at the centre of each edge */
void EdgeCentreNodes( double** nodes, EdgeIndex* edges, int num_edges, double** edge_nodes ) {
	int 	i;

	for( i=0; i<num_edges; i++ ) {
		edge_nodes[i][0] = 0.5 * (nodes[edges[i][0]][0] + nodes[edges[i][1]][0]);
		edge_nodes[i][1] = 0.5 * (nodes[edges[i][0]][1] + nodes[edges[i][1]][1]);
	}
}

/* regular triangular element shape funcs */
double IQG_M_0( double coord[2] ) { return 1.0 - coord[0] - coord[1]; }
double IQG_M_1( double coord[2] ) { return coord[0]; };
double IQG_M_2( double coord[2] ) { return coord[1]; };

/* interpolate the value onto a local coord from the triangular element shape functions */
double IrregularQuadGenerator_InterpFromLocalCoord_Tri( double phiAtNodes[3], double elCoord[2] ) {
	double	Mi[3];
	double	phi	= 0.0;
	int	node_i;
	
	/* evaluate shape functions at the local element coord */
	Mi[0] = IQG_M_0( elCoord ); 
	Mi[1] = IQG_M_1( elCoord ); 
	Mi[2] = IQG_M_2( elCoord );

	for( node_i=0; node_i<3; node_i++ )
		phi += phiAtNodes[node_i]*Mi[node_i];
	
	return phi;
}

/* iterpolate the heights of the triangle centre nodes (the voronoi nodes) from the triangular elements */
void TriCentreNodes( double** nodes, TriIndex* tris, int num_vnodes, double** vnodes ) {
	double	elCoord[2];
	double	gElNodes_x[3], gElNodes_y[3];
	int 	i;
		
	for( i=0; i<num_vnodes; i++ ) {
		/* element centroid */
		elCoord[0] = elCoord[1] = 0.33333333333333333333333333333333333333333333;

		/* map the local coord to the global coord - don't trust the voronoi node positions!!! */
		gElNodes_x[0] = nodes[tris[i][0]][0]; gElNodes_x[1] = nodes[tris[i][1]][0]; gElNodes_x[2] = nodes[tris[i][2]][0];
		gElNodes_y[0] = nodes[tris[i][0]][1]; gElNodes_y[1] = nodes[tris[i][1]][1]; gElNodes_y[2] = nodes[tris[i][2]][1];
		vnodes[i][0] = IrregularQuadGenerator_InterpFromLocalCoord_Tri( gElNodes_x, elCoord );
		vnodes[i][1] = IrregularQuadGenerator_InterpFromLocalCoord_Tri( gElNodes_y, elCoord );
	}
}

/* NOTE: triangle stores nodes in a counter clockwise ordering - make sure to do the same for edges too */
TriIndex* TriEdges( TriIndex* tris, int num_tris, EdgeIndex* edges, int num_edges ) {
	TriIndex*	tri_edges	= Memory_Alloc_Array_Unnamed( TriIndex, num_tris );
	int		i, j;

	memset( *tri_edges, -1, 3*num_tris*sizeof(int) );
	for( i=0; i<num_tris; i++ ) {
		for( j=0; j<num_edges; j++ ) {
			if( ( tris[i][0] == edges[j][0] && tris[i][1] == edges[j][1] ) ||
			    ( tris[i][0] == edges[j][1] && tris[i][1] == edges[j][0] ) ) {
				tri_edges[i][0] = j;
				continue;
			}
			else if( ( tris[i][1] == edges[j][0] && tris[i][2] == edges[j][1] ) ||
			         ( tris[i][1] == edges[j][1] && tris[i][2] == edges[j][0] ) ) {
				tri_edges[i][1] = j;
				continue;
			}
			else if( ( tris[i][2] == edges[j][0] && tris[i][0] == edges[j][1] ) ||
			         ( tris[i][2] == edges[j][1] && tris[i][0] == edges[j][0] ) ) {
				tri_edges[i][2] = j;
				continue;
			}
		}
	}

	return tri_edges;
}

/* generate the topology from a set of triangle files (.node, .ele & .edge) and spliting each triangle
 * into 3 quadrelaterals by generating additional nodes at the triangle barycentre and in the middle of
 * each vertex */
void IrregularQuadGenerator_GenTopoAndGeom( IrregularQuadGenerator* self, Mesh* mesh, IGraph* topo ) {
	Comm*		comm;
	File*		file;
	int		numNodes, numTris, numEdges;
	double		**nodes, **edgeCentreNodes, **triCentreNodes;
	TriIndex	*tris, *triEdges;
	EdgeIndex	*edges;
	unsigned	*vertInds, *elInds, *edgeInds;
	unsigned	elVerts[VERTS_PER_EL], edgeVerts[2];
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
	
	/* create the edge nodes */
	edgeCentreNodes = Memory_Alloc_2DArray( double, numEdges, 2, (Name)"edge centre nodes"  );
	EdgeCentreNodes( nodes, edges, numEdges, edgeCentreNodes );
	MPI_Barrier( self->mpiComm );

	/* create the trangle centre nodes */
	triCentreNodes = Memory_Alloc_2DArray( double, numTris, 2, (Name)"triangle centre nodes"  );
	TriCentreNodes( nodes, tris, numTris, triCentreNodes );
	MPI_Barrier( self->mpiComm );

	/* determine the indices of the edges for each triangle */
	triEdges = TriEdges( tris, numTris, edges, numEdges );
	MPI_Barrier( self->mpiComm );

	/* generate the vertices */
	vertInds = Memory_Alloc_Array( unsigned, numNodes + numTris + numEdges, "mesh vertices" );
	for( i = 0; i < numNodes + numTris + numEdges; i++ ) {
		vertInds[i] = i;
	}
	IGraph_SetElements( topo, MT_VERTEX, numNodes + numTris + numEdges, vertInds );
	FreeArray( vertInds );
	MPI_Barrier( self->mpiComm );

	/* generate the elements */
	elInds = Memory_Alloc_Array( unsigned, 3*numTris, "mesh elements" );
	for( i = 0; i < 3 * numTris; i++ ) {
		elInds[i] = i;
	}
	IGraph_SetElements( topo, topo->nDims, 3*numTris, elInds ); /* topo->nDims = 2 for surface of 3D volume */
	FreeArray( elInds );
	MPI_Barrier( self->mpiComm );

	/* generate the edges */
	edgeInds = Memory_Alloc_Array( unsigned, 2*numEdges + 3*numTris, "mesh edges" );
	for( i = 0; i < 2*numEdges + 3*numTris; i++ ) {
		edgeInds[i] = i;
	}
	IGraph_SetElements( topo, MT_EDGE, 2*numEdges + 3*numTris, edgeInds );
	FreeArray( edgeInds );
	MPI_Barrier( self->mpiComm );

	/* generate the geometry for the nodes */
	sync = (Sync*)IGraph_GetDomain( topo, 0 );
	mesh->verts = AllocNamedArray2D( double, Sync_GetNumDomains( sync ), topo->nDims, "Mesh::verts" );

	for( i = 0; i < numNodes; i++ ) {
		mesh->verts[i][0] = nodes[i][0];
		mesh->verts[i][1] = nodes[i][1];
	}
	for( i = 0; i < numEdges; i++ ) {
		mesh->verts[i + numNodes][0] = edgeCentreNodes[i][0];
		mesh->verts[i + numNodes][1] = edgeCentreNodes[i][1];
	}
	for( i = 0; i < numTris; i++ ) {
		mesh->verts[i + numNodes + numEdges][0] = triCentreNodes[i][0];
		mesh->verts[i + numNodes + numEdges][1] = triCentreNodes[i][1];
	}
	MPI_Barrier( self->mpiComm );

	/* generate the element-vertex incidence relationships */	
	for( i = 0; i < numTris; i++ ) {
		for( j = 0; j < 3; j++ ) {
			/* order the element vertices in the same way that the (linear) shape functions are ordered */
			elVerts[0] = tris[i][j];
			elVerts[1] = numNodes + triEdges[i][j];
			elVerts[2] = numNodes + triEdges[i][(j+2)%3];
			elVerts[3] = numNodes + numEdges + i;
	
			IrregularQuadGenerator_MapToDomain( self, (Sync*)IGraph_GetDomain( topo, 0 ), VERTS_PER_EL, elVerts );
			IGraph_SetIncidence( topo, topo->nDims, 3*i + j, MT_VERTEX, VERTS_PER_EL, elVerts );
		}
	}
	MPI_Barrier( self->mpiComm );

	/* generate the edge-vertex incidence relationships... first those derived from the triangle edges */
	for( i = 0; i < numEdges; i++ ) {
		edgeVerts[0] = edges[i][0];
		edgeVerts[1] = numNodes + i;	
		IrregularQuadGenerator_MapToDomain( self, (Sync*)IGraph_GetDomain( topo, 0 ), 2, edgeVerts );
		IGraph_SetIncidence( topo, MT_EDGE, 2*i, MT_VERTEX, 2, edgeVerts );

		edgeVerts[0] = numNodes + i;
		edgeVerts[1] = edges[i][1];	
		IrregularQuadGenerator_MapToDomain( self, (Sync*)IGraph_GetDomain( topo, 0 ), 2, edgeVerts );
		IGraph_SetIncidence( topo, MT_EDGE, 2*i + 1, MT_VERTEX, 2, edgeVerts );
	}
	/* and now those for the the internal triangle edges */
	for( i = 0; i < numTris; i++ ) {
		for( j = 0; j < 3; j++ ) {
			edgeVerts[0] = numNodes + triEdges[i][j];
			edgeVerts[1] = numNodes + numEdges + i;
			IrregularQuadGenerator_MapToDomain( self, (Sync*)IGraph_GetDomain( topo, 0 ), 2, edgeVerts );
			IGraph_SetIncidence( topo, MT_EDGE, 2*numEdges + 3*i + j, MT_VERTEX, 2, edgeVerts );
		}
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

	IrregularQuadGenerator_CompleteVertexNeighbours( self, topo );
	MPI_Barrier( self->mpiComm );

	IrregularQuadGenerator_GenBndVerts( self, topo );	
	MPI_Barrier( self->mpiComm );

	Memory_Free( nodes );
	Memory_Free( tris );
	Memory_Free( edges );
	Memory_Free( triEdges );
	Memory_Free( edgeCentreNodes );
	Memory_Free( triCentreNodes );
}

void IrregularQuadGenerator_GenBndVerts( IrregularQuadGenerator* self, IGraph* topo ) {
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
			/* assign the vertices which togather with the current vertex constitute a valid edge for this element */
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
void IrregularQuadGenerator_CompleteVertexNeighbours( IrregularQuadGenerator* self, IGraph* topo ) {
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

void IrregularQuadGenerator_MapToDomain( IrregularQuadGenerator* self, Sync* sync, unsigned nIncEls, unsigned* incEls )
{
	unsigned	inc_i;

	assert( self );
	assert( sync );
	assert( nIncEls );
	assert( incEls );

	for( inc_i = 0; inc_i < nIncEls; inc_i++ )
		incEls[inc_i] = Sync_GlobalToDomain( sync, incEls[inc_i] );
}

void IrregularQuadGenerator_Destruct( IrregularQuadGenerator* self ) {
	assert( self );

	IrregularQuadGenerator_DestructTopology( self );
	IrregularQuadGenerator_DestructGeometry( self );
	KillArray( self->minDecomp );
	KillArray( self->maxDecomp );
	Stg_Class_RemoveRef( self->comm );
}

void IrregularQuadGenerator_DestructTopology( IrregularQuadGenerator* self ) {
	assert( self );

	self->maxDecompDims = 0;
	memset( self->minDecomp, 0, self->nDims * sizeof(unsigned) );
	memset( self->maxDecomp, 0, self->nDims * sizeof(unsigned) );
}

void IrregularQuadGenerator_DestructGeometry( IrregularQuadGenerator* self ) {
	assert( self );
}

/* parallel implementation of the topology and geometry generator - only reads in nodes which are part of the local domain */
void IrregularQuadGenerator_GenTopoAndGeom_Parallel( IrregularQuadGenerator* self, Mesh* mesh, IGraph* topo ) {
	Comm*		comm;
	FILE*		file;
	double		**nodes, **edgeCentreNodes, **triCentreNodes;
	TriIndex	*tris, *triEdges;
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
	unsigned	*numNodesPerProc, numLocals, localOffset, *numEdgesPerProc, *numElsPerProc;

	MPI_Comm_rank( self->mpiComm, &rank );
	MPI_Comm_size( self->mpiComm, &size );

	Mesh_GetDomainCoordRange( self->referenceMesh, min, max );

	/* read in the nodal values for the local domain */
	filename = Memory_Alloc_Array( char, strlen( self->filename ) + 6, "triangle node file" );
	sprintf( filename, "%s.node", self->filename );
	file = CFile_NewRead( filename );
	Journal_Firewall(
		(long)file,
		Journal_Register( IrregularQuadGenerator_Type, Error_Type ),
		"File \"%s\" not found!\n",
		filename );
	fscanf( CFile_Ptr( file ), "%u  %d  %d  %d", &numNodesGlobal, dummy_int, dummy_int + 1, dummy_int + 2, dummy_int + 3 );
	nodeMap = Memory_Alloc_Array( int, numNodesGlobal, "global to local node map" );
	memset( nodeMap, -1, numNodesGlobal*sizeof( int ) );
	numNodesLocal = 0;
	for( i = 0; i < numNodesGlobal; i++ ) {
		fscanf( CFile_Ptr( file ), "%d %lf %lf %lf %d", dummy_int, coord, coord + 1, &dummy_double, dummy_int + 1 );
		if( coord[0] < min[I_AXIS] || coord[0] > max[I_AXIS] || coord[1] < min[K_AXIS] || coord[1] > max[K_AXIS] )
			continue;

		numNodesLocal++;
	}

	nodes = Memory_Alloc_2DArray( double, numNodesLocal, 2, (Name)"triangle nodes" );
	rewind( CFile_Ptr( file ) );
	fscanf( CFile_Ptr( file ), "%u  %d  %d  %d", &numNodesGlobal, dummy_int, dummy_int + 1, dummy_int + 2, dummy_int + 3 );
	localNode_i = 0;
	for( i = 0; i < numNodesGlobal; i++ ) {
		fscanf( CFile_Ptr( file ), "%d %lf %lf %lf %d", dummy_int, coord, coord + 1, &dummy_double, dummy_int + 1 );
		if( coord[0] < min[I_AXIS] || coord[0] > max[I_AXIS] || coord[1] < min[K_AXIS] || coord[1] > max[K_AXIS] )
			continue;

		nodes[localNode_i][0] = coord[0];
		nodes[localNode_i][1] = coord[1];
		nodeMap[i] = localNode_i++;
	}
	Stg_Class_Delete( file );
	Memory_Free( filename );

	/* read in the edge topology for the local domain */
	filename = Memory_Alloc_Array( char, strlen( self->filename ) + 6, "triangle edges file" );
	sprintf( filename, "%s.edge", self->filename );
	file = CFile_New( filename );
	fscanf( CFile_Ptr( file ), "%u  %d", &numEdgesGlobal, dummy_int );
	numEdgesLocal = 0;
	for( i = 0; i < numEdgesGlobal; i++ ) {
		fscanf( CFile_Ptr( file ), "%d  %u  %u  %d", dummy_int, edge, edge + 1, dummy_int + 1 );
		if( nodeMap[edge[0]] != -1 && nodeMap[edge[1]] != -1 )
			numEdgesLocal++;
	}

	edges = Memory_Alloc_Array( EdgeIndex, numEdgesLocal, "triangle edges" );
	rewind( CFile_Ptr( file ) );
	localEdge_i = 0;
	fscanf( CFile_Ptr( file ), "%u  %d", &numEdgesGlobal, dummy_int );
	for( i = 0; i < numEdgesGlobal; i++ ) {
		fscanf( CFile_Ptr( file ), "%d  %u  %u  %d", dummy_int, edge, edge + 1, dummy_int + 1 );
		if( nodeMap[edge[0]] != -1 && nodeMap[edge[1]] != -1 ) {
			edges[localEdge_i][0] = nodeMap[edge[0]];
			edges[localEdge_i][1] = nodeMap[edge[1]];
			localEdge_i++;
		}
	}
	Stg_Class_Delete( file );
	Memory_Free( filename );

	/* read the triangle topology for the local domain */
	filename = Memory_Alloc_Array( char, strlen( self->filename ) + 5, "triangle element file" );
	sprintf( filename, "%s.ele", self->filename );
	file = CFile_NewRead( filename );
	Journal_Firewall(
		(long)file,
		Journal_Register( IrregularQuadGenerator_Type, Error_Type ),
		"File \"%s\" not found!\n",
		filename );
	fscanf( CFile_Ptr( file ), "%u  %d  %d", &numTrisGlobal, dummy_int, dummy_int + 1 );
	numTrisLocal = 0;
	for( i = 0; i < numTrisGlobal; i++ ) {
		fscanf( CFile_Ptr( file ), "%d  %u  %u  %u", dummy_int, tri, tri + 1, tri + 2 );
		if( nodeMap[tri[0]] != -1 && nodeMap[tri[1]] != -1 && nodeMap[tri[2]] != -1 )
			numTrisLocal++;
	}

	tris = Memory_Alloc_Array( TriIndex, numTrisLocal, "triangle elements" );
	rewind( CFile_Ptr( file ) );
	localTri_i = 0;
	fscanf( CFile_Ptr( file ), "%u  %d  %d", &numTrisGlobal, dummy_int, dummy_int + 1 );
	for( i = 0; i < numTrisGlobal; i++ ) {
		fscanf( CFile_Ptr( file ), "%d  %u  %u  %u", dummy_int, tri, tri + 1, tri + 2 );
		if( nodeMap[tri[0]] != -1 && nodeMap[tri[1]] != -1 && nodeMap[tri[2]] != -1 ) {
			tris[localTri_i][0] = nodeMap[tri[0]];
			tris[localTri_i][1] = nodeMap[tri[1]];
			tris[localTri_i][2] = nodeMap[tri[2]];
			localTri_i++;
		}
	}
	Stg_Class_Delete( file );
	Memory_Free( filename );

	MPI_Barrier( self->mpiComm );
	
	/* create the edge nodes */
	edgeCentreNodes = Memory_Alloc_2DArray( double, numEdgesLocal, 2, (Name)"edge centre nodes"  );
	EdgeCentreNodes( nodes, edges, numEdgesLocal, edgeCentreNodes );
	MPI_Barrier( self->mpiComm );

	/* create the trangle centre nodes */
	triCentreNodes = Memory_Alloc_2DArray( double, numTrisLocal, 2, (Name)"triangle centre nodes"  );
	TriCentreNodes( nodes, tris, numTrisLocal, triCentreNodes );
	MPI_Barrier( self->mpiComm );

	/* determine the indices of the edges for each triangle */
	triEdges = TriEdges( tris, numTrisLocal, edges, numEdgesLocal );
	MPI_Barrier( self->mpiComm );

	/* generate the vertices - for parallel implementation this indices must correspond to the GLOBAL node indices */
	numLocals = numNodesLocal + numTrisLocal + numEdgesLocal;
	numNodesPerProc = Memory_Alloc_Array( unsigned, size, "number of nodes local to each processor" );
	numNodesPerProc[rank] = numLocals;
	for( i = 0; i < size; i++ ) {
		MPI_Bcast( &numNodesPerProc[i], 1, MPI_UNSIGNED, i, self->mpiComm );
	}
	vertInds = Memory_Alloc_Array( unsigned, numLocals, "mesh vertices" );
	localOffset = 0;
	for( i = 0; i < rank; i++ ) {
		localOffset += numNodesPerProc[i];
	}
	for( i = 0; i < numLocals; i++ ) {
		vertInds[i] = i + localOffset;
	}
	IGraph_SetElements( topo, MT_VERTEX, numLocals, vertInds );
	FreeArray( vertInds );
	FreeArray( numNodesPerProc );
	MPI_Barrier( self->mpiComm );

	/* generate the elements */
	numLocals = 3*numTrisLocal;
	numElsPerProc = Memory_Alloc_Array( unsigned, size, "number of elements local to each processor" );
	numElsPerProc[rank] = numLocals;
	for( i = 0; i < size; i++ ) {
		MPI_Bcast( &numElsPerProc[i], 1, MPI_UNSIGNED, i, self->mpiComm );
	}
	elInds = Memory_Alloc_Array( unsigned, numLocals, "mesh elements" );
	localOffset = 0;
	for( i = 0; i < rank; i++ ) {
		localOffset += numElsPerProc[i];
	}
	for( i = 0; i < numLocals; i++ ) {
		elInds[i] = i + localOffset;
	}
	IGraph_SetElements( topo, topo->nDims, numLocals, elInds ); /* topo->nDims = 2 for surface of 3D volume */
	FreeArray( elInds );
	FreeArray( numElsPerProc );
	MPI_Barrier( self->mpiComm );

	/* generate the edges */
	numLocals = 2*numEdgesLocal + 3*numTrisLocal; 
	numEdgesPerProc = Memory_Alloc_Array( unsigned, size, "number of edges local to each processor" );
	numEdgesPerProc[rank] = numLocals;
	for( i = 0; i < size; i++ ) {
		MPI_Bcast( &numEdgesPerProc[i], 1, MPI_UNSIGNED, i, self->mpiComm );
	}
	edgeInds = Memory_Alloc_Array( unsigned, numLocals, "mesh edges" );
	localOffset = 0;
	for( i = 0; i < rank; i++ ) {
		localOffset += numEdgesPerProc[i];
	}
	for( i = 0; i < numLocals; i++ ) {
		edgeInds[i] = i + localOffset;
	}
	IGraph_SetElements( topo, MT_EDGE, numLocals, edgeInds );
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
	for( i = 0; i < numEdgesLocal; i++ ) {
		mesh->verts[i + numNodesLocal][0] = edgeCentreNodes[i][0];
		mesh->verts[i + numNodesLocal][1] = edgeCentreNodes[i][1];
	}
	for( i = 0; i < numTrisLocal; i++ ) {
		mesh->verts[i + numNodesLocal + numEdgesLocal][0] = triCentreNodes[i][0];
		mesh->verts[i + numNodesLocal + numEdgesLocal][1] = triCentreNodes[i][1];
	}
	MPI_Barrier( self->mpiComm );

	/* generate the element-vertex incidence relationships */	
	for( i = 0; i < numTrisLocal; i++ ) {
		for( j = 0; j < 3; j++ ) {
			/* order the element vertices in the same way that the (linear) shape functions are ordered */
			elVerts[0] = tris[i][j];
			elVerts[1] = numNodesLocal + triEdges[i][j];
			elVerts[2] = numNodesLocal + triEdges[i][(j+2)%3];
			elVerts[3] = numNodesLocal + numEdgesLocal + i;
	
			//IrregularQuadGenerator_MapToDomain( self, (Sync*)IGraph_GetDomain( topo, 0 ), VERTS_PER_EL, elVerts );
			IGraph_SetIncidence( topo, topo->nDims, 3*i + j, MT_VERTEX, VERTS_PER_EL, elVerts );
		}
	}
	MPI_Barrier( self->mpiComm );

	/* generate the edge-vertex incidence relationships... first those derived from the triangle edges */
	for( i = 0; i < numEdgesLocal; i++ ) {
		edgeVerts[0] = edges[i][0];
		edgeVerts[1] = numNodesLocal + i;	
		//IrregularQuadGenerator_MapToDomain( self, (Sync*)IGraph_GetDomain( topo, 0 ), 2, edgeVerts );
		IGraph_SetIncidence( topo, MT_EDGE, 2*i, MT_VERTEX, 2, edgeVerts );

		edgeVerts[0] = numNodesLocal + i;
		edgeVerts[1] = edges[i][1];	
		//IrregularQuadGenerator_MapToDomain( self, (Sync*)IGraph_GetDomain( topo, 0 ), 2, edgeVerts );
		IGraph_SetIncidence( topo, MT_EDGE, 2*i + 1, MT_VERTEX, 2, edgeVerts );
	}
	/* and now those for the the internal triangle edges */
	for( i = 0; i < numTrisLocal; i++ ) {
		for( j = 0; j < 3; j++ ) {
			edgeVerts[0] = numNodesLocal + triEdges[i][j];
			edgeVerts[1] = numNodesLocal + numEdgesLocal + i;
			//IrregularQuadGenerator_MapToDomain( self, (Sync*)IGraph_GetDomain( topo, 0 ), 2, edgeVerts );
			IGraph_SetIncidence( topo, MT_EDGE, 2*numEdgesLocal + 3*i + j, MT_VERTEX, 2, edgeVerts );
		}
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

	IrregularQuadGenerator_CompleteVertexNeighbours( self, topo );
	MPI_Barrier( self->mpiComm );

	IrregularQuadGenerator_GenBndVerts( self, topo );	
	MPI_Barrier( self->mpiComm );

	Memory_Free( nodes );
	Memory_Free( tris );
	Memory_Free( edges );
	Memory_Free( triEdges );
	Memory_Free( edgeCentreNodes );
	Memory_Free( triCentreNodes );
	Memory_Free( nodeMap );
}
