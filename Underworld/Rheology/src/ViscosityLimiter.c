/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
**       * Redistributions of source code must retain the above copyright notice,
**          this list of conditions and the following disclaimer.
**       * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**       * Neither the name of the Monash University nor the names of its contributors
**       may be used to endorse or promote products derived from this software
**       without specific prior written permission.
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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include "types.h"
#include "RheologyClass.h"
#include "ConstitutiveMatrix.h"
#include "ViscosityLimiter.h"

#include <assert.h>


/* Textual name of this class */
const Type ViscosityLimiter_Type = "ViscosityLimiter";

/* Private Constructor */
ViscosityLimiter* _ViscosityLimiter_New(  VISCOSITYLIMITER_DEFARGS  ) 
{
	ViscosityLimiter* self;

	assert( _sizeOfSelf >= sizeof(ViscosityLimiter) );
	self = (ViscosityLimiter*) _Rheology_New(  RHEOLOGY_PASSARGS  );
	
	return self;
}

void* _ViscosityLimiter_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                     _sizeOfSelf = sizeof(ViscosityLimiter);
	Type                                                             type = ViscosityLimiter_Type;
	Stg_Class_DeleteFunction*                                     _delete = _Rheology_Delete;
	Stg_Class_PrintFunction*                                       _print = _Rheology_Print;
	Stg_Class_CopyFunction*                                         _copy = _Rheology_Copy;
	Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _ViscosityLimiter_DefaultNew;
	Stg_Component_ConstructFunction*                           _construct = _ViscosityLimiter_AssignFromXML;
	Stg_Component_BuildFunction*                                   _build = _Rheology_Build;
	Stg_Component_InitialiseFunction*                         _initialise = _Rheology_Initialise;
	Stg_Component_ExecuteFunction*                               _execute = _Rheology_Execute;
	Stg_Component_DestroyFunction*                               _destroy = _Rheology_Destroy;
	Rheology_ModifyConstitutiveMatrixFunction*  _modifyConstitutiveMatrix = _ViscosityLimiter_ModifyConstitutiveMatrix;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*) _ViscosityLimiter_New(  VISCOSITYLIMITER_PASSARGS  );
}

void _ViscosityLimiter_Init( ViscosityLimiter* self, double maxViscosity, double minViscosity ) {

   Journal_Firewall( maxViscosity >= minViscosity, Journal_Register( Error_Type, (Name)self->type ),
         "Error is function '%s'. The input values for '%s' show the maxViscosity isn't >= minViscosity.\n"
         "The bad input is. maxViscosity = %g minViscosity = %g\n\n", 
         __func__, self->name, self->minViscosity, self->maxViscosity );

   self->maxViscosity = maxViscosity;
   self->minViscosity = minViscosity;
}

void _ViscosityLimiter_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ){
	ViscosityLimiter*  self = (ViscosityLimiter*)_self;

	/* Construct Parent */
	_Rheology_AssignFromXML( self, cf, data );
	
   _ViscosityLimiter_Init( self,
        self->maxViscosity = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"maxViscosity", 1E4  ),
        self->minViscosity = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"minViscosity", 1E-2  ) );
}

void _ViscosityLimiter_ModifyConstitutiveMatrix( void*                  _self, 
							     ConstitutiveMatrix*    constitutiveMatrix,
							     MaterialPointsSwarm*   swarm,
							     Element_LocalIndex     lElement_I,
							     MaterialPoint*         materialPoint,
							     Coord                  xi )
{
	ViscosityLimiter* self = (ViscosityLimiter*) _self;
	double viscosity = 1.0;

	/* Calculate New Viscosity */
	viscosity = ConstitutiveMatrix_GetIsotropicViscosity( constitutiveMatrix );
	if( viscosity > self->maxViscosity ) {
	  viscosity = self->maxViscosity;
	}
	if( viscosity < self->minViscosity ) {
	  viscosity = self->minViscosity;
	}	
	ConstitutiveMatrix_SetIsotropicViscosity( constitutiveMatrix, viscosity );

}


