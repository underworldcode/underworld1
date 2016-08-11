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
#include <gLucifer/Base/DrawingObject.h>

#ifndef __lucCrossSection_h__
#define __lucCrossSection_h__

/** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
extern const Type lucCrossSection_Type;

/** Class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
#define __lucCrossSection                    \
      /* Macro defining parent goes here - This means you can cast this class as its parent */ \
      __lucDrawingObject                  \
      /* Virtual functions go here */           \
      /* Other info */                          \
      FieldVariable*       fieldVariable;       \
      Name                 fieldVariableName;   \
      double               value;               \
      Index                defaultResolution;   \
      Index                resolutionA;         \
      Index                resolutionB;         \
      XYZ                  normal;              \
      XYZ                  coord1;              \
      XYZ                  coord2;              \
      XYZ                  coord3;              \
      Axis                 axis;                \
      Axis                 axis1;               \
      Axis                 axis2;               \
      Bool                 interpolate;         \
      Bool                 axisAligned;         \
      Bool                 gatherData;          \
      Bool                 offsetEdges;         \
      Bool                 cullface;            \
      float***             vertices;            \
      float***             values;              \
      Coord                min;                 \
      Coord                max;                 \
 
struct lucCrossSection
{
   __lucCrossSection
};

/** Private Constructor: This will accept all the virtual functions for this class as arguments. */

#ifndef ZERO
#define ZERO 0
#endif

#define LUCCROSSSECTION_DEFARGS \
                LUCDRAWINGOBJECT_DEFARGS

#define LUCCROSSSECTION_PASSARGS \
                LUCDRAWINGOBJECT_PASSARGS

lucCrossSection* _lucCrossSection_New(  LUCCROSSSECTION_DEFARGS  );

void _lucCrossSection_Delete( void* drawingObject ) ;
void _lucCrossSection_Print( void* drawingObject, Stream* stream ) ;

void* _lucCrossSection_DefaultNew( Name name ) ;
void _lucCrossSection_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data );
void _lucCrossSection_Build( void* drawingObject, void* data ) ;
void _lucCrossSection_Initialise( void* drawingObject, void* data ) ;
void _lucCrossSection_Execute( void* drawingObject, void* data );
void _lucCrossSection_Destroy( void* drawingObject, void* data ) ;

void _lucCrossSection_Setup( void* drawingObject, lucDatabase* database, void* _context );
void _lucCrossSection_Draw( void* drawingObject, lucDatabase* database, void* _context );
void lucCrossSection_Interpolate2d(void* crossSection, double factorA, double factorB, XYZ coord);
void lucCrossSection_SetPosition(void* crossSection);
double lucCrossSection_GetValue(void* crossSection, double min, double max);
lucCrossSection* lucCrossSection_Set(void* crossSection, double val, Axis axis, Bool interpolate);
lucCrossSection* lucCrossSection_Slice(void* crossSection, double val, Bool interpolate);

void lucCrossSection_AllocateSampleData(void* drawingObject, int dims);
void lucCrossSection_SampleField(void* drawingObject, Bool reverse);
void lucCrossSection_FreeSampleData(void* drawingObject);
#endif

