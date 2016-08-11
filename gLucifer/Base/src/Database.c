/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2010, Monash University
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
#include <stdio.h>
#include <float.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>

#include "Base.h"

#include "types.h"

/* Process: fork/exec/wait */
#include <unistd.h>
#include <sys/wait.h>

#define MINIZ_HEADER_FILE_ONLY
#include "miniz.c"

char SQL[1024];

const Type lucDatabase_Type = "lucDatabase";

lucDatabase* _lucDatabase_New(  LUCDATABASE_DEFARGS  )
{
   lucDatabase*    self;

   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(lucDatabase) );
   /* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
   /* This means that any values of these parameters that are passed into this function are not passed onto the parent function
      and so should be set to ZERO in any children of this class. */
   nameAllocationType = NON_GLOBAL;

   self = (lucDatabase*) _Stg_Component_New(  STG_COMPONENT_PASSARGS  );

   return self;
}

void* _lucDatabase_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof( lucDatabase );
   Type                                                      type = lucDatabase_Type;
   Stg_Class_DeleteFunction*                              _delete = _lucDatabase_Delete;
   Stg_Class_PrintFunction*                                _print = NULL;
   Stg_Class_CopyFunction*                                  _copy = NULL;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _lucDatabase_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _lucDatabase_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _lucDatabase_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _lucDatabase_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _lucDatabase_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _lucDatabase_Destroy;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return _lucDatabase_New(  LUCDATABASE_PASSARGS  );
}

void _lucDatabase_Init( 
   lucDatabase*         self,
   AbstractContext*     context,
   lucDrawingObject**   drawingObjectList,
   DrawingObject_Index  drawingObjectCount,
   int                  deleteAfter,
   Bool                 writeimage,
   Bool                 splitTransactions,
   Bool                 transparent,
   Bool                 compressed,
   Bool                 singleFile,
   char*                filename,
   char*                vfs,
   Name                 timeUnits,
   Bool                 disabled,
   Bool                 blocking )
{
   DrawingObject_Index object_I;

   self->isConstructed = True;
   self->context = (DomainContext*)context;
   self->deleteAfter = deleteAfter;
   self->writeimage = writeimage;
   self->splitTransactions = splitTransactions;
   self->transparent = transparent;

   self->compressed = compressed;
   self->singleFile = singleFile;
   self->filename = StG_Strdup(filename);
   self->vfs = NULL;
   if (vfs && strlen(vfs)) self->vfs = StG_Strdup(vfs);
   self->db = NULL;
   self->db2 = NULL;
   self->memdb = NULL;
   self->timeUnits = StG_Strdup(timeUnits);
   self->disabled = disabled || !self->context->vis;

   self->minValue[ I_AXIS ] = Stg_ComponentFactory_GetRootDictDouble( self->context->CF, (Dictionary_Entry_Key)"minX", 0.0  );
   self->minValue[ J_AXIS ] = Stg_ComponentFactory_GetRootDictDouble( self->context->CF, (Dictionary_Entry_Key)"minY", 0.0  );
   self->minValue[ K_AXIS ] = Stg_ComponentFactory_GetRootDictDouble( self->context->CF, (Dictionary_Entry_Key)"minZ", 0.0  );

   self->maxValue[ I_AXIS ] = Stg_ComponentFactory_GetRootDictDouble( self->context->CF, (Dictionary_Entry_Key)"maxX", 1.0  );
   self->maxValue[ J_AXIS ] = Stg_ComponentFactory_GetRootDictDouble( self->context->CF, (Dictionary_Entry_Key)"maxY", 1.0  );
   self->maxValue[ K_AXIS ] = Stg_ComponentFactory_GetRootDictDouble( self->context->CF, (Dictionary_Entry_Key)"maxZ", 1.0  );

   self->drawingObject_Register = lucDrawingObject_Register_New();

   for ( object_I = 0 ; object_I < drawingObjectCount ; object_I++ )
      lucDrawingObject_Register_Add( self->drawingObject_Register, drawingObjectList[ object_I ] );

   /* Set minZ = maxZ for 2d models */
   if (self->context->dim == 2)
   {
      self->minValue[K_AXIS] += 0.5 * (self->maxValue[K_AXIS] = self->minValue[K_AXIS]);
      self->maxValue[K_AXIS] = self->minValue[K_AXIS];
   }

   /* Create geometry data stores */
   lucGeometryType type;
   lucGeometryDataType data_type;
   for (type=lucMinType; type<lucMaxType; type++)
      for (data_type=lucMinDataType; data_type<lucMaxDataType; data_type++)
         self->data[type][data_type] = lucGeometryData_New(data_type);
   for (type=lucMinType; type<lucMaxType; type++)
   {
      self->labels[type] = NULL;
      self->label_lengths[type] = 0;
   }
   self->dump_pid = 0;

   /* Find dump script */
   self->dump_script = NULL;
   char binfile[256], binpath[256];
   FILE* fp;
   int pos;
   /* Strip lib from libpath */
   strncpy(binpath, LIB_DIR, 255);
   pos = strlen(binpath)-4;
   if (pos > 0) 
   {
      binpath[pos] = 0;
      sprintf(binfile, "%s/bin/dump.sh", binpath);
      fp = fopen(binfile, "r");
      if (fp)
      {
         self->dump_script = StG_Strdup(binfile);
         fclose(fp);
      }
   }
   if (!self->dump_script)
     self->dump_script = StG_Strdup("dump.sh");

   /* Add to entry points if we have our own list of drawing objects to output, bypassing window/viewport structures */
   if (!disabled)
   {
      /* Ensure our execute called before window execute by using Prepend...
       * If we have our own list of drawing objects they will be dumped here */
      EP_PrependClassHook(  Context_GetEntryPoint( context, AbstractContext_EP_DumpClass ), self->_execute, self );
   }
   
   self->blocking = blocking;
}

lucDatabase* lucDatabase_New(
   AbstractContext*  context,
   int               deleteAfter,
   Bool              writeimage,
   Bool              splitTransactions,
   Bool              transparent,
   Bool              compressed,
   Bool              singleFile,
   char*             filename,
   char*             vfs)
{
   lucDatabase* self = (lucDatabase*)_lucDatabase_DefaultNew("database");
   _lucDatabase_Init(self, context, NULL, 0, deleteAfter, writeimage, splitTransactions, transparent, compressed, singleFile, filename, vfs, "", False, False);
   return self;
}

