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
#include <mpi.h>

#include "pcu/pcu.h"
#include "StGermain/Base/Foundation/Foundation.h"
#include "StGermain/Base/IO/IO.h"
#include "StGermain/Base/Container/Container.h"
#include "RangeSetSuite.h"

#define NUM_ITEMS 100

typedef struct {
   RangeSet*    set0;
   RangeSet*    set1;
} RangeSetSuiteData;


void RangeSetSuite_Setup( RangeSetSuiteData* data ) {
   //Index         idx;

   data->set0 = RangeSet_New();
   data->set1 = RangeSet_New();
}


void RangeSetSuite_Teardown( RangeSetSuiteData* data ) {
   FreeObject( data->set0 );
   FreeObject( data->set1 );
}


void RangeSetSuite_TestIndices( RangeSetSuiteData* data ) {
   unsigned   nInds = NUM_ITEMS;
   unsigned   inds[NUM_ITEMS];
   unsigned   nDstInds, *dstInds;
   unsigned   i;
   unsigned   ind_i;

   for( i = 0; i < nInds; i++ ) {
      inds[i] = i;
   }

   RangeSet_SetIndices( data->set0, nInds, inds );
   pcu_check_true( RangeSet_GetSize( data->set0 ) == nInds );
   pcu_check_true( RangeSet_GetNumRanges( data->set0 ) == 1 );

   dstInds = NULL;
   RangeSet_GetIndices( data->set0, &nDstInds, &dstInds );

   pcu_check_true( nDstInds == nInds );

   for( ind_i = 0; ind_i < nDstInds; ind_i++ ) {
      pcu_check_true( dstInds[ind_i] == inds[ind_i] );
   }
   FreeArray( dstInds );
}


void RangeSetSuite_TestRanges( RangeSetSuiteData* data ) {
   unsigned          nInds = NUM_ITEMS;
   unsigned          inds[NUM_ITEMS];
   unsigned          nDstInds, *dstInds;
   unsigned          i;
   RangeSet_Range*   rng;
   unsigned          ind_i;

   for( i = 0; i < nInds; i++ )
      inds[i] = (i/10)*10 + i;

   RangeSet_SetIndices( data->set0, nInds, inds );

   pcu_check_true( RangeSet_GetSize( data->set0 ) == nInds );
   pcu_check_true( RangeSet_GetNumRanges( data->set0 ) == nInds/10 );

   for( i = 0; i < nInds; i++ ) {

      rng = RangeSet_GetRange( data->set0, i / 10 );
      pcu_check_true( rng->begin == (i/10)*20 ); 
      pcu_check_true( rng->end == (i/10)*20 + 10 );
      pcu_check_true( rng->step == 1 );
   }

   dstInds = NULL;
   RangeSet_GetIndices( data->set0, &nDstInds, &dstInds );
   for( ind_i = 0; ind_i < nDstInds; ind_i++ ) {
      pcu_check_true( dstInds[ind_i] == inds[ind_i] );
   }
   FreeArray( dstInds );
}


void RangeSetSuite_TestUnion( RangeSetSuiteData* data ) {
   unsigned   nInds = NUM_ITEMS;
   unsigned   inds0[NUM_ITEMS];
   unsigned   inds1[NUM_ITEMS];
   unsigned   nDstInds, *dstInds;
   unsigned   i;
   unsigned   ind_i;

   for( i = 0; i < nInds; i++ )
      inds0[i] = (i/10)*10 + i;
   RangeSet_SetIndices( data->set0, nInds, inds0 );

   for( i = 0; i < nInds; i++ )
      inds1[i] = (i/10)*10 + NUM_ITEMS + i;
   RangeSet_SetIndices( data->set1, nInds, inds1 );

   RangeSet_Union( data->set0, data->set1 );
   pcu_check_true( RangeSet_GetSize( data->set0 ) == nInds + nInds / 2 ); 
   pcu_check_true( RangeSet_GetNumRanges( data->set0 ) == nInds / 10 + nInds / 20 );

   dstInds = NULL;
   RangeSet_GetIndices( data->set0, &nDstInds, &dstInds );
   /* During the union, the 2nd half of the second set of indices should have been added on */
   for( ind_i = 0; ind_i < nInds; ind_i++ ) {
      pcu_check_true( dstInds[ind_i] == inds0[ind_i] );
   }
   for( ind_i = 0; ind_i < nInds/2; ind_i++ ) {
      pcu_check_true( dstInds[nInds+ind_i] == inds1[nInds/2+ind_i] );
   }
   FreeArray( dstInds );
}


