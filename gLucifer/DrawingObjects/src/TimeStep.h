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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/



#ifndef __lucTimeStep_h__
#define __lucTimeStep_h__

extern const Type lucTimeStep_Type;

#define __lucTimeStep                              \
		__lucDrawingObject \
		Bool                                frame;   \
		Bool                                showtime;    \
		char*                               units;    \

struct lucTimeStep
{
   __lucTimeStep
};

#ifndef ZERO
#define ZERO 0
#endif

#define LUCTIMESTEP_DEFARGS \
                LUCDRAWINGOBJECT_DEFARGS

#define LUCTIMESTEP_PASSARGS \
                LUCDRAWINGOBJECT_PASSARGS

lucTimeStep* _lucTimeStep_New(  LUCTIMESTEP_DEFARGS  );

void _lucTimeStep_Init(
   lucTimeStep*                                       self,
   Bool                                               frame,
   Bool                                               showtime,
   char*                                              units);


/** Virtual Functions */
void _lucTimeStep_Delete( void* timeStep ) ;
void _lucTimeStep_Print( void* timeStep, Stream* stream ) ;
void* _lucTimeStep_Copy( void* timeStep, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) ;
void* _lucTimeStep_DefaultNew( Name name ) ;
void _lucTimeStep_AssignFromXML( void* timeStep, Stg_ComponentFactory* cf, void* data ) ;
void _lucTimeStep_Build( void* timeStep, void* data );
void _lucTimeStep_Initialise( void* timeStep, void* data );
void _lucTimeStep_Execute( void* timeStep, void* data );
void _lucTimeStep_Destroy( void* timeStep, void* data );
void _lucTimeStep_Draw( void* drawingObject, lucDatabase* database, void* _context );

#endif

