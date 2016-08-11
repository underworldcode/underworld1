/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Monash Cluster Computing, Australia
** (C) 2003-2004 All Rights Reserved
**
** Primary Authors:
** Luke Hodkinson
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <mpi.h>

#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Rheology/Rheology.h>

#include "types.h"
#include "SmoothVelGradField.h"

const Type SmoothVelGradField_Type = "SmoothVelGradField";

/*
** Constructors/Destructors.
*/

SmoothVelGradField* _SmoothVelGradField_New(  SMOOTHVELGRADFIELD_DEFARGS  ) {
   SmoothVelGradField* self;

   assert( _sizeOfSelf >= sizeof(SmoothVelGradField) );
   self = (SmoothVelGradField*)_ParticleFeVariable_New(  PARTICLEFEVARIABLE_PASSARGS  );

   return self;
}

void _SmoothVelGradField_Init(
	SmoothVelGradField*		self,
	Variable_Register*		variable_Register,
	FeVariable*					velField,
	SystemLinearEquations*	sle)
{
   self->variable_Register = variable_Register;
   self->fieldComponentCount = 1;
   self->useDeriv = True;
   self->velField = velField;

   if( sle )
      SystemLinearEquations_AddPostNonLinearEP( sle, SmoothVelGradField_Type, SmoothVelGradField_NonLinearUpdate );
}

void* _SmoothVelGradField_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                       _sizeOfSelf = sizeof(SmoothVelGradField);
	Type                                                               type = SmoothVelGradField_Type;
	Stg_Class_DeleteFunction*                                       _delete = _SmoothVelGradField_Delete;
	Stg_Class_PrintFunction*                                         _print = _SmoothVelGradField_Print;
	Stg_Class_CopyFunction*                                           _copy = _SmoothVelGradField_Copy;
	Stg_Component_DefaultConstructorFunction*           _defaultConstructor = _SmoothVelGradField_DefaultNew;
	Stg_Component_ConstructFunction*                             _construct = _SmoothVelGradField_AssignFromXML;
	Stg_Component_BuildFunction*                                     _build = _SmoothVelGradField_Build;
	Stg_Component_InitialiseFunction*                           _initialise = _SmoothVelGradField_Initialise;
	Stg_Component_ExecuteFunction*                                 _execute = _SmoothVelGradField_Execute;
	Stg_Component_DestroyFunction*                                 _destroy = _SmoothVelGradField_Destroy;
	FieldVariable_InterpolateValueAtFunction*           _interpolateValueAt = _FeVariable_InterpolateValueAt;
	FieldVariable_GetCoordFunction*                _getMinAndMaxLocalCoords = _FeVariable_GetMinAndMaxLocalCoords;
	FieldVariable_GetCoordFunction*               _getMinAndMaxGlobalCoords = _FeVariable_GetMinAndMaxGlobalCoords;
	FeVariable_InterpolateWithinElementFunction*  _interpolateWithinElement = _FeVariable_InterpolateNodeValuesToElLocalCoord;
	FeVariable_GetValueAtNodeFunction*                      _getValueAtNode = _FeVariable_GetValueAtNode;
	ParticleFeVariable_ValueAtParticleFunction*            _valueAtParticle = _SmoothVelGradField_ValueAtParticle;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType                             nameAllocationType = ZERO;
	FieldVariable_GetValueFunction*   _getMinGlobalFieldMagnitude = ZERO;
	FieldVariable_GetValueFunction*   _getMaxGlobalFieldMagnitude = ZERO;
	FieldVariable_CacheValuesFunction* _cacheMinMaxGlobalFieldMagnitude = ZERO;
	FeVariable_SyncShadowValuesFunc*            _syncShadowValues = ZERO;

   return (void*) _SmoothVelGradField_New(  SMOOTHVELGRADFIELD_PASSARGS  );
}

void _SmoothVelGradField_Delete( void* _self ) {
   SmoothVelGradField* self = (SmoothVelGradField*) _self;

   _ParticleFeVariable_Delete( self );
}

/*
** Methods.
*/

void _SmoothVelGradField_Print( void* _self, Stream* stream ) {
   SmoothVelGradField* self = (SmoothVelGradField*)_self;

   /* General info */
   Journal_Printf( stream, "SmoothVelGradField (ptr): %p\n", self );

   /* Print parent */
   _ParticleFeVariable_Print( self, stream );
}

void* _SmoothVelGradField_Copy( void* _self, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
   abort();
}

void SmoothVelGradField_NonLinearUpdate( void* _sle, void* _ctx ) {
   SystemLinearEquations* sle;
   DomainContext* ctx = (DomainContext*)_ctx;
   FieldVariable_Register* fieldVar_Register;
   SmoothVelGradField* preVar;

	sle = (SystemLinearEquations*)_sle;
   fieldVar_Register = ctx->fieldVariable_Register;
   preVar = (SmoothVelGradField*)FieldVariable_Register_GetByName( fieldVar_Register, "VelocityGradientsField" );
   ParticleFeVariable_Update( preVar );
}

void _SmoothVelGradField_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ){
   SmoothVelGradField* self = (SmoothVelGradField*) _self;
   Variable_Register* variable_Register;
   SystemLinearEquations* sle;
   FeVariable* velField;

   /* Construct Parent */
   _ParticleFeVariable_AssignFromXML( self, cf, data );

   variable_Register = self->context->variable_Register;
   assert( variable_Register );

   velField = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"VelocityField", FeVariable, True, data  );

	/*
   ** If we're using this field for non-linear feedback, we'll need to update it in between
   ** non-linear iterations. */
   sle = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"SLE", SystemLinearEquations, False, data  );

   _SmoothVelGradField_Init( self, variable_Register, velField, sle );
}

