/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University 
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
**       * Redistributions of source code must retain the above copyright notice, 
**          this list of conditions and the following disclaimer.
**       * Redistributions in binary form must reproduce the above copyright 
**         notice, this list of conditions and the following disclaimer in the 
**         documentation and/or other materials provided with the distribution.
**       * Neither the name of the Monash University nor the names of its contributors 
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
** Contact:
*%  Louis.Moresi - Louis.Moresi@monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "Underworld_Velic_solI.h"

const Type Underworld_Velic_solI_Type = "Underworld_Velic_solI";

void Underworld_Velic_solI_VelocityFunction( void* _context, double* coord, double* velocity ) {
   UnderworldContext*     context = (UnderworldContext*)_context;
   Underworld_Velic_solI* self = (Underworld_Velic_solI*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solI_Type );

   _Velic_solI( coord, self->sigma, self->B, self->xc, velocity, NULL, NULL, NULL );
}

void Underworld_Velic_solI_PressureFunction( void* _context, double* coord, double* pressure ) {
   UnderworldContext*     context = (UnderworldContext*)_context;
   Underworld_Velic_solI* self = (Underworld_Velic_solI*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solI_Type );
   
   _Velic_solI( coord, self->sigma, self->B, self->xc, NULL, pressure, NULL, NULL );
}

void Underworld_Velic_solI_StressFunction( void* _context, double* coord, double* stress ) {
   UnderworldContext*     context = (UnderworldContext*)_context;
   Underworld_Velic_solI* self = (Underworld_Velic_solI*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solI_Type );
   
   _Velic_solI( coord, self->sigma, self->B, self->xc, NULL, NULL, stress, NULL );
}

void Underworld_Velic_solI_StrainRateFunction( void* _context, double* coord, double* strainRate ) {
   UnderworldContext*     context = (UnderworldContext*)_context;
   Underworld_Velic_solI* self = (Underworld_Velic_solI*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solI_Type );
   
   _Velic_solI( coord, self->sigma, self->B, self->xc, NULL, NULL, NULL, strainRate );
}

void _Underworld_Velic_solI_Init( Underworld_Velic_solI* self, double sigma, double B, double xc ) {
   self->sigma = sigma;
   self->B = B;
   self->xc = xc;
}

void _Underworld_Velic_solI_AssignFromXML( void* analyticSolution, Stg_ComponentFactory* cf, void* data ) {
   Underworld_Velic_solI* self = (Underworld_Velic_solI*)analyticSolution;
   double                 sigma, B, xc, twiceB;

   sigma = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solI_sigma", 1.0 );
   twiceB = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solI_twiceB", 2.0 );
   B = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solI_B", 0.5 * twiceB );
   xc = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solI_xc", 0.3 );

   _Underworld_Velic_solI_Init( self, sigma, B, xc );
}

void _Underworld_Velic_solI_Build( void* analyticSolution, void* data ) {
   Underworld_Velic_solI* self = (Underworld_Velic_solI*)analyticSolution;

   _Codelet_Build( self, data );

   /* Add this plugin's analytic functions into the register. */
   AnalyticFunction_Add( Underworld_Velic_solI_VelocityFunction, (Name)"Underworld_Velic_solI_VelocityFunction" );
   AnalyticFunction_Add( Underworld_Velic_solI_PressureFunction, (Name)"Underworld_Velic_solI_PressureFunction" );
   AnalyticFunction_Add( Underworld_Velic_solI_StressFunction, (Name)"Underworld_Velic_solI_StressFunction" );
   AnalyticFunction_Add( Underworld_Velic_solI_StrainRateFunction, (Name)"Underworld_Velic_solI_StrainRateFunction" );
}

void* _Underworld_Velic_solI_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                             _sizeOfSelf = sizeof(Underworld_Velic_solI);
   Type                                                     type = Underworld_Velic_solI_Type;
   Stg_Class_DeleteFunction*                             _delete = _Codelet_Delete;
   Stg_Class_PrintFunction*                               _print = _Codelet_Print;
   Stg_Class_CopyFunction*                                 _copy = _Codelet_Copy;
   Stg_Component_DefaultConstructorFunction* _defaultConstructor = _Underworld_Velic_solI_DefaultNew;
   Stg_Component_ConstructFunction*                   _construct = _Underworld_Velic_solI_AssignFromXML;
   Stg_Component_BuildFunction*                           _build = _Underworld_Velic_solI_Build;
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

Index Underworld_Velic_solI_Register( PluginsManager* pluginsManager ) {
   return PluginsManager_Submit( pluginsManager, Underworld_Velic_solI_Type, (Name)"0", _Underworld_Velic_solI_DefaultNew );
}