void _lucDatabase_Delete( void* database )
{
   lucDatabase* self = (lucDatabase*)database;

   lucDatabase_Wait(database);

   /* Delete geometry data stores */
   lucGeometryType type;
   lucGeometryDataType data_type;
   for (type=lucMinType; type<lucMaxType; type++)
      for (data_type=lucMinDataType; data_type<lucMaxDataType; data_type++)
         if (self->data[type][data_type]) lucGeometryData_Delete(self->data[type][data_type]);
   for (type=lucMinType; type<lucMaxType; type++)
      if (self->labels[type]) Memory_Free(self->labels[type]);

   if (self->db) sqlite3_close(self->db);
   if (self->db2) sqlite3_close(self->db2);
   if (self->memdb) sqlite3_close(self->memdb);

   if (self->filename) Memory_Free(self->filename);
   if (self->vfs) Memory_Free(self->vfs);

   _Stg_Component_Delete( self );
}

void _lucDatabase_AssignFromXML( void* database, Stg_ComponentFactory* cf, void* data )
{
   lucDatabase* self = (lucDatabase*) database ;
   AbstractContext* context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", AbstractContext, False, data );
   DrawingObject_Index drawingObjectCount;
   lucDrawingObject**  drawingObjectList;

   if (!context)
      context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", AbstractContext, True, data  );

   /* Optional global drawing object list, can provide to database instead of windows+viewports */
   drawingObjectList = Stg_ComponentFactory_ConstructByList( cf, self->name, (Dictionary_Entry_Key)"DrawingObject", Stg_ComponentFactory_Unlimited, lucDrawingObject, False, &drawingObjectCount, data);

   _lucDatabase_Init( self, context, 
      drawingObjectList,
      drawingObjectCount,
      Stg_ComponentFactory_GetInt( cf, self->name, (Dictionary_Entry_Key)"deleteAfter", 0),
      Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"writeimage", True),
      Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"splitTransactions", False),
      Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"transparent", False),
      Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"compressed", True),
      Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"singleFile", True),
      Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"filename", "gLucifer"),
      Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"vfs", NULL),
      Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"timeUnits", ""  ),
      Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"disable", False  ),
      Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"blocking", False  )
      );
}

void _lucDatabase_Build( void* database, void* data ) 
{
   lucDatabase* self = (lucDatabase*) database ;
   /* Get rendering engine to write images at each 'Dump' entry point. */
   if (self->context->rank == 0 && !self->disabled)
      EP_AppendClassHook(  Context_GetEntryPoint( self->context, AbstractContext_EP_DumpClass ), lucDatabase_Dump, self);
}

void _lucDatabase_Initialise( void* database, void* data ) {}

void _lucDatabase_Execute( void* database, void* data ) 
{
   /* This begins the transaction, if drawing objects assigned
    * to the database, they are also output here, otherwise
    * they will be output by their parent window:
    * Window_Execute(Display) -> Viewport->Draw */
   lucDatabase* self = (lucDatabase*)database;
   if (!self->context->vis) return;

   if (self->context->rank == 0)
   {
      if (!self->db)
      {
         lucDrawingObject_Register* dr = self->drawingObject_Register;
         Index objectCount = lucDrawingObject_Register_GetCount( dr );
         Index object_I;

         /* Open and create database */
         lucDatabase_OpenDatabase(self);

         /* Output own object settings */
         for ( object_I = 0 ; object_I < objectCount ; object_I++ )
         {
            lucDrawingObject* object = lucDrawingObject_Register_GetByIndex( dr, object_I );
            lucDatabase_OutputDrawingObject(self, NULL, object);
         }
      }
   }

   /* Call setup on drawing objects (if any) !This must be called on all procs! */
   lucDrawingObject_Register_SetupAll( self->drawingObject_Register, database );

   if (self->context->rank == 0)
   {
      /* Multi-file database setup */
      if (!self->singleFile)
      {
         if (!self->memdb)
         {
            /* Copy structure to template file */
            sqlite3_open(":memory", &self->memdb);
            lucDatabase_BackupDb(self->db, self->memdb);
            /* Remove any existing data in the memory copy (can only be one timestep) */
            lucDatabase_IssueSQL(self->memdb, "delete from geometry");
            lucDatabase_IssueSQL(self->memdb, "delete from timestep");
         }
 
         if (self->context->timeStep > 0)
         {
            /* Attach db for new timestep */
            lucDatabase_AttachDatabase(self);
            /* Copy base table structure */
            lucDatabase_BackupDb(self->memdb, self->db2);
         }
      }

      /* Do each timestep database output in a single transaction 
       * (defaults to on, fastest option but possibly will require more memory) */
      if (!self->splitTransactions)
         Journal_Firewall(lucDatabase_BeginTransaction(self), lucError, "Begin transaction failed! %s '%s'.\n", self->type, self->name );

      /* If a data window is set, delete expired geometry */
      if (self->deleteAfter > 0)
      {
         int deleteEnd = self->context->timeStep - self->deleteAfter - 1;
         if (deleteEnd >= 0)
            lucDatabase_DeleteGeometry(self, -1, deleteEnd);
      }

      /* Get scaling coefficient and units for timestep */
      double dimCoeff = 1.0; /* coefficient for dimensionalising field */
      Scaling* theScaling;
      /* very hacky to get the scaling component */
      theScaling = self->context->scaling;
      if( theScaling && self->timeUnits) 
         dimCoeff = Scaling_ParseDimCoeff(theScaling, self->timeUnits);

      /* Enter timestep in database */

      /* When restarted, delete any existing geometry at current timestep */
      if (self->context->loadFromCheckPoint)
         lucDatabase_DeleteGeometry(self, self->context->timeStep, self->context->timeStep);

      /* Enter timestep in database */
      /* Write and update timestep */
      snprintf(SQL, 1024, "insert into timestep (id, time, dim_factor, units) values (%d, %g, %g, '%s')", self->context->timeStep, self->context->currentTime, dimCoeff, self->timeUnits);
      /*printf("%s\n", SQL);*/
      if (!lucDatabase_IssueSQL(self->db, SQL)) return;
      /* Also write to attached db */
      if (!self->singleFile && self->context->timeStep > 0)
        if (!lucDatabase_IssueSQL(self->db2, SQL)) return;
   }

   /* If we have global list of drawing objects to output, process them here */
   if (lucDrawingObject_Register_GetCount( self->drawingObject_Register ) > 0)
   {
      lucDrawingObject_Register_DrawAll( self->drawingObject_Register, self);
      lucDrawingObject_Register_CleanUpAll( self->drawingObject_Register, self->context );
   }
}

