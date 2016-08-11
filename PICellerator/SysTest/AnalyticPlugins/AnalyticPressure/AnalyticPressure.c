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
*%  Louis Moresi - Louis.Moresi@monash.edu
*%
** Contributors:
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "AnalyticPressure.h"

const Type PICellerator_AnalyticPressure_Type = "PICellerator_AnalyticPressure";

void _PICellerator_AnalyticPressure_PressureFunction( void* _context, double* coord, double* pressure ) {
   FiniteElementContext* context = (FiniteElementContext*)_context;
   AnalyticPressure*     self = (AnalyticPressure*)LiveComponentRegister_Get( context->CF->LCRegister,
      (Name)PICellerator_AnalyticPressure_Type );
   double                density = self->density;
   double                gravity = self->gravity;
   double                y;

   /* Find coordinate of node */
   y = -coord[ J_AXIS ] + (self->minY+self->maxY)/2.0;

   *pressure = y * density * gravity;
}

void _PICellerator_AnalyticPressure_Init(
   AnalyticPressure* self,
   double            density,
   double            gravity,
   double            maxY,
   double            minY )
{
   self->density = density;
   self->gravity = gravity;
   self->maxY = maxY;
   self->minY = minY;
}

void _PICellerator_AnalyticPressure_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
   AnalyticPressure* self = (AnalyticPressure*) _self;
   double            density;
   double            gravity;
   double            maxY;
   double            minY;

   /* Create Analytic Fields */
   density = Stg_ComponentFactory_GetDouble( cf, "layer", (Dictionary_Entry_Key)"density", 0.0 );
   gravity = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"gravity", 0 );
   maxY = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"maxY", 0 );
   minY = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"minY", 0 );

   _PICellerator_AnalyticPressure_Init( self, density, gravity, maxY, minY );
}

void _PICellerator_AnalyticPressure_Build( void* _self, void* data ) {
   AnalyticPressure* self = (AnalyticPressure*) _self;

   _Codelet_Build( self, data );

   /* Add this plugin's analytic functions into the register. */
   AnalyticFunction_Add( _PICellerator_AnalyticPressure_PressureFunction,
      (Name)"_PICellerator_AnalyticPressure_PressureFunction" );
}

void* _PICellerator_AnalyticPressure_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                             _sizeOfSelf = sizeof(AnalyticPressure);
	Type                                                     type = PICellerator_AnalyticPressure_Type;
	Stg_Class_DeleteFunction*                             _delete = _Codelet_Delete;
	Stg_Class_PrintFunction*                               _print = _Codelet_Print;
	Stg_Class_CopyFunction*                                 _copy = _Codelet_Copy;
	Stg_Component_DefaultConstructorFunction* _defaultConstructor = _PICellerator_AnalyticPressure_DefaultNew;
	Stg_Component_ConstructFunction*                   _construct = _PICellerator_AnalyticPressure_AssignFromXML;
	Stg_Component_BuildFunction*                           _build = _PICellerator_AnalyticPressure_Build;
	Stg_Component_InitialiseFunction*                 _initialise = _Codelet_Initialise;
	Stg_Component_ExecuteFunction*                       _execute = _Codelet_Execute;
	Stg_Component_DestroyFunction*                       _destroy = _Codelet_Destroy;

	/* 
    * Variables that are set to ZERO are variables that will be set either by the 
    * current _New function or another parent _New function further up the hierachy.
    */

   /* default value NON_GLOBAL */
	AllocationType nameAllocationType = NON_GLOBAL;

   return _Codelet_New( CODELET_PASSARGS );
}

Index PICellerator_AnalyticPressure_Register( PluginsManager* pluginsManager ) {
   return PluginsManager_Submit( pluginsManager, PICellerator_AnalyticPressure_Type, (Name)"0", _PICellerator_AnalyticPressure_DefaultNew );
}
