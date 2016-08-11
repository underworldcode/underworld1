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


#include <ctype.h>
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>

#include <gLucifer/Base/Base.h>


#include "types.h"
#include <gLucifer/Base/DrawingObject.h>
#include "CrossSection.h"

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type lucCrossSection_Type = "lucCrossSection";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
lucCrossSection* _lucCrossSection_New(  LUCCROSSSECTION_DEFARGS  )
{
   lucCrossSection*					self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucCrossSection) );
   self = (lucCrossSection*) _lucDrawingObject_New(  LUCDRAWINGOBJECT_PASSARGS  );

   return self;
}

void _lucCrossSection_Init(
   lucCrossSection*        self,
   Index                   resolutionA,
   Index                   resolutionB,
   XYZ                     coord1,
   XYZ                     coord2,
   XYZ                     coord3,
   double                  value,
   Axis                    axis,
   Bool                    interpolate)
{
   Journal_Firewall( resolutionA > 1 && resolutionB > 1, lucError,
                     "Error - in %s(): Resolution below 2x2 ==> %d x %d\n", __func__, resolutionA, resolutionB);
   self->resolutionA = resolutionA;
   self->resolutionB = resolutionB;
   memcpy( self->coord1, coord1, sizeof(XYZ) );
   memcpy( self->coord2, coord2, sizeof(XYZ) );
   memcpy( self->coord3, coord3, sizeof(XYZ) );
   self->value = value;
   self->axis = axis;
   self->interpolate = interpolate;
   self->gatherData = True;   /* Default behaviour for sampling is to gather all data on root proc */
   self->offsetEdges = False; /* Pushes min/max edges by half of sample size to avoid sampling boundaries */
   self->values = NULL;
   self->vertices = NULL;
   /* Set cull face to off - default */
   self->cullface = False;
}

void _lucCrossSection_Delete( void* drawingObject )
{
   lucCrossSection*  self = (lucCrossSection*)drawingObject;

   _lucDrawingObject_Delete( self );
}

void _lucCrossSection_Print( void* drawingObject, Stream* stream )
{
   lucCrossSection*  self = (lucCrossSection*)drawingObject;
   _lucDrawingObject_Print( self, stream );
}

void* _lucCrossSection_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof(lucCrossSection);
   Type                                                             type = lucCrossSection_Type;
   Stg_Class_DeleteFunction*                                     _delete = _lucCrossSection_Delete;
   Stg_Class_PrintFunction*                                       _print = _lucCrossSection_Print;
   Stg_Class_CopyFunction*                                         _copy = NULL;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _lucCrossSection_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _lucCrossSection_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _lucCrossSection_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _lucCrossSection_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _lucCrossSection_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _lucCrossSection_Destroy;
   lucDrawingObject_SetupFunction*                                _setup = _lucCrossSection_Setup;
   lucDrawingObject_DrawFunction*                                  _draw = _lucCrossSection_Draw;
   lucDrawingObject_CleanUpFunction*                            _cleanUp = lucDrawingObject_CleanUp;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _lucCrossSection_New(  LUCCROSSSECTION_PASSARGS  );
}

