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


#ifndef __lucSwarmShapes_h__
#define __lucSwarmShapes_h__

/** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
extern const Type lucSwarmShapes_Type;

/** Class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
#define __lucSwarmShapes \
		/* Macro defining parent goes here - This means you can cast this class as its parent */ \
		__lucSwarmViewer \
		/* Virtual functions go here */ \
		/* Other info */\
		/* Colour Stuff */ \
		SwarmVariable*                                     planeVectorVariable;          \
		Name                                               planeVectorVariableName;      \
		SwarmVariable*                                     lengthVariable;               \
		Name                                               lengthVariableName;           \
		SwarmVariable*                                     widthVariable;                \
		Name                                               widthVariableName;            \
		SwarmVariable*                                     heightVariable;               \
		Name                                               heightVariableName;           \
      double                                             length;                       \
      double                                             width;                        \
      double                                             height;                       \
      Bool                                               square;                       \
 
struct lucSwarmShapes
{
   __lucSwarmShapes
};

/** Private Constructor: This will accept all the virtual functions for this class as arguments. */

#ifndef ZERO
#define ZERO 0
#endif

#define LUCSwarmShapes_DEFARGS \
                LUCSWARMVIEWER_DEFARGS

#define LUCSwarmShapes_PASSARGS \
                LUCSWARMVIEWER_PASSARGS

lucSwarmShapes* _lucSwarmShapes_New(  LUCSwarmShapes_DEFARGS  );

void _lucSwarmShapes_Delete( void* drawingObject ) ;
void _lucSwarmShapes_Print( void* drawingObject, Stream* stream ) ;
void* _lucSwarmShapes_Copy( void* drawingObject, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap) ;

/* 'Stg_Component' implementations */
void* _lucSwarmShapes_DefaultNew( Name name ) ;
void _lucSwarmShapes_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data );
void _lucSwarmShapes_Build( void* drawingObject, void* data ) ;
void _lucSwarmShapes_Initialise( void* drawingObject, void* data ) ;
void _lucSwarmShapes_Execute( void* drawingObject, void* data );
void _lucSwarmShapes_Destroy( void* drawingObject, void* data ) ;

void _lucSwarmShapes_Setup( void* drawingObject, lucDatabase* database, void* _context ) ;

/* SwarmViewer Implementations */
void _lucSwarmShapes_PlotParticle( void* drawingObject, lucDatabase* database, void* _context, Particle_Index lParticle_I ) ;

void lucSwarmShapes_UpdateVariables( void* drawingObject ) ;
#endif

