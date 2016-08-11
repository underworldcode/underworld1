/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2014, Melbourne University 
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
**       * Redistributions of source code must retain the above copyright notice, 
**          this list of conditions and the following disclaimer.
**       * Redistributions in binary form must reproduce the above copyright 
**         notice, this list of conditions and the following disclaimer in the 
**         documentation and/or other materials provided with the distribution.
**       * Neither the name of Melbourne University nor the names of its contributors 
**         may be used to endorse or promote products derived from this software 
**         without specific prior written permission.
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
**
*%  Louis.Moresi - Louis.Moresi@unimelb.edu.au
*%  Mirko.Velic - Mirko.Velic@unimelb.edu.au
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
** 
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
   Will swap material types as particles cross Height

   Inputs:
     Two RheologyMaterials to be swapped with each other based on Height parameter
     MaterialPointsSwarm
     Height
     Context

   Example Usage:
      'background'      : is a RheologyMaterial type
      'lightLayer'          : is a RheologyMaterial type (the usual example from the RayleighTaylorBenchmark.xml)
      'materialSwarm'  : The usual name for the MaterialPointsSwarm

    Include this in the existing "plugins" list

    <list name="plugins">
      <struct>
        <param name="Type">Underworld_SwapRheologies</param>
        <param name="UpperRheologyMaterial">background</param>
        <param name="LowerRheologyMaterial">lightLayer</param>
        <param name="Height">0.343</param>
        <param name="MaterialSwarm">materialSwarm</param>
        <param name="Context">context</param>
      </struct>
     </list>
*/
#include <mpi.h>
#include <assert.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>
#include <StgFEM/FrequentOutput/FrequentOutput.h>

#include "SwapRheologies.h"

const Type Underworld_SwapRheologies_Type = "Underworld_SwapRheologies";

void _Underworld_SwapRheologies_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
   Underworld_SwapRheologies* self = (Underworld_SwapRheologies*)component;

   self->context = (AbstractContext*)Stg_ComponentFactory_PluginConstructByKey(
      cf, self, (Dictionary_Entry_Key)"Context", UnderworldContext, True, data );

   self->upperRM = Stg_ComponentFactory_PluginConstructByKey(
      cf, self, (Dictionary_Entry_Key)"UpperRheologyMaterial", RheologyMaterial, True, data );

   self->lowerRM = Stg_ComponentFactory_PluginConstructByKey(
      cf, self, (Dictionary_Entry_Key)"LowerRheologyMaterial", RheologyMaterial, True, data );

   self->swarm = Stg_ComponentFactory_PluginConstructByKey(
      cf, self, (Dictionary_Entry_Key)"MaterialSwarm", MaterialPointsSwarm, True, data );

   self->height = Stg_ComponentFactory_PluginGetDouble(
       cf, self, (Dictionary_Entry_Key)"Height", 0.5); // default = 0.5

   ContextEP_Append( self->context, AbstractContext_EP_Step, Underworld_SwapRheologies_Run );
}

void _Underworld_SwapRheologies_Build( void* component, void* data ) {
   Underworld_SwapRheologies* self = (Underworld_SwapRheologies*)component;

   assert( self );

   Stg_Component_Build( self->upperRM, data, False );
   Stg_Component_Build( self->lowerRM, data, False );
   Stg_Component_Build( self->swarm, data, False );
   
   _Codelet_Build( self, data );
}

void _Underworld_SwapRheologies_Initialise( void* component, void* data ) {
   Underworld_SwapRheologies* self = (Underworld_SwapRheologies*)component;

   assert( self );

   Stg_Component_Initialise( self->upperRM, data, False );
   Stg_Component_Initialise( self->lowerRM, data, False );
   Stg_Component_Initialise( self->swarm, data, False );
   
   _Codelet_Initialise( self, data );
}

void _Underworld_SwapRheologies_Destroy( void* component, void* data ) {
   Underworld_SwapRheologies* self = (Underworld_SwapRheologies*)component;

   assert( self );

   _Codelet_Destroy( self, data );
   
   Stg_Component_Destroy( self->upperRM, data, False );
   Stg_Component_Destroy( self->lowerRM, data, False );
   Stg_Component_Destroy( self->swarm, data, False );
}

void* _Underworld_SwapRheologies_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof(Underworld_SwapRheologies);
   Type                                                      type = Underworld_SwapRheologies_Type;
   Stg_Class_DeleteFunction*                              _delete = _Codelet_Delete;
   Stg_Class_PrintFunction*                                _print = _Codelet_Print;
   Stg_Class_CopyFunction*                                  _copy = _Codelet_Copy;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Underworld_SwapRheologies_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _Underworld_SwapRheologies_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _Underworld_SwapRheologies_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _Underworld_SwapRheologies_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _Codelet_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _Underworld_SwapRheologies_Destroy;

    /* default value NON_GLOBAL */
   AllocationType nameAllocationType = NON_GLOBAL;

   return _Codelet_New( CODELET_PASSARGS );
}

Index Underworld_SwapRheologies_Register( PluginsManager* pluginsManager ) {
   return PluginsManager_Submit( pluginsManager, Underworld_SwapRheologies_Type, (Name)"0", _Underworld_SwapRheologies_DefaultNew );
}

void Underworld_SwapRheologies_Run( void* _context ) {
   UnderworldContext*   context = (UnderworldContext* ) _context;
   RheologyMaterial*    rheologymaterialUpper;
   RheologyMaterial*    rheologymaterialLower;
   double               height;
   Material_Index       indexUpper;
   Material_Index       indexLower;
   Material_Index       indexp;
   //Dimension_Index      dim = context->dim;
   MaterialPointsSwarm* materialPointsSwarm;
   MaterialPoint*       particle;
   Particle_Index       lParticle_I;
   Underworld_SwapRheologies* self;

   self = (Underworld_SwapRheologies*)LiveComponentRegister_Get( context->CF->LCRegister, (Name)Underworld_SwapRheologies_Type );

   // A dirty way to get the swarm. 
   //materialPointsSwarm = (MaterialPointsSwarm*) LiveComponentRegister_Get( context->CF->LCRegister, (Name)"materialSwarm" );

   materialPointsSwarm = self->swarm;

   rheologymaterialUpper=self->upperRM;
   rheologymaterialLower=self->lowerRM;
   height = self->height;

   indexUpper = rheologymaterialUpper->index;
   indexLower = rheologymaterialLower->index;

   for ( lParticle_I = 0 ; lParticle_I < materialPointsSwarm->particleLocalCount ; lParticle_I++ ) {
       particle = (MaterialPoint* )Swarm_ParticleAt( materialPointsSwarm, lParticle_I );
       indexp = particle->materialIndex;

       if(particle->coord[1] > height && indexp == indexLower){
           particle->materialIndex=indexUpper;
       }
       if(particle->coord[1] < height && indexp == indexUpper){
           particle->materialIndex=indexLower;
       }
       
   }
}



