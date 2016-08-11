#ifndef __PICellerator_Common_Ppc_Dt_h__
#define __PICellerator_Common_Ppc_Dt_h__

/** Textual name of this class */
extern const Type Ppc_Dt_Type;

/** Ppc_Dt class contents */
#define __Ppc_Dt				\
  /* Parent info */					\
  __Ppc									\
  /* General data */					\


	struct Ppc_Dt { __Ppc_Dt };

	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_OPERATION_DEFARGS \
                PPC_DEFARGS

	#define PPC_OPERATION_PASSARGS \
                PPC_PASSARGS

	Ppc_Dt* _Ppc_Dt_New(  PPC_OPERATION_DEFARGS  );
	
	void _Ppc_Dt_Delete( void* _self );
	void _Ppc_Dt_Print( void* _self, Stream* stream );
	void* _Ppc_Dt_DefaultNew( Name name );
   void _Ppc_Dt_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_Dt_Build( void* _self, void* data );
	void _Ppc_Dt_Initialise( void* _self, void* data );
	void _Ppc_Dt_Execute( void* _self, void* data );
	void _Ppc_Dt_Destroy( void* _self, void* data );


   /* Public functions */
	int _Ppc_Dt_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );
	
#endif

