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
#include <PICellerator/PopulationControl/PopulationControl.h>
#include <PICellerator/Weights/Weights.h>


#include "MaterialPoints.h"

#include <assert.h>
#include <string.h>

const Type SwarmVariableField_Type = "SwarmVariableField";

/* Creation implementation / Virtual constructor */
SwarmVariableField* _SwarmVariableField_New(  SWARMVARIABLEFIELD_DEFARGS  ) 
{
	SwarmVariableField* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(SwarmVariableField) );

	self = (SwarmVariableField*) _ParticleFeVariable_New(  PARTICLEFEVARIABLE_PASSARGS  );
	/* Virtual info */
	
	return self;
}

void _SwarmVariableField_Init( SwarmVariableField* swarmVariableField, Variable_Register* variable_Register, Name swarmVarName, MaterialPointsSwarm*	materialSwarm ) {
	SwarmVariableField* 	self 	= (SwarmVariableField*)swarmVariableField;
	self->variable_Register = variable_Register;
	self->swarmVarName = swarmVarName;
	self->materialSwarm = materialSwarm;
}

void _SwarmVariableField_Delete( void* swarmVariableField ) {
	SwarmVariableField* self = (SwarmVariableField*)swarmVariableField;

	_ParticleFeVariable_Delete( self );
}

void _SwarmVariableField_Print( void* swarmVariableField, Stream* stream ) {
	SwarmVariableField* self = (SwarmVariableField*)swarmVariableField;
	
	_ParticleFeVariable_Print( self, stream );
}

void* _SwarmVariableField_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                       _sizeOfSelf = sizeof(SwarmVariableField);
	Type                                                               type = SwarmVariableField_Type;
	Stg_Class_DeleteFunction*                                       _delete = _SwarmVariableField_Delete;
	Stg_Class_PrintFunction*                                         _print = _SwarmVariableField_Print;
	Stg_Class_CopyFunction*                                           _copy = NULL;
	Stg_Component_DefaultConstructorFunction*           _defaultConstructor = _SwarmVariableField_DefaultNew;
	Stg_Component_ConstructFunction*                             _construct = _SwarmVariableField_AssignFromXML;
	Stg_Component_BuildFunction*                                     _build = _SwarmVariableField_Build;
	Stg_Component_InitialiseFunction*                           _initialise = _SwarmVariableField_Initialise;
	Stg_Component_ExecuteFunction*                                 _execute = _SwarmVariableField_Execute;
	Stg_Component_DestroyFunction*                                 _destroy = _SwarmVariableField_Destroy;
	FieldVariable_InterpolateValueAtFunction*           _interpolateValueAt = _FeVariable_InterpolateValueAt;
	FieldVariable_GetCoordFunction*                _getMinAndMaxLocalCoords = _FeVariable_GetMinAndMaxLocalCoords;
	FieldVariable_GetCoordFunction*               _getMinAndMaxGlobalCoords = _FeVariable_GetMinAndMaxGlobalCoords;
	FeVariable_InterpolateWithinElementFunction*  _interpolateWithinElement = _FeVariable_InterpolateNodeValuesToElLocalCoord;
	FeVariable_GetValueAtNodeFunction*                      _getValueAtNode = _FeVariable_GetValueAtNode;
	ParticleFeVariable_ValueAtParticleFunction*            _valueAtParticle = (ParticleFeVariable_ValueAtParticleFunction*)_SwarmVariableField_ValueAtParticle;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType                             nameAllocationType = ZERO;
	FieldVariable_GetValueFunction*   _getMinGlobalFieldMagnitude = ZERO;
	FieldVariable_GetValueFunction*   _getMaxGlobalFieldMagnitude = ZERO;
	FieldVariable_CacheValuesFunction* _cacheMinMaxGlobalFieldMagnitude = ZERO;
	FeVariable_SyncShadowValuesFunc*            _syncShadowValues = ZERO;

	return (void*)_SwarmVariableField_New(  SWARMVARIABLEFIELD_PASSARGS  );
}

void _SwarmVariableField_AssignFromXML( void* swarmVariableField, Stg_ComponentFactory* cf, void* data ) {
	SwarmVariableField*		self = (SwarmVariableField*)swarmVariableField;
	IntegrationPointsSwarm*	integrationSwarm;
	Variable_Register*		variable_Register;
   Name			            swarmVarName;
   MaterialPointsSwarm*	   materialSwarm;
	_ParticleFeVariable_AssignFromXML( self, cf, data );

	variable_Register = self->context->variable_Register; 

	// TODO: just get the textual name here - see gLucifer's SwarmPlotter DrawignObject 
	swarmVarName = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"swarmVariable", ""  );

	materialSwarm = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"MaterialSwarm", MaterialPointsSwarm, True, data  );
	integrationSwarm = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Swarm", IntegrationPointsSwarm, True, NULL );
	assert( integrationSwarm  );

	_SwarmVariableField_Init( self, variable_Register, swarmVarName, materialSwarm );
}

