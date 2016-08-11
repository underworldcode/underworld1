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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include <assert.h>
#include <string.h>

const Type DepthDependentAnalytic3D_Type = "DepthDependentAnalytic3D";

typedef struct {
   __Codelet;
   AnalyticFunction* viscosityFunc;
   AnalyticFunction* viscosityDerivativeFunc;
   AnalyticFunction* viscosity2ndDerivativeFunc;
   double            Ra;
   double            V0;
   FeMesh*           elementMesh; 
} DepthDependentAnalytic3D;

void DepthDependentAnalytic3D_ViscosityFunc_Isoviscous( void* analyticSolution, double* coord, double* eta ) {
   *eta = 1.0;
}
void DepthDependentAnalytic3D_ViscosityDerivativeFunc_Isoviscous( void* analyticSolution, double* coord, double* d_eta_dy ) {
   *d_eta_dy = 0.0;
}
void DepthDependentAnalytic3D_Viscosity2ndDerivativeFunc_Isoviscous( void* analyticSolution, double* coord, double* d2_eta_dy2 ) {
   *d2_eta_dy2 = 0.0;
}

void DepthDependentAnalytic3D_ViscosityFunc_Linear( void* analyticSolution, double* coord, double* eta ) {
   double y = coord[ J_AXIS ];
   *eta = 2.0 - y;
}

void DepthDependentAnalytic3D_ViscosityDerivativeFunc_Linear( void* analyticSolution, double* coord, double* d_eta_dy ) {
   *d_eta_dy = -1.0;
}

void DepthDependentAnalytic3D_Viscosity2ndDerivativeFunc_Linear( void* analyticSolution, double* coord, double* d2_eta_dy2 ) {
   *d2_eta_dy2 = 0.0;
}

void DepthDependentAnalytic3D_ViscosityFunc_Exponential( void* analyticSolution, double* coord, double* eta ) {
   double y = coord[ J_AXIS ];
   *eta = exp( 1.0 - y );
}

void DepthDependentAnalytic3D_ViscosityDerivativeFunc_Exponential( void* analyticSolution, double* coord, double* d_eta_dy ) {
   double y = coord[ J_AXIS ];
   *d_eta_dy = - exp( 1.0 - y );
}

void DepthDependentAnalytic3D_Viscosity2ndDerivativeFunc_Exponential( void* analyticSolution, double* coord, double* d2_eta_dy2 ) {
   double y = coord[ J_AXIS ];
   *d2_eta_dy2 = exp( 1.0 - y );
}

void DepthDependentAnalytic3D_ViscosityFunc_Exponential2( void* analyticSolution, double* coord, double* eta ) {
   double y = coord[ J_AXIS ];
   double viscosityContrast = 1e6;
   double gamma = log(viscosityContrast);
   *eta = viscosityContrast * exp( - gamma *( 1.0 - y ) );
}

void DepthDependentAnalytic3D_ViscosityDerivativeFunc_Exponential2( void* analyticSolution, double* coord, double* d_eta_dy ) {
   double y = coord[ J_AXIS ];
   double viscosityContrast = 1e6;
   double gamma = log(viscosityContrast);
   *d_eta_dy = viscosityContrast * gamma * exp( - gamma *( 1.0 - y ) );
}

void DepthDependentAnalytic3D_Viscosity2ndDerivativeFunc_Exponential2( void* analyticSolution, double* coord, double* d2_eta_dy2 ) {
   double y = coord[ J_AXIS ];
   double viscosityContrast = 1e6;
   double gamma = log(viscosityContrast);
   *d2_eta_dy2 = viscosityContrast * gamma * gamma * exp( - gamma *( 1.0 - y ) );
}

void _DepthDependentAnalytic3D_VelocityFunction( void* _context, double* coord, double* velocity ) {
   double                    V0 ;
   double                    x; 
   double                    y;
   double                    z;
   XYZ                       min, max;
   UnderworldContext*        context = (UnderworldContext*)_context;
   DepthDependentAnalytic3D* self = (DepthDependentAnalytic3D*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)DepthDependentAnalytic3D_Type );

   V0 = self->V0;

   Mesh_GetGlobalCoordRange( self->elementMesh, min, max );
   x = coord[ I_AXIS ] - min[ I_AXIS ];
   y = coord[ J_AXIS ] - min[ J_AXIS ];
   z = coord[ K_AXIS ] - min[ K_AXIS ];

   velocity[ I_AXIS ] =        V0 * sin( M_PI * x ) * cos( M_PI * y ) * cos( M_PI * z ) ;
   velocity[ J_AXIS ] = -2.0 * V0 * cos( M_PI * x ) * sin( M_PI * y ) * cos( M_PI * z ) ;
   velocity[ K_AXIS ] =        V0 * cos( M_PI * x ) * cos( M_PI * y ) * sin( M_PI * z ) ;
}

