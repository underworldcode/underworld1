/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University 
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 		* Redistributions of source code must retain the above copyright notice, 
** 			this list of conditions and the following disclaimer.
** 		* Redistributions in binary form must reproduce the above copyright 
**			notice, this list of conditions and the following disclaimer in the 
**			documentation and/or other materials provided with the distribution.
** 		* Neither the name of the Monash University nor the names of its contributors 
**			may be used to endorse or promote products derived from this software 
**			without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
** THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
** PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS 
** BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
** OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**
** Contact:
*%  Louis.Moresi - Louis.Moresi@monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
** 
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __Underworld_Rheology_ConstitutiveMatrixCartesian_h__
#define __Underworld_Rheology_ConstitutiveMatrixCartesian_h__

	/** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
	extern const Type ConstitutiveMatrixCartesian_Type;

	/** ConstitutiveMatrixCartesian class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
	#define __ConstitutiveMatrixCartesian \
		/* Macro defining parent goes here - This means you can cast this class as its parent */ \
		__ConstitutiveMatrix \
		\
		/* Virtual functions go here */ \
		\
		/* ConstitutiveMatrixCartesian info */ \
		double** Dtilda_B; \
		double*  Ni;       \
		int      beenHere;

	struct ConstitutiveMatrixCartesian { __ConstitutiveMatrixCartesian };

	ConstitutiveMatrixCartesian* ConstitutiveMatrixCartesian_New( 
		Name                                                name,
		StiffnessMatrix*                                    stiffnessMatrix,
		Swarm*                                              swarm,
		Dimension_Index                                     dim,
		FiniteElementContext*                               context,	
		Materials_Register*                                 materials_Register );

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define CONSTITUTIVEMATRIXCARTESIAN_DEFARGS \
                CONSTITUTIVEMATRIX_DEFARGS

	#define CONSTITUTIVEMATRIXCARTESIAN_PASSARGS \
                CONSTITUTIVEMATRIX_PASSARGS

	ConstitutiveMatrixCartesian* _ConstitutiveMatrixCartesian_New(  CONSTITUTIVEMATRIXCARTESIAN_DEFARGS  );
	
	void _ConstitutiveMatrixCartesian_Delete( void* constitutiveMatrix );
	void _ConstitutiveMatrixCartesian_Print( void* constitutiveMatrix, Stream* stream );
   void _ConstitutiveMatrixCartesian_Init( ConstitutiveMatrixCartesian* self );

	void* _ConstitutiveMatrixCartesian_DefaultNew( Name name ) ;
void _ConstitutiveMatrixCartesian_AssignFromXML( void* constitutiveMatrix, Stg_ComponentFactory* cf, void* data ) ;
	void _ConstitutiveMatrixCartesian_Build( void* constitutiveMatrix, void* data ) ;
	void _ConstitutiveMatrixCartesian_Initialise( void* constitutiveMatrix, void* data ) ;
	void _ConstitutiveMatrixCartesian_Execute( void* constitutiveMatrix, void* data ) ;
	void _ConstitutiveMatrixCartesian_Destroy( void* constitutiveMatrix, void* data ) ;

	void _ConstitutiveMatrixCartesian_AssembleElement( 
		void*                                                constitutiveMatrix,
		StiffnessMatrix*                                     stiffnessMatrix, 
		Element_LocalIndex                                   lElement_I, 
		SystemLinearEquations*                               sle,
		FiniteElementContext*                                context,
		double**                                             elStiffMat ) ;

	void _ConstitutiveMatrixCartesian2D_SetValueInAllEntries( void* constitutiveMatrix, double value ) ;
	void _ConstitutiveMatrixCartesian3D_SetValueInAllEntries( void* constitutiveMatrix, double value ) ;

	double _ConstitutiveMatrixCartesian2D_GetIsotropicViscosity( void* constitutiveMatrix ) ;
	double _ConstitutiveMatrixCartesian3D_GetIsotropicViscosity( void* constitutiveMatrix ) ;

	void _ConstitutiveMatrixCartesian2D_IsotropicCorrection( void* constitutiveMatrix, double isotropicCorrection ) ;
	void _ConstitutiveMatrixCartesian3D_IsotropicCorrection( void* constitutiveMatrix, double isotropicCorrection ) ;

	void _ConstitutiveMatrixCartesian2D_SetSecondViscosity( void* constitutiveMatrix, double deltaViscosity, XYZ director );
	void _ConstitutiveMatrixCartesian3D_SetSecondViscosity( void* constitutiveMatrix, double deltaViscosity, XYZ director );

	void _ConstitutiveMatrixCartesian2D_Assemble_D_B( void* constitutiveMatrix, double** GNx, Node_Index node_I, double** D_B );
	void _ConstitutiveMatrixCartesian3D_Assemble_D_B( void* constitutiveMatrix, double** GNx, Node_Index node_I, double** D_B );

	void _ConstitutiveMatrixCartesian2D_CalculateStress( void* constitutiveMatrix, SymmetricTensor strainRate, SymmetricTensor stress ) ;
	void _ConstitutiveMatrixCartesian3D_CalculateStress( void* constitutiveMatrix, SymmetricTensor strainRate, SymmetricTensor stress ) ;

	/* a function which defines the storage of each particle's constitutive information on the particle, 
	 * should be called before the "Build" phase */
	void ConstitutiveMatrixCartesian_SetupParticleStorage( ConstitutiveMatrixCartesian* self );

#endif

