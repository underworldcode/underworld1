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

/* This plugin will cause call hooks on the stokesSLE_EP to be removed until a specified number of timesteps, n.
 * Once the simulation has run n timesteps this plugins puts back the EP's as specified in SystemLinearEquations.c.
 * This causes the velocity field to be 0 for the first n timesteps, thereby diffusing the experiment.
 *
 * User can specify the number of diffusion only timsteps by including
 *    --PureDiffusion_TimeSteps=n
 * 
 * This plugins assumes all the hooks needed by the Stokes_SLE are those defined below... this is dangerous and will
 * need modifications if more or less hooks are desired on the stokesSLE execute ep.
 */ 
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

const Type Underworld_PureDiffusion_Type = "Underworld_PureDiffusion";

UnderworldContext* Underworld_PureDiffusion_ctx = NULL;
static int PureDiffusionBeenHere;

void Underworld_PureDiffusion_Diffuse( Stokes_SLE* sle, UnderworldContext* context ) {
  int timeStep = Stg_ComponentFactory_GetRootDictInt( context->CF, "PureDiffusion_TimeSteps", 3 ); 

  /* awkward PureDiffusionBeenHere logic, because deleting a hook, whilst running the associated EP
   * causes a hook in the list to be skipped. 'PureDiffusionBeenHere' means the EP_Remove() is no longer needed */
	if ( PureDiffusionBeenHere == 0 && context->timeStep > timeStep ) {
    EntryPoint* stokesSLE_EP = sle->executeEP;
    Journal_RPrintf( context->info, "\nFunction %s, is adding methods back onto the stokesSLE execute EP. Advection should also occur now.\n", __func__ );
    EntryPoint_Append( stokesSLE_EP, "BC_Setup", SystemLinearEquations_BC_Setup, Underworld_PureDiffusion_Type);
    EntryPoint_Append( stokesSLE_EP, "LM_Setup", SystemLinearEquations_LM_Setup, Underworld_PureDiffusion_Type);
    EntryPoint_Append( stokesSLE_EP, "IntegrationSetup", SystemLinearEquations_IntegrationSetup, Underworld_PureDiffusion_Type );
    EntryPoint_Append( stokesSLE_EP, "ZeroAllVectors", SystemLinearEquations_ZeroAllVectors, Underworld_PureDiffusion_Type);
    EntryPoint_Append( stokesSLE_EP, "MatrixSetup", SystemLinearEquations_MatrixSetup, Underworld_PureDiffusion_Type);
    EntryPoint_Append( stokesSLE_EP, "VectorSetup", SystemLinearEquations_VectorSetup, Underworld_PureDiffusion_Type);
    EntryPoint_Append( stokesSLE_EP, "ExecuteSolver", SystemLinearEquations_ExecuteSolver, Underworld_PureDiffusion_Type);
    EntryPoint_Append( stokesSLE_EP, "UpdateSolutionOntoNodes",SystemLinearEquations_UpdateSolutionOntoNodes, Underworld_PureDiffusion_Type);
    EntryPoint_PrintConcise( stokesSLE_EP, context->info );
    PureDiffusionBeenHere = 1;
  }
}

void Underworld_PureDiffusion_Setup( UnderworldContext* context ) {
  PureDiffusionBeenHere = 0;
	FeVariable* velocityField = (FeVariable*)LiveComponentRegister_Get( context->CF->LCRegister, (Name)"VelocityField" );
   Stokes_SLE* stokesSLE = (Stokes_SLE* )LiveComponentRegister_Get( context->CF->LCRegister, (Name)"stokesEqn" );
	DofLayout_SetAllToZero( velocityField->dofLayout );

  if ( stokesSLE  ) {
    Journal_RPrintf( context->info, "\n\nFunction %s, purging all methods off the stokesSLE execute EP. Diffusion should only occur now\n", __func__ );
    EntryPoint_Purge( stokesSLE->executeEP );
    /* args: 1st EP Name, 2nd hook name, 3rd hook funcPtr, 4th AddedByName */
    EntryPoint_Prepend_AlwaysFirst( stokesSLE->executeEP, Underworld_PureDiffusion_Type, Underworld_PureDiffusion_Diffuse, "Underworld_PureDiffusion_Plugin");
  }
}

void _Underworld_PureDiffusion_Initialise(void* component, void* data) {
   Underworld_PureDiffusion_Setup( Underworld_PureDiffusion_ctx );
}


void _Underworld_PureDiffusion_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
	UnderworldContext* context;

	context = (UnderworldContext*)Stg_ComponentFactory_ConstructByName( cf, (Name)"context", UnderworldContext, True, data  ); 
	
	ContextEP_Append( context, AbstractContext_EP_AssignFromXMLExtensions, Underworld_PureDiffusion_Setup );

        Underworld_PureDiffusion_ctx = context;
}


void* _Underworld_PureDiffusion_DefaultNew( Name name ) {
	return Codelet_New(
			Underworld_PureDiffusion_Type,
			_Underworld_PureDiffusion_DefaultNew,
			_Underworld_PureDiffusion_AssignFromXML,
			_Codelet_Build,
			_Underworld_PureDiffusion_Initialise,
			_Codelet_Execute,
			_Codelet_Destroy,
			name );
}


Index Underworld_PureDiffusion_Register( PluginsManager* pluginsManager ) {
	return PluginsManager_Submit( pluginsManager, Underworld_PureDiffusion_Type, (Name)"0", _Underworld_PureDiffusion_DefaultNew  );
}


