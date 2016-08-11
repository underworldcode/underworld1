/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
** Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
** Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
** AuScope - http://www.auscope.org
** Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
** Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
** Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
** Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
** Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
** David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
** Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
** Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
** Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
** Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
** Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
** Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
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


#ifndef __Underworld_Utils_VectorAssemblyTerm_NA_i__Fi_h__
#define __Underworld_Utils_VectorAssemblyTerm_NA_i__Fi_h__

   /** Textual name of this class */
   extern const Type VectorAssemblyTerm_NA_i__Fi_Type;

   /** VectorAssemblyTerm_NA_i__Fi class contents */
   #define __VectorAssemblyTerm_NA_i__Fi \
      /* General info */ \
      __ForceTerm \
      \
      /* Virtual info */ \
      \
      /* VectorAssemblyTerm_NA_i__Fi info */ \
      PpcManager* ppcManager; \
      Stream*     errorStream; \
      int         functionLabel;

   struct VectorAssemblyTerm_NA_i__Fi { __VectorAssemblyTerm_NA_i__Fi };


   #ifndef ZERO
   #define ZERO 0
   #endif

   #define VECTORASSEMBLYTERM_NA_I__FI_DEFARGS \
                FORCETERM_DEFARGS

   #define VECTORASSEMBLYTERM_NA_I__FI_PASSARGS \
                FORCETERM_PASSARGS

   VectorAssemblyTerm_NA_i__Fi* _VectorAssemblyTerm_NA_i__Fi_New(  VECTORASSEMBLYTERM_NA_I__FI_DEFARGS  );

   void _VectorAssemblyTerm_NA_i__Fi_Init( void* forceTerm, PpcManager* mgr, int functionLabel );

   void _VectorAssemblyTerm_NA_i__Fi_Delete( void* residual );

   void _VectorAssemblyTerm_NA_i__Fi_Print( void* residual, Stream* stream );

   void* _VectorAssemblyTerm_NA_i__Fi_DefaultNew( Name name );

   void _VectorAssemblyTerm_NA_i__Fi_AssignFromXML( void* residual, Stg_ComponentFactory* cf, void* data );

   void _VectorAssemblyTerm_NA_i__Fi_Build( void* residual, void* data );

   void _VectorAssemblyTerm_NA_i__Fi_Initialise( void* residual, void* data );

   void _VectorAssemblyTerm_NA_i__Fi_Execute( void* residual, void* data );

   void _VectorAssemblyTerm_NA_i__Fi_Destroy( void* residual, void* data );

   void _VectorAssemblyTerm_NA_i__Fi_AssembleElement( void* forceTerm, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elForceVec );

#endif

