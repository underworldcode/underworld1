

#ifndef __Underworld_Utils_ViscousHeatingTermPpc_h__
#define __Underworld_Utils_ViscousHeatingTermPpc_h__

/** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
extern const Type ViscousHeatingTermPpc_Type;

/** ViscousHeatingTermPpc class contents - this is defined as a macro so that sub-classes of this class can use this macro at the start of the definition of their struct */
#define __ViscousHeatingTermPpc \
		/* Macro defining parent goes here - This means you can cast this class as its parent */ \
		__ForceTerm \
		\
		/* Virtual functions go here */ \
		\
		/* ViscousHeatingTermPpc info */ \
		PpcManager*     mgr; \
		FeVariable*     stressField; \
		FeVariable*     strainRateField; \
      int           densityLabel; \
      int           cpLabel; \
 
struct ViscousHeatingTermPpc
{
   __ViscousHeatingTermPpc
};


#ifndef ZERO
#define ZERO 0
#endif

#define VISCOUSHEATINGTERMTP_DEFARGS \
                FORCETERM_DEFARGS

#define VISCOUSHEATINGTERMTP_PASSARGS \
                FORCETERM_PASSARGS

ViscousHeatingTermPpc* _ViscousHeatingTermPpc_New(  VISCOUSHEATINGTERMTP_DEFARGS  );

void _ViscousHeatingTermPpc_Init(
   void*          forceTerm,
   PpcManager*    mgr,
   FeVariable*    stressField,
   FeVariable*    strainRateField,
   int          densityLabel,
   int          cpLabel );

void _ViscousHeatingTermPpc_Delete( void* forceTerm );

void* _ViscousHeatingTermPpc_DefaultNew( Name name ) ;

void _ViscousHeatingTermPpc_AssignFromXML( void* forceTerm, Stg_ComponentFactory* cf, void* data ) ;

void _ViscousHeatingTermPpc_Build( void* forceTerm, void* data ) ;

void _ViscousHeatingTermPpc_Initialise( void* forceTerm, void* data ) ;

void _ViscousHeatingTermPpc_Execute( void* forceTerm, void* data ) ;

void _ViscousHeatingTermPpc_Destroy( void* forceTerm, void* data ) ;

void _ViscousHeatingTermPpc_AssembleElement( void* forceTerm, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elForceVec ) ;

#endif

