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


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include "types.h"
#include "ConstitutiveMatrix.h"
#include "RheologyClass.h"
#include "StrainWeakening.h"
#include "YieldRheology.h"

#include <assert.h>

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type YieldRheology_Type = "YieldRheology";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
YieldRheology* _YieldRheology_New(  YIELDRHEOLOGY_DEFARGS  ) 
{
	YieldRheology*					self;

	/* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
	assert( _sizeOfSelf >= sizeof(YieldRheology) );
	self = (YieldRheology*) _Rheology_New(  RHEOLOGY_PASSARGS  );
	
	/* Function pointers for this class that are not on the parent class should be set here */
	self->_getYieldCriterion = _getYieldCriterion;
	self->_getYieldIndicator = _getYieldIndicator;
	self->_hasYielded        = _hasYielded;
	
	return self;
}

void _YieldRheology_Init( YieldRheology* self, StrainWeakening* strainWeakening, MaterialPointsSwarm* materialPointsSwarm, double minVisc )
{
	ExtensionInfo_Index     handle             = (ExtensionInfo_Index) -1;
	self->strainWeakening = strainWeakening;

	Rheology_SetToNonLinear( self );
	
	if ( materialPointsSwarm ) {
		ArithPointer offset; 
		
		/* See if the YieldRheology Type has already added an extension to the particle 
		 * If handle is given a value of '-1' - that means that no extension has been added
		 * with the YieldRheology_Type
		 * We should then add the extension */
		
		handle = ExtensionManager_GetHandle( materialPointsSwarm->particleExtensionMgr, (Name)YieldRheology_Type );
		
		if ( handle == (ExtensionInfo_Index) -1  ) {
			handle = ExtensionManager_Add( materialPointsSwarm->particleExtensionMgr, (Name)YieldRheology_Type, sizeof(Particle_Bool));
		
			/* Adding variable for plotting purpose */
			offset = (ArithPointer ) 
				ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, NULL, handle );

			self->hasYieldedVariable = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"HasYielded", offset, Variable_DataType_Char  );
         LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*)self->hasYieldedVariable->variable );
         LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*)self->hasYieldedVariable );
		}
		else {
			/* if the variable has already been created - then just store the pointer */
			Name hasYieldedVariableName = Stg_Object_AppendSuffix( materialPointsSwarm, (Name)"HasYielded"  );
			self->hasYieldedVariable = SwarmVariable_Register_GetByName( materialPointsSwarm->swarmVariable_Register, hasYieldedVariableName );
			Memory_Free( hasYieldedVariableName );
		}
	}
	
	/* Store value of particle extension handle
	 * if there are no material points - this will be '-1'
	 * if there are material points - all YieldRheology objects will refer to the same handle */
	self->hasYieldedParticleExtHandle = handle;

   self->yieldCriterion = 0.0;
   self->minVisc = minVisc;
}


void _YieldRheology_Delete( void* rheology ) {
	YieldRheology*					self = (YieldRheology*)rheology;
	
	_Stg_Component_Delete( self );
}

void _YieldRheology_Print( void* rheology, Stream* stream ) {}

void* _YieldRheology_Copy( void* rheology, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
	YieldRheology*	self = (YieldRheology*)rheology;

	/* TODO */ abort();
	return (void*) self;
}

void _YieldRheology_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data ){
	YieldRheology*        self                 = (YieldRheology*)rheology;
	StrainWeakening*      strainWeakening;
	MaterialPointsSwarm*  materialPoints;
	double                minVisc;
	
	_Rheology_AssignFromXML( self, cf, data );

	strainWeakening =  Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"StrainWeakening", StrainWeakening, False, data   ) ;

	materialPoints = (MaterialPointsSwarm*)Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"MaterialPointsSwarm", MaterialPointsSwarm, True, data  );


   minVisc = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"minimumViscosity", 0.0  );
   
	_YieldRheology_Init( self, strainWeakening, materialPoints, minVisc );
}

