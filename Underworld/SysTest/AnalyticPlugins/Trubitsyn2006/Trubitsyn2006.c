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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include <assert.h>
#include <string.h>

#define SMALL 1.0e-5
#define IS_ODD(A)   ( (A) % 2 == 1 )

const Type Trubitsyn2006_Type = "Underworld_Trubitsyn2006";

typedef struct {
   __Codelet;
   AnalyticFunction* viscosityFunc;
   AnalyticFunction* viscosityDerivativeFunc;
   FeVariable*       velocityField;
   FeVariable*       pressureField;
   double            Ra;
   double            T0;
   int               wavenumberX;
   int               wavenumberY;
} Trubitsyn2006;

/** Analytic Solution taken from 
 * V. P. Trubitsyn, A. A. Baranov, A. N. Eyseev, and A. P. Trubitsyn. 
 *    Exact analytical solutions of the stokes equation for testing the equations of mantle convection with a variable viscosity. 
 *   Izvestiya Physics of the Solid Earth, 42(7):537–545, 2006. 
 *  
 *  All equations refer to this paper */

double Trubitsyn_G( double x, double x0, double a ) {
   return 1/(1 + exp( -2.0 * a * ( x - x0 ) ) );
}

double Trubitsyn_GDeriv( double x, double x0, double a ) {
   return 2.0 * a * pow( exp( a * ( x - x0 ) ) + exp( -a * ( x - x0 ) ), -2.0 );
}

void _Trubitsyn2006_ViscosityFunc_Isoviscous( void* analyticSolution, double* coord, double* eta ) {
   *eta = 1.0;
}

void _Trubitsyn2006_ViscosityDerivativeFunc_Isoviscous( void* analyticSolution, double* coord, double* viscDeriv ) {
   viscDeriv[0] = 0.0;
   viscDeriv[1] = 0.0;
}

void _Trubitsyn2006_ViscosityFunc_Model1( void* analyticSolution, double* coord, double* eta ) {
   *eta = 1.0 + 100.0 * coord[ I_AXIS ] * coord[ J_AXIS ];
}

void _Trubitsyn2006_ViscosityDerivativeFunc_Model1( void* analyticSolution, double* coord, double* viscDeriv ) {
   viscDeriv[0] = 100.0 * coord[ J_AXIS ];
   viscDeriv[1] = 100.0 * coord[ I_AXIS ];
}

void _Trubitsyn2006_ViscosityFunc_Model2( void* analyticSolution, double* coord, double* eta ) {
   /**eta = 999.0 * ( 1.0 - Trubitsyn_G( coord[ I_AXIS ], 0.5, 50 ) ) + 1;*/
   *eta = ( coord[ I_AXIS ] <= 0.5 ? 1000.0 : 1 );
}

void _Trubitsyn2006_ViscosityDerivativeFunc_Model2( void* analyticSolution, double* coord, double* viscDeriv ) {
   /*viscDeriv[0] = -999.0 * Trubitsyn_GDeriv( coord[ I_AXIS ], 0.5, 50 );*/
   viscDeriv[0] = 0.0;
   viscDeriv[1] = 0.0;
}

void _Trubitsyn2006_ViscosityFunc_Model3( void* analyticSolution, double* coord, double* eta ) {
   /**eta = 999.0 * ( 1.0 - Trubitsyn_G( coord[ J_AXIS ], 0.5, 50 ) ) + 1;*/
   *eta = ( coord[ J_AXIS ] <= 0.5 ? 1000.0 : 1 );
}

void _Trubitsyn2006_ViscosityDerivativeFunc_Model3( void* analyticSolution, double* coord, double* viscDeriv ) {
   viscDeriv[0] = 0.0;
   viscDeriv[1] = 0.0;
   /*viscDeriv[1] = -999.0 * Trubitsyn_GDeriv( coord[ J_AXIS ], 0.5, 50 );*/
}

double Trubitsyn2006_V0( void* analyticSolution ) {
   Trubitsyn2006* self = (Trubitsyn2006*)analyticSolution;
   double         T0 = self->T0;
   double         Ra = self->Ra;

   /* NB - Paper Trubitsyn [2006] is in error for this substitution - they leave out the square on pi */
   return 0.25 * Ra * T0 * M_1_PI * M_1_PI;
}

