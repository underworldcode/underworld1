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
**	Sets up and manages a Stokes system of linear equations.
**
** Assumptions:
**
** Comments:
**	The particular solver algorithm must be provided through the Solver Stg_Component that the user chooses
**	More on the system we are solving:
**	Ku + Grad p = f
**	Div u + C p = h
**	If C isn't provided, we assume its an incompressible system, and adjust accordingly.
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgFEM_SLE_ProvidedSystems_StokesFlow_SLE_h__
#define __StgFEM_SLE_ProvidedSystems_StokesFlow_SLE_h__

	/** Textual name of this class */
	extern const Type Stokes_SLE_Type;

	/** Stokes_SLE class contents */
	#define __Stokes_SLE \
		/* General info */ \
		__SystemLinearEquations \
		\
		/* Virtual info */ \
		\
		/* Stokes_SLE info */ \
		\
		StiffnessMatrix*	kStiffMat;	/** Stress tensor matrix */ \
		StiffnessMatrix*	gStiffMat;	/** Gradient matrix */ \
		StiffnessMatrix*	dStiffMat;	/** Divergence matrix */ \
		StiffnessMatrix*	cStiffMat;	/** Compressibility matrix */\
		SolutionVector*	uSolnVec;	/** velocity vector */\
		Vec	null_vector;	/** null vector */\
		SolutionVector*	pSolnVec;	/** pressure vector */\
		ForceVector*		fForceVec;	/** forcing term vector */\
		ForceVector*		hForceVec;	/** continuity force vector */\
		/* the following are to help choose a "fudge" factor to remove null-space from Jacobian in rheology */\
		double fnorm; /* current residual of rhs of Jacobian system J*dx=-F */\
		double knorm; /* current norm of stiffness matrix from Jacobian */

	struct Stokes_SLE { __Stokes_SLE };	

	Stokes_SLE* Stokes_SLE_New( 		
		Name							name,
		FiniteElementContext*	context,
		SLE_Solver*					solver,
		Bool							removeBCs,
		Bool							isNonLinear,
		double						nonLinearTolerance,
		Iteration_Index			nonLinearMaxIterations,
		Bool							killNonConvergent,			
		EntryPoint_Register*		entryPoint_Register,
		MPI_Comm						comm,
		StiffnessMatrix*			kStiffMat,
		StiffnessMatrix*			gStiffMat,
		StiffnessMatrix*			dStiffMat,
		StiffnessMatrix*			cStiffMat,
		SolutionVector*			uSolnVec,
		SolutionVector*			pSolnVec,
		ForceVector*				fForceVec,
		ForceVector*				hForceVec );

	/* Creation implementation / Virtual constructor */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define STOKES_SLE_DEFARGS \
                SYSTEMLINEAREQUATIONS_DEFARGS

	#define STOKES_SLE_PASSARGS \
                SYSTEMLINEAREQUATIONS_PASSARGS

	Stokes_SLE* _Stokes_SLE_New(  STOKES_SLE_DEFARGS  );

	void _Stokes_SLE_Init( 		
		void*					sle, 
		StiffnessMatrix*	kStiffMat,
		StiffnessMatrix*	gStiffMat,
		StiffnessMatrix*	dStiffMat,
		StiffnessMatrix*	cStiffMat,
		SolutionVector*	uSolnVec,
		SolutionVector*	pSolnVec,
		ForceVector*		fForceVec,
		ForceVector*		hForceVec ) ;

	void _Stokes_SLE_Print( void* stokesSleSolver, Stream* stream );

	void* _Stokes_SLE_DefaultNew( Name name );

	void _Stokes_SLE_AssignFromXML( void* sle, Stg_ComponentFactory* cf, void* data );
	
	void _Stokes_SLE_MG_SelectStiffMats( void* _sle, unsigned* nSMs, StiffnessMatrix*** sms );
	
#endif

