/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**
**   This library is free software; you can redistribute it and/or
**   modify it under the terms of the GNU Lesser General Public
**   License as published by the Free Software Foundation; either
**   version 2.1 of the License, or (at your option) any later version.
**
**   This library is distributed in the hope that it will be useful,
**   but WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.   See the GNU
**   Lesser General Public License for more details.
**
**   You should have received a copy of the GNU Lesser General Public
**   License along with this library; if not, write to the Free Software
**   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA   02110-1301   USA
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>

#include <StgDomain/Geometry/Geometry.h>
#include <StgDomain/Shape/Shape.h>

#include "types.h"
#include "shortcuts.h"
#include "Grid.h"
#include "Decomp.h"
#include "Sync.h"
#include "MeshTopology.h"
#include "IGraph.h"
#include "MeshClass.h"
#include "MeshGenerator.h"
#include "MeshAdaptor.h"
#include "NodeBunching.h"

typedef double (NodeBunching_DeformFunc)( NodeBunching* self, Mesh* mesh, unsigned* globalSize, unsigned vertex, unsigned* vertexInds );

/* Textual name of this class */
const Type NodeBunching_Type = "NodeBunching";

/* My private functions */
void NodeBunching_Debug_1( void* _self, char axis ) {
   NodeBunching* self = (NodeBunching*)_self;
   char          fileName[256];
   FILE*         fh = NULL; 

   if( self->debugMode ) {
      sprintf( (char*)fileName, "mappingFunction_%c.m", axis );
      fh = fopen( fileName, "w" );
      fprintf( fh, "mf = [ ...\n" );
      fclose( fh );
   }
}

void NodeBunching_Debug_2( void* _self, char axis, double x, double y ) {
   NodeBunching* self = (NodeBunching*)_self;
   char          fileName[256];
   FILE*         fh = NULL;

   if( self->debugMode ) {   
      sprintf( (char*)fileName, "mappingFunction_%c.m", axis );
      fh = fopen( fileName, "a" );
      fprintf( fh, "%f %f\n", x, y );
      fclose( fh );
   }
}

void NodeBunching_Debug_3( void* _self, char axis ) {
   NodeBunching* self = (NodeBunching*)_self;
   char          fileName[256];
   FILE*         fh = NULL; 

   if( self->debugMode ) {   
      sprintf( (char*)fileName, "mappingFunction_%c.m", axis );
      fh = fopen( fileName, "a" );
      fprintf( fh, "];\n" );
      fprintf( fh, "figure(33); clf; plot( mf(:,1), mf(:,2), '-b' )\n" );
      fclose( fh );
   }
}

/* Counts the number of nodes in one segment */
int NodeBunching_numNodesSegCteSize( double x1, double x2, double elemSize, double* newElemSize ) {
   int numElem = ceil( (x2 - x1) / elemSize );
   int numNode = numElem + 1;
   /* We want all elements of the same size.
      To avoid the last element to be smaller,
      we slightly decrease all elements */
   newElemSize[0] = (x2 - x1) / numElem;

   /* Remove last node */
   numNode--;
   
   return numNode;
}


/* Counts the number of nodes in one segment - last node not counted */
int NodeBunching_numNodesSegVarSize( double x1, double x2, double elemSize1, double elemSize2, double factor, int* numCte, int* numVar, double* cteElemSize ) {
   double  maxSize, minSize, curSize, len;
   int     numNodes, numCteNodes;
   Stream* errorStream = Journal_Register( Error_Type, (Name)CURR_MODULE_NAME );
   
   if( elemSize1 < elemSize2 ) {
      maxSize = elemSize2;
      minSize = elemSize1;
   }
   else {
      maxSize = elemSize1;
      minSize = elemSize2;
   }
   
   /* count transition nodes */
   len = 0;
   numNodes = 1;
   curSize = minSize;

   while( curSize <= maxSize ) {
      len += curSize;
      numNodes++;
      curSize = minSize * pow( factor, numNodes-1 );
   }
   /* last node not included */
   numNodes--;
      
   /* check the transition region length not to be smaller than the segment */
   Journal_Firewall( len < (x2-x1) , errorStream, "\n\n\n"
      "\tNode bunching configuration error...\n"
      "\tThe segment from %g to %g does not have enough space to\n"
      "\tchange the element size from %f to %f, using the factor=%f\n"
      "\tPlease, increase the length of the segment or the factor.\n\n\n"
      ,x1, x2, elemSize1, elemSize2, factor );
   
   /* count cte size elements */
   numCteNodes = NodeBunching_numNodesSegCteSize( x1+len, x2, maxSize, cteElemSize );

   numCte[0] = numCteNodes;
   numVar[0] = numNodes;
   
   return numNodes + numCteNodes;
}

