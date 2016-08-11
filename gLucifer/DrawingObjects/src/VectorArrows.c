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
#include <gLucifer/Base/DrawingObject.h>
#include "VectorArrowCrossSection.h"
#include "VectorArrows.h"

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type lucVectorArrows_Type = "lucVectorArrows";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
lucVectorArrows* _lucVectorArrows_New(  LUCVECTORARROWS_DEFARGS  )
{
   lucVectorArrows*					self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucVectorArrows) );
   self = (lucVectorArrows*) _lucVectorArrowCrossSection_New(  LUCVECTORARROWCROSSSECTION_PASSARGS  );

   return self;
}

void _lucVectorArrows_Init( lucVectorArrows*                                             self,
                            IJK                                                          resolution)
{
   memcpy( self->resolution, resolution, sizeof(IJK) );
   /* Set cross section resolutions to the X & Y resolution */
   self->resolutionA = resolution[I_AXIS];
   self->resolutionB = resolution[J_AXIS];
}

void _lucVectorArrows_Delete( void* drawingObject )
{
   lucVectorArrows*  self = (lucVectorArrows*)drawingObject;

   _lucVectorArrowCrossSection_Delete( self );
}

void _lucVectorArrows_Print( void* drawingObject, Stream* stream )
{
   lucVectorArrows*  self = (lucVectorArrows*)drawingObject;

   _lucVectorArrowCrossSection_Print( self, stream );
}

void* _lucVectorArrows_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof(lucVectorArrows);
   Type                                                             type = lucVectorArrows_Type;
   Stg_Class_DeleteFunction*                                     _delete = _lucVectorArrows_Delete;
   Stg_Class_PrintFunction*                                       _print = _lucVectorArrows_Print;
   Stg_Class_CopyFunction*                                         _copy = NULL;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _lucVectorArrows_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _lucVectorArrows_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _lucVectorArrowCrossSection_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _lucVectorArrowCrossSection_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _lucVectorArrowCrossSection_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _lucVectorArrowCrossSection_Destroy;
   lucDrawingObject_SetupFunction*                                _setup = _lucCrossSection_Setup;
   lucDrawingObject_DrawFunction*                                  _draw = _lucVectorArrows_Draw;
   lucDrawingObject_CleanUpFunction*                            _cleanUp = lucDrawingObject_CleanUp;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _lucVectorArrows_New(  LUCVECTORARROWS_PASSARGS  );
}

void _lucVectorArrows_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data )
{
   lucVectorArrows* self = (lucVectorArrows*)drawingObject;
   IJK resolution;

   /* Construct Parent */
   self->defaultResolution = 8;
   _lucVectorArrowCrossSection_AssignFromXML( self, cf, data );

   resolution[ I_AXIS ] = Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"resolutionX", self->defaultResolution  );
   resolution[ J_AXIS ] = Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"resolutionY", self->defaultResolution  );
   resolution[ K_AXIS ] = Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"resolutionZ", self->defaultResolution  );

   _lucVectorArrows_Init( self, resolution );
}

void _lucVectorArrows_Draw( void* drawingObject, lucDatabase* database, void* _context )
{
   lucVectorArrows*       self    = (lucVectorArrows*)drawingObject;
   DomainContext* context         = (DomainContext*) _context;
   Dimension_Index        dim     = context->dim;

   if ( dim == 2 )
   {
      _lucVectorArrowCrossSection_DrawCrossSection( lucCrossSection_Set(self, 0.0, K_AXIS, False), database, dim);
   }
   else
   {
      int idx;
      for ( idx=0; idx <= self->resolution[K_AXIS]; idx++)
      {
         _lucVectorArrowCrossSection_DrawCrossSection( lucCrossSection_Set(self, idx / (double)self->resolution[ K_AXIS ], K_AXIS, True), database, dim);
      }
   }
}


