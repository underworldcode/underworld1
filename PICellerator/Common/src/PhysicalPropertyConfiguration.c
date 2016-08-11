#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "MineralPhase.h"
#include "LookupTable.h"
#include "PhysicalPropertyConfiguration.h"

#define ASSUME_INTEGER_WATER_CONTENT

/* Textual name of this class */
const Type PhysicalPropertyConfiguration_Type = "PhysicalPropertyConfiguration";


/* My private functions */
double _PhysicalPropertyConfiguration_IntegralAlpha( void* _self, double dTemperature, double dPressure, MineralPhaseId phaseId ); 

double _PhysicalPropertyConfiguration_IntegralAlphaT1T2( double T1, double T2, double a, double b, double c, double d );


SetupPhysicalPropertyConfiguration  PhysicalPropertyConfiguration_Setup( char* str ) {
  
  if( strcmp( str, "constant" ) == 0 || strcmp( str, "Constant" ) == 0 )
    return useConstant;
  if( strcmp( str, "table" ) == 0 || strcmp( str, "Table" ) == 0 )
    return useTable;
  if( strcmp( str, "waterTables" ) == 0 || strcmp( str, "WaterTables" ) == 0 )
    return useWaterTables;
  if( strcmp( str, "deltaViscosity" ) == 0 || strcmp( str, "DeltaViscosity" ) == 0 )
    return useDeltaViscosity;
  if( strcmp( str, "linearDensity" ) == 0 || strcmp( str, "LinearDensity" ) == 0 )
    return useLinearDensity;
  if( strcmp( str, "linearDensityWithPhaseChanges" ) == 0 || strcmp( str, "LinearDensityWithPhaseChanges" ) == 0 )
    return useLinearDensityWithPhaseChanges;
  if( strcmp( str, "densityTPWF" ) == 0 || strcmp( str, "DensityTPWF" ) == 0 )
    return useDensityTPWF;

  return errorPPC;
}


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
PhysicalPropertyConfiguration* _PhysicalPropertyConfiguration_New(  PHYSICALPROPERTYCONFIGURATION_DEFARGS  )
{
	PhysicalPropertyConfiguration* self;
	
	assert( _sizeOfSelf >= sizeof(PhysicalPropertyConfiguration) );
	nameAllocationType = NON_GLOBAL;
	self = (PhysicalPropertyConfiguration*) _Stg_Component_New(  STG_COMPONENT_PASSARGS  );	
	return self;
}


void* _PhysicalPropertyConfiguration_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(PhysicalPropertyConfiguration);
	Type                                                      type = PhysicalPropertyConfiguration_Type;
	Stg_Class_DeleteFunction*                              _delete = _PhysicalPropertyConfiguration_Delete;
	Stg_Class_PrintFunction*                                _print = _PhysicalPropertyConfiguration_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _PhysicalPropertyConfiguration_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _PhysicalPropertyConfiguration_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _PhysicalPropertyConfiguration_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _PhysicalPropertyConfiguration_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _PhysicalPropertyConfiguration_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _PhysicalPropertyConfiguration_Destroy;

	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*)_PhysicalPropertyConfiguration_New(  PHYSICALPROPERTYCONFIGURATION_PASSARGS  );
}


