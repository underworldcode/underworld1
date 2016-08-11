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
#include <gLucifer/Base/DrawingObject.h>

#ifndef __lucPlot_h__
#define __lucPlot_h__

/** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
extern const Type lucPlot_Type;

/** Class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
#define __lucPlot                    \
		/* Macro defining parent goes here - This means you can cast this class as its parent */ \
		__lucDrawingObject                  \
		/* Virtual functions go here */           \
		/* Other info */                          \
		char*                dataFileName;        \
		char*                colourMapFileName;        \
      unsigned int         columns;             \
      unsigned int         columnX;          \
      unsigned int         columnY;          \
	  unsigned int         columnZ;          \
      unsigned int         skipRows;         \
      double               coordZ;           \
      char*                labelX;           \
      char*                labelY;           \
	char*                labelZ;           \
      Bool                 lines;            \
      Bool                 points;           \
      Bool                 bars;             \
	unsigned int         ticksX;          \
	unsigned int         ticksY;          \
	unsigned int         ticksZ;          \
	Bool                 flipY;            \
	Bool            setX; \
	Bool            setY; \
	Bool            setZ; \
	double			minX;	\
	double           minY;	\
	double           minZ;	\
	double			maxX;	\
	double           maxY;	\
	double           maxZ;	\
      unsigned int         pointSize;          \
	Bool                 colourMapFromFile;            \
      /* Axis data values */                 \
      double*              dataX;            \
      double*              dataY;            \
	double*              dataZ;            \
      unsigned int         rows;          \


struct lucPlot
{
   __lucPlot
};

/** Private Constructor: This will accept all the virtual functions for this class as arguments. */

#ifndef ZERO
#define ZERO 0
#endif

#define LUCPLOT_DEFARGS \
                LUCDRAWINGOBJECT_DEFARGS

#define LUCPLOT_PASSARGS \
                LUCDRAWINGOBJECT_PASSARGS

lucPlot* _lucPlot_New(  LUCPLOT_DEFARGS  );

void _lucPlot_Delete( void* drawingObject ) ;
void _lucPlot_Print( void* drawingObject, Stream* stream ) ;

/* 'Stg_Component' implementations */
void* _lucPlot_DefaultNew( Name name ) ;
void _lucPlot_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data );
void _lucPlot_Build( void* drawingObject, void* data ) ;
void _lucPlot_Initialise( void* drawingObject, void* data ) ;
void _lucPlot_Execute( void* drawingObject, void* data );
void _lucPlot_Destroy( void* drawingObject, void* data ) ;

void lucPlot_ReadDataFile( void* drawingObject );
void _lucPlot_Draw( void* drawingObject, lucDatabase* database, void* _context );

#endif

