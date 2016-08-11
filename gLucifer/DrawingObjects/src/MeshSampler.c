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

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>

#include <gLucifer/Base/Base.h>


#include "types.h"
#include <gLucifer/Base/DrawingObject.h>
#include "MeshSampler.h"

void lucMeshSampler_SampleLocal( void* drawingObject, void* _context );
void lucMeshSampler_SampleGlobal( void* drawingObject, void* _context );

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type lucMeshSampler_Type = "lucMeshSampler";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
lucMeshSampler* _lucMeshSampler_New(  LUCMESHSAMPLER_DEFARGS  )
{
   lucMeshSampler*               self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucMeshSampler) );
   self = (lucMeshSampler*) _lucMeshCrossSection_New(  LUCMESHCROSSSECTION_PASSARGS  );

   return self;
}

void _lucMeshSampler_Init(lucMeshSampler*           self)
{
   self->elementRes[I_AXIS] = Dictionary_GetInt( self->context->CF->rootDict, (Dictionary_Entry_Key)"elementResI"  );
   self->elementRes[J_AXIS] = Dictionary_GetInt( self->context->CF->rootDict, (Dictionary_Entry_Key)"elementResJ"  );
   self->elementRes[K_AXIS] = Dictionary_GetInt( self->context->CF->rootDict, (Dictionary_Entry_Key)"elementResK"  );
}

void _lucMeshSampler_Delete( void* drawingObject )
{
   lucMeshSampler*  self = (lucMeshSampler*)drawingObject;
   _lucMeshCrossSection_Delete( self );
}

void _lucMeshSampler_Print( void* drawingObject, Stream* stream )
{
   lucMeshSampler*  self = (lucMeshSampler*)drawingObject;

   _lucMeshCrossSection_Print( self, stream );
}

void* _lucMeshSampler_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof(lucMeshSampler);
   Type                                                             type = lucMeshSampler_Type;
   Stg_Class_DeleteFunction*                                     _delete = _lucMeshSampler_Delete;
   Stg_Class_PrintFunction*                                       _print = _lucMeshSampler_Print;
   Stg_Class_CopyFunction*                                         _copy = NULL;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _lucMeshSampler_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _lucMeshSampler_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _lucMeshSampler_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _lucMeshSampler_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _lucMeshSampler_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _lucMeshSampler_Destroy;
   lucDrawingObject_SetupFunction*                                _setup = _lucCrossSection_Setup;
   lucDrawingObject_DrawFunction*                                  _draw = _lucMeshSampler_Draw;
   lucDrawingObject_CleanUpFunction*                            _cleanUp = lucDrawingObject_CleanUp;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _lucMeshSampler_New(  LUCMESHSAMPLER_PASSARGS  );
}

void _lucMeshSampler_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data )
{
   lucMeshSampler*         self               = (lucMeshSampler*)drawingObject;
   Index                  defaultRes;

   /* Construct Parent */
   _lucMeshCrossSection_AssignFromXML( self, cf, data );

   _lucMeshSampler_Init(self);

   /* No lighting */
   self->lit = False;
}

void _lucMeshSampler_Build( void* drawingObject, void* data ) 
{
   /* Build field variable in parent */
   _lucMeshCrossSection_Build(drawingObject, data);
}

void _lucMeshSampler_Initialise( void* drawingObject, void* data ) 
{
   lucMeshSampler*  self = (lucMeshSampler*)drawingObject;
   /* Calculate number of samples */
   self->total = (self->elementRes[I_AXIS]+1) * (self->elementRes[J_AXIS]+1) * (self->elementRes[K_AXIS]+1);
}

void _lucMeshSampler_Execute( void* drawingObject, void* data ) {}
void _lucMeshSampler_Destroy( void* drawingObject, void* data ) {}

void lucMeshSampler_DrawSlice(lucMeshSampler* self, lucDatabase* database)
{
   /* Corners */
   float corners[6] = {self->min[0], self->min[1], self->min[2],
                       self->max[0], self->max[1], self->max[2]};

   /* Sample the 2d cross-section */
   lucMeshCrossSection_Sample(self, False);

   if (self->context->rank == 0 && database)
   {
      /* Write slice values on root processor */
      lucDatabase_AddVolumeSlice(database, self->elementRes[I_AXIS]+1, self->elementRes[J_AXIS]+1, corners, self->colourMap, &self->values[0][0][0]);
   }

   lucCrossSection_FreeSampleData(self);

   /* Start new geometry section - when used with multiple sections */
   lucDatabase_OutputGeometry(database, self->id);
}

void _lucMeshSampler_Draw( void* drawingObject, lucDatabase* database, void* _context )
{
   lucMeshSampler* self          = (lucMeshSampler*)drawingObject;
   DomainContext* context         = (DomainContext*) _context;
   Dimension_Index        dim     = context->dim;

   if (dim == 2)
   {
      lucMeshSampler_DrawSlice(lucCrossSection_Slice(self, 0.0, False), database);
   }
   else
   {
      int idx;
      for ( idx=0; idx <= self->elementRes[K_AXIS]; idx++)
      {
         lucMeshSampler_DrawSlice(lucCrossSection_Slice(self, idx, False), database);
      }
   }
}

