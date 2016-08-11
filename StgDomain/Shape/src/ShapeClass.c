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

#include "types.h"
#include "ShapeClass.h"

#include <assert.h>
#include <string.h>
#include <math.h>


/* Textual name of this class */
const Type Stg_Shape_Type = "Stg_Shape";

/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/

Stg_Shape* _Stg_Shape_New(  STG_SHAPE_DEFARGS  )
{
	Stg_Shape* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(Stg_Shape) );
	/* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
	/* This means that any values of these parameters that are passed into this function are not passed onto the parent function
	   and so should be set to ZERO in any children of this class. */
	nameAllocationType = NON_GLOBAL;

	self = (Stg_Shape*)_Stg_Component_New(  STG_COMPONENT_PASSARGS  );
		
	/* General info */

	/* Virtual Info */
	self->_isCoordInside = _isCoordInside;
	self->_calculateVolume = _calculateVolume;
	self->_distanceFromCenterAxis = _distanceFromCenterAxis,

	self->invert = False;
	
	return self;
}
	
void _Stg_Shape_Init( void* shape, Dimension_Index dim, Coord centre, Bool invert, double alpha, double beta, double gamma ) {
	Stg_Shape* self = (Stg_Shape*) shape;
    double convert = (M_PI/180.0);
	self->isConstructed = True;

	self->dim = dim;
	memcpy( self->centre, centre, sizeof(Coord) );

	self->invert = invert;
    alpha = convert * alpha;
    beta =  convert * beta;
    gamma = convert * gamma;

   /* coordinate system of StgDomain
                   +ve y
                    ^
                    |
                    |          -ve z
                    |        /
                    |      /
                    |    /
                    |  /
      -ve x _______________________________ +ve x
                  / |
                /   |
              /     |
            /       |
          +ve z     -ve y
   */

		/* There are six possible orderings of the three rotations: 
			XYZ, YXZ, XZY, ZXY, YZX, ZYX. None are 
		    equivalent in the general case */



   /* store rotations */
   self->rotations[0] = alpha; /* rotation in x-y */
   self->rotations[1] = beta; /* rotation in x-z */
   self->rotations[2] = gamma; /* rotation in y-z */

   /* This case is the ZYX rotation */ 

   self->rotationMatrix[0][I_AXIS] = cos(alpha) * cos(beta);
   self->rotationMatrix[1][I_AXIS] = - cos(beta) * sin(alpha);
   self->rotationMatrix[2][I_AXIS] = sin(beta);
   
   self->rotationMatrix[0][J_AXIS] = cos(gamma) * sin(alpha) + sin(gamma) * sin(beta) * cos(alpha);
   self->rotationMatrix[1][J_AXIS] = cos(gamma)* cos(alpha) - sin(gamma)*sin(beta)*sin(alpha);
   self->rotationMatrix[2][J_AXIS] = -sin(gamma) * cos(beta);
   
   self->rotationMatrix[0][K_AXIS] = sin(gamma)*sin(alpha) - cos(gamma)*sin(beta)*cos(alpha);
   self->rotationMatrix[1][K_AXIS] = sin(gamma)*cos(alpha) + cos(gamma)*sin(beta)*sin(alpha);
   self->rotationMatrix[2][K_AXIS] = cos(gamma)*cos(beta);
   
}


void Stg_Shape_InitAll( void* shape, Dimension_Index dim, Coord centre, double alpha, double beta, double gamma ) {
	Stg_Shape* self = (Stg_Shape*) shape;
	
	_Stg_Shape_Init( self, dim, centre, False, alpha, beta, gamma );
}
/*----------------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _Stg_Shape_Delete( void* shape ) {
	Stg_Shape* self = (Stg_Shape*) shape;
	
	/* Stg_Class_Delete parent */
	_Stg_Component_Delete( self );
}


void _Stg_Shape_Print( void* superellipsoid, Stream* stream ) {
	Stg_Shape* self = (Stg_Shape*)superellipsoid;
	
	/* Print parent */
	_Stg_Component_Print( self, stream );
	
	/* General info */
	Journal_Printf( stream, "Stg_Shape (ptr): (%p)\n", self );

	Journal_PrintPointer( stream, self->_isCoordInside );
	

}

void* _Stg_Shape_Copy( void* shape, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
	Stg_Shape*	self = (Stg_Shape*) shape;
	Stg_Shape*	newStg_Shape;
	
	newStg_Shape = (Stg_Shape*)_Stg_Class_Copy( self, dest, deep, nameExt, ptrMap );
	
	newStg_Shape->_isCoordInside = self->_isCoordInside;
	newStg_Shape->dim = self->dim;

	memcpy( newStg_Shape->centre, self->centre, sizeof(XYZ));

	return (void*)newStg_Shape;
}

