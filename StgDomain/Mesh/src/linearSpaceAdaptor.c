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

#include <StgDomain/StgDomain.h>

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
#include "linearSpaceAdaptor.h"


typedef double (LinearSpaceAdaptor_DeformFunc)( LinearSpaceAdaptor* self, Mesh* mesh, 
																unsigned* globalSize, unsigned vertex, unsigned* vertexInds );

/* Textual name of this class */
const Type LinearSpaceAdaptor_Type = "LinearSpaceAdaptor";

/* My private functions */
void   LinearSpaceAdaptor_FillTable( linearSpaceAdaptor_Segment* table, unsigned size  );
double LinearSpaceAdaptor_MapPoint( linearSpaceAdaptor_Segment* table, unsigned size, double x );


/*---------------------------------------------------------------------------------------------------
** Constructors
*/
LinearSpaceAdaptor* LinearSpaceAdaptor_New( Name name, AbstractContext* context ) {
  /* Variables set in this function */
  SizeT                                              _sizeOfSelf = sizeof(LinearSpaceAdaptor);
  Type                                                      type = LinearSpaceAdaptor_Type;
  Stg_Class_DeleteFunction*                              _delete = _LinearSpaceAdaptor_Delete;
  Stg_Class_PrintFunction*                                _print = _LinearSpaceAdaptor_Print;
  Stg_Class_CopyFunction*                                  _copy = NULL;
  Stg_Component_DefaultConstructorFunction*  _defaultConstructor = (void* (*)(Name))_LinearSpaceAdaptor_New;
  Stg_Component_ConstructFunction*                    _construct = _LinearSpaceAdaptor_AssignFromXML;
  Stg_Component_BuildFunction*                            _build = _LinearSpaceAdaptor_Build;
  Stg_Component_InitialiseFunction*                  _initialise = _LinearSpaceAdaptor_Initialise;
  Stg_Component_ExecuteFunction*                        _execute = _LinearSpaceAdaptor_Execute;
  Stg_Component_DestroyFunction*                        _destroy = _LinearSpaceAdaptor_Destroy;
  AllocationType                              nameAllocationType = NON_GLOBAL;
  MeshGenerator_SetDimSizeFunc*                   setDimSizeFunc = _MeshGenerator_SetDimSize;
  MeshGenerator_GenerateFunc*                       generateFunc = LinearSpaceAdaptor_Generate;

  LinearSpaceAdaptor* self = _LinearSpaceAdaptor_New(  LINEARSPACEADAPTOR_PASSARGS  );

  _MeshGenerator_Init( (MeshGenerator*)self, context );
  _MeshAdaptor_Init( (MeshAdaptor*)self );
  _LinearSpaceAdaptor_Init( self );

  return self;
}


LinearSpaceAdaptor* _LinearSpaceAdaptor_New(  LINEARSPACEADAPTOR_DEFARGS  ) {
  LinearSpaceAdaptor* self;
	
  /* Allocate memory */
  assert( _sizeOfSelf >= sizeof(LinearSpaceAdaptor) );
  self = (LinearSpaceAdaptor*)_MeshAdaptor_New(  MESHADAPTOR_PASSARGS  );

  /* Virtual info */
  return self;
}


void _LinearSpaceAdaptor_Init( void* _self ) {
  LinearSpaceAdaptor* self = (LinearSpaceAdaptor*)_self;

  if( self->nSegmentsx > 0 )
	 LinearSpaceAdaptor_FillTable( self->tablex, self->nSegmentsx );
  if( self->nSegmentsy > 0 )
	 LinearSpaceAdaptor_FillTable( self->tabley, self->nSegmentsy );
  if( self->nSegmentsz > 0 )
	 LinearSpaceAdaptor_FillTable( self->tablez, self->nSegmentsz );
}


