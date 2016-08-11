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
#include "ScalarFieldOnMeshCrossSection.h"

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type lucScalarFieldOnMeshCrossSection_Type = "lucScalarFieldOnMeshCrossSection";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
lucScalarFieldOnMeshCrossSection* _lucScalarFieldOnMeshCrossSection_New(  LUCSCALARFIELDONMESHCROSSSECTION_DEFARGS  )
{
   lucScalarFieldOnMeshCrossSection*					self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucScalarFieldOnMeshCrossSection) );
   self = (lucScalarFieldOnMeshCrossSection*) _lucMeshCrossSection_New(  LUCMESHCROSSSECTION_PASSARGS  );

   return self;
}

void _lucScalarFieldOnMeshCrossSection_Init(
   lucScalarFieldOnMeshCrossSection*                            self)
{
   self->flipNormals = False;
}

void _lucScalarFieldOnMeshCrossSection_Delete( void* drawingObject )
{
   lucScalarFieldOnMeshCrossSection*  self = (lucScalarFieldOnMeshCrossSection*)drawingObject;

   _lucMeshCrossSection_Delete( self );
}

void _lucScalarFieldOnMeshCrossSection_Print( void* drawingObject, Stream* stream )
{
   lucScalarFieldOnMeshCrossSection*  self = (lucScalarFieldOnMeshCrossSection*)drawingObject;

   _lucMeshCrossSection_Print( self, stream );
}

void* _lucScalarFieldOnMeshCrossSection_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof(lucScalarFieldOnMeshCrossSection);
   Type                                                             type = lucScalarFieldOnMeshCrossSection_Type;
   Stg_Class_DeleteFunction*                                     _delete = _lucScalarFieldOnMeshCrossSection_Delete;
   Stg_Class_PrintFunction*                                       _print = _lucScalarFieldOnMeshCrossSection_Print;
   Stg_Class_CopyFunction*                                         _copy = NULL;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _lucScalarFieldOnMeshCrossSection_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _lucScalarFieldOnMeshCrossSection_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _lucScalarFieldOnMeshCrossSection_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _lucScalarFieldOnMeshCrossSection_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _lucScalarFieldOnMeshCrossSection_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _lucScalarFieldOnMeshCrossSection_Destroy;
   lucDrawingObject_SetupFunction*                                _setup = _lucCrossSection_Setup;
   lucDrawingObject_DrawFunction*                                  _draw = _lucScalarFieldOnMeshCrossSection_Draw;
   lucDrawingObject_CleanUpFunction*                            _cleanUp = lucDrawingObject_CleanUp;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _lucScalarFieldOnMeshCrossSection_New(  LUCSCALARFIELDONMESHCROSSSECTION_PASSARGS  );
}

void _lucScalarFieldOnMeshCrossSection_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data )
{
   lucScalarFieldOnMeshCrossSection*     self = (lucScalarFieldOnMeshCrossSection*)drawingObject;

   /* Construct Parent */
   _lucMeshCrossSection_AssignFromXML( self, cf, data );

   _lucScalarFieldOnMeshCrossSection_Init(self);
}

void _lucScalarFieldOnMeshCrossSection_Build( void* drawingObject, void* data )
{
   lucScalarFieldOnMeshCrossSection*     self    = (lucScalarFieldOnMeshCrossSection*)drawingObject;
   FeVariable*                     feVariable;
   Mesh*                           mesh;

   /* Build field variable in parent */
   _lucMeshCrossSection_Build(self, data);

   Journal_Firewall( self->fieldVariable->fieldComponentCount == 1, lucError,
                     "Error - in %s(): provided FieldVariable \"%s\" has %u components - but %s Component "
                     "can only visualise FieldVariables with 1 component. Did you mean to visualise the "
                     "magnitude of the given field?\n", __func__, self->fieldVariable->name,
                     self->fieldVariable->fieldComponentCount, self->type );
}

void _lucScalarFieldOnMeshCrossSection_Initialise( void* drawingObject, void* data ) {}
void _lucScalarFieldOnMeshCrossSection_Execute( void* drawingObject, void* data ) {}
void _lucScalarFieldOnMeshCrossSection_Destroy( void* drawingObject, void* data ) {}

void _lucScalarFieldOnMeshCrossSection_Draw( void* drawingObject, lucDatabase* database, void* _context )
{
   lucScalarFieldOnMeshCrossSection*       self            = (lucScalarFieldOnMeshCrossSection*)drawingObject;
   lucScalarFieldOnMeshCrossSection_DrawCrossSection( self, database, False);
}

void lucScalarFieldOnMeshCrossSection_DrawCrossSection( void* drawingObject, lucDatabase* database, Bool backFacing)
{
   lucScalarFieldOnMeshCrossSection*       self            = (lucScalarFieldOnMeshCrossSection*)drawingObject;

   lucMeshCrossSection_Sample(self, backFacing);

   /* Calibrate Colour Map */
   if (self->colourMap)
      lucColourMap_CalibrateFromFieldVariable(self->colourMap, self->fieldVariable);

   if (self->context->rank == 0 && database)
   {
      int count = self->dim[1] * self->dim[2];
      lucDatabase_AddGridVertices(database, count, self->dim[2], self->vertices[0][0]);
      lucDatabase_AddValues(database, count, lucGridType, lucColourValueData, self->colourMap, &self->values[0][0][0]);
   }

   lucCrossSection_FreeSampleData(self);

   /* Start new geometry section - when used with multiple sections */
   lucDatabase_OutputGeometry(database, self->id);
}


