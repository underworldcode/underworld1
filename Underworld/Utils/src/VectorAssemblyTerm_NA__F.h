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


#ifndef __Underworld_Utils_VectorAssemblyTerm_NA__F_h__
#define __Underworld_Utils_VectorAssemblyTerm_NA__F_h__

   /** Textual name of this class */
   extern const Type VectorAssemblyTerm_NA__F_Type;

   /** VectorAssemblyTerm_NA__F class contents */
   #define __VectorAssemblyTerm_NA__F \
      /* General info */ \
      __ForceTerm \
      \
      /* Virtual info */ \
      \
      /* VectorAssemblyTerm_NA__F info */ \
      PpcManager* ppcManager; \
      Stream*     errorStream; \
      int         functionLabel;

   struct VectorAssemblyTerm_NA__F { __VectorAssemblyTerm_NA__F };


   #ifndef ZERO
   #define ZERO 0
   #endif

   #define FORCEASSEMBLYTERM_NA__FN_DEFARGS \
                FORCETERM_DEFARGS

   #define FORCEASSEMBLYTERM_NA__FN_PASSARGS \
                FORCETERM_PASSARGS

   VectorAssemblyTerm_NA__F* _VectorAssemblyTerm_NA__F_New(  FORCEASSEMBLYTERM_NA__FN_DEFARGS  );

   void _VectorAssemblyTerm_NA__F_Init( void* forceTerm, PpcManager* mgr, int functionLabel );

   void _VectorAssemblyTerm_NA__F_Delete( void* residual );

   void _VectorAssemblyTerm_NA__F_Print( void* residual, Stream* stream );

   void* _VectorAssemblyTerm_NA__F_DefaultNew( Name name );

   void _VectorAssemblyTerm_NA__F_AssignFromXML( void* residual, Stg_ComponentFactory* cf, void* data );

   void _VectorAssemblyTerm_NA__F_Build( void* residual, void* data );

   void _VectorAssemblyTerm_NA__F_Initialise( void* residual, void* data );

   void _VectorAssemblyTerm_NA__F_Execute( void* residual, void* data );

   void _VectorAssemblyTerm_NA__F_Destroy( void* residual, void* data );

   void _VectorAssemblyTerm_NA__F_AssembleElement( void* forceTerm, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elForceVec );

#endif

