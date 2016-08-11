#ifndef __PICellerator_Common_Ppc_Condition_h__
#define __PICellerator_Common_Ppc_Condition_h__

/** Textual name of this class */
extern const Type Ppc_Condition_Type;

/** Ppc_Condition class contents */
#define __Ppc_Condition								 \
  /* Parent info */									 \
  __Ppc													 \
  /* General data */									 \
  int             fieldTag;						 \
  int             valueTag;						 \
  int             valueTrueTag;				 \
  int             valueFalseTag;				 \
  char*             condition;					 \
  
	struct Ppc_Condition { __Ppc_Condition };

	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_CONDITION_DEFARGS \
                PPC_DEFARGS

	#define PPC_CONDITION_PASSARGS \
                PPC_PASSARGS

	Ppc_Condition* _Ppc_Condition_New(  PPC_CONDITION_DEFARGS  );
	
	void _Ppc_Condition_Delete( void* _self );
	void _Ppc_Condition_Print( void* _self, Stream* stream );
	void* _Ppc_Condition_DefaultNew( Name name );
   void _Ppc_Condition_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_Condition_Build( void* _self, void* data );
	void _Ppc_Condition_Initialise( void* _self, void* data );
	void _Ppc_Condition_Execute( void* _self, void* data );
	void _Ppc_Condition_Destroy( void* _self, void* data );

   /* Public functions */
   int _Ppc_Condition_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

