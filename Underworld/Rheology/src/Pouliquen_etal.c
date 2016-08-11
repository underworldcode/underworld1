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
#include "RheologyClass.h"
#include "StrainWeakening.h"
#include "YieldRheology.h"
#include "VonMises.h"
#include "Pouliquen_etal.h"
#include "ConstitutiveMatrix.h"

#include <assert.h>

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type Pouliquen_etal_Type = "Pouliquen_etal";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Pouliquen_etal* _Pouliquen_etal_New(  POULIQUEN_ETAL_DEFARGS  ) 
{
	Pouliquen_etal*					self;

	/* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
	assert( _sizeOfSelf >= sizeof(Pouliquen_etal) );
	self = (Pouliquen_etal*) _VonMises_New(  VONMISES_PASSARGS  );
	
	/* Function pointers for this class that are not on the parent class should be set here */
	return self;
}

void _Pouliquen_etal_Init(
		Pouliquen_etal*                                     self,
		FeVariable*                                        pressureField,
		FeVariable*                                        strainRateInvField,
		MaterialPointsSwarm*                               materialPointsSwarm,
		double                                             minimumYieldStress,
		double                                             frictionCoefficient,
		double                                             frictionCoefficientAfterSoftening,
		double                                             grainDiameter,
		double                                             Io,
		double                                             rho_s,
		double                                             mu_2,
		double                                             mu_s,
		double                                             mu_2_afterSoftening,
		double                                             mu_s_afterSoftening,
		double                                             maxViscosity,
		double                                             minViscosity )
{
	Pouliquen_etal_Particle*   particleExt;
	StandardParticle          materialPoint;
	
	self->particleExtHandle = ExtensionManager_Add( materialPointsSwarm->particleExtensionMgr, (Name)Pouliquen_etal_Type, sizeof(Pouliquen_etal_Particle) );
		
	/* Assign Pointers */
	self->pressureField       = pressureField;
	self->strainRateInvField    = strainRateInvField;

	self->frictionCoefficient = frictionCoefficient;
	self->minimumYieldStress  = minimumYieldStress;
	
	/* Strain softening of Friction - (linear weakening is assumed ) */
	/* needs a softening factor between +0 and 1 and a reference strain > 0 */
	self->frictionCoefficientAfterSoftening = frictionCoefficientAfterSoftening;

	self->grainDiameter = grainDiameter;
	self->Io = Io;
	self->rho_s = rho_s;
	self->mu_2 = mu_2;
	self->mu_s = mu_s;
	self->mu_2_afterSoftening = mu_2_afterSoftening;
	self->mu_s_afterSoftening = mu_s_afterSoftening;
	self->maxViscosity = maxViscosity;
	self->minViscosity = minViscosity;

	/* Update Drawing Parameters */
	EP_PrependClassHook( Context_GetEntryPoint( self->context, AbstractContext_EP_DumpClass ),
								_Pouliquen_etal_UpdateDrawParameters, self );
	
	particleExt = (Pouliquen_etal_Particle*)ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, &materialPoint, self->particleExtHandle );
	
	/* Setup Variables for Visualisation */
	self->brightness = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"Pouliquen_etalBrightness", (ArithPointer) &particleExt->brightness - (ArithPointer) &materialPoint, Variable_DataType_Float  );
	
	self->opacity = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"Pouliquen_etalOpacity", (ArithPointer) &particleExt->opacity - (ArithPointer) &materialPoint, Variable_DataType_Float  );
	
	self->diameter = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"Pouliquen_etalDiameter", (ArithPointer) &particleExt->diameter - (ArithPointer) &materialPoint, Variable_DataType_Float  );

	/* The tensileFailure variable allows to check whether a materialPoint has failed in tensile mode or not */
	self->tensileFailure = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"Pouliquen_etalTensileFailure", (ArithPointer) &particleExt->tensileFailure - (ArithPointer) &materialPoint, Variable_DataType_Char );

}

