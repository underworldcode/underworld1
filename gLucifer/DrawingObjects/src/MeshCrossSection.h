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

#include "CrossSection.h"

#ifndef __lucMeshCrossSection_h__
#define __lucMeshCrossSection_h__

/** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
extern const Type lucMeshCrossSection_Type;

/** Class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
#define __lucMeshCrossSection \
      /* Macro defining parent goes here - This means you can cast this class as its parent */ \
      __lucCrossSection \
      /* Virtual functions go here */ \
      /* Other info */\
      ExtensionInfo_Index     vertexGridHandle;       \
      int                     dim[3];                 \
      int                     fieldDim;               \
 
struct lucMeshCrossSection
{
   __lucMeshCrossSection
};

/** Private Constructor: This will accept all the virtual functions for this class as arguments. */

#ifndef ZERO
#define ZERO 0
#endif

#define LUCMESHCROSSSECTION_DEFARGS \
                LUCCROSSSECTION_DEFARGS

#define LUCMESHCROSSSECTION_PASSARGS \
                LUCCROSSSECTION_PASSARGS

lucMeshCrossSection* _lucMeshCrossSection_New(  LUCMESHCROSSSECTION_DEFARGS  );

void _lucMeshCrossSection_Delete( void* drawingObject ) ;
void _lucMeshCrossSection_Print( void* drawingObject, Stream* stream ) ;

/* 'Stg_Component' implementations */
void* _lucMeshCrossSection_DefaultNew( Name name ) ;
void _lucMeshCrossSection_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data );
void _lucMeshCrossSection_Build( void* drawingObject, void* data ) ;
void _lucMeshCrossSection_Initialise( void* drawingObject, void* data ) ;
void _lucMeshCrossSection_Execute( void* drawingObject, void* data );
void _lucMeshCrossSection_Destroy( void* drawingObject, void* data ) ;

void _lucMeshCrossSection_Draw( void* drawingObject, lucDatabase* database, void* _context );

void lucMeshCrossSection_Sample(void* drawingObject, Bool reverse);

#endif

