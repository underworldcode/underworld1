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
#include "ThermalDataMaterial.h"


const Type ThermalDataMaterial_Type = (const Type)"ThermalDataMaterial";


void ThermalDataMaterial_Load( ThermalDataMaterial* self ) {
    FILE* f;
    char line[200], *tok;
    int curTok, nLines, nPoints, newParticleInd, element_i, ind;
    double val, *points, scale;
    MaterialPoint* matParticle;
    int ii, jj;
    double particleToPointDist;
    int cell_i, particle_i;
    FeVariable* feVariable;
    int feVariable_i;
    int rank, size, proc_i;
    double min[3], max[3], testCoord[3];

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    Mesh_GetLocalCoordRange( self->mesh, min, max );

    if( !rank )
        printf( "reading geothermal data file on %d procs...\n", size );

    f = fopen( self->filename, "rt" );
    if(!f) {
        printf("*** ERROR ***\nFailed to find input file '%s'\n", self->filename);
        exit(1);
    }
    for( ii = 0; ii < 4; ii++ )
        fgets( line, 200, f );

    nPoints = self->nSamplePoints;
    points = (double*)malloc( ((nPoints / self->step) + 1) * 3 * sizeof(double) );
    nLines = 0;
    while( fgets( line, 200, f ) ) {
        tok = strtok( line, "," );
        if( tok ) {
            curTok = 0;
            do {
                if( curTok >= 1 && curTok <= 3 ) {
                    val = atof( tok );
	            ind = (curTok == 2) ? 3 : (curTok == 3) ? 2 : curTok;

                    testCoord[ind-1] = val;
                }
                curTok++;
            }
            while( (tok = strtok( NULL, "," )) );
        }

        if( testCoord[0] < min[0] || testCoord[0] > max[0] ||
            testCoord[1] < min[1] || testCoord[1] > max[1] ||
            testCoord[2] < min[2] || testCoord[2] > max[2] )
            continue;

        points[3*nLines + 0] = testCoord[0];
        points[3*nLines + 1] = testCoord[1];
        points[3*nLines + 2] = testCoord[2];

        nLines++;
        if( nLines == nPoints ) break;
        for( ii = 0; ii < self->step - 1; ii++ ) {
            if(!fgets( line, 200, f ))
                break;
        }
    }
    nPoints = nLines;
    fclose( f );

    printf( "%2d: done reading file... %8d sample points found in local domain\n", rank, nPoints );
    MPI_Barrier( MPI_COMM_WORLD );

    nLines = 0;
    for( ii = 0; ii < nPoints; ii++ ) {
        if( Mesh_SearchElements( self->mesh, points + 3 * ii, &element_i ) ) {
            if( element_i >= Mesh_GetLocalSize( self->mesh, MT_VOLUME ) )
                continue;

            nLines++;

            cell_i = CellLayout_MapElementIdToCellId( self->swarm->cellLayout, element_i );
            particle_i = Swarm_FindClosestParticleInCell( self->swarm, cell_i, 3, points + 3 * ii, &particleToPointDist );
            matParticle = (MaterialPoint*) Swarm_ParticleInCellAt( self->swarm, cell_i, particle_i );
            matParticle->materialIndex = self->material->index;
	}
    }
    
    printf( "%2d: done assigning material points... %8d particles updated\n", rank, nLines );
    MPI_Barrier( MPI_COMM_WORLD );

    printf("\n%2d: Geothermal information:\n", rank);
    printf("%2d:   Read %8d points... %8d points found within mesh.\n", rank, nPoints, nLines);

    /* Free the enormous array of points. */
    free( points );
    
    for( feVariable_i = 0; feVariable_i < self->context->fieldVariable_Register->objects->count; feVariable_i++ ) {
	feVariable = (FeVariable*) FieldVariable_Register_GetByIndex( self->context->fieldVariable_Register, feVariable_i );
    	FeVariable_SyncShadowValues( feVariable );
    }

    MPI_Barrier( MPI_COMM_WORLD );
}

