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
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
** 
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include "JAMBox.h"

#include <stdio.h>

/** Initialises this package, then any init for this package
such as streams etc */

Bool JAMBox_Init( int* argc, char** argv[] ) {
	/* This init function tells StGermain of all the component types, etc this module contributes. Because it can be linked at compile
	   time or linked in by a toolbox at runtime, we need to make sure it isn't run twice (compiled in and loaded through a toolbox.*/
	if( !ToolboxesManager_IsInitialised( stgToolboxesManager, "JAMBox" ) ) {
		int tmp;
		char* directory;

		JAMBox_Components_Init(argc, argv); 

		Journal_Printf( Journal_Register( DebugStream_Type, (Name)"Context"  ), "In: %s\n", __func__ ); /* DO NOT CHANGE OR REMOVE */
		tmp = Stream_GetPrintingRank( Journal_Register( InfoStream_Type, (Name)"Context" )  );
		Stream_SetPrintingRank( Journal_Register( InfoStream_Type, (Name)"Context"  ), 0 );
		Journal_Printf( /* DO NOT CHANGE OR REMOVE */
			Journal_Register( InfoStream_Type, (Name)"Context"  ), 
			"JAMBox. Copyright (C) 2012 Monash University.\n" );
		Stream_Flush( Journal_Register( InfoStream_Type, (Name)"Context" )  );
		Stream_SetPrintingRank( Journal_Register( InfoStream_Type, (Name)"Context"  ), tmp );

		/* Add the JAMBox path to the global xml path dictionary */
		directory = Memory_Alloc_Array( char, 200, "xmlDirectory" ) ;
		sprintf(directory, "%s%s", LIB_DIR, "/StGermain" );
		XML_IO_Handler_AddDirectory( "JAMBox", directory );
		Memory_Free(directory);

		/* Add the plugin path to the global plugin list */
		ModulesManager_AddDirectory( "JAMBox", LIB_DIR );
	
		return True;
	}
	return False;
}


