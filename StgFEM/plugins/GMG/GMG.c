/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC)
** Ltd,
** 110 Victoria Street, Melbourne, 3053, Australia.
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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <mpi.h>
#include <petsc.h>
#include <petscvec.h>
#include <petscmat.h>
#include <petscksp.h>
#include <petscsnes.h>

#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>

#include "GMG.h"

const Type StgFEM_GMG_Type = "StgFEM_GMG";
StgFEM_GMG* StgFEM_GMG_selfPointer = NULL;

void StgFEM_GMG_SolverSetup( void* _solver, void* _stokesSLE ) {
    StgFEM_GMG* self = StgFEM_GMG_selfPointer;
    Stokes_SLE_UzawaSolver* solver = (Stokes_SLE_UzawaSolver*)_solver;
    Stokes_SLE* sle = (Stokes_SLE*)_stokesSLE;
    KSP ksp = solver->velSolver;
    PC pc;
    int ii;

    Journal_DPrintf( solver->debug, "In %s:\n", __func__ );
    Stream_IndentBranch( StgFEM_Debug );

    KSPSetType( ksp, KSPFGMRES );
    KSPGetPC( ksp, &pc );
    PCSetType( pc, PCMG );
    PCMGSetLevels( pc, self->numLevels, PETSC_NULL );
    PCMGSetType( pc, PC_MG_MULTIPLICATIVE );
    #if ((PETSC_VERSION_MAJOR==3) && (PETSC_VERSION_MINOR>=2) )
    PCMGSetGalerkin( pc, PETSC_TRUE );
    #else
    PCMGSetGalerkin( pc );
    #endif
    /* Set the operators for each level. */
    {
        Mat *pOps, *rOps;
        MGOpGenerator_SetNumLevels( self->opGen, self->numLevels );
        MGOpGenerator_Generate( self->opGen, &pOps, &rOps );
        for( ii = 1; ii < self->numLevels; ii++ )
            PCMGSetInterpolation( pc, ii, pOps[ii] );
        Memory_Free( pOps );
        Memory_Free( rOps );
    }

    /* Set the solvers on each level. */
    for( ii = 1; ii < self->numLevels; ii++ ) {
        KSP smoother;
        PC smPc;
        PCMGGetSmoother( pc, ii, &smoother );
        KSPSetType( smoother, KSPRICHARDSON );
        KSPGetPC( smoother, &smPc );
        PCSetType( smPc, PCSOR );
        KSPSetTolerances( smoother, PETSC_DEFAULT, PETSC_DEFAULT,
                          PETSC_DEFAULT, 2 );
    }
    

    Stg_KSPSetOperators( ksp, sle->kStiffMat->matrix, sle->kStiffMat->matrix,
                     DIFFERENT_NONZERO_PATTERN );
    KSPSetFromOptions( ksp );

    /* Because we stole the setup routine we need to do this too. */
    if( solver->pcSolver ) {
        Journal_DPrintfL( solver->debug, 1,
                          "Setting up MatrixSolver for the "
                          "Preconditioner.\n" );
        Stg_KSPSetOperators( solver->pcSolver, solver->preconditioner->matrix,
                         solver->preconditioner->matrix,
                         DIFFERENT_NONZERO_PATTERN );
        KSPSetFromOptions( solver->pcSolver );
    }

    Stream_UnIndentBranch( StgFEM_Debug );
}


void StgFEM_GMG_AssignFromXML( void* _self, Stg_ComponentFactory* cf,
                               void* data )
{
    StgFEM_GMG* self = (StgFEM_GMG*)_self;

    StgFEM_GMG_selfPointer = self;

    self->sle = Stg_ComponentFactory_ConstructByName( cf, (Name)"stokesEqn",
                                                      Stokes_SLE, True, data );

    self->numLevels = Stg_ComponentFactory_PluginGetInt( cf, self, (Dictionary_Entry_Key)"NumLevels", 3 );

    /* Replace the setup routine... hacky. */
    self->sle->solver->_solverSetup = StgFEM_GMG_SolverSetup;   

    /* Create our operator generator. */
    self->opGen = SROpGenerator_New( "GMGOpGen" );
   
}

void StgFEM_GMG_Build( void* _self, void* data ) {
    StgFEM_GMG* self = (StgFEM_GMG*)_self;

    Stg_Component_Build( self->sle, data, False );

    /* Setup the operator generator. */
    SROpGenerator_SetFineVariable( self->opGen,
                                   self->sle->uSolnVec->feVariable );
    Stg_Component_Build( self->opGen, data, False );
}

void StgFEM_GMG_Initialise( void* _self, void* data ) {
    StgFEM_GMG* self = (StgFEM_GMG*)_self;

    Stg_Component_Initialise( self->sle, data, False );
    Stg_Component_Initialise( self->opGen, data, False );
}

void* StgFEM_GMG_New( Name name ) {
    /* Variables set in this function */
    SizeT                                              _sizeOfSelf = sizeof(StgFEM_GMG);
    Type                                                      type = StgFEM_GMG_Type;
    Stg_Class_DeleteFunction*                              _delete = _Codelet_Delete;
    Stg_Class_PrintFunction*                                _print = _Codelet_Print;
    Stg_Class_CopyFunction*                                  _copy = _Codelet_Copy;
    Stg_Component_DefaultConstructorFunction*  _defaultConstructor = StgFEM_GMG_New;
    Stg_Component_ConstructFunction*                    _construct = StgFEM_GMG_AssignFromXML;
    Stg_Component_BuildFunction*                            _build = StgFEM_GMG_Build;
    Stg_Component_InitialiseFunction*                  _initialise = StgFEM_GMG_Initialise;
    Stg_Component_ExecuteFunction*                        _execute = _Codelet_Execute;
    Stg_Component_DestroyFunction*                        _destroy = _Codelet_Destroy;

    /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
    AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

    return _Codelet_New(  CODELET_PASSARGS  );
}

Index StgFEM_GMG_Register( PluginsManager* mgr ) {
    return PluginsManager_Submit( mgr, StgFEM_GMG_Type, (Name)"0",
                                  StgFEM_GMG_New  );
}

