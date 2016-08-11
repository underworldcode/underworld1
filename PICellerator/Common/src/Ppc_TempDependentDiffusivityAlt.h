#ifndef __PICellerator_Common_Ppc_TempDependentDiffusivityAlt_h__
#define __PICellerator_Common_Ppc_TempDependentDiffusivityAlt_h__

/** Textual name of this class */
extern const Type Ppc_TempDependentDiffusivityAlt_Type;

/** Ppc_TempDependentDiffusivityAlt class contents */
#define __Ppc_TempDependentDiffusivityAlt \
/* Parent info */                \
  __Ppc                          \
  /* General data */             \
   FeVariable* temperatureField; \
   double T0;                    \
   double TCrit;                 \
   double K0;                    \
   double KCrit;

struct Ppc_TempDependentDiffusivityAlt {__Ppc_TempDependentDiffusivityAlt};

#ifndef ZERO
#define ZERO 0
#endif

#define PPC_TEMPDEPENDENTDIFFUSIVITYALT_DEFARGS \
                PPC_DEFARGS

#define PPC_TEMPDEPENDENTDIFFUSIVITYALT_PASSARGS \
                PPC_PASSARGS

Ppc_TempDependentDiffusivityAlt* _Ppc_TempDependentDiffusivityAlt_New(  PPC_TEMPDEPENDENTDIFFUSIVITY_DEFARGS  );

void _Ppc_TempDependentDiffusivityAlt_Delete( void* _self );
void _Ppc_TempDependentDiffusivityAlt_Print( void* _self, Stream* stream );
void* _Ppc_TempDependentDiffusivityAlt_DefaultNew( Name name );
void _Ppc_TempDependentDiffusivityAlt_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
void _Ppc_TempDependentDiffusivityAlt_Build( void* _self, void* data );
void _Ppc_TempDependentDiffusivityAlt_Initialise( void* _self, void* data );
void _Ppc_TempDependentDiffusivityAlt_Execute( void* _self, void* data );
void _Ppc_TempDependentDiffusivityAlt_Destroy( void* _self, void* data );
void _Ppc_TempDependentDiffusivityAlt_Init( void* _self, FeVariable* temperatureField, double T0, double TCrit, double K0, double KCrit );

/* Public functions */
int _Ppc_TempDependentDiffusivityAlt_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

