/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Monash Cluster Computing, Australia
** (C) 2003-2004 All Rights Reserved
**
** Primary Authors:
** Robert Turnbull, MCC
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include <Underworld/Rheology/Rheology.h>

#include "types.h"
#include "MaterialIndexField.h"
#include <assert.h>

const Type MaterialIndexField_Type = "MaterialIndexField";

MaterialIndexField* _MaterialIndexField_New(  MATERIALINDEXFIELD_DEFARGS  ) {
	MaterialIndexField* self;
	
	/* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree.
	At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
	assert( _sizeOfSelf >= sizeof(MaterialIndexField) );
	self = (MaterialIndexField*) _ParticleFeVariable_New(  PARTICLEFEVARIABLE_PASSARGS  );
	
	return self;
}

void _MaterialIndexField_Init( MaterialIndexField* self, Variable_Register* variable_Register ) {
	/* Assign Pointers */
	self->variable_Register = variable_Register;
	
	/* Set pointers to swarm to be the same as the one on the constitutive matrix */
}

/* --- Virtual Function Implementations --- */
void _MaterialIndexField_Delete( void* materialIndexField ) {
	MaterialIndexField* self = (MaterialIndexField*) materialIndexField;

	_ParticleFeVariable_Delete( self );
}

void _MaterialIndexField_Print( void* materialIndexField, Stream* stream ) {
	MaterialIndexField* self = (MaterialIndexField*) materialIndexField;
	
	/* General info */
	Journal_Printf( stream, "MaterialIndexField (ptr): %p\n", self );
	
	/* Print parent */
	_FeVariable_Print( self, stream );
	
}

void* _MaterialIndexField_Copy( void* feVariable, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
	MaterialIndexField*	self = (MaterialIndexField*)feVariable;
	MaterialIndexField*	newMaterialIndexField;
	
	newMaterialIndexField = (MaterialIndexField*) _FeVariable_Copy( feVariable, dest, deep, nameExt, ptrMap );
	
	return (void*)newMaterialIndexField;
}

void* _MaterialIndexField_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                       _sizeOfSelf = sizeof(MaterialIndexField);
	Type                                                               type = MaterialIndexField_Type;
	Stg_Class_DeleteFunction*                                       _delete = _MaterialIndexField_Delete;
	Stg_Class_PrintFunction*                                         _print = _MaterialIndexField_Print;
	Stg_Class_CopyFunction*                                           _copy = _MaterialIndexField_Copy;
	Stg_Component_DefaultConstructorFunction*           _defaultConstructor = _MaterialIndexField_DefaultNew;
	Stg_Component_ConstructFunction*                             _construct = _MaterialIndexField_AssignFromXML;
	Stg_Component_BuildFunction*                                     _build = _MaterialIndexField_Build;
	Stg_Component_InitialiseFunction*                           _initialise = _MaterialIndexField_Initialise;
	Stg_Component_ExecuteFunction*                                 _execute = _MaterialIndexField_Execute;
	Stg_Component_DestroyFunction*                                 _destroy = _MaterialIndexField_Destroy;
	FieldVariable_InterpolateValueAtFunction*           _interpolateValueAt = _FeVariable_InterpolateValueAt;
	FieldVariable_GetCoordFunction*                _getMinAndMaxLocalCoords = _FeVariable_GetMinAndMaxLocalCoords;
	FieldVariable_GetCoordFunction*               _getMinAndMaxGlobalCoords = _FeVariable_GetMinAndMaxGlobalCoords;
	FeVariable_InterpolateWithinElementFunction*  _interpolateWithinElement = _FeVariable_InterpolateNodeValuesToElLocalCoord;
	FeVariable_GetValueAtNodeFunction*                      _getValueAtNode = _FeVariable_GetValueAtNode;
	ParticleFeVariable_ValueAtParticleFunction*            _valueAtParticle = _MaterialIndexField_ValueAtParticle;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType                             nameAllocationType = ZERO;
	FieldVariable_GetValueFunction*   _getMinGlobalFieldMagnitude = ZERO;
	FieldVariable_GetValueFunction*   _getMaxGlobalFieldMagnitude = ZERO;
	FieldVariable_CacheValuesFunction* _cacheMinMaxGlobalFieldMagnitude = ZERO;
	FeVariable_SyncShadowValuesFunc*            _syncShadowValues = ZERO;

	return (void*) _MaterialIndexField_New(  MATERIALINDEXFIELD_PASSARGS  );
}

void _MaterialIndexField_AssignFromXML( void* materialIndexField, Stg_ComponentFactory* cf, void* data ){
	MaterialIndexField*			self = (MaterialIndexField*) materialIndexField;
	Variable_Register*	variable_Register;

	_ParticleFeVariable_AssignFromXML( self, cf, data );

   variable_Register = self->context->variable_Register;
	assert( variable_Register );

   _MaterialIndexField_Init( self, variable_Register );
}

void _MaterialIndexField_Build( void* materialIndexField, void* data ) {
	MaterialIndexField*			self = (MaterialIndexField*) materialIndexField;
	Variable_Register*	variable_Register = (Variable_Register*) self->variable_Register;
	Name						tmpName;

  	/* Create Dof Layout */
   tmpName = Stg_Object_AppendSuffix( self, (Name)"materialIndexVariable" );
	self->dataVariable = Variable_NewScalar( tmpName, (AbstractContext*)self->context, Variable_DataType_Double, (Index*)&((IGraph*)self->feMesh->topo)->remotes[MT_VERTEX]->nDomains, NULL, (void**)&self->data, variable_Register );
	Memory_Free( tmpName  );
	self->fieldComponentCount = 1;
	
   tmpName = Stg_Object_AppendSuffix( self, (Name)"materialIndexDOF" );
	self->dofLayout = DofLayout_New( tmpName, self->context, variable_Register, 0, self->feMesh );
	self->dofLayout->_numItemsInLayout = FeMesh_GetNodeDomainSize( self->feMesh );
	DofLayout_AddAllFromVariableArray( self->dofLayout, 1, &self->dataVariable );
	Memory_Free( tmpName );
	self->eqNum->dofLayout = self->dofLayout;

	_ParticleFeVariable_Build( self, data );
}
void _MaterialIndexField_Initialise( void* materialIndexField, void* data ) {
	MaterialIndexField* self = (MaterialIndexField*) materialIndexField;

	_ParticleFeVariable_Initialise( self, data );
}
void _MaterialIndexField_Execute( void* materialIndexField, void* data ) {
	MaterialIndexField* self = (MaterialIndexField*) materialIndexField;

	_ParticleFeVariable_Execute( self, data );
}
void _MaterialIndexField_Destroy( void* materialIndexField, void* data ) {
	MaterialIndexField* self = (MaterialIndexField*) materialIndexField;

	_ParticleFeVariable_Destroy( self, data );
}

void _MaterialIndexField_ValueAtParticle( void* materialIndexField, IntegrationPointsSwarm* swarm, Element_LocalIndex lElement_I, void* _particle, double* materialIndex ) {
	MaterialIndexField*                    self = (MaterialIndexField*) materialIndexField;
	IntegrationPoint*                particle = (IntegrationPoint*) _particle;
	Material*                        material = NULL;
	
	material = IntegrationPointsSwarm_GetMaterialOn( (IntegrationPointsSwarm*)swarm, particle );
   *materialIndex = (double)material->index;
}



