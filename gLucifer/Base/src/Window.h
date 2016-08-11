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

#ifndef __lucWindow_h__
#define __lucWindow_h__

typedef void (lucWindow_DisplayFunction) ( void* object );
typedef int (lucWindow_EventsWaitingFunction) ( void* object );
typedef Bool (lucWindow_EventProcessorFunction) ( void* object );
typedef void (lucWindow_ResizeFunction) ( void* object );

lucDatabase* defaultDatabase;

extern const Type lucWindow_Type;

#define __lucWindow                                                     \
      __Stg_Component                                                   \
      /* Virtual Functions */                                           \
      lucWindow_DisplayFunction*          _displayWindow;               \
                                                                        \
      /* Other Info */                                                  \
      lucViewport**                       viewportList;                 \
      Viewport_Index                      viewportCount;                \
      int*                                viewportLayout;               \
      Pixel_Index                         width;                        \
      Pixel_Index                         height;                       \
      lucColour                           backgroundColour;             \
      DomainContext*                      context;                      \
      Bool                                isMaster;                     \
      char*                               title;                        \
      Bool                                antialias;                    \
      lucDatabase*                        database;                     \
      Bool                                useModelBounds;               \
      Bool                                disabled;                     \
   
struct lucWindow
{
   __lucWindow
};


#ifndef ZERO
#define ZERO 0
#endif

#define LUCWINDOW_DEFARGS \
                STG_COMPONENT_DEFARGS, \
                lucWindow_DisplayFunction*          _displayWindow

#define LUCWINDOW_PASSARGS \
                STG_COMPONENT_PASSARGS, \
           _displayWindow

lucWindow* _lucWindow_New(  LUCWINDOW_DEFARGS  );

void _lucWindow_Delete( void* window ) ;
void _lucWindow_Print( void* window, Stream* stream ) ;
void* _lucWindow_Copy( void* window, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) ;

/* Stg_Component Virtual Functions */
void* _lucWindow_DefaultNew( Name name ) ;
void _lucWindow_AssignFromXML( void* window, Stg_ComponentFactory* cf, void* data ) ;
void _lucWindow_Build( void* window, void* data ) ;
void _lucWindow_Initialise( void* window, void* data ) ;
void _lucWindow_Execute( void* window, void* data ) ;
void _lucWindow_Destroy( void* window, void* data ) ;

/* Window Virtual Functions */
void lucWindow_Display( void* window );

/* +++ Public Functions +++ */
void lucWindow_CleanUp( void* window, void* context ) ;

lucViewport** lucWindow_ConstructViewportList(
   lucWindow* self,
   Stg_ComponentFactory* cf,
   Viewport_Index* viewportCount,
   void* data ) ;

#endif

