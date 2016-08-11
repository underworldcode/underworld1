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
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include "types.h"
#include "ThermalDataMaterial.h"
#include "IrregularQuadGenerator.h"
#include "IrregularTriGenerator.h"
#include "FieldVariableSurfaceAdaptor.h"


/* Textual name of this class */
const Type FieldVariableSurfaceAdaptor_Type = (const Type)"FieldVariableSurfaceAdaptor";

/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/

FieldVariableSurfaceAdaptor* FieldVariableSurfaceAdaptor_New( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(FieldVariableSurfaceAdaptor);
	Type                                                      type = FieldVariableSurfaceAdaptor_Type;
	Stg_Class_DeleteFunction*                              _delete = _FieldVariableSurfaceAdaptor_Delete;
	Stg_Class_PrintFunction*                                _print = _FieldVariableSurfaceAdaptor_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = (void* (*)(Name))_FieldVariableSurfaceAdaptor_New;
	Stg_Component_ConstructFunction*                    _construct = _FieldVariableSurfaceAdaptor_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _FieldVariableSurfaceAdaptor_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _FieldVariableSurfaceAdaptor_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _FieldVariableSurfaceAdaptor_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _FieldVariableSurfaceAdaptor_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
	MeshGenerator_SetDimSizeFunc*                   setDimSizeFunc = _MeshGenerator_SetDimSize;
	MeshGenerator_GenerateFunc*                       generateFunc = FieldVariableSurfaceAdaptor_Generate;

	return _FieldVariableSurfaceAdaptor_New(  FIELDVARIABLESURFACEADAPTOR_PASSARGS  );
}

FieldVariableSurfaceAdaptor* _FieldVariableSurfaceAdaptor_New(  FIELDVARIABLESURFACEADAPTOR_DEFARGS  ) {
	FieldVariableSurfaceAdaptor* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(FieldVariableSurfaceAdaptor) );
	self = (FieldVariableSurfaceAdaptor*)_MeshGenerator_New(  MESHGENERATOR_PASSARGS  );

	/* Virtual info */

	return self;
}

/*----------------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _FieldVariableSurfaceAdaptor_Delete( void* adaptor ) {
	FieldVariableSurfaceAdaptor*	self = (FieldVariableSurfaceAdaptor*)adaptor;

	/* Delete the parent. */
	_MeshGenerator_Delete( self );
}

void _FieldVariableSurfaceAdaptor_Print( void* adaptor, Stream* stream ) {
	FieldVariableSurfaceAdaptor*	self = (FieldVariableSurfaceAdaptor*)adaptor;
	
	/* Set the Journal for printing informations */
	Stream* adaptorStream;
	adaptorStream = Journal_Register( InfoStream_Type, (Name)"FieldVariableSurfaceAdaptorStream"  );

	/* Print parent */
	Journal_Printf( stream, "FieldVariableSurfaceAdaptor (ptr): (%p)\n", self );
	_MeshGenerator_Print( self, stream );
}

void _FieldVariableSurfaceAdaptor_AssignFromXML( void* adaptor, Stg_ComponentFactory* cf, void* data ) {
	FieldVariableSurfaceAdaptor*	self 		= (FieldVariableSurfaceAdaptor*)adaptor;

	assert( self );
	assert( cf );

	/* Call parent construct. */
	_MeshAdaptor_AssignFromXML( self, cf, data );

        /* Check if we want to keep a certain depth at the bottom reserved for contact elements. */
        self->contactDepth = Stg_ComponentFactory_GetInt( cf, self->name, (Dictionary_Entry_Key)"contactDepth", 0  );

	self->filename = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"heightFileName", ""  );
	self->heightField = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"HeightField", FeVariable, True, data  );
	Stg_Component_AssignFromXML( self->heightField->feMesh->generator, cf, data, False );
	self->generator = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Generator", MeshGenerator, True, data  );

	/* verticle mesh adaption parameters */
	self->minElSize[0] = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"minElementSize_X", 10.0  );
	self->minElSize[1] = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"minElementSize_Y", 10.0  );
	self->minElSize[2] = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"minElementSize_Z", 10.0  );
	self->elDilatePower = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"elementDilationPower", 1.0  );

	self->adaptForMat = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"adaptForMaterial", False  );
	self->elBunching = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"elementBunching", True  );
}

