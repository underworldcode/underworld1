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
#include "SwarmViewer.h"
#include "SwarmViewer.h"
#include "SwarmShapes.h"

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type lucSwarmShapes_Type = "lucSwarmShapes";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
lucSwarmShapes* _lucSwarmShapes_New(  LUCSwarmShapes_DEFARGS  )
{
   lucSwarmShapes*					self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucSwarmShapes) );
   self = (lucSwarmShapes*) _lucSwarmViewer_New(  LUCSWARMVIEWER_PASSARGS  );

   return self;
}

void _lucSwarmShapes_Init(
   lucSwarmShapes*                                   self,
   Name                                               colourVariableName,
   Name                                               planeVectorVariableName,
   Name                                               lengthVariableName,
   Name                                               widthVariableName,
   Name                                               heightVariableName,
   double                                             length,
   double                                             width,
   double                                             height,
   Bool                                               square)
{
   self->colourVariableName  = colourVariableName;
   self->planeVectorVariableName = planeVectorVariableName;
   self->lengthVariableName      = lengthVariableName;
   self->widthVariableName       = widthVariableName;
   self->heightVariableName      = heightVariableName;
   self->length                  = length;
   self->width                   = width;
   self->height                  = height;
   self->square                  = square;
   self->geomType = lucShapeType;   /* Draw 3d shapes */

   /* Append shape type to property string */
   lucDrawingObject_AppendProps(self, "shape=%d\nlength=%g\nwidth=%g\nheight=%g\n", (square ? 1 : 0), length, width, height);
}

void _lucSwarmShapes_Delete( void* drawingObject )
{
   lucSwarmShapes*  self = (lucSwarmShapes*)drawingObject;

   _lucSwarmViewer_Delete( self );
}

void _lucSwarmShapes_Print( void* drawingObject, Stream* stream )
{
   lucSwarmShapes*  self = (lucSwarmShapes*)drawingObject;

   _lucSwarmViewer_Print( self, stream );
}

void* _lucSwarmShapes_Copy( void* drawingObject, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap)
{
   lucSwarmShapes*  self = (lucSwarmShapes*)drawingObject;
   lucSwarmShapes* newDrawingObject;

   newDrawingObject = _lucSwarmViewer_Copy( self, dest, deep, nameExt, ptrMap );

   /* TODO */
   abort();

   return (void*) newDrawingObject;
}


void* _lucSwarmShapes_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof(lucSwarmShapes);
   Type                                                             type = lucSwarmShapes_Type;
   Stg_Class_DeleteFunction*                                     _delete = _lucSwarmShapes_Delete;
   Stg_Class_PrintFunction*                                       _print = _lucSwarmShapes_Print;
   Stg_Class_CopyFunction*                                         _copy = NULL;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _lucSwarmShapes_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _lucSwarmShapes_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _lucSwarmShapes_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _lucSwarmShapes_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _lucSwarmShapes_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _lucSwarmShapes_Destroy;
   lucDrawingObject_SetupFunction*                                _setup = _lucSwarmShapes_Setup;
   lucDrawingObject_DrawFunction*                                  _draw = _lucSwarmViewer_Draw;
   lucDrawingObject_CleanUpFunction*                            _cleanUp = lucDrawingObject_CleanUp;
   lucSwarmViewer_PlotParticleFunction*                _plotParticle = _lucSwarmShapes_PlotParticle;
   lucSwarmViewer_SetParticleColourFunction*      _setParticleColour = _lucSwarmViewer_SetParticleColour;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _lucSwarmShapes_New(  LUCSwarmShapes_PASSARGS  );
}

