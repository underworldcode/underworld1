/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University 
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 		* Redistributions of source code must retain the above copyright notice, 
** 			this list of conditions and the following disclaimer.
** 		* Redistributions in binary form must reproduce the above copyright 
**			notice, this list of conditions and the following disclaimer in the 
**			documentation and/or other materials provided with the distribution.
** 		* Neither the name of the Monash University nor the names of its contributors 
**			may be used to endorse or promote products derived from this software 
**			without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
** THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
** PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS 
** BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
** OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**
** Contact:
*%  Louis.Moresi - Louis.Moresi@monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
** 
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/** \file 
\details Calculate temperature at internal boundary layers.
**/
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include <StgFEM/FrequentOutput/FrequentOutput.h>
#include <Underworld/Nusselt/Nusselt.h>

#include "BoundaryLayers.h"

#include <assert.h>
#include <string.h>

const Type Underworld_BoundaryLayers_Type = "Underworld_BoundaryLayers";

void _Underworld_BoundaryLayers_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
	UnderworldContext* context;

	/*printf("AKJFHQEIUH)@$UFKAJSDHRF\nKJASHRFOPUEH\nsaljkdh9\n9)#@$\n213\n\n"); The coordinates of the California Moon ??   */ 
	
	context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", UnderworldContext, True, data  ); 

	/* Add functions to entry points */
	ContextEP_Append( context, AbstractContext_EP_FrequentOutput, Underworld_BoundaryLayers_Output );
	
	Underworld_BoundaryLayers_Setup( context );
}

void* _Underworld_BoundaryLayers_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Underworld_BoundaryLayers);
	Type                                                      type = Underworld_BoundaryLayers_Type;
	Stg_Class_DeleteFunction*                              _delete = _Codelet_Delete;
	Stg_Class_PrintFunction*                                _print = _Codelet_Print;
	Stg_Class_CopyFunction*                                  _copy = _Codelet_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Underworld_BoundaryLayers_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Underworld_BoundaryLayers_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Codelet_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Codelet_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Codelet_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Codelet_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	/* printf("2  AKJFHQEIUH)@$UFKAJSDHRF\nKJASHRFOPUEH\nsaljkdh9\n9)#@$\n213\n\n"); */
	return _Codelet_New(  CODELET_PASSARGS  );
}

Index Underworld_BoundaryLayers_Register( PluginsManager* pluginsManager ) {
	return PluginsManager_Submit( pluginsManager, Underworld_BoundaryLayers_Type, (Name)"0", _Underworld_BoundaryLayers_DefaultNew );
}

void Underworld_BoundaryLayers_Setup( UnderworldContext* context ) {
	Underworld_BoundaryLayers* self;
	Dictionary*  dictionary = context->dictionary;

	self = (Underworld_BoundaryLayers* )LiveComponentRegister_Get( context->CF->LCRegister, (Name)Underworld_BoundaryLayers_Type  );

	self->internalTemperature = 0.5;
	self->tolerance = Dictionary_GetDouble_WithDefault( dictionary, (Dictionary_Entry_Key)"BoundaryLayersTolerance", 0.0001  );
	self->maxIterations = Dictionary_GetUnsignedInt_WithDefault( dictionary, "BoundaryLayersMaxIterations", 100 );
	self->minIterations = Dictionary_GetUnsignedInt_WithDefault( dictionary, "BoundaryLayersMinIterations", 2 );

	StgFEM_FrequentOutput_PrintString( context, "InternalTemp" );
	StgFEM_FrequentOutput_PrintString( context, "HotLayer" );
	StgFEM_FrequentOutput_PrintString( context, "ColdLayer" );
}


