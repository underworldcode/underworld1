/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
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

#include <stdarg.h>
#include "Base/Foundation/Foundation.h"
#include "Base/IO/IO.h"
#include "Base/Container/Container.h"

#include "types.h"
#include "ConditionFunction.h"

#include <stdio.h>
#include <assert.h>


const Type ConditionFunction_Type = "ConditionFunction";


/*--------------------------------------------------------------------------------------------------------------------------
** Constructor
*/

ConditionFunction* ConditionFunction_New( ConditionFunction_ApplyFunc* apply, Name name, void* data ) {
   /* Variables set in this function */
   SizeT                     _sizeOfSelf = sizeof(ConditionFunction);
   Type                             type = ConditionFunction_Type;
   Stg_Class_DeleteFunction*     _delete = _ConditionFunction_Delete;
   Stg_Class_PrintFunction*       _print = _ConditionFunction_Print;
   Stg_Class_CopyFunction*         _copy = NULL;
   ConditionFunction_InitFunc*      init = NULL;

   return _ConditionFunction_New( CONDITIONFUNCTION_PASSARGS );
}

ConditionFunction* ConditionFunction_New2( ConditionFunction_ApplyFunc* apply, ConditionFunction_InitFunc* init, Name name, void* data ) {
   /* Variables set in this function */
   SizeT                     _sizeOfSelf = sizeof(ConditionFunction);
   Type                             type = ConditionFunction_Type;
   Stg_Class_DeleteFunction*     _delete = _ConditionFunction_Delete;
   Stg_Class_PrintFunction*       _print = _ConditionFunction_Print;
   Stg_Class_CopyFunction*         _copy = NULL;

   return _ConditionFunction_New( CONDITIONFUNCTION_PASSARGS );
}

ConditionFunction* _ConditionFunction_New( CONDITIONFUNCTION_DEFARGS ) {
   ConditionFunction* self;
   
   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(ConditionFunction) );
   self = (ConditionFunction*)_Stg_Class_New( STG_CLASS_PASSARGS );

   self->data = NULL;
   
   /* General info */
   
   /* Virtual functions */
   
   /* Stg_Class info */
   _ConditionFunction_Init(self, apply, init, name, data);
   
   return self;
}

void _ConditionFunction_Init( void* conditionFunction, ConditionFunction_ApplyFunc* apply, ConditionFunction_InitFunc* init, Name name, void* data) {
   ConditionFunction* self = (ConditionFunction*)conditionFunction;
   
   self->apply = apply;
   self->init  = init;
   self->name  = name;
   self->data  = data;
}

/*--------------------------------------------------------------------------------------------------------------------------
** General virtual functions
*/

void _ConditionFunction_Delete( void* conditionFunction ) {
   ConditionFunction* self = (ConditionFunction*)conditionFunction;
   
   /* Stg_Class_Delete parent class */
   _Stg_Class_Delete(self);
}

void _ConditionFunction_Print( void* conditionFunction, Stream* stream ) {
   ConditionFunction* self = (ConditionFunction*)conditionFunction;
   
   /* Set the Journal for printing informations */
   Stream* conditionFunctionStream = stream;
   
   /* General info */
   Journal_Printf( conditionFunctionStream, "ConditionFunction (ptr): %p\n", self );
   
   /* Virtual info */
   
   /* Stg_Class info */
   Journal_Printf( conditionFunctionStream, "\tapply (func ptr): %p\n", self->apply );
   Journal_Printf( conditionFunctionStream, "\tname (ptr): %p\n", self->name );

   if (self->name)
      Journal_Printf( conditionFunctionStream, "\t\tname: %s\n", self->name );
   
   /* Print parent class */
   _Stg_Class_Print( self, conditionFunctionStream );
}

/*--------------------------------------------------------------------------------------------------------------------------
** Macros
*/


/*--------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/


/*--------------------------------------------------------------------------------------------------------------------------
** Build functions
*/


/*--------------------------------------------------------------------------------------------------------------------------
** Functions
*/

void _ConditionFunction_Apply(
   void*          conditionFunction,
   Index          index,
   Variable_Index var_I,
   void*          context,
   void*          result )
{
   ConditionFunction* self = (ConditionFunction*)conditionFunction;
   
   ConditionFunction_Apply( self, index, var_I, context, result );
}


