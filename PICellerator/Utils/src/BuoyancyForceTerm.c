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
#include <PICellerator/MaterialPoints/MaterialPoints.h>

#include "types.h"
#include "BuoyancyForceTerm.h"
#include "HydrostaticTerm.h"
#include "MaterialSwarmVariable.h"

#include <assert.h>
#include <string.h>
#include <stddef.h>

/* Textual name of this class */
const Type BuoyancyForceTerm_Type = "BuoyancyForceTerm";

/* Creation implementation / Virtual constructor */
BuoyancyForceTerm* _BuoyancyForceTerm_New(  BUOYANCYFORCETERM_DEFARGS  )
{
	BuoyancyForceTerm* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(BuoyancyForceTerm) );
	/* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
	/* This means that any values of these parameters that are passed into this function are not passed onto the parent function
	   and so should be set to ZERO in any children of this class. */
	nameAllocationType = NON_GLOBAL;

	self = (BuoyancyForceTerm*) _ForceTerm_New(  FORCETERM_PASSARGS  );
	
	/* Virtual info */
	self->_calcGravity = _calcGravity;
	self->materialPointsSwarm = NULL;
	
	return self;
}

void _BuoyancyForceTerm_Delete( void* forceTerm ) {
	BuoyancyForceTerm* self = (BuoyancyForceTerm*)forceTerm;

	_ForceTerm_Delete( self );
}

void _BuoyancyForceTerm_Print( void* forceTerm, Stream* stream ) {
	BuoyancyForceTerm* self = (BuoyancyForceTerm*)forceTerm;
	
	_ForceTerm_Print( self, stream );

	/* General info */
	Journal_PrintPointer( stream, self->temperatureField );
	Journal_PrintDouble( stream, self->gravity );
}

void* _BuoyancyForceTerm_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(BuoyancyForceTerm);
	Type                                                      type = BuoyancyForceTerm_Type;
	Stg_Class_DeleteFunction*                              _delete = _BuoyancyForceTerm_Delete;
	Stg_Class_PrintFunction*                                _print = _BuoyancyForceTerm_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _BuoyancyForceTerm_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _BuoyancyForceTerm_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _BuoyancyForceTerm_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _BuoyancyForceTerm_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _BuoyancyForceTerm_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _BuoyancyForceTerm_Destroy;
	ForceTerm_AssembleElementFunction*            _assembleElement = _BuoyancyForceTerm_AssembleElement;
	BuoyancyForceTerm_CalcGravityFunction*            _calcGravity = _BuoyancyForceTerm_CalcGravity;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*)_BuoyancyForceTerm_New(  BUOYANCYFORCETERM_PASSARGS  );
}

