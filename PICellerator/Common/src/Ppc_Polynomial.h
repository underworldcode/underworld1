#ifndef __PICellerator_Common_Ppc_Polynomial_h__
#define __PICellerator_Common_Ppc_Polynomial_h__

/** Textual name of this class */
extern const Type Ppc_Polynomial_Type;

/** Ppc_Polynomial class contents */
#define __Ppc_Polynomial							 \
  /* Parent info */									 \
  __Ppc													 \
  /* General data */									 \
  int             fieldTag;						 \
  double            referenceValue;				 \
  int               termsCount;					 \
  double*           coefficient;					 \
  double*           power;							 \

	struct Ppc_Polynomial { __Ppc_Polynomial };
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_POLYNOMIAL_DEFARGS \
                PPC_DEFARGS

	#define PPC_POLYNOMIAL_PASSARGS \
                PPC_PASSARGS

	Ppc_Polynomial* _Ppc_Polynomial_New(  PPC_POLYNOMIAL_DEFARGS  );
	
	void _Ppc_Polynomial_Delete( void* _self );
	void _Ppc_Polynomial_Print( void* _self, Stream* stream );
	void* _Ppc_Polynomial_DefaultNew( Name name );
   void _Ppc_Polynomial_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_Polynomial_Build( void* _self, void* data );
	void _Ppc_Polynomial_Initialise( void* _self, void* data );
	void _Ppc_Polynomial_Execute( void* _self, void* data );
	void _Ppc_Polynomial_Destroy( void* _self, void* data );

   /* Public functions */
   int _Ppc_Polynomial_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

