
#ifndef __Underworld_Rheology_ViscosityLimiter_h__
#define __Underworld_Rheology_ViscosityLimiter_h__


	/** Textual name of this class */
	extern const Type ViscosityLimiter_Type;

		
	/** Rheology class contents */
	#define __ViscosityLimiter                                        \
		__Rheology                                                            \
		/* Parameters */                        			      \
		double                                              maxViscosity;     \
		double                                              minViscosity;    


	struct ViscosityLimiter { __ViscosityLimiter };
	
	/** Private Constructor */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define VISCOSITYLIMITER_DEFARGS \
                RHEOLOGY_DEFARGS

	#define VISCOSITYLIMITER_PASSARGS \
                RHEOLOGY_PASSARGS

	ViscosityLimiter* _ViscosityLimiter_New(  VISCOSITYLIMITER_DEFARGS  );

	void* _ViscosityLimiter_DefaultNew( Name name );
	void  _ViscosityLimiter_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data );

	void _ViscosityLimiter_ModifyConstitutiveMatrix( 
		void*                                              rheology, 
		ConstitutiveMatrix*                                constitutiveMatrix,
		MaterialPointsSwarm*                               swarm,
		Element_LocalIndex                                 lElement_I,
		MaterialPoint*                                     materialPoint,
		Coord                                              xi );
#endif