void _lucCrossSection_AssignFromXML( void* drawingObject, Stg_ComponentFactory* cf, void* data )
{
   lucCrossSection*     self = (lucCrossSection*)drawingObject;
   Name crossSectionStr;
   char axisChar;
   char crossSectionVal[20];
   char modifierChar = ' ';
   double value = 0.0;
   Axis axis = I_AXIS;
   Bool interpolate = False;
   XYZ  coord1;
   XYZ  coord2;
   XYZ  coord3;

   /* Construct Parent */
   _lucDrawingObject_AssignFromXML( self, cf, data );

   /* Get Values which define the cross section 
    * if not specified, use axis parameter instead */
   if (Stg_ComponentFactory_TryDouble( cf, self->name, (Dictionary_Entry_Key)"X1", &coord1[ I_AXIS ] ) &&
       Stg_ComponentFactory_TryDouble( cf, self->name, (Dictionary_Entry_Key)"Y1", &coord1[ J_AXIS ] ) &&
       Stg_ComponentFactory_TryDouble( cf, self->name, (Dictionary_Entry_Key)"Z1", &coord1[ K_AXIS ] ) &&
       Stg_ComponentFactory_TryDouble( cf, self->name, (Dictionary_Entry_Key)"X2", &coord2[ I_AXIS ] ) &&
       Stg_ComponentFactory_TryDouble( cf, self->name, (Dictionary_Entry_Key)"Y2", &coord2[ J_AXIS ] ) &&
       Stg_ComponentFactory_TryDouble( cf, self->name, (Dictionary_Entry_Key)"Z2", &coord2[ K_AXIS ] ) &&
       Stg_ComponentFactory_TryDouble( cf, self->name, (Dictionary_Entry_Key)"X3", &coord3[ I_AXIS ] ) &&
       Stg_ComponentFactory_TryDouble( cf, self->name, (Dictionary_Entry_Key)"Y3", &coord3[ J_AXIS ] ) &&
       Stg_ComponentFactory_TryDouble( cf, self->name, (Dictionary_Entry_Key)"Z3", &coord3[ K_AXIS ] ))
   {
      /* No axis alignment, using specified coords to define a cross-section plane */
      self->axisAligned = False;
   }
   else
   {
      self->axisAligned = True;

      /* If any of these direct value parameters are set use them instead of parsing the crossSection string
       * (added to allow scaling to work as a value in a string will not have scaling applied) */
      if (Stg_ComponentFactory_TryDouble( cf, self->name, (Dictionary_Entry_Key)"crossSectionX", &value ))
         axis = I_AXIS;
      else if (Stg_ComponentFactory_TryDouble( cf, self->name, (Dictionary_Entry_Key)"crossSectionY", &value ))
         axis = J_AXIS;
      else if (Stg_ComponentFactory_TryDouble( cf, self->name, (Dictionary_Entry_Key)"crossSectionZ", &value ))
         axis = K_AXIS;
      else
      {
         /* Interpolate between max and min value using provided value */
         interpolate = True;
         /* Read the cross section string specification */
         crossSectionStr = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"crossSection", "z=0" );

         /* axis=value    : draw at min + value * range, ie: x=0.25 will be a quarter along the x range
          * axis=min      : draw at minimum of range on axis
          * axis=max      : draw at maximum of range on axis
          * axis=value%   : draw at interpolated percentage value of range on axis
          * Axis is a single character, one of [xyzXYZ] */

         /* Parse the input string */
         if (strlen(crossSectionStr) > 0 && sscanf( crossSectionStr, "%c=%s", &axisChar, crossSectionVal ) == 2 )
         {
            /* Axis X/Y/Z */
            if ( toupper( axisChar ) >= 'X' )
               axis = toupper( axisChar ) - 'X';   /* x=0 y=1 z=2 */

            if (sscanf( crossSectionVal, "%lf%c", &value, &modifierChar) >= 1)
            {
               /* Found a numeric value  + optional modifier character */
               //fprintf(stderr, "CROSS SECTION VALUE %lf on Axis %c\n",self->value, axisChar);

               /* Interpolate cross section using percentage value */
               if (modifierChar == '%')
                  //fprintf(stderr, "PERCENTAGE %lf %% CROSS SECTION on Axis %c\n", self->value, axisChar);
                  value *= 0.01;
            }
            /* Max or Min specified? */
            else if (strcmp(crossSectionVal, "min") == 0)
               value = 0.0;
               //fprintf(stderr, "MIN CROSS SECTION AT %lf on Axis %c\n", self->value, axisChar);
            else if (strcmp(crossSectionVal, "max") == 0)
               value = 1.0;
               //fprintf(stderr, "MAX CROSS SECTION AT %lf on Axis %c\n", self->value, axisChar);
         }
      }
   }

   self->fieldVariableName = Memory_Alloc_Array(char, 50, "fieldVariableName");
   strcpy(self->fieldVariableName, "FieldVariable");

   if (self->defaultResolution < 2) self->defaultResolution = 100;   /* Default sampling res */
   self->defaultResolution = Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"resolution", self->defaultResolution);

   _lucCrossSection_Init(
      self,
      Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"resolutionA", self->defaultResolution),
      Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"resolutionB", self->defaultResolution),
      coord1,
      coord2,
      coord3,
      value,
      axis,
      interpolate
   );
}

void _lucCrossSection_Build( void* drawingObject, void* data )
{
   lucCrossSection* self    = (lucCrossSection*)drawingObject;
   AbstractContext* context = self->context;
   Stg_ComponentFactory* cf = context->CF;

   /* Append cullface setting to property string */
   lucDrawingObject_AppendProps(self, "cullface=%d\n", self->cullface);

   /* HACK - Get pointer to FieldVariable in build phase just to let FieldVariables be created in plugins */
   self->fieldVariable =  Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)self->fieldVariableName, FieldVariable, True, data  );
   Stg_Component_Build( self->fieldVariable, data, False );
}

