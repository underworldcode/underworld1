/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing 
** (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**   Victorian Partnership for Advanced Computing Ltd, Computational Software 
**     Development - http://csd.vpac.org
**   Australian Computational Earth Systems Simulator - 
**     http://www.access.edu.au
**   Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**   Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Robert Turnbull, Research Assistant, Monash University. 
**     (robert.turnbull@sci.monash.edu.au)
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**   Louis Moresi, Associate Professor, Monash University. 
**     (louis.moresi@sci.monash.edu.au)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**   Julian Giordani, Research Assistant, Monash University.
**     (julian.giordani@sci.monash.edu.au)
**   Vincent Lemiale, Postdoctoral Fellow, Monash University. 
**     (vincent.lemiale@sci.monash.edu.au)
**   Kent Humphries, Software Engineer, VPAC. (kenth@vpac.org)
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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <assert.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>
#include "types.h"
#include "ThermalDataShape.h"


const Type ThermalDataShape_Type = (const Type)"ThermalDataShape";


void ThermalDataShape_Load( ThermalDataShape* self ) {
    FILE* f;
    char line[200], *tok;
    int curTok, nLines, nPoints, newParticleInd, el, ind;
    double val, min[3], max[3], *points, scale;
    MaterialPoint* particle;
    IndexSet* coalElSet;
    int nCoalEls, *coalEls;
    int ii, jj;

    f = fopen( self->filename, "rt" );
    if(!f) {
        printf("*** ERROR ***\nFailed to find input file '%s'\n", 
self->filename);
        exit(1);
    }
    for( ii = 0; ii < 4; ii++ )
        fgets( line, 200, f );

    nPoints = 10754883;
    points = (double*)malloc( ((nPoints / self->step) + 1) * 3 * 
sizeof(double) );
    nLines = 0;
    max[0] = max[1] = max[2] = -FLT_MAX;
    min[0] = min[1] = min[2] = FLT_MAX;
    while( fgets( line, 200, f ) ) {
        tok = strtok( line, "," );
        if( tok ) {
            curTok = 0;
            do {
                if( curTok >= 1 && curTok <= 3 ) {
                    val = atof( tok );
		    ind = (curTok == 2) ? 3 : (curTok == 3) ? 2 : curTok;
                    if( val < min[ind - 1] )
                        min[ind - 1] = val;
                    if( val > max[ind - 1] )
                        max[ind - 1] = val;
                    points[3 * nLines + ind - 1] = val;
                }
                curTok++;
            }
            while( (tok = strtok( NULL, "," )) );
        }

        nLines++;
        if( nLines == nPoints ) break;
        for( ii = 0; ii < self->step - 1; ii++ ) {
            if(!fgets( line, 200, f ))
                break;
        }
    }
    nPoints = nLines;

    scale = max[0] - min[0];
    for( ii = 1; ii < 3; ii++ )
        if( max[ii] - min[ii] > scale ) scale = max[ii] - min[ii];
    for( ii = 0; ii < nPoints; ii++ ) {
        for( jj = 0; jj < 3; jj++ ) {
            points[3 * ii + jj] =
		((points[3 * ii + jj] - min[jj]) / scale) -
		0.5 * (max[jj] - min[jj]) / scale;
        }
    }

    /* Build a list of elements with coal points in them. */
    coalElSet = IndexSet_New( Mesh_GetDomainSize( ((ElementCellLayout*)self->swarm->cellLayout)->mesh, 3 ) );
    for( ii = 0; ii < nPoints; ii++ ) {
        if( Mesh_SearchElements( ((ElementCellLayout*)self->swarm->cellLayout)->mesh, points + 3 * ii, &el ) )
	    IndexSet_Add( coalElSet, el );
    }
    /*IndexSet_GetMembers( coalElSet, &nCoalEls, &coalEls );
    Stg_Class_Delete( coalElSet );*/
    self->elSet = coalElSet;

#if 0
    /* Convert particles in the coal cells to coal. */
    for( ii = 0; ii < nCoalEls; ii++ ) {
	for( jj = 0; jj < self->swarm->cellParticleCountTbl[coalEls[ii]]; jj++ ) {
	    particle = (MaterialPoint*)Swarm_ParticleInCellAt( self->swarm, coalEls[ii], jj );
	    particle->materialIndex = self->material->index;
	}
    }

    /* Here's the magic. */
    for( ii = 0; ii < nPoints; ii++ ) {
        if( Mesh_SearchElements( self->swarm->mesh, points + 3 * ii, &el ) ) {
            particle = (MaterialPoint*)Swarm_CreateNewParticle( self->swarm, &newParticleInd );
            memcpy( particle->coord, points + 3 * ii, 3 * sizeof(double) );
            particle->materialIndex = self->material->index;
	    Swarm_AddParticleToCell( self->swarm, el, newParticleInd );
        }
    }
#endif

    /* Free the enormous array of points. */
    free( points );
}

ThermalDataShape* ThermalDataShape_New( Name name ) {
    ThermalDataShape* self = (ThermalDataShape*) _ThermalDataShape_DefaultNew( name );

    ThermalDataShape_InitAll( self );
    return self;
}

ThermalDataShape* _ThermalDataShape_New(  THERMALDATASHAPE_DEFARGS  )
{
    ThermalDataShape* self;

    /* Allocate memory */
    assert( _sizeOfSelf >= sizeof(ThermalDataShape) );
    self = (ThermalDataShape*)_Stg_Shape_New(  STG_SHAPE_PASSARGS  );

    return self;
}

void _ThermalDataShape_Init( ThermalDataShape* self ) {
    self->elSet = NULL;
    /*self->elSet = STree_New();
    STree_SetIntCallbacks( self->elSet );
    STree_SetItemSize( self->elSet, sizeof(int) );*/
}

void ThermalDataShape_InitAll( void* _self ) {
    ThermalDataShape* self = (ThermalDataShape*)_self;
    Coord center;

    Stg_Shape_InitAll( self, 3, center, 0.0, 0.0, 0.0 );
    _ThermalDataShape_Init( self );
    self->isConstructed = False;
}

void _ThermalDataShape_Delete( void* _self ) {
    ThermalDataShape* self = (ThermalDataShape*)_self;

    if( self->elSet )
	Stg_Class_Delete( self->elSet );

    /* Delete parent */
    _Stg_Shape_Delete( self );
}


void _ThermalDataShape_Print( void* _self, Stream* stream ) {
    ThermalDataShape* self = (ThermalDataShape*)_self;
	
    /* Print parent */
    _Stg_Shape_Print( self, stream );
}

void* _ThermalDataShape_Copy( void* _self, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
    ThermalDataShape*	self = (ThermalDataShape*)_self;
    ThermalDataShape*	newThermalDataShape;
	
    newThermalDataShape = (ThermalDataShape*)_Stg_Shape_Copy( self, dest, deep, nameExt, ptrMap );
	
    return (void*)newThermalDataShape;
}

void* _ThermalDataShape_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                  _sizeOfSelf = sizeof(ThermalDataShape);
	Type                                                          type = ThermalDataShape_Type;
	Stg_Class_DeleteFunction*                                  _delete = _ThermalDataShape_Delete;
	Stg_Class_PrintFunction*                                    _print = _ThermalDataShape_Print;
	Stg_Class_CopyFunction*                                      _copy = _ThermalDataShape_Copy;
	Stg_Component_DefaultConstructorFunction*      _defaultConstructor = _ThermalDataShape_DefaultNew;
	Stg_Component_ConstructFunction*                        _construct = _ThermalDataShape_AssignFromXML;
	Stg_Component_BuildFunction*                                _build = _ThermalDataShape_Build;
	Stg_Component_InitialiseFunction*                      _initialise = _ThermalDataShape_Initialise;
	Stg_Component_ExecuteFunction*                            _execute = _ThermalDataShape_Execute;
	Stg_Component_DestroyFunction*                            _destroy = _ThermalDataShape_Destroy;
	Stg_Shape_IsCoordInsideFunction*                    _isCoordInside = ThermalDataShape_IsCoordInside;
	Stg_Shape_CalculateVolumeFunction*                _calculateVolume = NULL;
	Stg_Shape_DistanceFromCenterAxisFunction*  _distanceFromCenterAxis = NULL;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

    return (void*) _ThermalDataShape_New(  THERMALDATASHAPE_PASSARGS  );
}

