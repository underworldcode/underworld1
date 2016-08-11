/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcu/pcu.h"
#include "StGermain/Base/Foundation/Foundation.h"
#include "StGermain/Base/IO/IO.h"
#include "StGermain/Base/Container/Container.h"
#include "MPIRoutinesSuite.h"

typedef struct {
   int rank;
   int nProcs;
} MPIRoutinesSuiteData;

void MPIRoutinesSuite_Setup( MPIRoutinesSuiteData* data ) {
   /* Assign the rank and nProcs */
   /* We can be sure MPI_Init called already, immediately in testStGermain.c the test harness code */
   MPI_Comm_rank( MPI_COMM_WORLD, &data->rank );
   MPI_Comm_size( MPI_COMM_WORLD, &data->nProcs );
}

void MPIRoutinesSuite_Teardown( MPIRoutinesSuiteData* data ) {
}


void fillArray( unsigned* array, unsigned size, unsigned rank ) {
 
   unsigned   i;

   for( i = 0; i < size; i++ )
      array[i] = rank * size + i;
}


void MPIRoutinesSuite_TestArrayConv( MPIRoutinesSuiteData* data ) {
   unsigned   nBlocks = 3;
   unsigned   sizes[3] = {2, 4, 1};
   unsigned   src[7] = {0, 1, 2, 3, 4, 5, 6};
   unsigned**   dst2D;
   unsigned*   dst1D;
   unsigned*   disps;

   Array_1DTo2D( nBlocks, sizes, src, (void***)&dst2D, sizeof(unsigned) );
   pcu_check_true( dst2D[0][0] == 0 && dst2D[0][1] == 1 && 
       dst2D[1][0] == 2 && dst2D[1][1] == 3 && dst2D[1][2] == 4 && dst2D[1][3] == 5 && 
       dst2D[2][0] == 6 );

   Array_2DTo1D( nBlocks, sizes, (void**)dst2D, (void**)&dst1D, sizeof(unsigned), &disps );
   pcu_check_true( dst1D[0] == 0 && dst1D[1] == 1 && dst1D[2] == 2 && dst1D[3] == 3 && 
       dst1D[4] == 4 && dst1D[5] == 5 && dst1D[6] == 6 && 
       disps[0] == 0 && disps[1] == 2 && disps[2] == 6 );

   /* This time it tests the function can handle a 2D array with a zero-size column */
   sizes[1] = 0;
   Array_1DTo2D( nBlocks, sizes, src, (void***)&dst2D, sizeof(unsigned) );
   pcu_check_true( dst2D[0][0] == 0 && dst2D[0][1] == 1 && 
       dst2D[2][0] == 2 );

   Array_2DTo1D( nBlocks, sizes, (void**)dst2D, (void**)&dst1D, sizeof(unsigned), &disps );
   pcu_check_true( dst1D[0] == 0 && dst1D[1] == 1 && dst1D[2] == 2 && 
       disps[0] == 0 && disps[1] == 2 && disps[2] == 2 );

   FreeArray( dst2D );
   FreeArray( dst1D );
   FreeArray( disps );
}


void MPIRoutinesSuite_TestBcast( MPIRoutinesSuiteData* data ) {
   unsigned    size = 200;
   unsigned*   src = NULL;
   Index       ii;

   if( data->rank == 0 ) {
      src = Memory_Alloc_Array_Unnamed( unsigned, size );
      fillArray( src, size, data->rank );
   }

   MPIArray_Bcast( &size, (void**)&src, sizeof(unsigned), 
         0, MPI_COMM_WORLD );

   /* Make sure the contents of the broadcast array are the same as the fillArray function predicates */
   for( ii = 0; ii < size; ii++ )
      if( src[ii] != ii ) break;

   pcu_check_true( ii == size );

   FreeArray( src );
   return;
}


void MPIRoutinesSuite_TestGather( MPIRoutinesSuiteData* data ) {
   unsigned     size = 200;
   unsigned     src[200];
   unsigned*    dstSizes = NULL;
   unsigned**   dstArrays = NULL;
   Index        ii;
   Index        watch;

   fillArray( src, size, data->rank );

   /* We should run the gather on each processor, to be thorough */
   for ( watch = 0; watch < data->nProcs; watch++ ) {
      MPIArray_Gather( size, src, &dstSizes, (void***)&dstArrays, sizeof(unsigned), watch, MPI_COMM_WORLD );

      /* Only the gathering processor in each loop needs to check the dstArrays content.
       * The others shouldn't receive anything in dstArrays */
      if ( data->rank == watch ) {
         for( ii = 0; ii < data->nProcs; ii++ ) {
            pcu_check_true( dstSizes[ii] == size );
         }

         /* Check the contents of the arrays are correct. Note, I've kept Alan's approach
          *  of breaking out of the loop when a problem's detected and pcu_asserting on
          *  the result, to avoid potentially hundreds of pcu_assert prints if there's
          *  a problem. Just one will suffice I think.
          *  -- PatrickSunter, 1 April 2009.
          */ 
         for( ii = 0; ii < data->nProcs; ii++ ) {
            unsigned   jj;

            for( jj = 0; jj < size; jj++ )
               if( dstArrays[ii][jj] != ii * size + jj ) break;
            if( jj < size ) break;
         }
         pcu_check_true( ii == data->nProcs );

         /* Free the dst arrays, so we can test they are autmatically re-created properly
          * next loop */
         FreeArray( dstSizes );
         FreeArray( dstArrays );

         dstSizes = NULL;
         dstArrays = NULL;
      }
   }

   /* Hmmm, shouldn't we test some variable size arrays as well?
    * --PatrickSunter, 1 Apr 2009 */
}


