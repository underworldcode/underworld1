#ifndef __PICellerator_Common_Ppc_Depth_h__
#define __PICellerator_Common_Ppc_Depth_h__

/** Textual name of this class */
extern const Type Ppc_Depth_Type;

/** Ppc_Depth class contents */
#define __Ppc_Depth \
		/* Parent info */ \
		__Ppc \
		/* General data */ \
		FeMesh          *mesh; \
		Swarm           *ms; /* for passive tracer swarm */ \
      double          refHeight;

struct Ppc_Depth
{
   __Ppc_Depth
};


#ifndef ZERO
#define ZERO 0
#endif

#define PPC_CONSTANT_DEFARGS \
                PPC_DEFARGS

#define PPC_CONSTANT_PASSARGS \
                PPC_PASSARGS

Ppc_Depth* _Ppc_Depth_New(  PPC_CONSTANT_DEFARGS  );

void _Ppc_Depth_Delete( void* _self );
void _Ppc_Depth_Print( void* _self, Stream* stream );
void* _Ppc_Depth_DefaultNew( Name name );
void _Ppc_Depth_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
void _Ppc_Depth_Build( void* _self, void* data );
void _Ppc_Depth_Initialise( void* _self, void* data );
void _Ppc_Depth_Execute( void* _self, void* data );
void _Ppc_Depth_Destroy( void* _self, void* data );

void _Ppc_Depth_ReferenceHeight( Ppc_Depth* self, void* data);
/* Public functions */
int _Ppc_Depth_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

