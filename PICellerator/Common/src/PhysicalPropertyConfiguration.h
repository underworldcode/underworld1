#ifndef __PICellerator_Common_PhysicalPropertyConfiguration_h__
#define __PICellerator_Common_PhysicalPropertyConfiguration_h__

/** Textual name of this class */
extern const Type PhysicalPropertyConfiguration_Type;


typedef enum {useConstant,
				  useTable,
				  useWaterTables,
				  useHofmeister,
				  useLinearDensity,
				  useLinearDensityWithPhaseChanges,
				  useDensityTPWF,
				  useDeltaViscosity,
				  errorPPC} SetupPhysicalPropertyConfiguration;

/*  */
typedef int MineralPhaseId;

/* Structures to store data inside the class  */
typedef struct{
  double        K298;					
  double        a;					
  double        gamma;					
  double        K0;					
  double        K0p;					
  double        rad1, rad2, rad3, rad4;			
  /* Private properties */
  double*       _B;			                
  double        _B2;			                
} Hofmeister1999;

typedef struct {
  double p0;
  double A1,B1,A2,B2;
  double maxMelt;
} PartialMeltingAfonso2008;

typedef struct {
  double dryF,A,B;
  double viscosityFactor;
} WaterContentAfonso2008;

/** Scaling class contents */
      #define __PhysicalPropertyConfiguration					               \
		/* Parent info */                                                              \
		__Stg_Component                                                                \
		FiniteElementContext*				    context;		       \
		/* General data */					                       \
      Stg_ComponentFactory*                               cf;                        \
		/*Scaling*                                            scaling;	*/	\
		/*		UnitDefinition_Type                                 units;*/ \
		SetupPhysicalPropertyConfiguration                  setup;                     \
		/* Configuration for constant values */                                        \
		double                                              value;                     \
		/*double                                              scaledValue;*/ \
		/* Configuration for Lookup table values */                                    \
		char*                                               tableName;		       \
		LookupTable*                                        table;                     \
		/* Mineral phases */                                                           \
		MineralPhase**                                      phases;                    \
		Index                                               phasesCount;               \
		/* Configuration with water */                                                 \
		Dictionary_Entry_Value*                             waterTableSpecs;           \
		LookupTable**                                       waterTables;   	       \
		double*                                             waterIndex;   	       \
		Index                                               nWaterTables;              \
		\
		Hofmeister1999                                      cond;                      \
		PartialMeltingAfonso2008                            partialMelt;               \
		WaterContentAfonso2008                              waterContent;              \
		/* Density */                                                                  \
		double                                              referenceDensity;          \
		double                                              referencePressure;         \
		double                                              referenceTemperature;      \
		double                                              compressibility;	\
		double                                              thermalExpansivity;	\



	struct PhysicalPropertyConfiguration { __PhysicalPropertyConfiguration };
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PHYSICALPROPERTYCONFIGURATION_DEFARGS \
                STG_COMPONENT_DEFARGS

	#define PHYSICALPROPERTYCONFIGURATION_PASSARGS \
                STG_COMPONENT_PASSARGS

	PhysicalPropertyConfiguration* _PhysicalPropertyConfiguration_New(  PHYSICALPROPERTYCONFIGURATION_DEFARGS  );
	

	void _PhysicalPropertyConfiguration_Delete( void* _self );
	void _PhysicalPropertyConfiguration_Print( void* _self, Stream* stream );

	void* _PhysicalPropertyConfiguration_DefaultNew( Name name );
   void _PhysicalPropertyConfiguration_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _PhysicalPropertyConfiguration_Build( void* _self, void* data );
	void _PhysicalPropertyConfiguration_Initialise( void* _self, void* data );
	void _PhysicalPropertyConfiguration_Execute( void* _self, void* data );
	void _PhysicalPropertyConfiguration_Destroy( void* _self, void* data );


   /* Public functions receive and return dimensionless values! */
   double PhysicalPropertyConfiguration_GetValueTP( void* _self, double temperature, double pressure );
   double PhysicalPropertyConfiguration_GetValueTPW( void* _self, double temperature, double pressure, double totalWater );
	double PhysicalPropertyConfiguration_GetMeltTPWF( void* _self, double temperature, double pressure, double assWater, double meltingAcc );
   double PhysicalPropertyConfiguration_GetDensityTPWF( void* _self, double temperature, double pressure, double totalWater, double  meltFraction, double  meltingAcc, double* outDensityTable );
   double PhysicalPropertyConfiguration_GetWaterAfterMelt( void* _self, double assWater, double meltFraction );

#endif