void MPIRoutinesSuite_TestAllgather( MPIRoutinesSuiteData* data ) {
   unsigned    size = 200;
   unsigned    src[200];
   unsigned*   dstSizes;
   unsigned**  dstArrays;
   Index       ii;

   fillArray( src, size, data->rank );

   /* allgather runs just once, on all processors */
   MPIArray_Allgather( size, src, &dstSizes, (void***)&dstArrays, sizeof(unsigned), MPI_COMM_WORLD );

   /* Since all processors should now have the resulting dstArrays, all can do the test
    *  concurrently */

   for( ii = 0; ii < data->nProcs; ii++ ) {
      pcu_check_true( dstSizes[ii] == size );
   }

   for( ii = 0; ii < data->nProcs; ii++ ) {
      Index   jj;

      if( !dstArrays[ii] ) break;
      for( jj = 0; jj < size; jj++ ) {
         if( dstArrays[ii][jj] != size * ii + jj ) break;
      }
      if( jj < size ) break;
   }
   pcu_check_true( ii == data->nProcs );

   FreeArray( dstSizes );
   FreeArray( dstArrays );

   return;
}


/* MPI_AlltoAll is a tricky one - basically, it interleaves the original array on each processor
 * into a different destination array for each.
 * E.g. if Proc 0 has src [0A,0B] and Proc 1 has [1A,1B], then the result of an All to All would be:
 * Proc 0 having [0A,1A] and Proc 1 having [0B,1B]
 * So this test basically checks the above case works, except with multiples
 * of 100 for different source procs.
 */
void MPIRoutinesSuite_TestAlltoall_1D( MPIRoutinesSuiteData* data ) {
   unsigned*   srcSizes;
   unsigned**  srcArrays;
   unsigned*   dstSizes;
   unsigned**  dstArrays;
   Index       procNum;
   unsigned    procMulti = 100;

   srcSizes = Memory_Alloc_Array_Unnamed( unsigned, data->nProcs );
   srcArrays = Memory_Alloc_2DArray_Unnamed( unsigned, data->nProcs, 1 );

   for( procNum = 0; procNum < data->nProcs; procNum++ ) {
      srcSizes[procNum] = 1;
      srcArrays[procNum][0] = data->rank * procMulti + procNum;
   }

   MPIArray_Alltoall( srcSizes, (void**)srcArrays, &dstSizes, (void***)&dstArrays, sizeof(unsigned), MPI_COMM_WORLD );
   FreeArray( srcSizes );
   FreeArray( srcArrays );

   for( procNum = 0; procNum < data->nProcs; procNum++ ) {
      pcu_check_true( dstSizes[procNum] == 1);
      pcu_check_true( dstArrays[procNum][0] == data->rank + procMulti * procNum );
   }

   FreeArray( dstSizes );
   FreeArray( dstArrays );

   return;
}


/* Since this StG Alltoall wrapper is supposed to work on 2D arrays, need to test this.
 * Strategy is very similar to above test, except we also use multiples of 10 to handle extra
 * dimension */
void MPIRoutinesSuite_TestAlltoall_2D( MPIRoutinesSuiteData* data ) {
   unsigned*   srcSizes;
   unsigned**  srcArrays;
   unsigned*   dstSizes;
   unsigned**  dstArrays;
   Index       procNum;
   Index       ii;
   unsigned    procMulti = 100;
   Index       SecondDimSize=10;

   srcSizes = Memory_Alloc_Array_Unnamed( unsigned, data->nProcs );
   srcArrays = Memory_Alloc_2DArray_Unnamed( unsigned, data->nProcs, SecondDimSize );

   for( procNum = 0; procNum < data->nProcs; procNum++ ) {
      srcSizes[procNum] = SecondDimSize;
      for ( ii=0; ii<SecondDimSize; ii++ ) {
         srcArrays[procNum][ii] = data->rank * procMulti + procNum * SecondDimSize + ii;
      }
   }

   MPIArray_Alltoall( srcSizes, (void**)srcArrays, &dstSizes, (void***)&dstArrays, sizeof(unsigned), MPI_COMM_WORLD );
   FreeArray( srcSizes );
   FreeArray( srcArrays );

   for( procNum = 0; procNum < data->nProcs; procNum++ ) {
      pcu_check_true( dstSizes[procNum] == SecondDimSize);
      for ( ii=0; ii<SecondDimSize; ii++ ) {
         pcu_check_true( dstArrays[procNum][ii] == data->rank * SecondDimSize + procNum * procMulti + ii );
      }
   }

   FreeArray( dstSizes );
   FreeArray( dstArrays );

   return;
}


void MPIRoutinesSuite( pcu_suite_t* suite ) {
   pcu_suite_setData( suite, MPIRoutinesSuiteData );
   pcu_suite_setFixtures( suite, MPIRoutinesSuite_Setup, MPIRoutinesSuite_Teardown );
   pcu_suite_addTest( suite, MPIRoutinesSuite_TestArrayConv );
   pcu_suite_addTest( suite, MPIRoutinesSuite_TestBcast );
   pcu_suite_addTest( suite, MPIRoutinesSuite_TestGather );
   pcu_suite_addTest( suite, MPIRoutinesSuite_TestAllgather );
   pcu_suite_addTest( suite, MPIRoutinesSuite_TestAlltoall_1D );
   pcu_suite_addTest( suite, MPIRoutinesSuite_TestAlltoall_2D );
}


