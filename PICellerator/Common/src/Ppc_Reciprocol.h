#ifndef __PICellerator_Common_Ppc_Reciprocol_h__
#define __PICellerator_Common_Ppc_Reciprocol_h__

/** Textual name of this class */
extern const Type Ppc_Reciprocol_Type;

/** Ppc_Reciprocol class contents */
#define __Ppc_Reciprocol									 \
  /* Parent info */									 \
  __Ppc													 \
  /* General data */									 \
  int             alphaTag;						 \

	struct Ppc_Reciprocol { __Ppc_Reciprocol };

	Ppc_Reciprocol* Ppc_Reciprocol_New( Name name, Stg_Component* _self );
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_LINEAR_DEFARGS \
                PPC_DEFARGS

	#define PPC_LINEAR_PASSARGS \
                PPC_PASSARGS

	Ppc_Reciprocol* _Ppc_Reciprocol_New(  PPC_LINEAR_DEFARGS  );
	
	void _Ppc_Reciprocol_Delete( void* _self );
	void _Ppc_Reciprocol_Print( void* _self, Stream* stream );
	void* _Ppc_Reciprocol_DefaultNew( Name name );
   void _Ppc_Reciprocol_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_Reciprocol_Build( void* _self, void* data );
	void _Ppc_Reciprocol_Initialise( void* _self, void* data );
	void _Ppc_Reciprocol_Execute( void* _self, void* data );
	void _Ppc_Reciprocol_Destroy( void* _self, void* data );

   /* Public functions */
   int _Ppc_Reciprocol_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

