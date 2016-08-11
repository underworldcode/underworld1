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

#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>

const Type CosineHillRotate_Type = "CosineHillRotate";

typedef struct { 
   __Codelet
   double hillHeight;
   double hillDiameter;
   Coord  rotationCentre;
} CosineHillRotate;

void CosineHillRotate_TemperatureFunction( void* _context, double* coord, double* temperature ) {
   double                distanceFromCentre;
   FiniteElementContext* context = (FiniteElementContext*)_context;
   CosineHillRotate*     self = (CosineHillRotate*)LiveComponentRegister_Get(
      context->CF->LCRegister,
      (Name)CosineHillRotate_Type );

   distanceFromCentre = StGermain_DistanceBetweenPoints( self->rotationCentre, coord, 2 );
   
   if (distanceFromCentre < self->hillDiameter ) 
      *temperature = self->hillHeight * (0.5 + 0.5 * cos( 2.0 * M_PI/self->hillDiameter * distanceFromCentre + M_PI ) );
   else
      *temperature = 0.0;
}

void CosineHillRotate_TemperatureBC( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* _data, void* _result ) {
   DomainContext*    context = (DomainContext*)_context;
   CosineHillRotate* self = NULL;
   FeVariable*       feVariable = NULL;
   FeMesh*           mesh = NULL;
   double*           result = (double*) _result;
   double*           coord;

   self = Stg_ComponentFactory_ConstructByName(
      context->CF,
      (Name)CosineHillRotate_Type,
      CosineHillRotate,
      True,
      0 ); 

   feVariable = (FeVariable* )FieldVariable_Register_GetByName(
      context->fieldVariable_Register,
      "TemperatureField" );

   mesh = feVariable->feMesh;
   coord = Mesh_GetVertex( mesh, node_lI );

   CosineHillRotate_TemperatureFunction( self, coord, result );
}

void _CosineHillRotate_Init(
   CosineHillRotate* self,
   double            hillHeight,
   double            hillDiameter,
   double            i_axis,
   double            j_axis,
   double            k_axis )
{
   self->hillHeight = hillHeight;
   self->hillDiameter = hillDiameter;
   self->rotationCentre[ I_AXIS ] = i_axis;
   self->rotationCentre[ J_AXIS ] = j_axis;
   self->rotationCentre[ K_AXIS ] = k_axis;
}   

void _CosineHillRotate_AssignFromXML( void* analyticSolution, Stg_ComponentFactory* cf, void* data ) {
   CosineHillRotate*  self = (CosineHillRotate*)analyticSolution;
   ConditionFunction* condFunc;
   double             hillHeight;
   double             hillDiameter;
   double             i_axis;
   double             j_axis;
   double             k_axis;

   /* Read values from dictionary */
   hillHeight = Stg_ComponentFactory_GetRootDictDouble(
      cf,
      (Dictionary_Entry_Key)"CosineHillHeight",
      1.0 );

   hillDiameter = Stg_ComponentFactory_GetRootDictDouble(
      cf,
      (Dictionary_Entry_Key)"CosineHillDiameter",
      1.0 );

   i_axis = Stg_ComponentFactory_GetRootDictDouble(
      cf,
      (Dictionary_Entry_Key)"SolidBodyRotationCentreX",
      0.0 );

   j_axis = Stg_ComponentFactory_GetRootDictDouble(
      cf,
      (Dictionary_Entry_Key)"SolidBodyRotationCentreY",
      0.0 );

   k_axis = Stg_ComponentFactory_GetRootDictDouble(
      cf,
      (Dictionary_Entry_Key)"SolidBodyRotationCentreZ",
      0.0 );

   /* Create Condition Functions */
   condFunc = ConditionFunction_New( CosineHillRotate_TemperatureBC, (Name)"Temperature_CosineHill", NULL );
   ConditionFunction_Register_Add( condFunc_Register, condFunc );

   _CosineHillRotate_Init(
      self,
      hillHeight,
      hillDiameter,
      i_axis,
      j_axis,
      k_axis );
}

void _CosineHillRotate_Build( void* analyticSolution, void* data ) {
   CosineHillRotate* self = (CosineHillRotate*)analyticSolution;

   _Codelet_Build( self, data );

   /* Add this plugin's analytic functions into the register. */
   AnalyticFunction_Add( CosineHillRotate_TemperatureFunction, (Name)"CosineHillRotate_TemperatureFunction" );
}

void* _CosineHillRotate_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                             _sizeOfSelf = sizeof(CosineHillRotate);
   Type                                                     type = CosineHillRotate_Type;
   Stg_Class_DeleteFunction*                             _delete = _Codelet_Delete;
   Stg_Class_PrintFunction*                               _print = _Codelet_Print;
   Stg_Class_CopyFunction*                                 _copy = _Codelet_Copy;
   Stg_Component_DefaultConstructorFunction* _defaultConstructor = _CosineHillRotate_DefaultNew;
   Stg_Component_ConstructFunction*                   _construct = _CosineHillRotate_AssignFromXML;
   Stg_Component_BuildFunction*                           _build = _CosineHillRotate_Build;
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
Index StgFEM_CosineHillRotate_Register( PluginsManager* pluginsManager ) {
   /* 
    * A plugin is only properly registered once it returns the handle provided
    * when submitting a codelet to StGermain. 
    */
   return PluginsManager_Submit( pluginsManager, CosineHillRotate_Type, (Name)"0", _CosineHillRotate_DefaultNew );
}
