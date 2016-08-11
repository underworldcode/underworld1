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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/** \file
** <b>Role:</b>
**	Binary Tree class for objects.
**
** <b>Assumptions:</b>
**	None
**
** <b>Comments:</b>
**	None
**
**
**/

#include <Base/Foundation/Foundation.h>
#include <Base/IO/IO.h>

#include "types.h"
#include "Heap.h"
#include "MaxHeap.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

const Type MaxHeap_Type = "MaxHeap";

MaxHeap* MaxHeap_New( void **keys, int keyLength, int numArrayElements, 
						Heap_KeySwapFunction *keySwap,
						Heap_CompareFunction *keyCompare,
						Heap_ExtendArrayFunc *extendArray )
{
	/* Variables set in this function */
	SizeT                          _sizeOfSelf = sizeof( MaxHeap );
	Type                                  type = MaxHeap_Type;
	Stg_Class_DeleteFunction*          _delete = _MaxHeap_DeleteFunc;
	Stg_Class_PrintFunction*            _print = _MaxHeap_PrintFunc;
	Stg_Class_CopyFunction*              _copy = NULL;
	Heap_HeapifyFunction*              heapify = MaxHeap_Heapify;
	Heap_ExtractFunction*              extract = (Heap_ExtractFunction*)MaxHeap_Extract;
	Heap_InsertFunction*                insert = MaxHeap_InsertFunc;
	Heap_KeySwapFunction*      keySwapFunction = keySwap;
	Heap_CompareFunction*      compareFunction = keyCompare;

	return _MaxHeap_New(  MAXHEAP_PASSARGS  );
}

MaxHeap* _MaxHeap_New(  MAXHEAP_DEFARGS  )
{
	_Heap *heap;

	heap = _Heap_New(  HEAP_PASSARGS  );

	_MaxHeap_Init( (MaxHeap*)heap );
	return ( MaxHeap* )heap;
}

void _MaxHeap_Init( MaxHeap* self )
{

}
	
void MaxHeap_Heapify( _Heap *heap, int index )
{
	int l, r, largest;

	assert( heap );
	
	l = _Heap_Left( heap, index );
	r = _Heap_Right( heap, index );

	if( (l <= heap->numHeapElements) &&
			(heap->compare( heap->keys[l-1], heap->keys[index-1]) == 1) ){
		largest = l;
	}
	else{
		largest = index;
	}

	if( (r <= heap->numHeapElements) &&
			(heap->compare( heap->keys[r-1], heap->keys[largest-1] ) == 1) ){
		largest = r;
	}

	if( largest != index ){

		heap->keySwap( &(heap->keys[index-1]), &(heap->keys[largest-1]) );
		
		MaxHeap_Heapify( heap, largest );
	}
}

void* MaxHeap_Extract( void* heap ) {
	MaxHeap*	self = (MaxHeap*)heap;
	void*		max;
	
	assert( self );

	if( self->numHeapElements < 1 ){
		fprintf( stderr, "Heap Underflow..!!\n Aborting..!!\n" );
		assert( 0 );
	}

	max = self->keys[0];

	self->keySwap( &(self->keys[0]), &(self->keys[self->numHeapElements-1]) );
	self->numHeapElements -= 1;

	self->heapify( (_Heap*)self, 1 );

	return max;
}

	/** Stg_Class_Delete interface. */
	/** Stg_Class delete function */
void _MaxHeap_DeleteFunc ( void *heap )
{
	MaxHeap *self = (MaxHeap*)heap;

	assert( self );

	_Heap_DeleteFunc( self );
}
	
	/** Print interface. */
	/** Stg_Class print function */
void _MaxHeap_PrintFunc ( void *heap, Stream* stream )
{
	MaxHeap *self = (MaxHeap*)heap;
	assert( self );

	/* print parent */
	_Heap_PrintFunc( (void*) self, stream );

	/* general info */
	Journal_Printf( stream, "MaxHeap (ptr): (%p)\n", self );

	/* Virtual Info */
	
}

	/** Public functions */

void MaxHeap_InsertFunc( _Heap *maxHeap, void *key )
{
	int i = 0;

	assert( maxHeap );
	assert( key );

	if( maxHeap->numHeapElements >= maxHeap->numArrayElements ){

		maxHeap->numArrayElements += 1;		
		maxHeap->keys = maxHeap->extendArray( maxHeap->numArrayElements, &(maxHeap->keys) );		
	}

	maxHeap->numHeapElements += 1;

	maxHeap->keys[maxHeap->numHeapElements-1] = key;

	i = maxHeap->numHeapElements;
	while( ( (i > 1) && (maxHeap->compare( maxHeap->keys[_Heap_Parent( maxHeap, i )-1], maxHeap->keys[i-1] ) == -1) ) ){
		
		maxHeap->keySwap( &(maxHeap->keys[i-1]), &(maxHeap->keys[_Heap_Parent( maxHeap, i )-1]) );

		i = _Heap_Parent( maxHeap, i );
	}
}