void _FieldVariableSurfaceAdaptor_Build( void* adaptor, void* data ) {
	FieldVariableSurfaceAdaptor* 	self 		= (FieldVariableSurfaceAdaptor*)adaptor;
	File*				file;
	unsigned			numNodeVerts, numEdgeVerts, numTriVerts, vert_i;
	double				height;
	int				vertInds[3];
	double				vertHeights[3];
	double				dummyDouble[2];
	int				dummyInt[3];
	double				dist0sq, dist1sq, edgeLenSq;
	double				*vert_0, *vert_1, *vertCentre;
	char				*filename;
	double				triCentre[2];

	_FeVariable_Build( self->heightField, data );

	/* set the variable values for the height map from the input file */
	filename = Memory_Alloc_Array_Unnamed( char, strlen(self->filename) + 6 /* .node */ );
	sprintf( filename, "%s.node", self->filename );
	file = CFile_NewRead( filename );
	Journal_Firewall(
		(long)file,
		Journal_Register( FieldVariableSurfaceAdaptor_Type, Error_Type ),
		"File \"%s\" not found!\n",
		filename );
	fscanf( CFile_Ptr( file ), "%u  %d  %d  %d", &numNodeVerts, dummyInt, dummyInt+1, dummyInt+2 );

	/* for the moment, assume that we're reading in the height values from the triangle .node file, and
	 * that we're going to interpolate onto the additional nodal locations */
	/* first, just read in the values for the triangle vertex nodes */
	for( vert_i = 0; vert_i < numNodeVerts; vert_i++ ) {
		fscanf( CFile_Ptr( file ), "%d %lf %lf %lf %d", dummyInt, dummyDouble, dummyDouble+1, &height, dummyInt+1 );
		FeVariable_SetValueAtNode( self->heightField, vert_i, &height );
	}
	Stg_Class_Delete( file );
	Memory_Free( filename );

	/* if we are using an irregular trangular mesh rather than an irregular quadrilateral one, then we have no
	 * need to interpolate onto additional nodes */
	if( !strcmp( self->heightField->feMesh->generator->type, IrregularTriGenerator_Type ) )
		return;

	/* now interpolate the edge centre nodes */
	filename = Memory_Alloc_Array_Unnamed( char, strlen(self->filename) + 6 /* .edge */ );
	sprintf( filename, "%s.edge", self->filename );
	file = CFile_NewRead( filename );
	Journal_Firewall(
		(long)file,
		Journal_Register( FieldVariableSurfaceAdaptor_Type, Error_Type ),
		"File \"%s\" not found!\n",
		filename );
	fscanf( CFile_Ptr( file ), "%d  %d", &numEdgeVerts, dummyInt );
	for( vert_i = 0; vert_i < numEdgeVerts; vert_i++ ) {
		fscanf( CFile_Ptr( file ), "%d  %d  %d  %d", dummyInt, vertInds, vertInds+1, dummyInt+1 );

		FeVariable_GetValueAtNode( self->heightField, vertInds[0], vertHeights+0 );
		FeVariable_GetValueAtNode( self->heightField, vertInds[1], vertHeights+1 );

		vert_0 = Mesh_GetVertex( self->heightField->feMesh, vertInds[0] );
		vert_1 = Mesh_GetVertex( self->heightField->feMesh, vertInds[1] );
		vertCentre = Mesh_GetVertex( self->heightField->feMesh, numNodeVerts+vert_i );

		dist0sq = (vert_0[0] - vertCentre[0])*(vert_0[0] - vertCentre[0]) + 
			  (vert_0[1] - vertCentre[1])*(vert_0[1] - vertCentre[1]);
		dist1sq = (vert_1[0] - vertCentre[0])*(vert_1[0] - vertCentre[0]) + 
			  (vert_1[1] - vertCentre[1])*(vert_1[1] - vertCentre[1]);
		edgeLenSq = (vert_0[0] - vert_1[0])*(vert_0[0] - vert_1[0]) + 
			    (vert_0[1] - vert_1[1])*(vert_0[1] - vert_1[1]);

		height = (1.0/sqrt(edgeLenSq))*(sqrt(dist1sq)*vertHeights[0] + sqrt(dist0sq)*vertHeights[1]);
		FeVariable_SetValueAtNode( self->heightField, numNodeVerts+vert_i, &height );
	}
	Stg_Class_Delete( file );
	Memory_Free( filename );

	/* now interpolate the triangle centre nodes */
	filename = Memory_Alloc_Array_Unnamed( char, strlen(self->filename) + 5 /* .ele */ );
	sprintf( filename, "%s.ele", self->filename );
	file = CFile_NewRead( filename );
	Journal_Firewall(
		(long)file,
		Journal_Register( FieldVariableSurfaceAdaptor_Type, Error_Type ),
		"File \"%s\" not found!\n",
		filename );
	fscanf( CFile_Ptr( file ), "%d  %d  %d", &numTriVerts, dummyInt, dummyInt+1 );
	triCentre[0] = triCentre[1] = 0.33333333333333333333333333333333333333333333333;
	for( vert_i = 0; vert_i < numTriVerts; vert_i++ ) {
		fscanf( CFile_Ptr( file ), "%d  %d  %d  %d", dummyInt, vertInds, vertInds+1, vertInds+2 );
		
		FeVariable_GetValueAtNode( self->heightField, vertInds[0], vertHeights+0 );
		FeVariable_GetValueAtNode( self->heightField, vertInds[1], vertHeights+1 );
		FeVariable_GetValueAtNode( self->heightField, vertInds[2], vertHeights+2 );
		
		height = IrregularQuadGenerator_InterpFromLocalCoord_Tri( vertHeights, triCentre );
		FeVariable_SetValueAtNode( self->heightField, numNodeVerts+numEdgeVerts+vert_i, &height );
	}
	fclose( CFile_Ptr( file ) );
	Memory_Free( filename );
}

