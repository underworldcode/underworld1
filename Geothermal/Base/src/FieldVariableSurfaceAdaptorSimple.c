/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
** Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
** Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
** Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
** Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
** Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
** Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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
#include "FieldVariableSurfaceAdaptorSimple.h"


/* Textual name of this class */
const Type FieldVariableSurfaceAdaptorSimple_Type = (const Type)"FieldVariableSurfaceAdaptorSimple";

/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/

FieldVariableSurfaceAdaptorSimple* FieldVariableSurfaceAdaptorSimple_New( Name name ) {
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof(FieldVariableSurfaceAdaptorSimple);
   Type                                                      type = FieldVariableSurfaceAdaptorSimple_Type;
   Stg_Class_DeleteFunction*                              _delete = _FieldVariableSurfaceAdaptorSimple_Delete;
   Stg_Class_PrintFunction*                                _print = _FieldVariableSurfaceAdaptorSimple_Print;
   Stg_Class_CopyFunction*                                  _copy = NULL;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = (void* (*)(Name))_FieldVariableSurfaceAdaptorSimple_New;
   Stg_Component_ConstructFunction*                    _construct = _FieldVariableSurfaceAdaptorSimple_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _FieldVariableSurfaceAdaptorSimple_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _FieldVariableSurfaceAdaptorSimple_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _FieldVariableSurfaceAdaptorSimple_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _FieldVariableSurfaceAdaptorSimple_Destroy;
   AllocationType                              nameAllocationType = NON_GLOBAL;
   MeshGenerator_SetDimSizeFunc*                   setDimSizeFunc = _MeshGenerator_SetDimSize;
   MeshGenerator_GenerateFunc*                       generateFunc = FieldVariableSurfaceAdaptorSimple_Generate;

   return _FieldVariableSurfaceAdaptorSimple_New(  FIELDVARIABLESURFACEADAPTORSIMPLE_PASSARGS  );
}

FieldVariableSurfaceAdaptorSimple* _FieldVariableSurfaceAdaptorSimple_New(  FIELDVARIABLESURFACEADAPTORSIMPLE_DEFARGS  ) {
   FieldVariableSurfaceAdaptorSimple* self;

   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(FieldVariableSurfaceAdaptorSimple) );
   self = (FieldVariableSurfaceAdaptorSimple*)_MeshGenerator_New(  MESHGENERATOR_PASSARGS  );

   self->topHeightField    = NULL;
   self->bottomHeightField = NULL;

   /* Virtual info */

   return self;
}

/*----------------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _FieldVariableSurfaceAdaptorSimple_Delete( void* adaptor ) {
   FieldVariableSurfaceAdaptorSimple*  self = (FieldVariableSurfaceAdaptorSimple*)adaptor;

   /* Delete the parent. */
   _MeshGenerator_Delete( self );
}

void _FieldVariableSurfaceAdaptorSimple_Print( void* adaptor, Stream* stream ) {
   FieldVariableSurfaceAdaptorSimple*  self = (FieldVariableSurfaceAdaptorSimple*)adaptor;

   /* Set the Journal for printing informations */
   Stream* adaptorStream;
   adaptorStream = Journal_Register( InfoStream_Type, (Name)"FieldVariableSurfaceAdaptorSimpleStream"  );

   /* Print parent */
   Journal_Printf( stream, "FieldVariableSurfaceAdaptorSimple (ptr): (%p)\n", self );
   _MeshGenerator_Print( self, stream );
}

