#ifndef __PICellerator_Common_Ppc_PressureCalibrate_h__
#define __PICellerator_Common_Ppc_PressureCalibrate_h__

/** Textual name of this class */
extern const Type Ppc_PressureCalibrate_Type;

/** Ppc_PressureCalibrate class contents */
#define __Ppc_PressureCalibrate               \
  /* Parent info */									 \
  __Ppc                                       \
  /* General data */                          \
  FeVariable*       pressureFeVariable; \
  Swarm*            ms;                 \
  SLE_Solver*       stokes_solver;       \
  double            offset;             \
  int               blp_tag;  /* blp := background lithostatic pressure */

	struct Ppc_PressureCalibrate { __Ppc_PressureCalibrate };
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_LITHOSTATICPRESSURE_DEFARGS \
                PPC_DEFARGS

	#define PPC_LITHOSTATICPRESSURE_PASSARGS \
                PPC_PASSARGS

	Ppc_PressureCalibrate* _Ppc_PressureCalibrate_New(  PPC_LITHOSTATICPRESSURE_DEFARGS  );
	
	void _Ppc_PressureCalibrate_Delete( void* _self );
	void _Ppc_PressureCalibrate_Print( void* _self, Stream* stream );
	void* _Ppc_PressureCalibrate_DefaultNew( Name name );
   void _Ppc_PressureCalibrate_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_PressureCalibrate_Build( void* _self, void* data );
	void _Ppc_PressureCalibrate_Initialise( void* _self, void* data );
	void _Ppc_PressureCalibrate_Execute( void* _self, void* data );
	void _Ppc_PressureCalibrate_Destroy( void* _self, void* data );
   void _Ppc_PressureCalibrate_GetAndStorePressure( Ppc_PressureCalibrate* self, void* data);

   /* Public functions */
   int _Ppc_PressureCalibrate_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

