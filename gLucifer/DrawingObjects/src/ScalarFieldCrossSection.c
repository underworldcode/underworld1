/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
**       * Redistributions of source code must retain the above copyright notice,
**          this list of conditions and the following disclaimer.
**       * Redistributions in binary form must reproduce the above copyright
**         notice, this list of conditions and the following disclaimer in the
**         documentation and/or other materials provided with the distribution.
**       * Neither the name of the Monash University nor the names of its contributors
**         may be used to endorse or promote products derived from this software
**         without specific prior written permission.
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
#include <StgFEM/StgFEM.h>

#include <gLucifer/Base/Base.h>


#include "types.h"
#include "CrossSection.h"
#include "ScalarFieldCrossSection.h"

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type lucScalarFieldCrossSection_Type = "lucScalarFieldCrossSection";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
lucScalarFieldCrossSection* _lucScalarFieldCrossSection_New(  LUCSCALARFIELDCROSSSECTION_DEFARGS  )
{
   lucScalarFieldCrossSection*               self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucScalarFieldCrossSection) );
   self = (lucScalarFieldCrossSection*) _lucCrossSection_New(  LUCCROSSSECTION_PASSARGS  );

   return self;
}

void _lucScalarFieldCrossSection_Init(lucScalarFieldCrossSection* self)
{
}

void _lucScalarFieldCrossSection_Delete( void* drawingObject )
{
   lucScalarFieldCrossSection*  self = (lucScalarFieldCrossSection*)drawingObject;

   _lucCrossSection_Delete( self );
}

void _lucScalarFieldCrossSection_Print( void* drawingObject, Stream* stream )
{
   lucScalarFieldCrossSection*  self = (lucScalarFieldCrossSection*)drawingObject;

   _lucCrossSection_Print( self, stream );
}

void* _lucScalarFieldCrossSection_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof(lucScalarFieldCrossSection);
   Type                                                             type = lucScalarFieldCrossSection_Type;
   Stg_Class_DeleteFunction*                                     _delete = _lucScalarFieldCrossSection_Delete;
   Stg_Class_PrintFunction*                                       _print = _lucScalarFieldCrossSection_Print;
   Stg_Class_CopyFunction*                                         _copy = NULL;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _lucScalarFieldCrossSection_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _lucScalarFieldCrossSection_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _lucScalarFieldCrossSection_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _lucScalarFieldCrossSection_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _lucScalarFieldCrossSection_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _lucScalarFieldCrossSection_Destroy;
   lucDrawingObject_SetupFunction*                                _setup = _lucCrossSection_Setup;
   lucDrawingObject_DrawFunction*                                  _draw = _lucScalarFieldCrossSection_Draw;
   lucDrawingObject_CleanUpFunction*                            _cleanUp = lucDrawingObject_CleanUp;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _lucScalarFieldCrossSection_New(  LUCSCALARFIELDCROSSSECTION_PASSARGS  );
}

void _lucScalarFieldCrossSection_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data )
{
   lucScalarFieldCrossSection*     self = (lucScalarFieldCrossSection*)drawingObject;

   /* Construct Parent */
   self->defaultResolution = 100;   /* Default sampling res */
   _lucCrossSection_AssignFromXML( self, cf, data );

   _lucScalarFieldCrossSection_Init(self);
}

void _lucScalarFieldCrossSection_Build( void* drawingObject, void* data )
{
   lucScalarFieldCrossSection*     self        = (lucScalarFieldCrossSection*)drawingObject;

   /* Build field variable in parent */
   _lucCrossSection_Build(self, data);

   Journal_Firewall( self->fieldVariable->fieldComponentCount == 1, lucError,
                     "Error - in %s(): provided FieldVariable \"%s\" has %u components - but %s Component "
                     "can only visualise FieldVariables with 1 component. Did you mean to visualise the "
                     "magnitude of the given field?\n", __func__, self->fieldVariable->name,
                     self->fieldVariable->fieldComponentCount, self->type );
}

void _lucScalarFieldCrossSection_Initialise( void* drawingObject, void* data ) {}
void _lucScalarFieldCrossSection_Execute( void* drawingObject, void* data ) {}
void _lucScalarFieldCrossSection_Destroy( void* drawingObject, void* data ) {}

void _lucScalarFieldCrossSection_Draw( void* drawingObject, lucDatabase* database, void* _context )
{
   lucScalarFieldCrossSection*       self            = (lucScalarFieldCrossSection*)drawingObject;
   lucScalarFieldCrossSection_DrawCrossSection( self, database, False );
}

void lucScalarFieldCrossSection_DrawCrossSection( void* drawingObject, lucDatabase* database, Bool backFacing)
{
   lucScalarFieldCrossSection* self = (lucScalarFieldCrossSection*)drawingObject;

   /* Sample the 2d cross-section */
   lucCrossSection_SampleField(self, backFacing);

   /* Calibrate Colour Map */
   if (self->colourMap)
      lucColourMap_CalibrateFromFieldVariable(self->colourMap, self->fieldVariable);

   if (self->context->rank == 0)
   {
      int d;
      int count = self->resolutionA * self->resolutionB;
      lucDatabase_AddGridVertices(database, count, self->resolutionB, &self->vertices[0][0][0]);
      lucDatabase_AddValues(database, count, lucGridType, lucColourValueData, self->colourMap, &self->values[0][0][0]);

      /* Flip normal if back facing */
      if (backFacing)
         for (d=0; d<3; d++)
            self->normal[d] = 0 - self->normal[d];

      /* Add a single normal value */
      lucDatabase_AddNormal(database, lucGridType, self->normal);
   }

   /* Free memory */
   lucCrossSection_FreeSampleData(self);

   /* Start new geometry section - when used with multiple sections */
   lucDatabase_OutputGeometry(database, self->id);
}

