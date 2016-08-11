#ifndef __PICellerator_Common_Ppc_IsInsideShape_h__
#define __PICellerator_Common_Ppc_IsInsideShape_h__

/** Textual name of this class */
extern const Type Ppc_IsInsideShape_Type;

/** Ppc_IsInsideShape class contents */
#define __Ppc_IsInsideShape						 \
  /* Parent info */								 \
  __Ppc												 \
  /* General data */							    \
  Stg_Shape* shape;				 \

	struct Ppc_IsInsideShape { __Ppc_IsInsideShape };
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_ISINSIDESHAPE_DEFARGS \
                PPC_DEFARGS

	#define PPC_ISINSIDESHAPE_PASSARGS \
                PPC_PASSARGS

	Ppc_IsInsideShape* _Ppc_IsInsideShape_New(  PPC_ISINSIDESHAPE_DEFARGS  );
	
	void _Ppc_IsInsideShape_Delete( void* _self );
	void _Ppc_IsInsideShape_Print( void* _self, Stream* stream );
	void* _Ppc_IsInsideShape_DefaultNew( Name name );
   void _Ppc_IsInsideShape_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_IsInsideShape_Build( void* _self, void* data );
	void _Ppc_IsInsideShape_Initialise( void* _self, void* data );
	void _Ppc_IsInsideShape_Execute( void* _self, void* data );
	void _Ppc_IsInsideShape_Destroy( void* _self, void* data );

   /* Public functions */
   int _Ppc_IsInsideShape_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