void _FieldVariableSurfaceAdaptor_Initialise( void* adaptor, void* data ) {
   _MeshAdaptor_Initialise( adaptor, data );
}

void _FieldVariableSurfaceAdaptor_Execute( void* adaptor, void* data ) {
}

void _FieldVariableSurfaceAdaptor_Destroy( void* adaptor, void* data ) {
}

/*--------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/

Bool TestAdaption( FeMesh* mesh, unsigned dim ) {
    Grid** grid = (Grid**)Mesh_GetExtension( mesh, Grid*, "vertexGrid" );
    unsigned i, j, k, inds[3];
    unsigned* sizes = (*grid)->sizes;
    unsigned kAxis = dim;
    unsigned iAxis = (dim+1)%3;
    unsigned jAxis = (dim+2)%3;
    double x;
    unsigned gNode, lNode;

    for( i = 0; i < sizes[iAxis]; i++ ) {
        for( j = 0; j < sizes[jAxis]; j++ ) {
            inds[iAxis] = i;
            inds[jAxis] = j;
            inds[kAxis] = 0;
            gNode = Grid_Project( *grid, inds );
            Mesh_GlobalToDomain( mesh, MT_VERTEX, gNode, &lNode );
            x = mesh->verts[lNode][kAxis];
            for( k = 1; k < sizes[kAxis]; k++ ) {
                inds[kAxis] = k;
                gNode = Grid_Project( *grid, inds );
                Mesh_GlobalToDomain( mesh, MT_VERTEX, gNode, &lNode );
		Journal_Firewall(
			mesh->verts[lNode][kAxis] >= x,
			Journal_Register( FieldVariableSurfaceAdaptor_Type, Error_Type ),
			"!!!!!ERROR mesh adaption error in dimension: %u. (mesh->verts[lNode][kAxis] [%g] >= x [%g]) \n"
			"Blame DL for this error! This is the mesh refinement sanity check. Not sure how to translate into "
			"english, but generally this means that you're asking for a refinement in that direction that is "
			"impossible to achieve given the user inputs of: resolution, desired minimum element distance for that "
			"dimension, and the geometry extents for that dimension.\n",
			dim, mesh->verts[lNode][kAxis], x );
                x = mesh->verts[lNode][kAxis];
            }
        }
    }
    return True;
}

#define EPS 1.0e-6
#define TESTADAPT

/* NOTE: assumes linear elements for now... */
void FieldVariableSurfaceAdaptor_MeshAdapt_Sides( FieldVariableSurfaceAdaptor* self, FeMesh* mesh, 
                                                  double* matMin, double* matMax, unsigned dim ) 
{
	double		min[3], max[3];
	int		nElsLeft = 1, nElsRight = 1, nElsBox = 0;
	Stream*		error			= Journal_Register( ErrorStream_Type, FieldVariableSurfaceAdaptor_Type );
	Grid**		elGrid;
	Grid**		vertGrid;
	double*		Xi;
	int		vert_i;
	unsigned	gNode, inds[3];
	int		minSideEls = 0;

	Mesh_Algorithms_GetGlobalCoordRange( mesh->algorithms, min, max );

	/* material box sanity check */
	if( dim != J_AXIS )
		Journal_Firewall( min[dim] - EPS < matMin[dim] && max[dim] + EPS > matMax[dim], error, 
			  "Material bounding box [%.6e,%.6e] outside global domain [%.6e,%.6e] in dimension %u.\n", 
			  matMin[dim], matMax[dim], min[dim], max[dim], dim );

	/* If the refined region is less than one "min element size" away from the domain boundary... use the boundary as the
	 *  extent of the refined region. That is - we want to ensure that the refined region is more refined than outside. */
	if( fabs( matMin[dim] - min[dim] ) < self->minElSize[dim] ) { nElsLeft  = 0; matMin[dim] = min[dim]; }
	if( fabs( matMax[dim] - max[dim] ) < self->minElSize[dim] ) { nElsRight = 0; matMax[dim] = max[dim]; }

	elGrid = (Grid**)ExtensionManager_Get( mesh->info, mesh, ExtensionManager_GetHandle( mesh->info, "elementGrid" ) );

	/* Need to work out how many elements are (a) in the refined region, (b) to the left of it and (c) the right of it. At
	 * this point we can only work out with any certainty (a). We need to ensure even at this point that the refinement is
	 * possible/sane. */
	nElsBox = ceil( (matMax[dim] - matMin[dim])/self->minElSize[dim] );
	if( nElsLeft  ) { minSideEls++; }
	if( nElsRight ) { minSideEls++; }
	Journal_Firewall( nElsLeft + nElsBox + nElsRight <= (*elGrid)->sizes[dim], error,
			  "(draft) nElsLeft[%u] + nElsBox[%u] + (draft) nElsRight[%u] <= nEls[%u]\n"
			  "Material bounding box element size [%.6e] is too small in dimension %u - must be [%.6e]"
			  " or greater for the current material bounding box geometry in this dimension.\n",
			  nElsLeft, nElsBox, nElsRight, (*elGrid)->sizes[dim],
			  self->minElSize[dim], dim, (matMax[dim] - matMin[dim])/((*elGrid)->sizes[dim] - minSideEls) );

	/* If we get this far, we need to redfine the nEls correctly...
	 *   - redefine the "min element size" such that it cleanly fits as multiples in the refined region. 
	 *   - redefine left and right to have the right proportions of elements.*/
	self->minElSize[dim] = (matMax[dim] - matMin[dim])/nElsBox;
	if( !nElsLeft && !nElsRight ) { nElsBox   = (*elGrid)->sizes[dim]; }
	else if( !nElsLeft )          { nElsRight = (*elGrid)->sizes[dim] - nElsBox; }
	else if( !nElsRight )         { nElsLeft  = (*elGrid)->sizes[dim] - nElsBox; }
	else {
		double dx = ( (matMin[dim] - min[dim]) + (max[dim] - matMax[dim]) )/( (*elGrid)->sizes[dim] - nElsBox );
		double idealnElsLeft = floor( (matMin[dim] - min[dim])/dx );

		Journal_Firewall( dx >= self->minElSize[dim], error,
			"dx[%g] <= self->minElSize[dim] [%g]\n"
			"The current setup suggests that the refinement is higher outside the refined region for dimension %u.\n",
			dx, self->minElSize[dim],
			dim );

		nElsLeft  = nElsLeft > idealnElsLeft ? nElsLeft : idealnElsLeft;
		nElsRight = (*elGrid)->sizes[dim] - nElsBox - nElsLeft;

		Journal_Firewall( nElsRight >= 1, error,
			"nElsRight[%u] >= 1\n"
			"SHOULDNT GET HERE! for dimension %u.\n",
			nElsRight,
			dim );
	}

	Xi = Memory_Alloc_Array_Unnamed( double, (*elGrid)->sizes[dim] + 1 );

	/* global domain left side */
	Xi[0] = min[dim];
	/* left hand side vertices */
	for( vert_i = 1; vert_i < nElsLeft; vert_i++ ) {
		double elRatio   = ((double)(nElsLeft - vert_i))/nElsLeft;
		double scaleFac  = 1.0 - elRatio*elRatio;
		double remainder = matMin[dim] - min[dim] - nElsLeft*self->minElSize[dim];
		if( remainder > 0.0 ) {
			Xi[vert_i] = min[dim] + vert_i*self->minElSize[dim] + scaleFac*remainder;
		}
		else {
			double dx = fabs( matMin[dim] - min[dim] )/nElsLeft;
			Xi[vert_i] = min[dim] + vert_i*dx;
		}
	}
	/* material box left vertex */
	Xi[nElsLeft] = matMin[dim];
	/* inside the material box */
	for( vert_i = nElsLeft + 1; vert_i < nElsLeft + nElsBox; vert_i++ ) {
		Xi[vert_i] = matMin[dim] + (vert_i - nElsLeft)*self->minElSize[dim];
	}
	/* material box right vertex */
	Xi[nElsLeft + nElsBox] = matMax[dim];
	/* right hand side vertices */
	for( vert_i = nElsLeft + nElsBox + 1; vert_i < (*elGrid)->sizes[dim]; vert_i++ ) {
		double elRatio   = 1.0 - ((double)((*elGrid)->sizes[dim] - vert_i))/nElsRight;
		double scaleFac  = elRatio*elRatio;
		double remainder = max[dim] - matMax[dim] - nElsRight*self->minElSize[dim];
		if( remainder > 0.0 ) {
			Xi[vert_i] = matMax[dim] + (vert_i - nElsLeft - nElsBox)*self->minElSize[dim] + scaleFac*remainder;
		}
		else {
			double dx = fabs( max[dim] - matMax[dim] )/nElsRight;
			Xi[vert_i] = matMax[dim] + (vert_i - nElsLeft - nElsBox)*dx;
		}
	}
	/* global domain right side */
	Xi[(*elGrid)->sizes[dim]] = max[dim];

	vertGrid = (Grid**)ExtensionManager_Get( mesh->info, mesh, ExtensionManager_GetHandle( mesh->info, "vertexGrid" ) );

	for( vert_i = 0; vert_i < Sync_GetNumDomains( IGraph_GetDomain( mesh->topo, MT_VERTEX ) ); vert_i++ ) {
		gNode = Sync_DomainToGlobal( IGraph_GetDomain( mesh->topo, MT_VERTEX ), vert_i );
		Grid_Lift( *vertGrid, gNode, inds );
		mesh->verts[vert_i][dim] = Xi[inds[dim]];
	}

	Memory_Free( Xi );

#ifdef TESTADAPT
	TestAdaption( mesh, dim );
#endif
}