void _FieldVariableSurfaceAdaptorSimple_AssignFromXML( void* adaptor, Stg_ComponentFactory* cf, void* data ) {
   FieldVariableSurfaceAdaptorSimple*  self     = (FieldVariableSurfaceAdaptorSimple*)adaptor;

   assert( self );
   assert( cf );

   /* Call parent construct. */
   _MeshAdaptor_AssignFromXML( self, cf, data );

   self->topHeightField    = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"HeightField", FieldVariable, False, data  );
   self->bottomHeightField = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"BottomHeightField", FieldVariable, False, data  );

   self->contactDepth  = Stg_ComponentFactory_GetInt( cf, self->name, (Dictionary_Entry_Key)"contactDepth", 0  );
   /* verticle mesh adaption parameters */
   self->minElSize[0]  = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"minElementSize_X", 10.0  );
   self->minElSize[1]  = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"minElementSize_Y", 10.0  );
   self->minElSize[2]  = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"minElementSize_Z", 10.0  );
   self->elDilatePower = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"elementDilationPower", 1.0  );

   self->adaptForMat   = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"adaptForMaterial", False  );
   self->elBunching    = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"elementBunching", True  );

}

void _FieldVariableSurfaceAdaptorSimple_Build( void* adaptor, void* data ) {
   FieldVariableSurfaceAdaptorSimple*  self     = (FieldVariableSurfaceAdaptorSimple*)adaptor;

   _MeshAdaptor_Build( adaptor, data );

   if(self->topHeightField)    Stg_Component_Build( self->topHeightField, data, False );
   if(self->bottomHeightField) Stg_Component_Build( self->bottomHeightField, data, False );
   /** lets go ahead an initialise this now as well */
   if(self->topHeightField)    Stg_Component_Initialise( self->topHeightField, data, False );
   if(self->bottomHeightField) Stg_Component_Initialise( self->bottomHeightField, data, False );
}

void _FieldVariableSurfaceAdaptorSimple_Initialise( void* adaptor, void* data ) {
   FieldVariableSurfaceAdaptorSimple* self = (FieldVariableSurfaceAdaptorSimple*)adaptor;

   //_MeshAdaptor_Initialise( adaptor, data );
   
}

void _FieldVariableSurfaceAdaptorSimple_Execute( void* adaptor, void* data ) {
}

void _FieldVariableSurfaceAdaptorSimple_Destroy( void* adaptor, void* data ) {
}

/*--------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/
#define EPS 1.0e-6

/* NOTE: assumes linear elements for now... */
void FieldVariableSurfaceAdaptorSimple_MeshAdapt_Sides( FieldVariableSurfaceAdaptorSimple* self, FeMesh* mesh, 
                                                  double* matMin, double* matMax, unsigned dim ) 
{
	double		min[3], max[3];
	int		nElsLeft = 1, nElsRight = 1, nElsBox = 0;
	Stream*		error			= Journal_Register( ErrorStream_Type, FieldVariableSurfaceAdaptorSimple_Type );
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

}

void FieldVariableSurfaceAdaptorSimple_MeshAdapt_Height( FieldVariableSurfaceAdaptorSimple* self, FeMesh* mesh, double* matMin, double* matMax )  {
	double		min[3], max[3];
	FeVariable*	topHeightField = self->topHeightField;
	double		maxHeight		= FieldVariable_GetMaxGlobalFieldMagnitude( topHeightField );
	double		minHeight		= FieldVariable_GetMinGlobalFieldMagnitude( topHeightField );
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
	Stream*		error			= Journal_Register( ErrorStream_Type, FieldVariableSurfaceAdaptorSimple_Type );
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
			deforms[inds[I_AXIS]][inds[K_AXIS]] = FieldVariableSurfaceAdaptorSimple_InterpolateFromHeightMap( self, (Mesh*)mesh, vert_i );
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

}

