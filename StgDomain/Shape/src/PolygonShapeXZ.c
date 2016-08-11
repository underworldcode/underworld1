/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org) ) 
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

#include "types.h"
#include "ShapeClass.h"
#include "PolygonShapeXZ.h"

#include <assert.h>
#include <string.h>
#include <math.h>

/* Textual name of this class */
const Type PolygonShapeXZ_Type = "PolygonShapeXZ";

/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/

PolygonShapeXZ* PolygonShapeXZ_New(
		Name                                  name,
		Dimension_Index                       dim,
		Coord_List                            vertexList,
		Index                                 vertexCount,
		XYZ                                   start,
		XYZ                                   end )
{ 
	PolygonShapeXZ* self = (PolygonShapeXZ*)_PolygonShapeXZ_DefaultNew( name );

   _Stg_Shape_Init( self, dim, NULL, False, 0.0, 0.0, 0.0);
   _PolygonShapeXZ_Init( self, vertexList, vertexCount, start, end );
	return self;
}

PolygonShapeXZ* _PolygonShapeXZ_New(  POLYGONSHAPEXZ_DEFARGS  )
{
	PolygonShapeXZ* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(PolygonShapeXZ) );
	self = (PolygonShapeXZ*)_Stg_Shape_New(  STG_SHAPE_PASSARGS  );
	
	/* General info */
	return self;
}

void _PolygonShapeXZ_Init( void* polygon, Coord_List vertexList, Index vertexCount, XYZ start, XYZ end ) {
	PolygonShapeXZ* self = (PolygonShapeXZ*)polygon;
   /* original coordinate storage */
   XYZ           normal, centroid;
   double alpha, beta, gamma;
   double plane_constant;
   int v_i;

   self->vertexList = Memory_Alloc_Array( Coord, vertexCount, "vertexList" );
	memcpy( self->vertexList , vertexList, sizeof(Coord) * vertexCount );
	self->vertexCount = vertexCount;
	memcpy( self->start , start, sizeof(XYZ) );
	memcpy( self->end , end, sizeof(XYZ) );

	centroid[0] = centroid[1] = centroid[2] = 0.0;
	normal[0] = normal[1] = normal[2] = 0.0;
	normal[1] = 1.0;

   /* using any point, find plane eqn */
   plane_constant = normal[0]*vertexList[0][0] + normal[1]*vertexList[0][1] + normal[2]*vertexList[0][2];
   
   /* find centroid and check each vertex is satisfies this plane equation */
   for( v_i = 0 ; v_i < vertexCount ; v_i++ ) {
     double tmp = normal[0]*vertexList[v_i][0] + normal[1]*vertexList[v_i][1] + normal[2]*vertexList[v_i][2];
     if( fabs(tmp - plane_constant ) > 1e-5 ) {
         printf("Error in plane check, vertex %d, with coord (%g, %g, %g) has a plane constant of %g, where as other have plane constant of %g\n\n", v_i, vertexList[v_i][0], vertexList[v_i][1], vertexList[v_i][2], tmp, plane_constant );
         }
      
      /* do centroid calculation */
      centroid[0] += vertexList[v_i][0];
      centroid[1] += vertexList[v_i][1];
      centroid[2] += vertexList[v_i][2];
   }

   self->centroid[0] = centroid[0] / vertexCount;
   self->centroid[1] = 0.5 * (self->start[1]+self->end[1]);
   self->centroid[2] = centroid[2] / vertexCount;


   /* setup other rotations */
   alpha = 0.0;
   beta = 0.0;
   gamma = 0.0;
}
	
/*------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _PolygonShapeXZ_Delete( void* polygon ) {
	PolygonShapeXZ*       self = (PolygonShapeXZ*)polygon;
	
	/* Delete parent */
	_Stg_Shape_Delete( self );
}


void _PolygonShapeXZ_Print( void* polygon, Stream* stream ) {
	PolygonShapeXZ* self = (PolygonShapeXZ*)polygon;
	
	/* Print parent */
	_Stg_Shape_Print( self, stream );
}



void* _PolygonShapeXZ_Copy( void* polygon, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
	PolygonShapeXZ*	self = (PolygonShapeXZ*)polygon;
	PolygonShapeXZ*	newPolygonShapeXZ;
	
	newPolygonShapeXZ = (PolygonShapeXZ*)_Stg_Shape_Copy( self, dest, deep, nameExt, ptrMap );

	newPolygonShapeXZ->vertexList = Memory_Alloc_Array( Coord, self->vertexCount, "vertexList" );
	memcpy( newPolygonShapeXZ->vertexList , self->vertexList, sizeof(Coord) * self->vertexCount );

	newPolygonShapeXZ->vertexList  = self->vertexList;
	newPolygonShapeXZ->vertexCount = self->vertexCount;
	memcpy( newPolygonShapeXZ->start, self->start, sizeof(XYZ) );
	memcpy( newPolygonShapeXZ->end, self->end, sizeof(XYZ) );
	
	return (void*)newPolygonShapeXZ;
}