void _lucDatabase_Destroy( void* database, void* data ) { }

void lucDatabase_Dump(void* database)
{
   lucDatabase* self = (lucDatabase*)database;

   /* Commit the full timestep transaction */
   if (!self->splitTransactions)
   {
      double wtime = MPI_Wtime();
      lucDatabase_Commit(self);
      wtime = MPI_Wtime() - wtime;
      Journal_Printf(lucInfo, "    Transaction took %f seconds\n", wtime);
   }

   /* Skip if image output disabled or no script not found */
   if (!self->writeimage || !self->dump_script)
      return;

   /* Dump via fork+exec */
   /* Skip wait, should not cause lock problems as db access is read only
   lucDatabase_Wait(database);*/
   char* dumpcmd;
   asprintf(&dumpcmd, "%s %s %s%d %s",
            self->dump_script, self->context->outputPath,
            (self->transparent ? "-t " : ""), self->context->timeStep, self->path );

   Journal_Printf(lucInfo, "Dump command: %s\n", dumpcmd);


   if (!self->blocking)
   {
      self->dump_pid = vfork();
      if (self->dump_pid == 0)
      {
         char timestep[16];
         sprintf(timestep, "%d", self->context->timeStep);
         /* Code executed by child process */
         execl(self->dump_script, self->dump_script, self->context->outputPath,
               timestep, self->path, (self->transparent ? "-t" : (char*)0), (char *)0);
         /* If we are still here, exec call failed */
         Journal_Printf(lucError, "Exec failed!\n");
         exit(1); /* Terminate the child process */
      }
      else if (self->dump_pid < 0)
      {
         Journal_Printf(lucError, "Fork process failed!\n");
      }
   }
   else
   {
      system(dumpcmd);
   }
   
   free(dumpcmd);
}

void lucDatabase_Wait(lucDatabase* self)
{
   if (self->dump_pid > 0)
   {
      /* Wait for previous job to finish before starting next*/
      int childExitStatus;
      waitpid(self->dump_pid, &childExitStatus, 0);
      self->dump_pid = 0;
   }
}

void lucDatabase_DeleteWindows(lucDatabase* self)
{
   /* Delete any existing window->viewport->object structure information */
   lucDatabase_IssueSQL(self->db, "delete from window;");
   lucDatabase_IssueSQL(self->db, "delete from window_viewport;");
   lucDatabase_IssueSQL(self->db, "delete from viewport;");
   lucDatabase_IssueSQL(self->db, "delete from object;");
   lucDatabase_IssueSQL(self->db, "delete from viewport_object;");
}

void lucDatabase_OutputWindow(lucDatabase* self, lucWindow* window)
{
   Index   viewport_I, vertical_I, horizontal_I;
   Index   verticalCount, horizontalCount;
   float defaultMin[3] = {0,0,0};
   float defaultMax[3] = {FLT_EPSILON,FLT_EPSILON,FLT_EPSILON};
   int id;

   /* Open and create database */
   lucDatabase_OpenDatabase(self);

   if (lucDatabase_BeginTransaction(self))
   {
      float *min, *max;
      if (window->useModelBounds)
      {
         min = self->minValue;
         max = self->maxValue;
      }
      else 
      {
         /* Don't write model bounds with this window
          * (used if visualising something other than model domain, eg: plot) */
         min = defaultMin;
         max = defaultMax;
      }

      /* Save the window */
      snprintf(SQL, 1024, "insert into window (name, width, height, colour, minX, minY, minZ, maxX, maxY, maxZ) values ('%s', %d, %d, %d, %g, %g, %g, %g, %g, %g)", window->name, window->width, window->height, lucColour_ToInt(&window->backgroundColour), min[0], min[1], min[2], max[0], max[1], max[2]);
      /*printf("%s\n", SQL);*/
      if (!lucDatabase_IssueSQL(self->db, SQL)) return;
      id = sqlite3_last_insert_rowid(self->db);

      Journal_Printf(lucInfo, "Window: %s, id %d\n", window->name, id);

      viewport_I = 0;
      verticalCount = window->viewportLayout[0];
      for ( vertical_I = 0 ; vertical_I < verticalCount ; vertical_I++ )
      {
         /* Get horizontal count (columns) in this row */
         horizontalCount = window->viewportLayout[ vertical_I + 1 ];
         for ( horizontal_I = 0 ; horizontal_I < horizontalCount ; horizontal_I++ )
         {
            lucViewport* viewport = window->viewportList[ viewport_I ];
            if (!window->antialias) viewport->antialias = False;  /* Override anti-alias setting when disabled for window */
            lucDatabase_OutputViewport(self, viewport, id, horizontal_I / (float)horizontalCount, vertical_I / (float)verticalCount);
            viewport_I++;
         }
      }

      /* Commit transaction */
      lucDatabase_Commit(self);
   }
}

void lucDatabase_OutputViewport(lucDatabase* self, lucViewport* viewport, int window_id, float x, float y)
{
   lucDrawingObject_Register* dr = (lucDrawingObject_Register*)viewport->drawingObject_Register;
   Index objectCount = lucDrawingObject_Register_GetCount( dr );
   Index   object_I;
   char focus[64];
   int id;
   lucCamera* cam = viewport->camera;
   float translate[3] = {cam->translate[0], cam->translate[1], cam->translate[2]}; 

   /* Output objects (first so for back compatibility can write to some viewport properties) */
   for ( object_I = 0 ; object_I < objectCount ; object_I++ )
   {
      lucDrawingObject* object = lucDrawingObject_Register_GetByIndex( dr, object_I );
      lucDatabase_OutputDrawingObject(self, viewport, object);
   }

   /* Save the viewport - if added to multiple windows will create a new entry for each */
   /* Update properties string for viewport */
   lucViewport_Update(viewport);

   if (cam->centreFieldVariable)
      sprintf(focus, "null, null, null");
   else
      sprintf(focus, "%g, %g, %g", cam->focalPoint[0], cam->focalPoint[1], cam->focalPoint[2]);
   snprintf(SQL, 1024, "insert into viewport (title, x, y, near, far, aperture, orientation, focalPointX, focalPointY, focalPointZ, translateX, translateY, translateZ, rotateX, rotateY, rotateZ, scaleX, scaleY, scaleZ, properties) values ('%s', %g, %g, %g, %g, %g, %d, %s, %g, %g, %g, %g, %g, %g, %g, %g, %g, '%s')", viewport->title, x, y, viewport->nearClipPlane, viewport->farClipPlane, cam->aperture, cam->coordSystem, focus, translate[0], translate[1], translate[2], cam->rotate[0], cam->rotate[1], cam->rotate[2], viewport->scaleX, viewport->scaleY, viewport->scaleZ, viewport->properties);
   /*printf("%s\n", SQL);*/
   if (!lucDatabase_IssueSQL(self->db, SQL)) return;
   /* Return viewport id */
   id = sqlite3_last_insert_rowid(self->db);

   Journal_Printf(lucInfo, "   Viewport: %s id %d\n", viewport->name, id);

   /* Output object links */
   for ( object_I = 0 ; object_I < objectCount ; object_I++ )
   {
      lucDrawingObject* object = lucDrawingObject_Register_GetByIndex( dr, object_I );
      /* Write link to database */
      if (object->id)
      {
         /* Link object & viewport */
         snprintf(SQL, 1024, "insert into viewport_object (viewport_id, object_id) values (%d, %d)", id, object->id); 
         /*printf("%s\n", SQL);*/
         if (!lucDatabase_IssueSQL(self->db, SQL)) return;
      }
   }

   /* Link window & viewport */
   snprintf(SQL, 1024, "insert into window_viewport (window_id, viewport_id) values (%d, %d)", window_id, id); 
   /*printf("%s\n", SQL);*/
   if (!lucDatabase_IssueSQL(self->db, SQL)) return;
}

