#ifndef __PICellerator_Common_Ppc_h__
#define __PICellerator_Common_Ppc_h__

/* Virtual function Get */
typedef int (Ppc_GetFunction) ( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

/* Particle extension */
typedef struct {
  double value;
} Ppc_ParticleExt;

/** Textual name of this class */
extern const Type Ppc_Type;

/** Ppc class contents */
      #define __Ppc \
		/* Parent info */ \
		__Stg_Component \
		PICelleratorContext*	                                  context; \
		/* General data */ \
      Stg_ComponentFactory*                               cf; \
      PpcManager*                                         manager; \
		/* Virtual info */ \
		Ppc_GetFunction*                                    _get; \
		/* Threshold values*/ \
		double                                              minValue; \
		double                                              maxValue; \
      Bool                                                useMin; \
      Bool                                                useMax; \
      Bool                                                imposeThreshold; \
		/* Particle extension */ \
		Bool                                                storeValue; \
		ExtensionInfo_Index                                 particleExtHandle; \
      /* Setup streams */ \
      Stream*                                             info_stream; \
      Stream*                                             error_stream; \
      int                                                 ppcMgrID;\


	struct Ppc { __Ppc };
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_DEFARGS \
                STG_COMPONENT_DEFARGS,	\
					 Ppc_GetFunction* _get

	#define PPC_PASSARGS \
                STG_COMPONENT_PASSARGS, \
                _get

	Ppc* _Ppc_New(  PPC_DEFARGS  );
	
	void _Ppc_Delete( void* _self );
	void _Ppc_Print( void* _self, Stream* stream );
	void* _Ppc_DefaultNew( Name name );
   void _Ppc_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_Build( void* _self, void* data );
	void _Ppc_Initialise( void* _self, void* data );
	void _Ppc_Execute( void* _self, void* data );
	void _Ppc_Destroy( void* _self, void* data );

   void _Ppc_Init( void* _self, PpcManager* mgr, double min, double max, Bool impose );

   /* Private functions */
   int _Ppc_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );
   void _Ppc_GetValueFromFeVariable(void*, FeVariable*, Element_LocalIndex, IntegrationPoint*, double*);
#if 0
   void _Ppc_GetValueFromParticle(void*, IntegrationPointsSwarm*, Element_LocalIndex,IntegrationPoint*,double*);
#endif

   /* Public functions */
   int Ppc_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double * result );
   void Ppc_CheckLimits( void *_self, double value );
   double Ppc_ImposeLimits( void *_self, double value );
	void _Ppc_VariableConditionFunction( Index node_lI, Variable_Index var_I, void* context, void* _data, void* result );
	void _Ppc_Initialise_CF( void* _self, void* data ) ;
#endif

