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
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __Underworld_Utils_BelowHeightFieldClass_h__
#define __Underworld_Utils_BelowHeightFieldClass_h__

   /* Textual name of this class */
   extern const Type BelowHeightField_Type;

   /* BelowHeightField information */
   #define __BelowHeightField \
      /* General info */ \
      __Stg_Shape \
      /* Virtual Info */\
      \
      FieldVariable*       heightField; \
      unsigned             vertAxis;

   struct BelowHeightField { __BelowHeightField };
   
   
   /*---------------------------------------------------------------------------------------------------------------------
   ** Constructors
   */
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define BELOWHEIGHTFIELD_DEFARGS \
                STG_SHAPE_DEFARGS

   #define BELOWHEIGHTFIELD_PASSARGS \
                STG_SHAPE_PASSARGS

   BelowHeightField* _BelowHeightField_New(  BELOWHEIGHTFIELD_DEFARGS  );
   
   /* Stg_Class_Delete BelowHeightField implementation */
   void _BelowHeightField_Delete( void* belowHeightField );
   void _BelowHeightField_Print( void* belowHeightField, Stream* stream );
   #define BelowHeightField_Copy( self ) \
      (BelowHeightField*) Stg_Class_Copy( self, NULL, False, NULL, NULL )
   #define BelowHeightField_DeepCopy( self ) \
      (BelowHeightField*) Stg_Class_Copy( self, NULL, True, NULL, NULL )
   void* _BelowHeightField_Copy( void* belowHeightField, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
   
   void* _BelowHeightField_DefaultNew( Name name ) ;
   void _BelowHeightField_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) ;
   void _BelowHeightField_Build( void* belowHeightField, void* data ) ;
   void _BelowHeightField_Initialise( void* belowHeightField, void* data ) ;
   void _BelowHeightField_Execute( void* belowHeightField, void* data );
   void _BelowHeightField_Destroy( void* belowHeightField, void* data ) ;
   
   Bool _BelowHeightField_IsCoordInside( void* belowHeightField, Coord coord ) ;

   void _BelowHeightField_CalculateVolume( void* belowHeightField );
   void _BelowHeightField_DistanceFromCenterAxis( void* shape, Coord coord, double* disVec );

   /*---------------------------------------------------------------------------------------------------------------------
   ** Public member functions
   */
   
   /*---------------------------------------------------------------------------------------------------------------------
   ** Private Member functions
   */
   
   
#endif 