void _PhysicalPropertyConfiguration_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	PhysicalPropertyConfiguration* self = (PhysicalPropertyConfiguration*)_self;
	Dictionary*	theDictionary;
	char*	strBuffer;
	Dictionary_Entry_Value*	phaseList;
	Dictionary_Entry_Value*	phaseEntry;
	Name phaseName;
	Index phase_I;

	theDictionary = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );
	self->context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", FiniteElementContext, False, data );
	if( !self->context  )
		self->context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", FiniteElementContext, True, data  );

	/* The component factory*/
	self->cf = cf;

	/* The scaling component */
	/*self->scaling = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Scaling", Scaling, True, data  );*/

	/* The quantity */
	/*strBuffer = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"Units", "" );
	  self->units = Scaling_Parse( strBuffer );*/

	/* The setup: Constant, Table, Tables, Function */
	strBuffer = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"Setup", "Constant" );
	self->setup = PhysicalPropertyConfiguration_Setup( strBuffer );
	
	/* Tables */
	self->phases = NULL;
	self->waterTables = NULL;
	self->waterIndex = NULL;

	switch( self->setup  ) {
	case useConstant:
	  self->value = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"Value", 1.0  );
	  break;

	case useTable:
	  self->tableName = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"TableName", "lookup"  );
	  break;

	case useWaterTables:
	  self->waterTableSpecs = Dictionary_Get( theDictionary, (Dictionary_Entry_Key)"tableList"  );
	  break;
	  
	case useDeltaViscosity:
	  phaseList = Dictionary_Get( theDictionary, (Dictionary_Entry_Key)"MineralPhases" );

	  assert( phaseList );
	  self->phasesCount = Dictionary_Entry_Value_GetCount( phaseList  );
	  self->phases = Memory_Alloc_Array( MineralPhase*, self->phasesCount, "PhysicalPropertyConfiguration_phases" );
	
	  for( phase_I = 0; phase_I < self->phasesCount; phase_I++ ){
	    phaseEntry = Dictionary_Entry_Value_GetElement( phaseList, phase_I );
	    phaseName = Dictionary_Entry_Value_AsString( phaseEntry );
	    self->phases[phase_I] = Stg_ComponentFactory_ConstructByName( cf, (Name)phaseName, MineralPhase, True, data  );
	  }
	  break;

	case useLinearDensity:
	case useLinearDensityWithPhaseChanges:
	  self->referenceDensity = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ReferenceDensity", 1.0  );
	  self->referencePressure = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ReferencePressure", 0.0  );
	  self->referenceTemperature = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ReferenceTemperature", 0.13  );
	  self->compressibility = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"Compressibility", 0.0  );
	  self->thermalExpansivity = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ThermalExpansivity", 0.0 );
	 
	  if( self->setup == useLinearDensityWithPhaseChanges  ) {
	    phaseList = Dictionary_Get( theDictionary, (Dictionary_Entry_Key)"MineralPhases" );
	    assert( phaseList );
	    self->phasesCount = Dictionary_Entry_Value_GetCount( phaseList  );
	    self->phases = Memory_Alloc_Array( MineralPhase*, self->phasesCount, "PhysicalPropertyConfiguration_phases" );
	    
	    for( phase_I = 0; phase_I < self->phasesCount; phase_I++ ){
	      phaseEntry = Dictionary_Entry_Value_GetElement( phaseList, phase_I );
	      phaseName = Dictionary_Entry_Value_AsString( phaseEntry );
	      self->phases[phase_I] = Stg_ComponentFactory_ConstructByName( cf, (Name)phaseName, MineralPhase, True, data  );
	    }	  
	  }
	  break;

	case errorPPC:
	  Journal_Firewall( 0, 
			    Journal_Register( Error_Type, (Name)PhysicalPropertyConfiguration_Type  ), 
			    "\tPhysicalPropertyConfiguration has unknown setup %s.\n", strBuffer );	  	  
	  break;
   case useDensityTPWF:
	  /* FIXME */
	  break;
   case useHofmeister:
	  /* FIXME */
	  break;
	}

	/* thermal conductivity stuff */
	/*self->cond.K298 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"K298[W m^-1 K^-1)]", 4.65  );
	self->cond.a = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"Parameter-a[1]", 0.33  );
	self->cond.gamma = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"GruneinenParameter[1]", 1.28  );
	self->cond.K0 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"BulkModulus[GPa]", 128.1  );
	self->cond.K0p = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"dBulkModulus_dp[1]", 4.5  );
	self->cond.rad1 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"Rad1[1]", 0.0  );
	self->cond.rad2 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"Rad2[1]", 0.0  );
	self->cond.rad3 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"Rad3[1]", 0.0  );
	self->cond.rad4 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"Rad4[1]", 8.5E-11  );*/

	/* partial melt stuff */
	/*self->partialMelt.p0 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"PartialMeltReferencePressure[GPa]", 1.25  ) * 1E9;
	self->partialMelt.A1 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"PartialMeltA1[1]", -16.19  );
	self->partialMelt.B1 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"PartialMeltB1[1]", 25.23  );
	self->partialMelt.A2 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"PartialMeltA2[1]", -4.0   );
	self->partialMelt.B2 = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"PartialMeltB2[1]", 10.0   );
	self->partialMelt.maxMelt = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"maxMelt[%]", 25  );*/
	
	/* water content stuff */
	/*self->waterContent.dryF = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"WaterDryF[%]", 3.0  );
	self->waterContent.A = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"WaterA[1]", -0.3333  );
	self->waterContent.B = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"WaterB[1]", 1.0  );
	self->waterContent.viscosityFactor = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"DryViscosityFactor[1]", 100.0  );*/

	/* general stuff */
	/*self->compressibility = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"CompressibilityCoefficient[MPa^-1]", 1E-5  );
	self->gruneisenParameter = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"AndersonGruneisen[1]", 5.5   );
	self->referenceDensity = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ReferenceDensity[kg m^-3]", 3400  );
	self->referencePressure = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ReferencePressure[MPa]", 0.1  );
	self->referenceTemperature = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ReferenceTemperature[K]", 273  );
	self->Cp = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"Cp[J kg^-1 K^-1]", 1000  );
	self->gravity = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"Gravity[m s^-2]", 9.81  );*/

}


