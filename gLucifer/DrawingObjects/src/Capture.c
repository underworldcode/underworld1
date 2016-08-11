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
*%  Owen Kaluza - Owen.Kaluza(at)monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>

#include <gLucifer/Base/Base.h>

#include "types.h"
#include <gLucifer/Base/DrawingObject.h>
#include "Capture.h"

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type lucCapture_Type = "lucCapture";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
lucCapture* _lucCapture_New(  LUCCAPTURE_DEFARGS  )
{
   lucCapture*					self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucCapture) );
   self = (lucCapture*) _lucDrawingObject_New(  LUCDRAWINGOBJECT_PASSARGS  );

   return self;
}

void _lucCapture_Init(lucCapture* self, lucDatabase* database)
{
   self->database = database;
}

void* _lucCapture_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof(lucCapture);
   Type                                                             type = lucCapture_Type;
   Stg_Class_DeleteFunction*                                     _delete = _lucDrawingObject_Delete;
   Stg_Class_PrintFunction*                                       _print = _lucDrawingObject_Print;
   Stg_Class_CopyFunction*                                         _copy = NULL;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _lucCapture_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _lucCapture_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _lucDrawingObject_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _lucDrawingObject_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _lucDrawingObject_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _lucDrawingObject_Destroy;
   lucDrawingObject_SetupFunction*                                _setup = lucDrawingObject_Setup;
   lucDrawingObject_DrawFunction*                                  _draw = _lucCapture_Draw;
   lucDrawingObject_CleanUpFunction*                            _cleanUp = lucDrawingObject_CleanUp;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _lucCapture_New(  LUCCAPTURE_PASSARGS  );
}

void _lucCapture_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data )
{
   lucCapture* self = (lucCapture*)drawingObject;

   /* Construct Parent */
   _lucDrawingObject_AssignFromXML( self, cf, data );

   _lucCapture_Init(self, 
      Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Database", lucDatabase, False, data )
      );
}


void _lucCapture_Draw( void* drawingObject, lucDatabase* database, void* _context )
{
   /* Placeholder */
}
