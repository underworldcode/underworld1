/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgDomain/Geometry/Geometry.h>
#include <StgDomain/Shape/Shape.h>
#include <StgDomain/Mesh/Mesh.h>



#include "types.h"
#include "RegRemesh.h"

/* Textual name of this class */
const Type RegRemesh_Type = "RegRemesh";

/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/

RegRemesh* RegRemesh_New( Name name, AbstractContext* context, Mesh* mesh, int surfaceIdx ) {
    RegRemesh*self = _RegRemesh_DefaultNew( name );

    self->isConstructed = True;
    _Remesher_Init( self, context, mesh );
    _RegRemesh_Init( self, surfaceIdx );

    return self;
}

RegRemesh* _RegRemesh_DefaultNew( Name name ) {
    /* Variables set in this function */
    SizeT                                              _sizeOfSelf = sizeof(RegRemesh);
    Type                                                      type = RegRemesh_Type;
    Stg_Class_DeleteFunction*                              _delete = _RegRemesh_Delete;
    Stg_Class_PrintFunction*                                _print = _RegRemesh_Print;
    Stg_Class_CopyFunction*                                  _copy = NULL;
    Stg_Component_DefaultConstructorFunction*  _defaultConstructor = (void*(*)(Name))_RegRemesh_DefaultNew;
    Stg_Component_ConstructFunction*                    _construct = _RegRemesh_AssignFromXML;
    Stg_Component_BuildFunction*                            _build = _RegRemesh_Build;
    Stg_Component_InitialiseFunction*                  _initialise = _RegRemesh_Initialise;
    Stg_Component_ExecuteFunction*                        _execute = _RegRemesh_Execute;
    Stg_Component_DestroyFunction*                        _destroy = _RegRemesh_Destroy;
    AllocationType                              nameAllocationType = NON_GLOBAL;
    Remesher_RemeshFunc*                                remeshFunc = NULL;

    return _RegRemesh_New(  REGREMESH_PASSARGS  );
}

RegRemesh* _RegRemesh_New(  REGREMESH_DEFARGS  ) {
    RegRemesh*	self;

    /* Allocate memory. */
    self = (RegRemesh*)_Remesher_New(  REMESHER_PASSARGS  );

    /* RegRemesh info */

    return self;
}

void _RegRemesh_Init( void* remesher, int surfaceIdx ) {
    RegRemesh*	self = (RegRemesh*)remesher;

    /* RegRemesh info */
    self->surfaceIdx = surfaceIdx;
}

/*----------------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _RegRemesh_Delete( void* remesher ) {
    RegRemesh*	self = (RegRemesh*)remesher;

    /* Delete parent */
    _Stg_Component_Delete( self );
}

void _RegRemesh_Print( void* remesher, Stream* stream ) {
    RegRemesh*	self = (RegRemesh*)remesher;
    Stream*					myStream;
	
    /* Set the Journal for printing informations */
    myStream = Journal_Register( InfoStream_Type, (Name)"RegRemeshStream"  );

    /* Print parent */
    _Stg_Component_Print( self, stream );

    /* General info */
    Journal_Printf( myStream, "RegRemesh (ptr): (%p)\n", self );

    /* Virtual info */

    /* RegRemesh info */
}

void _RegRemesh_AssignFromXML( void* remesher, Stg_ComponentFactory* cf, void* data ) {
    RegRemesh* self = (RegRemesh*)remesher;
    int surfaceIdx;

    assert( self );
    assert( cf );

    _Remesher_AssignFromXML( remesher, cf, data );

    surfaceIdx = Stg_ComponentFactory_GetInt( cf, self->name, (Dictionary_Entry_Key)"SurfaceIndex", -1 );
    _RegRemesh_Init( self, surfaceIdx );
}

void _RegRemesh_Build( void* remesher, void* data ) {
    RegRemesh* self = (RegRemesh*)remesher;

    assert( self );

    _Remesher_Build( self, data );
}


void _RegRemesh_Initialise( void* remesher, void* data ) {
    RegRemesh*	self = (RegRemesh*)remesher;

    assert( self );

    _Remesher_Initialise( self, data );

    /* If we were not given a surface index, set it to be the top wall. */
    if( self->surfaceIdx == -1 ) {
        Grid* vertGrid;

        vertGrid = *Mesh_GetExtension( self->mesh, Grid**,  self->mesh->vertGridId );
        self->surfaceIdx = vertGrid->sizes[1] - 1;
    }
}


void _RegRemesh_Execute( void* remesher, void* data ) {
    RegRemesh* self = (RegRemesh*)remesher;
    Mat A;
    Vec b;
    int *eqs;

    assert( self );

    /* Go!! */

    RegRemesh_BuildEqs( self, &eqs );
    RegRemesh_BuildSystem( self, eqs, &A, &b );
    RegRemesh_Solve( self, A, b );

    free( eqs );
    Stg_MatDestroy(&A );
    Stg_VecDestroy(&b );

}