void _DepthDependentAnalytic3D_PressureFunction( void* _context, double* coord, double* pressure ) {
   double                    x; 
   double                    y;
   double                    z;
   XYZ                       min, max;
   double                    V0;
   double                    Ra;
   double                    C = 0;
   double                    f = 0;
   double                    eta, d_eta_dy;
   UnderworldContext*        context = (UnderworldContext*)_context;
   DepthDependentAnalytic3D* self = (DepthDependentAnalytic3D*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)DepthDependentAnalytic3D_Type );

   V0 = self->V0;
   Ra = self->Ra;

   Mesh_GetGlobalCoordRange( self->elementMesh, min, max );
   x = coord[ I_AXIS ] - min[ I_AXIS ];
   y = coord[ J_AXIS ] - min[ J_AXIS ];
   z = coord[ K_AXIS ] - min[ K_AXIS ];

   f = - Ra*y*y*0.5 + Ra*y + C;
   
   /* Get Viscositiy and derivatives */
   self->viscosityFunc->apply( self, coord, &eta );
   self->viscosityDerivativeFunc->apply( self, coord, &d_eta_dy );

   *pressure = V0 * cos( M_PI * x ) * cos( M_PI * z ) * ( 3.0 * eta * M_PI * cos( M_PI * y ) - d_eta_dy * sin( M_PI * y ) ) + f;
}

void _DepthDependentAnalytic3D_TemperatureFunction( void* _context, double* coord, double* temperature ) {
   double                    x; 
   double                    y;
   double                    z;
   double                    V0;
   double                    Ra;
   double                    perturbation;
   double                    eta, d_eta_dy, d2_eta_dy2;
   UnderworldContext*        context = (UnderworldContext*)_context;
   DepthDependentAnalytic3D* self = (DepthDependentAnalytic3D*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)DepthDependentAnalytic3D_Type );

   V0 = self->V0;
   Ra = self->Ra;
   
   /* Find coordinate of node */
   x = coord[ I_AXIS ];
   y = coord[ J_AXIS ];
   z = coord[ K_AXIS ];
   
   /* Get Viscositiy and derivatives */
   self->viscosityFunc->apply( self, coord, &eta );
   self->viscosityDerivativeFunc->apply( self, coord, &d_eta_dy );
   self->viscosity2ndDerivativeFunc->apply( self, coord, &d2_eta_dy2 );

   perturbation = V0 / Ra * cos( M_PI * x ) * cos( M_PI * z ) * 
      ( 6.0 * M_PI * d_eta_dy * cos( M_PI * y ) - ( 9.0 * M_PI * M_PI * eta + d2_eta_dy2 ) * sin( M_PI * y ) );

   *temperature = 1 - y + perturbation;
}

void _DepthDependentAnalytic3D_ViscosityFunction( void* _context, double* coord, double* viscosity ) {
   UnderworldContext*        context = (UnderworldContext*)_context;
   DepthDependentAnalytic3D* self = (DepthDependentAnalytic3D*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)DepthDependentAnalytic3D_Type );

   self->viscosityFunc->apply( self, coord, viscosity );
}

void DepthDependentAnalytic3D_TemperatureIC( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ) {
   DomainContext*            context = (DomainContext*)_context;
   FeVariable*               temperatureField = NULL;
   FeMesh*                   mesh = NULL;
   double*                   temperature = (double*) _result;
   double*                   coord = NULL;

   temperatureField = (FeVariable*) FieldVariable_Register_GetByName( context->fieldVariable_Register,
      "TemperatureField" );
   
   mesh = temperatureField->feMesh;
   
   /* Find coordinate of node */
   coord = Mesh_GetVertex( mesh, node_lI );
   
   _DepthDependentAnalytic3D_TemperatureFunction( context, coord, temperature );
}

