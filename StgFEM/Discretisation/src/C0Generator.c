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
#include <mpi.h>

#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include "Discretisation.h"


/* Textual name of this class */
const Type C0Generator_Type = "C0Generator";


/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/

C0Generator* C0Generator_New( Name name, AbstractContext* context ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(C0Generator);
	Type                                                      type = C0Generator_Type;
	Stg_Class_DeleteFunction*                              _delete = _C0Generator_Delete;
	Stg_Class_PrintFunction*                                _print = _C0Generator_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = (void* (*)(Name))_C0Generator_New;
	Stg_Component_ConstructFunction*                    _construct = _C0Generator_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _C0Generator_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _C0Generator_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _C0Generator_Execute;
	Stg_Component_DestroyFunction*                        _destroy = NULL;
	AllocationType                              nameAllocationType = NON_GLOBAL;
	MeshGenerator_SetDimSizeFunc*                   setDimSizeFunc = _MeshGenerator_SetDimSize;
	MeshGenerator_GenerateFunc*                       generateFunc = (MeshGenerator_GenerateFunc*)C0Generator_Generate;

	C0Generator* self = _C0Generator_New(  C0GENERATOR_PASSARGS  );
   
   _MeshGenerator_Init( (MeshGenerator*)self, context );
	_C0Generator_Init( self );

   return self;
}

C0Generator* _C0Generator_New(  C0GENERATOR_DEFARGS  ) {
	C0Generator*	self;

	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(C0Generator) );
	self = (C0Generator*)_MeshGenerator_New(  MESHGENERATOR_PASSARGS  );

	/* Virtual info */

	return self;
}

void _C0Generator_Init( C0Generator* self ) {
	assert( self && Stg_CheckType( self, C0Generator ) );

	self->elMesh = NULL;
}


/*----------------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _C0Generator_Delete( void* generator ) {
	C0Generator*	self = (C0Generator*)generator;

	/* Delete the parent. */
	_MeshGenerator_Delete( self );
}

void _C0Generator_Print( void* generator, Stream* stream ) {
	C0Generator*	self = (C0Generator*)generator;
	
	/* Set the Journal for printing informations */
	Stream* generatorStream;
	generatorStream = Journal_Register( InfoStream_Type, (Name)"C0GeneratorStream"  );

	/* Print parent */
	Journal_Printf( stream, "C0Generator (ptr): (%p)\n", self );
	_MeshGenerator_Print( self, stream );
}

void _C0Generator_AssignFromXML( void* generator, Stg_ComponentFactory* cf, void* data ) {
	C0Generator*	self = (C0Generator*)generator;
	Mesh*		elMesh;

	assert( self );
	assert( cf );

	_MeshGenerator_AssignFromXML( self, cf, data );

	elMesh = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"elementMesh", Mesh, True, data  );
	C0Generator_SetElementMesh( self, elMesh );
}

void _C0Generator_Build( void* generator, void* data ) {
	C0Generator*	self = (C0Generator*)generator;
   Stg_Component_Build( self->elMesh, data, False );
	_MeshGenerator_Build( generator, data );
}

void _C0Generator_Initialise( void* generator, void* data ) {
	C0Generator*	self = (C0Generator*)generator;
   Stg_Component_Initialise( self->elMesh, data, False );
	_MeshGenerator_Initialise( generator, data );
}

void _C0Generator_Execute( void* generator, void* data ) {
}

void _C0Generator_Destroy( void* generator, void* data ) {
}

void C0Generator_Generate( void* generator, void* _mesh ) {
	C0Generator*	self = (C0Generator*)generator;
	FeMesh*		mesh = (FeMesh*)_mesh;
	Grid**		grid;
	Grid*		elGrid;

	assert( self && Stg_CheckType( self, C0Generator ) );
	assert( mesh && Stg_CheckType( mesh, FeMesh ) );

	C0Generator_BuildTopology( self, mesh );
	C0Generator_BuildGeometry( self, mesh );
	C0Generator_BuildElementTypes( self, mesh );

   // Get the elementMesh's grid 
	elGrid = *(Grid**)ExtensionManager_Get( self->elMesh->info, self->elMesh, self->elMesh->elGridId  );

   // build a grid for this mesh
	mesh->elGridId = ExtensionManager_Add( mesh->info, (Name)"elementGrid", sizeof(Grid*) );
	grid = (Grid** )ExtensionManager_Get( mesh->info, mesh, mesh->elGridId );
	*grid = Grid_New( );

   // copy the values from the elementMesh's grid to this mesh's grid
	Grid_SetNumDims( *grid, elGrid->nDims );
	Grid_SetSizes( *grid, elGrid->sizes );

	/* this looks like a hack. */
	/* C0 is not an element mesh? */
        mesh->elementMesh = True;
}


