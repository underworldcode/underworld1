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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include "types.h"

#include <assert.h>
#include <string.h>

#include "ConstantField.h"

const Type ConstantField_Type = (const Type)"ConstantField";

ConstantField* ConstantField_New( 
   Name           name,
   Index          fieldComponentCount,
   double*        value,
   FeMesh*        feMesh ) 
{	
	ConstantField* self = (ConstantField*) _ConstantField_DefaultNew( name );

	self->isConstructed = True;	
	_ConstantField_Init( self, fieldComponentCount, value, feMesh );

	return self;
}


void* _ConstantField_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                                      _sizeOfSelf = sizeof(ConstantField);
   Type                                                              type = ConstantField_Type;
   Stg_Class_DeleteFunction*                                      _delete = _FieldVariable_Delete;
   Stg_Class_PrintFunction*                                        _print = _FieldVariable_Print;
   Stg_Class_CopyFunction*                                          _copy = _FieldVariable_Copy;
   Stg_Component_DefaultConstructorFunction*          _defaultConstructor = _ConstantField_DefaultNew;
   Stg_Component_ConstructFunction*                            _construct = _ConstantField_AssignFromXML;
   Stg_Component_BuildFunction*                                    _build = _ConstantField_Build;
   Stg_Component_InitialiseFunction*                          _initialise = _ConstantField_Initialise;
   Stg_Component_ExecuteFunction*                                _execute = _FieldVariable_Execute;
   Stg_Component_DestroyFunction*                                _destroy = _ConstantField_Destroy;
   AllocationType                                      nameAllocationType = NON_GLOBAL;
   FieldVariable_InterpolateValueAtFunction*          _interpolateValueAt = _ConstantField_InterpolateValueAt;
   FieldVariable_GetValueFunction*            _getMinGlobalFieldMagnitude = _ConstantField_GetMinGlobalFieldMagnitude;
   FieldVariable_GetValueFunction*            _getMaxGlobalFieldMagnitude = _ConstantField_GetMaxGlobalFieldMagnitude;
	FieldVariable_CacheValuesFunction*    _cacheMinMaxGlobalFieldMagnitude = _ConstantField_CacheMinMaxGlobalFieldMagnitude;
   FieldVariable_GetCoordFunction*               _getMinAndMaxLocalCoords = _ConstantField_GetMinAndMaxLocalCoords;
   FieldVariable_GetCoordFunction*              _getMinAndMaxGlobalCoords = _ConstantField_GetMinAndMaxGlobalCoords;
	FeVariable_InterpolateWithinElementFunction*    _interpolateWithinElement = _ConstantField_InterpolateWithinElement;
	FeVariable_GetValueAtNodeFunction*                        _getValueAtNode = _ConstantField_GetValueAtNode;
	FeVariable_SyncShadowValuesFunc*                        _syncShadowValues = _ConstantField_SyncShadowValues;

   return _ConstantField_New(  CONSTANTFIELD_PASSARGS  );
}

ConstantField* _ConstantField_New(  CONSTANTFIELD_DEFARGS  ) {
   ConstantField* self;

   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(ConstantField) );
   self = (ConstantField*)_FeVariable_New(  FEVARIABLE_PASSARGS  );

   /* Virtual functions */

   /* General info */

   /* ConstantField info */

   return self;
}

void _ConstantField_Init(
   ConstantField* self,
   Index          fieldComponentCount,
   double*        value,
   FeMesh* feMesh) 
{
   Index ii;
   double valueSum;

   self->feMesh              = feMesh;
   self->fieldComponentCount = fieldComponentCount;
   
   self->value = Memory_Alloc_Array( double, self->fieldComponentCount, "constantFieldValues" );
   valueSum = 0;
   for (ii = 0; ii < self->fieldComponentCount; ii++){
      self->value[ii]  = value[ii];
      valueSum        += self->value[ii]*self->value[ii]; 
   }
   valueSum /= self->fieldComponentCount;
   valueSum = sqrt( valueSum );
   
   self->maxFieldValue = valueSum;
   self->minFieldValue = valueSum;
   
}

void _ConstantField_AssignFromXML( void* _ConstantField, Stg_ComponentFactory* cf, void* data ) {
   ConstantField*          self = (ConstantField*)_ConstantField;
   Dictionary_Entry_Value* componentValueList = NULL;
   FeMesh*                 feMesh = NULL;
   Index                   ii;
   double*                 value;
   Index                   fieldComponentCount;

   _FieldVariable_AssignFromXML( self, cf, data );
   /** this is the list of values for the constant field*/
   /** for a scalar field, just have one value in the list */
   /** for vector fields, add as many components as required */
   componentValueList  = Dictionary_Get( Dictionary_GetDictionary( cf->componentDict, self->name ), (Dictionary_Entry_Key)"componentValueList" );
   fieldComponentCount = Dictionary_Entry_Value_GetCount( componentValueList );
   value = Memory_Alloc_Array( double, fieldComponentCount, "constantFieldValues" );
   for (ii = 0; ii < Dictionary_Entry_Value_GetCount( componentValueList ); ii++)
      value[ii]  = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetElement( componentValueList, ii ) );
   /** this feMesh is sometimes required for objects that believe this is an FeVariable */
	feMesh             = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"FEMesh", FeMesh, False, data  );

   _ConstantField_Init( self, fieldComponentCount, value, feMesh );

   Memory_Free(value);
}

void _ConstantField_Build( void* _ConstantField, void* data ) {
   ConstantField* self = (ConstantField*)_ConstantField;
   
   _FieldVariable_Build( self, data );

}

void _ConstantField_Initialise( void* _ConstantField, void* data ) {
   ConstantField* self = (ConstantField*)_ConstantField;
   
   _FieldVariable_Initialise( self, data );

}

void _ConstantField_Execute( void* ConstantField, void* data ) {

}

void _ConstantField_Destroy( void* _ConstantField, void* data ) {
   ConstantField* self = (ConstantField*)_ConstantField;

   _FieldVariable_Destroy( self, data );
   
}

InterpolationResult _ConstantField_InterpolateValueAt( void* _ConstantField, double* coord, double* value ){
   ConstantField* self = (ConstantField*)_ConstantField;   
	
	memcpy( value, self->value, self->fieldComponentCount*sizeof(double) );
	
   return LOCAL;
}


double _ConstantField_GetMinGlobalFieldMagnitude( void* _ConstantField ){
   ConstantField* self = (ConstantField*)_ConstantField;   

   return self->minFieldValue;
}

double _ConstantField_GetMaxGlobalFieldMagnitude( void* _ConstantField ){
   ConstantField* self = (ConstantField*)_ConstantField;   

   return self->maxFieldValue;
}

void _ConstantField_GetMinAndMaxLocalCoords( void* ConstantField, double* min, double* max ) {}
void _ConstantField_GetMinAndMaxGlobalCoords( void* ConstantField, double* min, double* max ) {}
void _ConstantField_CacheMinMaxGlobalFieldMagnitude( void* ConstantField, double* min, double* max ) {}

void _ConstantField_InterpolateWithinElement( void* _ConstantField, Element_DomainIndex dElement_I, Coord coord, double* value ) {
	
	_ConstantField_InterpolateValueAt( _ConstantField, NULL, value );
}

void _ConstantField_GetValueAtNode( void* _ConstantField, Node_DomainIndex dNode_I, double* value ) {
	
	_ConstantField_InterpolateValueAt( _ConstantField, NULL, value );
}

void _ConstantField_SyncShadowValues( void* _ConstantField ) {}

