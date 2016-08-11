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

#ifndef __Discretisation_ShapeFeVariable_h__
#define __Discretisation_ShapeFeVariable_h__

   /* Textual name of this class */
   extern const Type ShapeFeVariable_Type;

   /* ShapeFeVariable contents */
   #define __ShapeFeVariable \
      /* General info */ \
      __FeVariable \
      \
      /* Virtual info */ \
      Stg_Shape* shape;

   struct ShapeFeVariable { __ShapeFeVariable };   

   /* Public Constructor */

   void* ShapeFeVariable_DefaultNew( Name name );
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define SHAPEFEVARIABLE_DEFARGS \
      FEVARIABLE_DEFARGS

   #define SHAPEFEVARIABLE_PASSARGS \
      FEVARIABLE_PASSARGS

   /* Private Constructor */
   ShapeFeVariable* _ShapeFeVariable_New( SHAPEFEVARIABLE_DEFARGS );

   /* 'Stg_Class' Virtual Implementations */
   void _ShapeFeVariable_Delete( void* variable );

   void _ShapeFeVariable_Print( void* _swarmVariable, Stream* stream );

   void* _ShapeFeVariable_Copy( void* swarmVariable, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
   
   /* 'Stg_Component' Virtual Implementations */
   void _ShapeFeVariable_AssignFromXML( void* swarmVariable, Stg_ComponentFactory* cf, void* data );

   void _ShapeFeVariable_Build( void* swarmVariable, void* data );

   void _ShapeFeVariable_Execute( void* variable, void* data );

   void _ShapeFeVariable_Destroy( void* variable, void* data );

   void _ShapeFeVariable_Initialise( void* variable, void* data );

   /* FeVariable Virtual Implementations */
   
   /* Private Functions */

#endif /* __Discretisation_ShapeFeVariable_h__ */

