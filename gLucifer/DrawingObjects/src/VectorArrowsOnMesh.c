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

#include <gLucifer/Base/Base.h>


#include "types.h"
#include <gLucifer/Base/DrawingObject.h>
#include "VectorArrowMeshCrossSection.h"
#include "VectorArrowsOnMesh.h"

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type lucVectorArrowsOnMesh_Type = "lucVectorArrowsOnMesh";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
lucVectorArrowsOnMesh* _lucVectorArrowsOnMesh_New(  LUCVECTORARROWSONMESH_DEFARGS  )
{
   lucVectorArrowsOnMesh*					self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucVectorArrowsOnMesh) );
   self = (lucVectorArrowsOnMesh*) _lucVectorArrowMeshCrossSection_New(  LUCVECTORARROWMESHCROSSSECTION_PASSARGS  );

   return self;
}

void _lucVectorArrowsOnMesh_Init( lucVectorArrowsOnMesh* self)
{
}

void _lucVectorArrowsOnMesh_Delete( void* drawingObject )
{
   lucVectorArrowsOnMesh*  self = (lucVectorArrowsOnMesh*)drawingObject;

   _lucVectorArrowMeshCrossSection_Delete( self );
}

void _lucVectorArrowsOnMesh_Print( void* drawingObject, Stream* stream )
{
   lucVectorArrowsOnMesh*  self = (lucVectorArrowsOnMesh*)drawingObject;

   _lucVectorArrowMeshCrossSection_Print( self, stream );
}

void* _lucVectorArrowsOnMesh_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof(lucVectorArrowsOnMesh);
   Type                                                             type = lucVectorArrowsOnMesh_Type;
   Stg_Class_DeleteFunction*                                     _delete = _lucVectorArrowsOnMesh_Delete;
   Stg_Class_PrintFunction*                                       _print = _lucVectorArrowsOnMesh_Print;
   Stg_Class_CopyFunction*                                         _copy = NULL;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _lucVectorArrowsOnMesh_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _lucVectorArrowsOnMesh_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _lucVectorArrowMeshCrossSection_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _lucVectorArrowMeshCrossSection_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _lucVectorArrowMeshCrossSection_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _lucVectorArrowMeshCrossSection_Destroy;
   lucDrawingObject_SetupFunction*                                _setup = _lucCrossSection_Setup;
   lucDrawingObject_DrawFunction*                                  _draw = _lucVectorArrowsOnMesh_Draw;
   lucDrawingObject_CleanUpFunction*                            _cleanUp = lucDrawingObject_CleanUp;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _lucVectorArrowsOnMesh_New(  LUCVECTORARROWSONMESH_PASSARGS  );
}

void _lucVectorArrowsOnMesh_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data )
{
   lucVectorArrowsOnMesh* self = (lucVectorArrowsOnMesh*)drawingObject;

   /* Construct Parent */
   _lucVectorArrowMeshCrossSection_AssignFromXML( self, cf, data );

   _lucVectorArrowsOnMesh_Init( self );
}

void _lucVectorArrowsOnMesh_Draw( void* drawingObject, lucDatabase* database, void* _context )
{
   lucVectorArrowsOnMesh*       self    = (lucVectorArrowsOnMesh*)drawingObject;
   DomainContext* context         = (DomainContext*) _context;
   Dimension_Index        dim     = context->dim;

   if ( dim == 2 )
   {
      _lucVectorArrowMeshCrossSection_DrawCrossSection( lucCrossSection_Set(self, 0, K_AXIS, False), database, dim);
   }
   else
   {
      int idx;
      int zres = Dictionary_GetInt( context->CF->rootDict, (Dictionary_Entry_Key)"elementResK" );
      for ( idx=0; idx <= zres; idx++)
      {
         _lucVectorArrowMeshCrossSection_DrawCrossSection( lucCrossSection_Set(self, idx, K_AXIS, False), database, dim);
      }
   }
}


