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
** 
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include "types.h"
#include "RheologyClass.h"
#include "ConstitutiveMatrix.h"
#include "OrthotropicAligned.h"

#include <assert.h>

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type OrthotropicAligned_Type = "OrthotropicAligned";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
OrthotropicAligned* _OrthotropicAligned_New(  ORTHOTROPICALIGNED_DEFARGS  ) 
{
	OrthotropicAligned*					self;

	/* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
	assert( _sizeOfSelf >= sizeof(OrthotropicAligned) );
	self = (OrthotropicAligned*) _Rheology_New(  RHEOLOGY_PASSARGS  );
	
	return self;
}

void _OrthotropicAligned_Init( OrthotropicAligned* self,
			       double C11, double C22, double C33, 
			       double C44, double C55, double C66,
			       double C12, double C13, double C23 ) {
      self->C11  = C11;
      self->C22  = C22;
      self->C33  = C33;
      self->C44  = C44;
      self->C55  = C55;  
      self->C66  = C66;
      self->C12  = C12;
      self->C13  = C13;
      self->C23  = C23;
}

void* _OrthotropicAligned_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                     _sizeOfSelf = sizeof(OrthotropicAligned);
	Type                                                             type = OrthotropicAligned_Type;
	Stg_Class_DeleteFunction*                                     _delete = _Rheology_Delete;
	Stg_Class_PrintFunction*                                       _print = _Rheology_Print;
	Stg_Class_CopyFunction*                                         _copy = _Rheology_Copy;
	Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _OrthotropicAligned_DefaultNew;
	Stg_Component_ConstructFunction*                           _construct = _OrthotropicAligned_AssignFromXML;
	Stg_Component_BuildFunction*                                   _build = _Rheology_Build;
	Stg_Component_InitialiseFunction*                         _initialise = _Rheology_Initialise;
	Stg_Component_ExecuteFunction*                               _execute = _Rheology_Execute;
	Stg_Component_DestroyFunction*                               _destroy = _Rheology_Destroy;
	Rheology_ModifyConstitutiveMatrixFunction*  _modifyConstitutiveMatrix = _OrthotropicAligned_ModifyConstitutiveMatrix;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*) _OrthotropicAligned_New(  ORTHOTROPICALIGNED_PASSARGS   );
}

void _OrthotropicAligned_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data ){
	OrthotropicAligned*     self = (OrthotropicAligned*)rheology;
        /*	Director*        director; */
	double C11;
	double C22;
	double C33;
	double C44;
	double C55;
	double C66;
	double C12;
	double C13;
	double C23;

	/* Construct Parent */
	_Rheology_AssignFromXML( self, cf, data );
	
        /*	director =  Stg_ComponentFactory_ConstructByKey(  cf,  self->name,  "Director", Director,  True  ) ; */
	C11 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"C11", 2  );
	C22 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"C22", 2  );
	C33 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"C33", 2  );
	C44 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"C44", 1  );
	C55 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"C55", 1  );
	C66 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"C66", 1  );
	C12 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"C12", 0  );
	C13 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"C13", 0  );
	C23 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"C23", 0  );

	_OrthotropicAligned_Init( 
			self,
			C11, C22, C33, C44, C55, C66,
			C12, C13, C23 );
}

void _OrthotropicAligned_ModifyConstitutiveMatrix( 
		void*                                              rheology, 
		ConstitutiveMatrix*                                constitutiveMatrix, /* constitutive matrix */
		MaterialPointsSwarm*                               swarm,
		Element_LocalIndex                                 lElement_I,
		MaterialPoint*                                     materialPoint,
		Coord                                              xi )
{
	OrthotropicAligned*   self = (OrthotropicAligned*) rheology;
	Dimension_Index       dim  = swarm->dim;
	int i,j;
	double**   D  = constitutiveMatrix->matrixData;

	for(i=0;i<dim*(dim+1)/2;i++){
	      for(j=0;j<dim*(dim+1)/2;j++){
		    D[i][j] = 0.0;
	      }
	}
	/* This model takes a specific form when the fluid is incompressible which is not enforced right now */
	/* I think we will need a flag to say whether its incompressible or not and then set it up as needed */
	/* Anything other than the specific form will result in a compressible model */
	if(dim == 2){
	      D[0][0] = self->C11;
	      D[1][1] = self->C22;
	      D[0][1] = D[1][0] = self->C12;
	      D[2][2] = self->C44;
	}
	if(dim == 3){
	      D[0][0] = self->C11;
	      D[1][1] = self->C22;
	      D[2][2] = self->C33;
	      D[0][1] = D[1][0] = self->C12;
	      D[0][2] = D[2][0] = self->C13;
	      D[1][2] = D[2][1] = self->C23;
	      D[3][3] = self->C44;
	      D[4][4] = self->C55;
	      D[5][5] = self->C66;
	}
	constitutiveMatrix->isDiagonal = False;	   
}

#if 0
void _OrthotropicAligned_UpdateDrawParameters( void* rheology ) 
{
	OrthotropicAligned*                   self               = (OrthotropicAligned*) rheology;
	Particle_Index                   lParticle_I;
	Particle_Index                   particleLocalCount;
	StandardParticle*                	materialPoint;
	Dimension_Index                 dim                   = self->materialPointsSwarm->dim;
	OrthotropicAligned_ParticleExt*            particleExt;   /* this new type is defined in OrthotropicAligned.h */

	/* do stuff */
}
#endif


