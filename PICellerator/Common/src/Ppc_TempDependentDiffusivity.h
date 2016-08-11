#ifndef __PICellerator_Common_Ppc_TempDependentDiffusivity_h__
#define __PICellerator_Common_Ppc_TempDependentDiffusivity_h__

/** Textual name of this class */
extern const Type Ppc_TempDependentDiffusivity_Type;

/** Ppc_TempDependentDiffusivity class contents */
#define __Ppc_TempDependentDiffusivity \
/* Parent info */                \
  __Ppc                          \
  /* General data */             \
   FeVariable* temperatureField; \
   double T0;                    \
   double TCrit;                 \
   double K0;                    \
   double KCrit;

struct Ppc_TempDependentDiffusivity {__Ppc_TempDependentDiffusivity};

#ifndef ZERO
#define ZERO 0
#endif

#define PPC_TEMPDEPENDENTDIFFUSIVITY_DEFARGS \
                PPC_DEFARGS

#define PPC_TEMPDEPENDENTDIFFUSIVITY_PASSARGS \
                PPC_PASSARGS

Ppc_TempDependentDiffusivity* _Ppc_TempDependentDiffusivity_New(  PPC_TEMPDEPENDENTDIFFUSIVITY_DEFARGS  );

void _Ppc_TempDependentDiffusivity_Delete( void* _self );
void _Ppc_TempDependentDiffusivity_Print( void* _self, Stream* stream );
void* _Ppc_TempDependentDiffusivity_DefaultNew( Name name );
void _Ppc_TempDependentDiffusivity_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
void _Ppc_TempDependentDiffusivity_Build( void* _self, void* data );
void _Ppc_TempDependentDiffusivity_Initialise( void* _self, void* data );
void _Ppc_TempDependentDiffusivity_Execute( void* _self, void* data );
void _Ppc_TempDependentDiffusivity_Destroy( void* _self, void* data );
void _Ppc_TempDependentDiffusivity_Init( void* _self, FeVariable* temperatureField, double T0, double TCrit, double K0, double KCrit );

/* Public functions */
int _Ppc_TempDependentDiffusivity_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

