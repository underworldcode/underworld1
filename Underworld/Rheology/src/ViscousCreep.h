#ifndef __Underworld_Rheology_ViscousCreep_h__
#define __Underworld_Rheology_ViscousCreep_h__

	/** Textual name of this class */
	extern const Type ViscousCreep_Type;
		
	/** Rheology class contents */
	#define __ViscousCreep \
		/* Macro defining parent goes here */ \
		__Rheology \
		/* Virtual functions go here */ \
		/* Material Parameters */\
		int                                               strainRateInvTag; \
		int                                               temperatureLabel; \
		int                                               pressureLabel; \
		double                                            A_dis; \
		double                                            E_dis; \
		double                                            V_dis; \
		double                                            A_diff; \
		double                                            E_diff; \
		double                                            V_diff; \
		double                                            n_dis; \
		int                                               grainSizeLabel; \
		double                                            m_diff; \
		double                                            stress_peierls; \
		double                                            peierls_H; \
		double                                            peierls_p; \
		double                                            peierls_q; \
		double                                            A_p; \
                double                                            shear_modulus; \
                double                                            burgers_vector; /* Gerya book, p81 calls it this */ \
                double                                            peierls_lower_threshold_pressure; \
                double                                            peierls_lower_threshold_temperature; \
                double                                            peierls_upper_threshold_temperature; \
                double                                            defaultStrainRateInvariant; \
                Bool                                              enable_diffusion; \
                Bool                                              enable_dislocation; \
                Bool                                              enable_peierls; \
                Bool                                              enable_eta_limit; \
                Bool                                              no_peierls_NR; \
                double                                            limit_max; \
                double                                            limit_min; \
         /* silly extension stuff to visualise the rheological mechanism */ \
         SwarmVariable*                                      mechanismType;                        \
         ExtensionInfo_Index                                 particleExtHandle;                     \


	struct ViscousCreep { __ViscousCreep };
	
	/** Private Constructor: This will accept all the virtual functions for this class as arguments. */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define VISCOUSCREEP_DEFARGS \
                RHEOLOGY_DEFARGS

	#define VISCOUSCREEP_PASSARGS \
                RHEOLOGY_PASSARGS

	ViscousCreep* _ViscousCreep_New(  VISCOUSCREEP_DEFARGS  );

	/* 'Stg_Component' implementations */
	void* _ViscousCreep_DefaultNew( Name name ) ;
	void _ViscousCreep_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data );
   void _ViscousCreep_Build( void* _self, void* data );
   void _ViscousCreep_Initialise( void* _self, void* data );
   void _ViscousCreep_Destroy( void* _self, void* data );
   
	void _ViscousCreep_ModifyConstitutiveMatrix( 
		void*                                              rheology, 
		ConstitutiveMatrix*                                constitutiveMatrix,
		MaterialPointsSwarm*                               swarm,
		Element_LocalIndex                                 lElement_I,
		MaterialPoint*                                     materialPoint,
		Coord                                              xi );
#endif