void _Trubitsyn2006_VelocityFunction( void* _context, double* coord, double* velocity ) {
   UnderworldContext* context = (UnderworldContext*)_context;
   Trubitsyn2006*     self = (Trubitsyn2006*)LiveComponentRegister_Get( context->CF->LCRegister, (Name)Trubitsyn2006_Type );
   double             v0 = Trubitsyn2006_V0( self );
   double             x; 
   double             y;
   XYZ                min, max;

   Mesh_GetGlobalCoordRange( self->velocityField->feMesh, min, max );
   x = coord[ I_AXIS ] - min[ I_AXIS ];
   y = coord[ J_AXIS ] - min[ J_AXIS ];

   velocity[ J_AXIS ] =   v0 * sin( M_PI * y ) * cos( M_PI * x ); /* Equation 31 */
   velocity[ I_AXIS ] = - v0 * cos( M_PI * y ) * sin( M_PI * x ); /* Equation 32 */
}

void _Trubitsyn2006_PressureFunction( void* _context, double* coord, double* pressure ) {
   UnderworldContext* context = (UnderworldContext*)_context;
   Trubitsyn2006*     self = (Trubitsyn2006*)LiveComponentRegister_Get( context->CF->LCRegister, (Name)Trubitsyn2006_Type );
   double             T0 = self->T0;
   double             Ra = self->Ra;
   double             x; 
   double             y;
   XYZ                min, max;
   double             eta;
   double             eta01;
   double             upperLeftCorner[]  = { 0.0, 1.0, 0.0 };
   
   Mesh_GetGlobalCoordRange( self->velocityField->feMesh, min, max );
   x = coord[ I_AXIS ] - min[ I_AXIS ];
   y = coord[ J_AXIS ] - min[ J_AXIS ];

   /* Get Viscosities */
   self->viscosityFunc->apply( self, coord, &eta );
   self->viscosityFunc->apply( self, upperLeftCorner, &eta01 );

   *pressure = - Ra * ( ( y * y * 0.5 - y + 0.5 )
         + 0.5 * T0 / M_PI * ( eta * cos( M_PI * y ) * cos( M_PI * x ) + eta01 ) );/* Equation 46 */
   /*printf("pressure from t0 = %g\n", *pressure );
   *pressure = - 2.0 * M_PI * v0 * ( eta * cos( M_PI * y ) * cos( M_PI * x ) + eta01 ) 
         - Ra * ( y*y * 0.5 - y + 0.5 );
   printf("pressure from v0 = %g\n\n", *pressure );*/
}

void _Trubitsyn2006_ViscosityFunction( void* _context, double* coord, double* viscosity ) {
   UnderworldContext* context = (UnderworldContext*)_context;
   Trubitsyn2006*     self = (Trubitsyn2006*)LiveComponentRegister_Get( context->CF->LCRegister, (Name)Trubitsyn2006_Type );

   self->viscosityFunc->apply( self, coord, viscosity );
}

void _Trubitsyn2006_StreamFunction( void* _context, double* coord, double* psi ) {
   UnderworldContext* context = (UnderworldContext*)_context;
   Trubitsyn2006*     self = (Trubitsyn2006*)LiveComponentRegister_Get( context->CF->LCRegister, (Name)Trubitsyn2006_Type );
   double             v0 = Trubitsyn2006_V0( self );
   double             x; 
   double             y;
   XYZ                min, max;

   Mesh_GetGlobalCoordRange( self->velocityField->feMesh, min, max );
   x = coord[ I_AXIS ] - min[ I_AXIS ];
   y = coord[ J_AXIS ] - min[ J_AXIS ];

   *psi = - v0 / M_PI * sin( M_PI * y ) * sin( M_PI * x ); /* Equation 40 */
}

void Trubitsyn2006_TemperatureIC( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ) {
   DomainContext* context = (DomainContext*)_context;
   FeVariable*    temperatureField = NULL;
   FeMesh*        mesh = NULL;
   double*        temperature = (double*) _result;
   double*        coord;
   double         x; 
   double         y;
   Trubitsyn2006* self = NULL;
   double         T0;
   double         Ra;
   double         v0 ;
   XYZ            min, max;
   double         eta;
   double         d_eta_dy;
   XYZ            viscDeriv;

   self = Stg_ComponentFactory_ConstructByName( context->CF, (Name)Trubitsyn2006_Type,
      Trubitsyn2006, True, context );

   T0 = self->T0;
   Ra = self->Ra;
   v0 = Trubitsyn2006_V0( self );

   temperatureField = (FeVariable*) FieldVariable_Register_GetByName(
      context->fieldVariable_Register,
      "TemperatureField" );

   mesh = temperatureField->feMesh;
   
   /* Find coordinate of node */
   coord = Mesh_GetVertex( mesh, node_lI );
   Mesh_GetGlobalCoordRange( mesh, min, max );
   x = coord[ I_AXIS ] - min[ I_AXIS ];
   y = coord[ J_AXIS ] - min[ J_AXIS ];

   self->viscosityFunc->apply( self, coord, &eta );
   self->viscosityDerivativeFunc->apply( self, coord, viscDeriv );
   d_eta_dy = viscDeriv[ J_AXIS ];

   *temperature = 1 - y + T0 * cos( M_PI * x ) * ( eta * sin( M_PI * y ) - d_eta_dy / M_PI * cos( M_PI * y ) ) ;    /* Equation 47 */ 
   
   *temperature = 1 - y + 4.0 * M_PI * v0 / Ra * cos( M_PI * x ) * 
      ( M_PI * eta * sin( M_PI * y ) - d_eta_dy * cos( M_PI * y ) ) ;    /* Equation 47 */ 
}

