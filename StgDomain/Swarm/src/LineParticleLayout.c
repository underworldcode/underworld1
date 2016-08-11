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

#include <mpi.h>
#include <StGermain/StGermain.h>

#include <StgDomain/Geometry/Geometry.h>
#include <StgDomain/Shape/Shape.h>
#include <StgDomain/Mesh/Mesh.h>
#include <StgDomain/Utils/Utils.h>

#include "types.h"
#include "shortcuts.h"
#include "ParticleLayout.h"
#include "GlobalParticleLayout.h"
#include "LineParticleLayout.h"

#include "SwarmClass.h"
#include "StandardParticle.h"
#include "ShadowInfo.h"
#include "CellLayout.h"
#include "ElementCellLayout.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

const Type LineParticleLayout_Type = "LineParticleLayout";

LineParticleLayout* LineParticleLayout_New(
		Name             name,
      AbstractContext* context,
      CoordSystem      coordSystem,
      Bool             weightsInitialisedAtStartup,
      unsigned int     totalInitialParticles,
      double           averageInitialParticlesPerCell,
		Dimension_Index  dim,
		Index            vertexCount,
		Coord*           vertexList )
{
	LineParticleLayout* self = (LineParticleLayout*) _LineParticleLayout_DefaultNew( name );

   _ParticleLayout_Init( self, context, coordSystem, weightsInitialisedAtStartup );
   _GlobalParticleLayout_Init( self, totalInitialParticles, averageInitialParticlesPerCell );
	_LineParticleLayout_Init( self, dim, vertexCount, vertexList );
	return self;
}

LineParticleLayout* _LineParticleLayout_New(  LINEPARTICLELAYOUT_DEFARGS  )
{
	LineParticleLayout* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof( LineParticleLayout ) );
   self = (LineParticleLayout*)_GlobalParticleLayout_New(  GLOBALPARTICLELAYOUT_PASSARGS  );

		self->dim = dim;
		self->vertexCount = vertexCount;
		self->vertexList = vertexList;
	return self;
}


void _LineParticleLayout_Init( 
		void*                                            particleLayout,
		Dimension_Index                                  dim,
		Index                                            vertexCount,
		Coord*                                           vertexList )
{
	LineParticleLayout*     self        = (LineParticleLayout*) particleLayout;
	double                  totalLength = 0.0;
	Index                   segment_I;
	double                  length;
	
	assert( self->totalInitialParticles >= 2 );
	assert( vertexCount >= 2 );

	self->dim = dim;
	self->vertexCount = vertexCount;
	self->vertexList = Memory_Alloc_Array( Coord, vertexCount, "vertexList" );
	memcpy( self->vertexList, vertexList, sizeof(Coord) * vertexCount );

	self->segmentLengthList = Memory_Alloc_Array( double, vertexCount - 1, "segmentLengthList" );
	for ( segment_I = 0 ; segment_I < self->vertexCount - 1 ; segment_I++ ) {
		length = StGermain_DistanceBetweenPoints( self->vertexList[ segment_I + 1 ], self->vertexList[ segment_I ], dim );
		self->segmentLengthList[ segment_I ] = length;

		totalLength += length;
	}

	self->dx = totalLength/( (double) self->totalInitialParticles - 1.0 );
}


void* _LineParticleLayout_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                                _sizeOfSelf = sizeof(LineParticleLayout);
	Type                                                                        type = LineParticleLayout_Type;
	Stg_Class_DeleteFunction*                                                _delete = _LineParticleLayout_Delete;
	Stg_Class_PrintFunction*                                                  _print = _LineParticleLayout_Print;
	Stg_Class_CopyFunction*                                                    _copy = _LineParticleLayout_Copy;
	Stg_Component_DefaultConstructorFunction*                    _defaultConstructor = _LineParticleLayout_DefaultNew;
	Stg_Component_ConstructFunction*                                      _construct = _LineParticleLayout_AssignFromXML;
	Stg_Component_BuildFunction*                                              _build = _LineParticleLayout_Build;
	Stg_Component_InitialiseFunction*                                    _initialise = _LineParticleLayout_Initialise;
	Stg_Component_ExecuteFunction*                                          _execute = _LineParticleLayout_Execute;
	Stg_Component_DestroyFunction*                                          _destroy = _LineParticleLayout_Destroy;
	AllocationType                                                nameAllocationType = NON_GLOBAL;
	ParticleLayout_SetInitialCountsFunction*                       _setInitialCounts = _GlobalParticleLayout_SetInitialCounts;
	ParticleLayout_InitialiseParticlesFunction*                 _initialiseParticles = _GlobalParticleLayout_InitialiseParticles;
	CoordSystem                                                          coordSystem = GlobalCoordSystem;
	Bool                                                 weightsInitialisedAtStartup = False;
	GlobalParticleLayout_InitialiseParticleFunction*             _initialiseParticle = _LineParticleLayout_InitialiseParticle;
	Particle_Index                                             totalInitialParticles = 0;
	double                                            averageInitialParticlesPerCell = 0.0;
	Dimension_Index                                                              dim = 0;
	Index                                                                vertexCount = 0;
	Coord*                                                                vertexList = NULL;

	return (void*)_LineParticleLayout_New(  LINEPARTICLELAYOUT_PASSARGS  );
}

	
void _LineParticleLayout_Delete( void* particleLayout ) {
	LineParticleLayout* self = (LineParticleLayout*)particleLayout;

	_GlobalParticleLayout_Delete( self );

}

