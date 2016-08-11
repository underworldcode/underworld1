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


#ifndef __lucSwarmViewer_h__
#define __lucSwarmViewer_h__

typedef void (lucSwarmViewer_PlotParticleFunction) ( void* object, lucDatabase* database, void* context, Particle_Index lParticle_I );
typedef void (lucSwarmViewer_SetParticleColourFunction) ( void* object, lucDatabase* database, void* context, Particle_Index lParticle_I );

/** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
extern const Type lucSwarmViewer_Type;

/** Class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
#define __lucSwarmViewer \
      /* Macro defining parent goes here - This means you can cast this class as its parent */ \
      __lucDrawingObject \
      /* Virtual functions go here */ \
      lucSwarmViewer_PlotParticleFunction*           _plotParticle;          \
      lucSwarmViewer_SetParticleColourFunction*      _setParticleColour;     \
      /* Colour stuff */\
      Name                                               colourVariableName;     \
      SwarmVariable*                                     colourVariable;         \
      /* Size */\
      Name                                               sizeVariableName;       \
      SwarmVariable*                                     sizeVariable;           \
      /* Other info */\
      Swarm*                                             swarm;                  \
      /* Opacity Stuff */ \
      lucColourMap*                                      opacityColourMap;       \
      Name                                               opacityVariableName;    \
      SwarmVariable*                                     opacityVariable;        \
      /* Mask Info */ \
      Name                                               maskVariableName;       \
      SwarmVariable*                                     maskVariable;           \
      lucDrawingObjectMask                               mask;                   \
      /* Other Stuff */ \
      Bool                                               drawParticleNumber;     \
      Bool                                               sameParticleColour;     \
      int                                                subSample;              \
      Bool                                               positionRange;          \
      Coord                                              minPosition;            \
      Coord                                              maxPosition;            \
      lucGeometryType                                    geomType;               \
      float                                              scaling;                \
      float                                  pointSize; \
      Bool                                   pointSmoothing;

struct lucSwarmViewer
{
   __lucSwarmViewer
};

/** Private Constructor: This will accept all the virtual functions for this class as arguments. */

#ifndef ZERO
#define ZERO 0
#endif

#define LUCSWARMVIEWER_DEFARGS \
                LUCDRAWINGOBJECT_DEFARGS, \
                lucSwarmViewer_PlotParticleFunction*            _plotParticle, \
                lucSwarmViewer_SetParticleColourFunction*  _setParticleColour

#define LUCSWARMVIEWER_PASSARGS \
                LUCDRAWINGOBJECT_PASSARGS, \
           _plotParticle,      \
           _setParticleColour

lucSwarmViewer* _lucSwarmViewer_New(  LUCSWARMVIEWER_DEFARGS  );

void _lucSwarmViewer_Delete( void* drawingObject ) ;
void _lucSwarmViewer_Print( void* drawingObject, Stream* stream ) ;
void* _lucSwarmViewer_Copy( void* drawingObject, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap) ;
void* _lucSwarmViewer_DefaultNew( Name name );

/* 'Stg_Component' implementations */
void _lucSwarmViewer_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data );
void _lucSwarmViewer_Build( void* drawingObject, void* data ) ;
void _lucSwarmViewer_Initialise( void* drawingObject, void* data ) ;
void _lucSwarmViewer_Execute( void* drawingObject, void* data );
void _lucSwarmViewer_Destroy( void* drawingObject, void* data ) ;

SwarmVariable* lucSwarmViewer_InitialiseVariable(void* object, Name variableName, Bool scalarRequired, void* data );
float lucSwarmViewer_GetScalar(SwarmVariable* variable, Particle_Index lParticle_I, float defaultVal);

void _lucSwarmViewer_Setup( void* drawingObject, lucDatabase* database, void* _context ) ;
void _lucSwarmViewer_Draw( void* drawingObject, lucDatabase* database, void* _context ) ;

void lucSwarmViewBase_DrawParticleNumbers( void* drawingObject, void* _context ) ;

void lucSwarmViewer_UpdateVariables( void* drawingObject ) ;

void lucSwarmViewer_FindParticleLocalIndex(void *drawingObject, Coord coord, Particle_Index  *lParticle_I);

void lucSwarmViewer_SetColourComponent(void* object, lucDatabase* database, SwarmVariable* var, Particle_Index lParticle_I, lucGeometryDataType type, lucColourMap* colourMap);
void _lucSwarmViewer_SetParticleColour( void* drawingObject, lucDatabase* database, void* _context, Particle_Index lParticle_I ) ;

void _lucSwarmViewer_PlotParticle( void* drawingObject, lucDatabase* database, void* _context, Particle_Index lParticle_I );
#endif