void _PhysicalPropertyConfiguration_Build( void* _self, void* data ) {
   PhysicalPropertyConfiguration*	self = (PhysicalPropertyConfiguration*) _self;
   Index										tableSpec_I;
   Dictionary_Entry_Value*				tableSpec = NULL;
   Dictionary*								tableSpecDict;
   char*										tableName;

   switch( self->setup ){
   case useConstant:
     /*self->scaledValue = Scaling_Scale( self->scaling, self->value, self->quantity );*/
     break;

   case useTable:
     self->table = Stg_ComponentFactory_ConstructByName( self->cf, (Name)self->tableName, LookupTable, True, data );
     break;
 
   case useWaterTables:
     self->nWaterTables = Dictionary_Entry_Value_GetCount( self->waterTableSpecs  );
     self->waterTables = Memory_Alloc_Array( LookupTable*, self->nWaterTables, "listOfLookupTables" );
     self->waterIndex = Memory_Alloc_Array( double, self->nWaterTables, "listOfWaterContents" );

     for ( tableSpec_I = 0; tableSpec_I < self->nWaterTables; tableSpec_I++ ) {
       tableSpec = Dictionary_Entry_Value_GetElement( self->waterTableSpecs, tableSpec_I );
       tableSpecDict = Dictionary_Entry_Value_AsDictionary( tableSpec );       
       tableName = Dictionary_Entry_Value_AsString( Dictionary_Get( tableSpecDict, (Dictionary_Entry_Key)"TableName" )  );     
       self->waterTables[tableSpec_I] = Stg_ComponentFactory_ConstructByName( self->cf, (Name)tableName, LookupTable, True, data  );
       self->waterIndex[tableSpec_I] = Dictionary_Entry_Value_AsDouble( Dictionary_Get( tableSpecDict, (Dictionary_Entry_Key)"WaterContent" )  );
     }
     break;

   case useLinearDensity:
   case useLinearDensityWithPhaseChanges:
   case useDensityTPWF:
   case useHofmeister:
   case useDeltaViscosity:
   case errorPPC:
	  /* FIXME */
     /*self->scaledReferenceDensity     = Scaling_Scale( self->scaling, self->referenceDensity, sDensity );
     self->scaledReferencePressure    = Scaling_Scale( self->scaling, self->referencePressure, sPressure );
     self->scaledReferenceTemperature = Scaling_Scale( self->scaling, self->referenceTemperature, sTemperature );
     self->scaledCompressibility      = Scaling_Scale( self->scaling, self->compressibility, sCompressibility );
     self->scaledThermalExpansivity   = Scaling_Scale( self->scaling, self->thermalExpansivity, sThermalExpansivity );
	  */
     break;
   }
}


void _PhysicalPropertyConfiguration_Initialise( void* _self, void* data ) {
   PhysicalPropertyConfiguration* self;

	self = (PhysicalPropertyConfiguration*) _self;
}


void _PhysicalPropertyConfiguration_Delete( void* _self ) {

   PhysicalPropertyConfiguration*        self = (PhysicalPropertyConfiguration*) _self;
   
   if( self->phases )
   	 Memory_Free( self->phases );
   	 
   if( self->waterTables )
   	 Memory_Free( self->waterTables );
   	 
   if( self->waterIndex )
		 Memory_Free( self->waterIndex );
}

void _PhysicalPropertyConfiguration_Print( void* _self, Stream* stream ) {}

void _PhysicalPropertyConfiguration_Execute( void* _self, void* data ) {}

void _PhysicalPropertyConfiguration_Destroy( void* _self, void* data ) {}


/* 
 * Private functions 
 * 
 */


