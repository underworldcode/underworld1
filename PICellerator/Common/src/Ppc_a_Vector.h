#ifndef __PICellerator_Common_Ppc_a_Vector_h__
#define __PICellerator_Common_Ppc_a_Vector_h__

/** Textual name of this class */
extern const Type Ppc_a_Vector_Type;

/** Ppc_a_Vector class contents */
#define __Ppc_a_Vector									 \
  /* Parent info */									 \
  __Ppc													 \
  /* General data */									 \
  double          vector[3];				 \
  int             alphaTag;						 \
  int             vectorTag;						 \

	struct Ppc_a_Vector { __Ppc_a_Vector };
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_LINEAR_DEFARGS \
                PPC_DEFARGS

	#define PPC_LINEAR_PASSARGS \
                PPC_PASSARGS

	Ppc_a_Vector* _Ppc_a_Vector_New(  PPC_LINEAR_DEFARGS  );
	
	void _Ppc_a_Vector_Delete( void* _self );
	void _Ppc_a_Vector_Print( void* _self, Stream* stream );
	void* _Ppc_a_Vector_DefaultNew( Name name );
   void _Ppc_a_Vector_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_a_Vector_Build( void* _self, void* data );
	void _Ppc_a_Vector_Initialise( void* _self, void* data );
	void _Ppc_a_Vector_Execute( void* _self, void* data );
	void _Ppc_a_Vector_Destroy( void* _self, void* data );

   /* Public functions */
   int _Ppc_a_Vector_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

