#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "MineralPhase.h"
#include "LookupTable.h"
#include "Ppc.h"


/* Textual name of this class */
const Type Ppc_Type = "Ppc";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc* _Ppc_New(  PPC_DEFARGS  )
{
	Ppc* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc*) _Stg_Component_New(  STG_COMPONENT_PASSARGS  );	

   self->_get = _get;
	return self;
}


void* _Ppc_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc);
	Type                                                      type = Ppc_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_Get;

	return (void*)_Ppc_New(  PPC_PASSARGS  );
}


void _Ppc_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	Ppc* self = (Ppc*)_self;
	Dictionary*	theDictionary = NULL;
	PpcManager* mgr = NULL;
	double min, max;
	Bool impose;

	/* The dictionary */ 
	theDictionary = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );
							
	/* The component factory */							
	self->cf = cf;

	/* The PpcManager */
	mgr = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Manager", PpcManager, False, data );
	if( !mgr  )
		mgr = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_ppcManager", PpcManager, True, data  );

	/* Threshold values */
   min = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"MinValue", HUGE_VAL );
   max = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"MaxValue", HUGE_VAL );
	impose = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"ImposeMinMax", 0 );

	/* Init */
	_Ppc_Init( self, mgr, min, max, impose );
}


void _Ppc_Init( void* _self, PpcManager* mgr, double min, double max, Bool impose )
{
	Ppc* self = (Ppc*)_self;

	self->manager = mgr;
	self->context = mgr->context;
	self->useMin = min != HUGE_VAL;
	self->useMax = max != HUGE_VAL;
	self->minValue = min;
	self->maxValue = max;
	self->imposeThreshold = impose;

	self->info_stream = Journal_Register( Info_Type, (Name)self->type  );
	self->error_stream = Journal_Register( Error_Type, (Name)self->type  );

	PpcManager_AddSolePpcToList( mgr, self, self->name );

	/* lets also add it as a standard condition function */
	ConditionFunction_Register_Add( condFunc_Register, 
		                               ConditionFunction_New2( _Ppc_VariableConditionFunction,
	                                                       _Ppc_Initialise_CF,
	                                                       self->name, 
	                                                       _self ) 
		                               );

}


void _Ppc_Build( void* _self, void* data ) {}


void _Ppc_Initialise( void* _self, void* data ) {
   Ppc* self = (Ppc*) _self;

   if( self->manager ) 
	  Stg_Component_Initialise( self->manager, data, False );
}

void _Ppc_Initialise_CF( void* _self, void* data ) {
   Ppc* self = (Ppc*) _self;

  Stg_Component_Build( self, data, False );
  Stg_Component_Initialise( self, data, False );
}


void _Ppc_Delete( void* _self ) {
   _Stg_Component_Delete( _self );
}

void _Ppc_Print( void* _self, Stream* stream ) {}

void _Ppc_Execute( void* _self, void* data ) {}

void _Ppc_Destroy( void* _self, void* data ) {}

/* 
 * Public functions 
 *
 */
void Ppc_CheckLimits( void *_self, double value ) {
  Ppc* self = (Ppc*) _self;
  Stream* info = self->info_stream;

  if( self->useMin && value < self->minValue )
	 Journal_Printf( info, "Value (%g) lesser than minimum (%g) in %s (%s)\n", value, self->minValue, self->name, self->type );

  if( self->useMax && value > self->maxValue )
	 Journal_Printf( info, "Value (%g) greater than maximum (%g) in %s (%s)\n", value, self->minValue, self->name, self->type );
}


double Ppc_ImposeLimits( void *_self, double value ) {
  Ppc* self = (Ppc*) _self;
  double res = value;

  if( self->useMin && value < self->minValue )
	 res = self->minValue;

  if( self->useMax && value > self->maxValue )
	 res = self->maxValue;

  return res;
}


int Ppc_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double *result ) {
	Ppc* self = (Ppc*) _self;
	int err;

	Journal_Firewall( (Bool) self, 
							self->error_stream, "\n\n\n"
							"Error in Ppc_Get (%s): the Ppc is NULL\n"
							"\n\n\n", self->name );

	err = self->_get( self, lElement_I, particle, result );

   /* in debug mode check if the result[0] is not a nan and not infinite */
   assert( isnan(result[0]) == 0 || isinf(result[0] != 0 ));

	if( self->imposeThreshold )
	  result[0] = Ppc_ImposeLimits( self, result[0] );
	else
	  Ppc_CheckLimits( self, result[0] );

	return err;
}


int _Ppc_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
	Ppc* self = (Ppc*) _self;
 
	Journal_Printf( self->error_stream, "Error in func %s for %s '%s' - "
						 "This function is the default function which should never be called - "
						 "Please set this virtual function with appropriate application dependent function.\n",
						 __func__, self->type, self->name );
	abort();

	return -1;
}

void _Ppc_GetValueFromFeVariable( void* _self, FeVariable* field, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
	FeMesh* mesh = field->feMesh;

	FeVariable_InterpolateFromMeshLocalCoord( field, mesh, lElement_I, particle->xi, result );
}

void _Ppc_VariableConditionFunction( Index node_lI, Variable_Index var_I, void* context, void* _data, void* result ){
	Ppc* self = (Ppc*) _data;
	IntegrationPoint intPoint;
	double* coordGlobal;
	int* elements;
	IArray* inc;

	/* this is uggggly */
    inc = IArray_New();
	FeMesh_GetNodeElements( self->manager->mesh, node_lI, inc );
	coordGlobal = Mesh_GetVertex( self->manager->mesh, node_lI );
	elements = IArray_GetPtr( inc );

	/* just use first element this node belongs to */
	FeMesh_CoordGlobalToLocal( self->manager->mesh, elements[0], coordGlobal, &(intPoint.xi) );
	/* to be safe, lets ensure the local coords are valid and at element extremes, and then thou shalt fear no evil */
	int ii;
	for(ii=0; ii<3; ii++){
		if(      intPoint.xi[ii] >  0.999)
			intPoint.xi[ii] =  1.0;
		else if (intPoint.xi[ii] < -0.999)
			intPoint.xi[ii] = -1.0;
	}

	Ppc_Get( _data, elements[0], &intPoint, result );
	Stg_Class_Delete( inc );


}