void _lucCrossSection_Initialise( void* drawingObject, void* data ) 
{
}

void _lucCrossSection_Execute( void* drawingObject, void* data ) {}

void _lucCrossSection_Destroy( void* drawingObject, void* data )
{
   lucCrossSection* self    = (lucCrossSection*)drawingObject;
   Memory_Free(self->fieldVariableName);
}

void _lucCrossSection_Setup( void* drawingObject, lucDatabase* database, void* _context )
{
   lucCrossSection* self = (lucCrossSection*)drawingObject;

   /* Ensure the field is synchronised. */
   lucDrawingObject_SyncShadowValues( self, self->fieldVariable );
   if (self->colourMap) lucColourMap_CalibrateFromFieldVariable( self->colourMap, self->fieldVariable );

   /* Use provided setup function to correctly set axis etc */
   lucCrossSection_Set(self, self->value, self->axis, self->interpolate);
}

/* Default cross-section object allows drawing a cut plane at a specified coord on any axis */
void _lucCrossSection_Draw( void* drawingObject, lucDatabase* database, void* _context )
{
   lucCrossSection* self = (lucCrossSection*)drawingObject;

   /* Only draw on proc 0 */
   if (database->context->rank > 0) return;

   /* Create a plane, have 3 coords, get the 4th corner coord */
   double coord4[3];
   lucCrossSection_Interpolate2d(self, 1.0, 1.0, coord4);

   /* Dump vertex pos */
   float coords[12] = {self->coord3[0], self->coord3[1], self->coord3[2],
                       coord4[0], coord4[1], coord4[2],
                       self->coord1[0], self->coord1[1], self->coord1[2],
                       self->coord2[0], self->coord2[1], self->coord2[2]};

   lucDatabase_AddGridVertices(database, 4, 2, coords);

   /* Add a single normal value */
   lucDatabase_AddNormal(database, lucGridType, self->normal);
}

/* Set the position for depth sort to centre of cross section */
void lucCrossSection_SetPosition(void* crossSection)
{
   lucCrossSection* self = (lucCrossSection*)crossSection;
   lucCrossSection_Interpolate2d(self, 0.5, 0.5, self->position);
   self->depthSort = True;
}

/* Interpolate in 2 directions */
void lucCrossSection_Interpolate2d(void* crossSection, double factorA, double factorB, XYZ coord)
{
   lucCrossSection* self = (lucCrossSection*)crossSection;
   int i;
   for (i=0; i<3; i++)
      coord[i] = self->coord1[i] + factorA * (self->coord2[i] - self->coord1[i]) + factorB * (self->coord3[i] - self->coord1[i]);
}

/* Returns the cross section value, interpolating where necessary */
double lucCrossSection_GetValue(void* crossSection, double min, double max)
{
   lucCrossSection* self = (lucCrossSection*)crossSection;
   if (self->interpolate)
      /* Interpolation factor 0-1 provided to determine cross-section value */
      return min + self->value * (max - min);
   else
      /* Exact value provided */
      return self->value;
}

/* Function to set cross section parameters and return self for use in passing cross-sections to functions */
lucCrossSection* lucCrossSection_Set(void* crossSection, double val, Axis axis, Bool interpolate)
{
   lucCrossSection* self = (lucCrossSection*)crossSection;
   self->value = val;
   self->axis = axis;
   self->interpolate = interpolate;

   FieldVariable_GetMinAndMaxGlobalCoords(self->fieldVariable, self->min, self->max );

   if (self->axisAligned) 
   {
      /* Set other axis directions for drawing cross section:
       * These settings produce consistent polygon winding for cross sections on all axis */
      switch (self->axis)
      {
      case I_AXIS:
         self->axis1 = J_AXIS;
         self->axis2 = K_AXIS;
         break;
      case J_AXIS:
         self->axis1 = K_AXIS;
         self->axis2 = I_AXIS;
         break;
      case K_AXIS:
         self->axis1 = I_AXIS;
         self->axis2 = J_AXIS;
         break;
      }

      double halfSampleA = 0, halfSampleB = 0;
      if (self->offsetEdges)
      {
         halfSampleA = 0.5 * (self->max[self->axis1] - self->min[self->axis1]) / self->resolutionA;
         halfSampleB = 0.5 * (self->max[self->axis2] - self->min[self->axis2]) / self->resolutionB;
      }

      /* Use axis to calculate from min & max */
      double pos = lucCrossSection_GetValue(self, self->min[self->axis], self->max[self->axis]);
      self->coord1[self->axis] = pos;
      self->coord1[self->axis1] = self->min[self->axis1] + halfSampleA;
      self->coord1[self->axis2] = self->min[self->axis2] + halfSampleB;
      self->coord2[self->axis] = pos;
      self->coord2[self->axis1] = self->max[self->axis1] - halfSampleA;
      self->coord2[self->axis2] = self->min[self->axis2] + halfSampleB;
      self->coord3[self->axis] = pos;
      self->coord3[self->axis1] = self->min[self->axis1] + halfSampleA;
      self->coord3[self->axis2] = self->max[self->axis2] - halfSampleB;
   }

   /* Set the position for depth sort to centre of cross section */
   lucCrossSection_SetPosition(self);

   /* Create normal to plane */
   StGermain_NormalToPlane( self->normal, self->coord1, self->coord2, self->coord3);

   return self;
}

