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

#ifndef __Underworld_Utils_BetweenHeightFieldsClass_h__
#define __Underworld_Utils_BetweenHeightFieldsClass_h__

   /* Textual name of this class */
   extern const Type BetweenHeightFields_Type;

   /* BetweenHeightFields information */
   #define __BetweenHeightFields              \
      /* General info */                      \
      __Stg_Shape                             \
      /* Virtual Info */                      \
                                              \
      FieldVariable*       topHeightField;    \
      FieldVariable*       bottomHeightField; \
      unsigned             vertAxis;

   struct BetweenHeightFields { __BetweenHeightFields };
   
   
   /*---------------------------------------------------------------------------------------------------------------------
   ** Constructors
   */
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define BETWEENHEIGHTFIELDS_DEFARGS \
                STG_SHAPE_DEFARGS

   #define BETWEENHEIGHTFIELDS_PASSARGS \
                STG_SHAPE_PASSARGS

   BetweenHeightFields* _BetweenHeightFields_New(  BETWEENHEIGHTFIELDS_DEFARGS  );
   
   /* Stg_Class_Delete BetweenHeightFields implementation */
   void _BetweenHeightFields_Delete( void* betweenHeightFields );
   void _BetweenHeightFields_Print( void* betweenHeightFields, Stream* stream );
   #define BetweenHeightFields_Copy( self ) \
      (BetweenHeightFields*) Stg_Class_Copy( self, NULL, False, NULL, NULL )
   #define BetweenHeightFields_DeepCopy( self ) \
      (BetweenHeightFields*) Stg_Class_Copy( self, NULL, True, NULL, NULL )
   void* _BetweenHeightFields_Copy( void* betweenHeightFields, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
   
   void* _BetweenHeightFields_DefaultNew( Name name ) ;
   void _BetweenHeightFields_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) ;
   void _BetweenHeightFields_Build( void* betweenHeightFields, void* data ) ;
   void _BetweenHeightFields_Initialise( void* betweenHeightFields, void* data ) ;
   void _BetweenHeightFields_Execute( void* betweenHeightFields, void* data );
   void _BetweenHeightFields_Destroy( void* betweenHeightFields, void* data ) ;
   
   Bool _BetweenHeightFields_IsCoordInside( void* betweenHeightFields, Coord coord ) ;

   double _BetweenHeightFields_CalculateVolume( void* betweenHeightFields );
   void _BetweenHeightFields_DistanceFromCenterAxis( void* shape, Coord coord, double* disVec );

   /*---------------------------------------------------------------------------------------------------------------------
   ** Public member functions
   */
   
   /*---------------------------------------------------------------------------------------------------------------------
   ** Private Member functions
   */
   
   
#endif 