void _LineParticleLayout_Print( void* particleLayout, Stream* stream ) {
	LineParticleLayout* self  = (LineParticleLayout*)particleLayout;
	
	/* General info */
	Journal_Printf( stream, "LineParticleLayout (ptr): %p:\n", self );
	Stream_Indent( stream );
	
	/* Parent class info */
	_GlobalParticleLayout_Print( self, stream );
	
	/* LineParticleLayout */
	/*Journal_PrintArray( stream, self->startCoord, 3 );*/
	/*Journal_PrintArray( stream, self->endCoord, 3 );*/
	
	Stream_UnIndent( stream );
}


void* _LineParticleLayout_Copy( void* particleLayout, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
	LineParticleLayout*		self                    = (LineParticleLayout*)particleLayout;
	LineParticleLayout*		newLineParticleLayout;
	
	newLineParticleLayout = _GlobalParticleLayout_Copy( self, dest, deep, nameExt, ptrMap );

	/*memcpy( newLineParticleLayout->startCoord, self->startCoord, sizeof(Coord) );*/
	/*memcpy( newLineParticleLayout->endCoord, self->endCoord, sizeof(Coord) );*/

	return (void*)newLineParticleLayout;
}


void _LineParticleLayout_AssignFromXML( void* particleLayout, Stg_ComponentFactory *cf, void* data ){
	LineParticleLayout*     self                    = (LineParticleLayout*) particleLayout;
	Particle_Index          totalInitialParticles;
	Dimension_Index         dim;
	Coord*                  vertexList;
	Index                   vertexCount;
	Index                   vertex_I;
	Dictionary*             dictionary;
	Stream*                 stream;
	Dictionary_Entry_Value* entry;
	Dictionary_Entry_Value* list;
	double*                 coord;
	
	dictionary              = Dictionary_GetDictionary( cf->componentDict, self->name );
	/*stream                  = cf->infoStream;*/
	stream                  = Journal_MyStream( Info_Type, self );

   _GlobalParticleLayout_AssignFromXML( self, cf, data );

	dim = Stg_ComponentFactory_GetRootDictUnsignedInt( cf, (Dictionary_Entry_Key)"dim", 0  );

   /* Read list of vertices of each of the different line segments from the dictionary */
   list = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"vertices" );
   Journal_Firewall( list != NULL, Journal_MyStream(Error_Type, self ), "\n\n*** ERROR ***\nIn %s (Type %s)\nCan't find a definition of 'vertices'\nIn the xml file add \n\t<list name=\"vertices\">\n\t\t...\n\t</list>\n in the definition of %s\n\n", self->name, self->type, self->name );
   vertexCount = Dictionary_Entry_Value_GetCount( list  );
   vertexList = Memory_Alloc_Array( Coord , vertexCount, "Vertex Array" );
   memset( vertexList, 0, vertexCount * sizeof(Coord) );

	Stream_Indent( stream );
	for ( vertex_I = 0 ; vertex_I < vertexCount ; vertex_I++) { 
		entry = Dictionary_Entry_Value_GetElement( list, vertex_I );
		coord = vertexList[vertex_I];
		/* Read Vertex */
		coord[ I_AXIS ] = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( entry, (Dictionary_Entry_Key)"x") );
		coord[ J_AXIS ] = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( entry, (Dictionary_Entry_Key)"y"));
		if (dim == 3  ) {
			coord[ K_AXIS ] = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( entry, (Dictionary_Entry_Key)"z") );
		}

		/* Print Position */
		Journal_PrintfL( stream, 2, "(%0.3g, %0.3g, %0.3g)\n", coord[ I_AXIS ], coord[ J_AXIS ], coord[ K_AXIS ] );
	}
	Stream_UnIndent( stream );

	totalInitialParticles = Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"totalInitialParticles", 2  );

	_LineParticleLayout_Init( self, dim, vertexCount, vertexList );

	/* Delete this vertexList because _LineParticleLayout_Init made a copy of it */
	Memory_Free( vertexList );
}
	
void _LineParticleLayout_Build( void* particleLayout, void* data ) {
}
void _LineParticleLayout_Initialise( void* particleLayout, void* data ) {
}
void _LineParticleLayout_Execute( void* particleLayout, void* data ) {
}
void _LineParticleLayout_Destroy( void* particleLayout, void* data ) {
	LineParticleLayout*        self             = (LineParticleLayout*)particleLayout;

	Memory_Free( self->vertexList );
	Memory_Free( self->segmentLengthList );

   _GlobalParticleLayout_Destroy( self, data );
}

void _LineParticleLayout_InitialiseParticle( void* particleLayout, void* _swarm, Particle_Index newParticle_I, void* _particle )
{
	LineParticleLayout*        self             = (LineParticleLayout*)particleLayout;
	Axis                       axis_I;
	Index                      segment_I;
	double                     lengthFromVertex = self->dx * (double) newParticle_I;
	double                     factor;
	double                     segmentLength    = 0.0;
	GlobalParticle*            particle         = (GlobalParticle*)_particle;

	/* Find which segment the particle is in */
	for ( segment_I = 0 ; segment_I < self->vertexCount - 1 ; segment_I++ ) {
		segmentLength = self->segmentLengthList[ segment_I ];
		if ( lengthFromVertex < self->segmentLengthList[ segment_I ] )
			break;
		lengthFromVertex -= segmentLength;
	}
	
	/* Check if we are the last particle */
	if ( segment_I == self->vertexCount - 1 ) {
		memcpy( particle->coord, self->vertexList[ segment_I ], sizeof(Coord) );
		return;
	}

	factor = lengthFromVertex / segmentLength;

	memcpy( particle->coord, self->vertexList[ segment_I ], sizeof(Coord) );
	for ( axis_I = 0 ; axis_I < self->dim ; axis_I++ ) {
		particle->coord[axis_I] += factor 
			* ( self->vertexList[ segment_I + 1 ][ axis_I ] - self->vertexList[ segment_I ][ axis_I ] )  ;
	}
}