void* _Pouliquen_etal_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                     _sizeOfSelf = sizeof(Pouliquen_etal);
	Type                                                             type = Pouliquen_etal_Type;
	Stg_Class_DeleteFunction*                                     _delete = _YieldRheology_Delete;
	Stg_Class_PrintFunction*                                       _print = _YieldRheology_Print;
	Stg_Class_CopyFunction*                                         _copy = _YieldRheology_Copy;
	Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _Pouliquen_etal_DefaultNew;
	Stg_Component_ConstructFunction*                           _construct = _Pouliquen_etal_AssignFromXML;
	Stg_Component_BuildFunction*                                   _build = _Pouliquen_etal_Build;
	Stg_Component_InitialiseFunction*                         _initialise = _Pouliquen_etal_Initialise;
	Stg_Component_ExecuteFunction*                               _execute = _YieldRheology_Execute;
	Stg_Component_DestroyFunction*                               _destroy = _Pouliquen_etal_Destroy;
	Rheology_ModifyConstitutiveMatrixFunction*  _modifyConstitutiveMatrix = _YieldRheology_ModifyConstitutiveMatrix;
	YieldRheology_GetYieldCriterionFunction*           _getYieldCriterion = _Pouliquen_etal_GetYieldCriterion;
	YieldRheology_GetYieldIndicatorFunction*           _getYieldIndicator = _VonMises_GetYieldIndicator;
	YieldRheology_HasYieldedFunction*                         _hasYielded = _Pouliquen_etal_HasYielded;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*) _Pouliquen_etal_New(  POULIQUEN_ETAL_PASSARGS   );
}

void _Pouliquen_etal_AssignFromXML( void* pouliquen_etal, Stg_ComponentFactory* cf, void* data ){
	Pouliquen_etal*          self           = (Pouliquen_etal*)pouliquen_etal;
	FeVariable*             pressureField;
	FeVariable*             strainRateInvField;
	MaterialPointsSwarm*    materialPointsSwarm;

	/* Construct Parent */
	_VonMises_AssignFromXML( self, cf, data );
	
	pressureField      = (FeVariable *) 
			Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"PressureField", FeVariable, True, data );
			
	strainRateInvField      = (FeVariable * ) 
			Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"StrainRateInvariantField", FeVariable, True, data );

	materialPointsSwarm     = (MaterialPointsSwarm* )
			Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"MaterialPointsSwarm", MaterialPointsSwarm, True, data  );
		
	_Pouliquen_etal_Init( self, 
			pressureField,
			strainRateInvField,
			materialPointsSwarm, 
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"minimumYieldStress", 0.0  ),
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"frictionCoefficient", 0.0  ),
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"frictionCoefficientAfterSoftening", 0.0  ),
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"grainDiameter", 0.0  ),
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"Io", 0.0  ),
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"rho_s", 0.0  ),
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"mu_2", 0.0  ),
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"mu_s", 0.0  ),
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"mu_2_afterSoftening", 0.0  ),
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"mu_s_afterSoftening", 0.0  ),
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"maxViscosity", 0.0  ),
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"minViscosity", 0.0 )   );
}

void _Pouliquen_etal_Build( void* rheology, void* data ) {
	Pouliquen_etal*          self               = (Pouliquen_etal*) rheology;

	/* Build parent */
	_YieldRheology_Build( self, data );

	Stg_Component_Build( self->pressureField, data, False );
	Stg_Component_Build( self->strainRateInvField, data, False );
	
	Stg_Component_Build( self->brightness, data, False );
	Stg_Component_Build( self->opacity, data, False );
	Stg_Component_Build( self->diameter, data, False );
	Stg_Component_Build( self->tensileFailure, data, False );

}

