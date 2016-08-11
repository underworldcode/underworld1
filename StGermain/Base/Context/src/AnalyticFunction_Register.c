/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C ), 2003, Victorian Partnership for Advanced Computing (VPAC ) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org )
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org )
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org )
**   Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org )
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org )
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org )
**
**  This library is free software; you can redistribute it and/or
**  modify it under the terms of the GNU Lesser General Public
**  License as published by the Free Software Foundation; either
**  version 2.1 of the License, or (at your option ) any later version.
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
#include <string.h>
#include <assert.h>


const Type AnalyticFunction_Register_Type = "AnalyticFunction_Register";

AnalyticFunction_Register* analyticFunc_Register = 0;

/*--------------------------------------------------------------------------------------------------------------------------
** Constructor
*/

AnalyticFunction_Register* AnalyticFunction_Register_New( void ) {
   /* Variables set in this function */
   SizeT                     _sizeOfSelf = sizeof( AnalyticFunction_Register );
   Type                             type = AnalyticFunction_Register_Type;
   Stg_Class_DeleteFunction*     _delete = _AnalyticFunction_Register_Delete;
   Stg_Class_PrintFunction*       _print = _AnalyticFunction_Register_Print;
   Stg_Class_CopyFunction*         _copy = NULL;

   return _AnalyticFunction_Register_New( ANALYTICFUNCTION_REGISTER_PASSARGS );
}

void AnalyticFunction_Register_Init( AnalyticFunction_Register* self ) { 
   /* General info */
   self->type = AnalyticFunction_Register_Type;
   self->_sizeOfSelf = sizeof( AnalyticFunction_Register );
   self->_deleteSelf = False;
   
   /* Virtual info */
   self->_delete = _AnalyticFunction_Register_Delete;
   self->_print = _AnalyticFunction_Register_Print;
   self->_copy = NULL;
   
   _Stg_Class_Init((Stg_Class*)self );
   
   /* Stg_Class info */
   _AnalyticFunction_Register_Init( self );
}

AnalyticFunction_Register* _AnalyticFunction_Register_New( ANALYTICFUNCTION_REGISTER_DEFARGS ) {
   AnalyticFunction_Register* self;
   
   /* Allocate memory */
   self = (AnalyticFunction_Register*)_Stg_Class_New( STG_CLASS_PASSARGS );
   
   /* Virtual info */
   
   /* AnalyticFunction_Register info */
   _AnalyticFunction_Register_Init( self );
   
   return self;
}

void _AnalyticFunction_Register_Init( void* analyticFunction_Register ) { 
   AnalyticFunction_Register* self = (AnalyticFunction_Register*)analyticFunction_Register;
   
   self->count = 0;
   self->_size = 8;
   self->_delta = 8;
   self->_af = Memory_Alloc_Array( AnalyticFunction*, self->_size, "AnalyticFunction_Register->_af" );
   memset( self->_af, 0, sizeof( AnalyticFunction*)*self->_size );
}

/*--------------------------------------------------------------------------------------------------------------------------
** General virtual functions
*/

void _AnalyticFunction_Register_Delete( void* analyticFunction_Register ) {
   AnalyticFunction_Register* self = (AnalyticFunction_Register*)analyticFunction_Register;
   
   if( self->_af )
      Memory_Free( self->_af );
      
   /* Stg_Class_Delete parent class */
   _Stg_Class_Delete( self );
}

void _AnalyticFunction_Register_Print( void* analyticFunction_Register, Stream* stream ) {
   AnalyticFunction_Register* self = (AnalyticFunction_Register*)analyticFunction_Register;
   
   /* Set the Journal for printing informations */
   Stream* analyticFunction_RegisterStream = stream;
   
   /* General info */
   Journal_Printf( analyticFunction_RegisterStream, "AnalyticFunction_Register (ptr ): %p\n", self );
   
   /* Virtual info */
   
   /* AnalyticFunction_Register info */
   Journal_Printf( analyticFunction_RegisterStream, "\tcount: %u\n", self->count );
   Journal_Printf( analyticFunction_RegisterStream, "\t_size: %lu\n", self->_size );
   Journal_Printf( analyticFunction_RegisterStream, "\t_delta: %lu\n", self->_delta );
   Journal_Printf( analyticFunction_RegisterStream, "\t_af (ptr ): %p\n", self->_af );

   if( self->_af ) {
      AnalyticFunction_Index af_I;
      
      for( af_I = 0; af_I < self->count; af_I++ )
         Journal_Printf( analyticFunction_RegisterStream, "\t\t_af[%u]: %p\n", af_I, self->_af[af_I]);
   }
   
   /* Parent class info */
   _Stg_Class_Print( self, analyticFunction_RegisterStream );
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

AnalyticFunction_Index AnalyticFunction_Register_Add( void* analyticFunction_Register, AnalyticFunction* cf ) {
   AnalyticFunction_Register* self = (AnalyticFunction_Register*)analyticFunction_Register;
   Variable_Index             handle;

   if( self->count >= self->_size ) {
      SizeT currentSize = self->_size;
      
      /* Note: why is realloc not used here? */
      self->_size += self->_delta;
      
      self->_af = Memory_Realloc_Array( self->_af, AnalyticFunction*, self->_size );
      memset(
         (Pointer )((ArithPointer )self->_af + (sizeof( AnalyticFunction*) * currentSize ) ),
         0,
         sizeof( AnalyticFunction* ) * (self->_size - currentSize ) );
   }
   
   handle = self->count++;
   self->_af[handle] = cf;
   
   return handle;
}

/*--------------------------------------------------------------------------------------------------------------------------
** Functions
*/

AnalyticFunction_Index AnalyticFunction_Register_GetIndex( void* analyticFunction_Register, Name name ) {
   AnalyticFunction_Register* self = (AnalyticFunction_Register*)analyticFunction_Register;
   AnalyticFunction_Index     af_I;
   
   for( af_I = 0; af_I < self->count; af_I++ ) {
      if( self->_af[af_I]->name && !strcmp( name, self->_af[af_I]->name ) )
         return af_I;
   }
   
   return (AnalyticFunction_Index )-1;
}

AnalyticFunction* AnalyticFunction_Register_GetByName( void* analyticFunction_Register, Name name ) {
   AnalyticFunction_Register* self = (AnalyticFunction_Register*)analyticFunction_Register;
   AnalyticFunction_Index     af_I;
   
   for( af_I = 0; af_I < self->count; af_I++ ) {
      if( self->_af[af_I]->name && !strcmp( name, self->_af[af_I]->name ) )
         return self->_af[af_I];
   }
   
   return NULL;
}

void AnalyticFunction_Register_PrintNameOfEachFunc( void* analyticFunction_Register, Stream* stream ) {
   AnalyticFunction_Register* self = (AnalyticFunction_Register*)analyticFunction_Register;
   AnalyticFunction_Index     af_I;
   
   for( af_I = 0; af_I < self->count; af_I++ ) {
      Journal_Printf( stream, "\"%s\", ", self->_af[af_I]->name );
   }
}


