#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>

#include "types.h"
#include "MineralPhase.h"


/* Textual name of this class */
const Type MineralPhase_Type = "MineralPhase";


/* Private Constructor */
MineralPhase* _MineralPhase_New(  MINERALPHASE_DEFARGS  )
{
	MineralPhase* self;
	
	assert( _sizeOfSelf >= sizeof(MineralPhase) );
	nameAllocationType = NON_GLOBAL;

	self = (MineralPhase*) _Stg_Component_New(  STG_COMPONENT_PASSARGS  );
	
	return self;
}


void* _MineralPhase_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(MineralPhase);
	Type                                                      type = MineralPhase_Type;
	Stg_Class_DeleteFunction*                              _delete = _MineralPhase_Delete;
	Stg_Class_PrintFunction*                                _print = _MineralPhase_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _MineralPhase_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _MineralPhase_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _MineralPhase_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _MineralPhase_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _MineralPhase_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _MineralPhase_Destroy;

	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;


	return (void*)_MineralPhase_New(  MINERALPHASE_PASSARGS  );
}

void _MineralPhase_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	MineralPhase*	self = (MineralPhase*)_self;
	Dictionary*		dict;

	self->context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", FiniteElementContext, False, data );
	if( !self->context  )
	  self->context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", FiniteElementContext, True, data  );

	dict = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );

	self->referenceTemperature = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ReferenceTemperature", 273  );
	self->referencePressure    = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ReferencePressure", 1.0 );
	self->clapeyronSlope       = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ClapeyronSlope", 0.0 );
	self->clapeyronSlope2      = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ClapeyronSlope2", 0.0  );
	self->densityIncrement     = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"DensityIncrement", 0.0 );
	self->viscosityContrast    = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ViscosityContrast", 1.0 );

	self->teA = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ThermalExpansionParamA", 1E-5  );
	self->teB = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ThermalExpansionParamB", 0.0  );
	self->teC = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ThermalExpansionParamC", 0.0  );
	self->teD = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ThermalExpansionParamD", 0.0  );
}


void _MineralPhase_Initialise( void* _self, void* data ) {}

void _MineralPhase_Build( void* _self, void* data ) {
	MineralPhase* self = (MineralPhase*)_self;
	
	_MineralPhase_Init( self );
}

void _MineralPhase_Delete( void* _self ) {}

void _MineralPhase_Print( void* _self, Stream* stream ) {}

void _MineralPhase_Execute( void* _self, void* data ) {}

void _MineralPhase_Destroy( void* _self, void* data ) {}

void _MineralPhase_Init( void* _self ) {

        MineralPhase*        self = (MineralPhase*) _self;
	self->_B  = self->referencePressure - self->referenceTemperature * self->clapeyronSlope;
	self->_B2 = self->referencePressure - self->referenceTemperature * self->clapeyronSlope2;
}

MineralPhase* MineralPhase_New( 
		Name                                                name,
		Stg_Component*                                      _self ) {

        MineralPhase*        self = (MineralPhase*) _self;

	self->referenceTemperature = 273;
	self->referencePressure    = 1E5;
	self->clapeyronSlope       = 0.0;
	self->clapeyronSlope2      = 0.0;
	self->densityIncrement     = 0.0;
	self->viscosityContrast    = 1.0;

	self->teA = 1E-5;
	self->teB = 0.0;
	self->teC = 0.0;
	self->teD = 0.0;

	_MineralPhase_Init( _self );
	return self;
}


void MineralPhase_SetProperties( void* _self, 
				 double refT, 
				 double refP, 
				 double slope1, 
				 double slope2, 
				 double rhoInc, 
				 double viscContrast ) {

	MineralPhase*                 self = (MineralPhase*)_self;

	self->referenceTemperature = refT;
	self->referencePressure    = refP;
	self->clapeyronSlope       = slope1;
	self->clapeyronSlope2      = slope2;
	self->densityIncrement     = rhoInc;
	self->viscosityContrast    = viscContrast;

	_MineralPhase_Init( _self );
}


Bool MineralPhase_AboveClapeyronCurve( void*   _self,
					      double  temperature,
					      double  pressure ) {

	MineralPhase*                 self = (MineralPhase*)_self;

	if( self->clapeyronSlope2 == 0 ||  temperature < self->referenceTemperature ) 
	  return (pressure > temperature * self->clapeyronSlope + self->_B);
	else 
	  return (pressure > temperature * self->clapeyronSlope2 + self->_B2);
}


double MineralPhase_ThermalExpansivity( void*   _self,
					       double  temperature) {

	MineralPhase*                 self = (MineralPhase*)_self;

	return( self->teA + 
	        self->teB * temperature + 
	        self->teC / pow( temperature, 2 ) +
	        self->teD * pow( temperature, 4 ) );
}