void _Stg_Shape_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) {
	Stg_Shape*	    self      = (Stg_Shape*) shape;
	Dimension_Index dim;
	Coord           centre;
	Bool            invert        = False;
	double          alpha, beta, gamma;

	self->context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", AbstractContext, False, data );
	if( !self->context  )
		self->context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", AbstractContext, True, data  );

	dim = Stg_ComponentFactory_GetRootDictUnsignedInt( cf, (Dictionary_Entry_Key)"dim", 0  );
	
	centre[ I_AXIS ] = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"CentreX", 0.0  );
	centre[ J_AXIS ] = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"CentreY", 0.0  );
	centre[ K_AXIS ] = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"CentreZ", 0.0  );

	invert = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"invert", False  );

	alpha = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"alpha", 0.0  );
	beta  = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"beta", 0.0  );
	gamma = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"gamma", 0.0  );

	_Stg_Shape_Init( self, dim, centre, invert, alpha, beta, gamma );
}


void StGermain_GetEulerAngles( 
		XYZ     newYDirection,
		XYZ     oldYDirection,
		XYZ     newXDirection,
		XYZ     oldXDirection, 
		double* alpha,
		double* beta,
		double* gamma ) 
{
	/* TODO */
	*alpha = 0.0;
	*beta  = 0.0;
	*gamma = 0.0;
}

void _Stg_Shape_Build( void* shape, void* data ) {
}

void _Stg_Shape_Initialise( void* shape, void* data ) {
}

void _Stg_Shape_Execute( void* shape, void* data ) {
}

void _Stg_Shape_Destroy( void* shape, void* data ) {
}

/*--------------------------------------------------------------------------------------------------------------------------
** Virtual Functions
*/
Bool Stg_Shape_IsCoordInside( void* shape, Coord coord ) {
	Stg_Shape* self              = (Stg_Shape*)shape;

	/* XOR the output */
	/* Truth table:
	 * | invert | isCoordInside | output | 
	 * | 0      | 0             | 0      | With invert off, leave the result as it is
	 * | 0      | 1             | 1      |
	 * | 1      | 0             | 1      | With invert on, flip the result
	 * | 1      | 1             | 0      |
	 *
	 * bitwise xor is used because ^^ (logical) is not ansi-c
	 */
	return (self->invert != 0) ^ (self->_isCoordInside( self, coord ) != 0);
}	

double Stg_Shape_CalculateVolume( void* shape ) {
	Stg_Shape* self = (Stg_Shape*)shape;
	if ( self->invert ) {
		return 1.0 - self->_calculateVolume( self );
	}
	else {
		return self->_calculateVolume( self );
	}
}

void Stg_Shape_DistanceFromCenterAxis( void* shape, Coord coord, double* disVec ) {
	Stg_Shape* self = (Stg_Shape*)shape;

	self->_distanceFromCenterAxis( self, coord, disVec );
}

/*--------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/

void Stg_Shape_TransformCoord( void* shape, Coord coord, Coord transformedCoord ) {
	Stg_Shape* self              = (Stg_Shape*)shape;
	Coord      rotatedCoord;

	Stg_Shape_TranslateCoord( self, coord, transformedCoord );

	/* Rotate Using Rotation Matrix */
	rotatedCoord[ I_AXIS ] = 
		  self->rotationMatrix[I_AXIS][0] * transformedCoord[0] 
		+ self->rotationMatrix[I_AXIS][1] * transformedCoord[1];
	
	rotatedCoord[ J_AXIS ] = 
		  self->rotationMatrix[J_AXIS][0] * transformedCoord[0] 
		+ self->rotationMatrix[J_AXIS][1] * transformedCoord[1];
	
	rotatedCoord[ K_AXIS ] = 
		  self->rotationMatrix[K_AXIS][0] * transformedCoord[0] 
		+ self->rotationMatrix[K_AXIS][1] * transformedCoord[1];

	if ( self->dim > 2 ) {
		rotatedCoord[ I_AXIS ] += self->rotationMatrix[I_AXIS][2] * transformedCoord[2];
		rotatedCoord[ J_AXIS ] += self->rotationMatrix[J_AXIS][2] * transformedCoord[2];
		rotatedCoord[ K_AXIS ] += self->rotationMatrix[K_AXIS][2] * transformedCoord[2];
	}

	memcpy( transformedCoord, rotatedCoord, sizeof(Coord));

}

void Stg_Shape_TranslateCoord( void* shape, Coord coord, Coord translatedCoord ) {
	Stg_Shape* self              = (Stg_Shape*)shape;

   self->dim == 2 ? 
     Vec_Sub2D( translatedCoord, coord, self->centre ) :
     Vec_Sub3D( translatedCoord, coord, self->centre ) ;
}
