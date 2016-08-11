/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd,
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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgDomain_Utils_PETScCompatibility_h__
#define __StgDomain_Utils_PETScCompatibility_h__

#include "petsc.h"
#include "petscversion.h"
#ifdef I
#undef I
#endif

#if ( (PETSC_VERSION_MAJOR==3) && (PETSC_VERSION_MINOR>=5) )
   #define Stg_KSPGetOperators(a1,a2,a3,a4) KSPGetOperators(a1,a2,a3)
   #define Stg_KSPSetOperators(a1,a2,a3,a4) KSPSetOperators(a1,a2,a3)
   #define Stg_PCGetOperators(a1,a2,a3,a4) PCGetOperators(a1,a2,a3)
   #define Stg_PCSetOperators(a1,a2,a3,a4) PCSetOperators(a1,a2,a3)
   #define Stg_PetscNew(a1,a2) PetscNew(a2)
#else
   #define Stg_KSPGetOperators(a1,a2,a3,a4) KSPGetOperators(a1,a2,a3,a4)
   #define Stg_KSPSetOperators(a1,a2,a3,a4) KSPSetOperators(a1,a2,a3,a4)
   #define Stg_PCGetOperators(a1,a2,a3,a4) PCGetOperators(a1,a2,a3,a4)
   #define Stg_PCSetOperators(a1,a2,a3,a4) PCSetOperators(a1,a2,a3,a4)
   #define Stg_PetscNew(a1,a2) PetscNew(a1,a2)
#endif

#if (((PETSC_VERSION_MAJOR==3) && (PETSC_VERSION_MINOR>=4)) || (PETSC_VERSION_MAJOR>3) )
   #define Stg_PCMGDefaultResidual NULL
   #define KSP_DIVERGED_NAN KSP_DIVERGED_NANORINF
   #define Stg_VecRegister(a1,a2,a3,a4) VecRegister(a1,a4)
#if ( (PETSC_VERSION_MAJOR==3) && (PETSC_VERSION_MINOR>=5) )
   #define Stg_PetscObjectDepublish PetscObjectSAWsViewOff
   #define PetscObjectGrantAccess PetscObjectSAWsGrantAccess
   #define PetscObjectTakeAccess PetscObjectSAWsTakeAccess
#else
   #define Stg_PetscObjectDepublish PetscObjectAMSViewOff
   #define PetscObjectTakeAccess PetscObjectAMSTakeAccess
   #define PetscObjectGrantAccess PetscObjectAMSGrantAccess
#endif
   #define PetscGetTime PetscTime
   #define PetscObjectComposeFunctionDynamic( arg1, arg2, arg3, arg4 ) PetscObjectComposeFunction( arg1, arg2, arg4 )
   #define Stg_MatRegister(a1,a2,a3,a4) MatRegister(a1,a4)
   #define Stg_KSPRegister(a1,a2,a3,a4) KSPRegister(a1,a4)
   #define Stg_SNESRegister(a1,a2,a3,a4) SNESRegister(a1,a4)
   #define Stg_PCRegister(a1,a2,a3,a4) PCRegister(a1,a4)
   #define KSPDefaultDestroy KSPDestroyDefault
   #define SNESDefaultConverged SNESConvergedDefault
   #define SNESLogConvHistory SNESLogConvergenceHistory
   #define KSPDefaultBuildSolution KSPBuildSolutionDefault
   #define KSPDefaultBuildResidual KSPBuildResidualDefault
   #define PETSC_NULL NULL
#else
   #define Stg_PCMGDefaultResidual PCMGDefaultResidual
   #define Stg_VecRegister(a1,a2,a3,a4) VecRegister(a1,a2,a3,a4)
   #define Stg_PetscObjectDepublish PetscObjectDepublish
   #define Stg_MatRegister MatRegister
   #define Stg_KSPRegister KSPRegister
   #define Stg_SNESRegister SNESRegister
   #define Stg_PCRegister PCRegister
#endif