void _SmoothVelGradField_Build( void* _self, void* data ) {
   SmoothVelGradField* self = (SmoothVelGradField*) _self;
   Name tmpName;
   Name variableName[9];
   Variable_Index variable_I;
   Node_DomainIndex  node_I;
   int dim;

   Stg_Component_Build( self->feMesh, data, False );
   Stg_Component_Build( self->velField, data, False );
   dim = Mesh_GetDimSize( self->feMesh );
   self->fieldComponentCount = dim * dim;

   /* Need some names for sub variables. */
   if( dim == 2 ) {
      variableName[0] = StG_Strdup( "vel_grad_xx" );
      variableName[1] = StG_Strdup( "vel_grad_xy" );
      variableName[2] = StG_Strdup( "vel_grad_yx" );
      variableName[3] = StG_Strdup( "vel_grad_yy" );
   }
   else {
      variableName[0] = StG_Strdup( "vel_grad_xx" );
      variableName[1] = StG_Strdup( "vel_grad_xy" );
      variableName[2] = StG_Strdup( "vel_grad_xz" );
      variableName[3] = StG_Strdup( "vel_grad_yx" );
      variableName[4] = StG_Strdup( "vel_grad_yy" );
      variableName[5] = StG_Strdup( "vel_grad_yz" );
      variableName[6] = StG_Strdup( "vel_grad_zx" );
      variableName[7] = StG_Strdup( "vel_grad_zy" );
      variableName[8] = StG_Strdup( "vel_grad_zz" );
   }

   /* Create Variable to store data */
   tmpName = Stg_Object_AppendSuffix( self, (Name)"DataVariable"  );
   self->dataVariable = Variable_NewVector(
      tmpName,
		(AbstractContext*)self->context,	
      Variable_DataType_Double, 
      self->fieldComponentCount,
      &((IGraph*)self->feMesh->topo)->remotes[MT_VERTEX]->nDomains, 
      NULL,
      (void**)&self->data, 
      self->variable_Register,
      variableName[0],
      variableName[1],
      variableName[2],
      variableName[3],
      variableName[4],
      variableName[5],
      variableName[6],
      variableName[7],
      variableName[8] );
   Memory_Free( tmpName );

   /* Create Dof Layout */
   tmpName = Stg_Object_AppendSuffix( self, (Name)"DofLayout"  );
   self->dofLayout = DofLayout_New( tmpName, self->context, self->variable_Register, 0, self->feMesh );
   self->dofLayout->_numItemsInLayout = FeMesh_GetNodeDomainSize( self->feMesh );
   for( variable_I = 0; variable_I < self->fieldComponentCount ; variable_I++ ) {
      self->dataVariableList[ variable_I ] = Variable_Register_GetByName( self->variable_Register, 
									  variableName[ variable_I ] );
      for( node_I = 0; node_I < FeMesh_GetNodeDomainSize( self->feMesh ); node_I++ )
         DofLayout_AddDof_ByVarName( self->dofLayout, variableName[variable_I], node_I );
		/* Free Name */
		Memory_Free( variableName[ variable_I ] );
   }
   Memory_Free( tmpName );
   self->eqNum->dofLayout = self->dofLayout;

   /* Build and Update all Variables that this component has created */
   Stg_Component_Build( self->dataVariable, data, False);
   Variable_Update( self->dataVariable );
   for( variable_I = 0; variable_I < self->fieldComponentCount ; variable_I++ ) {
      Stg_Component_Build( self->dataVariableList[ variable_I ], data, False);
      Variable_Update( self->dataVariableList[ variable_I ] );
   }

   _ParticleFeVariable_Build( self, data );

   /* Update again, just in case things were changed/reallocated when ICs loaded */
   Variable_Update( self->dataVariable );
   for( variable_I = 0; variable_I < self->fieldComponentCount ; variable_I++ )
      Variable_Update( self->dataVariableList[ variable_I ] );
}

void _SmoothVelGradField_Initialise( void* _self, void* data ) {
   SmoothVelGradField* self = (SmoothVelGradField*) _self;
   Variable_Index variable_I;

   Stg_Component_Initialise( self->velField, data, False );

   /* Initialise and Update all Variables that this component has created */
   Stg_Component_Initialise( self->dataVariable, data, False); Variable_Update( self->dataVariable );
   for( variable_I = 0; variable_I < self->fieldComponentCount ; variable_I++ ) {
      Stg_Component_Initialise( self->dataVariableList[ variable_I ], data, False);
      Variable_Update( self->dataVariableList[ variable_I ] );
   }
	
   _ParticleFeVariable_Initialise( self, data );

   /* Do a post-update just in case */
   Variable_Update( self->dataVariable );
   for( variable_I = 0; variable_I < self->fieldComponentCount ; variable_I++ )
      Variable_Update( self->dataVariableList[ variable_I ] );
}

void _SmoothVelGradField_Execute( void* _self, void* data ) {
   SmoothVelGradField* self = (SmoothVelGradField*) _self;

   _ParticleFeVariable_Execute( self, data );
}

void _SmoothVelGradField_Destroy( void* _self, void* data ) {
   SmoothVelGradField* self = (SmoothVelGradField*) _self;

   Stg_Component_Destroy( self->velField, data, False );

   _ParticleFeVariable_Destroy( self, data );
}

void _SmoothVelGradField_ValueAtParticle(
	void*							_self,
	IntegrationPointsSwarm*	swarm,
	Element_LocalIndex		lElement_I,
	void*							_particle,
	double*						velGrad )
{
   SmoothVelGradField* self = (SmoothVelGradField*)_self;

   assert( self->useDeriv );
   assert( self->GNx );

   FeVariable_InterpolateDerivatives_WithGNx( self->velField, lElement_I, self->GNx, velGrad );
}


