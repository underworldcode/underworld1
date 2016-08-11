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


#ifndef __StgFEM_Assembly_GradientStiffnessMatrixTerm_h__
#define __StgFEM_Assembly_GradientStiffnessMatrixTerm_h__

	/** Textual name of this class */
	extern const Type GradientStiffnessMatrixTerm_Type;

	/** GradientStiffnessMatrixTerm class contents */
	#define __GradientStiffnessMatrixTerm \
		/* General info */ \
		__StiffnessMatrixTerm \
		/* Virtual info */ \
		/* GradientStiffnessMatrixTerm info */ \
		int		max_nElNodes_col;    \
		double	*Ni_col;

	struct GradientStiffnessMatrixTerm { __GradientStiffnessMatrixTerm };

	GradientStiffnessMatrixTerm* GradientStiffnessMatrixTerm_New( 
		Name							name,
		FiniteElementContext*	context,
		StiffnessMatrix*			stiffnessMatrix,
		Swarm*						integrationSwarm );

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define GRADIENTSTIFFNESSMATRIXTERM_DEFARGS \
                STIFFNESSMATRIXTERM_DEFARGS

	#define GRADIENTSTIFFNESSMATRIXTERM_PASSARGS \
                STIFFNESSMATRIXTERM_PASSARGS

	GradientStiffnessMatrixTerm* _GradientStiffnessMatrixTerm_New(  GRADIENTSTIFFNESSMATRIXTERM_DEFARGS  );

	void _GradientStiffnessMatrixTerm_Init( void* matrixTerm );
	
	void _GradientStiffnessMatrixTerm_Delete( void* matrixTerm );

	void _GradientStiffnessMatrixTerm_Print( void* matrixTerm, Stream* stream );

	void* _GradientStiffnessMatrixTerm_DefaultNew( Name name );

	void _GradientStiffnessMatrixTerm_AssignFromXML( void* matrixTerm, Stg_ComponentFactory* cf, void* data );

	void _GradientStiffnessMatrixTerm_Build( void* matrixTerm, void* data );

	void _GradientStiffnessMatrixTerm_Initialise( void* matrixTerm, void* data );

	void _GradientStiffnessMatrixTerm_Execute( void* matrixTerm, void* data );

	void _GradientStiffnessMatrixTerm_Destroy( void* matrixTerm, void* data );

	void _GradientStiffnessMatrixTerm_AssembleElement( 
		void*							matrixTerm,
		StiffnessMatrix*			stiffnessMatrix, 
		Element_LocalIndex		lElement_I, 
		SystemLinearEquations*	sle,
		FiniteElementContext*	context,
		double**						elStiffMat ) ;

#endif