void FieldVariableSurfaceAdaptor_MeshAdapt_Height( FieldVariableSurfaceAdaptor* self, FeMesh* mesh, double* matMin, double* matMax )  {
	double		min[3], max[3];
	FeVariable*	heightField		= self->heightField;
	double		maxHeight		= FieldVariable_GetMaxGlobalFieldMagnitude( heightField );
	double		minHeight		= FieldVariable_GetMinGlobalFieldMagnitude( heightField );
	double		height;
	Bool		midLayer		= matMax[J_AXIS] < minHeight;
	const Sync*	sync 			= IGraph_GetDomain( mesh->topo, MT_VERTEX );
	Grid**		vertGrid;
	double**	deforms;
	unsigned 	vert_i, inds[3], gNode;
	int		nElsBottom, nElsTop;
	int		nElsMat, nElsTot;
	double		boxDepth;
	double		minElSize 		= self->minElSize[J_AXIS];
	double		scaleFac, elRatio;
	unsigned*	localOrigin;
	Stream*		error			= Journal_Register( ErrorStream_Type, FieldVariableSurfaceAdaptor_Type );
	double		xBottom, xTop;
	double		delta_y;
	double		remainder;

	/* interpolate height map for  */	
	vertGrid = (Grid**)ExtensionManager_Get( mesh->info, mesh, ExtensionManager_GetHandle( mesh->info, (Name)"vertexGrid" )  );
	localOrigin = (unsigned* )ExtensionManager_Get( mesh->info, mesh, ExtensionManager_GetHandle( mesh->info, (Name)"localOrigin" )  );
	deforms	 = Memory_Alloc_2DArray( double, (*vertGrid)->sizes[I_AXIS], (*vertGrid)->sizes[K_AXIS], (Name)"heightMap" );
	for( vert_i = 0; vert_i < Sync_GetNumDomains( sync ); vert_i++ ) {
		gNode = Sync_DomainToGlobal( sync, vert_i );
		Grid_Lift( *vertGrid, gNode, inds );
		if( inds[J_AXIS] == localOrigin[J_AXIS] )
			deforms[inds[I_AXIS]][inds[K_AXIS]] = FieldVariableSurfaceAdaptor_InterpolateFromHeightMap( self, (Mesh*)mesh, vert_i );
	}

	nElsTot = (*vertGrid)->sizes[J_AXIS] - 1;
	Mesh_Algorithms_GetGlobalCoordRange( mesh->algorithms, min, max );
	
	/* material bounding box entirely lies beneath the surface topography, so adapt mesh on both top and bottom */
	if( midLayer ) {
		xBottom = matMin[J_AXIS] - min[J_AXIS];
		xTop 	= maxHeight - matMax[J_AXIS];
		nElsMat = ceil( (matMax[J_AXIS] - matMin[J_AXIS])/minElSize );
		nElsTop = ceil( ( xTop/(xTop + xBottom) )*(nElsTot - nElsMat) );
		nElsBottom = nElsTot - nElsMat - nElsTop;
		Journal_Firewall( nElsBottom > 1, error, "insuffucuent number of elements below the material bounding box... "
				  "either increase the resolution or the mininum size of the bounding box elements.\n" );
		
		for( vert_i = 0; vert_i < Sync_GetNumDomains( sync ); vert_i++ ) {
			gNode = Sync_DomainToGlobal( sync, vert_i );
			Grid_Lift( *vertGrid, gNode, inds );
     
			/* don't adapt the bottom vertices */
			if( inds[J_AXIS] == 0  )
				continue;

			height = deforms[inds[I_AXIS]][inds[K_AXIS]];

			/* vertices on the left side of the material bounding box */
			if( inds[J_AXIS] < nElsBottom ) {
				elRatio = ( (double)(nElsBottom - inds[J_AXIS]) ) / nElsBottom;
				scaleFac = 1.0 - elRatio*elRatio;
				remainder = xBottom - nElsBottom*minElSize;
				if( remainder > 0.0 ) {
					mesh->verts[vert_i][J_AXIS] = min[J_AXIS] + inds[J_AXIS]*minElSize + scaleFac*remainder;
				}
				else {
					double dx = fabs( matMin[J_AXIS] - min[J_AXIS] )/nElsBottom;
					mesh->verts[vert_i][J_AXIS] = min[J_AXIS] + inds[J_AXIS]*dx;
				}
			}
			/* vertices on the left edge of the material bounding box */
			else if( inds[J_AXIS] == nElsBottom ) {
				mesh->verts[vert_i][J_AXIS] = matMin[J_AXIS];
			}
			/* vertices inside the material bounding box */
			else if( inds[J_AXIS] < nElsBottom + nElsMat ) {
				mesh->verts[vert_i][J_AXIS] = matMin[J_AXIS] + minElSize*(inds[J_AXIS] - nElsBottom);
			}
			/* vertices on the right edge of the material bounding box */
			else if( inds[J_AXIS] == nElsBottom + nElsMat ) {
				mesh->verts[vert_i][J_AXIS] = matMax[J_AXIS];
			}
			/* vertices on the right side of the material bounding box */
			else if( inds[J_AXIS] < (*vertGrid)->sizes[J_AXIS] - 1 ){
				delta_y = (height - matMax[J_AXIS])/nElsTop;
				elRatio = 1.0 - ( (double)(nElsTot - inds[J_AXIS]) )/nElsTop;
				scaleFac = elRatio*elRatio;
				remainder = (height - matMax[J_AXIS]) - nElsTop*delta_y;
				if( remainder > 0.0 ) {
					mesh->verts[vert_i][J_AXIS] = matMax[J_AXIS] + (inds[J_AXIS] - nElsBottom - nElsMat)*delta_y + scaleFac*remainder;
				}
				else {
					double dx = fabs( height - matMax[J_AXIS] )/nElsTop;
					mesh->verts[vert_i][J_AXIS] = matMax[J_AXIS] + (inds[J_AXIS] - nElsBottom - nElsMat)*dx;
				}
			}
			/* vertices on the top */
			else {
				mesh->verts[vert_i][J_AXIS] = height;
			}
		}
	}
	/* material layer coincides with topography depth, so include the topography in the material bounding box */
	else {
		for( vert_i = 0; vert_i < Sync_GetNumDomains( sync ); vert_i++ ) {
			gNode = Sync_DomainToGlobal( sync, vert_i );
			Grid_Lift( *vertGrid, gNode, inds );

			if( inds[J_AXIS] == 0 )
				continue;

			height = deforms[inds[I_AXIS]][inds[K_AXIS]];
			nElsMat = (unsigned)ceil( (height - matMin[J_AXIS])/minElSize );
			nElsBottom = nElsTot - nElsMat;
			Journal_Firewall( nElsBottom >= 1, error, "material bounding box element size: %e in dimension 1 is too small. "
					  "given that %u elements have been specified, a material bounding box element size of %e or "
					  "greater is required.\n", minElSize, nElsTot, (height - matMin[J_AXIS])/(nElsTot - 1.0) );
			boxDepth = height - nElsMat*minElSize;

			if( inds[J_AXIS] >= nElsBottom ) { /* vertex is on or in material layer bounding box */
				mesh->verts[vert_i][J_AXIS] = height - (nElsTot - inds[J_AXIS])*minElSize;
			}
			else {
				scaleFac = ( (double)(nElsBottom - inds[J_AXIS]) )/nElsBottom;
				elRatio = pow( scaleFac, self->elDilatePower );
				mesh->verts[vert_i][J_AXIS] = boxDepth - minElSize*(nElsBottom - inds[J_AXIS]) - 
					elRatio*(boxDepth - min[J_AXIS] - nElsBottom*minElSize);
			}
		}
	}

	MPI_Barrier( mesh->generator->mpiComm );
	Mesh_Sync( mesh );
	Memory_Free( deforms );

#ifdef TESTADAPT
	TestAdaption( mesh, J_AXIS );
#endif
}

