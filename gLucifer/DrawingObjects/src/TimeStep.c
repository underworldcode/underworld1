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


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>

#include <gLucifer/Base/Base.h>


#ifdef HAVE_GL2PS
#include <gl2ps.h>
#endif

#include "types.h"
#include <gLucifer/Base/DrawingObject.h>
#include "TimeStep.h"

const Type lucTimeStep_Type = "lucTimeStep";

lucTimeStep* _lucTimeStep_New(  LUCTIMESTEP_DEFARGS  )
{
   lucTimeStep*    self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucTimeStep) );
   self = (lucTimeStep*)  _lucDrawingObject_New(  LUCDRAWINGOBJECT_PASSARGS  );


   return self;
}

void _lucTimeStep_Init(
   lucTimeStep*                                         self,
   Bool                                              frame,
   Bool                                              showtime,
   char*                                             units)
{
   self->frame = frame;
   self->showtime = showtime;
   self->units = StG_Strdup( units );
}

void _lucTimeStep_Delete( void* drawingObject )
{
   lucTimeStep*  self = (lucTimeStep*)drawingObject;

   _lucDrawingObject_Delete( self );
}

void _lucTimeStep_Print( void* drawingObject, Stream* stream )
{
   lucTimeStep*  self = (lucTimeStep*)drawingObject;

   _lucDrawingObject_Print( self, stream );
}

void* _lucTimeStep_Copy( void* timeStep, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap )
{
   lucTimeStep* self        = timeStep;
   lucTimeStep* newTimeStep;

   newTimeStep = _Stg_Component_Copy( self, dest, deep, nameExt, ptrMap );

   memcpy( &(newTimeStep->colour),       &(self->colour),       sizeof(lucColour) );
   newTimeStep->frame = self->frame;
   newTimeStep->showtime = self->showtime;

   return (void*) newTimeStep;
}

void* _lucTimeStep_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof( lucTimeStep );
   Type                                                             type = lucTimeStep_Type;
   Stg_Class_DeleteFunction*                                     _delete = _lucTimeStep_Delete;
   Stg_Class_PrintFunction*                                       _print = _lucTimeStep_Print;
   Stg_Class_CopyFunction*                                         _copy = _lucTimeStep_Copy;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _lucTimeStep_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _lucTimeStep_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _lucTimeStep_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _lucTimeStep_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _lucTimeStep_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _lucTimeStep_Destroy;
   lucDrawingObject_SetupFunction*                                _setup = lucDrawingObject_Setup;
   lucDrawingObject_DrawFunction*                                  _draw = _lucTimeStep_Draw;
   lucDrawingObject_CleanUpFunction*                            _cleanUp = lucDrawingObject_CleanUp;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return _lucTimeStep_New(  LUCTIMESTEP_PASSARGS  );
}

void _lucTimeStep_AssignFromXML( void* timeStep, Stg_ComponentFactory* cf, void* data )
{
   lucTimeStep*             self               = (lucTimeStep*) timeStep;
   Bool frame;
   Bool currentTime;
   char* units;

   /* Construct Parent */
   _lucDrawingObject_AssignFromXML( self, cf, data );

   frame  = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"frame", True ) ;
   currentTime = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"time", False ) ;
   units = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"outputUnits", ""),

   _lucTimeStep_Init( self, frame, currentTime, units);

}

void _lucTimeStep_Build( void* TimeStep, void* data ) { }
void _lucTimeStep_Initialise( void* TimeStep, void* data ) { }
void _lucTimeStep_Execute( void* TimeStep, void* data ) { }
void _lucTimeStep_Destroy( void* TimeStep, void* data ) { }

void _lucTimeStep_Draw( void* drawingObject, lucDatabase* database, void* _context )
{
   /* Deprecated: Time step now option on viewport */
}





