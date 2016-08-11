/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**	Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**	Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**	AuScope - http://www.auscope.org
**	Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**	Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**	Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**	Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**	Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
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
#include <StgDomain/StgDomain.h>

#include "units.h"
#include "types.h"
#include "shortcuts.h"
#include "ElementType.h"
#include "TrilinearElementType.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

const Type TrilinearElementType_Type = "TrilinearElementType";

#define _TrilinearElementType_NodeCount 8

void* _TrilinearElementType_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                                            _sizeOfSelf = sizeof(TrilinearElementType);
	Type                                                                                    type = TrilinearElementType_Type;
	Stg_Class_DeleteFunction*                                                            _delete = _TrilinearElementType_Delete;
	Stg_Class_PrintFunction*                                                              _print = _TrilinearElementType_Print;
	Stg_Class_CopyFunction*                                                                _copy = NULL;
	Stg_Component_DefaultConstructorFunction*                                _defaultConstructor = _TrilinearElementType_DefaultNew;
	Stg_Component_ConstructFunction*                                                  _construct = _TrilinearElementType_AssignFromXML;
	Stg_Component_BuildFunction*                                                          _build = _TrilinearElementType_Build;
	Stg_Component_InitialiseFunction*                                                _initialise = _TrilinearElementType_Initialise;
	Stg_Component_ExecuteFunction*                                                      _execute = _TrilinearElementType_Execute;
	Stg_Component_DestroyFunction*                                                      _destroy = _TrilinearElementType_Destroy;
	AllocationType                                                            nameAllocationType = NON_GLOBAL;
	ElementType_EvaluateShapeFunctionsAtFunction*                      _evaluateShapeFunctionsAt = _TrilinearElementType_SF_allNodes;
	ElementType_EvaluateShapeFunctionLocalDerivsAtFunction*  _evaluateShapeFunctionLocalDerivsAt = _TrilinearElementType_SF_allLocalDerivs_allNodes;
	ElementType_ConvertGlobalCoordToElLocalFunction*                _convertGlobalCoordToElLocal = _ElementType_ConvertGlobalCoordToElLocal;
	ElementType_JacobianDeterminantSurfaceFunction*                  _jacobianDeterminantSurface = _TrilinearElementType_JacobianDeterminantSurface;
	ElementType_SurfaceNormalFunction*                                            _surfaceNormal = _ElementType_SurfaceNormal;

	return _TrilinearElementType_New(  TRILINEARELEMENTTYPE_PASSARGS  );
}

TrilinearElementType* TrilinearElementType_New( Name name ) {
	TrilinearElementType* self = _TrilinearElementType_DefaultNew( name );

	self->isConstructed = True;
	_ElementType_Init( (ElementType*)self, _TrilinearElementType_NodeCount );
	_TrilinearElementType_Init( self );

	return self;	
}

TrilinearElementType* _TrilinearElementType_New(  TRILINEARELEMENTTYPE_DEFARGS  ) {
	TrilinearElementType*		self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(TrilinearElementType) );
	self = (TrilinearElementType*)_ElementType_New(  ELEMENTTYPE_PASSARGS  );
	
	/* General info */
	
	/* Virtual functions */
	
	/* TrilinearElementType info */
	
	return self;
}

void _TrilinearElementType_Init( TrilinearElementType* self ) {
	Dimension_Index dim, dim_I=0;

	/* General and Virtual info should already be set */
	
	/* TrilinearElementType info */
	dim = self->dim = 3;
	for ( dim_I = 0; dim_I < dim; dim_I++ ) {
		self->minElLocalCoord[dim_I] = -1;
		self->maxElLocalCoord[dim_I] = 1;
		self->elLocalLength[dim_I] = self->maxElLocalCoord[dim_I] - self->minElLocalCoord[dim_I];
	}

	/* Set up the tetrahedral indices. */
	self->tetInds = Memory_Alloc_2DArray( unsigned, 10, 4, (Name)"Mesh_HexType::tetInds" );
	self->tetInds[0][0] = 0; self->tetInds[0][1] = 1; self->tetInds[0][2] = 2; self->tetInds[0][3] = 4;
	self->tetInds[1][0] = 1; self->tetInds[1][1] = 2; self->tetInds[1][2] = 3; self->tetInds[1][3] = 7;
	self->tetInds[2][0] = 1; self->tetInds[2][1] = 4; self->tetInds[2][2] = 5; self->tetInds[2][3] = 7;
	self->tetInds[3][0] = 2; self->tetInds[3][1] = 4; self->tetInds[3][2] = 6; self->tetInds[3][3] = 7;
	self->tetInds[4][0] = 1; self->tetInds[4][1] = 2; self->tetInds[4][2] = 4; self->tetInds[4][3] = 7;
	self->tetInds[5][0] = 0; self->tetInds[5][1] = 1; self->tetInds[5][2] = 3; self->tetInds[5][3] = 5;
	self->tetInds[6][0] = 0; self->tetInds[6][1] = 4; self->tetInds[6][2] = 5; self->tetInds[6][3] = 6;
	self->tetInds[7][0] = 0; self->tetInds[7][1] = 2; self->tetInds[7][2] = 3; self->tetInds[7][3] = 6;
	self->tetInds[8][0] = 3; self->tetInds[8][1] = 5; self->tetInds[8][2] = 6; self->tetInds[8][3] = 7;
	self->tetInds[9][0] = 0; self->tetInds[9][1] = 3; self->tetInds[9][2] = 5; self->tetInds[9][3] = 6;
}

