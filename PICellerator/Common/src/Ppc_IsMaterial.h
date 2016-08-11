#ifndef __PICellerator_Common_Ppc_IsMaterial_h__
#define __PICellerator_Common_Ppc_IsMaterial_h__

/** Textual name of this class */
extern const Type Ppc_IsMaterial_Type;

/** Ppc_IsMaterial class contents */
#define __Ppc_IsMaterial						 \
  /* Parent info */								 \
  __Ppc												 \
  /* General data */							    \
  char*          materialName;				 \
  int            materialIndex;				 \

	struct Ppc_IsMaterial { __Ppc_IsMaterial };
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_ISMATERIAL_DEFARGS \
                PPC_DEFARGS

	#define PPC_ISMATERIAL_PASSARGS \
                PPC_PASSARGS

	Ppc_IsMaterial* _Ppc_IsMaterial_New(  PPC_ISMATERIAL_DEFARGS  );
	
	void _Ppc_IsMaterial_Delete( void* _self );
	void _Ppc_IsMaterial_Print( void* _self, Stream* stream );
	void* _Ppc_IsMaterial_DefaultNew( Name name );
   void _Ppc_IsMaterial_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_IsMaterial_Build( void* _self, void* data );
	void _Ppc_IsMaterial_Initialise( void* _self, void* data );
	void _Ppc_IsMaterial_Execute( void* _self, void* data );
	void _Ppc_IsMaterial_Destroy( void* _self, void* data );

   /* Public functions */
   int _Ppc_IsMaterial_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

