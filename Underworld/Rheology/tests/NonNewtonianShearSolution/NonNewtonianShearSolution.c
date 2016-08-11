
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Rheology/Rheology.h>

#include <assert.h>

typedef struct {
	__AnalyticSolution
	MaterialViscosity*               materialViscosity;
	Mesh*				 mesh;
	NonNewtonian*                    nonNewtonianRheology;
	double                           velocityTopOfBox;
	FeVariable*			 velocityField;
	FeVariable*			 strainRateField;
	FeVariable*			 stressField;
	FeVariable*			 viscosityField;
} NonNewtonianShearSolution;

const Type NonNewtonianShearSolution_Type = "NonNewtonianShearSolution";

void NonNewtonianShearSolution_VelocityFunction( void* analyticSolution, FeVariable* analyticFeVariable, double* coord, double* velocity ) {
	NonNewtonianShearSolution*   self = (NonNewtonianShearSolution*)analyticSolution;
	double                       height;
	double			     min[3], max[3];
	
	/* Get Parameters */
	Mesh_GetGlobalCoordRange( self->mesh, min, max );
	height = max[J_AXIS] - min[J_AXIS];

	velocity[ I_AXIS ] = coord[J_AXIS] / height * self->velocityTopOfBox;
	velocity[ J_AXIS ] = 0.0;
}

void NonNewtonianShearSolution_StrainRateFunction( void* analyticSolution, FeVariable* analyticFeVariable, double* coord, double* strainRate ) {
	NonNewtonianShearSolution*   self = (NonNewtonianShearSolution*)analyticSolution;
	double                       height;
	double			     min[3], max[3];
	
	/* Get Parameters */
	Mesh_GetGlobalCoordRange( self->mesh, min, max );
	height = max[J_AXIS] - min[J_AXIS];

	strainRate[ 0 ] = 0.0;
	strainRate[ 1 ] = 0.0;
	strainRate[ 2 ] = 0.5 * self->velocityTopOfBox / height;
}

void NonNewtonianShearSolution_StressFunction( void* analyticSolution, FeVariable* analyticFeVariable, double* coord, double* stress ) {
	NonNewtonianShearSolution*   self = (NonNewtonianShearSolution*)analyticSolution;
	double                         eta;
	double                         height;
	double                         tau;
	double                         n;
	double			     min[3], max[3];
	
	/* Get Parameters */
	Mesh_GetGlobalCoordRange( self->mesh, min, max );
	eta = self->materialViscosity->eta0;
	n = self->nonNewtonianRheology->stressExponent;
	height = max[J_AXIS] - min[J_AXIS];

	/* Calculate stress - without considering cohesion */
	tau = pow( eta * self->velocityTopOfBox / height, 1/n);

	/* Calculate Analytic Solution for the stress */
	stress[0] = 0.0;
	stress[1] = 0.0;
	stress[2] = tau;
}


void NonNewtonianShearSolution_ViscosityFunction( void* analyticSolution, FeVariable* analyticFeVariable, double* coord, double* viscosity ) {
	NonNewtonianShearSolution*     self = (NonNewtonianShearSolution*)analyticSolution;
	double                         eta0;
	double                         height;
	double                         n;
	double			     min[3], max[3];
	
	/* Get Parameters */
	eta0 = self->materialViscosity->eta0;
	n = self->nonNewtonianRheology->stressExponent;
	Mesh_GetGlobalCoordRange( self->mesh, min, max );
	height = max[J_AXIS] - min[J_AXIS];

	/* Calculate stress - without considering cohesion */
	*viscosity = eta0 * pow( eta0 * self->velocityTopOfBox / height, 1/n-1.0);
}

void NonNewtonianShearSolution_UpdateVelocityBC( NonNewtonianShearSolution* self, FiniteElementContext* context ) {
	self->velocityTopOfBox += context->dt;
}

void NonNewtonianShearSolution_VelocityBC( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ) {
	FiniteElementContext *	         context       = (FiniteElementContext*)_context;
	NonNewtonianShearSolution*   self          = (NonNewtonianShearSolution*) LiveComponentRegister_Get( context->CF->LCRegister, (Name)NonNewtonianShearSolution_Type );
	double*                          result        = (double* ) _result;

	*result = self->velocityTopOfBox;
}