void FieldVariableSurfaceAdaptor_Generate( void* adaptor, void* _mesh, void* data ) {
	FieldVariableSurfaceAdaptor* 	self 		= (FieldVariableSurfaceAdaptor*)adaptor;
	FeMesh* 			mesh 		= (FeMesh*)_mesh;
	double				min[3], max[3];
	ElementType* 			elType;

	MeshGenerator_Generate( self->generator, mesh, data );

	if( self->context->loadFromCheckPoint == True )
		return;

	/* need to initialise the irregular mesh element type, so that the global coords can be converted to
	 * local coords (even though we are technically still in the build phase) */
	elType = FeMesh_GetElementType( self->heightField->feMesh, 0 );
	Stg_Component_Initialise( elType, data, True );

	min[0] = Dictionary_GetDouble_WithDefault( self->context->dictionary, (Dictionary_Entry_Key)"materialBoxMinX", 0.0  );
	min[1] = Dictionary_GetDouble_WithDefault( self->context->dictionary, (Dictionary_Entry_Key)"materialBoxMinY", 0.0  );
	min[2] = Dictionary_GetDouble_WithDefault( self->context->dictionary, (Dictionary_Entry_Key)"materialBoxMinZ", 0.0  );
	max[0] = Dictionary_GetDouble_WithDefault( self->context->dictionary, (Dictionary_Entry_Key)"materialBoxMaxX", 0.0  );
	max[1] = Dictionary_GetDouble_WithDefault( self->context->dictionary, (Dictionary_Entry_Key)"materialBoxMaxY", 0.0  );
	max[2] = Dictionary_GetDouble_WithDefault( self->context->dictionary, (Dictionary_Entry_Key)"materialBoxMaxZ", 0.0 );

	if( self->adaptForMat ) {
		FieldVariableSurfaceAdaptor_MeshAdapt_Sides( self, mesh, min, max, I_AXIS );
		FieldVariableSurfaceAdaptor_MeshAdapt_Sides( self, mesh, min, max, K_AXIS );
		//FieldVariableSurfaceAdaptor_MeshAdapt_Height( self, mesh, min, max );
		FieldVariableSurfaceAdaptor_MeshAdapt_Sides( self, mesh, min, max, J_AXIS );
		FieldVariableSurfaceAdaptor_SurfaceAdapt( self, mesh, data );
	}
	else {
		FieldVariableSurfaceAdaptor_SurfaceAdapt( self, mesh, data );
	}
}