void _TrilinearElementType_Delete( void* elementType ) {
	TrilinearElementType* self = (TrilinearElementType*)elementType;
	
	/* Stg_Class_Delete parent*/
	_ElementType_Delete( self  );
}

void _TrilinearElementType_Print( void* elementType, Stream* stream ) {
	TrilinearElementType* self = (TrilinearElementType*)elementType;
	
	/* Set the Journal for printing informations */
	Stream* trilinearElementTypeStream = stream;
	
	/* General info */
	Journal_Printf( trilinearElementTypeStream, "TrilinearElementType (ptr): %p\n", self );
	
	/* Print parent */
	_ElementType_Print( self, trilinearElementTypeStream );
	
	/* Virtual info */
	
	/* TrilinearElementType info */
}

void _TrilinearElementType_AssignFromXML( void* elementType, Stg_ComponentFactory *cf, void* data ){
	
}
	
void _TrilinearElementType_Initialise( void* elementType, void *data ){
	TrilinearElementType* 	self = (TrilinearElementType*)elementType;
	unsigned**		faceNodes;

	faceNodes = Memory_Alloc_2DArray( unsigned, 6, 4, (Name)"node indices for element faces"  );

	faceNodes[0][0] = 0; faceNodes[0][1] = 1; faceNodes[0][2] = 4; faceNodes[0][3] = 5;
	/* the top face */
	faceNodes[1][0] = 2; faceNodes[1][1] = 3; faceNodes[1][2] = 6; faceNodes[1][3] = 7;
	faceNodes[2][0] = 0; faceNodes[2][1] = 4; faceNodes[2][2] = 2; faceNodes[2][3] = 6;
	faceNodes[3][0] = 1; faceNodes[3][1] = 5; faceNodes[3][2] = 3; faceNodes[3][3] = 7;
	faceNodes[4][0] = 0; faceNodes[4][1] = 1; faceNodes[4][2] = 2; faceNodes[4][3] = 3;
	faceNodes[5][0] = 4; faceNodes[5][1] = 5; faceNodes[5][2] = 6; faceNodes[5][3] = 7;

	self->faceNodes = faceNodes;

	self->evaluatedShapeFunc = Memory_Alloc_Array( double, self->nodeCount, "evaluatedShapeFuncs" );
	self->GNi = Memory_Alloc_2DArray( double, self->dim, self->nodeCount, (Name)"localShapeFuncDerivitives"  );

}
	
void _TrilinearElementType_Execute( void* elementType, void *data ){
	
}
	
void _TrilinearElementType_Destroy( void* elementType, void *data ){
	TrilinearElementType* 	self = (TrilinearElementType*)elementType;

	Memory_Free( self->faceNodes );
	Memory_Free( self->evaluatedShapeFunc );
	Memory_Free( self->GNi );

	FreeArray( self->tetInds );

	_ElementType_Destroy( self, data );
}

void _TrilinearElementType_Build( void* elementType, void *data ) {
}

#if 0
void _TrilinearElementType_ConvertGlobalCoordToElementLocal( void* elementType, Element_DomainIndex element,const Coord globalCoord, Coord localCoord ) 
{
	TrilinearElementType*	self = (TrilinearElementType*)elementType;
	Dimension_Index		dim_I;

	for ( dim_I=0; dim_I < 3; dim_I++ ) {
	}
}
#endif


