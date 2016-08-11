#ifndef __PICellerator_Common_Ppc_MeltHeating_h__
#define __PICellerator_Common_Ppc_MeltHeating_h__

/** Textual name of this class */
extern const Type Ppc_MeltHeating_Type;

/** Ppc_MeltHeating class contents */
#define __Ppc_MeltHeating				\
  /* Parent info */					\
  __Ppc									\
  /* General data */					\
  int           lhfLabel; /* latent heat of fusion */\
  int           cpLabel; \
  int           meltLabel; \
  double        timeStepTolerance; \


	struct Ppc_MeltHeating { __Ppc_MeltHeating };

	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_OPERATION_DEFARGS \
                PPC_DEFARGS

	#define PPC_OPERATION_PASSARGS \
                PPC_PASSARGS

	Ppc_MeltHeating* _Ppc_MeltHeating_New(  PPC_OPERATION_DEFARGS  );
	
	void _Ppc_MeltHeating_Delete( void* _self );
	void _Ppc_MeltHeating_Print( void* _self, Stream* stream );
	void* _Ppc_MeltHeating_DefaultNew( Name name );
   void _Ppc_MeltHeating_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_MeltHeating_Build( void* _self, void* data );
	void _Ppc_MeltHeating_Initialise( void* _self, void* data );
	void _Ppc_MeltHeating_Execute( void* _self, void* data );
	void _Ppc_MeltHeating_Destroy( void* _self, void* data );


   /* Public functions */
	int _Ppc_MeltHeating_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );
	
#endif

