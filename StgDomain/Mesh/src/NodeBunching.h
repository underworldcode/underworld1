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
**  This library is free software; you can redistribute it and/or
**  modify it under the terms of the GNU Lesser General Public
**  License as published by the Free Software Foundation; either
**  version 2.1 of the License, or (at your option) any later version.
**
**  This library is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public
**  License along with this library; if not, write to the Free Software
**  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
**
*/
/** \file
 **  Role:
 **
 ** Assumptions:
 **
 ** Invariants:
 **
 ** Comments:
 **
 **
 **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifndef __StgDomain_Mesh_NodeBunching_h__
#define __StgDomain_Mesh_NodeBunching_h__

   /** Textual name of this class */
   extern const Type NodeBunching_Type;

   /* The information provided by the user is stored 
      in one array (per axis) of the following struct */
   typedef struct {
     double x; /* at position x */ 
     double size; /* the element size is */
     int    numNodesCte;
     int    numNodesVar;
     double cteElemSize;
   } NodeBunching_PointData;

   /** Virtual function types */
   #define __NodeBunching \
      /* General info */ \
      __LinearSpaceAdaptor \
      /* user provided info */ \
      int                     nPointsx; \
      int                     nPointsy; \
      int                     nPointsz; \
      NodeBunching_PointData* pointsx; \
      NodeBunching_PointData* pointsy; \
      NodeBunching_PointData* pointsz; \
      /* new coords */ \
      double*                 newCoordsx; \
      double*                 newCoordsy; \
      double*                 newCoordsz; \
      int                     nCoordsx; \
      int                     nCoordsy; \
      int                     nCoordsz; \
      double                  factor; \
      /* mesh info */ \
      int                     nNodes[3]; \
      /* debug */ \
      Bool                    debugMode; \

   struct NodeBunching { __NodeBunching };

   /*----------------------------------------------------------------------------------
   ** Constructors
   */

   #ifndef ZERO
   #define ZERO 0
   #endif

   #define NODEBUNCHING_DEFARGS \
      LINEARSPACEADAPTOR_DEFARGS

   #define NODEBUNCHING_PASSARGS \
      LINEARSPACEADAPTOR_PASSARGS

   NodeBunching* NodeBunching_New( Name name, AbstractContext* context );
   NodeBunching* _NodeBunching_New( NODEBUNCHING_DEFARGS );
   void _NodeBunching_Init( void* _self );

   /*-----------------------------------------------------------------------------------
   ** Virtual functions
   */

   void _NodeBunching_Delete( void* _self );
   void _NodeBunching_Print( void* _self, Stream* stream );
   void _NodeBunching_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
   void _NodeBunching_Build( void* _self, void* data );
   void _NodeBunching_Initialise( void* _self, void* data );
   void _NodeBunching_Execute( void* _self, void* data );
   void _NodeBunching_Destroy( void* _self, void* data );
   
   /*------------------------------------------------------------------------------------
   ** Public functions
   */

   /*------------------------------------------------------------------------------------
   ** Private Member functions
   */

#endif /* __StgDomain_Mesh_NodeBunching_h__ */

