#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcu/pcu.h"
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

/* silly stgermain, I must define this */
#define CURR_MODULE_NAME "UnderworldContext.c"

typedef struct {
   UnderworldContext* context;
} PpcManagerSuiteData;

void PpcManagerSuite_Setup( PpcManagerSuiteData* data ) { 
   data->context = NULL;
}

void PpcManagerSuite_Teardown( PpcManagerSuiteData* data ) {
}

void PpcManagerSuite_Test( PpcManagerSuiteData* data ) {
   UnderworldContext*     context;
   Stg_ComponentFactory*  cf;
   char                   xml_input[PCU_PATH_MAX];
   PpcManager             *mgr = NULL;
   int                    tagID = -1;
   PpcTag                 *ppctag = NULL;


   pcu_docstring( "This test how the PpcManager stores and retrieves various Ppcs.");

   /* read in the xml input file */
   pcu_filename_input( "./runner.xml", xml_input );
   cf = stgMainInitFromXML( xml_input, MPI_COMM_WORLD, NULL );
   context = (UnderworldContext*)LiveComponentRegister_Get( cf->LCRegister, (Name)"context" );
   context->maxTimeSteps = 1;

   stgMainBuildAndInitialise( cf );

   /* get the PpcManager */
   mgr = (PpcManager*)LiveComponentRegister_Get( cf->LCRegister, (Name)"default_ppcManager" );
   pcu_check_true( mgr != NULL );
   mgr = Stg_CheckType( mgr, PpcManager );
   
   /* given the order of Ppc definitions from the xml file 
    * we test the mgr data structure. This is a basic test */

   /********** DEBUG INFO*****************
   printf( "\n\nmgr->tagsCount =%d\n\n", mgr->tagsCount );
   int i;
   for( i=0; i<mgr->tagsCount; i++ ) {
      printf( "\n%d = %s", i, (mgr->tagList[i]).name );
   }
   ***************************************/
   pcu_check_true( mgr->tagsCount == 27 );
   
   tagID = PpcManager_SearchTagList( mgr, "DensityLabel" );
   ppctag = &(mgr->tagList[tagID]);
   pcu_check_true( !strcmp( ppctag->name, "DensityLabel" ) );
   pcu_check_true( ppctag->materialExtHandle == 0 );
   pcu_check_true( ppctag->storeOnParticles );
   pcu_check_true( ppctag->particleExtHandle == 1 );

   tagID = PpcManager_SearchTagList( mgr, "DiffusivityLabel" );
   ppctag = &(mgr->tagList[tagID]);
   pcu_check_true( !strcmp( ppctag->name, "DiffusivityLabel" ) );
   pcu_check_true( ppctag->materialExtHandle == 1 );
   pcu_check_true( ppctag->particleExtHandle == (unsigned)-1 );

   tagID = PpcManager_SearchTagList( mgr, "CpLabel" );
   ppctag = &(mgr->tagList[tagID]);
   pcu_check_true( !strcmp( ppctag->name, "CpLabel" ) );
   pcu_check_true( ppctag->materialExtHandle == 2 );

   tagID = PpcManager_SearchTagList( mgr, "ThermalExpansivityLabel" );
   ppctag = &(mgr->tagList[tagID]);
   pcu_check_true( !strcmp( ppctag->name, "ThermalExpansivityLabel" ) );
   pcu_check_true( ppctag->materialExtHandle == 3);

   tagID = PpcManager_SearchTagList( mgr, "RadiogenicHeatProductionLabel" );
   ppctag = &(mgr->tagList[tagID]);
   pcu_check_true( !strcmp( ppctag->name, "RadiogenicHeatProductionLabel" ) );
   pcu_check_true( ppctag->materialExtHandle == 4 );

   tagID = PpcManager_SearchTagList( mgr, "default_pressure_ppc" );
   ppctag = &(mgr->tagList[tagID]);
   pcu_check_true( !strcmp( ppctag->name, "default_pressure_ppc" ) );
   pcu_check_true( ppctag->materialExtHandle == (unsigned)-1 );
   pcu_check_true( ppctag->solePpc );

   tagID = PpcManager_SearchTagList( mgr, "default_temperature_ppc" );
   ppctag = &(mgr->tagList[tagID]);
   pcu_check_true( !strcmp( ppctag->name, "default_temperature_ppc" ) );
   pcu_check_true( ppctag->materialExtHandle == (unsigned)-1 );
   pcu_check_true( ppctag->solePpc );

   /** this should fail and return -1 */
   tagID = PpcManager_SearchTagList( mgr, "0-solePpcNumber666" );
   pcu_check_true( tagID == -1 );

   tagID = PpcManager_SearchTagList( mgr, "0-solePpcNumber010" );
   pcu_check_true( tagID != -1 );
   ppctag = &(mgr->tagList[tagID]);
   pcu_check_true( !strcmp( ppctag->name, "0-solePpcNumber010" ) );
   pcu_check_true( ppctag->materialExtHandle == (unsigned)-1 );
   pcu_check_true( ppctag->solePpc );

   stgMainDestroy( cf );

}

void PpcManagerSuite( pcu_suite_t* suite ) {
   pcu_suite_setData( suite, PpcManagerSuiteData );
   pcu_suite_setFixtures( suite, PpcManagerSuite_Setup, PpcManagerSuite_Teardown );
   pcu_suite_addTest( suite, PpcManagerSuite_Test );
}