void _SwarmVariableField_Build( void* swarmVariableField, void* data ) {
	SwarmVariableField*	      self	            = (SwarmVariableField*)swarmVariableField;
	unsigned		               nDomainVerts      = Mesh_GetDomainSize( self->feMesh, MT_VERTEX );
	SwarmVariable_Register*		swarmVar_Register	= self->materialSwarm->swarmVariable_Register;
	Stream*				         errorStream		   = Journal_Register( Error_Type, (Name)self->type  );
	Name			               tmpName;

	Stg_Component_Build( self->materialSwarm, data, False );
	/* make this more flexible to handle vector values at each node - will have to get the num dofs from the XML
	 * as other components are not necessarily built yet... dave. 03.10.07 */
	tmpName = Stg_Object_AppendSuffix( self, (Name)"DataVariable"  );
	self->dataVariable = Variable_NewScalar( tmpName,
		(AbstractContext*)self->context,
		Variable_DataType_Double,
		&nDomainVerts,
		NULL,
		(void**)&self->data,
		self->variable_Register );
	Memory_Free( tmpName );
	self->fieldComponentCount = 1;

	tmpName = Stg_Object_AppendSuffix( self, (Name)"DofLayout"  );
	self->dofLayout = DofLayout_New( tmpName, self->context, self->variable_Register, 0, self->feMesh );

	/* must build before adding the variable to the dof layout, dave. 04.10.07 */
	//Stg_Component_Build( self->dofLayout, data, False );
	Stg_Component_Build( self->dofLayout->mesh, data, False );
	self->dofLayout->_numItemsInLayout = Mesh_GetDomainSize( self->dofLayout->mesh, MT_VERTEX );
	DofLayout_AddAllFromVariableArray( self->dofLayout, 1, &self->dataVariable );
	Memory_Free( tmpName );
	
	self->eqNum->dofLayout = self->dofLayout;

	/* assign the swarm variable, assume its already built */
	if( 0 != strcmp( self->swarmVarName, "" ) ) {
		self->swarmVar = SwarmVariable_Register_GetByName( swarmVar_Register, self->swarmVarName );
		Journal_Firewall( self->swarmVar != NULL, errorStream, "Error - cannot find swarm variable \"%s\" in %s() for swarm \"%s\".\n",
			          self->swarmVarName, __func__, self->materialSwarm->name );

		Stg_Component_Build( self->swarmVar, data, False );
	}

	_ParticleFeVariable_Build( self, data );
	/* TODO: build self->swarmVar */
	// TODO: granb self->SwarmVariableName out of the swarm vart register, save reference as self->swarmVar
	//_SwarmVariable_Build( self->swarmVar, data ); /* dave, 18.09.07 */
}

void _SwarmVariableField_Initialise( void* swarmVariableField, void* data ) {
	SwarmVariableField*		self			= (SwarmVariableField*)swarmVariableField;

   Stg_Component_Initialise( self->materialSwarm, data, False );

   if( self->swarmVar ) Stg_Component_Initialise( self->swarmVar, data, False );

	_ParticleFeVariable_Initialise( self, data );
}

void _SwarmVariableField_Execute( void* swarmVariableField, void* data ) {
	_ParticleFeVariable_Execute( swarmVariableField, data );
}

void _SwarmVariableField_Destroy( void* swarmVariableField, void* data ) {
	SwarmVariableField*		self			= (SwarmVariableField*)swarmVariableField;

   Stg_Component_Destroy( self->materialSwarm, data, False );
   if( self->swarmVar ) Stg_Component_Destroy( self->swarmVar, data, False );
   
	_ParticleFeVariable_Destroy( swarmVariableField, data );
}

void _SwarmVariableField_ValueAtParticle(
	void*							swarmVariableField, 
	IntegrationPointsSwarm*	swarm, 
	Element_LocalIndex		lElement_I, 
	IntegrationPoint*			particle,
	double*						value ) 
{
	SwarmVariableField*	self = (SwarmVariableField*)swarmVariableField;
	double					distance;
	Cell_Index				cell_I;
	Particle_InCellIndex	cParticle_I;
	Particle_Index			lParticle_I;

	cell_I = CellLayout_MapElementIdToCellId( swarm->cellLayout, lElement_I );
	cParticle_I = Swarm_FindClosestParticleInCell( swarm, cell_I, Mesh_GetDimSize( self->dofLayout->mesh ), particle->xi, &distance );

	// this function doesn't seem to be doing its joob properly!
	//lParticle_I = IntegrationPointMapper_GetMaterialIndexAt( swarm->mapper, swarm->cellParticleTbl[cell_I][cParticle_I] );
	
	/* assume that the material and intergation points swarms map 1:1 */
	lParticle_I = swarm->cellParticleTbl[cell_I][cParticle_I];
	
	
	SwarmVariable_ValueAt( self->swarmVar, lParticle_I, value ); /* does the copy inside this func. dave, 18.09.07 */
}

void _SwarmVariableField_GetValueAtNode( void* swarmVariableField, Node_DomainIndex dNode_I, double* value ) {
	FeVariable_GetValueAtNode( swarmVariableField, dNode_I, value );
}

/* implement these two functions later... */
/*
double _SwarmVariableField_GetMinGlobalMagnitude( void* swarmVariableField ) {
	return _SwarmVariable_GetMinGlobalMagnitude( swarmVariableField );
}
double _SwarmVariableField_GetMaxGlobalMagnitude( void* swarmVariableField ) {
	return _SwarmVariable_GetMaxGlobalMagnitude( swarmVariableField );
}
*/