void Trubitsyn2006_PressureIC( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ) {
   DomainContext* context = (DomainContext*)_context;
   FeVariable*    PressureField = NULL;
   FeMesh*        mesh;
   double*        pressure = (double*) _result;
   double*        coord;
  
   PressureField = (FeVariable*) FieldVariable_Register_GetByName( context->fieldVariable_Register,
      "PressureField" ); 

   mesh = PressureField->feMesh;

   /* Find coordinate of node */
   coord = Mesh_GetVertex( mesh, node_lI );
   
   _Trubitsyn2006_PressureFunction( context,  coord,  pressure );
}

void _Trubitsyn2006_Init( 
   Trubitsyn2006* self, 
   FeVariable*    velocityField,
   FeVariable*    pressureField,
   double         Ra,
   double         T0,
   int            wavenumberX,
   int            wavenumberY,
   char*          viscosityType )
{
   self->velocityField = velocityField;
   self->pressureField = pressureField;
   self->Ra = Ra;
   self->T0 = T0;
   self->wavenumberX = wavenumberX;
   self->wavenumberY = wavenumberY;   

   if ( strcasecmp( viscosityType, "Isoviscous" ) == 0 ) {
      self->viscosityFunc = AnalyticFunction_New(
         _Trubitsyn2006_ViscosityFunc_Isoviscous,
         (Name)"_Trubitsyn2006_ViscosityFunc_Isoviscous" );

      self->viscosityDerivativeFunc = AnalyticFunction_New(
         _Trubitsyn2006_ViscosityDerivativeFunc_Isoviscous,
         (Name)"_Trubitsyn2006_ViscosityDerivativeFunc_Isoviscous" );
   }
   else if ( strcasecmp( viscosityType, "Model1" ) == 0 ) {
      self->viscosityFunc = AnalyticFunction_New(
         _Trubitsyn2006_ViscosityFunc_Model1,
         (Name)"_Trubitsyn2006_ViscosityFunc_Model1" );

      self->viscosityDerivativeFunc = AnalyticFunction_New(
         _Trubitsyn2006_ViscosityDerivativeFunc_Model1,
         (Name)"_Trubitsyn2006_ViscosityDerivativeFunc_Model1" );
   }
   else if ( strcasecmp( viscosityType, "Model2" ) == 0 ) {
      self->viscosityFunc = AnalyticFunction_New(
         _Trubitsyn2006_ViscosityFunc_Model2,
         (Name)"_Trubitsyn2006_ViscosityFunc_Model2" );

      self->viscosityDerivativeFunc = AnalyticFunction_New(
         _Trubitsyn2006_ViscosityDerivativeFunc_Model2,
         (Name)"_Trubitsyn2006_ViscosityDerivativeFunc_Model2" );
   }
   else if ( strcasecmp( viscosityType, "Model3" ) == 0 ) {
      self->viscosityFunc = AnalyticFunction_New(
         _Trubitsyn2006_ViscosityFunc_Model3,
         (Name)"_Trubitsyn2006_ViscosityFunc_Model3" );

      self->viscosityDerivativeFunc = AnalyticFunction_New(
         _Trubitsyn2006_ViscosityDerivativeFunc_Model3,
         (Name)"_Trubitsyn2006_ViscosityDerivativeFunc_Model3" ); 
   }
   else {
      Journal_Printf(
         Journal_Register( Error_Type, (Name)viscosityType ),
         "Cannot understand viscosity type = '%s'\n",
         viscosityType );
      abort();
   }
}    

