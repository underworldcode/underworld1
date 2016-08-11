/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**   Owen L. Kaluza, Software Developer, Monash University. (owen.kaluza@monash.edu)
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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

#include <string.h>
#include <mpi.h>
#include <StGermain/StGermain.h>

#include <StgDomain/Geometry/Geometry.h>
#include <StgDomain/Shape/Shape.h>
#include <StgDomain/Mesh/Mesh.h>

#include "types.h"
#include "FieldVariable.h"
#include "CoordinateFieldVariable.h"
#include "DomainContext.h"
#include "FieldVariable_Register.h"

#include <assert.h>

const Type CoordinateFieldVariable_Type = "CoordinateFieldVariable";
const Name defaultCoordinateFieldVariableName = "defaultCoordinateFieldVariableName";

CoordinateFieldVariable* _CoordinateFieldVariable_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                                      _sizeOfSelf = sizeof(CoordinateFieldVariable);
   Type                                                              type = CoordinateFieldVariable_Type;
   Stg_Class_DeleteFunction*                                      _delete = _CoordinateFieldVariable_Delete;
   Stg_Class_PrintFunction*                                        _print = _CoordinateFieldVariable_Print;
   Stg_Class_CopyFunction*                                          _copy = _CoordinateFieldVariable_Copy;
   Stg_Component_DefaultConstructorFunction*          _defaultConstructor = (Stg_Component_DefaultConstructorFunction*)_CoordinateFieldVariable_DefaultNew;
   Stg_Component_ConstructFunction*                            _construct = _CoordinateFieldVariable_AssignFromXML;
   Stg_Component_BuildFunction*                                    _build = _CoordinateFieldVariable_Build;
   Stg_Component_InitialiseFunction*                          _initialise = _CoordinateFieldVariable_Initialise;
   Stg_Component_ExecuteFunction*                                _execute = _CoordinateFieldVariable_Execute;
   Stg_Component_DestroyFunction*                                _destroy = _CoordinateFieldVariable_Destroy;
   AllocationType                                      nameAllocationType = NON_GLOBAL;
   FieldVariable_InterpolateValueAtFunction*          _interpolateValueAt = _CoordinateFieldVariable_InterpolateValueAt;
   FieldVariable_GetValueFunction*            _getMinGlobalFieldMagnitude = _CoordinateFieldVariable_GetMinLocalFieldMagnitude;
   FieldVariable_GetValueFunction*            _getMaxGlobalFieldMagnitude = _CoordinateFieldVariable_GetMaxLocalFieldMagnitude;
   FieldVariable_CacheValuesFunction*    _cacheMinMaxGlobalFieldMagnitude = _CoordinateFieldVariable_CacheMinMaxGlobalFieldMagnitude;
   FieldVariable_GetCoordFunction*               _getMinAndMaxLocalCoords = _CoordinateFieldVariable_GetMinAndMaxLocalCoords;
   FieldVariable_GetCoordFunction*              _getMinAndMaxGlobalCoords = _CoordinateFieldVariable_GetMinAndMaxGlobalCoords;

   return _CoordinateFieldVariable_New( COORDINATEFIELDVARIABLE_PASSARGS );
}

CoordinateFieldVariable* _CoordinateFieldVariable_New( COORDINATEFIELDVARIABLE_DEFARGS ) {
   CoordinateFieldVariable* self;
   
   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(CoordinateFieldVariable) );
   self = (CoordinateFieldVariable*) _FieldVariable_New(  FIELDVARIABLE_PASSARGS  );

   return self;
} 

void _CoordinateFieldVariable_Delete( void* _fieldVariable ) {
   CoordinateFieldVariable* self = (CoordinateFieldVariable*) _fieldVariable;

   _FieldVariable_Delete( self );
}

void _CoordinateFieldVariable_Print( void* _fieldVariable, Stream* stream ) {
   CoordinateFieldVariable* self = (CoordinateFieldVariable*) _fieldVariable;
   Index                  fieldVariable_I;

   _FieldVariable_Print( self, stream );
}

void _CoordinateFieldVariable_Init(
   void*           cfv,
   Axis            axis)
{
   CoordinateFieldVariable* self = (CoordinateFieldVariable*)cfv;
   Stream*                errorStream = Journal_Register( Error_Type, (Name)self->type  );

   self->axis = axis;
}

void* _CoordinateFieldVariable_Copy(
   void*   fieldVariable,
   void*   dest,
   Bool    deep,
   Name    nameExt,
   PtrMap* ptrMap )
{
   CoordinateFieldVariable* self = (CoordinateFieldVariable*)fieldVariable;
   CoordinateFieldVariable* newCoordinateFieldVariable;
   
   newCoordinateFieldVariable = _FieldVariable_Copy( self, dest, deep, nameExt, ptrMap );
   newCoordinateFieldVariable->axis = self->axis;
   newCoordinateFieldVariable->fieldVariable = self->fieldVariable;
   
   return (void*)newCoordinateFieldVariable;
}