void _BuoyancyForceTerm_AssignFromXML( void* forceTerm, Stg_ComponentFactory* cf, void* data ) {
	BuoyancyForceTerm*		self = (BuoyancyForceTerm*)forceTerm;
	Dictionary*				dict;
	unsigned				nDims;
	Dictionary_Entry_Value*	direcList;
	double*					direc;
	unsigned				d_i;
	PICelleratorContext*	context;

	/* Construct Parent */
	_ForceTerm_AssignFromXML( self, cf, data );

	dict = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );
	self->temperatureField = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"TemperatureField", FeVariable, False, data  ) ;
	self->gravity  = Stg_ComponentFactory_GetDouble(      cf, self->name, (Dictionary_Entry_Key)"gravity",        0.0  );
	self->vertAxis = Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"VerticalAxis",     1  );
	self->adjust   = Stg_ComponentFactory_GetBool(        cf, self->name, (Dictionary_Entry_Key)"adjust",       False  );
	/* Particle storage */
	self->storeDensityOnParticles = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"StoreDensityOnParticles", 0  );
	self->materialPointsSwarm = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"MaterialSwarm", MaterialPointsSwarm, False, data  );
	if( self->storeDensityOnParticles && !self->materialPointsSwarm )
	  self->materialPointsSwarm = Stg_ComponentFactory_ConstructByName( cf, (Name)"materialSwarm", MaterialPointsSwarm, True, data );

	direcList = Dictionary_Get( dict, (Dictionary_Entry_Key)"gravityDirection" );

	if( direcList ) {
		nDims = Dictionary_Entry_Value_GetCount( direcList  );
		direc = AllocArray( double, nDims );

		for( d_i = 0; d_i < nDims; d_i++ ) {
			Dictionary_Entry_Value*	tmp;
			char* rootKey;
			tmp = Dictionary_Entry_Value_GetElement( direcList, d_i );
			rootKey = Dictionary_Entry_Value_AsString( tmp );

			if( !Stg_StringIsNumeric( (char *)rootKey )  )
				tmp = Dictionary_Get( cf->rootDict, (Dictionary_Entry_Key)rootKey );
			direc[d_i] = Dictionary_Entry_Value_AsDouble( tmp );
		}
		if( nDims == 2  )
			Vec_Norm2D( direc, direc );
		else
			Vec_Norm3D( direc, direc );
	}
	else
		direc = NULL;
	self->gHat = direc;

	context = (PICelleratorContext*)self->context;
	assert( Stg_CheckType( context, PICelleratorContext ) );
	self->materials_Register = context->materials_Register;
	assert( self->materials_Register );

    /* Extend particle storage */
	if( self->storeDensityOnParticles  ) {
		BuoyancyForceTerm_ParticleExt* particleExt = NULL;
		StandardParticle particle;
	    SwarmVariable* materialPointsSwarmVariable = NULL;
	    self->particleExtHandle = ExtensionManager_Add( self->materialPointsSwarm->particleExtensionMgr, (Name)self->type, sizeof( BuoyancyForceTerm_ParticleExt )  );
	    particleExt = ExtensionManager_Get( self->materialPointsSwarm->particleExtensionMgr, &particle, self->particleExtHandle );	  
	    materialPointsSwarmVariable = Swarm_NewScalarVariable( self->materialPointsSwarm, (Name)"Density", (ArithPointer) &particleExt->density - (ArithPointer) &particle, Variable_DataType_Double );
	}
	self->hydrostaticTerm = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"HydrostaticTerm", HydrostaticTerm, False, data ) ;
}