/*--------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/

void C0Generator_SetElementMesh( void* generator, void* mesh ) {
	C0Generator*	self = (C0Generator*)generator;

	assert( self && Stg_CheckType( self, C0Generator ) );

	self->elMesh = mesh;
}


/*----------------------------------------------------------------------------------------------------------------------------------
** Private Functions
*/

void C0Generator_BuildTopology( C0Generator* self, FeMesh* mesh ) {
	Mesh*		elMesh;
	MeshTopology	*topo, *elTopo;
	Sync*		elSync;
	unsigned	nDims;
	unsigned	*nIncEls, **incEls;
	unsigned	nDomainEls;
	unsigned	e_i;

	assert( self );
	assert( mesh );

	elMesh = self->elMesh;
	nDims = Mesh_GetDimSize( elMesh );
	elTopo = Mesh_GetTopology( elMesh );
	elSync = Mesh_GetSync( elMesh, nDims );

	topo = Mesh_GetTopology( mesh );
	MeshTopology_SetComm( topo, MeshTopology_GetComm( elTopo ) );
	MeshTopology_SetNumDims( topo, nDims );
	IGraph_SetDomain( topo, nDims, elSync );
	IGraph_SetDomain( topo, MT_VERTEX, elSync );
	topo->shadDepth = elTopo->shadDepth;

	nDomainEls = Mesh_GetDomainSize( elMesh, nDims );
	nIncEls = AllocArray( unsigned, nDomainEls );
	incEls = AllocArray2D( unsigned, nDomainEls, 1 );
	for( e_i = 0; e_i < nDomainEls; e_i++ ) {
		nIncEls[e_i] = 1;
		incEls[e_i][0] = e_i;
		IGraph_SetIncidence( topo, nDims, e_i, MT_VERTEX, nIncEls[e_i], incEls[e_i] );
	}
	FreeArray( nIncEls );
	FreeArray( incEls );

	IGraph_InvertIncidence( topo, MT_VERTEX, nDims );
}

void C0Generator_BuildGeometry( C0Generator* self, FeMesh* mesh ) {
	Mesh*			elMesh;
	double			*centroid, *vert;
	unsigned		nDims;
	unsigned		nDomainEls;
	Mesh_ElementType*	elType;
	unsigned		e_i;

	assert( self );
	assert( mesh );

	elMesh = self->elMesh;
	nDims = Mesh_GetDimSize( elMesh );
	nDomainEls = Mesh_GetDomainSize( elMesh, nDims );
	mesh->verts = AllocArray2D( double, nDomainEls, nDims );
	centroid = AllocArray( double, nDims );
	for( e_i = 0; e_i < nDomainEls; e_i++ ) {
		elType = Mesh_GetElementType( elMesh, e_i );
		Mesh_ElementType_GetCentroid( elType, e_i, centroid );
		vert = Mesh_GetVertex( mesh, e_i );
		memcpy( vert, centroid, nDims * sizeof(double) );
	}
	FreeArray( centroid );
}

void C0Generator_BuildElementTypes( C0Generator* self, FeMesh* mesh ) {
	unsigned		nDomainEls;
	Mesh_Algorithms*	algs;
	unsigned		e_i;

	assert( self );
	assert( mesh );

	mesh->nElTypes = 1;
	mesh->elTypes = AllocNamedArray( Mesh_ElementType*, mesh->nElTypes, "Mesh::elTypes" );
	mesh->elTypes[0] = (Mesh_ElementType*)Mesh_CentroidType_New();
	Mesh_ElementType_SetMesh( mesh->elTypes[0], mesh );
	Mesh_CentroidType_SetElementMesh( mesh->elTypes[0], self->elMesh );
	nDomainEls = Mesh_GetDomainSize( mesh, Mesh_GetDimSize( mesh ) );
	mesh->elTypeMap = AllocNamedArray( unsigned, nDomainEls, "Mesh::elTypeMap" );

	for( e_i = 0; e_i < nDomainEls; e_i++ )
		mesh->elTypeMap[e_i] = 0;

	algs = (Mesh_Algorithms*)Mesh_CentroidAlgorithms_New( "", NULL );
	Mesh_CentroidAlgorithms_SetElementMesh( algs, self->elMesh );
	Mesh_SetAlgorithms( mesh, algs );
}


