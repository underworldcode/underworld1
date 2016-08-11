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
**  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
**
**
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StGermain_Base_Context_AnalyticFunction_Register_h__
#define __StGermain_Base_Context_AnalyticFunction_Register_h__
   
   extern const Type AnalyticFunction_Register_Type;

   /* Global default instantiation of the register, created in Init.c */
   extern AnalyticFunction_Register* analyticFunc_Register;   
   
   #define __AnalyticFunction_Register \
      /* General info */ \
      __Stg_Class \
      \
      /* Virtual info */ \
      \
      /* Stg_Class info */ \
      AnalyticFunction_Index count; \
      SizeT                  _size; \
      SizeT                  _delta; \
      AnalyticFunction**     _af; 
      
   struct _AnalyticFunction_Register { __AnalyticFunction_Register };
   
   
   /*--------------------------------------------------------------------------------------------------------------------------
   ** Constructor
   */
   
   AnalyticFunction_Register* AnalyticFunction_Register_New(void);
   
   void AnalyticFunction_Register_Init(AnalyticFunction_Register* self);
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define ANALYTICFUNCTION_REGISTER_DEFARGS \
      STG_CLASS_DEFARGS

   #define ANALYTICFUNCTION_REGISTER_PASSARGS \
      STG_CLASS_PASSARGS

   AnalyticFunction_Register* _AnalyticFunction_Register_New( ANALYTICFUNCTION_REGISTER_DEFARGS );
      
   void _AnalyticFunction_Register_Init( void* analyticFunction_Register );
   
   /*--------------------------------------------------------------------------------------------------------------------------
   ** General virtual functions
   */
   
   void _AnalyticFunction_Register_Delete( void* analyticFunction_Register );
   
   void _AnalyticFunction_Register_Print( void* analyticFunction_Register, Stream* stream );
   
   /*--------------------------------------------------------------------------------------------------------------------------
   ** Macros
   */

   /*--------------------------------------------------------------------------------------------------------------------------
   ** Virtual functions
   */

   /*--------------------------------------------------------------------------------------------------------------------------
   ** Build functions
   */
   
   AnalyticFunction_Index AnalyticFunction_Register_Add(
      void*             analyticFunction_Register,
      AnalyticFunction* cf );

   /*--------------------------------------------------------------------------------------------------------------------------
   ** Functions
   */
   
   AnalyticFunction_Index AnalyticFunction_Register_GetIndex(
      void* analyticFunction_Register,
      Name  name );

   AnalyticFunction* AnalyticFunction_Register_GetByName(
      void* analyticFunction_Register,
      Name  name );

   void AnalyticFunction_Register_PrintNameOfEachFunc(
      void*   analyticFunction_Register,
      Stream* stream );

   
#endif /* __StGermain_Base_Context_AnalyticFunction_Register_h__ */

