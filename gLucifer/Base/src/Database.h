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

#ifndef __lucDatabase_h__
#define __lucDatabase_h__

#include <gLucifer/Viewer/Types.h>
#include <gLucifer/DrawingObjects/DrawingObjects.h>
#include "sqlite3.h"

/* For collating Geometry Data before writing to database */
typedef struct
{
   float* data;
   int size;
   int count;
   int allocated;
   int width;
   int height;
   float minimum;
   float maximum;
   float min[3];
   float max[3];
   float dimCoeff;         /* For scaling */
   const char* units;      /* Scaling units */
   const char* labels;     /* Label strings */
} lucGeometryData;

extern const Type lucDatabase_Type;

/** Class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
#define __lucDatabase                                       \
      /* Macro defining parent goes here - This means you can cast this class as its parent */ \
      __Stg_Component                                       \
      /* Other Info */ \
      DomainContext*    context;                            \
      /* Internal */ \
      lucDrawingObject_Register*    drawingObject_Register; \
      float             minValue[3];                        \
      float             maxValue[3];                        \
      char*             labels[lucMaxType];                 \
      unsigned int      label_lengths[lucMaxType];          \
      lucGeometryData*  data[lucMaxType][lucMaxDataType];   \
      pid_t             dump_pid;                           \
      char*             dump_script;                        \
      /* Database */ \
      sqlite3*          db;                     \
      sqlite3*          db2;                    \
      sqlite3*          memdb;                  \
      char              path[256];              \
      /* Params */ \
      char*             filename;               \
      char*             vfs;                    \
      Bool              compressed;             \
      Bool              singleFile;             \
      Bool              writeimage;             \
      Bool              splitTransactions;      \
      Bool              transparent;            \
      int               deleteAfter;            \
      char*             timeUnits;              \
      Bool              disabled;               \
      Bool              blocking;               \

struct lucDatabase
{
   __lucDatabase
};


#ifndef ZERO
#define ZERO 0
#endif

#define LUCDATABASE_DEFARGS \
                STG_COMPONENT_DEFARGS

#define LUCDATABASE_PASSARGS \
                STG_COMPONENT_PASSARGS

lucDatabase* _lucDatabase_New(  LUCDATABASE_DEFARGS  );

/* A public constructor for use in Window when creating default database */
lucDatabase* lucDatabase_New(
   AbstractContext*  context,
   int               deleteAfter,
   Bool              writeimage,
   Bool              splitTransactions,
   Bool              transparent,
   Bool              compressed,
   Bool              singleFile,
   char*             filename,
   char*             vfs);

void _lucDatabase_Delete( void* database ) ;

void* _lucDatabase_DefaultNew( Name name ) ;
void _lucDatabase_AssignFromXML( void* database, Stg_ComponentFactory* cf, void* data ) ;
void _lucDatabase_Build( void* database, void* data );
void _lucDatabase_Initialise( void* database, void* data );
void _lucDatabase_Execute( void* database, void* data );
void _lucDatabase_Destroy( void* database, void* data );

void lucDatabase_Dump(void* database);
void lucDatabase_Wait(lucDatabase* self);

void lucDatabase_OutputWindow(lucDatabase* self, lucWindow* window);
void lucDatabase_OutputViewport(lucDatabase* self, lucViewport* viewport, int window_id, float x, float y);
void lucDatabase_OutputDrawingObject(lucDatabase* self, lucViewport* viewport, lucDrawingObject* object);
void lucDatabase_OutputColourMap(lucDatabase* self, lucColourMap* colourMap, lucDrawingObject* object, lucGeometryDataType type);

void lucDatabase_ClearGeometry(lucDatabase* self);
void lucDatabase_OutputGeometry(lucDatabase* self, int object_id);
void lucDatabase_GatherGeometry(lucDatabase* self, lucGeometryType type, lucGeometryDataType data_type);

void lucDatabase_AddIsosurface(lucDatabase* self, lucIsosurface* iso, Bool walls);
void lucDatabase_AddGridVertices(lucDatabase* self, int n, int width, float* data);
void lucDatabase_AddGridVertex(lucDatabase* self, int width, int height, float* data);
void lucDatabase_AddVertices(lucDatabase* self, int n, lucGeometryType type, float* data);
void lucDatabase_AddVerticesWidth(lucDatabase* self, int n, lucGeometryType type, int width, float* data);
void lucDatabase_AddNormals(lucDatabase* self, int n, lucGeometryType type, float* data);
void lucDatabase_AddNormal(lucDatabase* self, lucGeometryType type, XYZ norm);
void lucDatabase_AddVectors(lucDatabase* self, int n, lucGeometryType type, float min, float max, float* data);
void lucDatabase_AddValues(lucDatabase* self, int n, lucGeometryType type, lucGeometryDataType data_type, lucColourMap* colourMap, float* data);
void lucDatabase_AddIndex(lucDatabase* self, lucGeometryType type, unsigned int index);
void lucDatabase_AddRGBA(lucDatabase* self, lucGeometryType type, float opacity, lucColour* colour);
void lucDatabase_AddTexCoord(lucDatabase* self, lucGeometryType type, float x, float y);
void lucDatabase_AddLabel(lucDatabase* self, lucGeometryType type, char* label);
void lucDatabase_AddVolumeSlice(lucDatabase* self, int width, int height, float* corners, lucColourMap* colourMap, float* data);

lucGeometryData* lucGeometryData_New(lucGeometryDataType data_type);
void lucGeometryData_Clear(lucGeometryData* self);
void lucGeometryData_Delete(lucGeometryData* self);
void lucGeometryData_Read(lucGeometryData* self, int items, float* data); //, int width, int height)
void lucGeometryData_Setup(lucGeometryData* self, float min, float max, float dimFactor, const char* units);

void lucDatabase_OpenDatabase(lucDatabase* self);
void lucDatabase_CreateDatabase(lucDatabase* self);
Bool lucDatabase_IssueSQL(sqlite3* db, const char* SQL);
Bool lucDatabase_BeginTransaction(lucDatabase* self);
void lucDatabase_Commit(lucDatabase* self);
void lucDatabase_AttachDatabase(lucDatabase* self);
void lucDatabase_DeleteGeometry(lucDatabase* self, int start_timestep, int end_timestep);

int lucDatabase_WriteGeometry(lucDatabase* self, int index, lucGeometryType type, lucGeometryDataType data_type, int object_id, lucGeometryData* block);

void lucDatabase_BackupDb(sqlite3 *fromDb, sqlite3* toDb);

#endif