void _NonNewtonianShearSolution_AssignFromXML( void* analyticSolution, Stg_ComponentFactory* cf, void* data ) {
	NonNewtonianShearSolution* self = (NonNewtonianShearSolution*)analyticSolution;
	FiniteElementContext*          context;

	/* Construct parent */
	_AnalyticSolution_AssignFromXML( self, cf, data );
	context = Stg_CheckType( self->context, FiniteElementContext );
	
	ConditionFunction_Register_Add( context->condFunc_Register, 
			ConditionFunction_New( NonNewtonianShearSolution_VelocityBC, (Name)"ShearTrigger"), NULL );	

	/* Create Analytic Velocity Field */
	self->velocityField = Stg_ComponentFactory_ConstructByName( cf, (Name)"VelocityField", FeVariable, True, data  );
	AnalyticSolution_RegisterFeVariableWithAnalyticFunction( self, self->velocityField, NonNewtonianShearSolution_VelocityFunction );

	/* Create Analytic Strain Rate Field */
	self->strainRateField = Stg_ComponentFactory_ConstructByName( cf, (Name)"StrainRateField", FeVariable, True, data  );
	AnalyticSolution_RegisterFeVariableWithAnalyticFunction( self, self->strainRateField, NonNewtonianShearSolution_StrainRateFunction );

	/* Create Analytic Stress Field */
	self->stressField = Stg_ComponentFactory_ConstructByName( cf, (Name)"StressField", FeVariable, True, data  );
	AnalyticSolution_RegisterFeVariableWithAnalyticFunction( self, self->stressField, NonNewtonianShearSolution_StressFunction );

	/* Create Analytic Viscosity Field */
	self->viscosityField = Stg_ComponentFactory_ConstructByName( cf, (Name)"ViscosityField", FeVariable, True, data  );
	AnalyticSolution_RegisterFeVariableWithAnalyticFunction( self, self->viscosityField, NonNewtonianShearSolution_ViscosityFunction );

	self->materialViscosity = Stg_ComponentFactory_ConstructByName( cf, (Name)"layerViscosity", MaterialViscosity, True, data  );
	self->nonNewtonianRheology = 
		Stg_ComponentFactory_ConstructByName( cf, (Name)"nonNewtonianRheology", NonNewtonian, True, data  );
	self->mesh = Stg_ComponentFactory_ConstructByName( cf, (Name)"linearMesh", Mesh, True, data  );

	/* Set Velocity Stuff */
	EP_AppendClassHook( Context_GetEntryPoint( context, AbstractContext_EP_UpdateClass ),
			    NonNewtonianShearSolution_UpdateVelocityBC, self );
	self->velocityTopOfBox = Stg_ComponentFactory_GetRootDictDouble( cf, (Dictionary_Entry_Key)"velocityTopOfBox", 0.5  );
}

void _NonNewtonianShearSolution_Build( void* analyticSolution, void* data ) {
	NonNewtonianShearSolution* self = (NonNewtonianShearSolution*)analyticSolution;

	_AnalyticSolution_Build( self, data );
}


/* This function will provide StGermain the abilty to instantiate (create) this codelet on demand. */
void* _NonNewtonianShearSolution_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(NonNewtonianShearSolution);
	Type                                                      type = NonNewtonianShearSolution_Type;
	Stg_Class_DeleteFunction*                              _delete = _AnalyticSolution_Delete;
	Stg_Class_PrintFunction*                                _print = _AnalyticSolution_Print;
	Stg_Class_CopyFunction*                                  _copy = _AnalyticSolution_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _NonNewtonianShearSolution_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _NonNewtonianShearSolution_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _NonNewtonianShearSolution_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _AnalyticSolution_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _AnalyticSolution_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _AnalyticSolution_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return _AnalyticSolution_New(  ANALYTICSOLUTION_PASSARGS  );	
}
	
/* This function is automatically run by StGermain when this plugin is loaded. The name must be "<plugin-name>_Register". */
Index Underworld_NonNewtonianShearSolution_Register( PluginsManager* pluginsManager ) {
	/* A plugin is only properly registered once it returns the handle provided when submitting a codelet to StGermain. */
	return PluginsManager_Submit( pluginsManager, NonNewtonianShearSolution_Type, (Name)"0", _NonNewtonianShearSolution_DefaultNew  );
}



