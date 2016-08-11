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

/* This analytic solution is provided by Spiegelman & Katz, "A semi-Lagrangian Crank-Nicolson algorithm for the numerical         */
/* solution of advection-diffusion problems", G^3, Vol 7, Num 4, 2006.                                                            */


#include "Underworld_PlaneWaveShearAdvectionDiffusion.h"

const Type Underworld_PlaneWaveShearAdvectionDiffusion_Type = "Underworld_PlaneWaveShearAdvectionDiffusion";

Underworld_PlaneWaveShearAdvectionDiffusion* Underworld_PlaneWaveShearAdvectionDiffusion_GlobalSelf;

double Underworld_PlaneWaveShearAdvectionDiffusion_dt( void* _class, PICelleratorContext* context ) {
        return Underworld_PlaneWaveShearAdvectionDiffusion_GlobalSelf->dt;
}

void Underworld_PlaneWaveShearAdvectionDiffusion_ShearVelocityX( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ) {
   UnderworldContext* context = (UnderworldContext*)_context;
   Underworld_PlaneWaveShearAdvectionDiffusion* self = Underworld_PlaneWaveShearAdvectionDiffusion_GlobalSelf;
   FeVariable*    velocityField = (FeVariable*) FieldVariable_Register_GetByName( context->fieldVariable_Register, "VelocityField" );
   FeMesh*        feMesh = velocityField->feMesh;
   double*        result = (double*) _result;
   double*        coord;

   /* Find coordinate of node */
   coord = Mesh_GetVertex( feMesh, node_lI );

   *result = self->Pe*coord[J_AXIS];
}

void Underworld_PlaneWaveShearAdvectionDiffusion_InitTemperature( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ) {
   UnderworldContext* context = (UnderworldContext*)_context;
   FeVariable*    temperatureField = (FeVariable*) FieldVariable_Register_GetByName( context->fieldVariable_Register, "TemperatureField" );
   FeMesh*        feMesh = temperatureField->feMesh;
   double*        result = (double*) _result;
   double*        coord;

   /* Find coordinate of node */
   coord = Mesh_GetVertex( feMesh, node_lI );

   *result = _PlaneWaveShearAdvectionDiffusion_Solution(coord[0],coord[1],0);
}

void Underworld_PlaneWaveShearAdvectionDiffusion_Temperature( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ) {
   UnderworldContext* context = (UnderworldContext*)_context;
   FeVariable*    temperatureField = (FeVariable*) FieldVariable_Register_GetByName( context->fieldVariable_Register, "TemperatureField" );
   FeMesh*        feMesh = temperatureField->feMesh;
   double*        result = (double*) _result;
   double*        coord;

   /* Find coordinate of node */
   coord = Mesh_GetVertex( feMesh, node_lI );

   *result = _PlaneWaveShearAdvectionDiffusion_Solution(coord[0],coord[1],context->currentTime);
}

void Underworld_PlaneWaveShearAdvectionDiffusion_TemperatureFunction( void* _context, double* coord, double* temp ) {
   UnderworldContext*      context = (UnderworldContext*)_context;

   *temp = _PlaneWaveShearAdvectionDiffusion_Solution( coord[0], coord[1], context->currentTime );
}

void _Underworld_PlaneWaveShearAdvectionDiffusion_Init( Underworld_PlaneWaveShearAdvectionDiffusion* self, double kx, double ky, double Pe, double dt ) {
   self->kx = kx;
   self->ky = ky;
   self->Pe = Pe;
   self->dt = dt;
}

void _Underworld_PlaneWaveShearAdvectionDiffusion_Build( void* analyticSolution, void* data ) {
   Underworld_PlaneWaveShearAdvectionDiffusion* self = (Underworld_PlaneWaveShearAdvectionDiffusion*)analyticSolution;

   _Codelet_Build( self, data );

   /* Add this plugin's analytic functions into the register. */
   AnalyticFunction_Add( Underworld_PlaneWaveShearAdvectionDiffusion_TemperatureFunction, (Name)"Underworld_PlaneWaveShearAdvectionDiffusion_TemperatureFunction" );
}

