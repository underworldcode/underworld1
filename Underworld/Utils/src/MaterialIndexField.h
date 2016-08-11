/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Monash Cluster Computing, Australia
** (C) 2003-2004 All Rights Reserved
**
** Primary Authors:
** Robert Turnbull, MCC
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __Underworld_Utils_MaterialIndexField_h__
#define __Underworld_Utils_MaterialIndexField_h__
	
	/** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
	extern const Type MaterialIndexField_Type;

	/** MaterialIndexField class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
	#define __MaterialIndexField \
		/* Macro defining parent goes here - This means you can cast this class as its parent */ \
		__ParticleFeVariable \
		\
		/* Virtual functions go here */ \
		\
		/* Passed in parameters */ \
		Variable_Register*       variable_Register;
		
	struct MaterialIndexField { __MaterialIndexField };
	
	/* --- Contstructors / Destructors --- */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define MATERIALINDEXFIELD_DEFARGS \
                PARTICLEFEVARIABLE_DEFARGS

	#define MATERIALINDEXFIELD_PASSARGS \
                PARTICLEFEVARIABLE_PASSARGS

	MaterialIndexField* _MaterialIndexField_New(  MATERIALINDEXFIELD_DEFARGS  );
	
	/** Print the contents of an MaterialIndexField construct */
	void* _MaterialIndexField_DefaultNew( Name name );
	void _MaterialIndexField_Delete( void* variable );
	void _MaterialIndexField_Print( void* variable, Stream* stream );
	void* _MaterialIndexField_Copy( void* feVariable, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );

	void _MaterialIndexField_AssignFromXML( void* variable, Stg_ComponentFactory* cf, void* data ) ;
	void _MaterialIndexField_Build( void* variable, void* data ) ;
	void _MaterialIndexField_Initialise( void* variable, void* data ) ;
	void _MaterialIndexField_Execute( void* variable, void* data ) ;
	void _MaterialIndexField_Destroy( void* variable, void* data ) ;
   void _MaterialIndexField_ValueAtParticle( void* materialIndexField, IntegrationPointsSwarm* swarm, Element_LocalIndex lElement_I, void* particle, double* materialIndex ) ;
	

#endif 