/*inline double _PhysicalPropertyConfiguration_ThermalExpansivity( void*            _self,
					 dimScalar        dTemperature,
					 dimScalar        dPressure, 
					 MineralPhaseId   phaseId ) {

	PhysicalPropertyConfiguration*                 self = (PhysicalPropertyConfiguration*)_self;
	double                        alpha;
	double                        a, b, c, d;
	double                        beta, gruneisen, p0, aux;

 	a = self->phases[phaseId]->teA;
 	b = self->phases[phaseId]->teB;
 	c = self->phases[phaseId]->teC;
	d = self->phases[phaseId]->teD;

	beta = self->compressibility;
	gruneisen = self->gruneisenParameter;
	p0 = self->referencePressure;

	alpha = a + 
                b * dTemperature + 
                c / pow( dTemperature, 2 ) + 
                d * pow( dTemperature, 4 );

        aux = 1 + beta*(dPressure/1E6 - p0);
        alpha *= pow( aux, -gruneisen/aux );

	return alpha;
}*/


/*inline double _PhysicalPropertyConfiguration_Density( void*            _self,
			      dimScalar        dTemperature,
			      dimScalar        dPressure, 
			      MineralPhaseId   phaseId  ) {

	PhysicalPropertyConfiguration*                 self = (PhysicalPropertyConfiguration*)_self;
	dimScalar                     dRefDens, dRefPres, dRefTemp, dAlpha, dBeta, dDeltaRho;
	Index                         phase_I;
	dimScalar                     dDensity;

	dRefDens = self->referenceDensity;
	dRefTemp = self->referenceTemperature;
	dRefTemp = self->referencePressure;

	dAlpha = _PhysicalPropertyConfiguration_ThermalExpansivity( _self, dTemperature, dPressure, phaseId );
	dBeta = self->compressibility;
	
	dDeltaRho = 0;
	for( phase_I = 0 ; phase_I <= phaseId ; phase_I++ )
	  dDeltaRho += self->phases[phase_I]->densityIncrement;

	dDensity = dDeltaRho + dRefDens * (1 - dAlpha*(dTemperature - dRefTemp)) * 
	                                  (1 + dBeta *(dPressure/1E6 - dRefPres));

	return dDensity;
}*/

/*inline double _PhysicalPropertyConfiguration_ThermalConductivity( void*            _self,
					  dimScalar        dTemperature,
					  dimScalar        dPressure, 
					  MineralPhaseId   phaseId ) {

	PhysicalPropertyConfiguration*                 self = (PhysicalPropertyConfiguration*)_self;
	double                        a, r1, r2, r3, r4, k0, k0p, k298, gamma;
	dimScalar                     dK;

	a     = self->cond.a;
	r1    = self->cond.rad1;
	r2    = self->cond.rad2;
	r3    = self->cond.rad3;
	r4    = self->cond.rad4;
	k0    = self->cond.K0;
	k0p   = self->cond.K0p;
	k298  = self->cond.K298;
	gamma = self->cond.gamma;

        dPressure /= 1E9;
	dK = k298 * pow( 298.0 / dTemperature, a );

	dK *= exp( -(4*gamma + 1/3) * _PhysicalPropertyConfiguration_IntegralAlpha( _self, dTemperature, dPressure, phaseId ) );

	dK *= (1 + k0p*dPressure/k0);

	dK += r1 + r2 * dTemperature + 
                  r3 * pow( dTemperature, 2 ) + 
                  r4 * pow( dTemperature, 3 );

	return dK;
}*/


/*
 * Computes the integral of the expansivity coefficient alpha 
 * between T1 and T2
 *
 * $int_T1^T2 \alpha(T) \, \textrm{d} T$
 *
 * $\alpha(T) = a + b*T + c*T^{-2} + d*T^4$
 *
 */
/*inline double _PhysicalPropertyConfiguration_IntegralAlphaT1T2( double T1, double T2, 
   double a, double b, double c, double d ) 
{ 
   return a * (T2 - T1) +
          b * (T2*T2 - T1*T1) * 0.5 +
          c * (1/T2 - 1/T1) +
          d * (pow( T2, 5 ) - pow( T1, 5 )) * 0.25;
}*/


/*
 * Computes the integral of the expansivity coefficient alpha
 * between 294 and T
 *
 * $\alpha(T) = a + b*T + c*T^{-2} + d*T^4$
 *
 */
