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
#include "AnalyticFunction.h"
#include "AnalyticFunction_Register.h"

#include <stdio.h>
#include <assert.h>


const Type AnalyticFunction_Type = "AnalyticFunction";

/*--------------------------------------------------------------------------------------------------------------------------
** Constructor
*/

AnalyticFunction* AnalyticFunction_New( AnalyticFunction_SolFunc* apply, Name name ) {
   /* Variables set in this function */
   SizeT                     _sizeOfSelf = sizeof(AnalyticFunction);
   Type                             type = AnalyticFunction_Type;
   Stg_Class_DeleteFunction*     _delete = _AnalyticFunction_Delete;
   Stg_Class_PrintFunction*       _print = _AnalyticFunction_Print;
   Stg_Class_CopyFunction*         _copy = NULL;

   return _AnalyticFunction_New( ANALYTICFUNCTION_PASSARGS );
}

void AnalyticFunction_Init( AnalyticFunction* self, AnalyticFunction_SolFunc* apply, Name name ) {
   /* General info */
   self->type = AnalyticFunction_Type;
   self->_sizeOfSelf = sizeof(AnalyticFunction);
   self->_deleteSelf = False;
   
   /* Virtual info */
   self->_delete = _AnalyticFunction_Delete;
   self->_print = _AnalyticFunction_Print;
   self->_copy = NULL;
   
   _Stg_Class_Init((Stg_Class*)self);
   
   /* Stg_Class info */
   _AnalyticFunction_Init( self, apply, name );
}

AnalyticFunction* _AnalyticFunction_New( ANALYTICFUNCTION_DEFARGS ) {
   AnalyticFunction* self;
   
   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(AnalyticFunction) );
   self = (AnalyticFunction*)_Stg_Class_New( STG_CLASS_PASSARGS );
   
   /* General info */
   
   /* Virtual functions */
   
   /* Stg_Class info */
   _AnalyticFunction_Init(self, apply, name);
   
   return self;
}

void _AnalyticFunction_Init( void* analyticFunction, AnalyticFunction_SolFunc* apply, Name name ) {
   AnalyticFunction* self = (AnalyticFunction*)analyticFunction;
   
   self->apply = apply;
   self->name = name;
}

/*--------------------------------------------------------------------------------------------------------------------------
** General virtual functions
*/

void _AnalyticFunction_Delete( void* analyticFunction ) {
   AnalyticFunction* self = (AnalyticFunction*)analyticFunction;
   
   /* Stg_Class_Delete parent class */
   _Stg_Class_Delete(self);
}

void _AnalyticFunction_Print( void* analyticFunction, Stream* stream ) {
   AnalyticFunction* self = (AnalyticFunction*)analyticFunction;
   
   /* Set the Journal for printing informations */
   Stream* analyticFunctionStream = stream;
   
   /* General info */
   Journal_Printf( analyticFunctionStream, "AnalyticFunction (ptr): %p\n", self );
   
   /* Virtual info */
   
   /* Stg_Class info */
   Journal_Printf( analyticFunctionStream, "\tapply (func ptr): %p\n", self->apply );
   Journal_Printf( analyticFunctionStream, "\tname (ptr): %p\n", self->name );

   if(self->name)
      Journal_Printf( analyticFunctionStream, "\t\tname: %s\n", self->name );
   
   /* Print parent class */
   _Stg_Class_Print( self, analyticFunctionStream );
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

void _AnalyticFunction_Apply( void* analyticFunction, double* coord, double* value ) {
   AnalyticFunction* self = (AnalyticFunction*)analyticFunction;
   
   AnalyticFunction_Apply( self, coord, value );
}

void AnalyticFunction_Add( void* analyticFunction, Name functionName ) {
   /* Add this plugin's analytic functions into the register. */
   AnalyticFunction* analyticFunc = AnalyticFunction_New( (AnalyticFunction_SolFunc*)analyticFunction, functionName );
   AnalyticFunction_Register_Add( analyticFunc_Register, analyticFunc );
}
