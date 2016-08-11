/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**	Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**	Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**	AuScope - http://www.auscope.org
**	Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**	Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**	Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**	Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**	Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
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
**	Abstract class defining the interface for a System of Linear Equations solver.
**
** Assumptions:
**
** Comments:
**	Note - as 1 September 2004 (rev 1994), the functioality for building Matrices etc
**	that was in this class is back in the SemiLagrangianIntegrator class.
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgFEM_Utils_SemiLagrangianIntegrator_h__
#define __StgFEM_Utils_SemiLagrangianIntegrator_h__

   /** Textual name of this class */
   extern const Type SemiLagrangianIntegrator_Type;

   /** SemiLagrangianIntegrator class contents */
   #define __SemiLagrangianIntegrator \
      __Stg_Component \
      \
      /* SemiLagrangianIntegrator info */ \
      FeVariable*                  velocityField;		     \
      Stg_ObjectList*              variableList;       	  \
      Stg_ObjectList*              varStarList;  		     \
      Stg_ObjectList*              varOldList;  	     	  \
      FiniteElementContext*        context;             	  \
      FeVariable*		              advectedField;		     \
      /* for problems with temporally evolving velocity */ \
      FeVariable*		              prevVelField;           \
      Energy_SLE*                  sle;

   /** Abstract class defining the interface for a SemiLagrangianIntegrator solver - see SemiLagrangianIntegrator.h */
   struct SemiLagrangianIntegrator { __SemiLagrangianIntegrator };

   /* --- Constructor functions --- */
   void* _SemiLagrangianIntegrator_DefaultNew( Name name );

   /** Creation implementation */

   #ifndef ZERO
   #define ZERO 0
   #endif

   #define SEMILAGRANGIANINTEGRATOR_DEFARGS \
                STG_COMPONENT_DEFARGS

   #define SEMILAGRANGIANINTEGRATOR_PASSARGS \
                STG_COMPONENT_PASSARGS

   SemiLagrangianIntegrator* _SemiLagrangianIntegrator_New(  SEMILAGRANGIANINTEGRATOR_DEFARGS  );

   /* --- Virtual function implementations --- */

   /** Class Virtual Functions Implementations */
   void* _SemiLagrangianIntegrator_Copy( void* slIntegrator, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
   void _SemiLagrangianIntegrator_Delete( void* slIntegrator );
   void _SemiLagrangianIntegrator_Print( void* slIntegrator, Stream* stream );

   /** Stg_Component_Build() implementation: does nothing by default as some solvers may not need it. */
   void _SemiLagrangianIntegrator_Build( void* slIntegrator, void* data );

   void _SemiLagrangianIntegrator_AssignFromXML( void* slIntegrator, Stg_ComponentFactory* cf, void* data );

   /** Stg_Component_Initialise() implementation: does nothing by default as some solvers may not neet it. */
   void _SemiLagrangianIntegrator_Initialise( void* slIntegrator, void* data );

   /** Stg_Component_Execute() implementation: Calls SolverSetup() for any per-iteration setup, then
   calls Solve() to calculate the new solutions. */
   void _SemiLagrangianIntegrator_Execute( void* slIntegrator, void* data );

   void _SemiLagrangianIntegrator_Destroy( void* slIntegrator, void* data );

   /* --- Private Functions --- */
   void SemiLagrangianIntegrator_InitSolve( void* slIntegrator, void* data );
   void SemiLagrangianIntegrator_UpdatePreviousVelocityField( void* slIntegrator, void* data );

   /* --- Public functions --- */
   void BicubicInterpolator( FeVariable* feVariable, double* coord, double* delta, unsigned* nNodes, double* result );
   Bool PeriodicUpdate( double* pos, double* min, double* max, unsigned dim, Bool isPeriodic );
   void InterpLagrange( double x, double* coords, double** values, unsigned numdofs, double* result );
   void IntegrateRungeKutta( FeVariable* velocityField, double dt, double* origin, double* position );
   void IntegrateRungeKutta_VariableVelocity( FeVariable* velocityField, FeVariable* prevVelField, double dt, double* origin, double* position );

   /** Does any required solver setup beyond assembly of the matrices to be solved: e.g. priming the Matrix solvers
   etc. */

   /** Solve:- calculate the new values for all solution vectors in the system. */
   void SemiLagrangianIntegrator_Solve( void* slIntegrator, FeVariable* variableField, FeVariable* variableFieldPrime );
#endif

