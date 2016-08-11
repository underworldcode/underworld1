#ifndef __PICellerator_Common_Ppc_LinearDensity_h__
#define __PICellerator_Common_Ppc_LinearDensity_h__

/** Textual name of this class */
extern const Type Ppc_LinearDensity_Type;

/** Ppc_LinearDensity class contents */
#define __Ppc_LinearDensity						 \
/* Parent info */										 \
  __Ppc													 \
  /* General data */									 \
  double            referenceDensity;			 \
  double            referenceTemperature;		 \
  double            referencePressure;			 \
  int             alphaTag;						 \
  int             betaTag;						 \
  int             temperatureTag;				 \
  int             pressureTag;					 \

	struct Ppc_LinearDensity { __Ppc_LinearDensity };
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_LINEARDENSITY_DEFARGS \
                PPC_DEFARGS

	#define PPC_LINEARDENSITY_PASSARGS \
                PPC_PASSARGS

	Ppc_LinearDensity* _Ppc_LinearDensity_New(  PPC_LINEARDENSITY_DEFARGS  );
	
	void _Ppc_LinearDensity_Delete( void* _self );
	void _Ppc_LinearDensity_Print( void* _self, Stream* stream );
	void* _Ppc_LinearDensity_DefaultNew( Name name );
   void _Ppc_LinearDensity_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_LinearDensity_Build( void* _self, void* data );
	void _Ppc_LinearDensity_Initialise( void* _self, void* data );
	void _Ppc_LinearDensity_Execute( void* _self, void* data );
	void _Ppc_LinearDensity_Destroy( void* _self, void* data );

   /* Public functions */
   int _Ppc_LinearDensity_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