/*

 - Shape function definitions
 - Local node numbering convention for billinear, trilinear element (xi, eta, zeta)
 - Local coordinate domain spans  -1 <= xi,eta,zeta <= 1

    eta
     |
     |____ xi
    /
   /
 zeta


  eta
   |
2-----3
|  |__|___xi
|     |
0-----1
(zeta = -1 plane)


  eta
   |
6-----7
|  |__|___xi
|     |
4-----5
(zeta = +1 plane)


*/
void _TrilinearElementType_SF_allNodes( void* elementType, const double localCoord[], double* const evaluatedValues ) {
	double xi, eta, zeta;
	
	xi   = localCoord[0];
	eta  = localCoord[1];
	zeta = localCoord[2];	
	
	evaluatedValues[0] = 0.125*( 1.0-xi )*( 1.0-eta )*( 1.0-zeta );
	evaluatedValues[2] = 0.125*( 1.0-xi )*( 1.0+eta )*( 1.0-zeta );
	evaluatedValues[3] = 0.125*( 1.0+xi )*( 1.0+eta )*( 1.0-zeta );
	evaluatedValues[1] = 0.125*( 1.0+xi )*( 1.0-eta )*( 1.0-zeta );
	
	evaluatedValues[4] = 0.125*( 1.0-xi )*( 1.0-eta )*( 1.0+zeta );
	evaluatedValues[6] = 0.125*( 1.0-xi )*( 1.0+eta )*( 1.0+zeta );
	evaluatedValues[7] = 0.125*( 1.0+xi )*( 1.0+eta )*( 1.0+zeta );
	evaluatedValues[5] = 0.125*( 1.0+xi )*( 1.0-eta )*( 1.0+zeta );
}


void _TrilinearElementType_SF_allLocalDerivs_allNodes( void* elementType, const double localCoord[],
		double** const evaluatedDerivatives )
{		
	double xi, eta, zeta;
	
	xi   = localCoord[0];
	eta  = localCoord[1];
	zeta = localCoord[2];	
	
	/* derivatives wrt xi */
	evaluatedDerivatives[0][0] = - 0.125*( 1.0-eta )*( 1.0-zeta );
	evaluatedDerivatives[0][2] = - 0.125*( 1.0+eta )*( 1.0-zeta );
	evaluatedDerivatives[0][3] =   0.125*( 1.0+eta )*( 1.0-zeta );
	evaluatedDerivatives[0][1] =   0.125*( 1.0-eta )*( 1.0-zeta );
	evaluatedDerivatives[0][4] = - 0.125*( 1.0-eta )*( 1.0+zeta );
	evaluatedDerivatives[0][6] = - 0.125*( 1.0+eta )*( 1.0+zeta );
	evaluatedDerivatives[0][7] =   0.125*( 1.0+eta )*( 1.0+zeta );
	evaluatedDerivatives[0][5] =   0.125*( 1.0-eta )*( 1.0+zeta );
	
	/* derivatives wrt eta */	
	evaluatedDerivatives[1][0] = - 0.125*( 1.0-xi )*( 1.0-zeta );
	evaluatedDerivatives[1][2] =   0.125*( 1.0-xi )*( 1.0-zeta );
	evaluatedDerivatives[1][3] =   0.125*( 1.0+xi )*( 1.0-zeta );
	evaluatedDerivatives[1][1] = - 0.125*( 1.0+xi )*( 1.0-zeta );
	evaluatedDerivatives[1][4] = - 0.125*( 1.0-xi )*( 1.0+zeta );
	evaluatedDerivatives[1][6] =   0.125*( 1.0-xi )*( 1.0+zeta );
	evaluatedDerivatives[1][7] =   0.125*( 1.0+xi )*( 1.0+zeta );
	evaluatedDerivatives[1][5] = - 0.125*( 1.0+xi )*( 1.0+zeta );
	
	/* derivatives wrt zeta */		
	evaluatedDerivatives[2][0] = -0.125*( 1.0-xi )*( 1.0-eta );
	evaluatedDerivatives[2][2] = -0.125*( 1.0-xi )*( 1.0+eta );
	evaluatedDerivatives[2][3] = -0.125*( 1.0+xi )*( 1.0+eta );
	evaluatedDerivatives[2][1] = -0.125*( 1.0+xi )*( 1.0-eta );
	evaluatedDerivatives[2][4] =  0.125*( 1.0-xi )*( 1.0-eta );
	evaluatedDerivatives[2][6] =  0.125*( 1.0-xi )*( 1.0+eta );
	evaluatedDerivatives[2][7] =  0.125*( 1.0+xi )*( 1.0+eta );
	evaluatedDerivatives[2][5] =  0.125*( 1.0+xi )*( 1.0-eta );
}