void Underworld_BoundaryLayers_Output( UnderworldContext* context ) {
	Underworld_BoundaryLayers* self;
	Underworld_Nusselt*        nusseltPlugin;
	Iteration_Index            iteration_I;
	Iteration_Index            maxIterations;
	Iteration_Index            minIterations;
	double                     internalTemperature;
	double                     prevInternalTemperature;
	double                     tolerance;
	double                     nusseltNumber;
	double                     hotLayerThickness;
	double                     coldLayerThickness;

	self = (Underworld_BoundaryLayers*)LiveComponentRegister_Get( context->CF->LCRegister, (Name)Underworld_BoundaryLayers_Type );
	nusseltPlugin = (Underworld_Nusselt* )LiveComponentRegister_Get( context->CF->LCRegister, (Name)Underworld_Nusselt_Type  );	

	iteration_I             = 0;
	maxIterations           = self->maxIterations;
	minIterations           = self->minIterations;
	internalTemperature     = 0.5;
	tolerance               = self->tolerance;
	nusseltNumber           = nusseltPlugin->nusseltNumber;	
	hotLayerThickness       = 0.0; 
	coldLayerThickness      = 0.0;
	
	do {
		prevInternalTemperature = internalTemperature;
		internalTemperature = Underworld_BoundaryLayers_InternalTemperature( context, hotLayerThickness, coldLayerThickness );
		if ( internalTemperature > 1.0 )
			internalTemperature = 1.0;
		if ( internalTemperature < 0.0 )
			internalTemperature = 0.0;
		
		hotLayerThickness = (1.0 - internalTemperature)/nusseltNumber;
		coldLayerThickness = internalTemperature/nusseltNumber;			

		assert( !isnan( hotLayerThickness ) && !isnan( coldLayerThickness ) );

		iteration_I++;
		if ( iteration_I >= maxIterations ) {
			break;
		}
	} while ( fabs(internalTemperature - prevInternalTemperature) > tolerance || iteration_I < minIterations );

	StgFEM_FrequentOutput_PrintValue( context, internalTemperature );
	StgFEM_FrequentOutput_PrintValue( context, hotLayerThickness );
	StgFEM_FrequentOutput_PrintValue( context, coldLayerThickness );

	self->internalTemperature = internalTemperature;
	self->hotLayerThickness = hotLayerThickness;
	self->coldLayerThickness = coldLayerThickness;
}

