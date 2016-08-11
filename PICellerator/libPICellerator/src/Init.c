/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**   Melbourne, 3053, Australia.
** Copyright (c) 2005-2010, Monash University, Building 28, Monash University Clayton Campus,
**   Victoria, 3800, Australia
**
** Primary Contributing Organisations:
**   Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**   AuScope - http://www.auscope.org
**   Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**
** Contributors:
**   Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**   Patrick D. Sunter, Software Engineer, VPAC. (patrick@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**   Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
**   Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**   Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**   David Stegman, Postdoctoral Fellow, Monash University. (david.stegman@sci.monash.edu.au)
**   Wendy Sharples, PhD Student, Monash University (wendy.sharples@sci.monash.edu.au)
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

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>

#include "PICellerator.h"

#include <stdio.h>

/*
 * Initialises this package, then any init for this package
 * such as streams etc.
 */
Bool PICellerator_Init( int* argc, char** argv[] ) {
   /* 
    * This init function tells StGermain of all the component types, 
    * etc this module contributes. Because it can be linked at compile
    * time or linked in by a toolbox at runtime, we need to make sure it isn't run twice
    * (compiled in and loaded through a toolbox.
    */
   if( !ToolboxesManager_IsInitialised( stgToolboxesManager, "PICellerator" ) ) {
      int   tmp;
      char* directory;

      PICellerator_PopulationControl_Init( argc, argv );
      PICellerator_Weights_Init( argc, argv );
      PICellerator_MaterialPoints_Init( argc, argv );
      PICellerator_Utils_Init( argc, argv );
      PICellerator_Common_Init( argc, argv );

      /* DO NOT CHANGE OR REMOVE */   
      Journal_Printf( Journal_Register( DebugStream_Type, (Name)"Context"  ), "In: %s\n", __func__ );
      tmp = Stream_GetPrintingRank( Journal_Register( InfoStream_Type, (Name)"Context" )  );
      Stream_SetPrintingRank( Journal_Register( InfoStream_Type, (Name)"Context"  ), 0 );

      /* DO NOT CHANGE OR REMOVE */
      Journal_Printf( 
         Journal_Register( InfoStream_Type, (Name)"Context" ), 
         "PICellerator (Particle-In-Cell Framework) Revision %s. Copyright (C) 2005-2010 VPAC & Monash University.\n",
         VERSION );

      /* Add repo indentity info in the repo dictionary. */
      Dictionary_Add( versionDict, "PICellerator", Dictionary_Entry_Value_FromString( VERSION ) );
      Dictionary_Add( branchDict, "PICellerator", Dictionary_Entry_Value_FromString( BRANCH ) );
      Dictionary_Add( pathDict, "PICellerator", Dictionary_Entry_Value_FromString( PATH ) );

      Stream_Flush( Journal_Register( InfoStream_Type, (Name)"Context" ) );
      Stream_SetPrintingRank( Journal_Register( InfoStream_Type, (Name)"Context" ), tmp );

      /* Add the PICellerator path to the global xml path dictionary */
      directory = Memory_Alloc_Array( char, 200, "xmlDirectory" ) ;
      sprintf(directory, "%s%s", LIB_DIR, "/StGermain" );
      XML_IO_Handler_AddDirectory( "PICellerator", directory );
      Memory_Free(directory);
   
      /* Add the plugin path to the global plugin list */
      ModulesManager_AddDirectory( "PICellerator", LIB_DIR );

      return True;
   }
   return False;
}


