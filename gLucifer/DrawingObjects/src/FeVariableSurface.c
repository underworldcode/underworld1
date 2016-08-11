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
#include <float.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>

#include <gLucifer/Base/Base.h>


#include "types.h"
#include "MeshCrossSection.h"
#include "FeVariableSurface.h"

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type lucFeVariableSurface_Type = "lucFeVariableSurface";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
lucFeVariableSurface* _lucFeVariableSurface_New(  LUCFEVARIABLESURFACE_DEFARGS  )
{
   lucFeVariableSurface*					self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucFeVariableSurface) );
   self = (lucFeVariableSurface*) _lucMeshCrossSection_New(  LUCMESHCROSSSECTION_PASSARGS  );

   return self;
}

void _lucFeVariableSurface_Init(
   lucFeVariableSurface*                                        self,
   float                                                        scaleHeight )
{
   self->scaling = scaleHeight;

   /* Append to property string */
   lucDrawingObject_AppendProps(self, "scaling=%g\n", scaleHeight); 
}

void _lucFeVariableSurface_Delete( void* drawingObject )
{
   lucFeVariableSurface*  self = (lucFeVariableSurface*)drawingObject;

   _lucMeshCrossSection_Delete( self );
}

void* _lucFeVariableSurface_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof(lucFeVariableSurface);
   Type                                                             type = lucFeVariableSurface_Type;
   Stg_Class_DeleteFunction*                                     _delete = _lucFeVariableSurface_Delete;
   Stg_Class_PrintFunction*                                       _print = NULL;
   Stg_Class_CopyFunction*                                         _copy = NULL;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _lucFeVariableSurface_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _lucFeVariableSurface_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _lucFeVariableSurface_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _lucFeVariableSurface_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _lucFeVariableSurface_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _lucFeVariableSurface_Destroy;
   lucDrawingObject_SetupFunction*                                _setup = lucDrawingObject_Setup;
   lucDrawingObject_DrawFunction*                                  _draw = _lucFeVariableSurface_Draw;
   lucDrawingObject_CleanUpFunction*                            _cleanUp = lucDrawingObject_CleanUp;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _lucFeVariableSurface_New(  LUCFEVARIABLESURFACE_PASSARGS  );
}

void _lucFeVariableSurface_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data )
{
   lucFeVariableSurface*  self = (lucFeVariableSurface*)drawingObject;

   /* Construct Parent */
   _lucMeshCrossSection_AssignFromXML( self, cf, data );

   _lucFeVariableSurface_Init(
      self,
      (float) Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"scaleHeight", 1.0 )  );
}

void _lucFeVariableSurface_Build( void* drawingObject, void* data ) 
{
   /* Build field variable in parent */
   _lucMeshCrossSection_Build(drawingObject, data);
}

void _lucFeVariableSurface_Initialise( void* drawingObject, void* data ) {}
void _lucFeVariableSurface_Execute( void* drawingObject, void* data ) {}
void _lucFeVariableSurface_Destroy( void* drawingObject, void* data ) {}

void _lucFeVariableSurface_Draw( void* drawingObject, lucDatabase* database, void* _context )
{
   lucFeVariableSurface* self = (lucFeVariableSurface*)drawingObject;
   int i,j;

   lucCrossSection_Set(self, 0.0, K_AXIS, False);

   lucMeshCrossSection_Sample(self, False);

   /* Calibrate Colour Map */
   if (self->colourMap)
      lucColourMap_CalibrateFromFieldVariable(self->colourMap, self->fieldVariable);

   /* Set 3rd dimension to scaled values */
   for ( i=0 ; i < self->dim[1]; i++ )
   {
      for ( j=0 ; j < self->dim[2]; j++ )
      {
         self->vertices[i][j][2] = self->values[i][j][0] * self->scaling;
      }
   }

   if (self->context->rank == 0 && database)
   {
      int count = self->dim[1] * self->dim[2];
      lucDatabase_AddGridVertices(database, count, self->dim[2], self->vertices[0][0]);
      lucDatabase_AddValues(database, count, lucGridType, lucColourValueData, self->colourMap, self->values[0][0]);
   }

   lucCrossSection_FreeSampleData(self);
}


