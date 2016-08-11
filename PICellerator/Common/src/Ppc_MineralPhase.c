#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_MineralPhase.h"


/* Textual name of this class */
const Type Ppc_MineralPhase_Type = "Ppc_MineralPhase";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_MineralPhase* _Ppc_MineralPhase_New(  PPC_MINERALPHASE_DEFARGS  )
{
	Ppc_MineralPhase* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_MineralPhase) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_MineralPhase*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_MineralPhase_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_MineralPhase);
	Type                                                      type = Ppc_MineralPhase_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_MineralPhase_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_MineralPhase_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_MineralPhase_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_MineralPhase_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_MineralPhase_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_MineralPhase_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_MineralPhase_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_MineralPhase_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_MineralPhase_Get;

	return (void*)_Ppc_MineralPhase_New(  PPC_MINERALPHASE_PASSARGS  );
}


void _Ppc_MineralPhase_Init( void* _self, char* t, char* p )
{
  Ppc_MineralPhase* self = (Ppc_MineralPhase*)_self;

  self->temperatureName = t;
  self->pressureName = p;
}



void _Ppc_MineralPhase_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	Ppc_MineralPhase* self = (Ppc_MineralPhase*)_self;
	Dictionary*	theDictionary;
	Dictionary_Entry_Value*	phasesList;
	Dictionary_Entry_Value*	phaseEntry;
	Index phase_I;

	/* Construct parent */
	_Ppc_AssignFromXML( self, cf, data );

	/* The dictionary */ 
	theDictionary = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );

	/* Read phase info */
	phasesList = Dictionary_Get( theDictionary, (Dictionary_Entry_Key)"MineralPhases" );
	assert( phasesList );
	self->phasesCount = Dictionary_Entry_Value_GetCount( phasesList );

	self->phasesList = Memory_Alloc_Array( Ppc_MineralPhase_Phase, self->phasesCount, "Ppc_MineralPhase_phases" );
	
	for( phase_I = 0; phase_I < self->phasesCount; phase_I++ ){
	  phaseEntry = Dictionary_Entry_Value_GetElement( phasesList, phase_I );

	  self->phasesList[phase_I].name = Dictionary_Entry_Value_AsString( 
        Dictionary_Entry_Value_GetMember( phaseEntry, (Dictionary_Entry_Key)"Name") );

	  self->phasesList[phase_I].referenceTemperature = Dictionary_Entry_Value_AsDouble( 
        Dictionary_Entry_Value_GetMember( phaseEntry, (Dictionary_Entry_Key)"ReferenceTemperature") );

	  self->phasesList[phase_I].referencePressure = Dictionary_Entry_Value_AsDouble( 
        Dictionary_Entry_Value_GetMember( phaseEntry, (Dictionary_Entry_Key)"ReferencePressure") );

	  self->phasesList[phase_I].clapeyronSlope = Dictionary_Entry_Value_AsDouble( 
	     Dictionary_Entry_Value_GetMember( phaseEntry, (Dictionary_Entry_Key)"ClapeyronSlope") );

	  self->phasesList[phase_I].densityIncrement = Dictionary_Entry_Value_AsDouble( 
	     Dictionary_Entry_Value_GetMember( phaseEntry, (Dictionary_Entry_Key)"DensityIncrement") );

	  self->phasesList[phase_I].viscosityContrast = Dictionary_Entry_Value_AsDouble( 
	     Dictionary_Entry_Value_GetMember( phaseEntry, (Dictionary_Entry_Key)"ViscosityContrast") );
	}

	_Ppc_MineralPhase_Init( 
      self, 
		Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"TemperatureName", "default_temperature_ppc" ), 
		Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"PressureName", "default_pressure_ppc" ) );
}


void _Ppc_MineralPhase_Build( void* _self, void* data ) {
   Ppc_MineralPhase* self = (Ppc_MineralPhase*)_self;

	/* Build parent */
	_Ppc_Build( self, data );
}

void _Ppc_MineralPhase_Initialise( void* _self, void* data ) {
  Ppc_MineralPhase* self = (Ppc_MineralPhase*)_self;
  Ppc_MineralPhase_Phase* phase;
  int I;
  /* Initialize parent */
  _Ppc_Initialise( self, data );

  for( I = 0 ; I < self->phasesCount ; I ++ ) {
	 phase = &self->phasesList[I];
	 phase->B  = phase->referencePressure - phase->referenceTemperature * phase->clapeyronSlope;
	}
}

void _Ppc_MineralPhase_Delete( void* _self ) {
  Ppc_MineralPhase* self = (Ppc_MineralPhase*)_self;

  if( self->phasesList )
	 Memory_Free( self->phasesList );

  /* Delete parent */
  _Ppc_Delete( self );
}

void _Ppc_MineralPhase_Print( void* _self, Stream* stream ) {
   Ppc_MineralPhase* self = (Ppc_MineralPhase*)_self;

	/* Print parent */
	_Ppc_Print( self, stream );
}

void _Ppc_MineralPhase_Execute( void* _self, void* data ) {
   Ppc_MineralPhase* self = (Ppc_MineralPhase*)_self;

	/* Execute parent */
	_Ppc_Execute( self, data );
}

void _Ppc_MineralPhase_Destroy( void* _self, void* data ) {
   Ppc_MineralPhase* self = (Ppc_MineralPhase*)_self;

	/* Destroy parent */
	_Ppc_Destroy( self, data );
}


Bool _Ppc_MineralPhase_AboveClapeyronCurve( void* _self, double temperature, double pressure, Ppc_MineralPhase_Phase* phase ) {
  MineralPhase* self = (MineralPhase*)_self;

  return (pressure > temperature * phase->clapeyronSlope + phase->B);
}

int _Ppc_MineralPhase_GetPhase( void* _self, double temperature, double pressure ) {
  Ppc_MineralPhase* self = (Ppc_MineralPhase*)_self;
  int I, res = 0;

  for( I = 0 ; I < self->phasesCount ; I ++ ) 
	 if( _Ppc_MineralPhase_AboveClapeyronCurve( self, temperature, pressure, &self->phasesList[I] ) )
		res = I;
  
  return res;
}

/* 
 * Public functions 
 *
 */

int _Ppc_MineralPhase_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
  Ppc_MineralPhase* self = (Ppc_MineralPhase*) _self;
  double T, P;
  int err;

  /* get temperature */
  err = PpcManager_Get( self->manager, lElement_I, particle, self->temperatureName, &T );
  assert( !err );

  /* get pressure */
  err = PpcManager_Get( self->manager, lElement_I, particle, self->pressureName, &P );
  assert( !err );

  result[0] = (double) _Ppc_MineralPhase_GetPhase( self, T, P );

  return 0;
}
