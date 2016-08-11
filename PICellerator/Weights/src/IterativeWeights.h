/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**      Melbourne, 3053, Australia.
** Copyright (c) 2005-2010, Monash University, Building 28, Monash University Clayton Campus,
**      Victoria, 3800, Australia
**
** Primary Contributing Organisations:
**      Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**      AuScope - http://www.auscope.org
**      Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**
** Contributors:
**      Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**      Patrick D. Sunter, Software Engineer, VPAC. (patrick@vpac.org)
**      Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**      Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**      David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**      Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
**      Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**      Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**      Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**      Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**      David Stegman, Postdoctoral Fellow, Monash University. (david.stegman@sci.monash.edu.au)
**      Wendy Sharples, PhD Student, Monash University (wendy.sharples@sci.monash.edu.au)
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
 ** Comments:
 **
 ** $Id: IterativeWeights.h 374 2006-10-12 08:59:41Z SteveQuenette $
 **
 **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __PICellerator_Weights_IterativeWeightsClass_h__
#define __PICellerator_Weights_IterativeWeightsClass_h__

	/* Textual name of this class */
	extern const Type IterativeWeights_Type;

	/* IterativeWeights information */
	#define __IterativeWeights \
	/* General info */ \
		__ConstantWeights \
		\
		/* Virtual Info */ \
		WeightsCalculator*	initialWeights; \
		Iteration_Index		maxIterations; \
		double					tolerance; \
		double					alpha; \
		Bool						freeInitialWeights;
	
	struct IterativeWeights { __IterativeWeights };

	/*---------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/

	#ifndef ZERO
	#define ZERO 0
	#endif

	#define ITERATIVEWEIGHTS_DEFARGS \
		CONSTANTWEIGHTS_DEFARGS

	#define ITERATIVEWEIGHTS_PASSARGS \
		CONSTANTWEIGHTS_PASSARGS

	IterativeWeights* _IterativeWeights_New(  ITERATIVEWEIGHTS_DEFARGS  );

	/* Stg_Class_Delete IterativeWeights implementation */
	void _IterativeWeights_Delete( void* iterativeWeights );

	void _IterativeWeights_Print( void* iterativeWeights, Stream* stream );

	#define IterativeWeights_Copy( self ) \
		(IterativeWeights*) Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define IterativeWeights_DeepCopy( self ) \
		(IterativeWeights*) Stg_Class_Copy( self, NULL, True, NULL, NULL )

	void* _IterativeWeights_Copy( void* iterativeWeights, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
        
	void* _IterativeWeights_DefaultNew( Name name );

	void _IterativeWeights_AssignFromXML( void* iterativeWeights, Stg_ComponentFactory* cf, void* data );

	void _IterativeWeights_Build( void* iterativeWeights, void* data );

	void _IterativeWeights_Initialise( void* iterativeWeights, void* data );

	void _IterativeWeights_Execute( void* iterativeWeights, void* data );

	void _IterativeWeights_Destroy( void* iterativeWeights, void* data );
                
	void _IterativeWeights_Calculate( void* iterativeWeights, void* _swarm, Cell_LocalIndex lCell_I );
	/*---------------------------------------------------------------------------------------------------------------------
	** Private functions
	*/
        
	/*---------------------------------------------------------------------------------------------------------------------
	** Public functions
	*/
	void IterativeWeights_ScaleForConstantConstraint( void* iterativeWeights, void* _swarm, Cell_LocalIndex lCell_I ) ;
        
#endif 

