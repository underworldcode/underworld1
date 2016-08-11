#ifndef __PICellerator_Common_Ppc_LithostaticPressure_h__
#define __PICellerator_Common_Ppc_LithostaticPressure_h__

/** Textual name of this class */
extern const Type Ppc_LithostaticPressure_Type;

/** Ppc_LithostaticPressure class contents */
#define __Ppc_LithostaticPressure									 \
  /* Parent info */									 \
  __Ppc													 \
  /* General data */									 \
  FeMesh*           mesh;							 \
  double            refDepth;		 \
  int               density_id;			 \
  Swarm*            ms;							 \

	struct Ppc_LithostaticPressure { __Ppc_LithostaticPressure };
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_LITHOSTATICPRESSURE_DEFARGS \
                PPC_DEFARGS

	#define PPC_LITHOSTATICPRESSURE_PASSARGS \
                PPC_PASSARGS

	Ppc_LithostaticPressure* _Ppc_LithostaticPressure_New(  PPC_LITHOSTATICPRESSURE_DEFARGS  );
	
	void _Ppc_LithostaticPressure_Delete( void* _self );
	void _Ppc_LithostaticPressure_Print( void* _self, Stream* stream );
	void* _Ppc_LithostaticPressure_DefaultNew( Name name );
   void _Ppc_LithostaticPressure_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_LithostaticPressure_Build( void* _self, void* data );
	void _Ppc_LithostaticPressure_Initialise( void* _self, void* data );
	void _Ppc_LithostaticPressure_Execute( void* _self, void* data );
	void _Ppc_LithostaticPressure_Destroy( void* _self, void* data );
   void _Ppc_LithostaticPressure_ReferenceHeight( Ppc_LithostaticPressure* self, void* data);

   /* Public functions */
   int _Ppc_LithostaticPressure_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

