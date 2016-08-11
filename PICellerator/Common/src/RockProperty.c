#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "PhysicalPropertyConfiguration.h"
#include "RockProperty.h"


/* Textual name of this class */
const Type RockProperty_Type = "RockProperty";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
RockProperty* _RockProperty_New(  ROCKPROPERTY_DEFARGS  )
{
	RockProperty* self;  
	assert( _sizeOfSelf >= sizeof(RockProperty) );
	nameAllocationType = NON_GLOBAL;
	self = (RockProperty*) _Stg_Component_New(  STG_COMPONENT_PASSARGS  );	
	return self;
}


void* _RockProperty_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(RockProperty);
	Type                                                      type = RockProperty_Type;
	Stg_Class_DeleteFunction*                              _delete = _RockProperty_Delete;
	Stg_Class_PrintFunction*                                _print = _RockProperty_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _RockProperty_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _RockProperty_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _RockProperty_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _RockProperty_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _RockProperty_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _RockProperty_Destroy;

	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;
	return (void*)_RockProperty_New(  ROCKPROPERTY_PASSARGS  );
}

void _RockProperty_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	RockProperty* self = (RockProperty*)_self;
	Dictionary*	theDictionary;
	MaterialPointsSwarm* materialPointsSwarm;
	StandardParticle particle;
	SwarmVariable* materialPointsSwarmVariable;

	theDictionary = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );

	self->context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", PICelleratorContext, False, data );
	if( !self->context  )
		self->context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", PICelleratorContext, True, data  );
							
	/* The component factory */							
	self->cf = cf;

	/* Add stuff to the materials */
	self->materials_Register = self->context->materials_Register; 
	assert( self->materials_Register  );
	
	/* General stuff */
	self->referenceDensity = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"ReferenceDensity", 3400  );
	self->gravity = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"Gravity", 9.81  );


	self->meltActivationTime = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"MeltActivationTime", 0 );  
	/* Threshold values */
	self->densityLimits.minimum = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"MinDensity", 1000 );
	self->densityLimits.maximum = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"MaxDensity", 10000 );

	/* Particle storage for diffusivity. The storage is actually done by ThermalConductivityDependingOnTP.c */
	self->storeDiffusivityOnParticles = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"StoreDiffusivity", 0 );
	/* Particle storage for conductivity. The storage is actually done by ThermalConductivityDependingOnTP.c */
	self->storeConductivityOnParticles = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"StoreConductivity", 0 );
	/* Particle storage for Cp. The storage is actually done by ThermalConductivityDependingOnTP.c */
	self->storeCpOnParticles = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"StoreCp", 0 );
	/* Particle storage for density. The storage is actually done by BouyancyForceTermTP.c */
	self->storeDensityOnParticles = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"StoreDensity", 0 );
	/* Particle storage for phase Id. The storage is actually done by BouyancyForceTermTP.c */
	self->storePhaseIdOnParticles = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"StorePhaseId", 0 );
	/* Particle storage for water. The storage is actually done by BouyancyForceTermTPW.c */
	self->storeWaterOnParticles = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"StoreWater", 0 );
	/* Particle storage for shear heating. The storage is actually done by ViscousHeatingTermTP.c */
	self->storeShearHeatingOnParticles = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"StoreShearHeating", 0 );
	/* Particle storage for adiabatic heating. The storage is actually done by AdiabaticHeatingTerm.c */
	self->storeAdiabaticHeatingOnParticles = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"StoreAdiabaticHeating", 0 );
	/* Particle storage for oldDensity. The storage is actually done by massCorrection.c */
	self->storeOldDensityOnParticles = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"StoreOldDensity", 0 );
	/* Particle storage for melt. The storage is actually done by the ppu plugin */
	self->storeMeltOnParticles = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"StoreMelt", 0 );
	/* Particle storage for divergence. The storage is actually done by the massCorrectionTerm component */
	self->storeDivergenceOnParticles = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"StoreDivergence", 0 );

	materialPointsSwarm = Stg_ComponentFactory_ConstructByName( cf, (Name)"materialSwarm", MaterialPointsSwarm, False, data );
	self->materialPointsSwarm = materialPointsSwarm;

	if( self->storeDiffusivityOnParticles  ) {
	  ParticleExt_Diffusivity*   particleExt;
	  
	  self->diffParticleExtHandle = ExtensionManager_Add( materialPointsSwarm->particleExtensionMgr, (Name)self->type, sizeof( ParticleExt_Diffusivity )  );	    
	  particleExt = ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, &particle, 
					      self->diffParticleExtHandle );
	  materialPointsSwarmVariable = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"ThermalDiffusivity", (ArithPointer) &particleExt->diffusivity - (ArithPointer) &particle, Variable_DataType_Double );

	}
	if( self->storeConductivityOnParticles  ) {
	  ParticleExt_Conductivity*  particleExt;
	  
	  self->condParticleExtHandle = ExtensionManager_Add( materialPointsSwarm->particleExtensionMgr, (Name)self->type, sizeof( ParticleExt_Conductivity )  );
	  
	  particleExt = ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, &particle, 
					      self->condParticleExtHandle );
	  
	  materialPointsSwarmVariable = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"ThermalConductivity", (ArithPointer) &particleExt->conductivity - (ArithPointer) &particle, Variable_DataType_Double );
	}
	if( self->storeCpOnParticles  ) {

	  ParticleExt_Cp*            particleExt;
	  
	  self->CpParticleExtHandle = ExtensionManager_Add( materialPointsSwarm->particleExtensionMgr, (Name)self->type, sizeof( ParticleExt_Cp )  );
	  
	  particleExt = ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, &particle, 
					      self->CpParticleExtHandle );
	  
	  materialPointsSwarmVariable = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"Cp", (ArithPointer) &particleExt->Cp - (ArithPointer) &particle, Variable_DataType_Double );

	}
	if( self->storeDensityOnParticles  ) {
	  ParticleExt_Density*       particleExt;
	  
	  self->densParticleExtHandle = ExtensionManager_Add( materialPointsSwarm->particleExtensionMgr, (Name)self->type, sizeof( ParticleExt_Density )  );
	  
	  particleExt = ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, &particle, 
					      self->densParticleExtHandle );
	  
	  materialPointsSwarmVariable = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"Density", (ArithPointer) &particleExt->density - (ArithPointer) &particle, Variable_DataType_Double );
	}
	if( self->storePhaseIdOnParticles  ) {
	  ParticleExt_PhaseId*       particleExt;
	  
	  self->phaseIdParticleExtHandle = ExtensionManager_Add( materialPointsSwarm->particleExtensionMgr, (Name)self->type, sizeof( ParticleExt_PhaseId )  );
	  
	  particleExt = ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, &particle, 
					      self->phaseIdParticleExtHandle );
	  
	  materialPointsSwarmVariable = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"PhaseId", (ArithPointer) &particleExt->id - (ArithPointer) &particle, Variable_DataType_Double );
	}
	if( self->storeWaterOnParticles  ) {
	  ParticleExt_Water*         particleExt;

	  
	  self->waterParticleExtHandle = ExtensionManager_Add( materialPointsSwarm->particleExtensionMgr, (Name)self->type, sizeof( ParticleExt_Water )  );
	  
	  particleExt = ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, &particle, self->waterParticleExtHandle );
	  
	  materialPointsSwarmVariable = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"WaterInAssemblage", (ArithPointer) &particleExt->waterInAssemblage - (ArithPointer) &particle, Variable_DataType_Double  );
	  materialPointsSwarmVariable = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"WaterSource", (ArithPointer) &particleExt->waterSource - (ArithPointer) &particle, Variable_DataType_Double );	  
	}
	if( self->storeShearHeatingOnParticles  ) {
	  ParticleExt_ShearHeating*       particleExt;
	  
	  self->shearHeatingParticleExtHandle = ExtensionManager_Add( materialPointsSwarm->particleExtensionMgr, (Name)self->type, sizeof( ParticleExt_ShearHeating )  );
	  
	  particleExt = ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, &particle, 
					      self->shearHeatingParticleExtHandle );
	  
	  materialPointsSwarmVariable = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"ShearHeating", (ArithPointer) &particleExt->value - (ArithPointer) &particle, Variable_DataType_Double );
	}
	if( self->storeAdiabaticHeatingOnParticles  ) {
	  ParticleExt_AdiabaticHeating*       particleExt;
	  
	  self->adiabaticHeatingParticleExtHandle = ExtensionManager_Add( materialPointsSwarm->particleExtensionMgr, (Name)self->type, sizeof( ParticleExt_AdiabaticHeating )  );
	  
	  particleExt = ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, &particle, 
					      self->adiabaticHeatingParticleExtHandle );
	  
	  materialPointsSwarmVariable = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"AdiabaticHeating", (ArithPointer) &particleExt->value - (ArithPointer) &particle, Variable_DataType_Double  );
	}
	if( self->storeOldDensityOnParticles ) {
	  ParticleExt_MassCorrection*       particleExt;
	  
	  self->oldDensityParticleExtHandle = ExtensionManager_Add( materialPointsSwarm->particleExtensionMgr, (Name)self->type, sizeof( ParticleExt_MassCorrection ) );
	  
	  particleExt = ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, &particle, 
					      self->oldDensityParticleExtHandle );
	  
	  materialPointsSwarmVariable = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"OldDensity", (ArithPointer) &particleExt->oldDensity - (ArithPointer) &particle, Variable_DataType_Double );
	  materialPointsSwarmVariable = Swarm_NewScalarVariable( materialPointsSwarm, (Name) "NewDensity", (ArithPointer) &particleExt->newDensity - (ArithPointer) &particle, Variable_DataType_Double );
	}
	if( self->storeMeltOnParticles ) {
	  ParticleExt_Melt*         particleExt;
	  
	  self->meltParticleExtHandle = ExtensionManager_Add( materialPointsSwarm->particleExtensionMgr, (Name)self->type, sizeof( ParticleExt_Melt ) );
	  
	  particleExt = ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, &particle, self->meltParticleExtHandle );
	  
	  materialPointsSwarmVariable = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"Melt", (ArithPointer) &particleExt->melt - (ArithPointer) &particle, Variable_DataType_Double );
	  materialPointsSwarmVariable = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"MeltAcc", (ArithPointer) &particleExt->meltAcc - (ArithPointer) &particle,	Variable_DataType_Double );
	}
	if( self->storeDivergenceOnParticles ) {
	  ParticleExt_Divergence* particleExt;
	  
	  self->divergenceParticleExtHandle = ExtensionManager_Add( materialPointsSwarm->particleExtensionMgr, (Name)self->type, sizeof( ParticleExt_Divergence ) );
	  
	  particleExt = ExtensionManager_Get( materialPointsSwarm->particleExtensionMgr, &particle, self->divergenceParticleExtHandle );
	  
	  materialPointsSwarmVariable = Swarm_NewScalarVariable( materialPointsSwarm, (Name)"Divergence", (ArithPointer) &particleExt->value - (ArithPointer) &particle, Variable_DataType_Double );
	}
}