/* if the step size is too small, then just assign all the particles within a sampled cell to the given material type... */
void ThermalDataMaterial_Load_ByCell( ThermalDataMaterial* self ) {
    FILE* f;
    char line[200], *tok;
    int curTok, nLines, newParticleInd, element_i, ind;
    double val, scale;
    MaterialPoint* matParticle;
    int ii, jj;
    double particleToPointDist;
    int cell_i, particle_i;
    FeVariable* feVariable;
    int feVariable_i;
    int rank, size, proc_i;
    double min[3], max[3], testCoord[3];
    IndexSet* iSet = IndexSet_New( Mesh_GetDomainSize( self->mesh, MT_VOLUME ) );
    ParticleLayout* particleLayout = self->swarm->particleLayout;
    unsigned partsPerCell;
    unsigned numLinesRead = 0;
    double* points;
    unsigned numMatEls = 0;

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    Mesh_GetLocalCoordRange( self->mesh, min, max );

    if( !rank )
        printf( "reading geothermal data file on %d procs...\n", size );

    f = fopen( self->filename, "rt" );
    if(!f) {
        printf("*** ERROR ***\nFailed to find input file '%s'\n", self->filename);
        exit(1);
    }
    for( ii = 0; ii < 4; ii++ )
        fgets( line, 200, f );

    if( Stg_Class_IsInstance( particleLayout, GaussParticleLayout_Type ) ) {
        GaussParticleLayout* gaussLayout = (GaussParticleLayout*)particleLayout;
        partsPerCell = gaussLayout->particlesPerDim[0] * gaussLayout->particlesPerDim[1] * gaussLayout->particlesPerDim[2];
    }
    else if( Stg_Class_IsInstance( particleLayout, IrregularMeshParticleLayout_Type ) ) {
        IrregularMeshParticleLayout* irregularLayout = (IrregularMeshParticleLayout*)particleLayout;
	partsPerCell = irregularLayout->cellParticleCount;
    }

    nLines = 0;
    points = (double*)malloc( ((self->nSamplePoints / self->step) + 1) * 3 * sizeof(double) );
    while( fgets( line, 200, f ) ) {
        tok = strtok( line, "," );
        if( tok ) {
            curTok = 0;
            do {
                if( curTok >= 1 && curTok <= 3 ) {
                    val = atof( tok );
	            ind = (curTok == 2) ? 3 : (curTok == 3) ? 2 : curTok;

                    testCoord[ind-1] = val;
                }
                curTok++;
            }
            while( (tok = strtok( NULL, "," )) );
        }

        numLinesRead++;

        if( testCoord[0] < min[0] || testCoord[0] > max[0] ||
            testCoord[1] < min[1] || testCoord[1] > max[1] ||
            testCoord[2] < min[2] || testCoord[2] > max[2] )
            continue;
/*
	Mesh_SearchElements( self->mesh, testCoord, &element_i );
        if( IndexSet_IsMember( iSet, element_i ) )
            continue;

        IndexSet_Add( iSet, element_i );
*/
	points[nLines*3 + 0] = testCoord[0];
	points[nLines*3 + 1] = testCoord[1];
	points[nLines*3 + 2] = testCoord[2];

        nLines++;
        if( nLines == self->nSamplePoints ) break;
        for( ii = 0; ii < self->step - 1; ii++ ) {
            if(!fgets( line, 200, f ))
                break;
        }
    }
    fclose( f );

    for( ii=0; ii<nLines; ii++ ) {
        Mesh_SearchElements( self->mesh, &points[ii*3], &element_i );
        if( !IndexSet_IsMember( iSet, element_i ) ) {
            IndexSet_Add( iSet, element_i );
            numMatEls++;
        }
    }
    free( points );

    printf( "%2d: done assigning material cells from sample points....\n", rank );
    MPI_Barrier( MPI_COMM_WORLD );

    nLines = 0;
    for( ii = 0; ii < numMatEls; ii++ ) {
        element_i = IndexSet_GetIndexOfNthMember( iSet, ii );
        cell_i = CellLayout_MapElementIdToCellId( self->swarm->cellLayout, element_i );
        for( particle_i = 0; particle_i < partsPerCell; particle_i++ ) {
            matParticle = (MaterialPoint*) Swarm_ParticleInCellAt( self->swarm, cell_i, particle_i );
            matParticle->materialIndex = self->material->index;
	}
    }
    
    printf( "%2d: done assigning material points... %8d particles updated\n", rank, nLines * partsPerCell );
    MPI_Barrier( MPI_COMM_WORLD );

    for( feVariable_i = 0; feVariable_i < self->context->fieldVariable_Register->objects->count; feVariable_i++ ) {
	feVariable = (FeVariable*) FieldVariable_Register_GetByIndex( self->context->fieldVariable_Register, feVariable_i );
    	FeVariable_SyncShadowValues( feVariable );
    }

    MPI_Barrier( MPI_COMM_WORLD );
}

