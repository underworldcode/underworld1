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
*%  Louis Moresi - Louis.Moresi(at)monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include "types.h"
#include "JAMBoxExampleComponent.h"

const Type JAMBoxExampleComponent_Type = "JAMBoxExampleComponent";

JAMBoxExampleComponent* JAMBoxExampleComponent_New(
   Name                    name,
   Bool                    someParameter)
{
   JAMBoxExampleComponent* self = (JAMBoxExampleComponent*) _JAMBoxExampleComponent_DefaultNew( name );

   self->isConstructed = True;
   _JAMBoxExampleComponent_Init( self, someParameter );

   return self;
}

void* _JAMBoxExampleComponent_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof( JAMBoxExampleComponent );
   Type                                                      type = JAMBoxExampleComponent_Type;
   Stg_Class_DeleteFunction*                              _delete = _JAMBoxExampleComponent_Delete;
   Stg_Class_PrintFunction*                                _print = _Stg_Component_Print;
   Stg_Class_CopyFunction*                                  _copy = _Stg_Component_Copy;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _JAMBoxExampleComponent_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _JAMBoxExampleComponent_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _JAMBoxExampleComponent_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _JAMBoxExampleComponent_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _JAMBoxExampleComponent_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _JAMBoxExampleComponent_Destroy;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return _JAMBoxExampleComponent_New(  JAMBoxEXAMPLECOMPONENT_PASSARGS  );
}

JAMBoxExampleComponent* _JAMBoxExampleComponent_New(  JAMBoxEXAMPLECOMPONENT_DEFARGS  )
{
   JAMBoxExampleComponent* self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(JAMBoxExampleComponent) );
   /* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
   /* This means that any values of these parameters that are passed into this function are not passed onto the parent function
      and so should be set to ZERO in any children of this class. */
   nameAllocationType = NON_GLOBAL;

   self = (JAMBoxExampleComponent*) _Stg_Component_New(  STG_COMPONENT_PASSARGS  );

   return self;
}

void _JAMBoxExampleComponent_Init(
   void*                _self,
   Bool                 someParameter)
{
   JAMBoxExampleComponent* self = _self;

   self->someParameter         = someParameter;
}

void _JAMBoxExampleComponent_Delete( void* _self )
{
   JAMBoxExampleComponent* self = _self;

   _Stg_Component_Delete( self );
}

void _JAMBoxExampleComponent_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data )
{
   JAMBoxExampleComponent*  self = (JAMBoxExampleComponent*) _self;
   Bool                     someParameter;

   self->context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", AbstractContext, False, data );
   if ( !self->context  )
      self->context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", AbstractContext, True, data  );

   /** add the parameters and dependencies here */
   someParameter = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"SomeParameter", True );

   _JAMBoxExampleComponent_Init( self, someParameter );

}

void _JAMBoxExampleComponent_Build( void* _self, void* data ){}
void _JAMBoxExampleComponent_Initialise( void* _self, void* data ){}
void _JAMBoxExampleComponent_Execute( void* _self, void* data ){}
void _JAMBoxExampleComponent_Destroy( void* _self, void* data ){}

