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


#ifndef __Underworld_Rheology_RheologyClass_h__
#define __Underworld_Rheology_RheologyClass_h__

	/** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
	extern const Type Rheology_Type;
	
	/* virtual function interface */
	typedef void (Rheology_ModifyConstitutiveMatrixFunction) ( 		
		void*                                              rheology, 
		ConstitutiveMatrix*                                constitutiveMatrix,
		MaterialPointsSwarm*                               swarm,
		Element_LocalIndex                                 lElement_I,
		MaterialPoint*                                     materialPoint,
		Coord                                              xi );

	/** Rheology class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
	#define __Rheology \
		/* Macro defining parent goes here - This means you can cast this class as its parent */ \
		__Stg_Component \
		PICelleratorContext*				    context;				    \
      	PpcManager*                       mgr; \
		/* Virtual functions go here */ \
		Rheology_ModifyConstitutiveMatrixFunction*          _modifyConstitutiveMatrix;              \
		/* Other info */ \
		ConstitutiveMatrix*                                 constitutiveMatrix;                     \
		Materials_Register*                                 materialsRegister;                      \
		ExtensionInfo_Index                                 materialExtHandle;                      \
		Bool                                                nonLinear;                              \
		Stream*                                             debug;                                  \

	struct Rheology { __Rheology };

	/** Private Constructor: This will accept all the virtual functions for this class as arguments. */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define RHEOLOGY_DEFARGS \
                STG_COMPONENT_DEFARGS, \
                Rheology_ModifyConstitutiveMatrixFunction*  _modifyConstitutiveMatrix

	#define RHEOLOGY_PASSARGS \
                STG_COMPONENT_PASSARGS, \
	        	_modifyConstitutiveMatrix

	Rheology* _Rheology_New(  RHEOLOGY_DEFARGS  );

	void _Rheology_Init(
		void*                                              rheology,
		PICelleratorContext*                               context );

	/* 'Stg_Class' implementations */
	void _Rheology_Delete( void* rheology );
	void _Rheology_Print( void* rheology, Stream* stream );
	#define Rheology_Copy( self ) \
		(Rheology*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define Rheology_DeepCopy( self ) \
		(Rheology*)Stg_Class_Copy( self, NULL, True, NULL, NULL )
	void* _Rheology_Copy( void* rheology, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	
	/* 'Stg_Component' implementations */
	void _Rheology_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data );
	void _Rheology_Build( void* rheology, void* data );
	void _Rheology_Initialise( void* rheology, void* data );
	void _Rheology_Execute( void* rheology, void* data );
	void _Rheology_Destroy( void* rheology, void* data );
	
	/* Defining this function as a macro for speed */
	#define Rheology_ModifyConstitutiveMatrix( rheology, constitutiveMatrix, swarm, lElement_I, materialPoint, xi  ) \
		( ((Rheology*)(rheology))->_modifyConstitutiveMatrix( rheology, constitutiveMatrix, swarm, lElement_I, materialPoint, xi  ) )

	#define Rheology_SetToNonLinear( rheology ) \
		( ((Rheology*)(rheology))->nonLinear = True )

	#define Rheology_SetToLinear( rheology ) \
		( ((Rheology*)(rheology))->nonLinear = False )

#endif

