#ifndef __PICellerator_Common_Ppc_PartialMelt_h__
#define __PICellerator_Common_Ppc_PartialMelt_h__

/** Textual name of this class */
extern const Type Ppc_PartialMelt_Type;

/** Ppc_PartialMelt class contents */
#define __Ppc_PartialMelt						 \
/* Parent info */										 \
  __Ppc													 \
  /* General data */									 \
  int             liquidusTag;              \
  int             solidusTag;               \
  int             temperatureTag;				 \
 
struct Ppc_PartialMelt
{
   __Ppc_PartialMelt
};

#ifndef ZERO
#define ZERO 0
#endif

#define PPC_LINEARDENSITY_DEFARGS \
                PPC_DEFARGS

#define PPC_LINEARDENSITY_PASSARGS \
                PPC_PASSARGS

Ppc_PartialMelt* _Ppc_PartialMelt_New(  PPC_LINEARDENSITY_DEFARGS  );

void _Ppc_PartialMelt_Delete( void* _self );
void _Ppc_PartialMelt_Print( void* _self, Stream* stream );
void* _Ppc_PartialMelt_DefaultNew( Name name );
void _Ppc_PartialMelt_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
void _Ppc_PartialMelt_Build( void* _self, void* data );
void _Ppc_PartialMelt_Initialise( void* _self, void* data );
void _Ppc_PartialMelt_Execute( void* _self, void* data );
void _Ppc_PartialMelt_Destroy( void* _self, void* data );

/* Public functions */
int _Ppc_PartialMelt_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