void lucDatabase_OutputDrawingObject(lucDatabase* self, lucViewport* viewport, lucDrawingObject* object)
{
   /* Save the object */
   if (!object->id) /* Not already written */
   {
      /* Check for types that aren't exportable, for back compatibility set new viewport props */
      if (viewport)
      {
         char* type = object->type;
         if (strcmp(type, "lucAxis") == 0)
         {
            lucAxis* axis = (lucAxis*)object;
            viewport->axis = True;
            viewport->axisLength = axis->length;
            return;
         }
         if (strcmp(type, "lucFieldVariableBorder") == 0)
         {
            viewport->border = (int)object->lineWidth;
            viewport->borderColour = object->colour;
            return;
         }
         if (strcmp(type, "lucTimeStep") == 0)
         {
            viewport->timestep = True;
            return;
         }
         if (strcmp(type, "lucTitle") == 0)
         {
            lucTitle* title = (lucTitle*)object;
            viewport->title = title->titleString;
            return;
         }
      }

      snprintf(SQL, 1024, "insert into object (name, colourmap_id, colour, opacity, properties) values ('%s', 0, %d, %g, '%s')", object->name, lucColour_ToInt(&object->colour), object->opacity, object->properties); 
      /*printf("%s\n", SQL);*/
      if (!lucDatabase_IssueSQL(self->db, SQL)) return;

      /* Save object id */
      object->id = sqlite3_last_insert_rowid(self->db);
      Journal_Printf(lucInfo, "      Drawing object: %s %s id %d\n", object->name, object->type, object->id);

      /* Save colour maps */
      if (object->colourMap)
         lucDatabase_OutputColourMap(self, object->colourMap, object, lucColourValueData);

      /* Would probably be nice to extend opacity & RGB maps to base object, for now check type first */
      if (object->type == lucSwarmViewer_Type || object->type == lucSwarmVectors_Type || 
          object->type == lucSwarmShapes_Type || object->type == lucSwarmRGBColourViewer_Type)
      {
         lucSwarmViewer* svobj = (lucSwarmViewer*)object;
         if (svobj->opacityVariable)
            lucDatabase_OutputColourMap(self, svobj->opacityColourMap, object, lucOpacityValueData);
         /* Only RGB viewer supports these for now */
         if (object->type == lucSwarmRGBColourViewer_Type)
         {
            lucSwarmRGBColourViewer* srgbobj = (lucSwarmRGBColourViewer*)object;
            if (srgbobj->colourRedVariable)
              lucDatabase_OutputColourMap(self, srgbobj->redColourMap, object, lucRedValueData);
            if (srgbobj->colourGreenVariable)
               lucDatabase_OutputColourMap(self, srgbobj->greenColourMap, object, lucGreenValueData);
            if (srgbobj->colourBlueVariable)
               lucDatabase_OutputColourMap(self, srgbobj->blueColourMap, object, lucBlueValueData);
         }
      }
   }
}

void lucDatabase_OutputColourMap(lucDatabase* self, lucColourMap* colourMap, lucDrawingObject* object, lucGeometryDataType type)
{
   /* Save colourMap */
   if (!colourMap->id) /* Not already written */
   {
      Index colour_I;

      snprintf(SQL, 1024, "insert into colourmap (name, minimum, maximum, logscale, discrete, centreValue) values ('%s', %g, %g, %d, %d, %g)", colourMap->name, colourMap->minimum, colourMap->maximum, colourMap->logScale, colourMap->discrete, colourMap->centreValue );
      /*printf("%s\n", SQL);*/
      if (!lucDatabase_IssueSQL(self->db, SQL)) return;

      /* Save id */
      colourMap->id = sqlite3_last_insert_rowid(self->db);

      Journal_Printf(lucInfo, "         ColourMap: %s, id %d\n", colourMap->name, colourMap->id);

      /* Write colours and values */
      for (colour_I=0; colour_I < colourMap->colourCount; colour_I++)
      {
         lucColourMapping* cm = &colourMap->colourList[colour_I];
         char value[32] = "null";
         if (cm->value)
            sprintf(value, "%g", *cm->value);
         snprintf(SQL, 1024, "insert into colourvalue (colourmap_id, colour, value) values (%d, %d, %s)", 
                 colourMap->id, lucColour_ToInt(cm->colour), value);
         /*printf("%s\n", SQL);*/
         if (!lucDatabase_IssueSQL(self->db, SQL)) return;
      }
   }

   /* Add reference for object */
   Journal_Printf(lucInfo, "         Linking colourMap: %s to object %s\n", colourMap->name, object->name);
   /* Link object & colour map */
   snprintf(SQL, 1024, "insert into object_colourmap (object_id, colourmap_id, data_type) values (%d, %d, %d)", object->id, colourMap->id, type); 
   /*printf("%s\n", SQL);*/
   if (!lucDatabase_IssueSQL(self->db, SQL)) return;
}

void lucDatabase_ClearGeometry(lucDatabase* self)
{
   /* Clear existing render objects */
   lucGeometryType type;
   lucGeometryDataType data_type;
   for (type=lucMinType; type<lucMaxType; type++)
      for (data_type=lucMinDataType; data_type<lucMaxDataType; data_type++)
         lucGeometryData_Clear(self->data[type][data_type]);
   for (type=lucMinType; type<lucMaxType; type++)
      if (self->labels[type]) self->labels[type][0] = 0;
}

