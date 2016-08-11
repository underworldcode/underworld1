#ifndef __PICellerator_ParticleMelting_h__
#define __PICellerator_ParticleMelting_h__
	
/* implemetation of the melting model of de Smet, van den Berg and Vlaar, Lithos, vol 48, 153-170, 1999 */


	/** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
	extern const Type ParticleMelting_Type;

	typedef double (ParticleMelting_GetMeltFractionFunc) ( double temp, double pressure ) ;

	typedef struct { 
		int TurnMeltOn;
		double lhf;
	 	double Cp;
	 	double sCoeff[4]; /* coefficients for solidus temperature polynomial */
	 	double lCoeff[4]; /* coefficients for liquidus temperature polynomial */
	   double deepPressure; /* pressure to change from deep polynomials*/
	 	double sCoeffDeep[4]; /* coefficients for solidus temperature polynomial */
	 	double lCoeffDeep[4]; /* coefficients for liquidus temperature polynomial */
	} ParticleMelting_MatExt;

	typedef struct { 
  	   double melt;
		double F;
	 	double prevF;
	   double  maxTheta;
	} ParticleMelting_ParExt;
	
	/** ParticleMelting class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
	#define __ParticleMelting \
		/* Parent macro */ \
		__Stg_Component \
		\
		PICelleratorContext*	context; \
		/* Virtual functions */ \
		ParticleMelting_GetMeltFractionFunc*   _getMeltFraction; \
		\
		/* other information */						  \
		FeVariable* temperatureField;				  \
		FeVariable* pressureField;					  \
		MaterialPointsSwarm* materialSwarm;		  \
      Scaling*       scaling;						  \
		unsigned       materialExtHandle;		  \
		unsigned       particleExtHandle;		  \
		Bool           extractMelt;	           \
		SwarmVariable* meltVariable;				  \
		SwarmVariable* FVariable;					  \
		SwarmVariable* prevFVariable;				  \
		SwarmVariable* maxThetaVariable;			  \
		double         refRho;						  \
		double         gravity;						  \
		double         maxdMelt_dt;						  \
		
	struct ParticleMelting { __ParticleMelting };
	
	/* --- Contstructors / Destructors --- */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PARTICLEMELTING_DEFARGS \
                STG_COMPONENT_DEFARGS

	#define PARTICLEMELTING_PASSARGS \
                STG_COMPONENT_PASSARGS

	ParticleMelting* _ParticleMelting_New(  PARTICLEMELTING_DEFARGS  );
	
	/** Print the contents of an ParticleMelting construct */
	void* _ParticleMelting_DefaultNew( Name name );
	void _ParticleMelting_Delete( void* variable );
	void _ParticleMelting_Print( void* variable, Stream* stream );
	void _ParticleMelting_AssignFromXML( void* variable, Stg_ComponentFactory* cf, void* data ) ;
	void _ParticleMelting_Build( void* variable, void* data ) ;
	void _ParticleMelting_Initialise( void* variable, void* data ) ;
	void _ParticleMelting_Execute( void* variable, void* data ) ;
	void _ParticleMelting_Destroy( void* variable, void* data ) ;

   /** Hook to stick on particle update EP */
   void ParticleMelting_MeltFrationUpdate( ParticleMelting *self, void *data );

   /** Returns Depletion as a double.
    * Input is dimensional pressure and dimensional temperature */
   double ParticleMelting_CalculateSuperSolidusTemperature( double p , double t, double *sc, double *lc );
   double ParticleMelting_CalculateMeltFraction( double ss  );


   /** Function for retriving values off a material point */
   int ParticleMelting_Get( void* _self, MaterialPoint *mp, ParticleMeltingProps prop, double *value );

#endif 

