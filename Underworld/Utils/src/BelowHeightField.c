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
#include "BelowHeightField.h"

#include <assert.h>
#include <string.h>
#include <math.h>


/* Textual name of this class */
const Type BelowHeightField_Type = (const Type)"BelowHeightField";

/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/
BelowHeightField* _BelowHeightField_New(  BELOWHEIGHTFIELD_DEFARGS  )
{
   BelowHeightField* self;
   
   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(BelowHeightField) );
   self = (BelowHeightField*)_Stg_Shape_New(  STG_SHAPE_PASSARGS  );
   
   /* General info */
   return self;
}

/*------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/
void _BelowHeightField_Delete( void* belowHeightField ) {
   BelowHeightField* self = (BelowHeightField*)belowHeightField;
   
   /* Delete parent */
   _Stg_Shape_Delete( self );
}

void _BelowHeightField_Print( void* belowHeightField, Stream* stream ) {
   BelowHeightField* self = (BelowHeightField*)belowHeightField;
   
   /* Print parent */
   _Stg_Shape_Print( self, stream );
}

void* _BelowHeightField_Copy( void* belowHeightField, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
   BelowHeightField* self = (BelowHeightField*)belowHeightField;
   BelowHeightField* newBelowHeightField;
   
   newBelowHeightField = (BelowHeightField*)_Stg_Shape_Copy( self, dest, deep, nameExt, ptrMap );

   newBelowHeightField->heightField = self->heightField;
   
   return (void*)newBelowHeightField;
}

void* _BelowHeightField_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                                  _sizeOfSelf = sizeof(BelowHeightField);
   Type                                                          type = BelowHeightField_Type;
   Stg_Class_DeleteFunction*                                  _delete = _BelowHeightField_Delete;
   Stg_Class_PrintFunction*                                    _print = _BelowHeightField_Print;
   Stg_Class_CopyFunction*                                      _copy = _BelowHeightField_Copy;
   Stg_Component_DefaultConstructorFunction*      _defaultConstructor = _BelowHeightField_DefaultNew;
   Stg_Component_ConstructFunction*                        _construct = _BelowHeightField_AssignFromXML;
   Stg_Component_BuildFunction*                                _build = _BelowHeightField_Build;
   Stg_Component_InitialiseFunction*                      _initialise = _BelowHeightField_Initialise;
   Stg_Component_ExecuteFunction*                            _execute = _BelowHeightField_Execute;
   Stg_Component_DestroyFunction*                            _destroy = _BelowHeightField_Destroy;
   Stg_Shape_IsCoordInsideFunction*                    _isCoordInside = _BelowHeightField_IsCoordInside;
   Stg_Shape_CalculateVolumeFunction*                _calculateVolume = _BelowHeightField_CalculateVolume;
   Stg_Shape_DistanceFromCenterAxisFunction*  _distanceFromCenterAxis = _BelowHeightField_DistanceFromCenterAxis;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _BelowHeightField_New(  BELOWHEIGHTFIELD_PASSARGS  );
}


void _BelowHeightField_AssignFromXML( void* belowHeightField, Stg_ComponentFactory* cf, void* data ) {
   BelowHeightField*  self          = (BelowHeightField*) belowHeightField;
   FieldVariable*     heightField;

   _Stg_Shape_AssignFromXML( self, cf, data );

   /** fieldVariable which determines height values at any location within domain */
   self->heightField = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"HeightField", FieldVariable, True, data  );
   /** determines which axis should be considered the vertical axis */
   self->vertAxis = Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"VerticalAxis", 1 );

}

void _BelowHeightField_Build( void* belowHeightField, void* data ) {
   BelowHeightField* self = (BelowHeightField*)belowHeightField;
   
   Stg_Component_Build( self->heightField, NULL, False );
   /** go ahead and initialise this as well, as it is required before the standard init phase */
   Stg_Component_Initialise( self->heightField, NULL, False ); 

   _Stg_Shape_Build( self, data );
}
void _BelowHeightField_Initialise( void* belowHeightField, void* data ) {
   BelowHeightField* self = (BelowHeightField*)belowHeightField;

   Stg_Component_Initialise( self->heightField, NULL, False ); 

   _Stg_Shape_Initialise( self, data );
}
void _BelowHeightField_Execute( void* belowHeightField, void* data ) {
   BelowHeightField* self = (BelowHeightField*)belowHeightField;

   _Stg_Shape_Execute( self, data );
}
void _BelowHeightField_Destroy( void* belowHeightField, void* data ) {
   BelowHeightField* self = (BelowHeightField*)belowHeightField;

   Stg_Component_Destroy( self->heightField, NULL, False ); 
   _Stg_Shape_Destroy( self, data );
}

/*--------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/
   
/*---------------------------------------------------------------------------------------------------------------------
** Private Member functions
*/

Bool _BelowHeightField_IsCoordInside( void* belowHeightField, Coord coord ) {
   BelowHeightField*    self       = (BelowHeightField*)belowHeightField;
   double               height;
   InterpolationResult  interpRes;

   /** first check global max of field before proceeding further */
   if( FieldVariable_GetMaxGlobalFieldMagnitude( self->heightField ) < coord[ self->vertAxis ] )
      return False;

   interpRes = FieldVariable_InterpolateValueAt( self->heightField, coord, &height );
   if( interpRes == OUTSIDE_GLOBAL ) return False;
   
   if ( coord[ self->vertAxis ] < height ) {
      return True;
   }
   return False;
}

void _BelowHeightField_CalculateVolume( void* belowHeightField ) {
   BelowHeightField* self = (BelowHeightField*)belowHeightField;

   Journal_Firewall( False, Journal_Register( Error_Type, (Name)self->type  ),
      "Error in function %s: This functions hasn't been implemented.", __func__ );
}

void _BelowHeightField_DistanceFromCenterAxis( void* shape, Coord coord, double* disVec ){
   Stg_Shape* self = (Stg_Shape*)shape;

   Journal_Firewall( False, Journal_Register( Error_Type, (Name)self->type  ),
      "Error in function %s: This functions hasn't been implemented.", __func__ );
}



