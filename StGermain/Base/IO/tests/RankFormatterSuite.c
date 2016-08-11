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
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdio.h>
#include <stdlib.h>

#include "pcu/pcu.h"
#include "StGermain/Base/Foundation/Foundation.h"
#include "StGermain/Base/IO/IO.h"
#include "RankFormatterSuite.h"

typedef struct {
   int            myRank;
   int            nProcs;
} RankFormatterSuiteData;


void RankFormatterSuite_Setup( RankFormatterSuiteData* data ) {
   MPI_Comm_rank( MPI_COMM_WORLD, &data->myRank );
   MPI_Comm_size( MPI_COMM_WORLD, &data->nProcs );
}

void RankFormatterSuite_Teardown( RankFormatterSuiteData* data ) {
}
   

void RankFormatterSuite_TestPrintWithRank( RankFormatterSuiteData* data ) {
   Index       ii=0;
   Stream*     myInfo = NULL;      
   #define     MAXLINE 1000
   char        expLine[MAXLINE];
   char        outLine[MAXLINE];
   char*       testOutFilename = NULL;
   FILE*       testOutFile = NULL;
   char*       prefixStr = NULL;

   Stg_asprintf( &testOutFilename, "./testRankFormatter.%d.txt", data->myRank );

   if ( 1 == data->nProcs ) {
      Stg_asprintf( &prefixStr, "");
   }
   else {
      Stg_asprintf( &prefixStr, "%d: ", data->myRank );
   }

   myInfo = Journal_Register( Info_Type, (Name)"MyInfo"  );
   Stream_AddFormatter( myInfo, RankFormatter_New() );
   Stream_SetPrintingRank( myInfo, STREAM_ALL_RANKS );
   Stream_RedirectFile( myInfo, testOutFilename );

   testOutFile = fopen( testOutFilename, "r" );

   Journal_Printf( myInfo, "Hello world\n" );
   sprintf( expLine, "%sHello world\n", prefixStr );
   pcu_check_true(         fgets( outLine, MAXLINE, testOutFile ));
   pcu_check_streq( outLine, expLine );
   Journal_Printf( myInfo, "\n\n\n" );
   sprintf( expLine, "%s\n", prefixStr );
   for (ii=0; ii<3; ii++ ) {
      pcu_check_true(         fgets( outLine, MAXLINE, testOutFile ));
      pcu_check_streq( outLine, expLine );
   }
   Journal_Printf( myInfo, "abc\ndef\nghi\n" );
   sprintf( expLine, "%sabc\n", prefixStr );
   pcu_check_true(         fgets( outLine, MAXLINE, testOutFile ));
   pcu_check_streq( outLine, expLine );
   sprintf( expLine, "%sdef\n", prefixStr );
   pcu_check_true(         fgets( outLine, MAXLINE, testOutFile ));
   pcu_check_streq( outLine, expLine );
   sprintf( expLine, "%sghi\n", prefixStr );
   pcu_check_true(         fgets( outLine, MAXLINE, testOutFile ));
   pcu_check_streq( outLine, expLine );

   fclose( testOutFile );
   remove( testOutFilename );
}


void RankFormatterSuite( pcu_suite_t* suite ) {
   pcu_suite_setData( suite, RankFormatterSuiteData );
   pcu_suite_setFixtures( suite, RankFormatterSuite_Setup, RankFormatterSuite_Teardown );
   pcu_suite_addTest( suite, RankFormatterSuite_TestPrintWithRank );
}


