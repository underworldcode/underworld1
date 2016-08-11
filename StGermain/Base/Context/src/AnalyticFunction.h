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
**
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StGermain_Base_Context_AnalyticFunction_h__
#define __StGermain_Base_Context_AnalyticFunction_h__


   typedef void (AnalyticFunction_SolFunc) ( void* context, double* coord, double* value );
   
   extern const Type AnalyticFunction_Type;
   
   #define __AnalyticFunction \
      /* General info */ \
      __Stg_Class \
      \
      /* Virtual info */ \
      \
      /* Stg_Class info */ \
      Dictionary*               dictionary; \
      AnalyticFunction_SolFunc* apply; \
      Name                      name; \
      
   struct _AnalyticFunction { __AnalyticFunction };
   
   /*--------------------------------------------------------------------------------------------------------------------------
   ** Constructor
   */
   
   AnalyticFunction* AnalyticFunction_New( AnalyticFunction_SolFunc* apply, Name name );
   
   void AnalyticFunction_Init( AnalyticFunction* self, AnalyticFunction_SolFunc* apply, Name name );
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define ANALYTICFUNCTION_DEFARGS \
      STG_CLASS_DEFARGS, \
      AnalyticFunction_SolFunc* apply, \
      Name                      name

   #define ANALYTICFUNCTION_PASSARGS \
      STG_CLASS_PASSARGS, \
      apply, \
      name 

   AnalyticFunction* _AnalyticFunction_New( ANALYTICFUNCTION_DEFARGS );
   
   void _AnalyticFunction_Init( void* analyticFunction, AnalyticFunction_SolFunc* apply, Name name );
   
   
   /*--------------------------------------------------------------------------------------------------------------------------
   ** General virtual functions
   */

   void _AnalyticFunction_Delete( void* analyticFunction );
   
   void _AnalyticFunction_Print( void* analyticFunction, Stream* stream );
   
   
   /*--------------------------------------------------------------------------------------------------------------------------
   ** Macros
   */
   
   #define AnalyticFunction_Apply(self, coord, value) \
      (self)->apply((self), (coord), (value))
   
   
   /*--------------------------------------------------------------------------------------------------------------------------
   ** Virtual functions
   */
   
   
   /*--------------------------------------------------------------------------------------------------------------------------
   ** Build functions
   */
   
   
   /*--------------------------------------------------------------------------------------------------------------------------
   ** Functions
   */
   
   /* Apply the condition function */
   void _AnalyticFunction_Apply( void* analyticFunction, double* coord, double* value ); 

   void AnalyticFunction_Add( void* analyticFunction, Name functionName );

#endif /* __StGermain_Base_Context_AnalyticFunction_h__ */

