/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University 
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 		* Redistributions of source code must retain the above copyright notice, 
** 			this list of conditions and the following disclaimer.
** 		* Redistributions in binary form must reproduce the above copyright 
**			notice, this list of conditions and the following disclaimer in the 
**			documentation and/or other materials provided with the distribution.
** 		* Neither the name of the Monash University nor the names of its contributors 
**			may be used to endorse or promote products derived from this software 
**			without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
** THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
** PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS 
** BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
** OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**
** Contact:
*%  Louis.Moresi - Louis.Moresi@monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
** 
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include "types.h"
#include "ConstitutiveMatrix.h"
#include "RheologyClass.h"

#include <assert.h>

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type Rheology_Type = "Rheology";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Rheology* _Rheology_New(  RHEOLOGY_DEFARGS  ) 
{
	Rheology*					self;

	/* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
	assert( _sizeOfSelf >= sizeof(Rheology) );
	/* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
	/* This means that any values of these parameters that are passed into this function are not passed onto the parent function
	   and so should be set to ZERO in any children of this class. */
	nameAllocationType = NON_GLOBAL;

	self = (Rheology*) _Stg_Component_New(  STG_COMPONENT_PASSARGS  );
	
	/* Function pointers for this class that are not on the parent class should be set here */
	self->_modifyConstitutiveMatrix = _modifyConstitutiveMatrix;
	
	return self;
}

void _Rheology_Init( void* rheology, PICelleratorContext* context )
{
	Rheology* self = (Rheology*)rheology;

	self->context = context;
	self->debug = Journal_Register( Debug_Type, (Name)self->type ); /* TODO make child of Underworld_Debug */
}

void _Rheology_Delete( void* rheology ) {
	Rheology*					self = (Rheology*)rheology;

	_Stg_Component_Delete( self  );
}

void _Rheology_Print( void* rheology, Stream* stream ) {}

void* _Rheology_Copy( void* rheology, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
	Rheology*	self = (Rheology*)rheology;

	/* TODO */ abort();
	return (void*) self;
}

void _Rheology_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data ){
   Rheology*            self = (Rheology*)rheology;
   PICelleratorContext* context;

   context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", PICelleratorContext, False, data );
   if( !context  ) 
      context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", PICelleratorContext, True, data  );

   /* The PpcManager */
   self->mgr = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Manager", PpcManager, False, data );
   if( !self->mgr )
      self->mgr = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_ppcManager", PpcManager, True, data  );

   _Rheology_Init( self, context );
}

void _Rheology_Build( void* rheology, void* data ) {}
void _Rheology_Initialise( void* rheology, void* data ) {}
void _Rheology_Execute( void* rheology, void* data ) {}
void _Rheology_Destroy( void* rheology, void* data ) {}



