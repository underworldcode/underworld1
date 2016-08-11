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
#include <StgFEM/StgFEM.h>

#include <gLucifer/Base/Base.h>


#include "types.h"
#include "CrossSection.h"
#include "VectorArrowCrossSection.h"

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type lucVectorArrowCrossSection_Type = "lucVectorArrowCrossSection";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
lucVectorArrowCrossSection* _lucVectorArrowCrossSection_New(  LUCVECTORARROWCROSSSECTION_DEFARGS  )
{
   lucVectorArrowCrossSection*					self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucVectorArrowCrossSection) );
   self = (lucVectorArrowCrossSection*) _lucCrossSection_New(  LUCCROSSSECTION_PASSARGS  );

   return self;
}

void _lucVectorArrowCrossSection_Init(
   lucVectorArrowCrossSection*      self,
   double                           arrowHeadSize,
   double                           maximum,
   Bool                             dynamicRange,
   double                           lengthScale,
   int                              glyphs)
{
   self->arrowHead = arrowHeadSize;
   self->maximum = maximum;
   self->dynamicRange = dynamicRange;
   self->scaling = lengthScale;
   self->glyphs = glyphs;

   /* Get the sampler to move away from boundaries */
   self->offsetEdges = True;

   /* Append to property string */
   lucDrawingObject_AppendProps(self, "glyphs=%d\nscaling=%g\narrowhead=%g\n", glyphs, lengthScale, arrowHeadSize); 
}

void _lucVectorArrowCrossSection_Delete( void* drawingObject )
{
   lucVectorArrowCrossSection*  self = (lucVectorArrowCrossSection*)drawingObject;
   _lucCrossSection_Delete( self );
}

void _lucVectorArrowCrossSection_Print( void* drawingObject, Stream* stream )
{
   lucVectorArrowCrossSection*  self = (lucVectorArrowCrossSection*)drawingObject;

   _lucCrossSection_Print( self, stream );
}

void* _lucVectorArrowCrossSection_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof(lucVectorArrowCrossSection);
   Type                                                             type = lucVectorArrowCrossSection_Type;
   Stg_Class_DeleteFunction*                                     _delete = _lucVectorArrowCrossSection_Delete;
   Stg_Class_PrintFunction*                                       _print = _lucVectorArrowCrossSection_Print;
   Stg_Class_CopyFunction*                                         _copy = NULL;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _lucVectorArrowCrossSection_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _lucVectorArrowCrossSection_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _lucVectorArrowCrossSection_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _lucVectorArrowCrossSection_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _lucVectorArrowCrossSection_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _lucVectorArrowCrossSection_Destroy;
   lucDrawingObject_SetupFunction*                                _setup = _lucCrossSection_Setup;
   lucDrawingObject_DrawFunction*                                  _draw = _lucVectorArrowCrossSection_Draw;
   lucDrawingObject_CleanUpFunction*                            _cleanUp = lucDrawingObject_CleanUp;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _lucVectorArrowCrossSection_New(  LUCVECTORARROWCROSSSECTION_PASSARGS  );
}

void _lucVectorArrowCrossSection_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data )
{
   lucVectorArrowCrossSection* self = (lucVectorArrowCrossSection*)drawingObject;

   /* Construct Parent */
   self->defaultResolution = 8;  /* Default sampling res */
   _lucCrossSection_AssignFromXML( self, cf, data );
   self->gatherData = False;     /* Drawn in parallel */
   strcpy(self->fieldVariableName, "VectorVariable");

   _lucVectorArrowCrossSection_Init(
      self,
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"arrowHeadSize", 0.3  ),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"maximum", 1.0  ),
      Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"dynamicRange", True  ),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"lengthScale", 0.3  ),
      Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"glyphs", 3));
}

void _lucVectorArrowCrossSection_Build( void* drawingObject, void* data )
{
   /* Build field variable in parent */
   _lucCrossSection_Build(drawingObject, data);
}

void _lucVectorArrowCrossSection_Initialise( void* drawingObject, void* data ) {}
void _lucVectorArrowCrossSection_Execute( void* drawingObject, void* data ) {}
void _lucVectorArrowCrossSection_Destroy( void* drawingObject, void* data ) {}

void _lucVectorArrowCrossSection_Draw( void* drawingObject, lucDatabase* database, void* _context )
{
   lucVectorArrowCrossSection*       self            = (lucVectorArrowCrossSection*)drawingObject;
   DomainContext*            context         = (DomainContext*) _context;

   _lucVectorArrowCrossSection_DrawCrossSection( self, database, context->dim );
}

void _lucVectorArrowCrossSection_DrawCrossSection( void* drawingObject, lucDatabase* database, Dimension_Index dim )
{
   lucVectorArrowCrossSection*  self           = (lucVectorArrowCrossSection*)drawingObject;
   FieldVariable*    vectorVariable = self->fieldVariable;
   double            min = 0.0, max = self->maximum;
   Index          aIndex, bIndex;

   Journal_Firewall( vectorVariable->fieldComponentCount == vectorVariable->dim, lucError,
                     "Error - in %s(): provided FieldVariable \"%s\" has %u components - but %s Component "
                     "can only visualse FieldVariables with %d components.\n", __func__, vectorVariable->name,
                     vectorVariable->fieldComponentCount, self->type, vectorVariable->dim );

   if ( True == self->dynamicRange )
   {
      min = FieldVariable_GetMinGlobalFieldMagnitude( vectorVariable );
      max = FieldVariable_GetMaxGlobalFieldMagnitude( vectorVariable );
   }

   /* Force 3d vectors */
   lucCrossSection_AllocateSampleData(self, 3);

   /* Sample the 2d cross-section */
   lucCrossSection_SampleField(self, False);

   /* Write only values that have data on this processor! */
   for ( aIndex = 0 ; aIndex < self->resolutionA ; aIndex++ )
   {
      for ( bIndex = 0 ; bIndex < self->resolutionB ; bIndex++ )
      {
         if (self->values[aIndex][bIndex][0] != HUGE_VAL)
         {
            lucDatabase_AddVertices(database, 1, lucVectorType, &self->vertices[aIndex][bIndex][0]);
            lucDatabase_AddVectors(database, 1, lucVectorType, min, max, &self->values[aIndex][bIndex][0]);
         }
      }
   }

   lucCrossSection_FreeSampleData(self);
}


