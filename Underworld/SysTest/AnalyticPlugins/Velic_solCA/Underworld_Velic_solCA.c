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

#include "Underworld_Velic_solCA.h"

const Type Underworld_Velic_solCA_Type = "Underworld_Velic_solCA";

void Underworld_Velic_solCA_VelocityFunction( void* _context, double* coord, double* velocity ) {
   UnderworldContext*     context = (UnderworldContext*)_context;
   Underworld_Velic_solCA* self = (Underworld_Velic_solCA*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solCA_Type );

   _Velic_solCA( coord, self->sigma, self->eta, self->dx, self->x0, velocity, NULL, NULL, NULL );
}

void Underworld_Velic_solCA_PressureFunction( void* _context, double* coord, double* pressure ) {
   UnderworldContext*     context = (UnderworldContext*)_context;
   Underworld_Velic_solCA* self = (Underworld_Velic_solCA*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solCA_Type );

   _Velic_solCA( coord, self->sigma, self->eta, self->dx, self->x0, NULL, pressure, NULL, NULL );
}

void Underworld_Velic_solCA_StressFunction( void* _context, double* coord, double* stress ) {
   UnderworldContext*     context = (UnderworldContext*)_context;
   Underworld_Velic_solCA* self = (Underworld_Velic_solCA*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solCA_Type );
   
   _Velic_solCA( coord, self->sigma, self->eta, self->dx, self->x0, NULL, NULL, stress, NULL );
}

void Underworld_Velic_solCA_StrainRateFunction( void* _context, double* coord, double* strainRate ) {
   UnderworldContext*     context = (UnderworldContext*)_context;
   Underworld_Velic_solCA* self = (Underworld_Velic_solCA*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solCA_Type );
   
   _Velic_solCA( coord, self->sigma, self->eta, self->dx, self->x0, NULL, NULL, NULL, strainRate );
}

void _Underworld_Velic_solCA_Init( Underworld_Velic_solCA* self, double sigma, double eta, double dx, double x0 ) {
   Bool isCorrectInput = True;
   
   self->sigma = sigma;
   self->eta = eta;
   self->dx = dx;
   self->x0 = x0;

   isCorrectInput = _checkInputParams( self );

   Journal_Firewall( isCorrectInput, Journal_Register( Error_Type, (Name)"Velic_solCA" ),
      "Error in function %s: Bad Input parameters, solution check valid values in .tex documentation\n",
      __func__ );
}

void _Underworld_Velic_solCA_AssignFromXML( void* analyticSolution, Stg_ComponentFactory* cf, void* data ) {
   Underworld_Velic_solCA* self = (Underworld_Velic_solCA*)analyticSolution;
   double                  sigma, eta, dx, x0, startX, endX;

   /* Create Analytic Fields */
   sigma = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solCA_sigma", 1.0 );
   eta = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solCA_eta", 1.0 );
   startX = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solCA_startX", 0.4 );
   endX = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solCA_endX", 0.8 );

   dx = endX - startX;
   x0 = 0.5 * (endX + startX );
   
   _Underworld_Velic_solCA_Init( self, sigma, eta, dx, x0 );
}

void _Underworld_Velic_solCA_Build( void* analyticSolution, void* data ) {
   Underworld_Velic_solCA* self = (Underworld_Velic_solCA*)analyticSolution;

   _Codelet_Build( self, data );

   /* Add this plugin's analytic functions into the register. */
   AnalyticFunction_Add( Underworld_Velic_solCA_VelocityFunction, (Name)"Underworld_Velic_solCA_VelocityFunction" );
   AnalyticFunction_Add( Underworld_Velic_solCA_PressureFunction, (Name)"Underworld_Velic_solCA_PressureFunction" );
   AnalyticFunction_Add( Underworld_Velic_solCA_StressFunction, (Name)"Underworld_Velic_solCA_StressFunction" );
   AnalyticFunction_Add( Underworld_Velic_solCA_StrainRateFunction, (Name)"Underworld_Velic_solCA_StrainRateFunction" );
}

Bool _checkInputParams( Underworld_Velic_solCA* self ) {
   return ( 
      ( self->sigma > 0.0 ) && ( self->eta > 0.0 ) &&
      ( self->dx >= 0.0 ) && ( self->x0 > 0 ) &&
      ( self->dx < 1.0 ) && ( self->x0 < 1.0 )
   );
}

void* _Underworld_Velic_solCA_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                             _sizeOfSelf = sizeof(Underworld_Velic_solCA);
   Type                                                     type = Underworld_Velic_solCA_Type;
   Stg_Class_DeleteFunction*                             _delete = _Codelet_Delete;
   Stg_Class_PrintFunction*                               _print = _Codelet_Print;
   Stg_Class_CopyFunction*                                 _copy = _Codelet_Copy;
   Stg_Component_DefaultConstructorFunction* _defaultConstructor = _Underworld_Velic_solCA_DefaultNew;
   Stg_Component_ConstructFunction*                   _construct = _Underworld_Velic_solCA_AssignFromXML;
   Stg_Component_BuildFunction*                           _build = _Underworld_Velic_solCA_Build;
   Stg_Component_InitialiseFunction*                 _initialise = _Codelet_Initialise;
   Stg_Component_ExecuteFunction*                       _execute = _Codelet_Execute;
   Stg_Component_DestroyFunction*                       _destroy = _Codelet_Destroy;

   /* 
    * Variables that are set to ZERO are variables that will be set either by the 
    * current _New function or another parent _New function further up the hierachy.
    */

   /* default value NON_GLOBAL */
   AllocationType nameAllocationType = NON_GLOBAL;

   return _AnalyticSolution_New( ANALYTICSOLUTION_PASSARGS );
}

Index Underworld_Velic_solCA_Register( PluginsManager* pluginsManager ) {
   return PluginsManager_Submit( pluginsManager, Underworld_Velic_solCA_Type, (Name)"0", _Underworld_Velic_solCA_DefaultNew );
}