void* _PolygonShapeXZ_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                  _sizeOfSelf = sizeof(PolygonShapeXZ);
	Type                                                          type = PolygonShapeXZ_Type;
	Stg_Class_DeleteFunction*                                  _delete = _PolygonShapeXZ_Delete;
	Stg_Class_PrintFunction*                                    _print = _PolygonShapeXZ_Print;
	Stg_Class_CopyFunction*                                      _copy = _PolygonShapeXZ_Copy;
	Stg_Component_DefaultConstructorFunction*      _defaultConstructor = _PolygonShapeXZ_DefaultNew;
	Stg_Component_ConstructFunction*                        _construct = _PolygonShapeXZ_AssignFromXML;
	Stg_Component_BuildFunction*                                _build = _PolygonShapeXZ_Build;
	Stg_Component_InitialiseFunction*                      _initialise = _PolygonShapeXZ_Initialise;
	Stg_Component_ExecuteFunction*                            _execute = _PolygonShapeXZ_Execute;
	Stg_Component_DestroyFunction*                            _destroy = _PolygonShapeXZ_Destroy;
	Stg_Shape_IsCoordInsideFunction*                    _isCoordInside = _PolygonShapeXZ_IsCoordInside;
	Stg_Shape_CalculateVolumeFunction*                _calculateVolume = _PolygonShapeXZ_CalculateVolume;
	Stg_Shape_DistanceFromCenterAxisFunction*  _distanceFromCenterAxis = _PolygonShapeXZ_DistanceFromCenterAxis;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*) _PolygonShapeXZ_New(  POLYGONSHAPEXZ_PASSARGS  );
}


void _PolygonShapeXZ_AssignFromXML( void* polygon, Stg_ComponentFactory* cf, void* data ) {
	PolygonShapeXZ*           self       = (PolygonShapeXZ*)polygon;
	Index                   vertexCount;
	Index                   vertex_I;
	Coord_List              vertexList;
	XYZ                     start;
	XYZ                     end;
	double*                 coord;
	Dictionary_Entry_Value* optionSet;
	Dictionary_Entry_Value* optionsList;
	Dictionary*             dictionary  = Dictionary_GetDictionary( cf->componentDict, self->name );
	Stream*                 stream      = cf->infoStream;
	Stream*                 errorStream = Journal_Register( Error_Type, (Name)self->type  );
	
	_Stg_Shape_AssignFromXML( self, cf, data );

	start[J_AXIS] = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"startY", 0.0  );
	end[J_AXIS]   = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"endY", 0.0  );

   /* check if depth parameters are ok */
   if( start[1] > end[1] ) {
      /* swap start and end, because algorithm assumes start is less than end */
      double tmp;
      tmp = start[1]; 
      start[1] = end[1];
      end[1] = tmp;
   }

	optionsList = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"vertices" );
	
	Journal_Firewall( optionsList != NULL , errorStream, 
			"\n\nUnable to find vertices list required to build shape '%s' named '%s'.\n"
			"Please check that you have specified a vertices list, and that the list is named correctly.\n\n\n",
			self->type, self->name );

	vertexCount = Dictionary_Entry_Value_GetCount(optionsList );
	Journal_Firewall( vertexCount >= 3, errorStream, 
			"Too few vertices given in trying to build shape '%s' named '%s'.\n"
			"A polygon needs at least three vertices.\n",
			self->type, self->name );

	/* Allocate space */
	vertexList = Memory_Alloc_Array( Coord , vertexCount, "Vertex Array" );
	memset( vertexList, 0, vertexCount * sizeof(Coord) );

	Stream_Indent( stream );
	for ( vertex_I = 0 ; vertex_I < vertexCount ; vertex_I++) { 
		optionSet = Dictionary_Entry_Value_GetElement(optionsList, vertex_I );
		coord = vertexList[vertex_I];
		/* Read Vertex */
			coord[ I_AXIS ] = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( optionSet, (Dictionary_Entry_Key)"x"));
			coord[ J_AXIS ] = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( optionSet, (Dictionary_Entry_Key)"y"));
			coord[ K_AXIS ] = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( optionSet, (Dictionary_Entry_Key)"z") );

		/* Print Position */
			Journal_PrintfL( stream, 2, "(%0.3g, %0.3g, %0.3g)\n", coord[I_AXIS], coord[J_AXIS], coord[K_AXIS] );
	}
	Stream_UnIndent( stream );

	_PolygonShapeXZ_Init( self, vertexList, vertexCount, start, end );

	Memory_Free( vertexList );
}

