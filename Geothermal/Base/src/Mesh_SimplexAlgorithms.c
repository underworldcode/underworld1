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

#include "types.h"
#include "Mesh_SimplexAlgorithms.h"


/* Textual name of this class */
const Type Mesh_SimplexAlgorithms_Type = (const Type)"Mesh_SimplexAlgorithms";

/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/

Mesh_SimplexAlgorithms* Mesh_SimplexAlgorithms_New( Name name, AbstractContext* context) {
	/* Variables set in this function */
	SizeT                                                   _sizeOfSelf = sizeof(Mesh_SimplexAlgorithms);
	Type                                                           type = Mesh_SimplexAlgorithms_Type;
	Stg_Class_DeleteFunction*                                   _delete = _Mesh_SimplexAlgorithms_Delete;
	Stg_Class_PrintFunction*                                     _print = _Mesh_SimplexAlgorithms_Print;
	Stg_Class_CopyFunction*                                       _copy = NULL;
	Stg_Component_DefaultConstructorFunction*       _defaultConstructor = (void* (*)(Name))_Mesh_SimplexAlgorithms_New;
	Stg_Component_ConstructFunction*                         _construct = _Mesh_SimplexAlgorithms_AssignFromXML;
	Stg_Component_BuildFunction*                                 _build = _Mesh_SimplexAlgorithms_Build;
	Stg_Component_InitialiseFunction*                       _initialise = _Mesh_SimplexAlgorithms_Initialise;
	Stg_Component_ExecuteFunction*                             _execute = _Mesh_SimplexAlgorithms_Execute;
	Stg_Component_DestroyFunction*                             _destroy = _Mesh_SimplexAlgorithms_Destroy;
	AllocationType                                   nameAllocationType = NON_GLOBAL;
	Mesh_Algorithms_SetMeshFunc*                            setMeshFunc = _Mesh_Algorithms_SetMesh;
	Mesh_Algorithms_UpdateFunc*                              updateFunc = _Mesh_Algorithms_Update;
	Mesh_Algorithms_NearestVertexFunc*                nearestVertexFunc = _Mesh_Algorithms_NearestVertex;
	Mesh_Algorithms_SearchFunc*                              searchFunc = _Mesh_Algorithms_Search;
	Mesh_Algorithms_SearchElementsFunc*              searchElementsFunc = _Mesh_SimplexAlgorithms_SearchElements;
	Mesh_Algorithms_GetMinimumSeparationFunc*  getMinimumSeparationFunc = _Mesh_Algorithms_GetMinimumSeparation;
	Mesh_Algorithms_GetLocalCoordRangeFunc*      getLocalCoordRangeFunc = _Mesh_Algorithms_GetLocalCoordRange;
	Mesh_Algorithms_GetDomainCoordRangeFunc*    getDomainCoordRangeFunc = _Mesh_Algorithms_GetDomainCoordRange;
	Mesh_Algorithms_GetGlobalCoordRangeFunc*    getGlobalCoordRangeFunc = _Mesh_Algorithms_GetGlobalCoordRange;

   Mesh_SimplexAlgorithms* self = _Mesh_SimplexAlgorithms_New(  MESH_HEXALGORITHMS_PASSARGS  );
	/* Mesh_SimplexAlgorithms info */
	_Mesh_Algorithms_Init( (Mesh_Algorithms*)self, context );
	_Mesh_SimplexAlgorithms_Init( self );

   return self;

}

Mesh_SimplexAlgorithms* _Mesh_SimplexAlgorithms_New(  MESH_HEXALGORITHMS_DEFARGS  ) {
	Mesh_SimplexAlgorithms* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(Mesh_SimplexAlgorithms) );
	self = (Mesh_SimplexAlgorithms*)_Mesh_Algorithms_New(  MESH_ALGORITHMS_PASSARGS  );

	return self;
}

void _Mesh_SimplexAlgorithms_Init( void* simplexAlgorithms ) {
}

/*----------------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _Mesh_SimplexAlgorithms_Delete( void* simplexAlgorithms ) {
	Mesh_SimplexAlgorithms*	self = (Mesh_SimplexAlgorithms*)simplexAlgorithms;

	/* Delete the parent. */
	_Mesh_Algorithms_Delete( self );
}

void _Mesh_SimplexAlgorithms_Print( void* simplexAlgorithms, Stream* stream ) {
	Mesh_SimplexAlgorithms*	self = (Mesh_SimplexAlgorithms*)simplexAlgorithms;
	
	/* Set the Journal for printing informations */
	Stream* simplexAlgorithmsStream;
	simplexAlgorithmsStream = Journal_Register( InfoStream_Type, (Name)"Mesh_SimplexAlgorithmsStream"  );

	/* Print parent */
	Journal_Printf( stream, "Mesh_SimplexAlgorithms (ptr): (%p)\n", self );
	_Mesh_Algorithms_Print( self, stream );
}

void _Mesh_SimplexAlgorithms_AssignFromXML( void* simplexAlgorithms, Stg_ComponentFactory* cf, void* data ) {

   Mesh_SimplexAlgorithms*	self = (Mesh_SimplexAlgorithms*)simplexAlgorithms;
   _Mesh_Algorithms_AssignFromXML( self, cf, data );
   _Mesh_SimplexAlgorithms_Init( self );
}

