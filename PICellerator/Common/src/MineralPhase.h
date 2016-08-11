

#ifndef __PICellerator_Common_MineralPhase_h__
#define __PICellerator_Common_MineralPhase_h__

	/** Textual name of this class */
	extern const Type MineralPhase_Type;

 	/** MineralPhase class contents */
        #define __MineralPhase						\
		/* Parent info */                                       \
                __Stg_Component					        \
		FiniteElementContext*		context;		\
		/* Phases - public properties*/				\
		double        referenceTemperature;			\
		double        referencePressure;			\
		double        clapeyronSlope;			        \
		double        clapeyronSlope2;			        \
		double        densityIncrement;			        \
		double        viscosityContrast;			\
		/* Thermal expansion parameters, Fei 1995 */		\
		double        teA;					\
		double        teB;					\
		double        teC;					\
		double        teD;					\
		/* Private properties */				\
		double        _B;			                \
		double        _B2;			                

	MineralPhase* MineralPhase_New( Name name, Stg_Component* _self ) ;

	struct MineralPhase { __MineralPhase };
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define MINERALPHASE_DEFARGS \
                STG_COMPONENT_DEFARGS

	#define MINERALPHASE_PASSARGS \
                STG_COMPONENT_PASSARGS

	MineralPhase* _MineralPhase_New(  MINERALPHASE_DEFARGS  );
	

	void _MineralPhase_Delete( void* _self );
	void _MineralPhase_Print( void* _self, Stream* stream );

	void* _MineralPhase_DefaultNew( Name name ) ;
	void _MineralPhase_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) ;
	void _MineralPhase_Build( void* _self, void* data ) ;
	void _MineralPhase_Initialise( void* _self, void* data ) ;
	void _MineralPhase_Execute( void* _self, void* data ) ;
	void _MineralPhase_Destroy( void* _self, void* data ) ;
	void _MineralPhase_Init( void* _self );

   /* Public functions */
   void MineralPhase_SetProperties( void* _self, double refT, double refP, double slope1, double slope2, double rhoInc, double viscContrast );

   /* This tells you if the (T,p) point is above the clapeyron line in the T-p space */
   Bool MineralPhase_AboveClapeyronCurve( void* _self, double temperature, double pressure );
   double MineralPhase_ThermalExpansivity( void* _self, double temperature );
					       
#endif