void _PolygonShapeXZ_Build( void* polygon, void* data ) {
	PolygonShapeXZ*	self = (PolygonShapeXZ*)polygon;

	_Stg_Shape_Build( self, data );
}
void _PolygonShapeXZ_Initialise( void* polygon, void* data ) {
	PolygonShapeXZ*	self = (PolygonShapeXZ*)polygon;
	
	_Stg_Shape_Initialise( self, data );
}
void _PolygonShapeXZ_Execute( void* polygon, void* data ) {
	PolygonShapeXZ*	self = (PolygonShapeXZ*)polygon;
	
	_Stg_Shape_Execute( self, data );
}
void _PolygonShapeXZ_Destroy( void* polygon, void* data ) {
	PolygonShapeXZ*	self = (PolygonShapeXZ*)polygon;

	Coord_List     vertexList = self->vertexList;
	Memory_Free( vertexList );

	_Stg_Shape_Destroy( self, data );
}

/*--------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/
	
/*---------------------------------------------------------------------------------------------------------------------
** Private Member functions
*/
/* Algorithm describe Paul Bourke's page http://astronomy.swin.edu.au/~pbourke/geometry/insidepoly/ (see solution 2)
 *
 * Algorithm works by summing the angles between the test coordinate and each pair of vertices that make up an edge 
 * in the polygon. An inside point will give an angle of 2pi and and outside point will give an angle of 0 */

Bool _PolygonShapeXZ_IsCoordInside( void* polygon, Coord coord ) {
	PolygonShapeXZ*        self                = (PolygonShapeXZ*) polygon;
	Index           vertexCount         = self->vertexCount;
   /* original coordinate storage */
	Coord_List      o_vertexList  = self->vertexList;
   /* transformed coordinate storage */
	XYZ             vectorToStartVertex = { 0.0, 0.0, 0.0 };
	XYZ             vectorToEndVertex   = { 0.0, 0.0, 0.0 };
	XYZ             crossproduct        = { 0.0, 0.0, 0.0 };
	double          currAngle;
	double          totalAngle          = 0.0;
	Index           vertex_I;
	double*         startVertex;
	double*         endVertex;

	// StGermain_RotateAroundPoint( coord, self->rotationMatrix, self->centroid, newCoord ) ;
	
	// fprintf(stdout,"Inside/Outside check %g,%g,%g\n",coord[0],coord[1],coord[2]);

	/* Check to make sure that the coordinate is within startY and endY  */
	if ( ( coord[ 1 ] < self->start[1] || coord[ 1 ] > self->end[1] )) {
		// fprintf(stderr,"%g is not in Y range %g -> %g\n", coord[1], self->start[1], self->end[1] );
		return False;	
	}

	// fprintf(stderr,"%g is in Y range %g -> %g\n", coord[1], self->start[1], self->end[1] );
	

	for ( vertex_I = 0 ; vertex_I < vertexCount ; vertex_I++ ) {
		/* Get vertices of current edge */
		startVertex = o_vertexList[ vertex_I ];
		endVertex   = o_vertexList[ (vertex_I + 1) % vertexCount ];

		/* Work out vectors */
		StGermain_VectorSubtraction( vectorToStartVertex, coord, startVertex, 3 );
		StGermain_VectorSubtraction( vectorToEndVertex,   coord, endVertex,  3 );

		vectorToStartVertex[ 1 ] = 0;
		vectorToEndVertex[ 1 ] = 0;

		/* Work out angle - just by doing dot product - will always be positive */
		currAngle = StGermain_AngleBetweenVectors( vectorToStartVertex, vectorToEndVertex, 3 );

		/* Work out 'sign' of angle by working out cross product */
		StGermain_VectorCrossProduct( crossproduct, vectorToEndVertex, vectorToStartVertex );

		if ( crossproduct[ 1 ] > 0.0 )
			totalAngle += currAngle;
		else
			totalAngle -= currAngle;
	}	

	/* work out whether the coord is within the polygon */
	if ( fabs( totalAngle ) < M_PI ) 
		return False;
	else  {
		return True;
	}
}


double _PolygonShapeXZ_CalculateVolume( void* polygon ) {
	assert( 0 );
	return 0.0;
}

void _PolygonShapeXZ_DistanceFromCenterAxis( void* shape, Coord coord, double* disVec ){
	Stg_Shape* self = (Stg_Shape*)shape;
	Journal_Firewall( False, Journal_Register( Error_Type, (Name)self->type  ),
	"Error in function %s: This functions hasn't been implemented.", __func__ );
}


