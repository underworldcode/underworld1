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

#include "Underworld_Velic_solB.h"

const Type Underworld_Velic_solB_Type = "Underworld_Velic_solB";

void Underworld_Velic_solB_VelocityFunction( void* _context, double* coord, double* velocity ) {
   UnderworldContext*     context = (UnderworldContext*)_context;
   Underworld_Velic_solB* self = (Underworld_Velic_solB*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solB_Type );

   _Velic_solB( coord, self->sigma, self->Z, self->n, self->km, velocity, NULL, NULL, NULL );
}

void Underworld_Velic_solB_PressureFunction( void* _context, double* coord, double* pressure ) {
   UnderworldContext*     context = (UnderworldContext*)_context;
   Underworld_Velic_solB* self = (Underworld_Velic_solB*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solB_Type );
   
   _Velic_solB( coord, self->sigma, self->Z, self->n, self->km, NULL, pressure, NULL, NULL );
}

void Underworld_Velic_solB_StressFunction( void* _context, double* coord, double* stress ) {
   UnderworldContext*     context = (UnderworldContext*)_context;
   Underworld_Velic_solB* self = (Underworld_Velic_solB*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solB_Type );
   
   _Velic_solB( coord, self->sigma, self->Z, self->n, self->km, NULL, NULL, stress, NULL );
}

void Underworld_Velic_solB_StrainRateFunction( void* _context, double* coord, double* strainRate ) {
   UnderworldContext*     context = (UnderworldContext*)_context;
   Underworld_Velic_solB* self = (Underworld_Velic_solB*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)Underworld_Velic_solB_Type );
   
   _Velic_solB( coord, self->sigma, self->Z, self->n, self->km, NULL, NULL, NULL, strainRate );
}

void _Underworld_Velic_solB_Init( Underworld_Velic_solB* self, double sigma, double Z, double wavenumberY, int n ) {
   Bool isCorrectInput = True;

   self->sigma = sigma;
   self->Z = Z;
   self-> km = wavenumberY;
   self->n = n; 

   isCorrectInput = solB_checkInputParams( self );

   Journal_Firewall( isCorrectInput, Journal_Register( Error_Type, (Name)"Analytic_solB" ),
      "Error in function %s: Bad Input parameters, solution check valid values in .tex documentation\n",
      __func__ );
}

void _Underworld_Velic_solB_AssignFromXML( void* analyticSolution, Stg_ComponentFactory* cf, void* data ) {
   Underworld_Velic_solB* self = (Underworld_Velic_solB*)analyticSolution;
   double                 sigma, Z, wavenumberY, n;

   sigma = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solB_sigma", 1.0 );
   Z = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"solB_Z", 1.0 );
   wavenumberY = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"wavenumberY", 1.0 );
   n = Stg_ComponentFactory_GetRootDictInt( cf, "wavenumberX", 1 );
   
   _Underworld_Velic_solB_Init( self, sigma, Z, wavenumberY, n );
}

void _Underworld_Velic_solB_Build( void* analyticSolution, void* data ) {
   Underworld_Velic_solB* self = (Underworld_Velic_solB*)analyticSolution;

   _Codelet_Build( self, data );

   /* Add this plugin's analytic functions into the register. */
   AnalyticFunction_Add( Underworld_Velic_solB_VelocityFunction, (Name)"Underworld_Velic_solB_VelocityFunction" );
   AnalyticFunction_Add( Underworld_Velic_solB_PressureFunction, (Name)"Underworld_Velic_solB_PressureFunction" );
   AnalyticFunction_Add( Underworld_Velic_solB_StressFunction, (Name)"Underworld_Velic_solB_StressFunction" );
   AnalyticFunction_Add( Underworld_Velic_solB_StrainRateFunction, (Name)"Underworld_Velic_solB_StrainRateFunction" );
}

Bool solB_checkInputParams( Underworld_Velic_solB* self ) {
   return ( 
      ( self->sigma > 0.0 ) && ( self->Z > 0.0 ) &&
      ( self->km > 0.0 ) && ( self->n > 0 )  
   );
}

void* _Underworld_Velic_solB_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                             _sizeOfSelf = sizeof(Underworld_Velic_solB);
   Type                                                     type = Underworld_Velic_solB_Type;
   Stg_Class_DeleteFunction*                             _delete = _Codelet_Delete;
   Stg_Class_PrintFunction*                               _print = _Codelet_Print;
   Stg_Class_CopyFunction*                                 _copy = _Codelet_Copy;
   Stg_Component_DefaultConstructorFunction* _defaultConstructor = _Underworld_Velic_solB_DefaultNew;
   Stg_Component_ConstructFunction*                   _construct = _Underworld_Velic_solB_AssignFromXML;
   Stg_Component_BuildFunction*                           _build = _Underworld_Velic_solB_Build;
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

Index Underworld_Velic_solB_Register( PluginsManager* pluginsManager ) {
   return PluginsManager_Submit( pluginsManager, Underworld_Velic_solB_Type, (Name)"0", _Underworld_Velic_solB_DefaultNew );
}