/* Counts the total number of nodes needed to satisfy the element sizes. 
    Also it fills the table with useful info for later usage */
int NodeBunching_countNodes( NodeBunching_PointData* table, int tableSize, double factor ) {
   double x1, x2, elemSize1, elemSize2, cteElemSize;
   int    i, numNodes = 0, numCte, numVar, aux;
   
   for( i = 0 ; i < tableSize - 1 ; i++ ) {
      x1 = table[i].x;
      x2 = table[i+1].x;
      elemSize1 = table[i].size;
      elemSize2 = table[i+1].size;
    
      if( elemSize1 == elemSize2 ) {
         aux = NodeBunching_numNodesSegCteSize( x1, x2, elemSize1, &cteElemSize );
         numNodes += aux;
         table[i].numNodesCte = aux;
         table[i].numNodesVar = 0;
         table[i].cteElemSize = cteElemSize;
      }
      else {
         numNodes += NodeBunching_numNodesSegVarSize( x1, x2, elemSize1, elemSize2, factor, &numCte, &numVar, &cteElemSize );
         table[i].numNodesCte = numCte;
         table[i].numNodesVar = numVar;
         table[i].cteElemSize = cteElemSize;
      }
   }   
   /* last segment includes last node */
   return numNodes + 1;
}

/* stores on self->newCoors the new positions */
void NodeBunching_newPositions( void* _self, int axis ) {
   NodeBunching*           self = (NodeBunching*)_self;
   Stream*                 errorStream = Journal_Register( Error_Type, (Name)CURR_MODULE_NAME );
   unsigned                tableSize = 0;
   NodeBunching_PointData* table = NULL;
   double*                 newCoords = NULL;
   double                  factor = self->factor;
   int                     totNodes = 0;
   int                     meshNodes = 0;
   int                     numNodes = 0;
   char                    straxis, uwAxis;
   unsigned                i, j, k;
   double                  x1, x2, elemSize1, elemSize2, curElemSize, curNodePos;
   
   /* chose axis */
   switch( axis ) {
      case I_AXIS:
         table = self->pointsx;
         tableSize = self->nPointsx;
         newCoords = self->newCoordsx;
         meshNodes = self->nNodes[I_AXIS];
         straxis = 'x';
         uwAxis = 'I';
         break;   
      case J_AXIS:
         table = self->pointsy;
         tableSize = self->nPointsy;
         newCoords = self->newCoordsy;
         meshNodes = self->nNodes[J_AXIS];
         straxis = 'y';
         uwAxis = 'J';
         break;
      case K_AXIS:
         table = self->pointsz;
         tableSize = self->nPointsz;
         newCoords = self->newCoordsz;
         meshNodes = self->nNodes[K_AXIS];
         straxis = 'z';
         uwAxis = 'K';
         break;
   }

   totNodes = NodeBunching_countNodes( table, tableSize, factor );

   Journal_Firewall( totNodes == meshNodes, errorStream, "\n\n\n"
      "\tThe configuration of the NodeBunching component"
      "\tfor the %c-axis requires exactly %i elements\n\n"
      "\tPlease modify your input file and set elementRes%c=%i\n\n"
      "\t(or you can also wait for a new version of this component :)\n\n\n", 
      straxis, totNodes-1, uwAxis, totNodes-1 );
   
   newCoords = Memory_Alloc_Array( double , totNodes, "new coords" );

   k = 0;
   for( i = 0 ; i < tableSize - 1 ; i++ ) {
      x1 = table[i].x;
      x2 = table[i+1].x;
      elemSize1 = table[i].size;
      elemSize2 = table[i+1].size;
    
      if( elemSize1 == elemSize2 ) {
         /** segment - cte element size **/
         numNodes = table[i].numNodesCte;
         curElemSize = table[i].cteElemSize;         
         curNodePos = x1;
         for( j = 0 ; j < numNodes ; j++ ) {
            newCoords[k++] = curNodePos;
            curNodePos += curElemSize;
         }
      
      }
      else if( elemSize1 < elemSize2 ) {
         /** segment - increasing element size **/
         numNodes = table[i].numNodesVar;
         curNodePos = x1;
         curElemSize = elemSize1;
         /* increasing elements */
         for( j = 0 ; j < numNodes ; j++ ) {
            newCoords[k++] = curNodePos;
            curElemSize *= self->factor;
            curNodePos += curElemSize;
         }
         curNodePos -= curElemSize;
         /* cte elements */
         numNodes = table[i].numNodesCte;
         curElemSize = table[i].cteElemSize;
         curNodePos += curElemSize;
         for( j = 0 ; j < numNodes ; j++ ) {
            newCoords[k++] = curNodePos;
            curNodePos += curElemSize;
         }
         
      }
      else {
         /** segment - decreasing element size **/
         numNodes = table[i].numNodesCte;
         curElemSize = table[i].cteElemSize;
         /* cte elements */
         curNodePos = x1;
         for( j = 0 ; j < numNodes ; j++ ) {
            newCoords[k++] = curNodePos;
            curNodePos += curElemSize;
         }
         /* increasing elements */
         numNodes = table[i].numNodesVar;
         //curElemSize = elemSize1;
         curElemSize = elemSize2 * pow( self->factor, numNodes );
         for( j = 0 ; j < numNodes ; j++ ) {
            newCoords[k++] = curNodePos;
            //curElemSize /= self->factor;
            curElemSize = elemSize2 * pow( self->factor, numNodes-j-1 );
            curNodePos += curElemSize;
         }         
      }
   }
   /* add the last node */
   newCoords[k++] = table[tableSize-1].x;
   assert( k == meshNodes );

   switch( axis ) {
      case I_AXIS:
         self->newCoordsx = newCoords;
         self->nCoordsx = totNodes;
         break;   
      case J_AXIS:
         self->newCoordsy = newCoords;
         self->nCoordsy = totNodes;
         break;
      case K_AXIS:
         self->newCoordsz = newCoords;
         self->nCoordsz = totNodes;
         break;
   }
}

