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
#include <math.h>
#include <string.h>
#include <assert.h>

#include <mpi.h>
#include <StGermain/StGermain.h>

#include <StgDomain/Geometry/Geometry.h>
#include <StgDomain/Shape/Shape.h>

#include "types.h"
#include "shortcuts.h"
#include "Grid.h"
#include "Decomp.h"
#include "Sync.h"
#include "MeshTopology.h"
#include "IGraph.h"
#include "MeshClass.h"
#include "MeshGenerator.h"
#include "MeshAdaptor.h"
#include "CompressionAdaptor.h"


typedef double (CompressionAdaptor_DeformFunc)( CompressionAdaptor* self, Mesh* mesh, 
					    unsigned* globalSize, unsigned vertex, unsigned* vertexInds );


/* Textual name of this class */
const Type CompressionAdaptor_Type = "CompressionAdaptor";


/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/

CompressionAdaptor* CompressionAdaptor_New( Name name, AbstractContext* context ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(CompressionAdaptor);
	Type                                                      type = CompressionAdaptor_Type;
	Stg_Class_DeleteFunction*                              _delete = _CompressionAdaptor_Delete;
	Stg_Class_PrintFunction*                                _print = _CompressionAdaptor_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = (void* (*)(Name))_CompressionAdaptor_New;
	Stg_Component_ConstructFunction*                    _construct = _CompressionAdaptor_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _CompressionAdaptor_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _CompressionAdaptor_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _CompressionAdaptor_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _CompressionAdaptor_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
	MeshGenerator_SetDimSizeFunc*                   setDimSizeFunc = _MeshGenerator_SetDimSize;
	MeshGenerator_GenerateFunc*                       generateFunc = CompressionAdaptor_Generate;

	CompressionAdaptor* self = _CompressionAdaptor_New(  COMPRESSIONADAPTOR_PASSARGS  );
   
   _MeshGenerator_Init( (MeshGenerator*)self, context );
   _MeshAdaptor_Init( (MeshAdaptor*)self );
	/* CompressionAdaptor info */
	_CompressionAdaptor_Init( self );

   return self;
}

CompressionAdaptor* _CompressionAdaptor_New(  COMPRESSIONADAPTOR_DEFARGS  ) {
	CompressionAdaptor* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(CompressionAdaptor) );
	self = (CompressionAdaptor*)_MeshAdaptor_New(  MESHADAPTOR_PASSARGS  );

	/* Virtual info */
	return self;
}

void _CompressionAdaptor_Init( CompressionAdaptor* self ) {
	self->compressionfactor  = 0.0;
}


/*----------------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _CompressionAdaptor_Delete( void* adaptor ) {
	CompressionAdaptor*	self = (CompressionAdaptor*)adaptor;

	/* Delete the parent. */
	_MeshAdaptor_Delete( self );
}

void _CompressionAdaptor_Print( void* adaptor, Stream* stream ) {
	CompressionAdaptor*	self = (CompressionAdaptor*)adaptor;
	
	/* Set the Journal for printing informations */
	Stream* adaptorStream;
	adaptorStream = Journal_Register( InfoStream_Type, (Name)"CompressionAdaptorStream"  );

	/* Print parent */
	Journal_Printf( stream, "CompressionAdaptor (ptr): (%p)\n", self );
	_MeshAdaptor_Print( self, stream );
}

void _CompressionAdaptor_AssignFromXML( void* adaptor, Stg_ComponentFactory* cf, void* data ) {
	CompressionAdaptor*	self = (CompressionAdaptor*)adaptor;

	assert( self );
	assert( cf );

	/* Call parent construct. */
	_MeshAdaptor_AssignFromXML( self, cf, data );

	self->compressionfactor = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"compressionfactor", 0.0 );
	if(self->compressionfactor == 0.0){
		printf("cannot have a zero compression factor exiting program\n");
		abort( );
	}
}

void _CompressionAdaptor_Build( void* adaptor, void* data ) {
	_MeshAdaptor_Build( adaptor, data );
}

void _CompressionAdaptor_Initialise( void* adaptor, void* data ) {
	_MeshAdaptor_Initialise( adaptor, data );
}

void _CompressionAdaptor_Execute( void* adaptor, void* data ) {
	_MeshAdaptor_Execute( adaptor, data );
}

void _CompressionAdaptor_Destroy( void* adaptor, void* data ) {
	_MeshAdaptor_Destroy( adaptor, data );
}

/*--------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/

void CompressionAdaptor_Generate( void* adaptor, void* _mesh, void* data ) {
	CompressionAdaptor*		self = (CompressionAdaptor*)adaptor;
	Mesh*				mesh = (Mesh*)_mesh;
	const Sync*			sync;
	Grid				*grid;
	unsigned*			inds;
	double				min,max[3],min2[3];
	unsigned			gNode;
	double				x,b;
	unsigned			d_i, n_i;

	/* Build base mesh, which is assumed to be cartesian. */
	MeshGenerator_Generate( self->generator, mesh, data );

	/* If we're not 2D or 3D, forget about it. */
	if( mesh->topo->nDims != 2 && mesh->topo->nDims != 3 )
		return;

	/* Extract the cartesian information. */
	grid = *(Grid**)ExtensionManager_Get( mesh->info, mesh, mesh->vertGridId  );
	inds = AllocArray( unsigned, Mesh_GetDimSize( mesh ) );
        
	//get factor b so that there is always a 1 to 1 relationship with the top position normally and top position when compressed
	Mesh_GetGlobalCoordRange( mesh, min2, max);
	b = (max[1] - min2[1])/(pow((max[1] - min2[1]), (1.0/self->compressionfactor)));
	for( d_i = 0; d_i < Mesh_GetDimSize( mesh ); d_i++ )
		inds[d_i] = 0;
	gNode = Grid_Project( grid, inds );
	insist( Mesh_GlobalToDomain( mesh, MT_VERTEX, gNode, &gNode ), == True );
	min = mesh->verts[gNode][1];

	/* Loop over domain nodes. */
	sync = IGraph_GetDomain( mesh->topo, MT_VERTEX );
	for( n_i = 0; n_i < Sync_GetNumDomains( sync ); n_i++ ) {
		gNode = Sync_DomainToGlobal( sync, n_i );
		Grid_Lift( grid, gNode, inds );

		/* Deform this node. */
		x = mesh->verts[n_i][1] - min;
		//a compressionfactor function, eg if compression factor = 3 then would be a cubed root function
		mesh->verts[n_i][1] = b*(pow(x, (1.0/self->compressionfactor)));
	}

	/* Free resources. */
	FreeArray( inds );
}


/*----------------------------------------------------------------------------------------------------------------------------------
** Private Functions
*/


