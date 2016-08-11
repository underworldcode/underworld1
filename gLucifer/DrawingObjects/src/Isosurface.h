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


#ifndef __lucIsosurface_h__
#define __lucIsosurface_h__

/** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
extern const Type lucIsosurface_Type;

typedef struct
{
   float pos[3][3];
   float value[3];
   Bool  wall;
} Surface_Triangle;

typedef struct
{
   double value;
   double pos[3];
   int   element_I;
} Vertex;


/** Class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
#define __lucIsosurface \
      /* Macro defining parent goes here - This means you can cast this class as its parent */ \
      __lucDrawingObject \
      /* Virtual functions go here */ \
      /* Other info */\
      FieldVariable*                      isosurfaceField;        \
      double                              isovalue;               \
      IJK                                 resolution;             \
      Bool                                drawWalls;              \
      Bool                                sampleGlobal;           \
      Coord                               globalMin;              \
      Coord                               globalMax;              \
      /* Colour Parameters */ \
      FieldVariable*                      colourField;            \
      /* Masking parameters */\
      FieldVariable*                      maskField;              \
      lucDrawingObjectMask                mask;                   \
      /* Calculated Values */ \
      Surface_Triangle*                   triangleList;           \
      Index                               triangleCount;          \
      Index                               trianglesAlloced;       \
      Index                               nx;                     \
      Index                               ny;                     \
      Index                               nz;                     \
      Index                               elementRes[3];          \
 
struct lucIsosurface
{
   __lucIsosurface
};

/** Private Constructor: This will accept all the virtual functions for this class as arguments. */

#ifndef ZERO
#define ZERO 0
#endif

#define LUCISOSURFACE_DEFARGS \
                LUCDRAWINGOBJECT_DEFARGS

#define LUCISOSURFACE_PASSARGS \
                LUCDRAWINGOBJECT_PASSARGS

lucIsosurface* _lucIsosurface_New(  LUCISOSURFACE_DEFARGS  );

void _lucIsosurface_Delete( void* drawingObject ) ;
void _lucIsosurface_Print( void* drawingObject, Stream* stream ) ;

/* 'Stg_Component' implementations */
void* _lucIsosurface_DefaultNew( Name name ) ;
void _lucIsosurface_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data );
void _lucIsosurface_Build( void* drawingObject, void* data ) ;
void _lucIsosurface_Initialise( void* drawingObject, void* data ) ;
void _lucIsosurface_Execute( void* drawingObject, void* data );
void _lucIsosurface_Destroy( void* drawingObject, void* data ) ;

/* Drawing Object Implementations */
void _lucIsosurface_Setup( void* drawingObject, lucDatabase* database, void* _context ) ;
void _lucIsosurface_Draw( void* drawingObject, lucDatabase* database, void* _context ) ;

void lucIsosurface_MarchingCubes( lucIsosurface* self, Vertex*** vertex ) ;
void lucIsosurface_DrawWalls( lucIsosurface* self, Vertex*** array ) ;

void lucIsosurface_MarchingRectangles( lucIsosurface* self, Vertex** points, char squareType ) ;
void lucIsosurface_WallElement( lucIsosurface* self, Vertex** points ) ;

#endif

