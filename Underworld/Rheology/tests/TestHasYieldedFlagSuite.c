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
** Role:
**   Tests the TestHasYieldedFlag
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdio.h>
#include <stdlib.h>

#include "pcu/pcu.h"
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>
#include "TestHasYieldedFlagSuite.h"

#define TOLERANCE 1e-10
#define RANDOM_DIRECTOR_ERROR 1.0
#define CURR_MODULE_NAME "TestHasYieldedFlagSuite"

typedef struct {
	UnderworldContext* context;
} TestHasYieldedFlagSuiteData;

void TestHasYieldedFlagSuite_Setup( TestHasYieldedFlagSuiteData* data ) {
	Journal_Enable_AllTypedStream( False );
}

void TestHasYieldedFlagSuite_Teardown( TestHasYieldedFlagSuiteData* data ) {
	Journal_Enable_AllTypedStream( True );
}


void testHasYieldedFlag( FiniteElementContext* context ) {
   Stream*             stream;
    Journal_Enable_AllTypedStream( True );
	stream = Journal_Register( Info_Type, (Name)"testHasYieldedFlag" );
	Swarm*            swarm  = (Swarm*)LiveComponentRegister_Get( context->CF->LCRegister, (Name)"materialSwarm" );
	
	/* Make sure that there is a material Swarm */
	if (swarm == NULL) {
		printf("This unit test can only be run if the material Swarm is initialised\n");
		exit(0);
	}
	
	Journal_Enable_AllTypedStream( False );
}

void TestHasYieldedFlagSuite_SwarmVariableList( TestHasYieldedFlagSuiteData* data ) {
	Dictionary*           dictionary;
	UnderworldContext*    context;
	Stg_ComponentFactory* cf;
	char                  expected_file[PCU_PATH_MAX], output_file[PCU_PATH_MAX];
	char                  xml_input[PCU_PATH_MAX];
	char                  rFile[PCU_PATH_MAX];
	int                   err;
	
	pcu_docstring( "This test makes sure there is a hasYielded flag on each particle if the materialSwarm is not NULL\n" );
	
	/* read in the xml input file */
	pcu_filename_input( "testHasYieldedFlag.xml", xml_input );
	cf = stgMainInitFromXML( xml_input, MPI_COMM_WORLD, NULL );
	context = (UnderworldContext*)LiveComponentRegister_Get( cf->LCRegister, (Name)"context"  );
	data->context = context;
	dictionary = context->dictionary;
	
	/* replace the Execute EP with the function to test the ConstitutiveMatrix */
	ContextEP_ReplaceAll( context, AbstractContext_EP_Execute, testHasYieldedFlag );
	
	/* build and initialise all components */
	stgMainBuildAndInitialise( cf );
	
	/* now run the all EPs */
	stgMainLoop( cf );
	
	/* create the expected file string */
	pcu_filename_expected( "testHasYieldedFlag_SwarmVariableList.expected", expected_file );
	
	/* create the string for the output.dat */
	strcpy( output_file, context->outputPath );
	strcat( output_file, "/SwarmVariables.list" );
	
	/* check the output file against the expected output file */
	pcu_check_fileEq( output_file, expected_file );
	
	/* Now clean output path */
	sprintf(rFile, "%s/SwarmVariables.list", data->context->outputPath );
	err = remove( rFile );
	if( err == -1 ) printf("Error in %s, can't delete the input.xml\n", __func__);
	
	/* end the run */
	stgMainDestroy( cf );
}

void TestHasYieldedFlagSuite( pcu_suite_t* suite ) {
	pcu_suite_setData( suite, TestHasYieldedFlagSuiteData );
	pcu_suite_setFixtures( suite, TestHasYieldedFlagSuite_Setup, TestHasYieldedFlagSuite_Teardown );
	pcu_suite_addTest( suite, TestHasYieldedFlagSuite_SwarmVariableList );
}