/*inline double _PhysicalPropertyConfiguration_IntegralAlpha(  void*            _self,
				     dimScalar        dTemperature,
				     dimScalar        dPressure, 
				     MineralPhaseId   phaseId) {

	PhysicalPropertyConfiguration*                 self = (PhysicalPropertyConfiguration*)_self;
	double                        a, b, c, d;
	dimScalar                     Salpha, dT1, dT2;
	Index                         phase_I;

	Salpha = 0;

	a   = self->phases[phaseId]->teA;
	b   = self->phases[phaseId]->teB;
	c   = self->phases[phaseId]->teC;
	d   = self->phases[phaseId]->teD;
	dT1 = self->phases[phaseId]->referenceTemperature;
	dT2 = dTemperature;	  
	Salpha += _PhysicalPropertyConfiguration_IntegralAlphaT1T2( dT1, dT1, a, b, c, d  );

	for( phase_I = phaseId - 1; phase_I < 0; phase_I++ ){
	  a   = self->phases[phase_I]->teA;
	  b   = self->phases[phase_I]->teB;
	  c   = self->phases[phase_I]->teC;
	  d   = self->phases[phase_I]->teD;
	  dT1 = self->phases[phaseId]->referenceTemperature;
	  dT2 = self->phases[phaseId-1]->referenceTemperature;
	  Salpha += _PhysicalPropertyConfiguration_IntegralAlphaT1T2( dT1, dT1, a, b, c, d  );
	}

	if( phaseId > 0 ) {
	  a   = self->phases[phaseId]->teA;
	  b   = self->phases[phaseId]->teB;
	  c   = self->phases[phaseId]->teC;
	  d   = self->phases[phaseId]->teD;
	  dT1 = 298;
	  dT2 = self->phases[phaseId]->referenceTemperature;
	  Salpha += _PhysicalPropertyConfiguration_IntegralAlphaT1T2( dT1, dT1, a, b, c, d  );
	}

	return Salpha;
}*/



/*double PhysicalPropertyConfiguration_ThermalExpansivity( void*            _self,
					double           temperature,
					double           pressure ) {

	PhysicalPropertyConfiguration*                 self = (PhysicalPropertyConfiguration*)_self;
	dimScalar                     dTemperature, dPressure, dAlpha;
	dimlessScalar                 alpha;
	MineralPhaseId                phaseId;

	if( self->useConstantAlpha ) {
	  return self->scaledAlpha;
	}

	dTemperature = Scaling_Unscale( self->scaling, temperature, sTemperature );
	dPressure = Scaling_Unscale( self->scaling, pressure, sPressure );

	phaseId = _PhysicalPropertyConfiguration_ActualMineralPhase( self, dTemperature, dPressure );

        dAlpha = _PhysicalPropertyConfiguration_ThermalExpansivity( _self, dTemperature, dPressure, phaseId );

 	alpha = Scaling_Scale( self->scaling, dAlpha, sThermalExpansivity );

 	Journal_Firewall( !isnan( alpha ), 
			  Journal_Register( Error_Type, (Name)PhysicalPropertyConfiguration_Type  ), 
			  "Thermal expansivity is nan at %gK and %gGPa\n", dTemperature, dPressure/1e9 );

	return alpha;
}*/

/*double PhysicalPropertyConfiguration_ThermalConductivity( void*            _self,
					double           temperature,
					double           pressure ) {

	PhysicalPropertyConfiguration*                 self = (PhysicalPropertyConfiguration*)_self;
	dimScalar                     dTemperature, dPressure;
	dimScalar                     dK;
	double                        K;
	MineralPhaseId                phaseId;

	dTemperature = Scaling_Unscale( self->scaling, temperature, sTemperature );
	dPressure = Scaling_Unscale( self->scaling, pressure, sPressure );

	phaseId = _PhysicalPropertyConfiguration_ActualMineralPhase( self, dTemperature, dPressure );

        dK = _PhysicalPropertyConfiguration_ThermalConductivity( self, dTemperature, dPressure, phaseId  );

	K = Scaling_Scale( self->scaling, dK, sThermalConductivity );

	Journal_Firewall( !isnan( K ), 
			  Journal_Register( Error_Type, (Name)PhysicalPropertyConfiguration_Type  ), 
			  "Thermal conductivity is nan at %gK and %gGPa\n", dTemperature, dPressure/1e9 );

	return K;
}*/


