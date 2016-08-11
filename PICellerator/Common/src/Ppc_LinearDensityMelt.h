#ifndef __PICellerator_Common_Ppc_LinearDensityMelt_h__
#define __PICellerator_Common_Ppc_LinearDensityMelt_h__

/** Textual name of this class */
extern const Type Ppc_LinearDensityMelt_Type;

/** Ppc_LinearDensityMelt class contents */
#define __Ppc_LinearDensityMelt						 \
/* Parent info */										 \
  __Ppc_LinearDensity								 \
  /* General data */									 \
  int             meltDensityCoeffTag;			 \
  int             meltFracTag;					

  struct Ppc_LinearDensityMelt { __Ppc_LinearDensityMelt };
 

	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_LINEARDENSITYMELT_DEFARGS \
                PPC_LINEARDENSITY_DEFARGS 

	#define PPC_LINEARDENSITYMELT_PASSARGS \
                PPC_LINEARDENSITY_PASSARGS

	Ppc_LinearDensityMelt* _Ppc_LinearDensityMelt_New(  PPC_LINEARDENSITYMELT_DEFARGS  );
	
	void* _Ppc_LinearDensityMelt_DefaultNew( Name name );
   void _Ppc_LinearDensityMelt_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );

   /* Public functions */
   int _Ppc_LinearDensityMelt_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

