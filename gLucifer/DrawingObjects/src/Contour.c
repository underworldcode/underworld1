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

#include <gLucifer/Base/Base.h>


#include "types.h"
#include "Contour.h"

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type lucContour_Type = "lucContour";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
lucContour* _lucContour_New(  LUCCONTOUR_DEFARGS  )
{
   lucContour*					self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucContour) );
   self = (lucContour*) _lucContourCrossSection_New(  LUCCONTOURCROSSSECTION_PASSARGS  );

   return self;
}

void _lucContour_Init(lucContour*      self)
{
}

void _lucContour_Delete( void* drawingObject )
{
   lucContour*  self = (lucContour*)drawingObject;

   _lucContourCrossSection_Delete( self );
}

void _lucContour_Print( void* drawingObject, Stream* stream )
{
   lucContour*  self = (lucContour*)drawingObject;

   _lucContourCrossSection_Print( self, stream );
}

void* _lucContour_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof(lucContour);
   Type                                                             type = lucContour_Type;
   Stg_Class_DeleteFunction*                                     _delete = _lucContour_Delete;
   Stg_Class_PrintFunction*                                       _print = _lucContour_Print;
   Stg_Class_CopyFunction*                                         _copy = NULL;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _lucContour_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _lucContour_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _lucContourCrossSection_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _lucContourCrossSection_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _lucContourCrossSection_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _lucContourCrossSection_Destroy;
   lucDrawingObject_SetupFunction*                                _setup = _lucContourCrossSection_Setup;
   lucDrawingObject_DrawFunction*                                  _draw = _lucContour_Draw;
   lucDrawingObject_CleanUpFunction*                            _cleanUp = _lucContourCrossSection_CleanUp;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _lucContour_New(  LUCCONTOUR_PASSARGS  );
}

void _lucContour_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data )
{
   lucContour*      self = (lucContour*)drawingObject;
   /* Construct Parent */
   _lucContourCrossSection_AssignFromXML( self, cf, data );
   _lucContour_Init(self);
}

void _lucContour_Draw( void* drawingObject, lucDatabase* database, void* _context )
{
   lucContour*            self            = (lucContour*)drawingObject;
   Dimension_Index        dim             = self->fieldVariable->dim;

   if ( dim == 2 )
   {
      _lucContourCrossSection_Draw(drawingObject, database, _context);
   }
   else
   {
      self->coordIndex = 0;
      self->printedIndex = -1;
      lucContourCrossSection_DrawCrossSection( lucCrossSection_Set(self, 0.0, I_AXIS, True), database );
      lucContourCrossSection_DrawCrossSection( lucCrossSection_Set(self, 1.0, I_AXIS, True), database );
      lucContourCrossSection_DrawCrossSection( lucCrossSection_Set(self, 0.0, J_AXIS, True), database );
      lucContourCrossSection_DrawCrossSection( lucCrossSection_Set(self, 1.0, J_AXIS, True), database );
      lucContourCrossSection_DrawCrossSection( lucCrossSection_Set(self, 0.0, K_AXIS, True), database );
      lucContourCrossSection_DrawCrossSection( lucCrossSection_Set(self, 1.0, K_AXIS, True), database );
   }
}