/*double PhysicalPropertyConfiguration_ThermalDiffusivity( void*            _self,
				       double           temperature,
				       double           pressure ) {

	PhysicalPropertyConfiguration*                 self = (PhysicalPropertyConfiguration*)_self;
	dimScalar                     dTemperature, dPressure, dKappa, dK, dDensity, dCp;
	double                        kappa;
	MineralPhaseId                phaseId;

	if( self->useConstantKappa )
	  return self->scaledKappa;

	dTemperature = Scaling_Unscale( self->scaling, temperature, sTemperature );
	dPressure = Scaling_Unscale( self->scaling, pressure, sPressure );

	phaseId = _PhysicalPropertyConfiguration_ActualMineralPhase( self, dTemperature, dPressure );

        dK = _PhysicalPropertyConfiguration_ThermalConductivity( self, dTemperature, dPressure, phaseId  );
        dDensity = _PhysicalPropertyConfiguration_Density( self, dTemperature, dPressure, phaseId  );
	dCp = self->Cp;

	dKappa = dK / (dDensity*dCp);

	kappa = Scaling_Scale( self->scaling, dKappa, sThermalDiffusivity );

	Journal_Firewall( !isnan( kappa ), 
			  Journal_Register( Error_Type, (Name)PhysicalPropertyConfiguration_Type  ), 
			  "Thermal diffusivity is nan at %gK and %gGPa\n", dTemperature, dPressure/1e9 );

	return kappa;
}*/


MineralPhaseId _ActualMineralPhase( void* _self, double temperature, double pressure ) {
	PhysicalPropertyConfiguration*  self = (PhysicalPropertyConfiguration*)_self;
	Index phase_I;

	for( phase_I = self->phasesCount - 1; phase_I > 0; phase_I-- ){
	  if( MineralPhase_AboveClapeyronCurve( self->phases[phase_I], temperature, pressure ) )
	    return (MineralPhaseId)phase_I;
	}
	
	return 0;
}


double computeWaterAfterMelt( void* _self, double melt, double  water ) {
	const double partitionCoeff = 0.008;
	double waterInResidual = water / ( melt/partitionCoeff + (1 - melt) );
	return waterInResidual;
}


/* FIXME dTemperature in C, dPressure in GPa */
double computeF( double dTemperature, double dPressure, double assWater, double meltingAcc, double melt ) {

  double meltWater, ATw, ATf, Ts, Tl, waterSaturatedSolidus, outmelt;
  const double gamma = 0.75;
  const double beta = 1.5;
  const double K = 43;
  const double c = 250;
  const double partCoef = 0.01;
  const double a1 = 1085.7;
  const double a2 = 132.9;
  const double a3 = -5.1;
  const double b1 = 1475.0;
  const double b2 = 80.0;
  const double b3 = -3.2;
  const double psi1 = 12;
  const double psi2 = 1;
  const double lambda = 0.6;

  outmelt = 0.0;
  meltWater = assWater / (partCoef + melt*(1 - partCoef));
  if( dPressure < 2 )
    ATw = K * pow( meltWater, gamma );
  else
    ATw = psi1  * pow( dPressure, lambda ) + psi2 + dPressure;
  ATf = c * meltingAcc;
  Ts = a1  +  a2 * dPressure  +  a3 * pow( dPressure, 2.0 )  -  ATw  +  ATf;
  Tl = b1  +  b2 * dPressure  +  b3 * pow( dPressure, 2.0 )  -  ATw  +  ATf;
  
  /* correction due to water saturation ?????????????????????????/ */
  waterSaturatedSolidus = 900 + 15*dPressure;
  if( Ts < waterSaturatedSolidus ) {
    Ts = waterSaturatedSolidus;
    /*Tl = Ts + 250;*/
  }

  if( dTemperature > Ts ) {
    if( dTemperature < Tl )
      outmelt = pow( (dTemperature - Ts) / (Tl - Ts), beta );
    else
      outmelt = 1.0;
  }
  return outmelt;
}

/* FIXME */
double computeMeltTPWF( void* _self, double dTemperature, double dPressure, double assWater, double meltingAcc ) {
	PhysicalPropertyConfiguration*  self = (PhysicalPropertyConfiguration*) _self;
	/* From Katz & Spiegelman, G3, 2003 */
	double melt = 0.0;
	int it = 0;
	double residual = 1;
	const double meltTolerance = 0.002;
	const double meltInc = 0.0001;
	const int maxIt = 10000;

	/* pressure in GPa */
	dPressure = dPressure / 1e9;

	if( dPressure < 10 ) {
	  /* temperature in C */
	  dTemperature = dTemperature - 273;

	  melt = computeF( dTemperature, dPressure, assWater, meltingAcc, melt );

	  if( melt > meltTolerance ) {
	    it = 0;
	    residual = meltTolerance + 1;
	    melt = 0;
	    while( fabs( residual ) > meltTolerance && it < maxIt ) {
	      melt = melt + meltInc;
	      residual = computeF( dTemperature, dPressure, assWater, meltingAcc, melt ) - melt;
	      it++;
	    }
	    if( it >= maxIt) {
	      Stream* warningStr = Journal_Register( Error_Type, self->type );

	      Journal_Printf( warningStr, "Warning - in %s: melt does not converge, assuming zero! - temperature=%gC, pressure=%gGPa, water=%gwt, accMelt=%g\n", 
			      __func__, dTemperature, dPressure, assWater, meltingAcc );
	      melt = 0;
	    }
	  }
	}	
	return melt;
}