void _BuoyancyForceTerm_Build( void* forceTerm, void* data ) {
	BuoyancyForceTerm*               self               = (BuoyancyForceTerm*)forceTerm;
	BuoyancyForceTerm_MaterialExt*   materialExt;
	Material_Index                   material_I;
	Material*                        material;
	Materials_Register*              materials_Register = self->materials_Register;
	IntegrationPointsSwarm*          swarm              = (IntegrationPointsSwarm*)self->integrationSwarm;
	MaterialPointsSwarm**            materialSwarms;
	Index                            materialSwarm_I;
	Name                             name;
	Stg_ComponentFactory*            cf;

	cf = self->context->CF;

	_ForceTerm_Build( self, data );

	if ( self->temperatureField )
		Stg_Component_Build( self->temperatureField, data, False );

	/* Sort out material extension stuff */
	self->materialExtHandle = Materials_Register_AddMaterialExtension( 
			self->materials_Register, 
			self->type, 
			sizeof(BuoyancyForceTerm_MaterialExt) );
	for ( material_I = 0 ; material_I < Materials_Register_GetCount( materials_Register ) ; material_I++) {
		material = Materials_Register_GetByIndex( materials_Register, material_I );
		materialExt = ExtensionManager_GetFunc( material->extensionMgr, material, self->materialExtHandle );

		materialExt->density = Stg_ComponentFactory_GetDouble( cf, material->name, (Dictionary_Entry_Key)"density", 0.0  );
		materialExt->alpha   = Stg_ComponentFactory_GetDouble( cf, material->name, (Dictionary_Entry_Key)"alpha", 0.0  );
		materialExt->refTemp = Stg_ComponentFactory_GetDouble( cf, material->name, (Dictionary_Entry_Key)"referenceTemperature", 0.0  );
		materialExt->beta   = Stg_ComponentFactory_GetDouble( cf, material->name, (Dictionary_Entry_Key)"beta", 0.0  );
		materialExt->refDepth   = Stg_ComponentFactory_GetDouble( cf, material->name, (Dictionary_Entry_Key)"referenceDepth", 0.0  );
	}
	
	/* Create Swarm Variables of each material swarm this ip swarm is mapped against */
	materialSwarms = IntegrationPointMapper_GetMaterialPointsSwarms( swarm->mapper, &(self->materialSwarmCount) );
	self->densitySwarmVariables = Memory_Alloc_Array( MaterialSwarmVariable*, self->materialSwarmCount, "DensityVariables" );
	self->alphaSwarmVariables   = Memory_Alloc_Array( MaterialSwarmVariable*, self->materialSwarmCount, "AlphaVariables" );
	
	for ( materialSwarm_I = 0; materialSwarm_I < self->materialSwarmCount; ++materialSwarm_I ) {
		name = Stg_Object_AppendSuffix( materialSwarms[materialSwarm_I], (Name)"Density"  );
		self->densitySwarmVariables[materialSwarm_I] = MaterialSwarmVariable_New( 
				name,
				(AbstractContext*)self->context,
				materialSwarms[materialSwarm_I], 
				1, 
				self->materials_Register, 
				self->materialExtHandle, 
				GetOffsetOfMember( *materialExt, density ) );
		Memory_Free( name );

		name = Stg_Object_AppendSuffix( materialSwarms[materialSwarm_I], (Name)"Alpha"  );
		self->alphaSwarmVariables[materialSwarm_I] = MaterialSwarmVariable_New( 
				name,
				(AbstractContext*)self->context,
				materialSwarms[materialSwarm_I], 
				1, 
				self->materials_Register, 
				self->materialExtHandle, 
				GetOffsetOfMember( *materialExt, alpha ) );
		Memory_Free( name );
	
		/* Build new Swarm Variables */
		Stg_Component_Build( self->densitySwarmVariables[materialSwarm_I], data, False );
		Stg_Component_Build( self->alphaSwarmVariables[materialSwarm_I],   data, False );
	}
}

void _BuoyancyForceTerm_Initialise( void* forceTerm, void* data ) {
	BuoyancyForceTerm* self = (BuoyancyForceTerm*)forceTerm;
   MaterialPointsSwarm* swarm = self->materialPointsSwarm;
   MaterialPoint*	particle = NULL;
	Index i, p_i;

	_ForceTerm_Initialise( self, data );

	if ( self->temperatureField )
		Stg_Component_Initialise( self->temperatureField, data, False );
	
	for ( i = 0; i < self->materialSwarmCount; ++i ) {
		Stg_Component_Initialise( self->densitySwarmVariables[i], data, False );
		Stg_Component_Initialise( self->alphaSwarmVariables[i],   data, False );
	}
	
	/* Initialize particle extensions */
   if( self->storeDensityOnParticles && !self->context->loadFromCheckPoint ) {
     BuoyancyForceTerm_ParticleExt* particleExt = NULL;

     for( p_i = 0; p_i < swarm->particleLocalCount; p_i++ ) {
       particle = (MaterialPoint*)Swarm_ParticleAt( swarm, p_i );
       particleExt = ExtensionManager_Get( swarm->particleExtensionMgr, particle, self->particleExtHandle );
       particleExt->density = 0.0;
     }
   }

}

void _BuoyancyForceTerm_Execute( void* forceTerm, void* data ) {
	BuoyancyForceTerm* self = (BuoyancyForceTerm*)forceTerm;

	_ForceTerm_Execute( self, data );
}