void lucDatabase_OutputGeometry(lucDatabase* self, int object_id)
{
   lucGeometryType type;
   lucGeometryDataType data_type;
   unsigned int procs = self->context->nproc;
   unsigned int bytes = 0, outbytes = 0;
   double time, gtotal = 0, wtotal = 0;
   //Journal_Printf(lucInfo, "gLucifer: writing geometry to database ...\n");

   /* Write geometry to database */
   time = MPI_Wtime();
   if (self->context->rank > 0 || !self->splitTransactions || lucDatabase_BeginTransaction(self))
   {
      for (type=lucMinType; type<lucMaxType; type++)
      {
         //printf("%d Writing geometry %d procs, type %d\n", self->context->rank, procs, type);
         time = MPI_Wtime();
         if (procs > 1)
         {
            for (data_type=lucMinDataType; data_type<lucMaxDataType; data_type++)
               lucDatabase_GatherGeometry(self, type, data_type);
         }
         if (self->context->rank > 0) continue;
         gtotal += MPI_Wtime() - time;

         /* Loop through all geometry types and write to database */
         for (data_type=lucMinDataType; data_type<lucMaxDataType; data_type++)
         {
            lucGeometryData* data = self->data[type][data_type];
            bytes += data->count * sizeof(float);
            if (self->labels[type]) data->labels = self->labels[type];
            outbytes += lucDatabase_WriteGeometry(self, 0, type, data_type, object_id, data);
         }
      }

      /* Commit transaction */
      if (self->context->rank == 0 && self->splitTransactions)
      {
         time = MPI_Wtime();
         lucDatabase_Commit(self);
         wtotal += MPI_Wtime() - time;
      }
   }

   if (bytes > 0)
   {
      if (self->context->nproc > 1)
         Journal_Printf(lucInfo, "    Gather data, took %f sec\n", gtotal);
      if (self->context->rank == 0 && self->splitTransactions)
         Journal_Printf(lucInfo, "    Transaction took %f seconds\n", wtotal);
      Journal_Printf(lucInfo, "    %.3f kb of geometry data saved, %.3f kb written.\n", bytes/1000.0f, outbytes/1000.0f);
   }

   /* Clear all objects */
   lucDatabase_ClearGeometry(self);
}

void lucDatabase_GatherGeometry(lucDatabase* self, lucGeometryType type, lucGeometryDataType data_type)
{
   unsigned int procs = self->context->nproc;
   lucGeometryData* block = self->data[type][data_type];

   /* Get the count on each proc */
   int p;
   int *counts = NULL;
   if (self->context->rank == 0)
      counts = Memory_Alloc_Array(int, procs, "displacements");
   /* 1 value from each proc */
   (void)MPI_Gather(&block->count, 1, MPI_INT, counts, 1, MPI_INT, 0, self->context->communicator);

   /* Now we have count per proc, gather all values */
   //double time = MPI_Wtime();
   int *displ = NULL;
   float *data = NULL;
   int total = 0;
   if (self->context->rank == 0)
   {
      displ = Memory_Alloc_Array(int, procs, "displacements");

      for (p=0; p<procs; p++)/* Get displacements */
         displ[p] = p==0 ? 0 : displ[p-1] + counts[p-1];

      total = displ[procs-1] + counts[procs-1];
      /* All values already on root? skip gather */
      if (total == block->count) total = 0;
      if (total) data = Memory_Alloc_Array(float, total, "FloatData");
   }

   /* Any data to gather? */
   MPI_Bcast(&total, 1, MPI_INT, 0, self->context->communicator);
   if (total > 0)
   {
      (void)MPI_Gatherv(block->data, block->count, MPI_FLOAT, data, counts, displ, MPI_FLOAT, 0, self->context->communicator);

      /* Reduce to get minimum & maximum from all procs */
      float min, max;
      int width = 0;
      MPI_Reduce( &block->minimum, &min, 1, MPI_FLOAT, MPI_MIN, 0, self->context->communicator );
      MPI_Reduce( &block->maximum, &max, 1, MPI_FLOAT, MPI_MAX, 0, self->context->communicator );
      if (data_type == lucVertexData)
         MPI_Reduce( &block->width, &width, 1, MPI_INT, MPI_SUM, 0, self->context->communicator );
      /* (keep dimcoeff and units from root proc) */
      if (self->context->rank == 0)
      {
         //Journal_Printf(lucInfo, "Gathered %d values, took %f sec\n", total, MPI_Wtime() - time);
         /* Add new data on master */
         for (p=1; p<procs; p++)/* Get displacements */
         {
            if (counts[p] == 0) continue;

            //printf("[%d - %d] Reading %d values at displacement %d from proc %d MIN %f MAX %f WIDTH %d\n", type, data_type, counts[p], displ[p], p, min, max, width);
            if (data_type == lucVertexData)
               block->width = width;   //Summed width
            lucGeometryData_Read(block, counts[p] / block->size, &data[displ[p]]);
         }
         //printf("count %d, \n", block->count);
         lucGeometryData_Setup(block, min, max, block->dimCoeff, block->units);
      }
   }
   if (data) Memory_Free(data);
   Memory_Free(counts);
   Memory_Free(displ);
}

/* Direct write functions to enter data into geom data store */
void lucDatabase_AddIsosurface(lucDatabase* self, lucIsosurface* iso, Bool walls)
{
   /* Export surface triangles */
   Index triangle_I;
   int i;
   for ( triangle_I = 0 ; triangle_I < iso->triangleCount ; triangle_I++)
   {
      if (iso->triangleList[triangle_I].wall != walls) continue;
      for (i=0; i<3; i++)
      {
         /* Dump vertex pos, [value] */
         float coordf[3] = {iso->triangleList[triangle_I].pos[i][0],
                            iso->triangleList[triangle_I].pos[i][1],
                            iso->triangleList[triangle_I].pos[i][2]};
         if (self->context->dim == 2) coordf[2] = 0;
         float value = iso->triangleList[triangle_I].value[i];
         lucDatabase_AddVertices(self, 1, lucTriangleType, coordf);
         if (iso->colourField && iso->colourMap)
            lucDatabase_AddValues(self, 1, lucTriangleType, lucColourValueData, iso->colourMap, &value);
      }
   }
}