/* FIXME */
double computeDensityTPWF( void*  _self, double temperature, double dTemperature, double pressure,  double dPressure, double totalWater, double meltFraction, double meltingAcc, double* outDensityTable ) {
	PhysicalPropertyConfiguration*  self = (PhysicalPropertyConfiguration*)_self;
	double densityTable, densitySolid, densityMelt, density;
	double alpha = 2;        /* kg/m3/deg_melt*/
	double rho0 = 2650;      /* kg/m3         */
	double dRhodP = 0.065;   /* kg/m3/GPa     */
	double dRhodT = -10e-4;  /* kg/m3/K       */
	double T0 = 800;         /* K             */
	double P0 = 1e-4;        /* Gpa           */
	double threshold = 0.01; /* max melt retained on the rock */ 

	double T = dTemperature;
	double P = dPressure / 1e9; /* GPa*/

	densityTable = PhysicalPropertyConfiguration_GetValueTPW( self, temperature, pressure, totalWater );
	outDensityTable[0] = densityTable;

	/* FIXME */
	/*densityTable = Scaling_Unscale( self->scaling, densityTable, sDensity );*/ /* km/m3 */

	/* Correction to account the depletion due to the melt the the rock has suffered in the past */
	densitySolid = densityTable - alpha * 100 * meltingAcc;
	/* Melt density */
	densityMelt = rho0 + dRhodT * (T-T0) + dRhodP * (P-P0);
	/*  */
	if( meltFraction > threshold )
	  meltFraction = threshold;
	/* Correction to account the current melt fraction */
	density = densitySolid * (1 - meltFraction) + densityMelt * meltFraction;

	return density; /* kg/m3 */
}


/* 
 * Public functions 
 *
 */

/* FIXME */
double PhysicalPropertyConfiguration_GetMeltTPWF( void* _self, double temperature, double pressure, double assWater, double meltingAcc ) {
	PhysicalPropertyConfiguration* self = (PhysicalPropertyConfiguration*) _self;
	double F, dTemperature, dPressure;

	assert( !isnan( temperature ) );
	assert( !isnan( pressure ) );
	assert( !isnan( assWater ) );
	assert( !isnan( meltingAcc ) );

   /* FIXME */
	/*dTemperature = Scaling_Unscale( self->scaling, temperature, sTemperature );
	  dPressure = Scaling_Unscale( self->scaling, pressure, sPressure );*/

	F = computeMeltTPWF( self, dTemperature, dPressure, assWater, meltingAcc );

	assert( !isnan( F ) && 0 <= F && F <= 1 );

	return F;
}


double PhysicalPropertyConfiguration_GetDensityTPWF( void* _self, double temperature, double pressure, double totalWater, double meltFraction, double meltingAcc, double* outDensityTable ) {
	
	PhysicalPropertyConfiguration* self = (PhysicalPropertyConfiguration*) _self;
	double density, dTemperature, dPressure;

	assert( !isnan( temperature ) );
	assert( !isnan( pressure ) );
	assert( !isnan( totalWater ) );
	assert( !isnan( meltFraction ) );
	assert( !isnan( meltingAcc ) );
	
	/* FIXME safety check for the holidays, all these checks should be as asserts */
	if( totalWater < 0 ) {
	  totalWater = 0;
	}
	if( meltFraction < 0 ) {
	  meltFraction = 0;
	}
	if( meltFraction > 1 ) {
	  meltFraction = 1;
	}
	if( meltingAcc < 0 ) {
	  meltingAcc = 0;
	}

	/* FIXME */
	/*dTemperature = Scaling_Unscale( self->scaling, temperature, sTemperature );
	  dPressure = Scaling_Unscale( self->scaling, pressure, sPressure );*/

	density = computeDensityTPWF( self, temperature, dTemperature, pressure, dPressure, totalWater, meltFraction, meltingAcc, outDensityTable );

	/* FIXME */
	/*density = Scaling_Scale( self->scaling, density, sDensity );*/

	assert( !isnan( density ) );

	return density;
}