void DepthDependentAnalytic3D_PressureIC( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ) {
   DomainContext*            context = (DomainContext*)_context;
   FeVariable*               PressureField = NULL;
   FeMesh*                   mesh = NULL;
   double*                   pressure = (double*) _result;
   double*                   coord = NULL;

   PressureField = (FeVariable*)FieldVariable_Register_GetByName( context->fieldVariable_Register,
      "PressureField" );

   mesh = PressureField->feMesh;

   /* Find coordinate of node */
   coord = Mesh_GetVertex( mesh, node_lI );
   
   _DepthDependentAnalytic3D_PressureFunction( context, coord, pressure );
}

void _DepthDependentAnalytic3D_Init(
   DepthDependentAnalytic3D* self,
   double                    Ra,
   double                    V0,
   FeMesh*                   elementMesh,
   char                      *viscosityType )
{
   if ( strcasecmp( viscosityType, "Isoviscous" ) == 0 ) {
      self->viscosityFunc = AnalyticFunction_New( 
         DepthDependentAnalytic3D_ViscosityFunc_Isoviscous,
         (Name)"DepthDependentAnalytic3D_ViscosityFunc_Isoviscous" );

      self->viscosityDerivativeFunc = AnalyticFunction_New( 
         DepthDependentAnalytic3D_ViscosityDerivativeFunc_Isoviscous,
         (Name)"DepthDependentAnalytic3D_ViscosityDerivativeFunc_Isoviscous" );

      self->viscosity2ndDerivativeFunc = AnalyticFunction_New(
         DepthDependentAnalytic3D_Viscosity2ndDerivativeFunc_Isoviscous,
         (Name)"DepthDependentAnalytic3D_Viscosity2ndDerivativeFunc_Isoviscous" );
   }
   else if ( strcasecmp( viscosityType, "Linear" ) == 0 ) {
      self->viscosityFunc = AnalyticFunction_New( 
         DepthDependentAnalytic3D_ViscosityFunc_Linear,
         (Name)"DepthDependentAnalytic3D_ViscosityFunc_Linear" );

      self->viscosityDerivativeFunc = AnalyticFunction_New(
         DepthDependentAnalytic3D_ViscosityDerivativeFunc_Linear,
         (Name)"DepthDependentAnalytic3D_ViscosityDerivativeFunc_Linear" );

      self->viscosity2ndDerivativeFunc = AnalyticFunction_New(
         DepthDependentAnalytic3D_Viscosity2ndDerivativeFunc_Linear,
         (Name)"DepthDependentAnalytic3D_Viscosity2ndDerivativeFunc_Linear" );
   }
   else if ( strcasecmp( viscosityType, "Exponential" ) == 0 ) {
      self->viscosityFunc = AnalyticFunction_New( 
         DepthDependentAnalytic3D_ViscosityFunc_Exponential,
         (Name)"DepthDependentAnalytic3D_ViscosityFunc_Exponential" );

      self->viscosityDerivativeFunc = AnalyticFunction_New(
         DepthDependentAnalytic3D_ViscosityDerivativeFunc_Exponential,
         (Name)"DepthDependentAnalytic3D_ViscosityDerivativeFunc_Exponential" );
      
      self->viscosity2ndDerivativeFunc = AnalyticFunction_New(
         DepthDependentAnalytic3D_Viscosity2ndDerivativeFunc_Exponential,
         (Name)"DepthDependentAnalytic3D_Viscosity2ndDerivativeFunc_Exponential");
   }
   else if ( strcasecmp( viscosityType, "Exponential2" ) == 0 ) {
      self->viscosityFunc = AnalyticFunction_New(
         DepthDependentAnalytic3D_ViscosityFunc_Exponential2,
         (Name)"DepthDependentAnalytic3D_ViscosityFunc_Exponential2" );
      
      self->viscosityDerivativeFunc = AnalyticFunction_New(
         DepthDependentAnalytic3D_ViscosityDerivativeFunc_Exponential2,
         (Name)"DepthDependentAnalytic3D_ViscosityDerivativeFunc_Exponential2" );

      self->viscosity2ndDerivativeFunc = AnalyticFunction_New(
         DepthDependentAnalytic3D_Viscosity2ndDerivativeFunc_Exponential2,
         (Name)"DepthDependentAnalytic3D_Viscosity2ndDerivativeFunc_Exponential2" );
   }

   self->Ra = Ra;
   self->V0 = V0;
   self->elementMesh = elementMesh;
}