/* Function to set cross section parameters for a volume slice */
lucCrossSection* lucCrossSection_Slice(void* crossSection, double val, Bool interpolate)
{
   lucCrossSection* self = (lucCrossSection*)crossSection;
   self->value = val;
   self->interpolate = interpolate;
   self->axis = K_AXIS;
   self->axis1 = J_AXIS;
   self->axis2 = I_AXIS;

   FieldVariable_GetMinAndMaxGlobalCoords(self->fieldVariable, self->min, self->max );

   double pos = lucCrossSection_GetValue(self, self->min[K_AXIS], self->max[K_AXIS]);
   self->coord1[self->axis] = self->coord2[self->axis] = self->coord3[self->axis] = pos;
   self->coord1[self->axis1] = self->coord3[self->axis1] = self->min[self->axis1];
   self->coord1[self->axis2] = self->coord2[self->axis2] = self->min[self->axis2];
   self->coord2[self->axis1] = self->max[self->axis1];
   self->coord3[self->axis2] = self->max[self->axis2];

   return self;
}

void lucCrossSection_AllocateSampleData(void* drawingObject, int dims)
{
   lucCrossSection* self = (lucCrossSection*)drawingObject;
   FieldVariable* fieldVariable = self->fieldVariable;
   Index          aIndex, bIndex, d;
   if (dims <= 0) dims = fieldVariable->fieldComponentCount;

   if ((!self->vertices && self->context->rank == 0) || !self->gatherData)
      self->vertices = Memory_Alloc_3DArray( float, self->resolutionA, self->resolutionB, 3, "quad vertices");
   else
      self->vertices = NULL;

   if (!self->values) 
      self->values = Memory_Alloc_3DArray( float, self->resolutionA, self->resolutionB, dims, "vertex values");

   if (dims > fieldVariable->fieldComponentCount)
   {
      for ( aIndex = 0 ; aIndex < self->resolutionA ; aIndex++ )
         for ( bIndex = 0 ; bIndex < self->resolutionB ; bIndex++ )
            for (d=fieldVariable->fieldComponentCount; d<dims; d++)
               self->values[aIndex][bIndex][d] = 0;
   }
}