void _RockProperty_Build( void* _self, void* data ) {
  RockProperty* self = (RockProperty*) _self;
  Stream* info = Journal_Register( Info_Type, (Name)self->type  );
  RockProperty_MaterialExt* materialExt;
  Material_Index material_I;
  Material* material = NULL;
  Index numMaterials = 0;
  char* configName;


  if( self->materialPointsSwarm ) {
	 /* Add configuration to the materials */
	 Journal_Printf( info, "Linking materials and properties... \n" );
	 
	 numMaterials = Materials_Register_GetCount( self->materials_Register );
	 
	 self->materialExtHandle = Materials_Register_AddMaterialExtension( self->materials_Register, self->type, 
																							  sizeof(RockProperty_MaterialExt) );
   
	 for( material_I = 0; material_I < numMaterials; material_I++ ) {
		material = Materials_Register_GetByIndex( self->materials_Register, material_I );
		materialExt = ExtensionManager_GetFunc( material->extensionMgr, material, self->materialExtHandle );

		Journal_Printf( info, "\nPhysical properties for material %s:\n", material->name );

		configName = Stg_ComponentFactory_GetString( self->cf, material->name, (Dictionary_Entry_Key)"DensityConfiguration", NULL );
		if( configName  ) {
		  materialExt->density = Stg_ComponentFactory_ConstructByName( self->cf, (Name)configName, PhysicalPropertyConfiguration, True, data  );
		} else {
		  materialExt->density = NULL;
		}

		configName = Stg_ComponentFactory_GetString( self->cf, material->name, (Dictionary_Entry_Key)"ConductivityConfiguration", NULL   );
		if( configName ) {
		  materialExt->thermalConductivity = Stg_ComponentFactory_ConstructByName( self->cf, (Name)configName, PhysicalPropertyConfiguration, True, data  );
		} else {
		  materialExt->thermalConductivity = NULL;
		}

										
		configName = Stg_ComponentFactory_GetString( self->cf, material->name, (Dictionary_Entry_Key)"ExpansivityConfiguration", NULL  );
		if( configName ) {
		  materialExt->thermalExpansion = Stg_ComponentFactory_ConstructByName( self->cf, (Name)configName, PhysicalPropertyConfiguration, True, data  ); 
		} else {
		  materialExt->thermalExpansion = NULL;
		}
       
		configName = Stg_ComponentFactory_GetString( self->cf, material->name, (Dictionary_Entry_Key)"CpConfiguration", NULL  );
		if( configName ) {
		  materialExt->Cp = Stg_ComponentFactory_ConstructByName( self->cf, (Name)configName, PhysicalPropertyConfiguration, True, data  );
		} else {
		  materialExt->Cp = NULL;
		}
		 
		configName = Stg_ComponentFactory_GetString( self->cf, material->name, (Dictionary_Entry_Key)"PhaseIdConfiguration", NULL  );
		if( configName ) {
		  materialExt->phaseId = Stg_ComponentFactory_ConstructByName( self->cf, (Name)configName, PhysicalPropertyConfiguration, True, data  );
		} else {
		  materialExt->phaseId = NULL;
		}
       
		configName = Stg_ComponentFactory_GetString( self->cf, material->name, (Dictionary_Entry_Key)"WaterConfiguration", NULL  );
		if( configName ) {
		  materialExt->waterInAssemblage = Stg_ComponentFactory_ConstructByName( self->cf, (Name)configName, PhysicalPropertyConfiguration, True, data  );
		} else {
		  materialExt->waterInAssemblage = NULL;
		}

		materialExt->initialWaterInAssemblage = Stg_ComponentFactory_GetDouble( self->cf, material->name, (Dictionary_Entry_Key)"InitialWaterInAssemblage", 0.0  );

		materialExt->useMassCorrection = Stg_ComponentFactory_GetBool( self->cf, material->name, (Dictionary_Entry_Key)"UseMassCorrection", 0 );

		materialExt->useMelt = Stg_ComponentFactory_GetBool( self->cf, material->name, (Dictionary_Entry_Key)"UseMelt", 0 );

	 }		     
	 Journal_Printf( info, "done\n" );
  }
}