void _Underworld_PlaneWaveShearAdvectionDiffusion_AssignFromXML( void* analyticSolution, Stg_ComponentFactory* cf, void* data ) {
   Underworld_PlaneWaveShearAdvectionDiffusion* self = (Underworld_PlaneWaveShearAdvectionDiffusion*)analyticSolution;
   double                  kx, ky, Pe, dt;
   UnderworldContext*      context;

   context = (UnderworldContext*)Stg_ComponentFactory_PluginConstructByKey( cf, self, (Dictionary_Entry_Key)"Context", UnderworldContext, True, data );

   kx = Stg_ComponentFactory_PluginGetDouble( cf, self, (Dictionary_Entry_Key)"kx", 1.0  );
   ky = Stg_ComponentFactory_PluginGetDouble( cf, self, (Dictionary_Entry_Key)"ky", 1.0  );
   Pe = Stg_ComponentFactory_PluginGetDouble( cf, self, (Dictionary_Entry_Key)"Pe", 1.0  );
   dt = Stg_ComponentFactory_PluginGetDouble( cf, self, (Dictionary_Entry_Key)"dt", 0.01 );

   EP_AppendClassHook( Context_GetEntryPoint( context, FiniteElementContext_EP_CalcDt ), Underworld_PlaneWaveShearAdvectionDiffusion_dt, context );

   _Underworld_PlaneWaveShearAdvectionDiffusion_Init( self, kx, ky, Pe, dt );
}

void* _Underworld_PlaneWaveShearAdvectionDiffusion_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                             _sizeOfSelf = sizeof(Underworld_PlaneWaveShearAdvectionDiffusion);
   Type                                                     type = Underworld_PlaneWaveShearAdvectionDiffusion_Type;
   Stg_Class_DeleteFunction*                             _delete = _Codelet_Delete;
   Stg_Class_PrintFunction*                               _print = _Codelet_Print;
   Stg_Class_CopyFunction*                                 _copy = _Codelet_Copy;
   Stg_Component_DefaultConstructorFunction* _defaultConstructor = _Underworld_PlaneWaveShearAdvectionDiffusion_DefaultNew;
   Stg_Component_ConstructFunction*                   _construct = _Underworld_PlaneWaveShearAdvectionDiffusion_AssignFromXML;
   Stg_Component_BuildFunction*                           _build = _Underworld_PlaneWaveShearAdvectionDiffusion_Build;
   Stg_Component_InitialiseFunction*                 _initialise = _Codelet_Initialise;
   Stg_Component_ExecuteFunction*                       _execute = _Codelet_Execute;
   Stg_Component_DestroyFunction*                       _destroy = _Codelet_Destroy;

   /*
    * Variables that are set to ZERO are variables that will be set either by the
    * current _New function or another parent _New function further up the hierachy.
    */

   /* default value NON_GLOBAL */
   AllocationType nameAllocationType = NON_GLOBAL;

   /* go ahead and add these to the register now */
   ConditionFunction_Register_Add( condFunc_Register, ConditionFunction_New( Underworld_PlaneWaveShearAdvectionDiffusion_ShearVelocityX,  (Name)"Underworld_PlaneWaveShearAdvectionDiffusion_ShearVelocityX", NULL  ) );
   ConditionFunction_Register_Add( condFunc_Register, ConditionFunction_New( Underworld_PlaneWaveShearAdvectionDiffusion_InitTemperature, (Name)"Underworld_PlaneWaveShearAdvectionDiffusion_InitTemperature", NULL ) );
   ConditionFunction_Register_Add( condFunc_Register, ConditionFunction_New( Underworld_PlaneWaveShearAdvectionDiffusion_Temperature, (Name)"Underworld_PlaneWaveShearAdvectionDiffusion_Temperature", NULL ) );

   Underworld_PlaneWaveShearAdvectionDiffusion_GlobalSelf = (Underworld_PlaneWaveShearAdvectionDiffusion*)_Codelet_New( CODELET_PASSARGS );
   return Underworld_PlaneWaveShearAdvectionDiffusion_GlobalSelf;
}

Index Underworld_PlaneWaveShearAdvectionDiffusion_Register( PluginsManager* pluginsManager ) {
   return PluginsManager_Submit( pluginsManager, Underworld_PlaneWaveShearAdvectionDiffusion_Type, (Name)"0", _Underworld_PlaneWaveShearAdvectionDiffusion_DefaultNew  );
}

double _PlaneWaveShearAdvectionDiffusion_Solution(double x, double y, double t){
   Underworld_PlaneWaveShearAdvectionDiffusion* self = Underworld_PlaneWaveShearAdvectionDiffusion_GlobalSelf;
   double Pe    = self->Pe;
   double kx    = self->kx;
   double ky    = self->ky;
   double k2    = pow(kx,2) + pow(ky,2);
   double s     = -t*(k2 - Pe*kx*ky*t + (1./3.)*pow(Pe*kx,2)*pow(t,2));
   double kdotx = kx*(x-Pe*y*t) + ky*y;

   return cos(kdotx)*exp(s);
}
