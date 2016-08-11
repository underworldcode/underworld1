/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**	Melbourne, 3053, Australia.
** Copyright (c) 2005-2010, Monash University, Building 28, Monash University Clayton Campus,
**	Victoria, 3800, Australia
**
** Primary Contributing Organisations:
**	Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**	AuScope - http://www.auscope.org
**	Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**
** Contributors:
**	Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**	Patrick D. Sunter, Software Engineer, VPAC. (patrick@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**	Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
**	Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**	Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**	David Stegman, Postdoctoral Fellow, Monash University. (david.stegman@sci.monash.edu.au)
**	Wendy Sharples, PhD Student, Monash University (wendy.sharples@sci.monash.edu.au)
**
**  This library is free software; you can redistribute it and/or
**  modify it under the terms of the GNU Lesser General Public
**  License as published by the Free Software Foundation; either
**  version 2.1 of the License, or (at your option) any later version.
**
**  This library is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public
**  License along with this library; if not, write to the Free Software
**  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include "types.h"
#include "ConductivityMaterialManager.h"
#include "HeatingForce.h"

#include <assert.h>
#include <string.h>

const Type HeatingForce_Type = (const Type)"HeatingForce";

/* Creation implementation / Virtual constructor */
HeatingForce* _HeatingForce_New(  HEATINGFORCE_DEFARGS  ) 
{
	HeatingForce* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(HeatingForce) );

	self = (HeatingForce*) _ParticleFeVariable_New(  PARTICLEFEVARIABLE_PASSARGS  );
	/* Virtual info */
	
	return self;
}

void _HeatingForce_Delete( void* heatingForce ) {
	HeatingForce* self = (HeatingForce*)heatingForce;

	_ParticleFeVariable_Delete( self );
}

void _HeatingForce_Print( void* heatingForce, Stream* stream ) {
	HeatingForce* self = (HeatingForce*)heatingForce;
	
	_ParticleFeVariable_Print( self, stream );
}

void* _HeatingForce_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                       _sizeOfSelf = sizeof(HeatingForce);
	Type                                                               type = HeatingForce_Type;
	Stg_Class_DeleteFunction*                                       _delete = _HeatingForce_Delete;
	Stg_Class_PrintFunction*                                         _print = _HeatingForce_Print;
	Stg_Class_CopyFunction*                                           _copy = NULL;
	Stg_Component_DefaultConstructorFunction*           _defaultConstructor = _HeatingForce_DefaultNew;
	Stg_Component_ConstructFunction*                             _construct = _HeatingForce_AssignFromXML;
	Stg_Component_BuildFunction*                                     _build = _HeatingForce_Build;
	Stg_Component_InitialiseFunction*                           _initialise = _HeatingForce_Initialise;
	Stg_Component_ExecuteFunction*                                 _execute = _HeatingForce_Execute;
	Stg_Component_DestroyFunction*                                 _destroy = _HeatingForce_Destroy;
	FieldVariable_InterpolateValueAtFunction*           _interpolateValueAt = _FeVariable_InterpolateValueAt;
	FieldVariable_GetCoordFunction*                _getMinAndMaxLocalCoords = _FeVariable_GetMinAndMaxLocalCoords;
	FieldVariable_GetCoordFunction*               _getMinAndMaxGlobalCoords = _FeVariable_GetMinAndMaxGlobalCoords;
	FeVariable_InterpolateWithinElementFunction*  _interpolateWithinElement = _FeVariable_InterpolateNodeValuesToElLocalCoord;
	FeVariable_GetValueAtNodeFunction*                      _getValueAtNode = _FeVariable_GetValueAtNode;
	ParticleFeVariable_ValueAtParticleFunction*            _valueAtParticle = (ParticleFeVariable_ValueAtParticleFunction*)_HeatingForce_ValueAtParticle;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType                             nameAllocationType = ZERO;
	FieldVariable_GetValueFunction*   _getMinGlobalFieldMagnitude = ZERO;
	FieldVariable_GetValueFunction*   _getMaxGlobalFieldMagnitude = ZERO;
	FieldVariable_CacheValuesFunction* _cacheMinMaxGlobalFieldMagnitude = ZERO;
	FeVariable_SyncShadowValuesFunc*            _syncShadowValues = ZERO;

	return (void*)_HeatingForce_New(  HEATINGFORCE_PASSARGS  );
}

void _HeatingForce_AssignFromXML( void* heatingForce, Stg_ComponentFactory* cf, void* data ) {
	HeatingForce*			self 	= (HeatingForce*)heatingForce;
	IntegrationPointsSwarm*		swarm;

	_FeVariable_AssignFromXML( self, cf, data );

	swarm = Stg_ComponentFactory_ConstructByKey( cf, self->name, "Swarm", IntegrationPointsSwarm, True, data );

	self->buoyancyMaterial = Stg_ComponentFactory_ConstructByKey( cf, self->name, "BuoyancyMaterial", BuoyancyMaterial, True, data );
	self->sle = Stg_ComponentFactory_ConstructByKey( cf, self->name, "SLE", SLE, True, data );
	self->materialIndex = Stg_ComponentFactory_GetUnsignedInt( cf, self->name, "materialIndex", 0 );
	self->cmm = Stg_ComponentFactory_ConstructByKey( cf, self->name, "MaterialsManager", ConductivityMaterialManager, False, data );

	/* need to do this evaluation BEFORE solve, since the heat field we derive will be used as a force term */
	EP_InsertClassHookBefore( Context_GetEntryPoint( self->context, self->sle->executeEPName ), "MatrixSetup", 
				  _ParticleFeVariable_Execute, self );

	_ParticleFeVariable_Init( self, swarm, False );
}

void _HeatingForce_Build( void* heatingForce, void* data ) {
	HeatingForce*	      	self	            = (HeatingForce*)heatingForce;
	unsigned		nDomainVerts;

	Stg_Component_Build( self->feMesh, data, False );
	nDomainVerts = Mesh_GetDomainSize( self->feMesh, MT_VERTEX );

	self->dataVariable = Variable_NewScalar( "dataVariable", self->context, Variable_DataType_Double, 
						 &nDomainVerts, NULL, (void**)&self->data, self->context->variable_Register );

	_ParticleFeVariable_Build( self, data );
	DofLayout_AddAllFromVariableArray( self->dofLayout, 1, &self->dataVariable );
}

void _HeatingForce_Initialise( void* heatingForce, void* data ) {
	HeatingForce*		self			= (HeatingForce*)heatingForce;

	if( self->cmm ) Stg_Component_Initialise( self->cmm, data, False );

	_ParticleFeVariable_Initialise( self, data );
}

void _HeatingForce_Execute( void* heatingForce, void* data ) {
	_ParticleFeVariable_Execute( heatingForce, data );
}

void _HeatingForce_Destroy( void* heatingForce, void* data ) {
	HeatingForce*		self			= (HeatingForce*)heatingForce;

	_ParticleFeVariable_Destroy( heatingForce, data );
}

void _HeatingForce_ValueAtParticle(
	void*				heatingForce, 
	IntegrationPointsSwarm*		swarm, 
	Element_LocalIndex		lElement_I, 
	IntegrationPoint*		particle,
	double*				value ) 
{
	HeatingForce*			self 		= (HeatingForce*)heatingForce;
	Material*			material;
	BuoyancyMaterial_MaterialExt*	materialExt;
	
	material = IntegrationPointsSwarm_GetMaterialOn( swarm, particle );
	materialExt = ExtensionManager_Get( material->extensionMgr, material, self->buoyancyMaterial->materialExtHandle );
	*value = materialExt->extensions[self->materialIndex];
}