void _BuoyancyForceTerm_Destroy( void* forceTerm, void* data ) {
	BuoyancyForceTerm* self = (BuoyancyForceTerm*)forceTerm;
	Index i;

	for ( i = 0; i < self->materialSwarmCount; ++i ) {
		_Stg_Component_Delete( self->densitySwarmVariables[i] );
		_Stg_Component_Delete( self->alphaSwarmVariables[i] );
	}

	FreeArray( self->gHat );

	Memory_Free( self->densitySwarmVariables );
	Memory_Free( self->alphaSwarmVariables );

	_ForceTerm_Destroy( forceTerm, data );
}

void _BuoyancyForceTerm_AssembleElement( void* forceTerm, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elForceVec ) {
	BuoyancyForceTerm*               self               = (BuoyancyForceTerm*) forceTerm;
	IntegrationPoint*                particle;
	Particle_InCellIndex             cParticle_I;
	Particle_InCellIndex             cellParticleCount;
	Element_NodeIndex                elementNodeCount;
	Dimension_Index                  dim                = forceVector->dim;
	IntegrationPointsSwarm*          swarm              = (IntegrationPointsSwarm*)self->integrationSwarm;
	FeMesh*              mesh               = forceVector->feVariable->feMesh;
	Node_ElementLocalIndex           eNode_I;
	Cell_Index                       cell_I;
	ElementType*                     elementType;
	Dof_Index                        nodeDofCount;
	double                           gravity;
	double                           detJac             = 0.0;
	double                           factor;
	double                           Ni[27];
	double                           force;
	double*                          xi;
#if 0
	BuoyancyForceTerm_MaterialExt*   materialExt;
	Material*                        material;
#endif
	FeVariable*                      temperatureField   = self->temperatureField;
	double                           temperature        = 0.0;
    double                           background_density = 0.0;
	double*				 gHat;
	unsigned			d_i;

	double totalWeight = 0.0;
	double adjustFactor = 0.0;
	double density, rho0;
	double alpha, beta;
	double refTemp, refDepth;
	double depth = 0.0;
	double coord[3], min[3], max[3]; 

	elementType       = FeMesh_GetElementType( mesh, lElement_I );
	elementNodeCount  = elementType->nodeCount;
	nodeDofCount      = dim;
	cell_I            = CellLayout_MapElementIdToCellId( swarm->cellLayout, lElement_I );
	cellParticleCount = swarm->cellParticleCountTbl[cell_I];
	gHat		  = self->gHat;
	Mesh_GetGlobalCoordRange( mesh, min, max );

	/* adjust & adjustFactor -- 20060411 Alan
	 *
	 * The adjust decides whether an adjustFactor should be applied to the resulting factor.
	 * If on, the total weight of the particles in the cell are scaled to the cell local volume.
	 *
	 * This is designed to be used when integrating with swarms which do not cover the whole domain
	 * (ie - I used it to do dave.m's test of 1 swarm for blob, 1 swarm for background)
	 */ 
	if ( self->adjust ) {
		for( cParticle_I = 0 ; cParticle_I < cellParticleCount ; cParticle_I++ ) {
			particle = (IntegrationPoint*) Swarm_ParticleInCellAt( swarm, cell_I, cParticle_I );
			totalWeight += particle->weight;
		}
		adjustFactor = swarm->weights->cellLocalVolume / totalWeight;
	}
	else {
		adjustFactor = 1.0;
	}

	for( cParticle_I = 0 ; cParticle_I < cellParticleCount ; cParticle_I++ ) {
		particle = (IntegrationPoint*) Swarm_ParticleInCellAt( swarm, cell_I, cParticle_I );
		xi       = particle->xi;

		detJac = ElementType_JacobianDeterminant( elementType, mesh, lElement_I, xi, dim );
		ElementType_EvaluateShapeFunctionsAt( elementType, xi, Ni );

		/* Depth - FIXME: cartesian coords assumed */
		FeMesh_CoordLocalToGlobal( mesh, lElement_I, xi, (double*)&coord );
		depth = fabs( max[self->vertAxis] - coord[self->vertAxis] );

		/* Get parameters */
		if ( temperatureField ) 
			FeVariable_InterpolateFromMeshLocalCoord( temperatureField, mesh, lElement_I, xi, &temperature );

		rho0 = IntegrationPointMapper_GetDoubleFromMaterial(
		    swarm->mapper, particle, self->materialExtHandle,
		    offsetof(BuoyancyForceTerm_MaterialExt, density));
		alpha = IntegrationPointMapper_GetDoubleFromMaterial(
		    swarm->mapper, particle, self->materialExtHandle,
		    offsetof(BuoyancyForceTerm_MaterialExt, alpha));
		refTemp = IntegrationPointMapper_GetDoubleFromMaterial(
		    swarm->mapper, particle, self->materialExtHandle,
		    offsetof(BuoyancyForceTerm_MaterialExt, refTemp));
		beta = IntegrationPointMapper_GetDoubleFromMaterial(
		    swarm->mapper, particle, self->materialExtHandle,
		    offsetof(BuoyancyForceTerm_MaterialExt, beta));
		refDepth = IntegrationPointMapper_GetDoubleFromMaterial(
		    swarm->mapper, particle, self->materialExtHandle,
		    offsetof(BuoyancyForceTerm_MaterialExt, refDepth));

/*
		material = IntegrationPointsSwarm_GetMaterialOn( (IntegrationPointsSwarm*) swarm, particle );
		materialExt = ExtensionManager_Get( material->extensionMgr, material, self->materialExtHandle );
*/
		/* Gale compatible bit */
		if(self->hydrostaticTerm)
        {
        	FeMesh_CoordLocalToGlobal(mesh, cell_I, xi, coord);
            background_density=HydrostaticTerm_Density(self->hydrostaticTerm,coord);
        }

		/* Calculate Density */
		density = rho0 * (1.0 - alpha * (temperature - refTemp));
		if( beta != 0 ) {
		  density *=  (1.0 + beta * (depth - refDepth));
		}
		density = density - background_density;

		/* Store the density on the particle */
		if( self->storeDensityOnParticles ) {
		  BuoyancyForceTerm_ParticleExt* particleExt = NULL;
		  MaterialPointsSwarm* materialSwarm;
		  MaterialPoint* materialPoint;

		  materialPoint = OneToOneMapper_GetMaterialPoint(  swarm->mapper, particle, &materialSwarm );
		  particleExt = ExtensionManager_Get( materialSwarm->particleExtensionMgr, materialPoint, self->particleExtHandle );
		  particleExt->density = density;
		}


		/* Calculate Force */
		gravity = BuoyancyForceTerm_CalcGravity( self, (Swarm*)swarm, lElement_I, particle );
		force = density * gravity;
		factor = detJac * particle->weight * adjustFactor * force;

		/* Apply force in the correct direction */
		for( eNode_I = 0 ; eNode_I < elementNodeCount; eNode_I++ ) {
			if( gHat ) {
				for( d_i = 0; d_i < dim; d_i++ )
					elForceVec[ eNode_I * nodeDofCount + d_i ] += gHat[d_i] * factor * Ni[ eNode_I ] ;
			}
			else {
				elForceVec[ eNode_I * nodeDofCount + self->vertAxis ] += -1.0 * factor * Ni[ eNode_I ] ;
			}
		}
	}
	
}

/* The default implementation is for the gravity to be constant. */
double _BuoyancyForceTerm_CalcGravity( void* forceTerm, Swarm* swarm, Element_DomainIndex dElement_I, void* particle ) {
	BuoyancyForceTerm*               self               = (BuoyancyForceTerm*) forceTerm;

	return self->gravity;
}