void ThermalDataMaterial_Load_BasinMaterial( ThermalDataMaterial* self ) {
    FILE*		fp		= fopen( self->basinMatFilename, "r" );
    double		coord[3];
    double		partToPoint;
    double		min[3], max[3];
    unsigned		element_i;
    unsigned		cell_i;
    unsigned		particle_i;
    MaterialPoint*	matParticle;

    Mesh_GetLocalCoordRange( self->mesh, min, max );

    while( !feof( fp ) ) {
        fscanf( fp, "%lf %lf %lf", coord+0, coord+2, coord+1 );
        
        if( coord[0] < min[0] || coord[0] > max[0] ||
            coord[1] < min[1] || coord[1] > max[1] ||
            coord[2] < min[2] || coord[2] > max[2] )
            continue;

	Mesh_SearchElements( self->mesh, coord, &element_i );
        if( element_i >= Mesh_GetLocalSize( self->mesh, MT_VOLUME ) )
            continue;

        cell_i = CellLayout_MapElementIdToCellId( self->swarm->cellLayout, element_i );
        particle_i = Swarm_FindClosestParticleInCell( self->swarm, cell_i, 3, coord, &partToPoint );
        matParticle = (MaterialPoint*) Swarm_ParticleInCellAt( self->swarm, cell_i, particle_i );
        matParticle->materialIndex = self->basinMat->index;
    }

    fclose( fp );
}

/* do some initial evaluations of the input file to determine the bounding box of the material layer, and the number or sample
 * points in the file */
void ThermalDataMaterial_FindMaterialBoundingBox( ThermalDataMaterial* self, double* min, double* max ) {
    FILE* f;
    char line[200], *tok;
    int curTok, nLines, ind, ii, rank, size;
    double val;

    MPI_Comm_rank( MPI_COMM_WORLD, &rank );
    MPI_Comm_size( MPI_COMM_WORLD, &size );

    f = fopen( self->filename, "rt" );
    if(!f) {
        printf("*** ERROR ***\nFailed to find input file '%s'\n", self->filename);
        exit(1);
    }
    for( ii = 0; ii < 4; ii++ )
        fgets( line, 200, f );

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
                }
                curTok++;
            }
            while( (tok = strtok( NULL, "," )) );
        }

        nLines++;
    }
    self->nSamplePoints = nLines;
    fclose( f );

    for( ii = 0; ii < 3; ii++ ) {
        self->min[ii] = min[ii];
        self->max[ii] = max[ii];
    }
}

ThermalDataMaterial* ThermalDataMaterial_New( Name name ) {
    ThermalDataMaterial* self = (ThermalDataMaterial*) _ThermalDataMaterial_DefaultNew( name );

    ThermalDataMaterial_InitAll( self );
    return self;
}

ThermalDataMaterial* _ThermalDataMaterial_New(  THERMALDATAMATERIAL_DEFARGS  )
{
    ThermalDataMaterial* self;

    /* Allocate memory */
    assert( _sizeOfSelf >= sizeof(ThermalDataMaterial) );
	/* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
	/* This means that any values of these parameters that are passed into this function are not passed onto the parent function
	   and so should be set to ZERO in any children of this class. */
	nameAllocationType = NON_GLOBAL;

    self = (ThermalDataMaterial*)_Stg_Component_New(  STG_COMPONENT_PASSARGS  );

    return self;
}

void _ThermalDataMaterial_Init( ThermalDataMaterial* self ) {
}

void ThermalDataMaterial_InitAll( void* _self ) {
    ThermalDataMaterial* self = (ThermalDataMaterial*)_self;
    Coord center;

    Stg_Shape_InitAll( self, 3, center, 0.0, 0.0, 0.0 );
    _ThermalDataMaterial_Init( self );
    self->isConstructed = False;
}

