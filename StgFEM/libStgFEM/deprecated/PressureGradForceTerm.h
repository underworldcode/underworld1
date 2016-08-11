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


#ifndef __StgFEM_Assembly_PressureGradForceTerm_h__
#define __StgFEM_Assembly_PressureGradForceTerm_h__

	/** Textual name of this class */
	extern const Type PressureGradForceTerm_Type;

	/** PressureGradForceTerm class contents */
	#define __PressureGradForceTerm \
		/* General info */ \
		__ForceTerm \
		\
		/* Virtual info */ \
		\
		/* PressureGradForceTerm info */ \
		Assembler*		asmb; \
		FeVariable*		pressureField; \
		FeVariable*		gradField; \
		double*			elForceVec; \
		double			factor;

	struct PressureGradForceTerm { __PressureGradForceTerm };

	PressureGradForceTerm* PressureGradForceTerm_New( 
		Name							name,	
		FiniteElementContext*	context,
		ForceVector*				forceVector,
		Swarm*						integrationSwarm,
		FeVariable*					pressureField, 
		FeVariable*					gradField );

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PRESSUREGRADFORCETERM_DEFARGS \
                FORCETERM_DEFARGS

	#define PRESSUREGRADFORCETERM_PASSARGS \
                FORCETERM_PASSARGS

	PressureGradForceTerm* _PressureGradForceTerm_New(  PRESSUREGRADFORCETERM_DEFARGS  );

	void _PressureGradForceTerm_Init( void* forceTerm, FeVariable* pressureField, FeVariable* gradField );
	
	void _PressureGradForceTerm_Delete( void* forceTerm );

	void _PressureGradForceTerm_Print( void* forceTerm, Stream* stream );

	void* _PressureGradForceTerm_DefaultNew( Name name ) ;

	void _PressureGradForceTerm_AssignFromXML( void* forceTerm, Stg_ComponentFactory* cf, void* data );

	void _PressureGradForceTerm_Build( void* forceTerm, void* data ) ;

	void _PressureGradForceTerm_Initialise( void* forceTerm, void* data ) ;

	void _PressureGradForceTerm_Execute( void* forceTerm, void* data ) ;

	void _PressureGradForceTerm_Destroy( void* forceTerm, void* data ) ;

	void _PressureGradForceTerm_AssembleElement( void* forceTerm, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elForceVec ) ;

	Bool PressureGradForceTerm_RowCB( PressureGradForceTerm* self, Assembler* assm );

	Bool PressureGradForceTerm_ColCB( PressureGradForceTerm* self, Assembler* assm );

#endif