void _DepthDependentAnalytic3D_AssignFromXML( void* analyticSolution, Stg_ComponentFactory* cf, void* data ) {
   DepthDependentAnalytic3D* self = (DepthDependentAnalytic3D*)analyticSolution;
   ConditionFunction*        condFunc;
   Dictionary_Entry_Value*   dictEntryVal;
   Dictionary*               pluginDict;
   FeMesh*                   elementMesh;
   char*                     viscosityType;
   double                    Ra, V0;

   /* Get pluginData info from the XML. */
   dictEntryVal = Dictionary_Get( cf->rootDict, (Dictionary_Entry_Key)"pluginData" );
   pluginDict = Dictionary_Entry_Value_AsDictionary( dictEntryVal );
   
   /* Add temperature initial condition */
   condFunc = ConditionFunction_New(
      DepthDependentAnalytic3D_TemperatureIC,
      (Name)"DepthDependentAnalytic3D_TemperatureIC", NULL );
   ConditionFunction_Register_Add( condFunc_Register, condFunc );

   /* Add pressure initial condition */
   condFunc = ConditionFunction_New(
      DepthDependentAnalytic3D_PressureIC,
      (Name)"DepthDependentAnalytic3D_PressureIC", NULL );
   ConditionFunction_Register_Add( condFunc_Register, condFunc );

   /* Setup Viscosity Functions */
   viscosityType = Stg_ComponentFactory_GetRootDictString(
      cf,
      (Dictionary_Entry_Key)"ViscosityType",
      "Isoviscous" );

   /* Get the element mesh. */
   elementMesh = (FeMesh*)((FieldTest*)LiveComponentRegister_Get( cf->LCRegister, (Name)"FieldTest" ))->elementMesh;

   Ra = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"Ra", 0.0 );
   V0 = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"V0", 0.0 );

   _DepthDependentAnalytic3D_Init( self, Ra, V0, elementMesh, viscosityType );
}

void _DepthDependentAnalytic3D_Build( void* analyticSolution, void* data ) {
   DepthDependentAnalytic3D* self = (DepthDependentAnalytic3D*)analyticSolution;

   _Codelet_Build( self, data );

   /* Add this plugin's analytic functions into the register. */
   AnalyticFunction_Add( _DepthDependentAnalytic3D_VelocityFunction, (Name)"_DepthDependentAnalytic3D_VelocityFunction" );
   AnalyticFunction_Add( _DepthDependentAnalytic3D_PressureFunction, (Name)"_DepthDependentAnalytic3D_PressureFunction" );
   AnalyticFunction_Add( _DepthDependentAnalytic3D_TemperatureFunction, (Name)"_DepthDependentAnalytic3D_TemperatureFunction" );
   AnalyticFunction_Add( _DepthDependentAnalytic3D_ViscosityFunction, (Name)"_DepthDependentAnalytic3D_ViscosityFunction" );
}

void* _DepthDependentAnalytic3D_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                             _sizeOfSelf = sizeof(DepthDependentAnalytic3D);
   Type                                                     type = DepthDependentAnalytic3D_Type;
   Stg_Class_DeleteFunction*                             _delete = _Codelet_Delete;
   Stg_Class_PrintFunction*                               _print = _Codelet_Print;
   Stg_Class_CopyFunction*                                 _copy = _Codelet_Copy;
   Stg_Component_DefaultConstructorFunction* _defaultConstructor = _DepthDependentAnalytic3D_DefaultNew;
   Stg_Component_ConstructFunction*                   _construct = _DepthDependentAnalytic3D_AssignFromXML;
   Stg_Component_BuildFunction*                           _build = _DepthDependentAnalytic3D_Build;
   Stg_Component_InitialiseFunction*                 _initialise = _Codelet_Initialise;
   Stg_Component_ExecuteFunction*                       _execute = _Codelet_Execute;
   Stg_Component_DestroyFunction*                       _destroy = _Codelet_Destroy;

   /* 
    * Variables that are set to ZERO are variables that will be set either by the current
    *_New function or another parent _New function further up the hierachy 
    */

   /* default value NON_GLOBAL */
   AllocationType nameAllocationType = NON_GLOBAL;

   return _Codelet_New( CODELET_PASSARGS );
}

Index Underworld_DepthDependentAnalytic3D_Register( PluginsManager* pluginsManager ) {
   return PluginsManager_Submit( pluginsManager, DepthDependentAnalytic3D_Type, (Name)"0", _DepthDependentAnalytic3D_DefaultNew );
}