/*---------------------------------------------------------------------------------------
** Constructors
*/
NodeBunching* NodeBunching_New( Name name, AbstractContext* context ) {
   /* Variables set in this function */
   SizeT                                             _sizeOfSelf = sizeof(NodeBunching);
   Type                                                     type = NodeBunching_Type;
   Stg_Class_DeleteFunction*                             _delete = _NodeBunching_Delete;
   Stg_Class_PrintFunction*                               _print = _NodeBunching_Print;
   Stg_Class_CopyFunction*                                 _copy = NULL;
   Stg_Component_DefaultConstructorFunction* _defaultConstructor = (void* (*)(Name))_NodeBunching_New;
   Stg_Component_ConstructFunction*                   _construct = _NodeBunching_AssignFromXML;
   Stg_Component_BuildFunction*                           _build = _NodeBunching_Build;
   Stg_Component_InitialiseFunction*                 _initialise = _NodeBunching_Initialise;
   Stg_Component_ExecuteFunction*                       _execute = _NodeBunching_Execute;
   Stg_Component_DestroyFunction*                       _destroy = _NodeBunching_Destroy;
   AllocationType                             nameAllocationType = NON_GLOBAL;
   MeshGenerator_SetDimSizeFunc*                  setDimSizeFunc = _MeshGenerator_SetDimSize;
   MeshGenerator_GenerateFunc*                      generateFunc = LinearSpaceAdaptor_Generate;

   NodeBunching* self = _NodeBunching_New( NODEBUNCHING_PASSARGS );

   _MeshGenerator_Init( (MeshGenerator*)self, context );
   _MeshAdaptor_Init( (MeshAdaptor*)self );
   _LinearSpaceAdaptor_Init( (LinearSpaceAdaptor*)self );
   _NodeBunching_Init( self );

   return self;
}

NodeBunching* _NodeBunching_New( NODEBUNCHING_DEFARGS ) {
   NodeBunching* self;
   
   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(NodeBunching) );
   self = (NodeBunching*)_LinearSpaceAdaptor_New( LINEARSPACEADAPTOR_PASSARGS );

   /* Virtual info */
   return self;
}

void _NodeBunching_Init( void* _self ) {
   // NodeBunching* self = (NodeBunching*)_self;
}

/*----------------------------------------------------------------------------------------
** Virtual functions
*/

void _NodeBunching_Delete( void* _self ) {
   NodeBunching* self = (NodeBunching*)_self;

   /* Delete the parent. */
   _LinearSpaceAdaptor_Delete( self );
}

void _NodeBunching_Print( void* _self, Stream* stream ) {
   NodeBunching* self = (NodeBunching*)_self;
   
   /* Set the Journal for printing informations */
   Stream* adaptorStream;
   adaptorStream = Journal_Register( InfoStream_Type, (Name)"NodeBunchingStream" );

   /* Print parent */
   Journal_Printf( stream, "NodeBunching (ptr): (%p)\n", self );
   _MeshAdaptor_Print( self, stream );
}

