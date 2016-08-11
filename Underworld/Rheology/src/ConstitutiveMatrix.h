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


#ifndef __Underworld_Rheology_ConstitutiveMatrix_h__
#define __Underworld_Rheology_ConstitutiveMatrix_h__
	
	/* typedefs for virtual functions: */
	typedef double  (ConstitutiveMatrix_GetValueFunc)            ( void* constitutiveMatrix );
	typedef void    (ConstitutiveMatrix_SetValueFunc)            ( void* constitutiveMatrix, double value );
	typedef void    (ConstitutiveMatrix_SetSecondViscosityFunc)  ( void* constitutiveMatrix, double value, XYZ vector );
	typedef void    (ConstitutiveMatrix_Assemble_D_B_Func)       ( void* constitutiveMatrix, double** GNx, Node_Index node_I, double** D_B );
	typedef void    (ConstitutiveMatrix_CalculateStressFunc)     ( void* constitutiveMatrix, SymmetricTensor strainRate, SymmetricTensor stress );
	
	extern const Type ConstitutiveMatrix_Type;
	
	/* ConstitutiveMatrix information */
	#define __ConstitutiveMatrix  \
		/* Parent info */ \
		__StiffnessMatrixTerm \
		\
		/* Virtual functions go here */                                                            \
		ConstitutiveMatrix_SetValueFunc*             _setValue;                         \
		ConstitutiveMatrix_GetValueFunc*             _getViscosity;                     \
		ConstitutiveMatrix_SetValueFunc*             _isotropicCorrection;              \
		ConstitutiveMatrix_SetSecondViscosityFunc*   _setSecondViscosity;               \
		ConstitutiveMatrix_Assemble_D_B_Func*        _assemble_D_B;                     \
		ConstitutiveMatrix_CalculateStressFunc*      _calculateStress;                  \
		\
		/* ConstitutiveMatrix info */                                                   \
		double**                                     matrixData;                        \
    double                                       derivs[9];                         \
		Dimension_Index                              dim;                               \
		Bool                                         isSwarmTypeIntegrationPointsSwarm; \
		Materials_Register*                          materials_Register;                \
		Bool                                         isDiagonal;                        \
		Index                                        columnSize;                        \
		Index                                        rowSize;                           \
		Bool                                         previousSolutionExists;            \
		Bool                                         restartUsingPreviousStokesSol;     \
		int                                          currentParticleIndex;              \
		SystemLinearEquations*                       sle;                               \
		Iteration_Index                              sleNonLinearIteration_I;           \
    /* below is needed to store the constitutiveMatrix per particle */ \
		Bool                                         storeConstitutiveMatrix;           \
		Index                                        storedConstHandle;                 \
		SwarmVariable*                               storedConstSwarmVar;               \
		
	struct ConstitutiveMatrix { __ConstitutiveMatrix };

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define CONSTITUTIVEMATRIX_DEFARGS \
                STIFFNESSMATRIXTERM_DEFARGS, \
                ConstitutiveMatrix_SetValueFunc*                       _setValue, \
                ConstitutiveMatrix_GetValueFunc*                   _getViscosity, \
                ConstitutiveMatrix_SetValueFunc*            _isotropicCorrection, \
                ConstitutiveMatrix_SetSecondViscosityFunc*   _setSecondViscosity, \
                ConstitutiveMatrix_Assemble_D_B_Func*              _assemble_D_B, \
                ConstitutiveMatrix_CalculateStressFunc*         _calculateStress

	#define CONSTITUTIVEMATRIX_PASSARGS \
                STIFFNESSMATRIXTERM_PASSARGS, \
	        _setValue,            \
	        _getViscosity,        \
	        _isotropicCorrection, \
	        _setSecondViscosity,  \
	        _assemble_D_B,        \
	        _calculateStress    

	ConstitutiveMatrix* _ConstitutiveMatrix_New(  CONSTITUTIVEMATRIX_DEFARGS  );

	/* 'Stg_Class' Virtual Functions */
	void _ConstitutiveMatrix_Delete( void* constitutiveMatrix );
	void _ConstitutiveMatrix_Print( void* constitutiveMatrix, Stream* stream );
	#define ConstitutiveMatrix_Copy( self ) \
		(ConstitutiveMatrix*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define ConstitutiveMatrix_DeepCopy( self ) \
		(ConstitutiveMatrix*)Stg_Class_Copy( self, NULL, True, NULL, NULL )
	void* _ConstitutiveMatrix_Copy( void* constitutiveMatrix, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	
	/* 'Stg_Component' Virtual Functions */
	void _ConstitutiveMatrix_AssignFromXML( void* constitutiveMatrix, Stg_ComponentFactory* cf, void* data );
	void _ConstitutiveMatrix_Build( void* constitutiveMatrix, void* data );
	void _ConstitutiveMatrix_Initialise( void* constitutiveMatrix, void* data );
	void _ConstitutiveMatrix_Execute( void* constitutiveMatrix, void* data );
	void _ConstitutiveMatrix_Destroy( void* constitutiveMatrix, void* data );
   void _ConstitutiveMatrix_Init(
         ConstitutiveMatrix*                    self,
         Dimension_Index                        dim,
         Bool                                   storeConstitutiveMatrix,
         Bool                                   restartUsingPreviousStokesSol );

	/* Wrapper macros to virtual functions - These must be macros for the sake of speed */
	#define ConstitutiveMatrix_SetValueInAllEntries( constitutiveMatrix, value ) \
		(((ConstitutiveMatrix*) constitutiveMatrix)->_setValue( constitutiveMatrix, value ))

	#define ConstitutiveMatrix_SetSecondViscosity( constitutiveMatrix, deltaViscosity, director ) \
		(((ConstitutiveMatrix*) constitutiveMatrix)->_setSecondViscosity( constitutiveMatrix, deltaViscosity, director ))

	#define ConstitutiveMatrix_GetIsotropicViscosity( constitutiveMatrix ) \
		(((ConstitutiveMatrix*) constitutiveMatrix)->_getViscosity( constitutiveMatrix ))

	#define ConstitutiveMatrix_IsotropicCorrection( constitutiveMatrix, isotropicCorrection ) \
		(((ConstitutiveMatrix*) constitutiveMatrix)->_isotropicCorrection( constitutiveMatrix, isotropicCorrection ))

	#define ConstitutiveMatrix_Assemble_D_B( constitutiveMatrix, GNx, node_I, D_B ) \
		(((ConstitutiveMatrix*) constitutiveMatrix)->_assemble_D_B( constitutiveMatrix, GNx, node_I, D_B ))

	#define ConstitutiveMatrix_CalculateStress( constitutiveMatrix, strainRate, stress ) \
		(((ConstitutiveMatrix*) constitutiveMatrix)->_calculateStress( constitutiveMatrix, strainRate, stress ) )

	/* +++ Public Functions +++ */
	void ConstitutiveMatrix_MultiplyByValue( void* constitutiveMatrix, double factor ) ;
	void ConstitutiveMatrix_PrintContents( void* constitutiveMatrix, Stream* stream ) ;
	
	void ConstitutiveMatrix_ZeroMatrix( void* constitutiveMatrix ) ;
	void ConstitutiveMatrix_SetIsotropicViscosity( void* constitutiveMatrix, double viscosity );

	void ConstitutiveMatrix_Assemble( 
		void*                                              constitutiveMatrix,
		Element_LocalIndex                                 lElement_I,
		int                                                particleIndex,
		IntegrationPoint*                                  particle );

        void ConstitutiveMatrix_AssembleMaterialPoint(void *constitutiveMatrix, int element,
						      MaterialPointsSwarm *matSwarm, int matPointInd);

	#define ConstitutiveMatrix_SetToNonLinear( constitutiveMatrix ) \
		StiffnessMatrix_SetToNonLinear( constitutiveMatrix->stiffnessMatrix )

	#define ConstitutiveMatrix_GetMesh( constitutiveMatrix ) \
		( (constitutiveMatrix)->stiffnessMatrix->rowVariable->feMesh )

  /* Returns the handle to the particle constitutiveMatrix storage extension */
  Index ConstitutiveMatrix_GetParticleConstExtHandle( void* constitutiveMatrix );

  /* Returns the stored constitutiveMatrix on a material point which maps
   * to the give integration point */
  void ConstitutiveMatrix_GetStoredMatrixOnParticle( 
    void* constitutiveMatrix,
    IntegrationPoint* particle,
    double** outputC );

#endif /* __Underworld_Rheology_ConstitutiveMatrix_h__ */