void _ThermalDataShape_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
    ThermalDataShape* self = (ThermalDataShape*)_self;

    _Stg_Shape_AssignFromXML( self, cf, data );
    _ThermalDataShape_Init( self );

    self->filename = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"filename", ""  );
    self->step = Stg_ComponentFactory_GetInt( cf, self->name, (Dictionary_Entry_Key)"step", 100 );
    self->swarm = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"swarm", MaterialPointsSwarm, True, data  );
    self->mesh = ((ElementCellLayout*)self->swarm->cellLayout)->mesh;
}

void _ThermalDataShape_Build( void* _self, void* data ) {
    ThermalDataShape*	self = (ThermalDataShape*)_self;

    _Stg_Shape_Build( self, data );
    ThermalDataShape_Load( self );
}

void _ThermalDataShape_Initialise( void* _self, void* data ) {
    ThermalDataShape*	self = (ThermalDataShape*)_self;
	
    _Stg_Shape_Initialise( self, data );
}

void _ThermalDataShape_Execute( void* _self, void* data ) {
    ThermalDataShape*	self = (ThermalDataShape*)_self;
	
    _Stg_Shape_Execute( self, data );
}

void _ThermalDataShape_Destroy( void* _self, void* data ) {
    ThermalDataShape*	self = (ThermalDataShape*)_self;
	
    _Stg_Shape_Destroy( self, data );
}

Bool ThermalDataShape_IsCoordInside( void* _self, Coord coord ) {
    ThermalDataShape* self = (ThermalDataShape*)_self;
    int el;

    if( Mesh_SearchElements( self->mesh, coord, &el ) )
       return IndexSet_IsMember( self->elSet, el );

    return False;
}

double _ThermalDataShape_CalculateVolume( void* _self ) {
    assert( 0 /* unsure how this _self is setup...but shouldn't be hard to implement -- Alan */ );
    return 0.0;
}