void FieldVariableSurfaceAdaptor_SurfaceAdapt( void* adaptor, void* _mesh, void* data ) {
	FieldVariableSurfaceAdaptor* 	self 	= (FieldVariableSurfaceAdaptor*)adaptor;
	Mesh* 				mesh 	= (Mesh*)_mesh;
	const Sync* 			sync;
	Grid*				grid;
	unsigned 			inds[3];
	double** 			deforms;
	unsigned 			n_i;
	double 				height;
	unsigned 			gNode;
    	unsigned*			localOrigin;

	/* If we're not 2D or 3D, forget about it. */
	if( mesh->topo->nDims != 2 && mesh->topo->nDims != 3 )
		return;

	grid = *(Grid**)ExtensionManager_Get( mesh->info, mesh, ExtensionManager_GetHandle( mesh->info, (Name)"vertexGrid" ) );
    	localOrigin = (unsigned* )ExtensionManager_Get( mesh->info, mesh, ExtensionManager_GetHandle( mesh->info, (Name)"localOrigin" )  );
	sync = IGraph_GetDomain( mesh->topo, MT_VERTEX );
	deforms = Memory_Alloc_2DArray_Unnamed( double, grid->sizes[I_AXIS], grid->sizes[K_AXIS] );

	for( n_i = 0; n_i < Sync_GetNumDomains( sync ); n_i++ ) {
		gNode = Sync_DomainToGlobal( sync, n_i );
		Grid_Lift( grid, gNode, inds );
		if( inds[J_AXIS] == localOrigin[J_AXIS] )	
			deforms[inds[I_AXIS]][inds[K_AXIS]] = FieldVariableSurfaceAdaptor_InterpolateFromHeightMap( self, mesh, n_i );
	}	

	for( n_i = 0; n_i < Sync_GetNumDomains( sync ); n_i++ ) {
		gNode = Sync_DomainToGlobal( sync, n_i );
		Grid_Lift( grid, gNode, inds );
	
		if( inds[J_AXIS] == 0 ) {
			continue;
		}
		else {
			height = (double)(inds[J_AXIS]) / (double)(grid->sizes[J_AXIS] - 1);
			mesh->verts[n_i][J_AXIS] += height * deforms[inds[I_AXIS]][inds[K_AXIS]];
		}
	}
	Memory_Free( deforms );

	MPI_Barrier( self->generator->mpiComm );
}


