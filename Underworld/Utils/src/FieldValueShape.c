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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include "types.h"
#include "FieldValueShape.h"

#include <assert.h>
#include <string.h>
#include <math.h>


/* Textual name of this class */
const Type FieldValueShape_Type = (const Type)"FieldValueShape";

/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/
FieldValueShape* FieldValueShape_New(
		Name               name,
		Dimension_Index    dim,
		XYZ                centre, 
		double             alpha,                                                                              
		double             beta,
		double             gamma,
		FieldVariable*     valueField,
		double             lowerLimit,
		double             upperLimit)
{
	FieldValueShape* self = (FieldValueShape*) _FieldValueShape_DefaultNew( name );

   _Stg_Shape_Init( self, dim, centre, False, alpha, beta, gamma );
	_FieldValueShape_Init( self, valueField, lowerLimit, upperLimit );

	LiveComponentRegister_IfRegThenAdd(self);
  
	return self;
}

FieldValueShape* _FieldValueShape_New(  FIELDVALUESHAPE_DEFARGS  )
{
	FieldValueShape* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(FieldValueShape) );
	self = (FieldValueShape*)_Stg_Shape_New(  STG_SHAPE_PASSARGS  );
	
	/* General info */
	return self;
}

void _FieldValueShape_Init( void* fieldValueShape, FieldVariable* valueField, double lowerLimit, double upperLimit ) {
	FieldValueShape* self = (FieldValueShape*)fieldValueShape;

	self->valueField = valueField;
	self->lowerLimit = lowerLimit;
	self->upperLimit = upperLimit;
}

/*------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/
void _FieldValueShape_Delete( void* fieldValueShape ) {
	FieldValueShape* self = (FieldValueShape*)fieldValueShape;
	
	/* Delete parent */
	_Stg_Shape_Delete( self );
}

void _FieldValueShape_Print( void* fieldValueShape, Stream* stream ) {
	FieldValueShape* self = (FieldValueShape*)fieldValueShape;
	
	/* Print parent */
	_Stg_Shape_Print( self, stream );
}

void* _FieldValueShape_Copy( void* fieldValueShape, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
	FieldValueShape*	self = (FieldValueShape*)fieldValueShape;
	FieldValueShape*	newFieldValueShape;
	
	newFieldValueShape = (FieldValueShape*)_Stg_Shape_Copy( self, dest, deep, nameExt, ptrMap );

	newFieldValueShape->valueField = self->valueField;
	newFieldValueShape->lowerLimit = self->lowerLimit;
	newFieldValueShape->upperLimit = self->upperLimit;
	
	return (void*)newFieldValueShape;
}

void* _FieldValueShape_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                  _sizeOfSelf = sizeof(FieldValueShape);
	Type                                                          type = FieldValueShape_Type;
	Stg_Class_DeleteFunction*                                  _delete = _FieldValueShape_Delete;
	Stg_Class_PrintFunction*                                    _print = _FieldValueShape_Print;
	Stg_Class_CopyFunction*                                      _copy = _FieldValueShape_Copy;
	Stg_Component_DefaultConstructorFunction*      _defaultConstructor = _FieldValueShape_DefaultNew;
	Stg_Component_ConstructFunction*                        _construct = _FieldValueShape_AssignFromXML;
	Stg_Component_BuildFunction*                                _build = _FieldValueShape_Build;
	Stg_Component_InitialiseFunction*                      _initialise = _FieldValueShape_Initialise;
	Stg_Component_ExecuteFunction*                            _execute = _FieldValueShape_Execute;
	Stg_Component_DestroyFunction*                            _destroy = _FieldValueShape_Destroy;
	Stg_Shape_IsCoordInsideFunction*                    _isCoordInside = _FieldValueShape_IsCoordInside;
	Stg_Shape_CalculateVolumeFunction*                _calculateVolume = _FieldValueShape_CalculateVolume;
	Stg_Shape_DistanceFromCenterAxisFunction*  _distanceFromCenterAxis = _FieldValueShape_DistanceFromCenterAxis;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*) _FieldValueShape_New(  FIELDVALUESHAPE_PASSARGS  );
}


void _FieldValueShape_AssignFromXML( void* fieldValueShape, Stg_ComponentFactory* cf, void* data ) {
	FieldValueShape*  self          = (FieldValueShape*) fieldValueShape;
	FieldVariable*    valueField;
	double lowerLimit, upperLimit;

	_Stg_Shape_AssignFromXML( self, cf, data );

	/** fieldVariable which determines values which must lie between limits to be considered inside shape */
   valueField = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"ValueField", FieldVariable, True, data  );

	/** value must be greater than this limit */
	lowerLimit = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"LowerLimit", 0.5  );
	/** value must be less than this limit */
	upperLimit = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"UpperLimit", 1.1  );

	_FieldValueShape_Init( self, valueField, lowerLimit, upperLimit );
}

void _FieldValueShape_Build( void* fieldValueShape, void* data ) {
	FieldValueShape*	self = (FieldValueShape*)fieldValueShape;
	
	Stg_Component_Build( self->valueField, NULL, False );
	/** go ahead and initialise this as well, as it is required before the standard init phase */
	Stg_Component_Initialise( self->valueField, NULL, False );	

	_Stg_Shape_Build( self, data );
}
void _FieldValueShape_Initialise( void* fieldValueShape, void* data ) {
	FieldValueShape*	self = (FieldValueShape*)fieldValueShape;

	Stg_Component_Initialise( self->valueField, NULL, False );	

	_Stg_Shape_Initialise( self, data );
}
void _FieldValueShape_Execute( void* fieldValueShape, void* data ) {
	FieldValueShape*	self = (FieldValueShape*)fieldValueShape;

	_Stg_Shape_Execute( self, data );
}
void _FieldValueShape_Destroy( void* fieldValueShape, void* data ) {
	FieldValueShape*	self = (FieldValueShape*)fieldValueShape;

	Stg_Component_Destroy( self->valueField, NULL, False );	
	_Stg_Shape_Destroy( self, data );
}

/*--------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/
	
/*---------------------------------------------------------------------------------------------------------------------
** Private Member functions
*/

Bool _FieldValueShape_IsCoordInside( void* fieldValueShape, Coord coord ) {
	FieldValueShape*    self       = (FieldValueShape*)fieldValueShape;
   double               value;
   InterpolationResult	interpRes;

   interpRes = FieldVariable_InterpolateValueAt( self->valueField, coord, &value );
   if( interpRes == OUTSIDE_GLOBAL ) return False;
   
	if ( (value > self->lowerLimit) && (value <= self->upperLimit) ) {
		return True;
	}
	return False;
}

double _FieldValueShape_CalculateVolume( void* fieldValueShape ) {
	FieldValueShape* self = (FieldValueShape*)fieldValueShape;
	Journal_Firewall( False, Journal_Register( Error_Type, (Name)self->type  ),
	"Error in function %s: This functions hasn't been implemented.", __func__ );
}

void _FieldValueShape_DistanceFromCenterAxis( void* shape, Coord coord, double* disVec ){
	Stg_Shape* self = (Stg_Shape*)shape;
	Journal_Firewall( False, Journal_Register( Error_Type, (Name)self->type  ),
	"Error in function %s: This functions hasn't been implemented.", __func__ );
}



