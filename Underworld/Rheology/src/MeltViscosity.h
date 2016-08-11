
#ifndef __Experimental_Rheology_MeltViscosity_h__
#define __Experimental_Rheology_MeltViscosity_h__

	/** Textual name of this class */
	extern const Type MeltViscosity_Type;
		
	/** Rheology class contents */
	#define __MeltViscosity                                              \
		__Rheology                                                         \
		/* Parameters */                        			                   \
      ParticleMelting *particleMelting; \
      PpcManager* ppcMgr; \
      int meltTag; \
		double   lowerlimit; \
		double   upperlimit; \
		double   dVis_dMelt; \
		double   minVisFac; \

	struct MeltViscosity { __MeltViscosity };
	
	/** Private Constructor */
   #ifndef ZERO
   #define ZERO 0
   #endif

#define MELTDEPENDENCE_DEFARGS \
   RHEOLOGY_DEFARGS

#define MELTDEPENDENCE_PASSARGS \
   RHEOLOGY_PASSARGS

   MeltViscosity* _MeltViscosity_New(  MELTDEPENDENCE_DEFARGS  );

   void* _MeltViscosity_DefaultNew( Name name );
   void  _MeltViscosity_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data );

   void _MeltViscosity_ModifyConstitutiveMatrix( 
      void*                                              rheology, 
      ConstitutiveMatrix*                                constitutiveMatrix,
      MaterialPointsSwarm*                               swarm,
      Element_LocalIndex                                 lElement_I,
      MaterialPoint*                                     materialPoint,
      Coord                                              xi );
#endif
