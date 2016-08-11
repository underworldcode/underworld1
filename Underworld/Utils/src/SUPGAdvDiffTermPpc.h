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
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
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


#ifndef __Underworld_SUPGAdvDiffTermPpc_h__
#define __Underworld_SUPGAdvDiffTermPpc_h__

   typedef double (SUPGAdvDiffTermPpc_UpwindParamFunction)( void* residual, double pecletNumber );
   
   typedef double (SUPGAdvDiffTermPpc_GetDiffusivityFromIntPoint)( void* self, void* lParticle_I );

   /** Textual name of this class */
   extern const Type SUPGAdvDiffTermPpc_Type;

   /** SUPGAdvDiffTermPpc class contents */
   #define __SUPGAdvDiffTermPpc \
      /* General info */ \
      __AdvDiffResidualForceTerm \
      \
      /* Virtual info */ \
      /* SUPGAdvDiffTermPpc info */ \
      FeVariable*            phiField; \
      AdvectionDiffusionSLE* sle; \
      PpcManager*            mgr; \
      int                    diffusivityLabel; \

   struct SUPGAdvDiffTermPpc { __SUPGAdvDiffTermPpc };   

   #ifndef ZERO
   #define ZERO 0
   #endif

   #define SUPGADVDIFFTERM_DEFARGS \
      ADVDIFFRESIDUALFORCETERM_DEFARGS

   #define SUPGADVDIFFTERM_PASSARGS \
      ADVDIFFRESIDUALFORCETERM_PASSARGS

   SUPGAdvDiffTermPpc* _SUPGAdvDiffTermPpc_New( SUPGADVDIFFTERM_DEFARGS );

   void _SUPGAdvDiffTermPpc_Init(
      void*                  residual,
      PpcManager*            mgr,
      AdvectionDiffusionSLE* sle,
      FeVariable*            velocityField,
      FeVariable*            temperatureField,
      char*                  upwindParamFuncName,
      int                    diffusivityLabel );
   
   void _SUPGAdvDiffTermPpc_Delete( void* residual );

   void* _SUPGAdvDiffTermPpc_DefaultNew( Name name );

   void _SUPGAdvDiffTermPpc_AssignFromXML( void* residual, Stg_ComponentFactory* cf, void* data );

   void _SUPGAdvDiffTermPpc_Build( void* residual, void* data );

   void _SUPGAdvDiffTermPpc_Initialise( void* residual, void* data );

   void _SUPGAdvDiffTermPpc_Execute( void* residual, void* data );

   void _SUPGAdvDiffTermPpc_Destroy( void* residual, void* data );

   void _SUPGAdvDiffTermPpc_AssembleElement( 
      void*              forceTerm, 
      ForceVector*       forceVector, 
      Element_LocalIndex lElement_I, 
      double*            elementResidual );

   double SUPGAdvDiffTermPpc_UpwindDiffusivity( 
      SUPGAdvDiffTermPpc*    self, 
      AdvectionDiffusionSLE* sle, 
      Swarm*                 swarm, 
      FeMesh*                mesh, 
      Element_LocalIndex     lElement_I, 
      Dimension_Index        dim );

   double SUPGAdvDiffTermPpc_UpwindXiExact( void* residual, double pecletNumber );

   double SUPGAdvDiffTermPpc_UpwindXiDoublyAsymptoticAssumption( void* residual, double pecletNumber );

   double SUPGAdvDiffTermPpc_UpwindXiCriticalAssumption( void* residual, double pecletNumber );

   double _SUPGAdvDiffTermPpc_UpwindParam( void* residual, double pecletNumber );

#endif

