#ifndef __Underworld_Utils_AdiabaticHeatingTermPpc_h__
#define __Underworld_Utils_AdiabaticHeatingTermPpc_h__


extern const Type AdiabaticHeatingTermPpc_Type;

/** AdiabaticHeatingTermPpc class contents */
#define __AdiabaticHeatingTermPpc									\
  __ForceTerm																\
  /* AdiabaticHeatingTermPpc info */								\
  PpcManager*                       manager;						\
  int										 velocity_id;	            \
  int                             cpLabel;						\
  int                             alphaLabel;					\
  int                             temperatureLabel;			\


struct AdiabaticHeatingTermPpc { __AdiabaticHeatingTermPpc };

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define ADIABATICHEATINGTERMPPC_DEFARGS \
                FORCETERM_DEFARGS

	#define ADIABATICHEATINGTERMPPC_PASSARGS \
                FORCETERM_PASSARGS

	AdiabaticHeatingTermPpc* _AdiabaticHeatingTermPpc_New(  ADIABATICHEATINGTERMPPC_DEFARGS  );
	
	void _AdiabaticHeatingTermPpc_Delete( void* _self );
	void _AdiabaticHeatingTermPpc_Print( void* _self, Stream* stream );
	void* _AdiabaticHeatingTermPpc_DefaultNew( Name name );
	void _AdiabaticHeatingTermPpc_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _AdiabaticHeatingTermPpc_Build( void* _self, void* data );
	void _AdiabaticHeatingTermPpc_Initialise( void* _self, void* data );
	void _AdiabaticHeatingTermPpc_Execute( void* _self, void* data );
	void _AdiabaticHeatingTermPpc_Destroy( void* _self, void* data );
	void _AdiabaticHeatingTermPpc_AssembleElement( void* _self, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elForceVec ) ;

#endif

