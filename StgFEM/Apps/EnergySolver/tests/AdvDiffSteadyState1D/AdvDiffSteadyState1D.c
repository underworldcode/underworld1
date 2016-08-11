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

/* This analytic solutions is just the advection of a diffusing temperature for one time step.
 *  The advection is in the direction of one of the i,j,k axis 
 *  */

#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <string.h>

const Type AdvDiffSteadyState1D_Type = "AdvDiffSteadyState1D";

typedef struct { 
	__AnalyticSolution
	AdvDiffResidualForceTerm* residual;
	/* Velocity in this analyticSolution is constant */
	double                    velocity;
	Axis                      velocityDirection;
	double                    A;
	double                    B;
	double                    c;
	FeVariable*		  temperatureField;
} AdvDiffSteadyState1D;

void AdvDiffSteadyState1D_TemperatureFunction( void* analyticSolution, FeVariable* analyticFeVariable, double* coord, double* temperature ) {
	AdvDiffSteadyState1D* self = (AdvDiffSteadyState1D*)analyticSolution;
	double                exponent;
	double                kappa = self->residual->defaultDiffusivity;

	exponent = self->velocity / kappa * ( coord[ self->velocityDirection ] - self->c );
	*temperature = self->A * exp( exponent ) + self->B;
}

void AdvDiffSteadyState1D_TemperatureBC( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* temperature ){
	DomainContext*	context    = (DomainContext*)_context;
	AdvDiffSteadyState1D*   self       = Stg_ComponentFactory_ConstructByName( context->CF, (Name)AdvDiffSteadyState1D_Type, AdvDiffSteadyState1D, True, 0 /* dummy */ );
	FeVariable*             feVariable = NULL;
	FeMesh*     mesh       = NULL;
	double*                 coord;
	
	feVariable = (FeVariable* )FieldVariable_Register_GetByName( context->fieldVariable_Register, "TemperatureField" );
	mesh       = feVariable->feMesh;
	coord      = Mesh_GetVertex( mesh, node_lI );

	AdvDiffSteadyState1D_TemperatureFunction( self, NULL, coord, temperature );
}


void _AdvDiffSteadyState1D_Build( void* analyticSolution, void* data ) {
	AdvDiffSteadyState1D* self = (AdvDiffSteadyState1D*)analyticSolution;
	FeVariable*           velocityField = Stg_CheckType( self->residual->velocityField, FeVariable );
	CompositeVC*          velocityICs   = Stg_CheckType( velocityField->ics, CompositeVC );
	Stream*               errorStream   = Journal_MyStream( Error_Type, self );
	AllNodesVC*           allNodesVC;
	AllNodesVC_Entry*     vcEntry;

	_AnalyticSolution_Build( self, data );

	/* Get AllNodes Variable Condition */
	Stg_Component_Build( velocityICs, data, False );
	Journal_Firewall( velocityICs->itemCount == 1, errorStream, 
			"Velocity Field needs to have one and only one Boundary Condition.\n"
			"Currently it has %d types of VariableConditions.\n", velocityICs->itemCount );
	allNodesVC    = Stg_CheckType( velocityICs->itemTbl[ 0 ], AllNodesVC );

	/* Get Variable Condition entry */
	Journal_Firewall( allNodesVC->_entryCount == 1, errorStream, 
			"Velocity Field has more than one Boundary Condition.\n"
			"Currently it has %d VariableCondition entries.\n", allNodesVC->_entryCount );
	vcEntry       = &allNodesVC->_entryTbl[0];

	/* Get Velocity Direction from Variable Condition */
	if ( strcmp( vcEntry->varName, "vx" ) == 0 ) {
		self->velocityDirection = I_AXIS;
	}
	else if ( strcmp( vcEntry->varName, "vy" ) == 0 ) {
		self->velocityDirection = J_AXIS;
	}
	else if ( strcmp( vcEntry->varName, "vz" ) == 0 ) {
		self->velocityDirection = K_AXIS;
	}
	else {
		Journal_Firewall( False, errorStream, "Cannot recognise Boundary Condition: %s.\n", vcEntry->varName );
	}
}

void _AdvDiffSteadyState1D_AssignFromXML( void* analyticSolution, Stg_ComponentFactory* cf, void* data ) {
	AdvDiffSteadyState1D*  self = (AdvDiffSteadyState1D*)analyticSolution;
	AbstractContext*       context;
	ConditionFunction*     condFunc;

	_AnalyticSolution_AssignFromXML( self, cf, data );

	self->temperatureField = Stg_ComponentFactory_ConstructByName( cf, (Name)"TemperatureField", FeVariable, True, data  );
	AnalyticSolution_RegisterFeVariableWithAnalyticFunction( self, self->temperatureField, AdvDiffSteadyState1D_TemperatureFunction );

	self->residual = Stg_ComponentFactory_ConstructByName( cf, (Name)"defaultResidualForceTerm", AdvDiffResidualForceTerm, True, data  );

	self->velocity = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"velocity", 1.0  );
	self->A        = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"A", 1.0  );
	self->B        = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"B", 0.0  );
	self->c        = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"c", 0.0  );
	
	context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", AbstractContext, True, data  );
	condFunc = ConditionFunction_New( AdvDiffSteadyState1D_TemperatureBC, (Name)"AnalyticSolutionFunction", NULL  );
	ConditionFunction_Register_Add( context->condFunc_Register, condFunc );

}

void* _AdvDiffSteadyState1D_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(AdvDiffSteadyState1D);
	Type                                                      type = AdvDiffSteadyState1D_Type;
	Stg_Class_DeleteFunction*                              _delete = _AnalyticSolution_Delete;
	Stg_Class_PrintFunction*                                _print = _AnalyticSolution_Print;
	Stg_Class_CopyFunction*                                  _copy = _AnalyticSolution_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _AdvDiffSteadyState1D_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _AdvDiffSteadyState1D_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _AdvDiffSteadyState1D_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _AnalyticSolution_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _AnalyticSolution_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _AnalyticSolution_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*) _AnalyticSolution_New(  ANALYTICSOLUTION_PASSARGS  );
}

/* This function is automatically run by StGermain when this plugin is loaded. The name must be "<plugin-name>_Register". */
Index StgFEM_AdvDiffSteadyState1D_Register( PluginsManager* pluginsManager ) {
	/* A plugin is only properly registered once it returns the handle provided when submitting a codelet to StGermain. */
	return PluginsManager_Submit( pluginsManager, AdvDiffSteadyState1D_Type, (Name)"0", _AdvDiffSteadyState1D_DefaultNew  );
}