void lucDatabase_AddGridVertices(lucDatabase* self, int n, int width, float* data)
{
   int height = n / width;
   self->data[lucGridType][lucVertexData]->width = width;
   self->data[lucGridType][lucVertexData]->height = height;
   lucDatabase_AddVertices(self, n, lucGridType, data);
}

void lucDatabase_AddGridVertex(lucDatabase* self, int width, int height, float* data)
{
   /* Adds a single grid vertex, setting width and height */
   self->data[lucGridType][lucVertexData]->width = width;
   self->data[lucGridType][lucVertexData]->height = height;
   lucDatabase_AddVertices(self, 1, lucGridType, data);
}

void lucDatabase_AddVertices(lucDatabase* self, int n, lucGeometryType type, float* data)
{
   //Detects bounding box by checking each vertex x,y,z
   float* min = self->data[type][lucVertexData]->min;
   float* max = self->data[type][lucVertexData]->max;
   int p;
   for (p=0; p < n*3; p++)
   {
      int d = p % 3;
      if (data[p] > max[d]) max[d] = data[p];
      if (data[p] < min[d]) min[d] = data[p];
   }

   lucGeometryData_Read(self->data[type][lucVertexData], n, data);
}

void lucDatabase_AddVerticesWidth(lucDatabase* self, int n, lucGeometryType type, int width, float* data)
{
   self->data[type][lucVertexData]->width = width;
   lucDatabase_AddVertices(self, n, type, data);
}

void lucDatabase_AddNormals(lucDatabase* self, int n, lucGeometryType type, float* data)
{
   lucGeometryData_Read(self->data[type][lucNormalData], n, data);
}

void lucDatabase_AddNormal(lucDatabase* self, lucGeometryType type, XYZ norm)
{
   float normal[3] = {norm[0], norm[1], norm[2]};
   lucGeometryData_Read(self->data[type][lucNormalData], 1, normal);
}

void lucDatabase_AddVectors(lucDatabase* self, int n, lucGeometryType type, float min, float max, float* data)
{
   lucGeometryData_Read(self->data[type][lucVectorData], n, data);
   lucGeometryData_Setup(self->data[type][lucVectorData], min, max, 1.0, "");
}

void lucDatabase_AddValues(lucDatabase* self, int n, lucGeometryType type, lucGeometryDataType data_type, lucColourMap* colourMap, float* data)
{
   /* Used for colour or colour component mapping to values */
   lucGeometryData_Read(self->data[type][data_type], n, data);

   if (!colourMap) return;

   /* Get scaling coefficient and units if available */
   double dimCoeff = 1.0;
   char* units = "";
   if (colourMap->fieldVariable)
   {
      /* very hacky to get the scaling component */
      Scaling* theScaling = colourMap->fieldVariable->context->scaling;
      if( colourMap->fieldVariable->o_units )
      {
         if( theScaling ) 
            dimCoeff = Scaling_ParseDimCoeff( theScaling, colourMap->fieldVariable->o_units );
         units = colourMap->fieldVariable->o_units;
      }
   } 
   /* Set colour map parameters */
   lucGeometryData_Setup(self->data[type][data_type], colourMap->minimum, colourMap->maximum, dimCoeff, units);
}


void lucDatabase_AddVolumeSlice(lucDatabase* self, int width, int height, float* corners, lucColourMap* colourMap, float* data)
{
   /* Output corner vertices */
   lucDatabase_AddVertices(self, 2, lucVolumeType, corners);
   self->data[lucVolumeType][lucVertexData]->width = width;
   self->data[lucVolumeType][lucVertexData]->height = height;
   lucDatabase_AddValues(self, width*height, lucVolumeType, lucColourValueData, colourMap, data);
}

void lucDatabase_AddIndex(lucDatabase* self, lucGeometryType type, unsigned int index)
{
   /* Convert a uint to a float and write value */
   union
   {
      float val;
      unsigned int idx;
   } fidx;
   fidx.idx = index;
   lucGeometryData_Read(self->data[type][lucIndexData], 1, &fidx.val);
}

void lucDatabase_AddRGBA(lucDatabase* self, lucGeometryType type, float opacity, lucColour* colour)
{
   /* Convert a colour to a float and write value */
   union
   {
      float val;
      char rgba[4];
   } fcolour;

   if (opacity >= 0) 
      fcolour.rgba[3] = 255 * opacity;
   else
      fcolour.rgba[3] = 255 * colour->opacity;
   fcolour.rgba[0] = 255 * colour->red;
   fcolour.rgba[1] = 255 * colour->green;
   fcolour.rgba[2] = 255 * colour->blue;

   lucGeometryData_Read(self->data[type][lucRGBAData], 1, &fcolour.val);
}

void lucDatabase_AddTexCoord(lucDatabase* self, lucGeometryType type, float x, float y)
{
   float coord[3] = {x, y};
   lucGeometryData_Read(self->data[type][lucTexCoordData], 1, coord);
}

void lucDatabase_AddLabel(lucDatabase* self, lucGeometryType type, char* label)
{
   if (!self->labels[type] || strlen(self->labels[type]) + strlen(label) + 2 > self->label_lengths[type])
   {
      self->labels[type] = Memory_Realloc_Array(self->labels[type], char, self->label_lengths[type] + 1024);
      self->label_lengths[type] += 1024;
      if (self->label_lengths[type] == 1024) self->labels[type][0] = 0;
   }

   strcat(self->labels[type], label);
   strcat(self->labels[type], "\n");
}

lucGeometryData* lucGeometryData_New(lucGeometryDataType data_type)
{
   lucGeometryData* self = Memory_Alloc(lucGeometryData, "Geometry data block");
   self->data = NULL;
   self->labels = NULL;
   self->allocated = 0;
   self->size = 1;

   if (data_type <= lucVectorData)
      self->size = 3;
   if (data_type == lucTexCoordData)
      self->size = 2;

   lucGeometryData_Clear(self);

   return self;
}

void lucGeometryData_Clear(lucGeometryData* self)
{
   self->count = 0;

   self->width = 0;
   self->height = 0;

   self->minimum = HUGE_VAL;
   self->maximum = -HUGE_VAL;

   self->min[I_AXIS] = self->min[J_AXIS] = self->min[K_AXIS] = HUGE_VAL;
   self->max[I_AXIS] = self->max[J_AXIS] = self->max[K_AXIS] = -HUGE_VAL;

   self->dimCoeff = 1.0;
   self->units = NULL;
}

void lucGeometryData_Delete(lucGeometryData* self)
{
   if (self->data) Memory_Free(self->data);
   Memory_Free(self);
}