void _ThermalDataMaterial_Delete( void* _self ) {
    ThermalDataMaterial* self = (ThermalDataMaterial*)_self;

    /* Delete parent */
    _Stg_Shape_Delete( self );
}


void _ThermalDataMaterial_Print( void* _self, Stream* stream ) {
    ThermalDataMaterial* self = (ThermalDataMaterial*)_self;
	
    /* Print parent */
    _Stg_Shape_Print( self, stream );
}

void* _ThermalDataMaterial_Copy( void* _self, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
    ThermalDataMaterial*	self = (ThermalDataMaterial*)_self;
    ThermalDataMaterial*	newThermalDataMaterial;
	
    newThermalDataMaterial = (ThermalDataMaterial*)_Stg_Shape_Copy( self, dest, deep, nameExt, ptrMap );
	
    return (void*)newThermalDataMaterial;
}

void* _ThermalDataMaterial_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(ThermalDataMaterial);
	Type                                                      type = ThermalDataMaterial_Type;
	Stg_Class_DeleteFunction*                              _delete = _ThermalDataMaterial_Delete;
	Stg_Class_PrintFunction*                                _print = _ThermalDataMaterial_Print;
	Stg_Class_CopyFunction*                                  _copy = _ThermalDataMaterial_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _ThermalDataMaterial_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _ThermalDataMaterial_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _ThermalDataMaterial_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _ThermalDataMaterial_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _ThermalDataMaterial_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _ThermalDataMaterial_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

    return (void*) _ThermalDataMaterial_New(  THERMALDATAMATERIAL_PASSARGS  );
}

void _ThermalDataMaterial_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
    ThermalDataMaterial* 	self = (ThermalDataMaterial*)_self;

    _ThermalDataMaterial_Init( self );
    Stg_Component_AssignFromXML( self, cf, data, False );

    self->context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", FiniteElementContext, False, data );
    if( !self->context  )
        self->context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", FiniteElementContext, True, data  );

    self->filename = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"filename", ""  );
    self->step     = Stg_ComponentFactory_GetInt( cf, self->name, (Dictionary_Entry_Key)"step", 1 );
    self->swarm    = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Swarm", MaterialPointsSwarm, True, data  );
    self->mesh     = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Mesh", FeMesh, True, data  );
    self->material = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Material", Material, True, data  );
    self->context  = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", FiniteElementContext, False, data );
    if( !self->context  )
        self->context  = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", FiniteElementContext, True, data  );

    self->loadByCell = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"loadMaterialByCell", False  );

    self->basinMat = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"BasinMaterial", Material, False, data  );
    self->basinMatFilename = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"basinMaterialFilename", ""  );
}

void _ThermalDataMaterial_Build( void* _self, void* data ) {
    ThermalDataMaterial*	self = (ThermalDataMaterial*)_self;
}

void _ThermalDataMaterial_Initialise( void* _self, void* data ) {
    ThermalDataMaterial*	self = (ThermalDataMaterial*)_self;
    double			bBoxMin[3], bBoxMax[3];

    /* initialise the material before we load it onto the particles.
     * this ensures that the particles which exist in the elements for which the shape has been
     * assigned are set to the relevant material index before we go an mess with them in the 
     * _Load() function. 
     * if we set the initail shape to be NOWHERE, then we can specify which particles we wish to 
     * to have the relevant material property in the _Load() function instead...
     */
    Stg_Component_Initialise( self->material, data, False );

    ThermalDataMaterial_FindMaterialBoundingBox( self, bBoxMin, bBoxMax );

    if( !self->loadByCell )
        ThermalDataMaterial_Load( self );
    else
        ThermalDataMaterial_Load_ByCell( self );

    if( self->basinMat )
        ThermalDataMaterial_Load_BasinMaterial( self );
}

void _ThermalDataMaterial_Execute( void* _self, void* data ) {
    ThermalDataMaterial*	self = (ThermalDataMaterial*)_self;
}

void _ThermalDataMaterial_Destroy( void* _self, void* data ) {
    ThermalDataMaterial*	self = (ThermalDataMaterial*)_self;
}