void _CoordinateFieldVariable_AssignFromXML( void* fieldVariable, Stg_ComponentFactory* cf, void* data ) {
   CoordinateFieldVariable* self = (CoordinateFieldVariable*) fieldVariable;
   Name axisStr;
   Axis axis;
   
   /* Construct Parent */
   _FieldVariable_AssignFromXML( self, cf, data );

   self->fieldVariableName = Memory_Alloc_Array(char, 50, "fieldVariableName");
   strcpy(self->fieldVariableName, "FieldVariable");

   axisStr = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"axis", "y");
   /* Axis X/Y/Z */
   if ( toupper( axisStr[0] ) >= 'x' )
      axis = toupper( axisStr[0] ) - 'x';   /* x=0 y=1 z=2 */
   else if ( toupper( axisStr[0] ) >= 'X' )
      axis = toupper( axisStr[0] ) - 'X';
   else /* Assume 0/1/2 */
      axis = toupper( axisStr[0] ) - '0';

   if (axis < 0) axis = 0;
   if (axis > 2) axis = 2;

   _CoordinateFieldVariable_Init( self, axis);
}

void _CoordinateFieldVariable_Build( void* fieldVariable, void* data ) {
   CoordinateFieldVariable* self = (CoordinateFieldVariable*) fieldVariable;
   Index                  fieldVariable_I;
   AbstractContext* context = self->context;
   Stg_ComponentFactory* cf = context->CF;

   self->fieldVariable =  Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)self->fieldVariableName, FieldVariable, True, data  );
   Stg_Component_Build( self->fieldVariable, data, False );
}

void _CoordinateFieldVariable_Execute( void* fieldVariable, void* data ) {}

void _CoordinateFieldVariable_Destroy( void* fieldVariable, void* data ) {
   CoordinateFieldVariable* self = (CoordinateFieldVariable*) fieldVariable;

   _FieldVariable_Destroy( self, data );
}

void _CoordinateFieldVariable_Initialise( void* fieldVariable, void* data ) {
   CoordinateFieldVariable* self = (CoordinateFieldVariable*) fieldVariable;
   Index                  fieldVariable_I;

   Stg_Component_Initialise( self->fieldVariable, data, False );
}

double _CoordinateFieldVariable_GetMinLocalFieldMagnitude( void* fieldVariable )
{
   CoordinateFieldVariable* self = (CoordinateFieldVariable*) fieldVariable;
   Coord min, max;
   _CoordinateFieldVariable_GetMinAndMaxLocalCoords(fieldVariable, min, max);
   return min[self->axis];
}

double _CoordinateFieldVariable_GetMaxLocalFieldMagnitude( void* fieldVariable )
{
   CoordinateFieldVariable* self = (CoordinateFieldVariable*) fieldVariable;
   Coord min, max;
   _CoordinateFieldVariable_GetMinAndMaxLocalCoords(fieldVariable, min, max);
   return max[self->axis];
}

void _CoordinateFieldVariable_CacheMinMaxGlobalFieldMagnitude( void* fieldVariable ) {
   CoordinateFieldVariable* self = (CoordinateFieldVariable*) fieldVariable;
   self->magnitudeMin = _CoordinateFieldVariable_GetMinLocalFieldMagnitude(self);
   self->magnitudeMax = _CoordinateFieldVariable_GetMaxLocalFieldMagnitude(self);
}

void  _CoordinateFieldVariable_GetMinAndMaxLocalCoords( void* fieldVariable, Coord min, Coord max ) {
   CoordinateFieldVariable* self = (CoordinateFieldVariable*) fieldVariable;

   FieldVariable_GetMinAndMaxLocalCoords( self->fieldVariable, min, max );
}

void  _CoordinateFieldVariable_GetMinAndMaxGlobalCoords( void* fieldVariable, Coord min, Coord max ) {
   CoordinateFieldVariable* self = (CoordinateFieldVariable*) fieldVariable;

   FieldVariable_GetMinAndMaxGlobalCoords( self->fieldVariable, min, max );
}

InterpolationResult _CoordinateFieldVariable_InterpolateValueAt(
   void*   fieldVariable,
   Coord   coord,
   double* value )
{
   CoordinateFieldVariable* self = (CoordinateFieldVariable*) fieldVariable;

   value[0] = coord[self->axis];
   return LOCAL;
}

