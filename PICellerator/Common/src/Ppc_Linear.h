#ifndef __PICellerator_Common_Ppc_Linear_h__
#define __PICellerator_Common_Ppc_Linear_h__

/** Textual name of this class */
extern const Type Ppc_Linear_Type;

/** Ppc_Linear class contents */
#define __Ppc_Linear									 \
  /* Parent info */									 \
  __Ppc													 \
  /* General data */									 \
  int             fieldTag;						 \
  double            referenceValue;				 \
  int             alphaTag;						 \

	struct Ppc_Linear { __Ppc_Linear };
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_LINEAR_DEFARGS \
                PPC_DEFARGS

	#define PPC_LINEAR_PASSARGS \
                PPC_PASSARGS

	Ppc_Linear* _Ppc_Linear_New(  PPC_LINEAR_DEFARGS  );
	
	void _Ppc_Linear_Delete( void* _self );
	void _Ppc_Linear_Print( void* _self, Stream* stream );
	void* _Ppc_Linear_DefaultNew( Name name );
   void _Ppc_Linear_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_Linear_Build( void* _self, void* data );
	void _Ppc_Linear_Initialise( void* _self, void* data );
	void _Ppc_Linear_Execute( void* _self, void* data );
	void _Ppc_Linear_Destroy( void* _self, void* data );

   /* Public functions */
   int _Ppc_Linear_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