void _RockProperty_Initialise( void* _self, void* data ) {

   RockProperty*               self = (RockProperty*) _self;
   MaterialPointsSwarm*        swarm = self->materialPointsSwarm;
   unsigned	               p_i;
   MaterialPoint*	       particle = NULL;

   if( swarm ) {
	  Stg_Component_Initialise( swarm, data, False );
   }

   if( self->storeWaterOnParticles && !self->context->loadFromCheckPoint ) {
     ParticleExt_Water*          particleExt = NULL;
     Material*                   material = NULL;
     RockProperty_MaterialExt*   materialExt = NULL;

     /* Initialize water in assemblage field (stored on particles) */
     for( p_i = 0; p_i < swarm->particleLocalCount; p_i++ ) {
       particle = (MaterialPoint*)Swarm_ParticleAt( swarm, p_i );

       material = Materials_Register_GetByIndex( self->materials_Register, particle->materialIndex );

       materialExt = ExtensionManager_GetFunc( material->extensionMgr, material, self->materialExtHandle );

       particleExt = ExtensionManager_Get( swarm->particleExtensionMgr, particle, self->waterParticleExtHandle );       

       particleExt->waterInAssemblage = materialExt->initialWaterInAssemblage;
       particleExt->waterSource = 0.0;
     }	  
   }
   if( self->storeOldDensityOnParticles && !self->context->loadFromCheckPoint ) {
     ParticleExt_MassCorrection* particleExt = NULL;

     for( p_i = 0; p_i < swarm->particleLocalCount; p_i++ ) {
       particle = (MaterialPoint*)Swarm_ParticleAt( swarm, p_i );
       particleExt = ExtensionManager_Get( swarm->particleExtensionMgr, particle, self->oldDensityParticleExtHandle );
       particleExt->oldDensity = 0.0;
       particleExt->newDensity = 0.0;
     }
   }
   if( self->storeDensityOnParticles && !self->context->loadFromCheckPoint ) {
     ParticleExt_Density*      particleExt = NULL;

     for( p_i = 0; p_i < swarm->particleLocalCount; p_i++ ) {
       particle = (MaterialPoint*)Swarm_ParticleAt( swarm, p_i );
       particleExt = ExtensionManager_Get( swarm->particleExtensionMgr, particle, self->densParticleExtHandle );
       particleExt->density = 0.0;
     }
   }
   if( self->storeMeltOnParticles && !self->context->loadFromCheckPoint ) {
     ParticleExt_Melt*      particleExt = NULL;

     for( p_i = 0; p_i < swarm->particleLocalCount; p_i++ ) {
       particle = (MaterialPoint*)Swarm_ParticleAt( swarm, p_i );
       particleExt = ExtensionManager_Get( swarm->particleExtensionMgr, particle, self->meltParticleExtHandle );
       particleExt->melt = 0.0;
       particleExt->meltAcc = 0.0;
     }
   }
   if( self->storeDivergenceOnParticles && !self->context->loadFromCheckPoint ) {
     ParticleExt_Divergence* particleExt = NULL;

     for( p_i = 0; p_i < swarm->particleLocalCount; p_i++ ) {
       particle = (MaterialPoint*)Swarm_ParticleAt( swarm, p_i );
       particleExt = ExtensionManager_Get( swarm->particleExtensionMgr, particle, self->divergenceParticleExtHandle );
       particleExt->value = 0.0;
     }
   }
}

void _RockProperty_Delete( void* _self ) {}

void _RockProperty_Print( void* _self, Stream* stream ) {}

void _RockProperty_Execute( void* _self, void* data ) {}

void _RockProperty_Destroy( void* _self, void* data ) {}


RockProperty* RockProperty_New( 
	Name				name,
	Stg_Component*	_self ) {

	RockProperty* self = (RockProperty*) _self;

	return self;
}

double RockProperty_DepthToPressure( void* _self, double depth ) {
	RockProperty* self = (RockProperty*)_self;

	return depth * self->gravity * self->referenceDensity;	
}

double RockProperty_Gravity( void* _self ) {
	RockProperty* self = (RockProperty*)_self;

	return self->gravity;
}

/* FIXME!!! depth is dimensional (meters) */
double RockProperty_GravityZ( void* _self, double depth ) {
	double gravity;

	depth /= 1000; /* depth in km */
	gravity = 9.8267 + 0.000279 * depth + 4.386e-7 * pow(depth,2) + 5.94e-10 * pow(depth,3);
	/*gravity = Scaling_Scale( self->scaling, gravity, sAcceleration );*/
	return gravity;
}