void NodeBunching_ReadAxisData(
   void*                    _self,
   char                     axis,
   Dictionary*              dictionary,
   int*                     nPoints,
   NodeBunching_PointData** pointsPtr,
   double                   axisMin,
   double                   axisMax )
{
   NodeBunching*           self = (NodeBunching*)_self;
   Stream*                 errorStream = Journal_Register( Error_Type, (Name)CURR_MODULE_NAME );
   Dictionary_Entry_Value* optionsList = NULL;
   Dictionary_Entry_Value* optionSet = NULL;
   NodeBunching_PointData* seg = NULL;
   NodeBunching_PointData* points = NULL;
   Index                   pointsCount, point_I;
   double                  lastx = 0;

   switch( axis ) {
      case 'x':
         optionsList = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"PointsX" );
         break;
      case 'y':
         optionsList = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"PointsY" );
         break;
      case 'z':
         optionsList = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"PointsZ" );
         break;
   }

   if( optionsList ) {
      pointsCount = Dictionary_Entry_Value_GetCount( optionsList );
      nPoints[0] = pointsCount;

      pointsPtr[0] = Memory_Alloc_Array( NodeBunching_PointData, pointsCount, "points table" );
      points = pointsPtr[0];
      memset( points, 0, pointsCount * sizeof(NodeBunching_PointData) );

      for ( point_I = 0 ; point_I < pointsCount ; point_I++) { 
         optionSet = Dictionary_Entry_Value_GetElement( optionsList, point_I );
         seg = &(points[point_I]);
         seg->x = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( optionSet, (Dictionary_Entry_Key)"Point" ) );
         seg->size = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( optionSet, (Dictionary_Entry_Key)"ElementSize" ) );

         Journal_Firewall( axisMin <= seg->x && seg->x <= axisMax, errorStream, "\n\n\n"
            "The configuration of the NodeBunching component"
            "has one point outside the domain (%f < %f < %f) in the %c axis.\n"
            "You must provide an element size for the first and last coordinate of your domain."
            "\n\n\n", axisMin, seg->x, axisMax, axis );
         Journal_Firewall( point_I == 0 || lastx < seg->x, errorStream, "\n\n\n"
            "Error in the configuration of the NodeBunching component (axis %c)\n"
            "The points must be in ascending order\n"
            "\n\n\n", axis );
         lastx = seg->x;      
      }
      Journal_Firewall( axisMin == points[0].x, errorStream, "\n\n\n"
         "Error in the configuration of the NodeBunching component (axis %c)\n"
         "You must define the element size for the first point of your domain."
         "\n\n\n", axis );
      Journal_Firewall( axisMax == points[pointsCount-1].x, errorStream, "\n\n\n"
         "Error in the configuration of the NodeBunching component (axis %c)\n"
         "You must define the element size for the last point of your domain."
         "\n\n\n", axis );
   }
   else {
      nPoints[0] = 0;
      pointsPtr[0] = NULL;
   }
}

void _NodeBunching_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
   NodeBunching* self = (NodeBunching*)_self;
   Dictionary*   dictionary = Dictionary_GetDictionary( cf->componentDict, self->name );

   assert( self );
   assert( cf );
   assert( dictionary );

   /* Call parent construct. */
   _LinearSpaceAdaptor_AssignFromXML( self, cf, data );

   if( self->loadFromCheckPoint )
      return;

   /* Element size factor */
   self->factor = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"IncreaseElementFactor", 1.5 );
   assert( self->factor > 0.0 );
   if( self->factor < 1.0 ) 
      self->factor++;

   /* Mesh info */
   self->nNodes[I_AXIS] = Dictionary_Entry_Value_AsInt( Dictionary_Get( cf->rootDict, (Dictionary_Entry_Key)"elementResI" ) ) + 1;
   self->nNodes[J_AXIS] = Dictionary_Entry_Value_AsInt( Dictionary_Get( cf->rootDict, (Dictionary_Entry_Key)"elementResJ" ) ) + 1;
   self->nNodes[K_AXIS] = Dictionary_Entry_Value_AsInt( Dictionary_Get( cf->rootDict, (Dictionary_Entry_Key)"elementResK" ) ) + 1;

   /* Read segments X-axis */
   NodeBunching_ReadAxisData( self, 'x', dictionary, &(self->nPointsx), &(self->pointsx),self->minX, self->maxX );
   NodeBunching_ReadAxisData( self, 'y', dictionary, &(self->nPointsy), &(self->pointsy),self->minY, self->maxY );
   NodeBunching_ReadAxisData( self, 'z', dictionary, &(self->nPointsz), &(self->pointsz),self->minZ, self->maxZ );

   /* Debug */
   self->debugMode = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"Debug", False );

   _NodeBunching_Init( self );
}