/*----------------------------------------------------------------------------------------------------------------------------------
** Private Functions
*/

double FieldVariableSurfaceAdaptor_InterpolateFromHeightMap( FieldVariableSurfaceAdaptor* self, Mesh* mesh, unsigned vertex ) {
	ElementType*	elType;
	double		gCoord[2], lCoord[2];
	double 		height		= 0.0;
	unsigned 	heightMeshEl, heightNearestVertex;

	/* original mesh is 3D, but we're only interested in the top (X-Z) surface */
	gCoord[0] = Mesh_GetVertex( mesh, vertex )[I_AXIS];
	gCoord[1] = Mesh_GetVertex( mesh, vertex )[K_AXIS];

	/* quadrelateral mesh */
	if( strcmp( self->heightField->feMesh->generator->type, IrregularTriGenerator_Type ) ) {
		/* coordinate doesn't exist in height mesh domain, so just map cloest height mesh coord to the vertex */
		if( !Mesh_SearchElements( self->heightField->feMesh, gCoord, &heightMeshEl ) ) {
			heightNearestVertex = Mesh_NearestVertex( self->heightField->feMesh, gCoord );
			FeVariable_GetValueAtNode( self->heightField, heightNearestVertex, &height );
		}
		else {
			elType = FeMesh_GetElementType( self->heightField->feMesh, heightMeshEl );
			ElementType_ConvertGlobalCoordToElLocal( elType, self->heightField->feMesh, heightMeshEl, gCoord, lCoord );
			FeVariable_InterpolateWithinElement( self->heightField, heightMeshEl, lCoord, &height );
		}
	}
	else {
		height = IrregularTriGenerator_InterpolateValueAt( self->heightField->feMesh->generator, self->heightField, gCoord );
	}

	return height;
}