double Underworld_BoundaryLayers_InternalTemperature( UnderworldContext* context, double hotLayerThickness, double coldLayerThickness ) {
	Dimension_Index     dim                    = context->dim;
	double		    maxCrd[3], minCrd[3];
	double              volume;
	double              internalTemperature;
	double              integral               = 0.0;
	double              integralGlobal         = 0.0;
	Swarm*              gaussSwarm             = (Swarm*)LiveComponentRegister_Get( context->CF->LCRegister, (Name)"gaussSwarm" );
	IntegrationPoint*   particle;
	double              bottomLayerHeight;
	double              topLayerHeight;
	FeVariable*         temperatureField       = (FeVariable* ) LiveComponentRegister_Get( context->CF->LCRegister, (Name)"TemperatureField"  );
	FeMesh*		    mesh                   = temperatureField->feMesh;
	Element_LocalIndex  lElement_I;
	Node_LocalIndex    	nodeAtElementBottom;
	Node_LocalIndex    	nodeAtElementTop;
	int                 elementNodeCount, *elementNodes;
	double              elementBottomHeight;
	double              elementTopHeight;
	double              detJac;
	double              factor;
	Coord               xi;
	double              temperature;
	Particle_Index      gaussPoint_I;
	Particle_Index      gaussPointCount        = gaussSwarm->particleLocalCount;
	ElementType*        elementType;
	IArray*			incArray;

	Mesh_GetGlobalCoordRange( mesh, minCrd, maxCrd );
	bottomLayerHeight = minCrd[ J_AXIS ] + hotLayerThickness;  
	topLayerHeight    = maxCrd[ J_AXIS ] - coldLayerThickness; 
	
	/* volume (or area) Integrating and Averaging over */
	volume = (maxCrd[ I_AXIS ] - minCrd[ I_AXIS ]) * (topLayerHeight - bottomLayerHeight);
	if (dim == 3)
		volume *= maxCrd[ K_AXIS ] - minCrd[ K_AXIS ];
	
	/************************************** Do Integration ************************************************/
	
	/* Loop over Elements */
	incArray = IArray_New();
	for ( lElement_I = 0 ; lElement_I < FeMesh_GetElementLocalSize( mesh ) ; lElement_I++ ) {
		elementType         = FeMesh_GetElementType( mesh, lElement_I );
		FeMesh_GetElementNodes( mesh, lElement_I, incArray );
		elementNodeCount = IArray_GetSize( incArray );
		elementNodes = IArray_GetPtr( incArray );

		nodeAtElementBottom = elementNodes[ 0 ] ;
		nodeAtElementTop    = elementNodes[ 2 ] ;

		elementBottomHeight = Mesh_GetVertex( mesh, nodeAtElementBottom )[ J_AXIS ];
		elementTopHeight    = Mesh_GetVertex( mesh, nodeAtElementTop    )[ J_AXIS ];
		
		/* Test where elements are relative to boundaries */
		if ( elementTopHeight < bottomLayerHeight || elementBottomHeight > topLayerHeight ) {
			/* Element in Boundary */
			continue;
		}
		else if ( elementBottomHeight >= bottomLayerHeight && elementTopHeight <= topLayerHeight ) {
			/* Element completely in interior */
			integral += FeVariable_IntegrateElement( temperatureField, gaussSwarm, lElement_I );
		}
		else if ( elementBottomHeight <= bottomLayerHeight && elementTopHeight >= topLayerHeight ) {
			/* Element in intersection of both Hot and Cold Bottom Boundary Layers */
			factor = ( topLayerHeight - bottomLayerHeight ) / ( elementTopHeight - elementBottomHeight );
			for ( gaussPoint_I = 0 ; gaussPoint_I < gaussPointCount ; gaussPoint_I++ ) {
				particle = (IntegrationPoint*) Swarm_ParticleAt( gaussSwarm, gaussPoint_I );

				/* Get Local Coord of particle in pseudo element */
				memcpy( xi, particle->xi, sizeof(Coord) );
				xi[ J_AXIS ] = factor * xi[ J_AXIS ] + ( topLayerHeight + bottomLayerHeight - 2.0 * elementBottomHeight )/( elementTopHeight - elementBottomHeight ) - 1.0;
				assert( fabs( xi[ J_AXIS ] ) <= 1.0 );

				/* Sum integral at this pseudo particle */
				FeVariable_InterpolateWithinElement( temperatureField, lElement_I, xi, &temperature );
				detJac = factor * ElementType_JacobianDeterminant( elementType, mesh, lElement_I, xi, dim );
				integral += particle->weight * temperature * detJac;
			}
		}
		else if (elementBottomHeight <= bottomLayerHeight) {		
			/* Element in intersection at Hot Bottom Boundary*/
			factor = ( elementTopHeight - bottomLayerHeight ) / ( elementTopHeight - elementBottomHeight );
			for ( gaussPoint_I = 0 ; gaussPoint_I < gaussPointCount ; gaussPoint_I++ ) {
				particle = (IntegrationPoint*) Swarm_ParticleAt( gaussSwarm, gaussPoint_I );

				/* Get Local Coord of particle in pseudo element */
				memcpy( xi, particle->xi, sizeof(Coord) );
				xi[ J_AXIS ] = 1.0 - factor * ( 1.0 - xi[ J_AXIS ] );
				assert( fabs( xi[ J_AXIS ] ) <= 1.0 );

				/* Sum integral at this pseudo particle */
				FeVariable_InterpolateWithinElement( temperatureField, lElement_I, xi, &temperature );
				detJac = factor * ElementType_JacobianDeterminant( elementType, mesh, lElement_I, xi, dim );
				integral += particle->weight * temperature * detJac;
			}
		}     
		/* Element in Cold Top Layer */
		else if (elementTopHeight >= topLayerHeight) {	
			factor = ( topLayerHeight - elementBottomHeight ) / ( elementTopHeight - elementBottomHeight );
			for ( gaussPoint_I = 0 ; gaussPoint_I < gaussPointCount ; gaussPoint_I++ ) {
				particle = (IntegrationPoint*) Swarm_ParticleAt( gaussSwarm, gaussPoint_I );

				/* Get Local Coord of particle in pseudo element */
				memcpy( xi, particle->xi, sizeof(Coord) );
				xi[ J_AXIS ] = factor * ( xi[ J_AXIS ] + 1.0 ) - 1.0;
				assert( fabs( xi[ J_AXIS ] ) <= 1.0 );

				/* Sum integral at this pseudo particle */
				FeVariable_InterpolateWithinElement( temperatureField, lElement_I, xi, &temperature );
				detJac = factor * ElementType_JacobianDeterminant( elementType, mesh, lElement_I, xi, dim );
				integral += particle->weight * temperature * detJac;
			}
		}
		else {
			assert(0);
		}
	}

	Stg_Class_Delete( incArray );
		
	/* Gather data from other processors */
	(void)MPI_Allreduce( &integral, &integralGlobal, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );
	
	/* Find Average */
	internalTemperature = integralGlobal/volume;
		
	return internalTemperature;
}




