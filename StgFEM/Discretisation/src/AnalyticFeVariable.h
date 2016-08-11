/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**   Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**   Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**   AuScope - http://www.auscope.org
**   Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**   Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**   Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**   Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**   Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
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
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __Discretisation_AnalyticFeVariable_h__
#define __Discretisation_AnalyticFeVariable_h__

   /* Textual name of this class */
   extern const Type AnalyticFeVariable_Type;

   /* AnalyticFeVariable contents */
   #define __AnalyticFeVariable \
      /* General info */ \
      __FeVariable \
      \
      /* Virtual info */ \
      Name        analyticFunctionName; \
      FeVariable* numericField;

   struct AnalyticFeVariable { __AnalyticFeVariable };   

   /* Public Constructor */
   void* AnalyticFeVariable_DefaultNew( Name name );
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define ANALYTICFEVARIABLE_DEFARGS \
      FEVARIABLE_DEFARGS

   #define ANALYTICFEVARIABLE_PASSARGS \
      FEVARIABLE_PASSARGS

   /* Private Constructor */
   AnalyticFeVariable* _AnalyticFeVariable_New( ANALYTICFEVARIABLE_DEFARGS );

   void _AnalyticFeVariable_Init( void* analyticFeVariable, Name functionName, FeVariable* field );

   /* 'Stg_Class' Virtual Implementations */
   void _AnalyticFeVariable_Delete( void* analyticFeVariable );

   void _AnalyticFeVariable_Print( void* _analyticFeVariable, Stream* stream );

   void* _AnalyticFeVariable_Copy( void* analyticFeVariable, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
   
   /* 'Stg_Component' Virtual Implementations */
   void _AnalyticFeVariable_AssignFromXML( void* analyticFeVariable, Stg_ComponentFactory* cf, void* data );

   void _AnalyticFeVariable_Build( void* analyticFeVariable, void* data );

   void _AnalyticFeVariable_Execute( void* analyticFeVariable, void* data );

   void _AnalyticFeVariable_Destroy( void* analyticFeVariable, void* data );

   void _AnalyticFeVariable_Initialise( void* analyticFeVariable, void* data );

#endif /* __Discretisation_AnalyticFeVariable_h__ */