void lucGeometryData_Read(lucGeometryData* self, int items, float* data)
{
   int new = items * self->size;
   if (self->allocated < self->count + new)
   {
      /* Dynamic reallocation when needed */
      if (items > 3)
         self->allocated += new;        /* Enough for this block read */
      else
         self->allocated += 100 * new;  /* Enough for next 100 small reads */

      self->data = Memory_Realloc_Array(self->data, float, self->allocated);
   }

   memcpy(self->data + self->count, data, new * sizeof(float));
   self->count += new;
}

void lucGeometryData_Setup(lucGeometryData* self, float min, float max, float dimFactor, const char* units)
{
   self->minimum = min;
   self->maximum = max;
   self->dimCoeff = dimFactor;
   self->units = units;  
}

void lucDatabase_OpenDatabase(lucDatabase* self)
{
   /* Skip if db already open */
   if (!self->db)
   {
      /* Copy db from checkpointReadPath?? /
      if (restart && strlen(self->context->checkpointReadPath) > 0)*/

      if (self->filename)
         sprintf(self->path, "%s/%s.gldb", self->context->outputPath, self->filename); 
      else
         sprintf(self->path, ":memory"); 


      if (!(self->context->loadFromCheckPoint))
         /* Remove existing file */
         remove(self->path);

      if(sqlite3_open_v2(self->path, &self->db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, self->vfs))
      {
         printf("Can't open database: %s\n", sqlite3_errmsg(self->db));
         self->db = NULL;
         return;
      }

      /* 10 sec timeout on busy(locked), as we are only accessing the db on root should not be necessary */
      sqlite3_busy_timeout(self->db, 10000);

      if (!(self->context->loadFromCheckPoint))
      {
         /* Remove existing data */
         lucDatabase_IssueSQL(self->db, "drop table IF EXISTS geometry");
         lucDatabase_IssueSQL(self->db, "drop table IF EXISTS timestep");
      }

      /* Create new tables when not present */
      lucDatabase_CreateDatabase(self);
   }
}

void lucDatabase_CreateDatabase(lucDatabase* self)
{
   /* Create database tables */
   lucDatabase_IssueSQL(self->db, "create table IF NOT EXISTS geometry (id INTEGER PRIMARY KEY ASC, object_id INTEGER, timestep INTEGER, rank INTEGER, idx INTEGER, type INTEGER, data_type INTEGER, size INTEGER, count INTEGER, width INTEGER, minimum REAL, maximum REAL, dim_factor REAL, units VARCHAR(32), minX REAL, minY REAL, minZ REAL, maxX REAL, maxY REAL, maxZ REAL, labels VARCHAR(2048), properties VARCHAR(2048), data BLOB, FOREIGN KEY (object_id) REFERENCES object (id) ON DELETE CASCADE ON UPDATE CASCADE, FOREIGN KEY (timestep) REFERENCES timestep (id) ON DELETE CASCADE ON UPDATE CASCADE)");

   /* Delete structure tables, always recreated */
   lucDatabase_IssueSQL(self->db, "drop table IF EXISTS object_colourmap");
   lucDatabase_IssueSQL(self->db, "drop table IF EXISTS colourmap");
   lucDatabase_IssueSQL(self->db, "drop table IF EXISTS colourvalue");
   lucDatabase_IssueSQL(self->db, "drop table IF EXISTS object");
   lucDatabase_IssueSQL(self->db, "drop table IF EXISTS window");
   lucDatabase_IssueSQL(self->db, "drop table IF EXISTS viewport");
   lucDatabase_IssueSQL(self->db, "drop table IF EXISTS window_viewport");
   lucDatabase_IssueSQL(self->db, "drop table IF EXISTS viewport_object");

   lucDatabase_IssueSQL(self->db, 
      "create table IF NOT EXISTS timestep (id INTEGER PRIMARY KEY ASC, time REAL, dim_factor REAL, units VARCHAR(32), properties VARCHAR(2048))");

   lucDatabase_IssueSQL(self->db, 
      "create table object_colourmap (id integer primary key asc, object_id integer, colourmap_id integer, data_type integer, foreign key (object_id) references object (id) on delete cascade on update cascade, foreign key (colourmap_id) references colourmap (id) on delete cascade on update cascade)");

   lucDatabase_IssueSQL(self->db, 
      "create table viewport_object (id INTEGER PRIMARY KEY ASC, viewport_id INTEGER, object_id INTEGER, FOREIGN KEY (object_id) REFERENCES object (id) ON DELETE CASCADE ON UPDATE CASCADE, FOREIGN KEY (viewport_id) REFERENCES viewport (id) ON DELETE CASCADE ON UPDATE CASCADE)");

   lucDatabase_IssueSQL(self->db, 
      "create table object (id INTEGER PRIMARY KEY ASC, name VARCHAR(256), colourmap_id INTEGER, colour INTEGER, opacity REAL, properties VARCHAR(2048), FOREIGN KEY (colourmap_id) REFERENCES colourmap (id) ON DELETE CASCADE ON UPDATE CASCADE)"); 

   lucDatabase_IssueSQL(self->db, 
      "create table colourvalue (id INTEGER PRIMARY KEY ASC, colourmap_id INTEGER, colour INTEGER, value REAL, FOREIGN KEY (colourmap_id) REFERENCES colourmap (id) ON DELETE CASCADE ON UPDATE CASCADE)"); 

   lucDatabase_IssueSQL(self->db, 
      "create table colourmap (id INTEGER PRIMARY KEY ASC, name VARCHAR(256), minimum REAL, maximum REAL, logscale INTEGER, discrete INTEGER, centreValue REAL, properties VARCHAR(2048))"); 

   lucDatabase_IssueSQL(self->db, 
      "create table viewport (id INTEGER PRIMARY KEY ASC, title VARCHAR(256), x REAL, y REAL, near REAL, far REAL, aperture REAL, orientation INTEGER, focalPointX REAL, focalPointY REAL, focalPointZ REAL, translateX REAL, translateY REAL, translateZ REAL, rotateX REAL, rotateY REAL, rotateZ REAL, scaleX REAL, scaleY REAL, scaleZ real, properties VARCHAR(2048))"); 

   lucDatabase_IssueSQL(self->db, 
      "create table window_viewport (id INTEGER PRIMARY KEY ASC, window_id INTEGER, viewport_id INTEGER, FOREIGN KEY (window_id) REFERENCES window (id) ON DELETE CASCADE ON UPDATE CASCADE, FOREIGN KEY (viewport_id) REFERENCES viewport (id) ON DELETE CASCADE ON UPDATE CASCADE)"); 

   lucDatabase_IssueSQL(self->db, 
      "create table window (id INTEGER PRIMARY KEY ASC, name VARCHAR(256), width INTEGER, height INTEGER, colour INTEGER, minX REAL, minY REAL, minZ REAL, maxX REAL, maxY REAL, maxZ REAL, properties VARCHAR(2048))");
}

