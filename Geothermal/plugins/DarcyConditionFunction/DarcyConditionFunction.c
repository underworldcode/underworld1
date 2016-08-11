/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**	Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**	Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**	AuScope - http://www.auscope.org
**	Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**	Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**	Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**	Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**	Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
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

#include <string.h>
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <assert.h>
#include "DarcyConditionFunction.h"

const Type Geothermal_DarcyConditionFunction_Type = (const Type)"Geothermal_DarcyConditionFunction";

void _Geothermal_DarcyConditionFunction_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
	Geothermal_DarcyConditionFunction* self = (Geothermal_DarcyConditionFunction*)component;
	AbstractContext*        context;
	Dictionary*		pluginDict	= Codelet_GetPluginDictionary( component, cf->rootDict );

	context = (AbstractContext*)Stg_ComponentFactory_ConstructByName( cf, Dictionary_GetString( pluginDict, (Dictionary_Entry_Key)"Context"  ), AbstractContext, True, data );
	self->context = context;
	
	ConditionFunction_Register_Add( condFunc_Register, ConditionFunction_New( Geothermal_SourceSinkConditionFunction_getVal, (Name)"SourceSinkConditionFunction", NULL  ) );
	ConditionFunction_Register_Add( condFunc_Register, ConditionFunction_New( Geothermal_VelXConditionFunction_getVal, (Name)"VelXConditionFunction", NULL        ) );
	ConditionFunction_Register_Add( condFunc_Register, ConditionFunction_New( Geothermal_VelYConditionFunction_getVal, (Name)"VelYConditionFunction", NULL        ) );
	ConditionFunction_Register_Add( condFunc_Register, ConditionFunction_New( Geothermal_PressureBoundaryConditionFunction_getVal, (Name)"PressureBoundaryConditionFunction", NULL ) );

}

void _Geothermal_DarcyConditionFunction_Build( void* component, void* data ) {
}

void _Geothermal_DarcyConditionFunction_Destroy( void* component, void* data ) {
	Geothermal_DarcyConditionFunction* self = (Geothermal_DarcyConditionFunction*)component;
   /** our work is done, so we clean up after ourselves */	
   _Codelet_Destroy( component, data);
}

void* _Geothermal_DarcyConditionFunction_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Geothermal_DarcyConditionFunction);
	Type                                                      type = Geothermal_DarcyConditionFunction_Type;
	Stg_Class_DeleteFunction*                              _delete = _Codelet_Delete;
	Stg_Class_PrintFunction*                                _print = _Codelet_Print;
	Stg_Class_CopyFunction*                                  _copy = _Codelet_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Geothermal_DarcyConditionFunction_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Geothermal_DarcyConditionFunction_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Geothermal_DarcyConditionFunction_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Codelet_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Codelet_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Geothermal_DarcyConditionFunction_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return _Codelet_New(  CODELET_PASSARGS  );
	
}

Index Geothermal_DarcyConditionFunction_Register( PluginsManager* pluginsManager ) {

	return PluginsManager_Submit( pluginsManager, Geothermal_DarcyConditionFunction_Type, (Name)"0", _Geothermal_DarcyConditionFunction_DefaultNew  );
}

void Geothermal_DarcyConditionFunction_getVal( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ){
   DomainContext*  context    = (DomainContext*)_context;
	double*         result     = (double*) _result;
   FeVariable*     feVariable = NULL;
   double          k          = 2.*M_PI;
   double*         coord;
	
   feVariable = (FeVariable*)FieldVariable_Register_GetByName( context->fieldVariable_Register, "sourceSinkField" );

   coord = Mesh_GetVertex( feVariable->feMesh, node_lI );
   
   *result =  -4.*k*k*cos(k*coord[0])*cos(k*coord[1]); 

}


void Geothermal_VelXConditionFunction_getVal( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ){
   DomainContext*  context    = (DomainContext*)_context;
	double*         result     = (double*) _result;
   FeVariable*     feVariable = NULL;
   double          k          = 2.*M_PI;
   double*         coord;
	
   feVariable = (FeVariable*)FieldVariable_Register_GetByName( context->fieldVariable_Register, "VelocityField" );

   coord = Mesh_GetVertex( feVariable->feMesh, node_lI );
   
   *result =  k*sin(k*coord[0])*cos(k*coord[1]); 

}


void Geothermal_VelYConditionFunction_getVal( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ){
   DomainContext*  context    = (DomainContext*)_context;
	double*         result     = (double*) _result;
   FeVariable*     feVariable = NULL;
   double          k          = 2.*M_PI;
   double*         coord;
	
   feVariable = (FeVariable*)FieldVariable_Register_GetByName( context->fieldVariable_Register, "VelocityField" );

   coord = Mesh_GetVertex( feVariable->feMesh, node_lI );
   
   *result =  k*cos(k*coord[0])*sin(k*coord[1]); 

}


void Geothermal_PressureBoundaryConditionFunction_getVal( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ){
   DomainContext*  context    = (DomainContext*)_context;
	double*         result     = (double*) _result;
   FeVariable*     feVariable = NULL;
   double          k          = 2.*M_PI;
   double*         coord;
	
	feVariable = (FeVariable*)FieldVariable_Register_GetByName( context->fieldVariable_Register, "PressureField" );
     
   coord = Mesh_GetVertex( feVariable->feMesh, node_lI );
   
   *result = cos(k*coord[0])*cos(k*coord[1]); 

}