/*----------------------------------------------------------------------------------------------------------
** Virtual functions
*/
void _LinearSpaceAdaptor_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
  LinearSpaceAdaptor* self = (LinearSpaceAdaptor*)_self;
  Dictionary* dictionary  = Dictionary_GetDictionary( cf->componentDict, self->name );
  Dictionary_Entry_Value* optionsList = NULL;
  Dictionary_Entry_Value* optionSet = NULL;
  linearSpaceAdaptor_Segment* seg = NULL;
  Index segmentCount;
  Index segment_I;
  AbstractContext* context;

  assert( self );
  assert( cf );

  /* Call parent construct. */
  _MeshAdaptor_AssignFromXML( self, cf, data );

  context = (AbstractContext*)Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", AbstractContext, False, data );
  if( !context  )
	 context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", AbstractContext, True, data );

  self->loadFromCheckPoint = context->loadFromCheckPoint;

  if( self->loadFromCheckPoint  )
	 return;

  self->minX = Dictionary_Entry_Value_AsDouble( Dictionary_Get( cf->rootDict, (Dictionary_Entry_Key)"minX" ) );
  self->maxX = Dictionary_Entry_Value_AsDouble( Dictionary_Get( cf->rootDict, (Dictionary_Entry_Key)"maxX" ) );
  self->minY = Dictionary_Entry_Value_AsDouble( Dictionary_Get( cf->rootDict, (Dictionary_Entry_Key)"minY" ) );
  self->maxY = Dictionary_Entry_Value_AsDouble( Dictionary_Get( cf->rootDict, (Dictionary_Entry_Key)"maxY" ) );
  self->minZ = Dictionary_Entry_Value_AsDouble( Dictionary_Get( cf->rootDict, (Dictionary_Entry_Key)"minZ" ) );
  self->maxZ = Dictionary_Entry_Value_AsDouble( Dictionary_Get( cf->rootDict, (Dictionary_Entry_Key)"maxZ" ) );

  /* Read mapping functions - X axis*/
  optionsList = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"mappingFunctionX" );

  if( optionsList ) {
	 segmentCount = Dictionary_Entry_Value_GetCount(optionsList );
	 self->nSegmentsx = segmentCount;

	 self->tablex = Memory_Alloc_Array( linearSpaceAdaptor_Segment , segmentCount, "mapping table x" );
	 memset( self->tablex, 0, segmentCount * sizeof(linearSpaceAdaptor_Segment) );

	 for ( segment_I = 0 ; segment_I < segmentCount ; segment_I++) { 
		optionSet = Dictionary_Entry_Value_GetElement(optionsList, segment_I );
		seg = &(self->tablex[segment_I]);
		seg->x = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( optionSet, (Dictionary_Entry_Key)"point" ) );
		seg->y = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( optionSet, (Dictionary_Entry_Key)"mappedTo" ) );
	 }
  } else {
	 self->nSegmentsx = 0;
  }

  /* Read mapping functions - Y axis*/
  optionsList = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"mappingFunctionY" );
	
  if( optionsList ) {
	 segmentCount = Dictionary_Entry_Value_GetCount(optionsList );
	 self->nSegmentsy = segmentCount;

	 self->tabley = Memory_Alloc_Array( linearSpaceAdaptor_Segment , segmentCount, "mapping table y" );
	 memset( self->tabley, 0, segmentCount * sizeof(linearSpaceAdaptor_Segment) );

	 for ( segment_I = 0; segment_I < segmentCount; segment_I++) { 
		optionSet = Dictionary_Entry_Value_GetElement(optionsList, segment_I );
		seg = &(self->tabley[segment_I]);
		seg->x = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( optionSet, (Dictionary_Entry_Key)"point" ) );
		seg->y = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( optionSet, (Dictionary_Entry_Key)"mappedTo" ) );
	 }
  } else {
	 self->nSegmentsy = 0;
  }

  /* Read mapping functions - Z axis*/
  optionsList = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"mappingFunctionZ" );
	
  if( optionsList && ((DomainContext*)context)->dim==3 ) {
	 segmentCount = Dictionary_Entry_Value_GetCount(optionsList );
	 self->nSegmentsz = segmentCount;

	 self->tablez = Memory_Alloc_Array( linearSpaceAdaptor_Segment , segmentCount, "mapping table z" );
	 memset( self->tablez, 0, segmentCount * sizeof(linearSpaceAdaptor_Segment) );

	 for ( segment_I = 0 ; segment_I < segmentCount ; segment_I++) { 
		optionSet = Dictionary_Entry_Value_GetElement(optionsList, segment_I );
		seg = &(self->tablez[segment_I]);
		seg->x = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( optionSet, (Dictionary_Entry_Key)"point" ) );
		seg->y = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( optionSet, (Dictionary_Entry_Key)"mappedTo" ) );
	 }
  } else {
	 self->nSegmentsz = 0;
  }

  _LinearSpaceAdaptor_Init( self );
}


void _LinearSpaceAdaptor_Print( void* _self, Stream* stream ) {
  LinearSpaceAdaptor* self = (LinearSpaceAdaptor*)_self;
	
  /* Set the Journal for printing informations */
  Stream* adaptorStream;
  adaptorStream = Journal_Register( InfoStream_Type, (Name)"LinearSpaceAdaptorStream"  );

  /* Print parent */
  Journal_Printf( stream, "LinearSpaceAdaptor (ptr): (%p)\n", self );
  _MeshAdaptor_Print( self, stream );
}


void _LinearSpaceAdaptor_Delete( void* _self ) {
  LinearSpaceAdaptor* self = (LinearSpaceAdaptor*)_self;

  _MeshAdaptor_Delete( self );
}


