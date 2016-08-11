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

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>

#include "types.h"
#include "ColourMap.h"
#include "Window.h"
#include "DrawingObject_Register.h"

#include "Viewport.h"

#include "DrawingObject.h"
#include "Camera.h"
#include "Init.h"

const Type lucViewport_Type = "lucViewport";

lucViewport* _lucViewport_New(  LUCVIEWPORT_DEFARGS  )
{
   lucViewport*    self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucViewport) );
   /* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
   /* This means that any values of these parameters that are passed into this function are not passed onto the parent function
      and so should be set to ZERO in any children of this class. */
   nameAllocationType = NON_GLOBAL;

   self = (lucViewport*) _Stg_Component_New(  STG_COMPONENT_PASSARGS  );

   return self;
}

void _lucViewport_Init(
   lucViewport*                  self,
   lucCamera*                    camera,
   lucDrawingObject**            drawingObjectList,
   DrawingObject_Index           drawingObjectCount,
   char*                         title,
   Bool                          axis,
   double                        axisLength,
   Bool                          antialias,
   Bool                          rulers,
   Bool                          timestep,
   int                           border,
   Name                          borderColourName,
   Bool                          disabled,
   Pixel_Index                   margin,
   double                        nearClipPlane,
   double                        farClipPlane,
   double                        scaleX,
   double                        scaleY,
   double                        scaleZ)
{
   DrawingObject_Index object_I;

   self->camera                   = camera;
   self->title                    = StG_Strdup( title );
   self->axis                     = axis;
   self->axisLength               = axisLength;
   self->antialias                = antialias;
   self->rulers                   = rulers;
   self->timestep                 = timestep;
   self->border                   = border;
   self->margin                   = margin;
   self->nearClipPlane            = nearClipPlane;
   self->farClipPlane             = farClipPlane;
   self->scaleX                   = scaleX;
   self->scaleY                   = scaleY;
   self->scaleZ                   = scaleZ;
   self->disabled = disabled || !self->context->vis;

   lucColour_FromString( &self->borderColour, borderColourName );

   self->drawingObject_Register = lucDrawingObject_Register_New();

   for ( object_I = 0 ; object_I < drawingObjectCount ; object_I++ )
      lucDrawingObject_Register_Add( self->drawingObject_Register, drawingObjectList[ object_I ] );
}

void _lucViewport_Delete( void* viewport )
{
   lucViewport* self        = viewport;

   if (self->properties) Memory_Free(self->properties);

   _Stg_Component_Delete( self );
}

void _lucViewport_Print( void* viewport, Stream* stream )
{
   lucViewport*          self        = viewport;

   Journal_Printf( stream, "lucViewport: %s\n", self->name );

   Stream_Indent( stream );

   /* Print Parent */
   _Stg_Component_Print( self, stream );

   lucDrawingObject_Register_PrintAllObjects( self->drawingObject_Register, stream );

   Stg_Class_Print( self->camera, stream );

   Journal_PrintValue( stream, self->margin );
   Journal_PrintValue( stream, self->nearClipPlane );
   Journal_PrintValue( stream, self->farClipPlane );

   Journal_PrintString( stream, self->title );
   Journal_PrintBool( stream, self->timestep );
   Journal_PrintBool( stream, self->disabled );

   Stream_UnIndent( stream );
}

void* _lucViewport_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof( lucViewport );
   Type                                                      type = lucViewport_Type;
   Stg_Class_DeleteFunction*                              _delete = _lucViewport_Delete;
   Stg_Class_PrintFunction*                                _print = _lucViewport_Print;
   Stg_Class_CopyFunction*                                  _copy = NULL;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _lucViewport_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _lucViewport_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _lucViewport_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _lucViewport_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _lucViewport_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _lucViewport_Destroy;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return _lucViewport_New(  LUCVIEWPORT_PASSARGS  );
}

void _lucViewport_AssignFromXML( void* viewport, Stg_ComponentFactory* cf, void* data )
{
   lucViewport*        self               = (lucViewport*) viewport;
   DrawingObject_Index drawingObjectCount;
   lucDrawingObject**  drawingObjectList;
   lucCamera*          camera;

   /* TODO Construct Parent */

   self->context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", AbstractContext, False, data );
   if ( !self->context  )
      self->context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", AbstractContext, True, data  );

   camera =  Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Camera", lucCamera, True, data  ) ;

   drawingObjectList = Stg_ComponentFactory_ConstructByList( cf, self->name, (Dictionary_Entry_Key)"DrawingObject", Stg_ComponentFactory_Unlimited, lucDrawingObject, True, &drawingObjectCount, data  );

   _lucViewport_Init(
      self,
      camera,
      drawingObjectList,
      drawingObjectCount,
      Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"title", ""),
      Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"axis", False  ),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"axisLength", 0.2 ),
      Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"antialias", True  ),
      Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"rulers", False  ),
      Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"timestep", False  ),
      Stg_ComponentFactory_GetInt( cf, self->name, (Dictionary_Entry_Key)"border", 0),
      Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"borderColour", "#888888"  ),
      Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"disable", False  ),
      Stg_ComponentFactory_GetInt( cf, self->name, (Dictionary_Entry_Key)"margin", 32),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"nearClipPlane", 0 ),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"farClipPlane", 0 ),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"scaleX", 1.0  ),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"scaleY", 1.0  ),
      Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"scaleZ", 1.0 ));

   Memory_Free( drawingObjectList );
}

void _lucViewport_Build( void* viewport, void* data ) { }
void _lucViewport_Initialise( void* viewport, void* data ) {}
void _lucViewport_Execute( void* viewport, void* data ) { }
void _lucViewport_Destroy( void* viewport, void* data ) { }

void lucViewport_Draw( void* viewport, lucDatabase* database )
{
   lucViewport*          self = (lucViewport*) viewport ;
   if (self->disabled) return;

   lucDrawingObject_Register_DrawAll( self->drawingObject_Register, database);
}

void lucViewport_CleanUp( void* viewport, void* context )
{
   lucViewport*          self = (lucViewport*) viewport ;

   lucDrawingObject_Register_CleanUpAll( self->drawingObject_Register, context );
}

void lucViewport_Update( void* viewport )
{
   lucViewport*       self = (lucViewport*) viewport ;

   /* Write property string */
   if (!self->properties)
   {
      self->properties = Memory_Alloc_Array(char, 2048, "properties");
      sprintf(self->properties, "title=%s\naxis=%d\naxislength=%g\nantialias=%d\nrulers=%d\ntimestep=%d\nborder=%d\nbordercolour=%d\nzoomstep=%d\nmargin=%d\n", self->title, self->axis, self->axisLength, self->antialias, self->rulers, self->timestep, self->border, lucColour_ToInt(&self->borderColour), self->camera->autoZoomTimestep, self->margin);
   }
}

void lucViewport_Setup( void* viewport, lucDatabase* database )
{
   lucViewport*       self = (lucViewport*) viewport ;
   lucDrawingObject_Register_SetupAll( self->drawingObject_Register, database );
}