void _Trubitsyn2006_AssignFromXML( void* analyticSolution, Stg_ComponentFactory* cf, void* data ) {
   Trubitsyn2006*     self = (Trubitsyn2006*)analyticSolution;
   ConditionFunction* condFunc;
   char*              viscosityType;
   FeVariable*        velocityField;
   FeVariable*        pressureField;
   double             Ra;
   double             T0;
   int                wavenumberX;
   int                wavenumberY;
   
   velocityField = Stg_ComponentFactory_PluginConstructByKey( cf, self, (Dictionary_Entry_Key)"VelocityField",
      FeVariable, True, data);

   pressureField = Stg_ComponentFactory_PluginConstructByKey( cf, self, (Dictionary_Entry_Key)"PressureField",
      FeVariable, True, data);

   /* Add temperature initial condition */
   condFunc = ConditionFunction_New(
      Trubitsyn2006_TemperatureIC,
      (Name)"Trubitsyn2006_TemperatureIC", NULL );
   ConditionFunction_Register_Add( condFunc_Register, condFunc );

   /* Add pressure initial condition */
   condFunc = ConditionFunction_New( Trubitsyn2006_PressureIC, (Name)"Trubitsyn2006_PressureIC", NULL );
   ConditionFunction_Register_Add( condFunc_Register, condFunc );
   
   Ra = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"Ra", 0.0 );
   T0 = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"T0", 0.0 );
   wavenumberX = Stg_ComponentFactory_GetRootDictInt( cf, "wavenumberX", 1 );
   wavenumberY = Stg_ComponentFactory_GetRootDictInt( cf, "wavenumberY", 1 );

   /* Setup Viscosity Functions */
   viscosityType = Stg_ComponentFactory_GetRootDictString( cf, (Dictionary_Entry_Key)"ViscosityType",
      "Isoviscous" );

   _Trubitsyn2006_Init(
      self,
      velocityField,
      pressureField,
      Ra,
      T0,
      wavenumberX,
      wavenumberY,
      viscosityType );
}

void _Trubitsyn2006_Build( void* analyticSolution, void* data ) {
   Trubitsyn2006* self = (Trubitsyn2006*)analyticSolution;

   _Codelet_Build( self, data );

   Stg_Component_Build(self->velocityField, data, False);
   Stg_Component_Build(self->pressureField, data, False);

   /* Add this plugin's analytic functions into the register. */
   AnalyticFunction_Add( _Trubitsyn2006_VelocityFunction, (Name)"_Trubitsyn2006_VelocityFunction" );     
   AnalyticFunction_Add( _Trubitsyn2006_PressureFunction, (Name)"_Trubitsyn2006_PressureFunction" );     
   AnalyticFunction_Add( _Trubitsyn2006_ViscosityFunction, (Name)"_Trubitsyn2006_ViscosityFunction" );     
}

void _Trubitsyn2006_Initialise( void* analyticSolution, void* data ) {
   Trubitsyn2006* self = (Trubitsyn2006*)analyticSolution;

   Stg_Component_Initialise(self->velocityField, data, False);
   Stg_Component_Initialise(self->pressureField, data, False);

   _Codelet_Initialise( self, data );
}

void _Trubitsyn2006_Destroy( void* analyticSolution, void* data ) {
   Trubitsyn2006* self = (Trubitsyn2006*)analyticSolution;

   _Codelet_Destroy( self, data );
   
   Stg_Component_Destroy(self->velocityField, data, False);
   Stg_Component_Destroy(self->pressureField, data, False);
}

void* _Trubitsyn2006_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                             _sizeOfSelf = sizeof(Trubitsyn2006);
   Type                                                     type = Trubitsyn2006_Type;
   Stg_Class_DeleteFunction*                             _delete = _Codelet_Delete;
   Stg_Class_PrintFunction*                               _print = _Codelet_Print;
   Stg_Class_CopyFunction*                                 _copy = _Codelet_Copy;
   Stg_Component_DefaultConstructorFunction* _defaultConstructor = _Trubitsyn2006_DefaultNew;
   Stg_Component_ConstructFunction*                   _construct = _Trubitsyn2006_AssignFromXML;
   Stg_Component_BuildFunction*                           _build = _Trubitsyn2006_Build;
   Stg_Component_InitialiseFunction*                 _initialise = _Trubitsyn2006_Initialise;
   Stg_Component_ExecuteFunction*                       _execute = _Codelet_Execute;
   Stg_Component_DestroyFunction*                       _destroy = _Trubitsyn2006_Destroy;

   /*
    * Variables that are set to ZERO are variables that will be set either by th
    * e current _New function or another parent _New function further up the hierachy.
    */

   /* default value NON_GLOBAL */
   AllocationType nameAllocationType = NON_GLOBAL;

   return _Codelet_New( CODELET_PASSARGS );
}

Index Underworld_Trubitsyn2006_Register( PluginsManager* pluginsManager ) {
   return PluginsManager_Submit( pluginsManager, Trubitsyn2006_Type, (Name)"0", _Trubitsyn2006_DefaultNew );
}


