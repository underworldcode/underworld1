#ifndef __PICellerator_Common_Ppc_Operation_h__
#define __PICellerator_Common_Ppc_Operation_h__

/** Textual name of this class */
extern const Type Ppc_Operation_Type;

/** Ppc_Operation class contents */
#define __Ppc_Operation				\
  /* Parent info */					\
  __Ppc									\
  /* General data */					\
  int*        propertyTagList;	\
  int         propertyTagCount;	\
  char*       operation;			\
  double*     values;	         \


	struct Ppc_Operation { __Ppc_Operation };

   Ppc_Operation* Ppc_Operation_New( Name name, PpcManager* mgr, int* tagList, int nPpc, char* operation ); 
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_OPERATION_DEFARGS \
                PPC_DEFARGS

	#define PPC_OPERATION_PASSARGS \
                PPC_PASSARGS

	Ppc_Operation* _Ppc_Operation_New(  PPC_OPERATION_DEFARGS  );
	
	void _Ppc_Operation_Delete( void* _self );
	void _Ppc_Operation_Print( void* _self, Stream* stream );
	void* _Ppc_Operation_DefaultNew( Name name );
   void _Ppc_Operation_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_Operation_Build( void* _self, void* data );
	void _Ppc_Operation_Initialise( void* _self, void* data );
	void _Ppc_Operation_Execute( void* _self, void* data );
	void _Ppc_Operation_Destroy( void* _self, void* data );


   /* Public functions */
	int _Ppc_Operation_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );
	
#endif