void _lucSwarmShapes_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data )
{
   lucSwarmShapes*  self = (lucSwarmShapes*)drawingObject;
   Name                    colourVariableName;

   /* Construct Parent */
   _lucSwarmViewer_AssignFromXML( self, cf, data );

   colourVariableName = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"ColourVariable", ""  );

   _lucSwarmShapes_Init(
      self,
      colourVariableName,
      Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"PlaneVectorVariable", ""  ),
      Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"LengthVariable", ""  ),
      Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"WidthVariable", ""  ),
      Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"HeightVariable", ""  ),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"length", 0.01 ),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"width", 0.01 ),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"height", 0.01 ),
      Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"square", False )
      );
}

void _lucSwarmShapes_Build( void* drawingObject, void* data ) {}
void _lucSwarmShapes_Initialise( void* drawingObject, void* data )
{
   lucSwarmShapes*         self                   = (lucSwarmShapes*)drawingObject;

   _lucSwarmViewer_Initialise( self, data );

   self->colourVariable = lucSwarmViewer_InitialiseVariable(self, self->colourVariableName, True, data);
   self->planeVectorVariable = lucSwarmViewer_InitialiseVariable(self, self->planeVectorVariableName, False, data);
   self->lengthVariable = lucSwarmViewer_InitialiseVariable(self, self->lengthVariableName, False, data);
   self->widthVariable = lucSwarmViewer_InitialiseVariable(self, self->widthVariableName, False, data);
   self->heightVariable = lucSwarmViewer_InitialiseVariable(self, self->heightVariableName, False, data);
}

void _lucSwarmShapes_Execute( void* drawingObject, void* data ) {}
void _lucSwarmShapes_Destroy( void* drawingObject, void* data ) {}

void _lucSwarmShapes_Setup( void* drawingObject, lucDatabase* database, void* _context )
{
   lucSwarmShapes_UpdateVariables( drawingObject );
   _lucSwarmViewer_Setup( drawingObject, database, _context );
}

void _lucSwarmShapes_PlotParticle( void* drawingObject, lucDatabase* database, void* _context, Particle_Index lParticle_I )
{
   lucSwarmShapes*         self                = (lucSwarmShapes*)drawingObject;
   float length = self->length;
   float width = self->width;
   float height = self->height;

   if ( self->planeVectorVariable )
   {
      XYZ planeVector = { 0, 0, 0 };
      SwarmVariable_ValueAt( self->planeVectorVariable, lParticle_I, planeVector );
      float vecf[3] = {planeVector[0], planeVector[1], planeVector[2]};
      lucDatabase_AddVectors(database, 1, self->geomType, 0, 0, vecf);
   }

   if (self->lengthVariable)
   {
      length = lucSwarmViewer_GetScalar(self->lengthVariable, lParticle_I, length);
      lucDatabase_AddValues(database, 1, self->geomType, lucZLengthData, NULL, &length);
   }
   if (self->heightVariable)
   {
      height = lucSwarmViewer_GetScalar(self->heightVariable, lParticle_I, height);
      lucDatabase_AddValues(database, 1, self->geomType, lucYHeightData, NULL, &height);
   }
   if (self->widthVariable)
   {
      width = lucSwarmViewer_GetScalar(self->widthVariable, lParticle_I, width);
      lucDatabase_AddValues(database, 1, self->geomType, lucXWidthData, NULL, &width);
   }
   /*fprintf(stderr, "Shape scaled %f,%f,%f\n", width,height,length);*/
}

void lucSwarmShapes_UpdateVariables( void* drawingObject )
{
   lucSwarmShapes*          self                = (lucSwarmShapes*)drawingObject;

   lucSwarmViewer_UpdateVariables( drawingObject ) ;

   if ( self->planeVectorVariable && self->planeVectorVariable->variable )
      Variable_Update( self->planeVectorVariable->variable );
   if ( self->lengthVariable && self->lengthVariable->variable )
      Variable_Update( self->lengthVariable->variable );
   if ( self->widthVariable && self->widthVariable->variable )
      Variable_Update( self->widthVariable->variable );
   if ( self->heightVariable && self->heightVariable->variable )
      Variable_Update( self->heightVariable->variable );
}