void lucCrossSection_SampleField(void* drawingObject, Bool reverse)
{
   lucCrossSection* self = (lucCrossSection*)drawingObject;
   FieldVariable* fieldVariable = self->fieldVariable;
   Coord          localMin, localMax;
   Coord          pos;
   Index          aIndex, bIndex;
   int d;
   int dims = fieldVariable->fieldComponentCount;

   FieldVariable_GetMinAndMaxLocalCoords( fieldVariable, localMin, localMax );

   lucCrossSection_AllocateSampleData(self, 0);

   /* Get mesh cross section vertices and values */
   double time = MPI_Wtime();
   Journal_Printf(lucInfo, "Sampling (%s) %d x %d...  0%", self->name, self->resolutionA, self->resolutionB);
   for ( aIndex = 0 ; aIndex < self->resolutionA ; aIndex++ )
   {
      int percent = 100 * (aIndex + 1) / self->resolutionA;
      Journal_Printf(lucInfo, "\b\b\b\b%3d%%", percent);
      fflush(stdout);

      /* Reverse order if requested */
      Index aIndex1 = aIndex;
      if (reverse) aIndex1 = self->resolutionA - aIndex - 1;

      for ( bIndex = 0 ; bIndex < self->resolutionB ; bIndex++ )
      {
         double value[dims];
         int d;
         /* Get position */
         lucCrossSection_Interpolate2d(self, aIndex1 / (double)(self->resolutionA-1), bIndex / (double)(self->resolutionB-1), pos);
         
         /* Check cross section is within local space, 
          * if outside then skip to avoid wasting time attempting to sample */
         /* This scales horrendously when mesh is irregular as we search for points that are not on the processor */
         /* A local coord sampling routine, such as implemented for isosurfaces would help with this problem */
         /*double TOL = 0.00000001; //((max[self->axis1] - min[self->axis1] + max[self->axis2] - min[self->axis2])/2.0) * 0.000001;
         if (pos[I_AXIS] + TOL > localMin[I_AXIS] && pos[I_AXIS] - TOL < localMax[I_AXIS] &&
             pos[J_AXIS] + TOL > localMin[J_AXIS] && pos[J_AXIS] - TOL < localMax[J_AXIS] &&
            (fieldVariable->dim < 3 || (pos[K_AXIS] + TOL > localMin[K_AXIS] && pos[K_AXIS] - TOL < localMax[K_AXIS])))*/
         if (pos[I_AXIS] >= localMin[I_AXIS] && pos[I_AXIS] <= localMax[I_AXIS] &&
             pos[J_AXIS] >= localMin[J_AXIS] && pos[J_AXIS] <= localMax[J_AXIS] &&
            (fieldVariable->dim < 3 || (pos[K_AXIS] >= localMin[K_AXIS] && pos[K_AXIS] <= localMax[K_AXIS])) && 
            FieldVariable_InterpolateValueAt( fieldVariable, pos, value ) == LOCAL)
         {
            /* Value found locally, save */
            for (d=0; d<dims; d++)
               self->values[aIndex][bIndex][d] = (float)value[d];
         }
         else
         {
            /* Flag not found */
            for (d=0; d<dims; d++)
               self->values[aIndex][bIndex][d] = HUGE_VAL;
         }

         /* Copy vertex data */
         if (self->context->rank == 0 || !self->gatherData)
            for (d=0; d<3; d++)
               self->vertices[aIndex][bIndex][d] = (float)pos[d];
      }
   }
   /* Show each proc as it finishes */
   printf(" (%d)", self->context->rank);
   fflush(stdout);
   MPI_Barrier(self->context->communicator); /* Sync here, then time will show accurately how long sampling took on ALL procs */
   Journal_Printf(lucInfo, " -- %f sec.\n", MPI_Wtime() - time);

   /* This gathers all sampled data back to the root processor,
    * useful for drawing objects that are connected across proc boundaries
    * eg: surfaces, switch this flag off for others (eg: vectors) */
   if (self->gatherData && self->context->nproc > 1)
   {
      int count = self->resolutionA * self->resolutionB * dims;
      int r;
      time = MPI_Wtime();
      for (r=1; r < self->context->nproc; r++)
      {
         if (self->context->rank == r)
         {
            /* Send */
            (void)MPI_Send(&self->values[0][0][0], count, MPI_FLOAT, 0, r, self->context->communicator);
            Memory_Free(self->values);
            self->values = NULL;
         }
         else if (self->context->rank == 0)
         {
            /* Receive */
            MPI_Status status;
            float*** rvalues = Memory_Alloc_3DArray( float, self->resolutionA, self->resolutionB, dims, "received vertex values");
            (void)MPI_Recv(&rvalues[0][0][0], count, MPI_FLOAT, r, r, self->context->communicator, &status);

            /* If value provided, copy into final data (duplicates overwritten) */
            for ( aIndex = 0 ; aIndex < self->resolutionA ; aIndex++ )
               for ( bIndex = 0 ; bIndex < self->resolutionB ; bIndex++ )
                  for (d=0; d<dims; d++)
                     if (rvalues[aIndex][bIndex][d] != HUGE_VAL)
                        self->values[aIndex][bIndex][d] = rvalues[aIndex][bIndex][d];
            Memory_Free(rvalues);
         }
      }
      Journal_Printf(lucInfo, " Gather in %f sec.\n", MPI_Wtime() - time);
   }
}

void lucCrossSection_FreeSampleData(void* drawingObject)
{
   lucCrossSection* self = (lucCrossSection*)drawingObject;
   if (self->values) Memory_Free(self->values);
   self->values = NULL;
   if (self->vertices) Memory_Free(self->vertices);
   self->vertices = NULL;
}
