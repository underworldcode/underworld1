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
#include <StgFEM/StgFEM.h>

#include <gLucifer/Base/Base.h>


#include "types.h"
#include <gLucifer/Base/DrawingObject.h>
#include "ScalarFieldOnMeshCrossSection.h"
#include "ScalarFieldOnMesh.h"

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type lucScalarFieldOnMesh_Type = "lucScalarFieldOnMesh";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
lucScalarFieldOnMesh* _lucScalarFieldOnMesh_New(  LUCSCALARFIELDONMESH_DEFARGS  )
{
   lucScalarFieldOnMesh*					self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucScalarFieldOnMesh) );
   self = (lucScalarFieldOnMesh*) _lucScalarFieldOnMeshCrossSection_New(  LUCSCALARFIELDONMESHCROSSSECTION_PASSARGS  );

   return self;
}

void _lucScalarFieldOnMesh_Init(lucScalarFieldOnMesh* self, char* drawSides)
{
   DomainContext* context = (DomainContext*) self->context;
   int i;
   for (i=0; i<3; i++) {
      self->drawSides[i][0] = True;
      self->drawSides[i][1] = True;
   } 
   /* Specify axis-aligned sides that should be plotted with character string representing axis */
   /* Small letter represents minimum, large maximum */
   if (!strchr(drawSides, 'x')) self->drawSides[I_AXIS][0] = False;
   if (!strchr(drawSides, 'X')) self->drawSides[I_AXIS][1] = False;
   if (!strchr(drawSides, 'y')) self->drawSides[J_AXIS][0] = False;
   if (!strchr(drawSides, 'Y')) self->drawSides[J_AXIS][1] = False;
   if (!strchr(drawSides, 'z')) self->drawSides[K_AXIS][0] = False;
   if (!strchr(drawSides, 'Z')) self->drawSides[K_AXIS][1] = False;

   /* Set cull face to on */
   if (context->dim == 2)
      self->cullface = False;
   else
      self->cullface = True;
}


void _lucScalarFieldOnMesh_Delete( void* drawingObject )
{
   lucScalarFieldOnMesh*  self = (lucScalarFieldOnMesh*)drawingObject;

   _lucScalarFieldOnMeshCrossSection_Delete( self );
}

void _lucScalarFieldOnMesh_Print( void* drawingObject, Stream* stream )
{
   lucScalarFieldOnMesh*  self = (lucScalarFieldOnMesh*)drawingObject;

   _lucScalarFieldOnMeshCrossSection_Print( self, stream );
}

void* _lucScalarFieldOnMesh_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof(lucScalarFieldOnMesh);
   Type                                                             type = lucScalarFieldOnMesh_Type;
   Stg_Class_DeleteFunction*                                     _delete = _lucScalarFieldOnMesh_Delete;
   Stg_Class_PrintFunction*                                       _print = _lucScalarFieldOnMesh_Print;
   Stg_Class_CopyFunction*                                         _copy = NULL;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _lucScalarFieldOnMesh_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _lucScalarFieldOnMesh_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _lucScalarFieldOnMesh_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _lucScalarFieldOnMeshCrossSection_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _lucScalarFieldOnMeshCrossSection_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _lucScalarFieldOnMeshCrossSection_Destroy;
   lucDrawingObject_SetupFunction*                                _setup = _lucCrossSection_Setup;
   lucDrawingObject_DrawFunction*                                  _draw = _lucScalarFieldOnMesh_Draw;
   lucDrawingObject_CleanUpFunction*                            _cleanUp = lucDrawingObject_CleanUp;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _lucScalarFieldOnMesh_New(  LUCSCALARFIELDONMESH_PASSARGS  );
}

void _lucScalarFieldOnMesh_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data )
{
   lucScalarFieldOnMesh*  self = (lucScalarFieldOnMesh*)drawingObject;

   /* Construct Parent */
   _lucScalarFieldOnMeshCrossSection_AssignFromXML( self, cf, data );

   _lucScalarFieldOnMesh_Init(self, Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"drawSides", "xyzXYZ"));
}

void _lucScalarFieldOnMesh_Build( void* drawingObject, void* data )
{
   lucScalarFieldOnMeshCrossSection*     self    = (lucScalarFieldOnMeshCrossSection*)drawingObject;
   FeVariable*                     feVariable;

   /* Build parent */
   _lucScalarFieldOnMeshCrossSection_Build(self, data);

   /* Drawing object default overrides */
   feVariable = (FeVariable*) self->fieldVariable;
   if (feVariable->dim == 2) self->lit = False;
}

void _lucScalarFieldOnMesh_Draw( void* drawingObject, lucDatabase* database, void* _context )
{
   lucScalarFieldOnMesh*  self          = (lucScalarFieldOnMesh*)drawingObject;
   FeVariable*            fieldVariable = (FeVariable*) self->fieldVariable;
   Mesh*                  mesh          = (Mesh*) fieldVariable->feMesh;
   Grid*                  vertGrid;

   vertGrid = *(Grid**)ExtensionManager_Get( mesh->info, mesh, self->vertexGridHandle );

   if (fieldVariable->dim == 2)
   {
      lucScalarFieldOnMeshCrossSection_DrawCrossSection( lucCrossSection_Set(self, 0.0, K_AXIS, False), database, False);
   }
   else
   {
      /* Cross sections at minimums, default winding for faces */
      self->flipNormals = False;
      if (self->drawSides[I_AXIS][0])
         lucScalarFieldOnMeshCrossSection_DrawCrossSection( lucCrossSection_Set(self, 0.0, I_AXIS, False), database, False);
      if (self->drawSides[J_AXIS][0])
         lucScalarFieldOnMeshCrossSection_DrawCrossSection( lucCrossSection_Set(self, 0.0, J_AXIS, False), database, False);
      if (self->drawSides[K_AXIS][0])
         lucScalarFieldOnMeshCrossSection_DrawCrossSection( lucCrossSection_Set(self, 0.0, K_AXIS, False), database, False);

      /* Cross sections at maximums, reverse winding for faces and flip normals to face backwards */
      self->flipNormals = True;
      if (self->drawSides[I_AXIS][1])
         lucScalarFieldOnMeshCrossSection_DrawCrossSection( lucCrossSection_Set(self, vertGrid->sizes[ I_AXIS ] - 1, I_AXIS, False), database, True);
      if (self->drawSides[J_AXIS][1])
         lucScalarFieldOnMeshCrossSection_DrawCrossSection( lucCrossSection_Set(self, vertGrid->sizes[ J_AXIS ] - 1, J_AXIS, False), database, True);
      if (self->drawSides[K_AXIS][1])
         lucScalarFieldOnMeshCrossSection_DrawCrossSection( lucCrossSection_Set(self, vertGrid->sizes[ K_AXIS ] - 1, K_AXIS, False), database, True);
   }
}



