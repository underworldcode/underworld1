#ifndef __StGermain_Utils_Scaling_h__
#define __StGermain_Utils_Scaling_h__

/* to be removed */
typedef int PhysicalQuantity;


/* This struct stores the unit of any quantity in terms of the "base units" 
   (length, mass, time, temperature, electrical current).
   The field "c" has the conversion factor from whatever to SI units (ussually it is one).
   Once the number is in SI units we scale it using the other values of the struct.
   Note that this do NOT stores the actual scaling coefficients, it only has info on the 
   units of a physical quantity.
   */
   typedef struct {
     double lengPower;
     double massPower;
     double timePower;
     double tempPower;
     double currPower;
     double c;
   } UnitDefinition_Type;

/* Some definitions of commonly used values */
#define sAcceleration             (UnitDefinition_Type){ 1, 0,-2, 0, 0, 1}
#define sActivationEnergy         (UnitDefinition_Type){ 2, 1,-2, 0, 0, 1}
#define sArea                     (UnitDefinition_Type){ 2, 0, 0, 0, 0, 1}
#define sClapeyronSlope           (UnitDefinition_Type){-1, 1,-2,-1, 0, 1}
#define sCompressibility          (UnitDefinition_Type){ 1,-1, 2, 0, 0, 1}
#define sCp                       (UnitDefinition_Type){ 2, 0,-2,-1, 0, 1}
#define sDensity                  (UnitDefinition_Type){-3, 1, 0, 0, 0, 1}
#define sDistance                 (UnitDefinition_Type){ 1, 0, 0, 0, 0, 1}
#define sEnergy                   (UnitDefinition_Type){ 2, 1,-2, 0, 0, 1}
#define sEntropy                  sCp
#define sForce                    (UnitDefinition_Type){ 1, 1,-2, 0, 0, 1}
#define sGasConstant              (UnitDefinition_Type){ 2, 1,-2,-1, 0, 1}
#define sLatentHeat               sGasConstant
#define sHeatSource               (UnitDefinition_Type){-1, 1,-3, 0, 0, 1}
#define sMass                     (UnitDefinition_Type){ 0, 1, 0, 0, 0, 1}
#define sPermeability             sArea
#define sPressure                 (UnitDefinition_Type){-1, 1,-2, 0, 0, 1}
#define sPower                    (UnitDefinition_Type){ 2, 1,-3, 0, 0, 1}
#define sStrainRate               (UnitDefinition_Type){ 0, 0,-1, 0, 0, 1}
#define sTemperature              (UnitDefinition_Type){ 0, 0, 0, 1, 0, 1}
#define sTime                     (UnitDefinition_Type){ 0, 1, 0, 0, 0, 1}
#define sThermalConductivity      (UnitDefinition_Type){ 1, 1,-3,-1, 0, 1}
#define sThermalDiffusivity       (UnitDefinition_Type){ 2, 0,-1, 0, 0, 1}
#define sThermalExpansivity       (UnitDefinition_Type){ 0, 0, 0,-1, 0, 1}
#define sThermalExpansivityA      sThermalExpansivity
#define sThermalExpansivityB      (UnitDefinition_Type){ 0, 0, 0,-2, 0, 1}
#define sThermalExpansivityC      sTemperature
#define sThermalExpansivityD      (UnitDefinition_Type){ 0, 0, 0,-5, 0, 1}
#define sVelocity                 (UnitDefinition_Type){ 1, 0,-1, 0, 0, 1}
#define sViscosity                (UnitDefinition_Type){-1, 1,-1, 0, 0, 1}
#define sVoid                     (UnitDefinition_Type){ 0, 0, 0, 0, 0, 1}
#define sVolume                   (UnitDefinition_Type){ 3, 0, 0, 0, 0, 1}
#define sVolumetricHeatCapacity   (UnitDefinition_Type){-1, 1,-2,-1, 0, 1}


	/** Textual name of this class */
	extern const Type Scaling_Type;

 	/** Scaling class contents */
	#define __Scaling                                                                 \
		/* Parent info */                                                              \
		__Stg_Component                                                                \
		/* Basic scaling factors */                                                    \
		double                                              length;                    \
		double                                              time;                      \
		double                                              mass;                      \
		double                                              temperature;               \
		double                                              electricCurrent;           \
      Stream* info_stream; \
      Stream* error_stream; \

		
	struct Scaling { __Scaling };

	Scaling* Scaling_New( 
		Name                                                name,
		double                                              space,
		double                                              time,
		double                                              mass,
		double                                              temperature ,
      double                                              ec );

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define SCALING_DEFARGS \
                STG_COMPONENT_DEFARGS

	#define SCALING_PASSARGS \
                STG_COMPONENT_PASSARGS

	Scaling* _Scaling_New(  SCALING_DEFARGS  );
	void* _Scaling_DefaultNew( Name name ) ;
   void _Scaling_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) ;
   void _Scaling_Build( void* _self, void* data );
   void _Scaling_Initialise( void* _self, void* data );
   void _Scaling_Destroy( void* _self, void* data );
   void _Scaling_Init( void* _self, double l, double m, double t, double T, double ec );

   void _Scaling_ScaleList( void* _self, Dictionary_Entry* entryPtr, Dictionary_Entry_Value_List* listPtr );
   void _Scaling_ScaleEntry( void* _self, Dictionary_Entry* entryPtr, Dictionary_Entry_Value* valuePtr );
   /* public Scaling functions */
   double Scaling_Unscale( void* self,	double theValue, UnitDefinition_Type theUnit );
   double Scaling_Scale( void* self, double theValue, UnitDefinition_Type theUnit );
   UnitDefinition_Type Scaling_Parse( char* );
   /* given parsable SI units, return the dimensionalising coefficient */
   double Scaling_ParseDimCoeff( void* self, char* unitsStr );
   /* public function for scaling a dictionary */
   void Scaling_ScaleDictionary( void* _self, Dictionary* dick );
#endif

