#ifndef __PICellerator_Common_Ppc_Constant_h__
#define __PICellerator_Common_Ppc_Constant_h__

/** Textual name of this class */
extern const Type Ppc_Constant_Type;

/** Ppc_Constant class contents */
      #define __Ppc_Constant \
		/* Parent info */ \
		__Ppc \
		/* General data */ \
		double                                      value; \


	struct Ppc_Constant { __Ppc_Constant };

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_CONSTANT_DEFARGS \
                PPC_DEFARGS

	#define PPC_CONSTANT_PASSARGS \
                PPC_PASSARGS

	Ppc_Constant* _Ppc_Constant_New(  PPC_CONSTANT_DEFARGS  );
	
	void _Ppc_Constant_Delete( void* _self );
	void _Ppc_Constant_Print( void* _self, Stream* stream );
	void* _Ppc_Constant_DefaultNew( Name name );
   void _Ppc_Constant_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_Constant_Build( void* _self, void* data );
	void _Ppc_Constant_Initialise( void* _self, void* data );
	void _Ppc_Constant_Execute( void* _self, void* data );
	void _Ppc_Constant_Destroy( void* _self, void* data );

   /* Public functions */
/** Simple Public constructor
    * no min/max is imposed and no particle storage is enabled */
   Ppc_Constant* Ppc_Constant_New(  Name name,
                                    PpcManager* mgr,
                                    double value );
   int _Ppc_Constant_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

