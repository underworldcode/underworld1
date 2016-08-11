/* This class provides methods to compute several physical properties of rocks based on 
 * temperature and pressure.  
 * 
 * All the public functions expect and return scaled (dimensionless) values.
 */

#ifndef __PICellerator_Common_RockProperty_h__
#define __PICellerator_Common_RockProperty_h__

/** Textual name of this class */
extern const Type RockProperty_Type;


/* Structures for storing thing on particles */
typedef struct {
  double diffusivity;
} ParticleExt_Diffusivity;

typedef struct {
  double conductivity;
} ParticleExt_Conductivity;

typedef struct {
  double Cp;
} ParticleExt_Cp;

typedef struct {
  double density;
} ParticleExt_Density;

typedef struct {
  double id;
} ParticleExt_PhaseId;

typedef struct {
  double waterInAssemblage;
  double waterSource;
} ParticleExt_Water;
				
typedef struct {
  double value;
} ParticleExt_ShearHeating;
					
typedef struct {
  double value;
} ParticleExt_AdiabaticHeating;

typedef struct {
  double oldDensity;
  double newDensity;
} ParticleExt_MassCorrection;

typedef struct {
  double melt;
  double meltAcc;
} ParticleExt_Melt;

typedef struct {
  double value;
} ParticleExt_Divergence;

/* Structures for storing thing on materials */
typedef struct {
  PhysicalPropertyConfiguration*  density;
  PhysicalPropertyConfiguration*  thermalConductivity;
  PhysicalPropertyConfiguration*  thermalExpansion;
  PhysicalPropertyConfiguration*  phaseId;
  PhysicalPropertyConfiguration*  Cp;
  PhysicalPropertyConfiguration*  waterInAssemblage;
  double                          initialWaterInAssemblage;
  Bool                            useMassCorrection;
  Bool                            useMelt;
} RockProperty_MaterialExt;

typedef struct {
  double minimum;
  double maximum;
} RockProperty_Threshold;

 	/** Scaling class contents */
	#define __RockProperty                                                                 \
		/* Parent info */                                                              \
		__Stg_Component                                                                \
		PICelleratorContext*				    context;		       \
		/* General data */    					                       \
		MaterialPointsSwarm*                                materialPointsSwarm;       \
		Stg_ComponentFactory*                               cf;	                       \
		double                                              referenceDensity;          \
		double                                              gravity;                   \
		double                                              meltActivationTime;        \
		/* Storage on particles */                                                     \
		Bool                                                storeDiffusivityOnParticles;\
		ExtensionInfo_Index                                 diffParticleExtHandle;     \
		Bool                                                storeConductivityOnParticles;\
		ExtensionInfo_Index                                 condParticleExtHandle;     \
		Bool                                                storeCpOnParticles;        \
		ExtensionInfo_Index                                 CpParticleExtHandle;       \
		Bool                                                storeDensityOnParticles;   \
		ExtensionInfo_Index                                 densParticleExtHandle;     \
		Bool                                                storeOldDensityOnParticles; \
		ExtensionInfo_Index                                 oldDensityParticleExtHandle; \
		Bool                                                storePhaseIdOnParticles;   \
		ExtensionInfo_Index                                 phaseIdParticleExtHandle;  \
		Bool                                                storeWaterOnParticles;     \
		ExtensionInfo_Index                                 waterParticleExtHandle;    \
		Bool                                                storeShearHeatingOnParticles; \
		ExtensionInfo_Index                                 shearHeatingParticleExtHandle; \
		Bool                                                storeAdiabaticHeatingOnParticles; \
		ExtensionInfo_Index                                 adiabaticHeatingParticleExtHandle; \
		Bool                                                storeMeltOnParticles;      \
		ExtensionInfo_Index                                 meltParticleExtHandle;     \
		Bool                                                storeDivergenceOnParticles;      \
		ExtensionInfo_Index                                 divergenceParticleExtHandle;     \
		/* Material extension */                                                       \
		Materials_Register*                                 materials_Register;	       \
		ExtensionInfo_Index                                 materialExtHandle;         \
		/* Threshold vaues */                                                          \
		RockProperty_Threshold                              densityLimits;             \



	struct RockProperty { __RockProperty };

	RockProperty* RockProperty_New( 
		Name                                                name,
		Stg_Component*                                      _self );

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define ROCKPROPERTY_DEFARGS \
                STG_COMPONENT_DEFARGS

	#define ROCKPROPERTY_PASSARGS \
                STG_COMPONENT_PASSARGS

	RockProperty* _RockProperty_New(  ROCKPROPERTY_DEFARGS  );
	

	void _RockProperty_Delete( void* _self );
	void _RockProperty_Print( void* _self, Stream* stream );

	void* _RockProperty_DefaultNew( Name name );
   void _RockProperty_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _RockProperty_Build( void* _self, void* data );
	void _RockProperty_Initialise( void* _self, void* data );
	void _RockProperty_Execute( void* _self, void* data );
	void _RockProperty_Destroy( void* _self, void* data );


   /* Public functions  */
   double RockProperty_DepthToPressure( void* _self, double depth );
	double RockProperty_Gravity( void* _self );
   double RockProperty_GravityZ( void* _self, double depth );

#endif

