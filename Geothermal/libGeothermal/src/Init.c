/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University 
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 		* Redistributions of source code must retain the above copyright notice, 
** 			this list of conditions and the following disclaimer.
** 		* Redistributions in binary form must reproduce the above copyright 
**			notice, this list of conditions and the following disclaimer in the 
**			documentation and/or other materials provided with the distribution.
** 		* Neither the name of the Monash University nor the names of its contributors 
**			may be used to endorse or promote products derived from this software 
**			without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
** THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
** PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS 
** BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
** OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**
** Contact:
*%  Louis.Moresi - Louis.Moresi@monash.edu
*%
** Contributors:
** 
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include "Geothermal.h"

#include <stdio.h>

/** Initialises this package, then any init for this package
such as streams etc */

Bool Geothermal_Init( int* argc, char** argv[] ) {
	/* 
    * This init function tells StGermain of all the component types, etc this module contributes.
    * Because it can be linked at compile time or linked in by a toolbox at runtime, we need to make
    * sure it isn't run twice (compiled in and loaded through a toolbox.
    */
	if( !ToolboxesManager_IsInitialised( stgToolboxesManager, "Geothermal" ) ) {
		int tmp;
		char* directory;

		Geothermal_Base_Init(argc, argv);
 
      /* DO NOT CHANGE OR REMOVE */
		Journal_Printf( Journal_Register( DebugStream_Type, (Name)"Context"  ), "In: %s\n", __func__ );
		tmp = Stream_GetPrintingRank( Journal_Register( InfoStream_Type, (Name)"Context" )  );
		Stream_SetPrintingRank( Journal_Register( InfoStream_Type, (Name)"Context"  ), 0 );

      /* DO NOT CHANGE OR REMOVE */
		Journal_Printf( 
			Journal_Register( InfoStream_Type, (Name)"Context"  ), 
			"Geothermal (Bleeding Edge Geodynamics framework) Revision %s. Copyright (C) 2005 Monash University.\n",
         VERSION );

      /* Add repo indentity info in the repo dictionary. */
      Dictionary_Add( versionDict, "Geothermal", Dictionary_Entry_Value_FromString( VERSION ) );
      Dictionary_Add( branchDict, "Geothermal", Dictionary_Entry_Value_FromString( BRANCH ) );
      Dictionary_Add( pathDict, "Geothermal", Dictionary_Entry_Value_FromString( PATH ) );

		Stream_Flush( Journal_Register( InfoStream_Type, (Name)"Context" )  );
		Stream_SetPrintingRank( Journal_Register( InfoStream_Type, (Name)"Context"  ), tmp );

		/* Add the Geothermal path to the global xml path dictionary */
		directory = Memory_Alloc_Array( char, 200, "xmlDirectory" ) ;
		sprintf(directory, "%s%s", LIB_DIR, "/StGermain" );
		XML_IO_Handler_AddDirectory( "Geothermal", directory );
		Memory_Free(directory);

		/* Add the plugin path to the global plugin list */
		ModulesManager_AddDirectory( "Geothermal", LIB_DIR );
	
		return True;
	}
	return False;
}


