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
#include "ConstantElementType.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

const Type ConstantElementType_Type = "ConstantElementType";
#define _ConstantElementType_NodeCount 1

ConstantElementType* ConstantElementType_New( Name name ) {
	ConstantElementType* self = ConstantElementType_DefaultNew( name );

	self->isConstructed = True;	
	_ElementType_Init( (ElementType*)self, _ConstantElementType_NodeCount );
	_ConstantElementType_Init( self );	

	return self;
}

void* ConstantElementType_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                                            _sizeOfSelf = sizeof(ConstantElementType);
	Type                                                                                    type = ConstantElementType_Type;
	Stg_Class_DeleteFunction*                                                            _delete = _ConstantElementType_Delete;
	Stg_Class_PrintFunction*                                                              _print = _ConstantElementType_Print;
	Stg_Class_CopyFunction*                                                                _copy = NULL;
	Stg_Component_DefaultConstructorFunction*                                _defaultConstructor = ConstantElementType_DefaultNew;
	Stg_Component_ConstructFunction*                                                  _construct = _ConstantElementType_AssignFromXML;
	Stg_Component_BuildFunction*                                                          _build = _ConstantElementType_Build;
	Stg_Component_InitialiseFunction*                                                _initialise = _ConstantElementType_Initialise;
	Stg_Component_ExecuteFunction*                                                      _execute = _ConstantElementType_Execute;
	Stg_Component_DestroyFunction*                                                      _destroy = _ConstantElementType_Destroy;
	AllocationType                                                            nameAllocationType = NON_GLOBAL;
	ElementType_EvaluateShapeFunctionsAtFunction*                      _evaluateShapeFunctionsAt = _ConstantElementType_SF_allNodes;
	ElementType_EvaluateShapeFunctionLocalDerivsAtFunction*  _evaluateShapeFunctionLocalDerivsAt = _ConstantElementType_SF_allLocalDerivs_allNodes;
	ElementType_ConvertGlobalCoordToElLocalFunction*                _convertGlobalCoordToElLocal = _ConstantElementType_ConvertGlobalCoordToElLocal;
	ElementType_JacobianDeterminantSurfaceFunction*                  _jacobianDeterminantSurface = _ElementType_JacobianDeterminantSurface;
	ElementType_SurfaceNormalFunction*                                            _surfaceNormal = _ConstantElementType_SurfaceNormal;

	return _ConstantElementType_New(  CONSTANTELEMENTTYPE_PASSARGS  );
}

ConstantElementType* _ConstantElementType_New(  CONSTANTELEMENTTYPE_DEFARGS  ) {
	ConstantElementType*		self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(ConstantElementType) );
	self = (ConstantElementType*)_ElementType_New(  ELEMENTTYPE_PASSARGS  );
	
	/* General info */
	
	/* Virtual functions */
	
	/* ConstantElementType info */

	return self;
}

void _ConstantElementType_Init( ConstantElementType* self ) {
	self->dim = 0;
}

void _ConstantElementType_Delete( void* elementType ) {
	ConstantElementType* self = (ConstantElementType*)elementType;

	/* Stg_Class_Delete parent*/
	_ElementType_Delete( self );
}

void _ConstantElementType_Print( void* elementType, Stream* stream ) {
	ConstantElementType* self = (ConstantElementType*)elementType;
	
	/* Set the Journal for printing informations */
	Stream* constantElementTypeStream = stream;
	
	/* General info */
	Journal_Printf( constantElementTypeStream, "ConstantElementType (ptr): %p\n", self );
	
	/* Print parent */
	_ElementType_Print( self, constantElementTypeStream );
	
	/* Virtual info */
	
	/* ConstantElementType info */
}

void _ConstantElementType_AssignFromXML( void* elementType, Stg_ComponentFactory *cf, void* data ){
	ConstantElementType* self = (ConstantElementType*)elementType;

	_ConstantElementType_Init( self );
}
	
void _ConstantElementType_Initialise( void* elementType, void *data ){
}
	
void _ConstantElementType_Execute( void* elementType, void *data ){
}
	
void _ConstantElementType_Destroy( void* elementType, void *data ){
	ConstantElementType* self = (ConstantElementType*)elementType;

	_ElementType_Destroy( self, data );
}

void _ConstantElementType_Build( void* elementType, void *data ) {

}

/*

 - Shape function definition
 - Constant shape function in 2d/3d has only one node at centroid of element.
 - Node id is 0 in both cases.
 - Local coordinate domain spans  -1 <= xi,eta <= 1 in 2d
 - Local coordinate domain spans  -1 <= xi,eta,zeta <= 1 in 3d

*/
void _ConstantElementType_SF_allNodes( void* elementType, const double localCoord[], double* const evaluatedValues ) {
	evaluatedValues[0] = 1.0;
}


/*
Since we use only have one constant shape func for 2d and 3d quads, then
if we want to return the zero derivatives we need to pass in "dim" so we know
whether to fill in GNi[0][0] = GNi[1][0] = 0.0 + GNi[2][0] = 0.0 if dim == 3
Should just return error if we try to take deriv of constant. No one would want
to do this!
*/
void _ConstantElementType_SF_allLocalDerivs_allNodes( void* elementType, const double localCoord[],
		double** const evaluatedDerivatives )
{		
	Stream* error = Journal_Register( ErrorStream_Type, (Name)ConstantElementType_Type  );
	/* all derivatives are zero*/
	Journal_Printf( error, "Error: Trying take derivative of a constant shape function \n" );
	assert( 0 );
}


void _ConstantElementType_ConvertGlobalCoordToElLocal(
		void*		elementType,
		void*		mesh, 
		unsigned	element, 
		const double*	globalCoord,
		double*		elLocalCoord )
{
	/* See header file function introduction for explanation... */
	elLocalCoord[0] = elLocalCoord[1] = elLocalCoord[2] = 0;
}

int _ConstantElementType_SurfaceNormal( void* elementType, unsigned element_I, unsigned dim, double* xi, double* normal ) {
	Stream*	errStream	= Journal_Register( ErrorStream_Type, (Name)ElementType_Type  );

	Journal_Printf( errStream, "surface normal not defined for this element type.\n" );
	assert( 0 );

	normal = NULL;

	return -1;
}



