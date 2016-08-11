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



#ifndef __lucAxis_h__
#define __lucAxis_h__

extern const Type lucAxis_Type;

#define __lucAxis                              \
      __lucDrawingObject \
      Coord                               origin;\
      float                               length;\
      Bool                                axes[3];\
      char*                               labels[3];\
      lucColour                           colours[3];\

struct lucAxis
{
   __lucAxis
};

#ifndef ZERO
#define ZERO 0
#endif

#define LUCAXIS_DEFARGS \
                LUCDRAWINGOBJECT_DEFARGS

#define LUCAXIS_PASSARGS \
                LUCDRAWINGOBJECT_PASSARGS

lucAxis* _lucAxis_New(  LUCAXIS_DEFARGS  );

/** Virtual Functions */
void _lucAxis_Delete( void* axis ) ;
void _lucAxis_Print( void* axis, Stream* stream ) ;
void* _lucAxis_DefaultNew( Name name ) ;
void _lucAxis_AssignFromXML( void* axis, Stg_ComponentFactory* cf, void* data ) ;
void _lucAxis_Build( void* axis, void* data );
void _lucAxis_Initialise( void* axis, void* data );
void _lucAxis_Execute( void* axis, void* data );
void _lucAxis_Destroy( void* axis, void* data );
void _lucAxis_Draw( void* drawingObject, lucDatabase* database, void* _context );

#endif