void RegRemesh_BuildEqs( RegRemesh* self, int** _eqs ) {
    Mesh *mesh;
    int numDims;
    int myRank, numRanks;
    int lNumNodes, dNumNodes;
    int *eqs, base;
    MPI_Status stat;
    Sync *sync;
    int ii;

    MPI_Comm_rank( MPI_COMM_WORLD, &myRank );
    MPI_Comm_size( MPI_COMM_WORLD, &numRanks );

    mesh = self->mesh;
    numDims = Mesh_GetDimSize( mesh );
    dNumNodes = Mesh_GetDomainSize( mesh, 0 );
    lNumNodes = Mesh_GetLocalSize( mesh, 0 );

    eqs = (int*)malloc( numDims*dNumNodes*sizeof(int) );

    if( myRank > 0 )
        (void)MPI_Recv( &base, 1, MPI_INT, myRank - 1, 10, MPI_COMM_WORLD, &stat );
    else
        base = 0;

    for( ii = 0; ii < numDims*lNumNodes; ii++ )
        eqs[ii] = base++;

    if( myRank < numRanks - 1 )
        (void)MPI_Send( &base, 1, MPI_INT, myRank + 1, 10, MPI_COMM_WORLD );

    sync = Mesh_GetSync( mesh, 0 );
    Sync_SyncArray( sync, eqs, numDims*sizeof(int), eqs + numDims*lNumNodes, numDims*sizeof(int), numDims*sizeof(int) );

    *_eqs = eqs;
}

void RegRemesh_BuildSystem( RegRemesh* self, int* eqs, Mat* _A, Vec* _b ) {
    Mesh *mesh;
    int numDims, lNumNodes;
    Mat A;
    Vec b;
    Grid *nodeGrid;
    int dimIdx, lNodeIdx, gNodeIdx;
    int nodeParam[3];
    int rowEq, colEqs[3];
    double vals[3];
    int ii, jj;

    mesh = self->mesh;
    numDims = Mesh_GetDimSize( mesh );
    lNumNodes = Mesh_GetLocalSize( mesh, 0 );
    nodeGrid = *Mesh_GetExtension( mesh, Grid**,  mesh->vertGridId );

    Stg_MatCreateAIJ( MPI_COMM_WORLD,
                     numDims*lNumNodes, numDims*lNumNodes,
                     PETSC_DECIDE, PETSC_DECIDE,
                     3, PETSC_NULL, 3, PETSC_NULL,
                     &A );

    VecCreateMPI( MPI_COMM_WORLD, numDims*lNumNodes, PETSC_DECIDE, &b );

    vals[0] = vals[1] = -0.5;
    vals[2] = 1.0;

    for( ii = 0; ii < numDims; ii++ ) {

        dimIdx = ii;

        for( jj = 0; jj < lNumNodes; jj++ ) {

            lNodeIdx = jj;
            gNodeIdx = Mesh_DomainToGlobal( mesh, 0, lNodeIdx );
            rowEq = eqs[numDims*lNodeIdx + dimIdx];

            Grid_Lift( nodeGrid, gNodeIdx, nodeParam );

            if( nodeParam[dimIdx] == 0 || nodeParam[dimIdx] == nodeGrid->sizes[dimIdx] - 1 || (nodeParam[1] == self->surfaceIdx && dimIdx == 1) ) {

                /* BC. */
                MatSetValue( A, rowEq, rowEq, 1.0, INSERT_VALUES );
                VecSetValue( b, rowEq, Mesh_GetVertex( mesh, lNodeIdx )[dimIdx], INSERT_VALUES );

            }
            else {

                nodeParam[dimIdx]--;
                gNodeIdx = Grid_Project( nodeGrid, nodeParam );
                insist( Mesh_GlobalToDomain( mesh, 0, gNodeIdx, &lNodeIdx ), == True );
                colEqs[0] = eqs[numDims*lNodeIdx + dimIdx];
                nodeParam[dimIdx]++;

                nodeParam[dimIdx]++;
                gNodeIdx = Grid_Project( nodeGrid, nodeParam );
                insist( Mesh_GlobalToDomain( mesh, 0, gNodeIdx, &lNodeIdx ), == True );
                colEqs[1] = eqs[numDims*lNodeIdx + dimIdx];
                nodeParam[dimIdx]--;

                colEqs[2] = rowEq;

                MatSetValues( A, 1, &rowEq, 3, colEqs, vals, INSERT_VALUES );

            }

        }

    }

    MatAssemblyBegin( A, MAT_FINAL_ASSEMBLY );
    VecAssemblyBegin( b );
    MatAssemblyEnd( A, MAT_FINAL_ASSEMBLY );
    VecAssemblyEnd( b );

    *_A = A;
    *_b = b;
}

void RegRemesh_Solve( RegRemesh* self, Mat A, Vec b ) {
    Mesh* mesh;
    int numDims, lNumNodes;
    Vec x;
    KSP ksp;
    double *vals;
    int valIdx;
    int ii, jj;

    mesh = self->mesh;
    numDims = Mesh_GetDimSize( mesh );
    lNumNodes = Mesh_GetLocalSize( mesh, 0 );

    VecDuplicate( b, &x );

    KSPCreate( MPI_COMM_WORLD, &ksp );
    Stg_KSPSetOperators( ksp, A, A, DIFFERENT_NONZERO_PATTERN );
    KSPSolve( ksp, b, x );
    Stg_KSPDestroy(&ksp );

    VecGetArray( x, &vals );
    valIdx = 0;

    for( jj = 0; jj < lNumNodes; jj++ ) {
        for( ii = 0; ii < numDims; ii++)
            Mesh_GetVertex( mesh, jj )[ii] = vals[valIdx++];
    }

    VecRestoreArray( x, &vals );
    Stg_VecDestroy(&x );

    /* Synchronise the mesh. */
    Mesh_Sync( mesh );
}

void _RegRemesh_Destroy( void* remesher, void* data ) {
    RegRemesh*	self = (RegRemesh*)remesher;

    assert( self );

    /* Delete the class itself */
}


/*----------------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/


/*----------------------------------------------------------------------------------------------------------------------------------
** Private Functions
*/