double PhysicalPropertyConfiguration_GetWaterAfterMelt( void* _self, double assWater, double meltFraction ) {
	PhysicalPropertyConfiguration* self = (PhysicalPropertyConfiguration*) _self;

	assert( !isnan( assWater ) );
	assert( !isnan( meltFraction ) );

	double water = computeWaterAfterMelt( self, meltFraction, assWater );

	if( water < 0 ) {
	  water = 0;
	}

	assert( !isnan( water ) );

	return water;
}


double PhysicalPropertyConfiguration_GetValueTP( void* _self, double temperature, double pressure ) {
	PhysicalPropertyConfiguration* self = (PhysicalPropertyConfiguration*)_self;
	MineralPhaseId phaseId;
	double result;

	assert( !isnan( temperature ) );
	assert( !isnan( pressure ) );

	switch( self->setup ) {
	case useConstant:
	  result = self->value;
	  break;

	case useTable:
 	  result = LookupTable_Search( self->table, temperature, pressure );
	  break;

	case useWaterTables:
	  assert( self->waterTables[0] );
 	  result = LookupTable_Search( self->waterTables[0], temperature, pressure );
	  break;
	  
	case useDeltaViscosity:
	  phaseId = _ActualMineralPhase( self, temperature, pressure );	
	  assert( phaseId < self->phasesCount );
	  assert( self->phases[phaseId] );
	  result = self->phases[phaseId]->viscosityContrast;	
	  break;

	case useLinearDensity:
	case useLinearDensityWithPhaseChanges:
	  result = self->referenceDensity * 
	    ( 1 - self->thermalExpansivity * ( temperature - self->referenceTemperature ) ) *
	    ( 1 + self->compressibility    * ( pressure - self->referencePressure ) );

	  if( self->setup == useLinearDensityWithPhaseChanges ) {
	    phaseId = _ActualMineralPhase( self, temperature, pressure );
	    assert( phaseId < self->phasesCount );
	    assert( self->phases[phaseId] );
	    result +=  self->phases[phaseId]->densityIncrement;
	  }
	  break;

	default:
	  result = 0;
	}

	assert( !isnan( result ) );
	
	return result;
}

double PhysicalPropertyConfiguration_GetValueTPW( void* _self, double temperature, double pressure, double  totalWater ) {
	PhysicalPropertyConfiguration* self = (PhysicalPropertyConfiguration*)_self;
	MineralPhaseId phaseId;
	Index ix, ix2;
	double result, alpha, result2;

	assert( !isnan( temperature ) );
	assert( !isnan( pressure ) );
	assert( !isnan( totalWater ) );

	switch( self->setup ) {
	case useConstant: 
	  result = self->value;
	  break;

	case useTable:
 	  result = LookupTable_Search( self->table, temperature, pressure );
	  break;

	case useWaterTables:
#ifdef ASSUME_INTEGER_WATER_CONTENT

	  /*ix = (Index) rint( totalWater );*/
	  ix  = (Index) floor( totalWater );
	  ix2 = (Index) ceil( totalWater );

	  if( ix2 >= self->nWaterTables )
	    ix2 = self->nWaterTables - 1;
	  if( ix >= self->nWaterTables )
	    ix = self->nWaterTables - 1;

	  assert( ix2 < self->nWaterTables );
	  assert( self->waterTables[ix2] );
	  assert( ix < self->nWaterTables );
	  assert( self->waterTables[ix] );

	  result  = LookupTable_Search( self->waterTables[ix], temperature, pressure );
	  result2 = LookupTable_Search( self->waterTables[ix2], temperature, pressure );

	  alpha = totalWater-(double)ix;

	  result = result * (1-alpha) + result2 * alpha;

#else
	  for( ix = 0; ix < self->nWaterTables && self->waterIndex[ix] < totalWater; ix++ );
	  result = LookupTable_Search( self->waterTables[ix], temperature, pressure );
#endif	
	  break;
	  
	case useDeltaViscosity:
	  phaseId = _ActualMineralPhase( self, temperature, pressure );	
	  assert( phaseId < self->phasesCount );
	  assert( self->phases[phaseId] );
	  result = self->phases[phaseId]->viscosityContrast;	
	  break;

	default:
	  result = 0;
	}

	assert( !isnan( result ) );
	
	return result;
}
