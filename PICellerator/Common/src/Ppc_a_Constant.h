#ifndef __PICellerator_Common_Ppc_a_Constant_h__
#define __PICellerator_Common_Ppc_a_Constant_h__

/** Textual name of this class */
extern const Type Ppc_a_Constant_Type;

/** Ppc_a_Constant class contents */
#define __Ppc_a_Constant									 \
  /* Parent info */									 \
  __Ppc													 \
  /* General data */									 \
  double          constant;				 \
  int             alphaTag;						 \

	struct Ppc_a_Constant { __Ppc_a_Constant };
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_LINEAR_DEFARGS \
                PPC_DEFARGS

	#define PPC_LINEAR_PASSARGS \
                PPC_PASSARGS

	Ppc_a_Constant* _Ppc_a_Constant_New(  PPC_LINEAR_DEFARGS  );
	
	void _Ppc_a_Constant_Delete( void* _self );
	void _Ppc_a_Constant_Print( void* _self, Stream* stream );
	void* _Ppc_a_Constant_DefaultNew( Name name );
   void _Ppc_a_Constant_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_a_Constant_Build( void* _self, void* data );
	void _Ppc_a_Constant_Initialise( void* _self, void* data );
	void _Ppc_a_Constant_Execute( void* _self, void* data );
	void _Ppc_a_Constant_Destroy( void* _self, void* data );

   /* Public functions */
   int _Ppc_a_Constant_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

