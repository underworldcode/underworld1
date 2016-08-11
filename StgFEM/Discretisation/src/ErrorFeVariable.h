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

#ifndef __Discretisation_ErrorFeVariable_h__
#define __Discretisation_ErrorFeVariable_h__

   /* Textual name of this class */
   extern const Type ErrorFeVariable_Type;

   typedef struct advent {
      double time;
      double place[3];
      double value[9];
   } Advent;

   /* ErrorFeVariable contents */
   #define __ErrorFeVariable \
      /* General info */ \
      __FeVariable \
      FeVariable* numericField; \
      FeMesh*     constantMesh;

   struct ErrorFeVariable { __ErrorFeVariable };   

   /* Public Constructor */
   void* ErrorFeVariable_DefaultNew( Name name );
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define ERRORFEVARIABLE_DEFARGS \
      FEVARIABLE_DEFARGS

   #define ERRORFEVARIABLE_PASSARGS \
      FEVARIABLE_PASSARGS

   /* Private Constructor */
   ErrorFeVariable* _ErrorFeVariable_New( ERRORFEVARIABLE_DEFARGS );

   void _ErrorFeVariable_Init( void* errorFeVariable, FeVariable* numericField, FeMesh* constantMesh );

   /* 'Stg_Class' Virtual Implementations */
   void _ErrorFeVariable_Delete( void* errorFeVariable );

   void _ErrorFeVariable_Print( void* errorFeVariable, Stream* stream );

   void* _ErrorFeVariable_Copy( void* errorFeVariable, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
   
   /* 'Stg_Component' Virtual Implementations */
   void _ErrorFeVariable_AssignFromXML( void* errorFeVariable, Stg_ComponentFactory* cf, void* data );

   void _ErrorFeVariable_Build( void* errorFeVariable, void* data );

   void _ErrorFeVariable_Execute( void* errorFeVariable, void* data );

   void _ErrorFeVariable_Destroy( void* errorFeVariable, void* data );

   void _ErrorFeVariable_Initialise( void* errorFeVariable, void* data );

#endif /* __Discretisation_ErrorFeVariable_h__ */