void _YieldRheology_Build( void* rheology, void* data ) {
	YieldRheology*                   self          = (YieldRheology*) rheology;

	_Rheology_Build( rheology, data );

	/* This variable only needs to be built if there are material points (hasYieldedVariable is created
	 * in _YieldRheology_Init only in that case) */
	
	if ( self->hasYieldedVariable ) Stg_Component_Build( self->hasYieldedVariable, data, False );

}

void _YieldRheology_Initialise( void* rheology, void* data ) {
	YieldRheology*                   self          = (YieldRheology*) rheology;
		
	
	_Rheology_Initialise( rheology, data );

	/* This variable only needs to be initialised if there are material points (hasYieldedVariable is created
	 * in _YieldRheology_Init only in that case) */
	
      
	if ( self->context->loadSwarmsFromCheckpoint == False ) {
		if ( self->hasYieldedVariable ){
		  int lParticle_I, particleLocalCount;
		  particleLocalCount = self->hasYieldedVariable->variable->arraySize;
			/* We don't need to Initialise hasYieldedVariable if it doesn't exist */
			Stg_Component_Initialise( self->hasYieldedVariable, data, False );
			for ( lParticle_I = 0 ; lParticle_I < particleLocalCount ; lParticle_I++ ) { 
				  Variable_SetValueChar( self->hasYieldedVariable->variable, lParticle_I, False );
			}
		}
	}	
	

}

void _YieldRheology_Execute( void* rheology, void* data ) {}

void _YieldRheology_Destroy( void* rheology, void* data ) {
   YieldRheology* self = (YieldRheology*) rheology;
   
   if( self->strainWeakening )     Stg_Component_Destroy( self->strainWeakening, data, False );
   if ( self->hasYieldedVariable ) Stg_Component_Destroy( self->hasYieldedVariable, data, False );
   
   _Rheology_Destroy( self, data ); 
}

void _YieldRheology_ModifyConstitutiveMatrix( 
		void*                                              yieldRheology, 
		ConstitutiveMatrix*                                constitutiveMatrix,
		MaterialPointsSwarm*                               materialPointsSwarm,
		Element_LocalIndex                                 lElement_I,
		MaterialPoint*                                     materialPoint,
		Coord                                              xi )
{
	YieldRheology*                   self          = (YieldRheology*) yieldRheology;
	double                           yieldCriterion;
	double                           yieldIndicator; /* A particle will yield if yieldCriterion < yieldIndicator */
	
	/* Don't want to yield on the first ever solve */
	if ( !constitutiveMatrix->previousSolutionExists )
		return;
		
	yieldIndicator = YieldRheology_GetYieldIndicator( self, constitutiveMatrix, materialPointsSwarm, lElement_I, materialPoint, xi );
	yieldCriterion = YieldRheology_GetYieldCriterion( self, constitutiveMatrix, materialPointsSwarm, lElement_I, materialPoint, xi );
	
	/* This function is called before YieldRheology_HasYielded  because :
	 * If a particle has not yielded, we still need to update the strain weakening for it (it will heal
	 * in that case rather than weaken)
	 * If a particle has yielded, the initial viscosity is needed to update the strain weakening. But since this 
	 * viscosity can be modified in the constitutive matrix by YieldRheology_HasYielded function, we need to do the update 
	 * before */
	
	if ( self->strainWeakening ) {
		StrainWeakening_AssignIncrement( self->strainWeakening, constitutiveMatrix, materialPoint, yieldCriterion, yieldIndicator );
	}

	/* Set a bool to TRUE or FLAG depending on whether a particle has failed or not */
	YieldRheology_SetParticleFlag( self, materialPointsSwarm, materialPoint, (yieldCriterion < yieldIndicator) );

	
	/* Test to see if it has yielded */
	if ( yieldCriterion < yieldIndicator ) {
		YieldRheology_HasYielded( self, constitutiveMatrix, materialPointsSwarm, lElement_I, materialPoint, yieldCriterion, yieldIndicator );
   }

   /* check if the viscosity is less than the minimum allowed, is independent 
    * of whether the particle yielded or not.... */
   if( ConstitutiveMatrix_GetIsotropicViscosity( constitutiveMatrix ) < self->minVisc )
      ConstitutiveMatrix_SetIsotropicViscosity( constitutiveMatrix, self->minVisc );

 
}



