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

#include "Underworld_Velic_solKz.h"

const Type Underworld_Velic_solKz_Type = "Underworld_Velic_solKz";

void Underworld_Velic_solKz_PressureFunction( void* _context, double* coord, double* pressure ) {
   UnderworldContext*      context = (UnderworldContext*)_context;
   Underworld_Velic_solKz* self = (Underworld_Velic_solKz*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solKz_Type );

   _Velic_solKz( coord, self->sigma, self->km, self->n, self->B, NULL, pressure, NULL, NULL );
}

void Underworld_Velic_solKz_VelocityFunction( void* _context, double* coord, double* velocity ) {
   UnderworldContext*      context = (UnderworldContext*)_context;
   Underworld_Velic_solKz* self = (Underworld_Velic_solKz*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solKz_Type );
   
   _Velic_solKz( coord, self->sigma, self->km, self->n, self->B, velocity, NULL, NULL, NULL );
}

void Underworld_Velic_solKz_StressFunction( void* _context, double* coord, double* stress ) {
   UnderworldContext*      context = (UnderworldContext*)_context;
   Underworld_Velic_solKz* self = (Underworld_Velic_solKz*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solKz_Type );
   
   _Velic_solKz( coord, self->sigma, self->km, self->n, self->B, NULL, NULL, stress, NULL );
}

void Underworld_Velic_solKz_StrainRateFunction( void* _context, double* coord, double* strainRate ) {
   UnderworldContext*      context = (UnderworldContext*)_context;
   Underworld_Velic_solKz* self = (Underworld_Velic_solKz*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solKz_Type );
   
   _Velic_solKz( coord, self->sigma, self->km, self->n, self->B, NULL, NULL, NULL, strainRate );
}

void Underworld_Velic_solKz_ViscosityFunction( void* _context, double* coord, double* viscosity ) {
   UnderworldContext*      context = (UnderworldContext*)_context;
   Underworld_Velic_solKz* self = (Underworld_Velic_solKz*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solKz_Type );
   
   *viscosity = exp( 2.0 * self->B * coord[ J_AXIS ] );
}
   
void _Underworld_Velic_solKz_Init( Underworld_Velic_solKz* self, double sigma, double km, double B, int n ) {
   self->sigma = sigma;
   self->km = km;
   self->B = B;
   self->n = n;
}

void _Underworld_Velic_solKz_Build( void* analyticSolution, void* data ) {
   Underworld_Velic_solKz* self = (Underworld_Velic_solKz*)analyticSolution;

   _Codelet_Build( self, data );

   /* Add this plugin's analytic functions into the register. */
   AnalyticFunction_Add( Underworld_Velic_solKz_VelocityFunction, (Name)"Underworld_Velic_solKz_VelocityFunction" );   
   AnalyticFunction_Add( Underworld_Velic_solKz_PressureFunction, (Name)"Underworld_Velic_solKz_PressureFunction" );   
   AnalyticFunction_Add( Underworld_Velic_solKz_StressFunction, (Name)"Underworld_Velic_solKz_StressFunction" );   
   AnalyticFunction_Add( Underworld_Velic_solKz_StrainRateFunction, (Name)"Underworld_Velic_solKz_StrainRateFunction" );   
   AnalyticFunction_Add( Underworld_Velic_solKz_ViscosityFunction, (Name)"Underworld_Velic_solKz_ViscosityFunction" );   
}

void _Underworld_Velic_solKz_AssignFromXML( void* analyticSolution, Stg_ComponentFactory* cf, void* data ) {
   Underworld_Velic_solKz* self = (Underworld_Velic_solKz*)analyticSolution;
   double                  sigma, m, B, twiceB, km;
   int                     n;

   sigma = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solKz_sigma", 1.0  );
   twiceB = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solKz_twiceB", 2.0  );
   B = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solKz_B", 0.5 * twiceB  );
   m = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solKz_m", 1  );
   n = Stg_ComponentFactory_GetRootDictInt( cf, "solKz_n", 1 );

   km = M_PI * m;
   
  /* Note: in the _Velic_solKz function km and n must be supplied */
   _Underworld_Velic_solKz_Init( self, sigma, km, B, n );
}

void* _Underworld_Velic_solKz_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                             _sizeOfSelf = sizeof(Underworld_Velic_solKz);
   Type                                                     type = Underworld_Velic_solKz_Type;
   Stg_Class_DeleteFunction*                             _delete = _Codelet_Delete;
   Stg_Class_PrintFunction*                               _print = _Codelet_Print;
   Stg_Class_CopyFunction*                                 _copy = _Codelet_Copy;
   Stg_Component_DefaultConstructorFunction* _defaultConstructor = _Underworld_Velic_solKz_DefaultNew;
   Stg_Component_ConstructFunction*                   _construct = _Underworld_Velic_solKz_AssignFromXML;
   Stg_Component_BuildFunction*                           _build = _Underworld_Velic_solKz_Build;
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

Index Underworld_Velic_solKz_Register( PluginsManager* pluginsManager ) {
   return PluginsManager_Submit( pluginsManager, Underworld_Velic_solKz_Type, (Name)"0", _Underworld_Velic_solKz_DefaultNew  );
}