void FieldVariableSurfaceAdaptorSimple_Generate( void* adaptor, void* _mesh, void* data ) {
	FieldVariableSurfaceAdaptorSimple* 	self 		= (FieldVariableSurfaceAdaptorSimple*)adaptor;
	FeMesh* 			mesh 		= (FeMesh*)_mesh;
	double				min[3], max[3];
	ElementType* 			elType;

	MeshGenerator_Generate( self->generator, mesh, data );

	if( self->context->loadFromCheckPoint == True )
		return;

	min[0] = Dictionary_GetDouble_WithDefault( self->context->dictionary, (Dictionary_Entry_Key)"materialBoxMinX", 0.0  );
	min[1] = Dictionary_GetDouble_WithDefault( self->context->dictionary, (Dictionary_Entry_Key)"materialBoxMinY", 0.0  );
	min[2] = Dictionary_GetDouble_WithDefault( self->context->dictionary, (Dictionary_Entry_Key)"materialBoxMinZ", 0.0  );
	max[0] = Dictionary_GetDouble_WithDefault( self->context->dictionary, (Dictionary_Entry_Key)"materialBoxMaxX", 0.0  );
	max[1] = Dictionary_GetDouble_WithDefault( self->context->dictionary, (Dictionary_Entry_Key)"materialBoxMaxY", 0.0  );
	max[2] = Dictionary_GetDouble_WithDefault( self->context->dictionary, (Dictionary_Entry_Key)"materialBoxMaxZ", 0.0 );

	if( self->adaptForMat ) {
		FieldVariableSurfaceAdaptorSimple_MeshAdapt_Sides( self, mesh, min, max, I_AXIS );
		FieldVariableSurfaceAdaptorSimple_MeshAdapt_Sides( self, mesh, min, max, K_AXIS );
		FieldVariableSurfaceAdaptorSimple_MeshAdapt_Height( self, mesh, min, max );
		//FieldVariableSurfaceAdaptorSimple_MeshAdapt_Sides( self, mesh, min, max, J_AXIS );
		//FieldVariableSurfaceAdaptorSimple_SurfaceAdapt( self, mesh, data );
	}
	else {
		FieldVariableSurfaceAdaptorSimple_SurfaceAdapt( self, mesh, data );
	}
}

void FieldVariableSurfaceAdaptorSimple_SurfaceAdapt( void* adaptor, void* _mesh, void* data ) {
	FieldVariableSurfaceAdaptorSimple* 	self 	= (FieldVariableSurfaceAdaptorSimple*)adaptor;
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

	grid =           *(Grid**)ExtensionManager_Get( mesh->info, mesh, ExtensionManager_GetHandle( mesh->info, (Name)"vertexGrid"  ) );
	localOrigin = (unsigned* )ExtensionManager_Get( mesh->info, mesh, ExtensionManager_GetHandle( mesh->info, (Name)"localOrigin" ) );
	sync = IGraph_GetDomain( mesh->topo, MT_VERTEX );
	deforms = Memory_Alloc_2DArray_Unnamed( double, grid->sizes[I_AXIS], grid->sizes[K_AXIS] );

	for( n_i = 0; n_i < Sync_GetNumDomains( sync ); n_i++ ) {
		gNode = Sync_DomainToGlobal( sync, n_i );
		Grid_Lift( grid, gNode, inds );
		if( inds[J_AXIS] == localOrigin[J_AXIS] )	
			deforms[inds[I_AXIS]][inds[K_AXIS]] = FieldVariableSurfaceAdaptorSimple_InterpolateFromHeightMap( self, mesh, n_i );
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

double FieldVariableSurfaceAdaptorSimple_InterpolateFromHeightMap( FieldVariableSurfaceAdaptorSimple* self, Mesh* mesh, unsigned vertex ) {
   ElementType*         elType;
   double               gCoord[2], lCoord[2];
   double               height      = 0.0;
   unsigned             heightMeshEl, heightNearestVertex;
   InterpolationResult	interpRes;

   /* original mesh is 3D, but we're only interested in the top (X-Z) surface */
   gCoord[0] = Mesh_GetVertex( mesh, vertex )[I_AXIS];
   gCoord[1] = Mesh_GetVertex( mesh, vertex )[K_AXIS];

   interpRes = FieldVariable_InterpolateValueAt( self->topHeightField, gCoord, &height );
   return height;
}