void _Pouliquen_etal_Initialise( void* rheology, void* data ) {
	Pouliquen_etal*	self = (Pouliquen_etal*) rheology;
	Particle_Index		lParticle_I;
	Particle_Index		particleLocalCount;

	_YieldRheology_Initialise( self, data );

   Stg_Component_Initialise( self->pressureField, data, False );
	Stg_Component_Initialise( self->strainRateInvField, data, False );

	/* We should only set initial conditions if in regular non-restart mode. If in restart mode, then
	the particle-based variables will be set correcty when we re-load the Swarm. */
	if ( self->context->loadSwarmsFromCheckpoint == False ) {
      /* Initialise variables that I've created - (mainly just SwarmVariables)
       * This will run a Variable_Update for us */
      Stg_Component_Initialise( self->brightness, data, False );
      Stg_Component_Initialise( self->opacity, data, False );
      Stg_Component_Initialise( self->diameter, data, False );
      Stg_Component_Initialise( self->tensileFailure, data, False );

		/* We don't need to Initialise hasYieldedVariable because it's a parent variable and _YieldRheology_Initialise
		 * has already been called */
		particleLocalCount = self->hasYieldedVariable->variable->arraySize;

		for ( lParticle_I = 0 ; lParticle_I < particleLocalCount ; lParticle_I++ ) { 
		
			Variable_SetValueFloat( self->brightness->variable, lParticle_I, 0.0 );
			Variable_SetValueFloat( self->opacity->variable,    lParticle_I, 0.0 );
			Variable_SetValueFloat( self->diameter->variable,   lParticle_I, 0.0 );
		
			Variable_SetValueChar( self->hasYieldedVariable->variable, lParticle_I, False );
			Variable_SetValueChar( self->tensileFailure->variable,lParticle_I, False );
		}
	}	
}

void _Pouliquen_etal_Destroy( void* rheology, void* data ) {
	Pouliquen_etal*          self               = (Pouliquen_etal*) rheology;

	Stg_Component_Destroy( self->pressureField, data, False );
	Stg_Component_Destroy( self->strainRateInvField, data, False );
	
	Stg_Component_Destroy( self->brightness, data, False );
	Stg_Component_Destroy( self->opacity, data, False );
	Stg_Component_Destroy( self->diameter, data, False );
	Stg_Component_Destroy( self->tensileFailure, data, False );

	/* Destroy parent */
	_YieldRheology_Destroy( self, data );

}


double _Pouliquen_etal_GetYieldCriterion( 
			void*                            pouliquen_etal,
			ConstitutiveMatrix*              constitutiveMatrix,
			MaterialPointsSwarm*             materialPointsSwarm,
			Element_LocalIndex               lElement_I,
			MaterialPoint*                   materialPoint,
			Coord                            xi )
{
	Pouliquen_etal*                    self             = (Pouliquen_etal*) pouliquen_etal;

	double                            frictionalStrength;
	double                            pressure;
	Pouliquen_etal_Particle*          particleExt;
	double                            strainRateInv;
	
	particleExt = (Pouliquen_etal_Particle*)ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, materialPoint, self->particleExtHandle );

	FeVariable_InterpolateWithinElement( self->pressureField, lElement_I, xi, &pressure );
	FeVariable_InterpolateWithinElement( self->strainRateInvField, lElement_I, xi, &strainRateInv );

	particleExt->pressure      = pressure;
	particleExt->strainRateInv = strainRateInv;

	frictionalStrength = self->mu_s * pressure;
	//the following is to ensure that every particle is yielding
	//frictionalStrength = -1.0;	

	return frictionalStrength;
}

