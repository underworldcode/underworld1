#ifndef __PICellerator_Common_Ppc_MineralPhase_h__
#define __PICellerator_Common_Ppc_MineralPhase_h__

/** Textual name of this class */
extern const Type Ppc_MineralPhase_Type;


typedef struct {
  char*   name;
  double  referenceTemperature;
  double  referencePressure;			  
  double  clapeyronSlope;
  double  densityIncrement;
  double  viscosityContrast;
  double  B;
} Ppc_MineralPhase_Phase;


/** Ppc_MineralPhase class contents */
#define __Ppc_MineralPhase														\
  /* Parent info */																\
  __Ppc																				\
  /* General data */																\
  Ppc_MineralPhase_Phase*                     phasesList;			\
  int                                         phasesCount;			\
  double                                      value;					\
  char*                                       temperatureName;		\
  char*                                       pressureName;			\


	struct Ppc_MineralPhase { __Ppc_MineralPhase };

	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_MINERALPHASE_DEFARGS \
                PPC_DEFARGS

	#define PPC_MINERALPHASE_PASSARGS \
                PPC_PASSARGS

	Ppc_MineralPhase* _Ppc_MineralPhase_New(  PPC_MINERALPHASE_DEFARGS  );
	
	void _Ppc_MineralPhase_Delete( void* _self );
	void _Ppc_MineralPhase_Print( void* _self, Stream* stream );
	void* _Ppc_MineralPhase_DefaultNew( Name name );
   void _Ppc_MineralPhase_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_MineralPhase_Build( void* _self, void* data );
	void _Ppc_MineralPhase_Initialise( void* _self, void* data );
	void _Ppc_MineralPhase_Execute( void* _self, void* data );
	void _Ppc_MineralPhase_Destroy( void* _self, void* data );

   /* Public functions */
   int _Ppc_MineralPhase_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

