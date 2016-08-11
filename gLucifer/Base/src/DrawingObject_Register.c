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
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>

#include "Base.h"
#include "types.h"
#include "Viewport.h"
#include "Viewport.h"
#include "ColourMap.h"
#include "Window.h"
#include "DrawingObject_Register.h"
#include "DrawingObject.h"

#include "Database.h"

const Type lucDrawingObject_Register_Type = "lucDrawingObject_Register";


lucDrawingObject_Register*	lucDrawingObject_Register_New( void )
{
   /* Variables set in this function */
   SizeT                      _sizeOfSelf = sizeof(lucDrawingObject_Register);
   Type                              type = lucDrawingObject_Register_Type;
   Stg_Class_DeleteFunction*      _delete = _NamedObject_Register_Delete;
   Stg_Class_PrintFunction*        _print = _NamedObject_Register_Print;
   Stg_Class_CopyFunction*          _copy = _NamedObject_Register_Copy;

   lucDrawingObject_Register* self;

   self = (lucDrawingObject_Register*) _NamedObject_Register_New(  NAMEDOBJECT_REGISTER_PASSARGS  );

   return self;
}

void lucDrawingObject_Register_SetupAll( void* drawingObject_Register, lucDatabase* database )
{
   lucDrawingObject_Register* self           = (lucDrawingObject_Register*) drawingObject_Register;
   DrawingObject_Index        object_I;
   DrawingObject_Index        objectCount    = lucDrawingObject_Register_GetCount( self );
   lucDrawingObject*          object;

   for ( object_I = 0 ; object_I < objectCount ; object_I++ )
   {
      object = lucDrawingObject_Register_GetByIndex( self, object_I );

      /* Ensure setup has been called to prepare for rendering (used to set position information) */
      if ( !object->disabled)
      {
         object->_setup( object, database, database->context );
         object->needsToCleanUp = True;
         object->needsToDraw = True;
      }
   }
}

void lucDrawingObject_Register_DrawAll( void* drawingObject_Register, lucDatabase* database)
{
   lucDrawingObject_Register* self          = (lucDrawingObject_Register*) drawingObject_Register;
   DrawingObject_Index        object_I;
   DrawingObject_Index        objectCount   = lucDrawingObject_Register_GetCount( self );
   lucDrawingObject*          object;

   for ( object_I = 0 ; object_I < objectCount ; object_I++ )
   {
      object = lucDrawingObject_Register_GetByIndex( self, object_I );
      lucDrawingObject_Draw( object, database, database->context );
      lucDatabase_OutputGeometry(database, object->id);
   }
}

void lucDrawingObject_Register_CleanUpAll( void* drawingObject_Register, void* context )
{
   lucDrawingObject_Register* self          = (lucDrawingObject_Register*) drawingObject_Register;
   DrawingObject_Index        object_I;
   DrawingObject_Index        objectCount   = lucDrawingObject_Register_GetCount( self );
   lucDrawingObject*          object;

   for ( object_I = 0 ; object_I < objectCount ; object_I++ )
   {
      object = lucDrawingObject_Register_GetByIndex( self, object_I );
      if (object->needsToCleanUp )
         object->_cleanUp( object, context );
   }
}

