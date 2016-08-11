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
#include "VectorArrowMeshCrossSection.h"

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type lucVectorArrowMeshCrossSection_Type = "lucVectorArrowMeshCrossSection";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
lucVectorArrowMeshCrossSection* _lucVectorArrowMeshCrossSection_New(  LUCVECTORARROWMESHCROSSSECTION_DEFARGS  )
{
   lucVectorArrowMeshCrossSection*					self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucVectorArrowMeshCrossSection) );
   self = (lucVectorArrowMeshCrossSection*) _lucMeshCrossSection_New(  LUCMESHCROSSSECTION_PASSARGS  );

   return self;
}

void _lucVectorArrowMeshCrossSection_Init(
   lucVectorArrowMeshCrossSection*                                  self,
   double                                                       arrowHeadSize,
   double                                                       maximum,
   Bool                                                         dynamicRange,
   double                                                       lengthScale)
{
   self->arrowHead = arrowHeadSize;
   self->maximum = maximum;
   self->dynamicRange = dynamicRange;
   self->scaling = lengthScale;

   /* Get the sampler to move away from boundaries */
   self->offsetEdges = True;

   /* Append to property string */
   lucDrawingObject_AppendProps(self, "scaling=%g\narrowhead=%g\n", lengthScale, arrowHeadSize); 
}

void _lucVectorArrowMeshCrossSection_Delete( void* drawingObject )
{
   lucVectorArrowMeshCrossSection*  self = (lucVectorArrowMeshCrossSection*)drawingObject;
   _lucMeshCrossSection_Delete( self );
}

void _lucVectorArrowMeshCrossSection_Print( void* drawingObject, Stream* stream )
{
   lucVectorArrowMeshCrossSection*  self = (lucVectorArrowMeshCrossSection*)drawingObject;

   _lucMeshCrossSection_Print( self, stream );
}

void* _lucVectorArrowMeshCrossSection_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof(lucVectorArrowMeshCrossSection);
   Type                                                             type = lucVectorArrowMeshCrossSection_Type;
   Stg_Class_DeleteFunction*                                     _delete = _lucVectorArrowMeshCrossSection_Delete;
   Stg_Class_PrintFunction*                                       _print = _lucVectorArrowMeshCrossSection_Print;
   Stg_Class_CopyFunction*                                         _copy = NULL;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _lucVectorArrowMeshCrossSection_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _lucVectorArrowMeshCrossSection_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _lucVectorArrowMeshCrossSection_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _lucVectorArrowMeshCrossSection_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _lucVectorArrowMeshCrossSection_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _lucVectorArrowMeshCrossSection_Destroy;
   lucDrawingObject_SetupFunction*                                _setup = _lucCrossSection_Setup;
   lucDrawingObject_DrawFunction*                                  _draw = _lucVectorArrowMeshCrossSection_Draw;
   lucDrawingObject_CleanUpFunction*                            _cleanUp = lucDrawingObject_CleanUp;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _lucVectorArrowMeshCrossSection_New(  LUCVECTORARROWMESHCROSSSECTION_PASSARGS  );
}

void _lucVectorArrowMeshCrossSection_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data )
{
   lucVectorArrowMeshCrossSection* self = (lucVectorArrowMeshCrossSection*)drawingObject;

   /* Construct Parent */
   _lucMeshCrossSection_AssignFromXML( self, cf, data );
   self->gatherData = False;     /* Drawn in parallel */
   strcpy(self->fieldVariableName, "VectorVariable");

   _lucVectorArrowMeshCrossSection_Init(
      self,
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"arrowHeadSize", 0.3  ),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"maximum", 1.0  ),
      Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"dynamicRange", True  ),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"lengthScale", 0.3  ) );
}

void _lucVectorArrowMeshCrossSection_Build( void* drawingObject, void* data )
{
   lucVectorArrowMeshCrossSection*  self           = (lucVectorArrowMeshCrossSection*)drawingObject;
   /* Build field variable in parent */
   _lucMeshCrossSection_Build(drawingObject, data);

   Journal_Firewall( self->fieldVariable->fieldComponentCount == self->fieldVariable->dim, lucError,
                     "Error - in %s(): provided FieldVariable \"%s\" has %u components - but %s Component "
                     "can only visualse FieldVariables with %d components.\n", __func__, self->fieldVariable->name,
                     self->fieldVariable->fieldComponentCount, self->type, self->fieldVariable->dim );
}

void _lucVectorArrowMeshCrossSection_Initialise( void* drawingObject, void* data ) {}
void _lucVectorArrowMeshCrossSection_Execute( void* drawingObject, void* data ) {}
void _lucVectorArrowMeshCrossSection_Destroy( void* drawingObject, void* data ) {}

void _lucVectorArrowMeshCrossSection_Draw( void* drawingObject, lucDatabase* database, void* _context )
{
   lucVectorArrowMeshCrossSection*       self            = (lucVectorArrowMeshCrossSection*)drawingObject;
   DomainContext*            context         = (DomainContext*) _context;

   _lucVectorArrowMeshCrossSection_DrawCrossSection( self, database, context->dim );
}

void _lucVectorArrowMeshCrossSection_DrawCrossSection( void* drawingObject, lucDatabase* database, Dimension_Index dim )
{
   lucVectorArrowMeshCrossSection*  self           = (lucVectorArrowMeshCrossSection*)drawingObject;
   FieldVariable*    vectorVariable = self->fieldVariable;
   double            min = 0.0, max = self->maximum;

   if ( True == self->dynamicRange )
   {
      min = FieldVariable_GetMinGlobalFieldMagnitude( vectorVariable );
      max = FieldVariable_GetMaxGlobalFieldMagnitude( vectorVariable );
   }

   /* Force 3d vectors */
   self->fieldDim = 3;

   /* Sample the 2d cross-section */
   lucMeshCrossSection_Sample(self, False);

   if (self->context->rank == 0 && database)
   {
      int count = self->dim[1] * self->dim[2];
      lucDatabase_AddVertices(database, count, lucVectorType, &self->vertices[0][0][0]);
      lucDatabase_AddVectors(database, count, lucVectorType, min, max, &self->values[0][0][0]);
   }

   lucCrossSection_FreeSampleData(self);

   /* Start new geometry section - when used with multiple sections */
   lucDatabase_OutputGeometry(database, self->id);
}