void _Pouliquen_etal_UpdateDrawParameters( void* rheology ) {
	Pouliquen_etal*                   self               = (Pouliquen_etal*) rheology;
	Particle_Index                   lParticle_I;
	Particle_Index                   particleLocalCount;
	StrainWeakening*                 strainWeakening    = self->strainWeakening;
	MaterialPoint*                   materialPoint;
	
	double                           length;
	double                           brightness;
	double                           opacity;
	double                           strainWeakeningRatio;
	double                           localMaxStrainIncrement;
	double                           localMeanStrainIncrement;
	Particle_Index                   localFailed;
	
	double                           globalMaxStrainIncrement;
	double                           globalMeanStrainIncrement;
	Particle_Index                   globalFailed;
	
	double                           averagedGlobalMaxStrainIncrement = 0.0;

	double                           oneOverGlobalMaxStrainIncrement;
	double                           postFailureWeakeningIncrement;

	/* Note : this function defines some drawing parameters (brightness, opacity, diameter) as
	 * functions of the strain weakening - this needs to be improved since most of the parameters
	 * that define this dependency are hard coded here. We need to have a more flexible way
	 * to construct the viz parameters as functions of material parameters */

	/* We should only update the drawing parameters if the strain weakening is defined */ 
	if (strainWeakening==NULL)
		return;
	
	localMaxStrainIncrement = 0.0;
	localMeanStrainIncrement = 0.0;
	localFailed = 0;

	/* Update all variables */
	Variable_Update( self->hasYieldedVariable->variable );
	Variable_Update( self->brightness->variable );
	Variable_Update( self->opacity->variable );
	Variable_Update( self->diameter->variable );
	Variable_Update( strainWeakening->postFailureWeakeningIncrement->variable );

	particleLocalCount = self->hasYieldedVariable->variable->arraySize;
	
	for ( lParticle_I = 0 ; lParticle_I < particleLocalCount ; lParticle_I++ ) { 
		if ( Variable_GetValueChar( self->hasYieldedVariable->variable, lParticle_I )) {
			localFailed++;

			postFailureWeakeningIncrement = 
					Variable_GetValueDouble( strainWeakening->postFailureWeakeningIncrement->variable, lParticle_I );
			
			localMeanStrainIncrement += postFailureWeakeningIncrement;
		
			if(localMaxStrainIncrement < postFailureWeakeningIncrement)
				localMaxStrainIncrement = postFailureWeakeningIncrement;
		}
	}
	
	(void)MPI_Allreduce( &localMaxStrainIncrement,  &globalMaxStrainIncrement,  1, MPI_DOUBLE, MPI_MAX, MPI_COMM_WORLD );
	(void)MPI_Allreduce( &localMeanStrainIncrement, &globalMeanStrainIncrement, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
	(void)MPI_Allreduce( &localFailed,              &globalFailed,              1, MPI_INT,    MPI_SUM, MPI_COMM_WORLD );
	
	if(globalFailed == 0) 
		return;
				
	globalMeanStrainIncrement /= (double) globalFailed;
	
	averagedGlobalMaxStrainIncrement = 
			0.5 * averagedGlobalMaxStrainIncrement + 
			0.25 * globalMeanStrainIncrement +
			0.25 * globalMaxStrainIncrement;
	
	/* Let's simply assume that twice the mean is a good place to truncate these values */
	oneOverGlobalMaxStrainIncrement = 1.0 / averagedGlobalMaxStrainIncrement;
	
	for ( lParticle_I = 0 ; lParticle_I < particleLocalCount ; lParticle_I++ ) { 
		materialPoint = (MaterialPoint*)Swarm_ParticleAt( strainWeakening->swarm, lParticle_I );

		if ( Variable_GetValueChar( self->hasYieldedVariable->variable, lParticle_I ) == False ||
					StrainWeakening_GetPostFailureWeakening( strainWeakening, materialPoint ) < 0.0 ) 
		{
			Variable_SetValueFloat( self->brightness->variable, lParticle_I, 0.0 );
			Variable_SetValueFloat( self->opacity->variable, lParticle_I, 0.0 );
			Variable_SetValueFloat( self->diameter->variable, lParticle_I, 0.0 );
			continue;
		}  

		postFailureWeakeningIncrement = 
				Variable_GetValueDouble( strainWeakening->postFailureWeakeningIncrement->variable, lParticle_I );
			
		strainWeakeningRatio = StrainWeakening_CalcRatio( strainWeakening, materialPoint );
		
		length     = 0.001 + 0.003 * strainWeakeningRatio;
		brightness = strainWeakeningRatio * postFailureWeakeningIncrement * oneOverGlobalMaxStrainIncrement;
		
		opacity = 0.5 * brightness; 
		
		if( brightness > 1.0 )
			brightness = 1.0;
		
		if( opacity > 0.5 )
			opacity = 0.5;
		
		if( opacity < 0.1 )
			opacity = 0.0;
		
		Variable_SetValueFloat( self->brightness->variable, lParticle_I, brightness );
		Variable_SetValueFloat( self->opacity->variable,    lParticle_I, opacity );
		Variable_SetValueFloat( self->diameter->variable,   lParticle_I, (float) length );
	}
	
}

void _Pouliquen_etal_HasYielded( 
		void*                            rheology,
		ConstitutiveMatrix*              constitutiveMatrix,
		MaterialPointsSwarm*             materialPointsSwarm,
		Element_LocalIndex               lElement_I,
		MaterialPoint*                   materialPoint,
		double                           yieldCriterion,
		double                           yieldIndicator )
{
	Pouliquen_etal*                  self             = (Pouliquen_etal*) rheology;
	double                    viscosity;
	double                    oneOnI;
	Pouliquen_etal_Particle*         particleExt;
	double                    mu;
	double                    strainWeakeningRatio;
	/*double                    mu_2_afterSoftening =	self->mu_2_afterSoftening;
	double                    mu_s_afterSoftening =	self->mu_s_afterSoftening;*/
	double                    effective_mu_s;
	double                    effective_mu_2;
	double                    pressure;

	strainWeakeningRatio = StrainWeakening_CalcRatio( self->strainWeakening, materialPoint );

	particleExt = (Pouliquen_etal_Particle*)ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, materialPoint, self->particleExtHandle );

	if (fabs(particleExt->strainRateInv) <1.0e-10)
		particleExt->strainRateInv = 1.0e-10;

	/* The initial rheology is only defined for positive pressures.
	   As the pressure gets closer to a null value, the effective viscosity tends to zero,
	   which is equivalent to saying that the pressure only acts against the motion when its
	   compressive. Therefore negative pressures (tensile environment) are equivalent to a null 
	   pressure in this sense. */
	if (particleExt->pressure<0) {
		pressure = 0.0;	
	}
	else {
		pressure = particleExt->pressure;
	}

	oneOnI = sqrt(pressure/self->rho_s)/(particleExt->strainRateInv*self->grainDiameter);

	/*
	effective_mu_s =  self->mu_s * (1.0 - strainWeakeningRatio) + 
			mu_s_afterSoftening * strainWeakeningRatio;	
	effective_mu_2 =  self->mu_2 * (1.0 - strainWeakeningRatio) + 
			mu_2_afterSoftening * strainWeakeningRatio;
	*/
	effective_mu_s =  self->mu_s;
	effective_mu_2 =  self->mu_2;

	mu = effective_mu_s + (effective_mu_2-effective_mu_s)/(self->Io*oneOnI+1.);

	viscosity = mu * pressure/particleExt->strainRateInv;

	/*
	printf("oneOnI=%f self->rho_s=%f strainWeakeningRatio=%f effective_mu_s =%f\n",oneOnI,self->rho_s,strainWeakeningRatio,effective_mu_s );
	printf("particleExt->pressure=%f particleExt->strainRateInv=%f self->grainDiameter=%f\n",particleExt->pressure,particleExt->strainRateInv,self->grainDiameter);
	printf("mu=%f viscosity=%f\n",mu,viscosity);
	*/	

	if (viscosity > self->maxViscosity)
		viscosity = self->maxViscosity;
	if (viscosity < self->minViscosity)
		viscosity = self->minViscosity;

	ConstitutiveMatrix_SetIsotropicViscosity( constitutiveMatrix, viscosity );
}



