#ifndef __Underworld_Utils_BuoyancyForceTermPpc_h__
#define __Underworld_Utils_BuoyancyForceTermPpc_h__


	/** Textual name of this class */
	extern const Type BuoyancyForceTermPpc_Type;

 	/** BuoyancyForceTermPpc class contents */
	#define __BuoyancyForceTermPpc \
		/* Parent info */ \
		__ForceTerm \
		/* Param passed in */ \
		MaterialPointsSwarm*	materialPointsSwarm; \
		PpcManager* manager; \
		int density_id; \


	struct BuoyancyForceTermPpc { __BuoyancyForceTermPpc };

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define BUOYANCYFORCETERMPPC_DEFARGS \
                FORCETERM_DEFARGS

	#define BUOYANCYFORCETERMPPC_PASSARGS \
                FORCETERM_PASSARGS

	BuoyancyForceTermPpc* _BuoyancyForceTermPpc_New(  BUOYANCYFORCETERMPPC_DEFARGS  );
	
   void _BuoyancyForceTermPpc_Init( void* _self, PpcManager* mgr );
	void _BuoyancyForceTermPpc_Delete( void* _self );
	void _BuoyancyForceTermPpc_Print( void* _self, Stream* stream );
	void* _BuoyancyForceTermPpc_DefaultNew( Name name );
	void _BuoyancyForceTermPpc_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _BuoyancyForceTermPpc_Build( void* _self, void* data );
	void _BuoyancyForceTermPpc_Initialise( void* _self, void* data );
	void _BuoyancyForceTermPpc_Execute( void* _self, void* data );
	void _BuoyancyForceTermPpc_Destroy( void* _self, void* data );
 	void _BuoyancyForceTermPpc_AssembleElement( void* _self, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elForceVec );

#endif

