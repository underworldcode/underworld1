/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org) ) {
IrregTopology* self = (IrregTopology*)ir
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**
** This file may be distributed under the terms of the VPAC Public License
** as defined by VPAC of Australia and appearing in the file
** LICENSE.VPL included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>

#include "types.h"
#include "WeightsCalculator.h"
#include "VolumeWeights.h"

#include <assert.h>
#include <string.h>
#include <math.h>


/* Textual name of this class */
const Type VolumeWeights_Type = "VolumeWeights";

/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/
VolumeWeights* VolumeWeights_New( Name name, Dimension_Index dim, Stg_Shape* shape, FeMesh* mesh ) {
	VolumeWeights *self = _VolumeWeights_DefaultNew( name );

	self->isConstructed = True;
	_WeightsCalculator_Init( self, dim );
	_VolumeWeights_Init( self, shape, mesh );

	return self;
}

VolumeWeights* _VolumeWeights_New(  VOLUMEWEIGHTS_DEFARGS  ) {
    VolumeWeights* self;
	
    /* Allocate memory */
    assert( _sizeOfSelf >= sizeof(VolumeWeights) );
    self = (VolumeWeights*)_WeightsCalculator_New(  WEIGHTSCALCULATOR_PASSARGS  );
	
    /* General info */

    /* Virtual Info */

    return self;
}

void _VolumeWeights_Init( void* weights, Stg_Shape* shape, FeMesh* mesh ) {
    VolumeWeights* self = (VolumeWeights*)weights;

    self->shape = shape;
    self->mesh  = mesh;

}

/*------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _VolumeWeights_Delete( void* weights ) {
    VolumeWeights* self = (VolumeWeights*)weights;
	
    /* Delete parent */
    _WeightsCalculator_Delete( self );
}


void _VolumeWeights_Print( void* weights, Stream* stream ) {
    VolumeWeights* self = (VolumeWeights*)weights;
	
    /* Print parent */
    _WeightsCalculator_Print( self, stream );
}



void* _VolumeWeights_Copy( void* weights, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
    VolumeWeights*	self = (VolumeWeights*)weights;
    VolumeWeights*	newVolumeWeights;
	
    newVolumeWeights = (VolumeWeights*)_WeightsCalculator_Copy( self, dest, deep, nameExt, ptrMap );
	
    return (void*)newVolumeWeights;
}

void* _VolumeWeights_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(VolumeWeights);
	Type                                                      type = VolumeWeights_Type;
	Stg_Class_DeleteFunction*                              _delete = _VolumeWeights_Delete;
	Stg_Class_PrintFunction*                                _print = _VolumeWeights_Print;
	Stg_Class_CopyFunction*                                  _copy = _VolumeWeights_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _VolumeWeights_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _VolumeWeights_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _VolumeWeights_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _VolumeWeights_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _VolumeWeights_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _VolumeWeights_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
	WeightsCalculator_CalculateFunction*                _calculate = _VolumeWeights_Calculate;

    return (void*) _VolumeWeights_New(  VOLUMEWEIGHTS_PASSARGS  );
}


void _VolumeWeights_AssignFromXML( void* weights, Stg_ComponentFactory* cf, void* data ) {
    VolumeWeights*	     self          = (VolumeWeights*) weights;
    Stg_Shape*           shape;
    FeMesh*  mesh;

    _WeightsCalculator_AssignFromXML( self, cf, data );

    shape = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Shape", Stg_Shape, True, data  );
    mesh  = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Mesh", FeMesh, True, data  );
/*
  Journal_Firewall(
  Stg_Class_IsInstance( shape, Sphere_Type ),
  Journal_MyStream( Error_Type, self ),
  "In func %s, VolumeWeights is only designed for spheres right now.\n",
  __func__ );
*/	
    _VolumeWeights_Init( self, shape, mesh );
}

void _VolumeWeights_Build( void* weights, void* data ) {
    VolumeWeights*	self = (VolumeWeights*)weights;

    Stg_Component_Build( self->shape, data, False );
    Stg_Component_Build( self->mesh, data, False );
    _WeightsCalculator_Build( self, data );
}
void _VolumeWeights_Initialise( void* weights, void* data ) {
    VolumeWeights*	self = (VolumeWeights*)weights;

    Stg_Component_Initialise( self->shape, data, False );
    Stg_Component_Initialise( self->mesh, data, False );	
    _WeightsCalculator_Initialise( self, data );
}
void _VolumeWeights_Execute( void* weights, void* data ) {
    VolumeWeights*	self = (VolumeWeights*)weights;
	
    _WeightsCalculator_Execute( self, data );
}
void _VolumeWeights_Destroy( void* weights, void* data ) {
    VolumeWeights*	self = (VolumeWeights*)weights;

    Stg_Component_Destroy( self->shape, data, False );
    Stg_Component_Destroy( self->mesh, data, False );	
    _WeightsCalculator_Destroy( self, data );
}

/*-------------------------------------------------------------------------------------------------------------------------
** Private Functions
*/
void _VolumeWeights_Calculate( void* weights, void* _swarm, Cell_LocalIndex lCell_I ) {
    VolumeWeights*               self               = (VolumeWeights*)  weights;
    Swarm*                       swarm              = (Swarm*) _swarm;
    Sphere*                      shape              = (Sphere*)self->shape;
    Index                        numberOfParticles;
    double                       volume;
    double                       dx;
    double                       dy;
    double                       dz;
    double                       weight;
    Grid*				vertGrid;
	
    (void)MPI_Allreduce( 
        &(swarm->particleLocalCount),
        &(numberOfParticles),
        1,
        MPI_UNSIGNED,
        MPI_SUM,
        MPI_COMM_WORLD );

    volume = Stg_Shape_CalculateVolume( shape );

    /*
    ** NOTE: Big assumption that the mesh is regular.
    */
    vertGrid = *(Grid**)ExtensionManager_Get( self->mesh->info, self->mesh, self->mesh->vertGridId );
	
    dx = 1.0 / (double)(vertGrid->sizes[0] - 1); /* size of an element */
    dy = 1.0 / (double)(vertGrid->sizes[1] - 1);
    if ( self->dim > 2 ) {
        dz = 1.0 / (double)(vertGrid->sizes[2] - 1);
    }
    else {
        dz = 1.0;
    }

    /* (V / np) * 4 / ( dx * dy * dz ) */
    /* Where 4 in the value of cellLocalVolume in 2D */
    weight = (volume / (double)numberOfParticles) * ( self->cellLocalVolume / ( dx * dy * dz )  );

    WeightsCalculator_SetWeightsValueAllInCell( self, swarm, lCell_I, weight );
}

/*-------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/




