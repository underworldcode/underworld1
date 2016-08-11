/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**   Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**   Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**   AuScope - http://www.auscope.org
**   Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**   Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**   Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**   Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**   Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
**
**  This library is free software; you can redistribute it and/or
**  modify it under the terms of the GNU Lesser General Public
**  License as published by the Free Software Foundation; either
**  version 2.1 of the License, or (at your option) any later version.
**
**  This library is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public
**  License along with this library; if not, write to the Free Software
**  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/** Pretty simple.
 * One time step with the diffusion coefficiants K = 1
 * TempBCs Bottom BC = 1, top BC = 0, sides = 0
 */
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>

const Type LinearTemperatureField_Type = "LinearTemperatureField";

typedef struct { __Codelet FeVariable* temperatureField; } LinearTemperatureField;

void LinearTemperatureField_TemperatureFunction( void* _context, double* coord, double* value ) {
   *value = 1.0 - coord[ J_AXIS ];
}

void _LinearTemperatureField_AssignFromXML( void* analyticSolution, Stg_ComponentFactory* cf, void* data ) {
   LinearTemperatureField *self = (LinearTemperatureField*)analyticSolution;

   self->temperatureField = Stg_ComponentFactory_ConstructByName(
      cf,
      (Name)"TemperatureField",
      FeVariable,
      True,
      data ); 
}

void _LinearTemperatureField_Build( void* analyticSolution, void* data ) {
   LinearTemperatureField* self = (LinearTemperatureField*)analyticSolution;

   _Codelet_Build( self, data );

   /* Add this plugin's analytic functions into the register. */
   AnalyticFunction_Add( LinearTemperatureField_TemperatureFunction, (Name)"LinearTemperatureField_TemperatureFunction" );
}

void* _LinearTemperatureField_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                             _sizeOfSelf = sizeof(LinearTemperatureField);
   Type                                                     type = LinearTemperatureField_Type;
   Stg_Class_DeleteFunction*                             _delete = _Codelet_Delete;
   Stg_Class_PrintFunction*                               _print = _Codelet_Print;
   Stg_Class_CopyFunction*                                 _copy = _Codelet_Copy;
   Stg_Component_DefaultConstructorFunction* _defaultConstructor = _LinearTemperatureField_DefaultNew;
   Stg_Component_ConstructFunction*                   _construct = _LinearTemperatureField_AssignFromXML;
   Stg_Component_BuildFunction*                           _build = _LinearTemperatureField_Build;
   Stg_Component_InitialiseFunction*                 _initialise = _Codelet_Initialise;
   Stg_Component_ExecuteFunction*                       _execute = _Codelet_Execute;
   Stg_Component_DestroyFunction*                       _destroy = _Codelet_Destroy;

   /*
    * Variables that are set to ZERO are variables that will be set either by the 
    * current _New function or another parent _New function further up the hierachy.
    */

   /* default value NON_GLOBAL */
   AllocationType nameAllocationType = NON_GLOBAL;

   return (void*) _Codelet_New( CODELET_PASSARGS );
}

/* 
 * This function is automatically run by StGermain when this plugin is loaded. 
 * The name must be "<plugin-name>_Register". 
 */
Index StgFEM_LinearTemperatureField_Register( PluginsManager* pluginsManager ) {
   /*
    * A plugin is only properly registered once it returns the handle provided
    * when submitting a codelet to StGermain. 
    */
   return PluginsManager_Submit( pluginsManager, LinearTemperatureField_Type, (Name)"0", _LinearTemperatureField_DefaultNew );
}
