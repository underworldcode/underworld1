/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
** Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
** Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
** Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
** Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org) )
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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include "types.h"
#include "BetweenHeightFields.h"

#include <assert.h>
#include <string.h>
#include <math.h>


/* Textual name of this class */
const Type BetweenHeightFields_Type = (const Type)"BetweenHeightFields";

/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/

BetweenHeightFields* _BetweenHeightFields_New(  BETWEENHEIGHTFIELDS_DEFARGS  )
{
   BetweenHeightFields* self;
   
   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(BetweenHeightFields) );
   self = (BetweenHeightFields*)_Stg_Shape_New(  STG_SHAPE_PASSARGS  );
   
   /* General info */
   return self;
}

/*------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/
void _BetweenHeightFields_Delete( void* betweenHeightFields ) {
   BetweenHeightFields* self = (BetweenHeightFields*)betweenHeightFields;
   
   /* Delete parent */
   _Stg_Shape_Delete( self );
}

void _BetweenHeightFields_Print( void* betweenHeightFields, Stream* stream ) {
   BetweenHeightFields* self = (BetweenHeightFields*)betweenHeightFields;
   
   /* Print parent */
   _Stg_Shape_Print( self, stream );
}

void* _BetweenHeightFields_Copy( void* betweenHeightFields, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
   BetweenHeightFields* self = (BetweenHeightFields*)betweenHeightFields;
   BetweenHeightFields* newBetweenHeightFields;
   
   newBetweenHeightFields = (BetweenHeightFields*)_Stg_Shape_Copy( self, dest, deep, nameExt, ptrMap );

   newBetweenHeightFields->topHeightField    = self->topHeightField;
   newBetweenHeightFields->bottomHeightField = self->bottomHeightField;
   
   return (void*)newBetweenHeightFields;
}

void* _BetweenHeightFields_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                                  _sizeOfSelf = sizeof(BetweenHeightFields);
   Type                                                          type = BetweenHeightFields_Type;
   Stg_Class_DeleteFunction*                                  _delete = _BetweenHeightFields_Delete;
   Stg_Class_PrintFunction*                                    _print = _BetweenHeightFields_Print;
   Stg_Class_CopyFunction*                                      _copy = _BetweenHeightFields_Copy;
   Stg_Component_DefaultConstructorFunction*      _defaultConstructor = _BetweenHeightFields_DefaultNew;
   Stg_Component_ConstructFunction*                        _construct = _BetweenHeightFields_AssignFromXML;
   Stg_Component_BuildFunction*                                _build = _BetweenHeightFields_Build;
   Stg_Component_InitialiseFunction*                      _initialise = _BetweenHeightFields_Initialise;
   Stg_Component_ExecuteFunction*                            _execute = _BetweenHeightFields_Execute;
   Stg_Component_DestroyFunction*                            _destroy = _BetweenHeightFields_Destroy;
   Stg_Shape_IsCoordInsideFunction*                    _isCoordInside = _BetweenHeightFields_IsCoordInside;
   Stg_Shape_CalculateVolumeFunction*                _calculateVolume = _BetweenHeightFields_CalculateVolume;
   Stg_Shape_DistanceFromCenterAxisFunction*  _distanceFromCenterAxis = _BetweenHeightFields_DistanceFromCenterAxis;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _BetweenHeightFields_New(  BETWEENHEIGHTFIELDS_PASSARGS  );
}


void _BetweenHeightFields_AssignFromXML( void* betweenHeightFields, Stg_ComponentFactory* cf, void* data ) {
   BetweenHeightFields*  self          = (BetweenHeightFields*) betweenHeightFields;
   FieldVariable*        topHeightField;
   FieldVariable*     bottomHeightField;

   _Stg_Shape_AssignFromXML( self, cf, data );

   /** fieldVariables which determines upper and lower height values at any location within domain */
   self->topHeightField    = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"TopHeightField"   , FieldVariable, True, data  );
   self->bottomHeightField = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"BottomHeightField", FieldVariable, True, data  );
   /** determines which axis should be considered the vertical axis */
   self->vertAxis = Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"VerticalAxis", 1 );

}

void _BetweenHeightFields_Build( void* betweenHeightFields, void* data ) {
   BetweenHeightFields* self = (BetweenHeightFields*)betweenHeightFields;
   
   Stg_Component_Build( self->topHeightField   , NULL, False );
   Stg_Component_Build( self->bottomHeightField, NULL, False );
   /** go ahead and initialise this as well, as it is required before the standard init phase */
   Stg_Component_Initialise( self->topHeightField   , NULL, False );
   Stg_Component_Initialise( self->bottomHeightField, NULL, False );

   _Stg_Shape_Build( self, data );
}
void _BetweenHeightFields_Initialise( void* betweenHeightFields, void* data ) {
   BetweenHeightFields* self = (BetweenHeightFields*)betweenHeightFields;

   Stg_Component_Initialise( self->topHeightField   , NULL, False );
   Stg_Component_Initialise( self->bottomHeightField, NULL, False );

   _Stg_Shape_Initialise( self, data );
}
void _BetweenHeightFields_Execute( void* betweenHeightFields, void* data ) {
   BetweenHeightFields* self = (BetweenHeightFields*)betweenHeightFields;

   _Stg_Shape_Execute( self, data );
}
void _BetweenHeightFields_Destroy( void* betweenHeightFields, void* data ) {
   BetweenHeightFields* self = (BetweenHeightFields*)betweenHeightFields;

   Stg_Component_Destroy( self->topHeightField   , NULL, False );
   Stg_Component_Destroy( self->bottomHeightField, NULL, False );
   _Stg_Shape_Destroy( self, data );
}

/*--------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/
   
/*---------------------------------------------------------------------------------------------------------------------
** Private Member functions
*/

Bool _BetweenHeightFields_IsCoordInside( void* betweenHeightFields, Coord coord ) {
   BetweenHeightFields*  self       = (BetweenHeightFields*)betweenHeightFields;
   double                topHeight, bottomHeight;
   InterpolationResult   interpRes;

   /** first check global max/min of fields before proceeding further */
   if( FieldVariable_GetMaxGlobalFieldMagnitude( self->topHeightField )    < coord[ self->vertAxis ] )
      return False;
   if( FieldVariable_GetMinGlobalFieldMagnitude( self->bottomHeightField ) > coord[ self->vertAxis ] )
      return False;
   
   interpRes = FieldVariable_InterpolateValueAt(    self->topHeightField, coord,    &topHeight );
   if( interpRes == OUTSIDE_GLOBAL ) return False;
   interpRes = FieldVariable_InterpolateValueAt( self->bottomHeightField, coord, &bottomHeight );
   if( interpRes == OUTSIDE_GLOBAL ) return False;
   
   /** if between layers, return True */
   if ( coord[ self->vertAxis ] < topHeight && coord[ self->vertAxis ] > bottomHeight ) {
      return True;
   }
   return False;
}

double _BetweenHeightFields_CalculateVolume( void* betweenHeightFields ) {
   BetweenHeightFields* self = (BetweenHeightFields*)betweenHeightFields;
   Journal_Firewall( False, Journal_Register( Error_Type, (Name)self->type  ),
   "Error in function %s: This functions hasn't been implemented.", __func__ );
}

void _BetweenHeightFields_DistanceFromCenterAxis( void* shape, Coord coord, double* disVec ){
   Stg_Shape* self = (Stg_Shape*)shape;
   Journal_Firewall( False, Journal_Register( Error_Type, (Name)self->type  ),
   "Error in function %s: This functions hasn't been implemented.", __func__ );
}



