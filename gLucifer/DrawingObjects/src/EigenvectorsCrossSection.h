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

#include "CrossSection.h"

#ifndef __lucEigenvectorsCrossSection_h__
#define __lucEigenvectorsCrossSection_h__

/** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
extern const Type lucEigenvectorsCrossSection_Type;

/** Class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
#define __lucEigenvectorsCrossSection                                                        \
		/* Macro defining parent goes here - This means you can cast this class as its parent */  \
		__lucCrossSection                                                                         \
		/* Virtual functions go here */                                                           \
		/* Other info */                                                                          \
		lucColour                                          colours[3];                            \
      float                                              scaling;                               \
      float                                              arrowHead;                             \
      int                                                glyphs;              \
		/* Colour used to display negative EigenValues */                                         \
		lucColour                                          colourForNegative[3];                  \
		/* Specifies if the eigenvalue is used to draw the vector - default true */               \
		Bool 						                              useEigenValue;                         \
		/* Specifies if the EigenVector and/or EigenValues are to be drawn */                     \
		/* Default is True for EigenVector, False for EigenValues */                              \
      Bool                                               plotEigenVector;                       \
		Bool                                               plotEigenValue;                        \
 

struct lucEigenvectorsCrossSection
{
   __lucEigenvectorsCrossSection
};

/** Private Constructor: This will accept all the virtual functions for this class as arguments. */

#ifndef ZERO
#define ZERO 0
#endif

#define LUCEIGENVECTORSCROSSSECTION_DEFARGS \
                LUCCROSSSECTION_DEFARGS

#define LUCEIGENVECTORSCROSSSECTION_PASSARGS \
                LUCCROSSSECTION_PASSARGS

lucEigenvectorsCrossSection* _lucEigenvectorsCrossSection_New(  LUCEIGENVECTORSCROSSSECTION_DEFARGS  );

void _lucEigenvectorsCrossSection_Delete( void* drawingObject ) ;
void _lucEigenvectorsCrossSection_Print( void* drawingObject, Stream* stream ) ;

/* 'Stg_Component' implementations */
void* _lucEigenvectorsCrossSection_DefaultNew( Name name ) ;
void _lucEigenvectorsCrossSection_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data );
void _lucEigenvectorsCrossSection_Build( void* drawingObject, void* data ) ;
void _lucEigenvectorsCrossSection_Initialise( void* drawingObject, void* data ) ;
void _lucEigenvectorsCrossSection_Execute( void* drawingObject, void* data );
void _lucEigenvectorsCrossSection_Destroy( void* drawingObject, void* data ) ;

void _lucEigenvectorsCrossSection_Draw( void* drawingObject, lucDatabase* database, void* _context );
void _lucEigenvectorsCrossSection_DrawCrossSection( void* drawingObject, lucDatabase* database, Dimension_Index dim );

#endif

