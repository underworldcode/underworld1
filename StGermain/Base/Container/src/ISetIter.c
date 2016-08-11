/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 
110 Victoria Street, Melbourne, 3053, Australia.
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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "StGermain/Base/Foundation/Foundation.h"
#include "types.h"
#include "Iter.h"
#include "ISet.h"
#include "ISetIter.h"


const Type ISetIter_Type = "ISetIter";


ISetIter* ISetIter_New() {
    ISetIter* self;
    SizeT _sizeOfSelf = sizeof(ISetIter);
    Type type = ISetIter_Type;
    Stg_Class_DeleteFunction* _delete = _Iter_Delete;
    Stg_Class_PrintFunction* _print = NULL;
    Stg_Class_CopyFunction* _copy = NULL;
    Iter_NextFunc* nextFunc = ISetIter_Next;

    self = _ISetIter_New( ISETITER_PASSARGS );
    return self;
}

ISetIter* _ISetIter_New( ISETITER_DEFARGS ) {
    ISetIter* self;

    self = (ISetIter*)_Iter_New( ITER_PASSARGS );
    _ISetIter_Init( self );
    return self;
}

void ISetIter_Init( ISetIter* self ) {
    _Stg_Class_Init( (Stg_Class*)self );
    _Iter_Init( (Iter*)self );
    _ISetIter_Init( self );
}

void _ISetIter_Init( void* _self ) {
   ISetIter* self = (ISetIter*)_self;

   self->iset = NULL;
   self->tblInd = 0;
   self->cur = NULL;
}

void ISetIter_Destruct( ISetIter* self ) {
}

void ISetIter_Copy( void* _self, const void* _op ) {
   ISetIter* self = (ISetIter*)_self;
   const ISetIter* op = (const ISetIter*)_op;

   Iter_Copy( self, op );
   self->iset = op->iset;
   self->tblInd = op->tblInd;
   self->cur = op->cur;
}

void ISetIter_Next( void* _self ) {
   ISetIter* self = (ISetIter*)_self;
   int i_i;

   assert( self );
   assert( self->tblInd < self->iset->tblSize && 
	   self->iset->used[self->tblInd] );
   assert( self->cur );
   assert( self->valid );
   if( !self->cur->next ) {
      for( i_i = self->tblInd + 1; i_i < self->iset->tblSize; i_i++ ) {
	 if( self->iset->used[i_i] )
	    break;
      }
      if( i_i < self->iset->tblSize ) {
	self->tblInd = i_i;
	self->cur = self->iset->tbl + i_i;
      }
      else
	self->valid = False;
   }
   else
      self->cur = self->cur->next;
}

int ISetIter_GetKey( const void* self ) {
   assert( self );
   assert( ((ISetIter*)self)->valid && ((ISetIter*)self)->cur );
   return ((ISetIter*)self)->cur->key;
}


