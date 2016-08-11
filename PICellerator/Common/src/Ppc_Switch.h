#ifndef __PICellerator_Common_Ppc_Switch_h__
#define __PICellerator_Common_Ppc_Switch_h__

/** Textual name of this class */
extern const Type Ppc_Switch_Type;

typedef struct {
  double constant;
  int  valueTag;
} Ppc_Switch_Case;

/** Ppc_Switch class contents */
#define __Ppc_Switch									\
  /* Parent info */									\
  __Ppc													\
  /* General data */									\
  Ppc_Switch_Case*  caseList;						\
  int               caseCount;					\
  char*             interpolation;				\
  int             fieldTag;						\

	struct Ppc_Switch { __Ppc_Switch };
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_SWITCH_DEFARGS \
                PPC_DEFARGS

	#define PPC_SWITCH_PASSARGS \
                PPC_PASSARGS

	Ppc_Switch* _Ppc_Switch_New(  PPC_SWITCH_DEFARGS  );
	
	void _Ppc_Switch_Delete( void* _self );
	void _Ppc_Switch_Print( void* _self, Stream* stream );
	void* _Ppc_Switch_DefaultNew( Name name );
   void _Ppc_Switch_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_Switch_Build( void* _self, void* data );
	void _Ppc_Switch_Initialise( void* _self, void* data );
	void _Ppc_Switch_Execute( void* _self, void* data );
	void _Ppc_Switch_Destroy( void* _self, void* data );


   /* Public functions */
	int _Ppc_Switch_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );
	
#endif