Bool lucDatabase_IssueSQL(sqlite3* db, const char* SQL)
{
   /* Executes a basic SQLite command (ie: without pointer objects and ignoring result sets) and checks for errors */
   if (!db) return False;
   char* zErrMsg;
   if (sqlite3_exec(db, SQL, NULL, 0, &zErrMsg) != SQLITE_OK)
   {
      printf("SQLite Issue: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
      return False;
   }
   return True;
}

Bool lucDatabase_BeginTransaction(lucDatabase* self)
{
   return lucDatabase_IssueSQL(self->db, "BEGIN EXCLUSIVE TRANSACTION");
}

void lucDatabase_Commit(lucDatabase* self)
{
   lucDatabase_IssueSQL(self->db, "COMMIT");
}

void lucDatabase_AttachDatabase(lucDatabase* self)
{
   /* Detach previous */
   char path[256];
   if (self->db2)
      sqlite3_close(self->db2);

   /* Create path */
   sprintf(path, "%s/%s%05d.gldb", self->context->outputPath, self->filename, self->context->timeStep); 

   /* Attach new database */
   if (sqlite3_open_v2(path, &self->db2, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, self->vfs))
   {
      printf("Can't open database: %s\n", sqlite3_errmsg(self->db2));
      self->db2 = NULL;
      return;
   }
   printf("Database file %s opened\n", path);
}

void lucDatabase_DeleteGeometry(lucDatabase* self, int start_timestep, int end_timestep)
{
   /* Remove data over timestep range */
   if (start_timestep < 0)
      snprintf(SQL, 1024,  "delete from geometry where timestep <= %d; delete from timestep where id <= %d;", end_timestep, end_timestep);
   else if (end_timestep < 0)
      snprintf(SQL, 1024,  "delete from geometry where timestep >= %d; delete from timestep where id >= %d;", start_timestep, start_timestep);
   else
      snprintf(SQL, 1024,  "delete from geometry where timestep between %d and %d; delete from timestep where id between %d and %d;", start_timestep, end_timestep, start_timestep, end_timestep);

   lucDatabase_IssueSQL(self->db, SQL);
}

int lucDatabase_WriteGeometry(lucDatabase* self, int index, lucGeometryType type, lucGeometryDataType data_type, int object_id, lucGeometryData* block)
{
   sqlite3* db = self->db;
   if (self->db2) db = self->db2; //Use secondary per-timestep database
   sqlite3_stmt* statement;
   if (!block->count) return 0;
   
   /* Compress the data if enabled and > 1kb */
   unsigned char* buffer = (unsigned char*)block->data;
   unsigned long src_len = block->count * sizeof(float);
   unsigned long cmp_len = 0;
   if (self->compressed && src_len > 1000)
   {
      cmp_len = compressBound(src_len);
      buffer = (mz_uint8*)malloc((size_t)cmp_len);
      Journal_Firewall(buffer != NULL, lucError, "Compress database: Out of memory! %s '%s'.\n", self->type, self->name );
      Journal_Firewall(compress(buffer, &cmp_len, (const unsigned char *)block->data, src_len) == Z_OK, 
         lucError, "Compress database failed! %s '%s'.\n", self->type, self->name );
      if (cmp_len == src_len)
      {
         free(buffer);
         cmp_len = 0;
      }
      src_len = cmp_len;
   }

   if (block->minimum == HUGE_VAL) block->minimum = 0;
   if (block->maximum == -HUGE_VAL) block->maximum = 0;

   if (block->min[0] > block->max[0]) block->min[0] = block->max[0] = 0;
   if (block->min[1] > block->max[1]) block->min[1] = block->max[1] = 0;
   if (block->min[2] > block->max[2]) block->min[2] = block->max[2] = 0;

   snprintf(SQL, 1024, "insert into geometry (object_id, timestep, rank, idx, type, data_type, size, count, width, minimum, maximum, dim_factor, units, minX, minY, minZ, maxX, maxY, maxZ, labels, data) values (%d, %d, %d, %d, %d, %d, %d, %d, %d, %g, %g, %g, '%s', %g, %g, %g, %g, %g, %g, ?, ?)", object_id, self->context->timeStep, 0, index, type, data_type, block->size, block->count, block->width, block->minimum, block->maximum, block->dimCoeff, block->units ? block->units : "", block->min[0], block->min[1], block->min[2], block->max[0], block->max[1], block->max[2]);

   /* Prepare statement... */
   if (sqlite3_prepare_v2(db, SQL, -1, &statement, NULL) != SQLITE_OK)
   {
      printf("SQL prepare error: (%s) %s\n", SQL, sqlite3_errmsg(db));
      abort(); //Database errors fatal?
      return 0;
   }

   /* Setup text data for insert */
   if (block->labels)
   {
      if (sqlite3_bind_text(statement, 1, block->labels, strlen(block->labels), SQLITE_STATIC) != SQLITE_OK)
      {
         printf("SQL bind error: %s\n", sqlite3_errmsg(db));
         abort(); //Database errors fatal?
         return 0;
      }
   }

   /* Setup blob data for insert */
   if (sqlite3_bind_blob(statement, 2, buffer, src_len, SQLITE_STATIC) != SQLITE_OK)
   {
      printf("SQL bind error: %s\n", sqlite3_errmsg(db));
      abort(); //Database errors fatal?
      return 0;
   }

   /* Execute statement */
   if (sqlite3_step(statement) != SQLITE_DONE )
   {
      printf("SQL step error: (%s) %s\n", SQL, sqlite3_errmsg(db));
      abort(); //Database errors fatal?
   }

   sqlite3_finalize(statement);

   /* Free compression buffer */
   if (cmp_len > 0)
      free(buffer);

   return src_len;
}

void lucDatabase_BackupDb(sqlite3 *fromDb, sqlite3* toDb)
{
   sqlite3_backup *pBackup;  /* Backup object used to copy data */
   pBackup = sqlite3_backup_init(toDb, "main", fromDb, "main");
   if (pBackup)
   {
      (void)sqlite3_backup_step(pBackup, -1);
      (void)sqlite3_backup_finish(pBackup);
   }
}

