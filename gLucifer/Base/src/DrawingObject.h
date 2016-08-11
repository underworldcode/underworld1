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


#ifndef __lucDrawingObject_h__
#define __lucDrawingObject_h__

extern const Type lucDrawingObject_Type;

typedef void (lucDrawingObject_SetupFunction) ( void* object, lucDatabase* database, void* context );
typedef void (lucDrawingObject_DrawFunction)  ( void* object, lucDatabase* database, void* context );
typedef void (lucDrawingObject_CleanUpFunction)  ( void* object, void* context );

/** Class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
#define __lucDrawingObject                           \
      /* Macro defining parent goes here - This means you can cast this class as its parent */ \
      __Stg_Component                                   \
      AbstractContext*               context;             \
      /* Virtual Functions */ \
      lucDrawingObject_SetupFunction*                    _setup;              \
      lucDrawingObject_DrawFunction*                     _draw;               \
      lucDrawingObject_CleanUpFunction*                  _cleanUp;            \
      /* Other Info */ \
      Bool                                               needsToCleanUp;      \
      Bool                                               needsToDraw;         \
      Bool                                               depthSort;           \
      Coord                                              position;            \
      /* Drawing settings */ \
      Bool                                               lit;                 \
      Bool                                               disabled;            \
      lucColourMap*                                      colourMap;           \
      lucColour                                          colour;              \
      float                                              opacity;             \
      Bool                                               wireframe;           \
      int                                                lineWidth;           \
      int                                                id;                  \
      char*                                              properties;          \

struct lucDrawingObject
{
   __lucDrawingObject
};


#ifndef ZERO
#define ZERO 0
#endif

#define LUCDRAWINGOBJECT_DEFARGS \
                STG_COMPONENT_DEFARGS, \
                lucDrawingObject_SetupFunction*      _setup, \
                lucDrawingObject_DrawFunction*        _draw, \
                lucDrawingObject_CleanUpFunction*  _cleanUp

#define LUCDRAWINGOBJECT_PASSARGS \
                STG_COMPONENT_PASSARGS, \
           _setup,   \
           _draw,    \
           _cleanUp

lucDrawingObject* _lucDrawingObject_New(  LUCDRAWINGOBJECT_DEFARGS  );


void _lucDrawingObject_Delete( void* drawingObject ) ;
void _lucDrawingObject_Print( void* drawingObject, Stream* stream ) ;
void* _lucDrawingObject_Copy( void* drawingObject, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) ;

void _lucDrawingObject_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data ) ;
void _lucDrawingObject_Build( void* camera, void* data );
void _lucDrawingObject_Initialise( void* camera, void* data );
void _lucDrawingObject_Execute( void* camera, void* data );
void _lucDrawingObject_Destroy( void* camera, void* data );

/* +++ Public Functions +++ */
void lucDrawingObject_Setup( void* drawingObject, lucDatabase* database, void* context ) ;
void lucDrawingObject_Draw( void* drawingObject, lucDatabase* database, void* context ) ;
void lucDrawingObject_CleanUp( void* drawingObject, void* context ) ;

typedef enum { GreaterThan, LessThan, EqualTo } lucDrawingObjectMask_Type;
typedef struct
{
   lucDrawingObjectMask_Type type;
   double                    value;
   double                    tolerance;
} lucDrawingObjectMask;

void lucDrawingObjectMask_Construct( lucDrawingObjectMask* self, Name drawingObjectName, Stg_ComponentFactory* cf, void* data ) ;
Bool lucDrawingObjectMask_Test( lucDrawingObjectMask* self, double value ) ;

/* HACK - a function to check whether a field is an FeVariable or not before it does an FeVariable_SyncShadowValues */
void lucDrawingObject_SyncShadowValues( void* drawingObject, void* field );

void lucDrawingObject_AppendProps(lucDrawingObject* self, const char *fmt, ...);
#endif

