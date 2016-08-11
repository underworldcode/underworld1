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
*%  Louis Moresi - Louis.Moresi@monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "Underworld_Velic_solC.h"

const Type Underworld_Velic_solC_Type = "Underworld_Velic_solC";

void Underworld_Velic_solC_VelocityFunction( void* _context, double* coord, double* velocity ) {
   UnderworldContext*     context = (UnderworldContext*)_context;
   Underworld_Velic_solC* self = (Underworld_Velic_solC*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solC_Type );

   _Velic_solC( coord, self->sigma, self->eta, self->x_c, velocity, NULL, NULL, NULL );
}

void Underworld_Velic_solC_PressureFunction( void* _context, double* coord, double* pressure ) {
   UnderworldContext*     context = (UnderworldContext*)_context;
   Underworld_Velic_solC* self = (Underworld_Velic_solC*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solC_Type );
   
   _Velic_solC( coord, self->sigma, self->eta, self->x_c, NULL, pressure, NULL, NULL );
}

void Underworld_Velic_solC_StressFunction( void* _context, double* coord, double* stress ) {
   UnderworldContext*     context = (UnderworldContext*)_context;
   Underworld_Velic_solC* self = (Underworld_Velic_solC*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solC_Type );
   
   _Velic_solC( coord, self->sigma, self->eta, self->x_c, NULL, NULL, stress, NULL );
}

void Underworld_Velic_solC_StrainRateFunction( void* _context, double* coord, double* strainRate ) {
   UnderworldContext*     context = (UnderworldContext*)_context;
   Underworld_Velic_solC* self = (Underworld_Velic_solC*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solC_Type );
   
   _Velic_solC( coord, self->sigma, self->eta, self->x_c, NULL, NULL, NULL, strainRate );
}

void _Underworld_Velic_solC_Init( Underworld_Velic_solC* self, double sigma, double eta, double x_c ) {
   Bool isCorrectInput = True;

   self->sigma = sigma;
   self->eta = eta;
   self->x_c = x_c;

   isCorrectInput = solC_checkInputParams( self );

   Journal_Firewall( isCorrectInput, Journal_Register( Error_Type, (Name)"solC" ),
      "Error in function %s: Bad Input parameters, solution check valid values in .tex documentation\n",
      __func__ );
}

void _Underworld_Velic_solC_AssignFromXML( void* analyticSolution, Stg_ComponentFactory* cf, void* data ) {
   Underworld_Velic_solC* self = (Underworld_Velic_solC*)analyticSolution;
   double                 sigma, eta, x_c;

   sigma = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solC_sigma", 1.0 );
   eta = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solC_eta", 1.0 );
   x_c = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solC_xc", 0.4 );

   _Underworld_Velic_solC_Init( self, sigma, eta, x_c );
}

void _Underworld_Velic_solC_Build( void* analyticSolution, void* data ) {
   Underworld_Velic_solC* self = (Underworld_Velic_solC*)analyticSolution;

   _Codelet_Build( self, data );

   /* Add this plugin's analytic functions into the register. */
   AnalyticFunction_Add( Underworld_Velic_solC_VelocityFunction, (Name)"Underworld_Velic_solC_VelocityFunction" );
   AnalyticFunction_Add( Underworld_Velic_solC_PressureFunction, (Name)"Underworld_Velic_solC_PressureFunction" );
   AnalyticFunction_Add( Underworld_Velic_solC_StressFunction, (Name)"Underworld_Velic_solC_StressFunction" );
   AnalyticFunction_Add( Underworld_Velic_solC_StrainRateFunction, (Name)"Underworld_Velic_solC_StrainRateFunction" );
}

Bool solC_checkInputParams( Underworld_Velic_solC* self ) {
   return ( 
      ( self->sigma > 0.0 ) && ( self->eta > 0.0 ) &&
      ( self->x_c >= 0.0 ) && (self->x_c <= 1.0 ) 
   );
}

void* _Underworld_Velic_solC_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof(Underworld_Velic_solC);
   Type                                                      type = Underworld_Velic_solC_Type;
   Stg_Class_DeleteFunction*                              _delete = _Codelet_Delete;
   Stg_Class_PrintFunction*                                _print = _Codelet_Print;
   Stg_Class_CopyFunction*                                  _copy = _Codelet_Copy;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Underworld_Velic_solC_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _Underworld_Velic_solC_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _Underworld_Velic_solC_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _Codelet_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _Codelet_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _Codelet_Destroy;

   /* 
    * Variables that are set to ZERO are variables that will be set either by the 
    * current _New function or another parent _New function further up the hierachy.
    */

   /* default value NON_GLOBAL */
   AllocationType nameAllocationType = NON_GLOBAL;

   return _Codelet_New( CODELET_PASSARGS );
}

Index Underworld_Velic_solC_Register( PluginsManager* pluginsManager ) {
   return PluginsManager_Submit( pluginsManager, Underworld_Velic_solC_Type, (Name)"0", _Underworld_Velic_solC_DefaultNew );
}