void _LinearSpaceAdaptor_Build( void* _self, void* data ) {
  LinearSpaceAdaptor* self = (LinearSpaceAdaptor*)_self;

  _MeshAdaptor_Build( self, data );
}


void _LinearSpaceAdaptor_Initialise( void* _self, void* data ) {
  LinearSpaceAdaptor* self = (LinearSpaceAdaptor*)_self;

  _MeshAdaptor_Initialise( self, data );
}


void _LinearSpaceAdaptor_Execute( void* _self, void* data ) {
}


void _LinearSpaceAdaptor_Destroy( void* _self, void* data ) {
  LinearSpaceAdaptor* self = (LinearSpaceAdaptor*)_self;

  if( self->nSegmentsx > 0 )
	 Memory_Free( self->tablex );
  if( self->nSegmentsy > 0 )
	 Memory_Free( self->tabley );
  if( self->nSegmentsz > 0 )
	 Memory_Free( self->tablez );

  _MeshAdaptor_Destroy( self, data );
}


/*--------------------------------------------------------------------------------------------
** Public Functions
*/
void LinearSpaceAdaptor_Generate( void* _self, void* _mesh, void* data ) {
  LinearSpaceAdaptor* self = (LinearSpaceAdaptor*)_self;
  Mesh* mesh = (Mesh*)_mesh;
  const Sync* sync;
  double	x;
  Index  n_i;

  /* Build base mesh (assumed to be cartesian) */
  MeshGenerator_Generate( self->generator, mesh, data );

  if( self->loadFromCheckPoint )
    return;

  /* Loop over domain nodes. */
  sync = IGraph_GetDomain( mesh->topo, MT_VERTEX );
  
  if( self->nSegmentsx > 0 ) {
    for( n_i = 0; n_i < Sync_GetNumDomains( sync ); n_i++ ) {
      /* get the x coord */
      x = mesh->verts[n_i][I_AXIS];      

      if( x != self->minX && x != self->maxX ) {	      
		  /* normalize the x coord*/
		  x = (x - self->minX) / (self->maxX - self->minX);
		  /* map the normalized point */
		  x = LinearSpaceAdaptor_MapPoint( self->tablex, self->nSegmentsx, x  );
		  /* denormalize mapped point */
		  x = (self->maxX - self->minX)*x + self->minX;
		  /* move the node */
		  mesh->verts[n_i][I_AXIS] = x;
      }
    }
  }

  if( self->nSegmentsy > 0 ) {
    for( n_i = 0; n_i < Sync_GetNumDomains( sync ); n_i++ ) {
      /* get the y coord */
      x = mesh->verts[n_i][J_AXIS];      

      if( x != self->minY && x != self->maxY ) {
		  /* normalize the y coord*/
		  x = (x - self->minY) / (self->maxY - self->minY);
		  /* map the normalized point */
		  x = LinearSpaceAdaptor_MapPoint( self->tabley, self->nSegmentsy, x  );
		  /* denormalize mapped point */
		  x = (self->maxY - self->minY)*x + self->minY;
		  /* move the node */
		  mesh->verts[n_i][J_AXIS] = x;
      }
    }
  }  

  if( self->nSegmentsz > 0 ) {
    for( n_i = 0; n_i < Sync_GetNumDomains( sync ); n_i++ ) {
      /* get the x coord */
      x = mesh->verts[n_i][K_AXIS];

      if( x != self->minZ && x != self->maxZ ) {	      
		  /* normalize the z coord*/
		  x = (x - self->minZ) / (self->maxZ - self->minZ);
		  /* map the normalized point */
		  x = LinearSpaceAdaptor_MapPoint( self->tablez, self->nSegmentsz, x  );
		  /* denormalize mapped point */
		  x = (self->maxZ - self->minZ)*x + self->minZ;
		  /* move the node */
		  mesh->verts[n_i][K_AXIS] = x;
      }
    }
  }
}


/*--------------------------------------------------------------------------------------------
** Private Functions
*/
void LinearSpaceAdaptor_FillTable( linearSpaceAdaptor_Segment* table, unsigned size ) {
  Index i;

  table[0].p  = table[0].y / table[0].x;
  table[0].y0 = 0; 
  for( i = 1; i < size; i++ ){
    table[i].p  = (table[i].y - table[i-1].y) / (table[i].x - table[i-1].x);
    table[i].y0 = table[i-1].y;
  }
}


double LinearSpaceAdaptor_MapPoint( linearSpaceAdaptor_Segment* table, unsigned size, double x ) {
  Index i;

  if( x < table[0].x )
    return x * table[0].p + table[0].y0;

  for( i = 1; i < size; i++ ){
    if( x < table[i].x )
      return (x - table[i-1].x) * table[i].p + table[i].y0;
  }
  return 0;
}