#if 0
void _TrilinearElementType_ConvertGlobalCoordToElLocal(
		void*		elementType,
		void*		_mesh, 
		unsigned	element, 
		const double*	globalCoord,
		double*		elLocalCoord )
{
	TrilinearElementType*	self = (TrilinearElementType*)elementType;
	Mesh*			mesh = (Mesh*)_mesh;
	unsigned		inside;
	double			bc[4];
	static double		lCrds[8][3] = {{-1.0, -1.0, -1.0}, {1.0, -1.0, -1.0}, 
					       {-1.0, 1.0, -1.0}, {1.0, 1.0, -1.0}, 
					       {-1.0, -1.0, 1.0}, {1.0, -1.0, 1.0}, 
					       {-1.0, 1.0, 1.0}, {1.0, 1.0, 1.0}};
	unsigned		nInc, *inc;
	unsigned		bc_i;

	Mesh_GetIncidence( mesh, MT_VOLUME, element, MT_VERTEX, self->inc );
	nInc = IArray_GetSize( self->inc );
	inc = IArray_GetPtr( self->inc );
	assert( nInc == 8 );

	insist( Simplex_Search3D( mesh->verts, inc, 10, self->tetInds, (double*)globalCoord, bc, &inside ), == True );

	elLocalCoord[0] = bc[0] * lCrds[self->tetInds[inside][0]][0];
	elLocalCoord[1] = bc[0] * lCrds[self->tetInds[inside][0]][1];
	elLocalCoord[2] = bc[0] * lCrds[self->tetInds[inside][0]][2];
	for( bc_i = 1; bc_i < 4; bc_i++ ) {
		elLocalCoord[0] += bc[bc_i] * lCrds[self->tetInds[inside][bc_i]][0];
		elLocalCoord[1] += bc[bc_i] * lCrds[self->tetInds[inside][bc_i]][1];
		elLocalCoord[2] += bc[bc_i] * lCrds[self->tetInds[inside][bc_i]][2];
	}
}
#endif

double _TrilinearElementType_JacobianDeterminantSurface( void* elementType, void* _mesh, unsigned element_I, const double localCoord[],
	       						 unsigned face_I, unsigned norm ) 
{
	TrilinearElementType*	self		= (TrilinearElementType*) elementType;
	Mesh*			mesh		= (Mesh*)_mesh;
	unsigned		surfaceDim[2];
	double			x[4], y[4];
	double			s, t;
	double			dxds, dxdt, dyds, dydt;
	double			detJac;
	unsigned		nodes[4];

	surfaceDim[0] = ( norm + 1 ) % 3;
	surfaceDim[1] = ( norm + 2 ) % 3;

	s = localCoord[surfaceDim[0]];
	t = localCoord[surfaceDim[1]];

	ElementType_GetFaceNodes( self, mesh, element_I, face_I, 4, nodes );

	x[0] = Mesh_GetVertex( mesh, nodes[0] )[surfaceDim[0]];
	x[1] = Mesh_GetVertex( mesh, nodes[1] )[surfaceDim[0]];
	x[2] = Mesh_GetVertex( mesh, nodes[2] )[surfaceDim[0]];
	x[3] = Mesh_GetVertex( mesh, nodes[3] )[surfaceDim[0]];

	y[0] = Mesh_GetVertex( mesh, nodes[0] )[surfaceDim[1]];
	y[1] = Mesh_GetVertex( mesh, nodes[1] )[surfaceDim[1]];
	y[2] = Mesh_GetVertex( mesh, nodes[2] )[surfaceDim[1]];
	y[3] = Mesh_GetVertex( mesh, nodes[3] )[surfaceDim[1]];

	dxds = 0.25 * ( - ( 1.0 - t )*x[0] + ( 1.0 - t )*x[1] - ( 1.0 + t )*x[2] + ( 1.0 + t )*x[3] );
	dyds = 0.25 * ( - ( 1.0 - t )*y[0] + ( 1.0 - t )*y[1] - ( 1.0 + t )*y[2] + ( 1.0 + t )*y[3] );
	dxdt = 0.25 * ( - ( 1.0 - s )*x[0] - ( 1.0 + s )*x[1] + ( 1.0 - s )*x[2] + ( 1.0 + s )*x[3] );
	dydt = 0.25 * ( - ( 1.0 - s )*y[0] - ( 1.0 + s )*y[1] + ( 1.0 - s )*y[2] + ( 1.0 + s )*y[3] );
	
	detJac = dxds * dydt - dxdt * dyds;

	return fabs( detJac );
}



