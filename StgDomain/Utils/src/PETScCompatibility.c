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
#include "PETScCompatibility.h"

/* wrapper functions for compatibility between Petsc version 3.2 and lower versions */

#if ((PETSC_VERSION_MAJOR==3) && (PETSC_VERSION_MINOR>=2))
   #undef __FUNCT__
   #define __FUNCT__ "Stg_MatDestroy"
   PetscErrorCode Stg_MatDestroy(Mat *A){
       return MatDestroy(A);
   }
   #undef __FUNCT__
   #define __FUNCT__ "Stg_VecDestroy"
   PetscErrorCode Stg_VecDestroy(Vec *A){
      return VecDestroy(A);
   }
   #undef __FUNCT__
   #define __FUNCT__ "Stg_KSPDestroy"
   PetscErrorCode Stg_KSPDestroy(KSP *A){
      return KSPDestroy(A);
   }
   #undef __FUNCT__
   #define __FUNCT__ "Stg_PCDestroy"
   PetscErrorCode Stg_PCDestroy(PC *A){
      return PCDestroy(A);
   }
   #undef __FUNCT__
   #define __FUNCT__ "Stg_SNESDestroy"
   PetscErrorCode Stg_SNESDestroy(SNES *A){
      return SNESDestroy(A);
   }
   #undef __FUNCT__
   #define __FUNCT__ "Stg_VecScatterDestroy"
   PetscErrorCode Stg_VecScatterDestroy(VecScatter *A){
      return VecScatterDestroy(A);
   }
   #undef __FUNCT__
   #define __FUNCT__ "Stg_ISDestroy"
   PetscErrorCode Stg_ISDestroy(IS *A){
      return ISDestroy(A);
   }
   #undef __FUNCT__
   #define __FUNCT__ "Stg_PetscViewerDestroy"
   PetscErrorCode Stg_PetscViewerDestroy(PetscViewer *A){
      return PetscViewerDestroy(A);
   }

   #undef __FUNCT__  
   #define __FUNCT__ "Stg_MatLoad"
   PetscErrorCode Stg_MatLoad(PetscViewer viewer, const MatType outtype, Mat *newmat){
       PetscErrorCode ierr;
       ierr = MatCreate( PETSC_COMM_SELF, newmat );CHKERRQ(ierr);
       ierr = MatSetType( *newmat, outtype);CHKERRQ(ierr);
       return MatLoad( *newmat, viewer);
   }
   #undef __FUNCT__  
   #define __FUNCT__ "Stg_VecLoad"
   PetscErrorCode Stg_VecLoad(PetscViewer viewer, const VecType outtype, Vec *newvec){
       PetscErrorCode ierr;
       ierr = VecCreate( PETSC_COMM_SELF, newvec );CHKERRQ(ierr);
       ierr = VecSetType( *newvec, outtype);CHKERRQ(ierr);
       return VecLoad( *newvec, viewer);
   }
#else
   #undef __FUNCT__
   #define __FUNCT__ "Stg_MatDestroy"
   PetscErrorCode Stg_MatDestroy(Mat *A){
     return MatDestroy(*A);
   }
   #undef __FUNCT__
   #define __FUNCT__ "Stg_VecDestroy"
   PetscErrorCode Stg_VecDestroy(Vec *A){
      return VecDestroy(*A);
   }
   #undef __FUNCT__
   #define __FUNCT__ "Stg_KSPDestroy"
   PetscErrorCode Stg_KSPDestroy(KSP *A){
      return KSPDestroy(*A);
   }
   #undef __FUNCT__
   #define __FUNCT__ "Stg_PCDestroy"
   PetscErrorCode Stg_PCDestroy(PC *A){
      return PCDestroy(*A);
   }
   #undef __FUNCT__
   #define __FUNCT__ "Stg_SNESDestroy"
   PetscErrorCode Stg_SNESDestroy(SNES *A){
      return SNESDestroy(*A);
   }
   #undef __FUNCT__
   #define __FUNCT__ "Stg_VecScatterDestroy"
   PetscErrorCode Stg_VecScatterDestroy(VecScatter *A){
      return VecScatterDestroy(*A);
   }
   #undef __FUNCT__
   #define __FUNCT__ "Stg_ISDestroy"
   PetscErrorCode Stg_ISDestroy(IS *A){
      return ISDestroy(*A);
   }
   #undef __FUNCT__
   #define __FUNCT__ "Stg_PetscViewerDestroy"
   PetscErrorCode Stg_PetscViewerDestroy(PetscViewer *A){
      return PetscViewerDestroy(*A);
   }

   #undef __FUNCT__  
   #define __FUNCT__ "Stg_MatLoad"
   PetscErrorCode Stg_MatLoad(PetscViewer viewer, const MatType outtype,Mat *newmat){
       return MatLoad( viewer, outtype, newmat);
   }
   #undef __FUNCT__  
   #define __FUNCT__ "Stg_VecLoad"
   PetscErrorCode Stg_VecLoad(PetscViewer viewer, const VecType outtype, Vec *newvec){
       return VecLoad( viewer, outtype, newvec);
   }
#endif


