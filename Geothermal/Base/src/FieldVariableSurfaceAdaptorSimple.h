/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
** Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
** Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
** Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
** Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
** Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
** Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __Geothermal_FieldVariableSurfaceAdaptorSimple_h__
#define __Geothermal_FieldVariableSurfaceAdaptorSimple_h__

   /** Textual name of this class */
   extern const Type FieldVariableSurfaceAdaptorSimple_Type;

   /** Virtual function types */

   #define __FieldVariableSurfaceAdaptorSimple        \
      /* General info */                              \
      __MeshAdaptor                                   \
                                                      \
      /* Virtual info */                              \
                                                      \
      /* FieldVariableSurfaceAdaptorSimple info */    \
		int               contactDepth;               	\
      FieldVariable*    topHeightField;               \
      FieldVariable*    bottomHeightField;            \
      double            elDilatePower;                \
      double            minElSize[3];                 \
      Bool              adaptForMat;                  \
      Bool              elBunching;                   \


   struct FieldVariableSurfaceAdaptorSimple { __FieldVariableSurfaceAdaptorSimple };

   /*--------------------------------------------------------------------------------------------------------------------------
   ** Constructors
   */




   #ifndef ZERO
   #define ZERO 0
   #endif

   #define FIELDVARIABLESURFACEADAPTORSIMPLE_DEFARGS \
                MESHGENERATOR_DEFARGS

   #define FIELDVARIABLESURFACEADAPTORSIMPLE_PASSARGS \
                MESHGENERATOR_PASSARGS

   FieldVariableSurfaceAdaptorSimple* FieldVariableSurfaceAdaptorSimple_New( Name name );
   FieldVariableSurfaceAdaptorSimple* _FieldVariableSurfaceAdaptorSimple_New(  FIELDVARIABLESURFACEADAPTORSIMPLE_DEFARGS  );

   /*--------------------------------------------------------------------------------------------------------------------------
   ** Virtual functions
   */

   void _FieldVariableSurfaceAdaptorSimple_Delete( void* adaptor );
   void _FieldVariableSurfaceAdaptorSimple_Print( void* adaptor, Stream* stream );
   void _FieldVariableSurfaceAdaptorSimple_AssignFromXML( void* adaptor, Stg_ComponentFactory* cf, void* data );
   void _FieldVariableSurfaceAdaptorSimple_Build( void* adaptor, void* data );
   void _FieldVariableSurfaceAdaptorSimple_Initialise( void* adaptor, void* data );
   void _FieldVariableSurfaceAdaptorSimple_Execute( void* adaptor, void* data );
   void _FieldVariableSurfaceAdaptorSimple_Destroy( void* adaptor, void* data );

   void FieldVariableSurfaceAdaptorSimple_MeshAdapt_Height( FieldVariableSurfaceAdaptorSimple* self, FeMesh* mesh, double* min, double* max );
   void FieldVariableSurfaceAdaptorSimple_MeshAdapt_Sides( FieldVariableSurfaceAdaptorSimple* self, FeMesh* mesh, double* min, double* max, unsigned dim );
   void FieldVariableSurfaceAdaptorSimple_Generate( void* adaptor, void* _mesh, void* data );
   void FieldVariableSurfaceAdaptorSimple_SurfaceAdapt( void* adaptor, void* _mesh, void* data );

   /*--------------------------------------------------------------------------------------------------------------------------
   ** Public functions
   */

   /*--------------------------------------------------------------------------------------------------------------------------
   ** Private Member functions
   */

   double FieldVariableSurfaceAdaptorSimple_InterpolateFromHeightMap( FieldVariableSurfaceAdaptorSimple* self, Mesh* mesh, unsigned vertex );

#endif /* __Geothermal_FieldVariableSurfaceAdaptorSimple_h__ */