#if ( (PETSC_VERSION_MAJOR >= 3) && (PETSC_VERSION_MINOR >= 2 ) )
  	#define Stg_SETERRQ( arg1, arg2 ) SETERRQ( PETSC_COMM_SELF, arg1, arg2 )
  	#define Stg_SETERRQ1( arg1, arg2, arg3 ) SETERRQ1( PETSC_COMM_SELF, arg1, arg2, arg3 )
  	#define Stg_SETERRQ2( arg1, arg2, arg3, arg4 ) SETERRQ2( PETSC_COMM_SELF, arg1, arg2, arg3, arg4 )
    #define Stg_SETERRQ3( arg1, arg2, arg3, arg4, arg5 ) SETERRQ3( PETSC_COMM_SELF, arg1, arg2, arg3, arg4, arg5 )
    #define Stg_SETERRQ4( arg1, arg2, arg3, arg4, arg5, arg6 ) SETERRQ4( PETSC_COMM_SELF, arg1, arg2, arg3, arg4, arg5, arg6 )
  	#define Stg_MatZeroRows( arg1, arg2, arg3, arg4 ) MatZeroRows( arg1, arg2, arg3, arg4, NULL, NULL )
  	#define ISCreateGeneralWithArray( arg1, arg2, arg3, arg4 ) ISCreateGeneral( arg1, arg2, arg3, PETSC_USE_POINTER, arg4 )
  	#define PetscTruth PetscBool
  	#define PetscOptionsGetTruth PetscOptionsGetBool
    #define PetscOptionsTruth PetscOptionsBool
  	#define PETSC_VIEWER_BINARY PETSCVIEWERBINARY
    #define PETSC_VIEWER_ASCII PETSCVIEWERASCII
    #define VecSqrt VecSqrtAbs
    #define PetscPublishAll(v)  0
    #define PetscViewerASCIIMonitorCreate(a1,a2,a3,a4) 0
    #define PetscViewerASCIIMonitorDestroy(args) 0
    #define PetscViewerASCIIMonitor PetscViewer
    #define PetscViewerASCIIMonitorPrintf PetscViewerASCIIPrintf
    #define KSP_NORM_NO KSP_NORM_NONE
    #define KSPSetPreconditionerSide KSPSetPCSide
    #define KSPGetPreconditionerSide KSPGetPCSide
    #if !defined(EXTERN)
    #define EXTERN extern
    #endif
   #if ( PETSC_VERSION_MINOR > 2  )
      #define PETSC_EXTERN_CXX_BEGIN EXTERN_C_BEGIN
      #define PETSC_EXTERN_CXX_END EXTERN_C_END
      #define Stg_MatCreateAIJ MatCreateAIJ
      #define Stg_PetscObjectTypeCompare PetscObjectTypeCompare
      #define Stg_PetscTypeCompare PetscObjectTypeCompare
   #else
      #define Stg_MatCreateAIJ MatCreateMPIAIJ
      #define Stg_PetscObjectTypeCompare PetscTypeCompare
      #define Stg_PetscTypeCompare PetscTypeCompare
   #endif
#else
/* need these for Mat Vec KSP etc in versions of petsc < 3.2 */
   #include "petscmat.h"
   #include "petscksp.h"
   #include "petscvec.h"
   #include "petscpc.h"
   #include "petscsnes.h"
   #define Stg_SETERRQ SETERRQ
   #define Stg_SETERRQ1 SETERRQ1
   #define Stg_SETERRQ2 SETERRQ2
   #define Stg_SETERRQ3 SETERRQ3
   #define Stg_SETERRQ4 SETERRQ4
   #define Stg_MatZeroRows MatZeroRows
   #define Stg_MatCreateAIJ MatCreateMPIAIJ
   #define Stg_PetscObjectTypeCompare PetscTypeCompare
   #define Stg_PetscTypeCompare PetscTypeCompare
#endif
/* wrapper functions for compatibility between Petsc versions >= 3.2 and lower versions */
PetscErrorCode Stg_MatDestroy(Mat *A);
PetscErrorCode Stg_VecDestroy(Vec *A);
PetscErrorCode Stg_KSPDestroy(KSP *A);
PetscErrorCode Stg_PCDestroy(PC *A);
PetscErrorCode Stg_SNESDestroy(SNES *A);
PetscErrorCode Stg_VecScatterDestroy(VecScatter *ctx);
PetscErrorCode Stg_ISDestroy(IS *is);
PetscErrorCode Stg_PetscViewerDestroy(PetscViewer *viewer);
/** lets make Stg_MatLoad look the same as pre-petsc_version 3.2 MatLoad */
PetscErrorCode Stg_MatLoad(PetscViewer viewer, const MatType outtype,Mat *newmat);
PetscErrorCode Stg_VecLoad(PetscViewer viewer, const VecType outtype,Vec *newvec);


#endif /* __StgDomain_Utils_PETScCompatibility_h__ */