void _Mesh_SimplexAlgorithms_Build( void* simplexAlgorithms, void* data ) {
	Mesh_SimplexAlgorithms*	self = (Mesh_SimplexAlgorithms*)simplexAlgorithms;
	_Mesh_Algorithms_Build( self, data );
}

void _Mesh_SimplexAlgorithms_Initialise( void* simplexAlgorithms, void* data ) {
	Mesh_SimplexAlgorithms*	self = (Mesh_SimplexAlgorithms*)simplexAlgorithms;
	_Mesh_Algorithms_Initialise( self, data );
}

void _Mesh_SimplexAlgorithms_Execute( void* simplexAlgorithms, void* data ) {
	Mesh_SimplexAlgorithms*	self = (Mesh_SimplexAlgorithms*)simplexAlgorithms;
	_Mesh_Algorithms_Execute( self, data );
}

void _Mesh_SimplexAlgorithms_Destroy( void* simplexAlgorithms, void* data ) {
	Mesh_SimplexAlgorithms*	self = (Mesh_SimplexAlgorithms*)simplexAlgorithms;
	_Mesh_Algorithms_Destroy( self, data );
}


/*--------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/


/*----------------------------------------------------------------------------------------------------------------------------------
** Private Functions
*/

Bool alreadyDone( unsigned* inc, unsigned nInc, unsigned el ) {
	int i;

	for( i = 0; i < nInc; i++ )
		if( inc[i] == el )
			return True;

	return False;
}

Bool _Mesh_SimplexAlgorithms_SearchElements( void* simplexAlgorithms, double* point, unsigned* elInd ) {
	Mesh_SimplexAlgorithms*	self 	= (Mesh_SimplexAlgorithms*)simplexAlgorithms;
	Mesh*			mesh	= self->mesh;
	unsigned		nInc, *inc;
	unsigned		nearestVertex;
	unsigned		el_i, el_j, el_k;
	unsigned		dim;
	IArray*			eArray;
	unsigned		nEinc, *eInc;

	nearestVertex = Mesh_NearestVertex( mesh, point );
	Mesh_GetIncidence( mesh, MT_VERTEX, nearestVertex, MT_FACE, self->incArray ); /* assumption!!! */
	nInc = IArray_GetSize( self->incArray );
	inc = IArray_GetPtr( self->incArray );

	*elInd = (unsigned)-1;

	for( el_i = 0; el_i < nInc; el_i++ ) {
		if( Mesh_SimplexType_ElementHasPoint( mesh->elTypes[0], inc[el_i], point, &dim, elInd ) ) {
			return True;
		}
	}

	/* point is not within the set of elements incident on the nearest vertex, so grow out one level, and
	 * try the next closest set of elements to that vertex (topologically speaking) */
	eArray = IArray_New();
	for( el_i = 0; el_i < nInc; el_i++ ) {
		Mesh_GetIncidence( mesh, MT_FACE, inc[el_i], MT_FACE, eArray );
		nEinc = IArray_GetSize( eArray );
		eInc = IArray_GetPtr( eArray );
		for( el_j = 0; el_j < nEinc; el_j++ ) {
			if( alreadyDone( inc, nInc, eInc[el_j] ) )
				continue;

			if( Mesh_SimplexType_ElementHasPoint( mesh->elTypes[0], eInc[el_j], point, &dim, elInd ) ) {
				Stg_Class_Delete( eArray );
				return True;
			}
		}
	}
	Stg_Class_Delete( eArray );

/* this is the more exhaustive algorithm, problem is that it gives very poor performance. if we're not overly concerned 
 * with the quality of results (ie: we're only interpolating onto height maps), then this is not required, dave 19.02.10 */
/*
	int bucketSize = nInc;
	int b_i = 0;
	unsigned* bucket = Memory_Alloc_Array_Unnamed( unsigned, Mesh_GetDomainSize( mesh, MT_FACE ) );
	unsigned* added = Memory_Alloc_Array_Unnamed( unsigned, Mesh_GetDomainSize( mesh, MT_FACE ) );
	memset( added, 0, Mesh_GetDomainSize( mesh, MT_FACE )*sizeof(unsigned) );
	for( el_i = 0; el_i < nInc; el_i++ ) {
		bucket[el_i] = inc[el_i];
		added[inc[el_i]] = 1;
	}
	while( b_i < Mesh_GetDomainSize( mesh, MT_FACE ) ) {
		if( Mesh_SimplexType_ElementHasPoint( mesh->elTypes[0], bucket[b_i], point, &dim, elInd ) ) {
			Memory_Free( bucket );
			Memory_Free( added );
			return True;
		}

		Mesh_GetIncidence( mesh, MT_FACE, bucket[b_i], MT_FACE, self->incArray );
		nInc = IArray_GetSize( self->incArray );
		inc = IArray_GetPtr( self->incArray );
		el_j = 0;
		for( el_i = 0; el_i < nInc; el_i++ ) {
			if( !added[inc[el_i]] ) {
				bucket[bucketSize + el_j++] = inc[el_i];
				added[inc[el_i]] = 1;
			}
		}
		bucketSize += el_j;
		
		b_i++;
	}
*/

	return False; /* point is outside the domain */
}


