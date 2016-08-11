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
#include "FrankKamenetskii.h"
#include "ConstitutiveMatrix.h"

#include <assert.h>

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type FrankKamenetskii_Type = "FrankKamenetskii";

/* Public Constructor */
FrankKamenetskii* FrankKamenetskii_New(
      Name                  name,
      AbstractContext*      context,
      FeVariable*           temperatureField, 
      double                eta0,
      double                theta )
{
   FrankKamenetskii* self = (FrankKamenetskii*) _FrankKamenetskii_DefaultNew( name );

   _Rheology_Init( self, (PICelleratorContext*)context );
   _FrankKamenetskii_Init( self, temperatureField, eta0, theta );
   self->isConstructed = True;
   return self;
}

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
FrankKamenetskii* _FrankKamenetskii_New(  FRANKKAMENETSKII_DEFARGS  ) 
{
	FrankKamenetskii*					self;

	/* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
	assert( _sizeOfSelf >= sizeof(FrankKamenetskii) );
	self = (FrankKamenetskii*) _Rheology_New(  RHEOLOGY_PASSARGS  );
	
	return self;
}

void _FrankKamenetskii_Init( FrankKamenetskii* self, FeVariable* temperatureField, double eta0, double theta ) {
	self->temperatureField = temperatureField;
	self->eta0  = eta0;
	self->theta = theta;
}

void* _FrankKamenetskii_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                     _sizeOfSelf = sizeof(FrankKamenetskii);
	Type                                                             type = FrankKamenetskii_Type;
	Stg_Class_DeleteFunction*                                     _delete = _Rheology_Delete;
	Stg_Class_PrintFunction*                                       _print = _Rheology_Print;
	Stg_Class_CopyFunction*                                         _copy = _Rheology_Copy;
	Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _FrankKamenetskii_DefaultNew;
	Stg_Component_ConstructFunction*                           _construct = _FrankKamenetskii_AssignFromXML;
	Stg_Component_BuildFunction*                                   _build = _Rheology_Build;
	Stg_Component_InitialiseFunction*                         _initialise = _Rheology_Initialise;
	Stg_Component_ExecuteFunction*                               _execute = _Rheology_Execute;
	Stg_Component_DestroyFunction*                               _destroy = _FrankKamenetskii_Destroy;
	Rheology_ModifyConstitutiveMatrixFunction*  _modifyConstitutiveMatrix = _FrankKamenetskii_ModifyConstitutiveMatrix;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*) _FrankKamenetskii_New(  FRANKKAMENETSKII_PASSARGS  );
}

void _FrankKamenetskii_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data ){
	FrankKamenetskii*  self = (FrankKamenetskii*)rheology;
	FeVariable* temperatureField;

	/* Construct Parent */
	_Rheology_AssignFromXML( self, cf, data );
	
	/* TODO: KeyFallback soon to be deprecated/updated */
	temperatureField = Stg_ComponentFactory_ConstructByNameWithKeyFallback( cf, self->name, (Name)"TemperatureField", (Dictionary_Entry_Key)"TemperatureField", FeVariable, True, data  );
	/*temperatureField = Stg_ComponentFactory_ConstructByKey( cf, self->name, "TemperatureField", FeVariable, True ); */
	
	_FrankKamenetskii_Init( 
			self, 
			temperatureField,
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"eta0", 1.0  ),
			Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"theta", 0.0 )  );
}

void _FrankKamenetskii_Destroy( void* rheology, void* data ) {
	FrankKamenetskii*          self               = (FrankKamenetskii*) rheology;

	Stg_Component_Destroy( self->temperatureField, data, False );

	/* Destroy parent */
	_Rheology_Destroy( self, data );

}

void _FrankKamenetskii_ModifyConstitutiveMatrix( 
		void*                                              rheology, 
		ConstitutiveMatrix*                                constitutiveMatrix,
		MaterialPointsSwarm*                               swarm,
		Element_LocalIndex                                 lElement_I,
		MaterialPoint*                                     materialPoint,
		Coord                                              xi )
{
	FrankKamenetskii*                 self              = (FrankKamenetskii*) rheology;
	double                            temperature;
	double                            viscosity;

	/* Calculate Parameters */
	FeVariable_InterpolateFromMeshLocalCoord( self->temperatureField, ConstitutiveMatrix_GetMesh( constitutiveMatrix ), lElement_I, xi, &temperature );

	/* Calculate New Viscosity */
	viscosity = self->eta0 * exp( - self->theta * temperature );
	ConstitutiveMatrix_SetIsotropicViscosity( constitutiveMatrix, viscosity );
}


