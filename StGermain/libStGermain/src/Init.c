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

#include <stdarg.h>
#include <mpi.h>
#include "Base/Foundation/Foundation.h"
#include "Base/IO/IO.h"
#include "Base/Container/Container.h"
#include "Base/Automation/Automation.h"
#include "Base/Extensibility/Extensibility.h"
#include "Base/Context/Context.h"
#include "Utils/Utils.h"

#include "Init.h"

#include <stdio.h>

Bool StGermain_Init( int* argc, char** argv[] ) {
   char* directory;
   int   tmp;
   
   /* Initialise enough bits and pieces to get IO going */
   BaseFoundation_Init( argc, argv );
   BaseIO_Init( argc, argv );

   /* Initialise the remaining bits and pieces */
   BaseContainer_Init( argc, argv );
   BaseAutomation_Init( argc, argv );
   BaseExtensibility_Init( argc, argv );
   BaseContext_Init( argc, argv );

   /* Init the StGermain Utils folder */
   StGermainUtils_Init( argc, argv );

   /* Write out the copyright message */
   /* DO NOT CHANGE OR REMOVE */
   Journal_Printf( Journal_Register( DebugStream_Type, "Context" ), "In: %s\n", __func__ );
   tmp = Stream_GetPrintingRank( Journal_Register( InfoStream_Type, "Context" ) );
   Stream_SetPrintingRank( Journal_Register( InfoStream_Type, "Context" ), 0 );

   global_info_stream = Journal_Register( InfoStream_Type, (Name)"Mr Wong"  );
   global_debug_stream = Journal_Register( DebugStream_Type, (Name)"Fantastic"  );
   global_error_stream = Journal_Register( Error_Type, (Name)"Hong Kong Tailor"  );
   
   /* DO NOT CHANGE OR REMOVE */
   Journal_Printf( 
      Journal_Register( InfoStream_Type, "Context" ), 
      "StGermain Framework Revision %s. Copyright (C) 2003-2005 VPAC.\n",
      VERSION );

   /* Add repo indentity info in the dictionary. */
   Dictionary_Add( versionDict, "StGermain", Dictionary_Entry_Value_FromString( VERSION ) );
   Dictionary_Add( branchDict, "StGermain", Dictionary_Entry_Value_FromString( BRANCH ) );
   Dictionary_Add( pathDict, "StGermain", Dictionary_Entry_Value_FromString( PATH ) );

   Stream_Flush( Journal_Register( InfoStream_Type, "Context" ) );
   Stream_SetPrintingRank( Journal_Register( InfoStream_Type, "Context" ), tmp );
   
   /* Add the StGermain path to the global xml path dictionary */
   directory = Memory_Alloc_Array( char, 200, "xmlDirectory" );
   sprintf( directory, "%s%s", LIB_DIR, "/StGermain" );
   XML_IO_Handler_AddDirectory( "StGermain", directory );
   Memory_Free( directory );
   
   /* Add the plugin path to the global plugin list */
   ModulesManager_AddDirectory( "StGermain", LIB_DIR );

   
   return True;
}