void RangeSetSuite_TestIntersection( RangeSetSuiteData* data ) {
   unsigned   nInds = NUM_ITEMS;
   unsigned   inds0[NUM_ITEMS];
   unsigned   inds1[NUM_ITEMS];
   unsigned   nDstInds, *dstInds;
   unsigned   i;
   unsigned   ind_i;

   for( i = 0; i < nInds; i++ )
      inds0[i] = (i/10)*10 + i;
   RangeSet_SetIndices( data->set0, nInds, inds0 );

   for( i = 0; i < nInds; i++ )
      inds1[i] = (i/10)*10 + NUM_ITEMS + i;
   RangeSet_SetIndices( data->set1, nInds, inds1 );

   RangeSet_Intersection( data->set0, data->set1 );
   pcu_check_true( RangeSet_GetSize( data->set0 ) == nInds / 2 ); 
   pcu_check_true( RangeSet_GetNumRanges( data->set0 ) == nInds / 20 );

   dstInds = NULL;
   /* During the union, the 2nd half of the first set (first half of second set) of indices should
    * be the result */
   RangeSet_GetIndices( data->set0, &nDstInds, &dstInds );
   for( ind_i = 0; ind_i < nDstInds; ind_i++ ) {
      pcu_check_true( dstInds[ind_i] == inds0[nInds/2+ind_i] );
   }
   FreeArray( dstInds );
}


void RangeSetSuite_TestSubtraction( RangeSetSuiteData* data ) {
   unsigned   nInds = NUM_ITEMS;
   unsigned   inds0[NUM_ITEMS];
   unsigned   inds1[NUM_ITEMS];
   unsigned   nDstInds, *dstInds;
   unsigned   i;
   unsigned   ind_i;

   for( i = 0; i < nInds; i++ )
      inds0[i] = (i/10)*10 + i;
   RangeSet_SetIndices( data->set0, nInds, inds0 );

   for( i = 0; i < nInds; i++ )
      inds1[i] = (i/10)*10 + NUM_ITEMS + i;
   RangeSet_SetIndices( data->set1, nInds, inds1 );

   RangeSet_Subtraction( data->set0, data->set1 );
   pcu_check_true( RangeSet_GetSize( data->set0 ) == nInds / 2 ); 
   pcu_check_true( RangeSet_GetNumRanges( data->set0 ) == nInds / 20 );

   dstInds = NULL;
   RangeSet_GetIndices( data->set0, &nDstInds, &dstInds );
   for( ind_i = 0; ind_i < nDstInds; ind_i++ ) {
      pcu_check_true( dstInds[ind_i] == inds0[ind_i] );
   }
   FreeArray( dstInds );
}


void RangeSetSuite_TestPickle( RangeSetSuiteData* data ) {
   unsigned   nInds = NUM_ITEMS;
   unsigned   inds[NUM_ITEMS];
   unsigned   nBytes;
   Stg_Byte*  bytes;
   unsigned   nDstInds, *dstInds;
   unsigned   i;
   unsigned   ind_i;

   for( i = 0; i < nInds; i++ )
      inds[i] = (i/10)*10 + i;
   RangeSet_SetIndices( data->set0, nInds, inds );

   RangeSet_Pickle( data->set0, &nBytes, &bytes );
   RangeSet_Clear( data->set0 );
   RangeSet_Unpickle( data->set0, nBytes, bytes );

   pcu_check_true( RangeSet_GetSize( data->set0 ) == nInds ); 
   pcu_check_true( RangeSet_GetNumRanges( data->set0 ) == nInds/10 );

   for( i = 0; i < nInds; i++ ) {
      RangeSet_Range*   rng;

      rng = RangeSet_GetRange( data->set0, i / 10 );
      pcu_check_true( rng->begin == (i/10)*20 ); 
      pcu_check_true( rng->end == (i/10)*20 + 10 ); 
      pcu_check_true( rng->step == 1 );
   }

   dstInds = NULL;
   RangeSet_GetIndices( data->set0, &nDstInds, &dstInds );
   for( ind_i = 0; ind_i < nDstInds; ind_i++ ) {
      pcu_check_true( dstInds[ind_i] == inds[ind_i] );
   }
   FreeArray( dstInds );
}


void RangeSetSuite( pcu_suite_t* suite ) {
   pcu_suite_setData( suite, RangeSetSuiteData );
   pcu_suite_setFixtures( suite, RangeSetSuite_Setup, RangeSetSuite_Teardown );
   pcu_suite_addTest( suite, RangeSetSuite_TestIndices );
   pcu_suite_addTest( suite, RangeSetSuite_TestRanges );
   pcu_suite_addTest( suite, RangeSetSuite_TestUnion );
   pcu_suite_addTest( suite, RangeSetSuite_TestIntersection );
   pcu_suite_addTest( suite, RangeSetSuite_TestSubtraction );
   pcu_suite_addTest( suite, RangeSetSuite_TestPickle );
}