void NodeBunching_ReplaceAxis(
   void*                        _self,
   char                         axis,
   linearSpaceAdaptor_Segment** tablePtr,
   unsigned*                    nSegments,
   int                          nCoords,
   double*                      newCoords,
   int                          nNodes,
   double                       axisMax,
   double                       axisMin )
{
   NodeBunching*               self = (NodeBunching*)_self;
   linearSpaceAdaptor_Segment* table = NULL;
   double                      elementSize, x, y;
   Index                       I;

   /* allocate mem */
   tablePtr[0] = Memory_Alloc_Array( linearSpaceAdaptor_Segment, nCoords, "mapping table" );
   table = tablePtr[0];
   assert( table );
   /* build new table */
   nSegments[0] = nCoords;
   elementSize = (axisMax - axisMin) / (nNodes - 1);
   NodeBunching_Debug_1(self,axis);

   for( I = 1 ; I < nCoords ; I++ ) {
      /* get */
      x = axisMin + elementSize*I;
      y = newCoords[I];
      /* normalize */
      x = (x - axisMin) / (axisMax - axisMin);
      y = (y - axisMin) / (axisMax - axisMin);
      assert( 0<=x && x<=1 );
      assert( 0<=y && y<=1 );
      /* store */
      table[I].x = x;
      table[I].y = y;
      NodeBunching_Debug_2(self,axis,x,y);
   }    
   NodeBunching_Debug_3(self,axis);
   LinearSpaceAdaptor_FillTable( table, nCoords );
}

void _NodeBunching_Build( void* _self, void* data ) {
   NodeBunching* self = (NodeBunching*)_self;

   /* Compute new node positions */
   if( self->nPointsx > 0 )
      NodeBunching_newPositions( self, I_AXIS );
   if( self->nPointsy > 0 )
      NodeBunching_newPositions( self, J_AXIS );
   if( self->nPointsz > 0 )
      NodeBunching_newPositions( self, K_AXIS );

   /* Get rid of previous mapping functions */
   if( self->nSegmentsx > 0 ) {
      Memory_Free( self->tablex );
      self->nSegmentsx = 0;
   }
   if( self->nSegmentsy > 0 ) {
      Memory_Free( self->tabley );
      self->nSegmentsy = 0;
   }
   if( self->nSegmentsz > 0 ) {
      Memory_Free( self->tablez );
      self->nSegmentsz = 0;
   }

   /** Replace mapping functions with the new ones **/
   if( self->nPointsx > 0 ) 
      NodeBunching_ReplaceAxis( self, 'x', &(self->tablex), &(self->nSegmentsx), self->nCoordsx, self->newCoordsx, self->nNodes[0], self->maxX, self->minX );

   if( self->nPointsy > 0 ) 
      NodeBunching_ReplaceAxis( self, 'y', &(self->tabley), &(self->nSegmentsy), self->nCoordsy, self->newCoordsy, self->nNodes[1], self->maxY, self->minY );

   if( self->nPointsz > 0 ) 
      NodeBunching_ReplaceAxis( self, 'z', &(self->tablez), &(self->nSegmentsz), self->nCoordsz, self->newCoordsz, self->nNodes[2], self->maxZ, self->minZ );

   _LinearSpaceAdaptor_Build( self, data );
}

void _NodeBunching_Initialise( void* _self, void* data ) {
   NodeBunching* self = (NodeBunching*)_self;

   _LinearSpaceAdaptor_Initialise( self, data );
}

void _NodeBunching_Execute( void* _self, void* data ) {
}

void _NodeBunching_Destroy( void* _self, void* data ) {
   NodeBunching* self = (NodeBunching*)_self;

   if( self->nPointsx > 0 )
      Memory_Free( self->pointsx );
   if( self->nPointsy > 0 )
      Memory_Free( self->pointsy );
   if( self->nPointsz > 0 )
      Memory_Free( self->pointsz );
   
   _LinearSpaceAdaptor_Destroy( self, data );
}

/*--------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/

/*------------------------------------------------------------------------------------
** Private Functions
*/
