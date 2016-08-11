#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcu/pcu.h"
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include "Underworld/Underworld.h"

/* silly stgermain, I must define this */
#define CURR_MODULE_NAME "UnderworldContext.c"

typedef struct {
   UnderworldContext* context;
} USOSuiteData;

void USOSuite_Setup( USOSuiteData* data ) { 
   data->context = NULL;
}

void USOSuite_Teardown( USOSuiteData* data ) {}

void USOSuite_TestParticleTrace( USOSuiteData* data ) {
   Dictionary*            dictionary;
   UnderworldContext*     context;
   Stg_ComponentFactory*  cf;
   char                   *filename,*outputPath;
   char                   xml_input[PCU_PATH_MAX];
   Underworld_SwarmOutput *uso = NULL;
   Swarm                  *swarm = NULL;
   FILE                   *p_file = NULL;
   GlobalParticle         *g_point = NULL;
   unsigned int           *part_ext = NULL;
   unsigned int           local_p_files, global_p_files, p_i, particleLocalCount, particleGlobalCount;
   int                    ierr, myRank, lengthID, lengthNP;
   unsigned int           p_id, p_rank;


   local_p_files = 0;
   pcu_docstring( "This test runs a model with the Underworld_SwarmOutput (uso) component turn on. The uso component is set to PARTICLE_TRACE mode, thus one output file per particle is expected and this test tests that.");

   /* read in the xml input file */
   pcu_filename_input( "testUSO_PARTICLE_TRACE.xml", xml_input );
   cf = stgMainInitFromXML( xml_input, MPI_COMM_WORLD, NULL );
   context = (UnderworldContext*)LiveComponentRegister_Get( cf->LCRegister, (Name)"context" );
   context->maxTimeSteps = 3;
   uso = (Underworld_SwarmOutput*)LiveComponentRegister_Get( cf->LCRegister, (Name)"uso" );
   pcu_check_true( uso != NULL );

   uso = Stg_CheckType( uso, Underworld_SwarmOutput );
   outputPath = context->outputPath;
   dictionary = context->dictionary;

   stgMainBuildAndInitialise( cf );

   stgMainLoop( cf );

   /* Test is simply to see if we end up with the same number of particle files
    * as the number of particles in the uso swarm 
    */
   swarm = uso->swarm;
   particleLocalCount = swarm->particleLocalCount;
   MPI_Comm_rank( swarm->comm, &myRank );

   /* Collect the number of local_p_files found */
   ierr = MPI_Reduce( &particleLocalCount, &particleGlobalCount, 1, MPI_UNSIGNED, MPI_SUM, 0, MPI_COMM_WORLD );

   for( p_i = 0 ; p_i <particleLocalCount ; p_i++ ){
      g_point = (GlobalParticle*)Swarm_ParticleAt( swarm, p_i );

      part_ext = (unsigned int*)ExtensionManager_Get( swarm->particleExtensionMgr, g_point, uso->pidHandle );
      p_rank = part_ext[0] ;
      p_id = part_ext[1] ;

      /* create the would be particle's outputfile name */
      if( p_rank == 0 ) lengthNP = 1;
      else lengthNP = (int)log10((double)p_rank)+1;

      if( p_id == 0 ) lengthID = 1;
      else lengthID = (int)log10((double)p_id)+1;

      filename = Memory_Alloc_Array_Unnamed( char,
         /*  OutputPath      .      particle       .   nproc  .  particle_id  .        swarm           .  dat\0 */
         strlen(outputPath) +1+ strlen("particle") + lengthNP +1 +lengthID + 1 + strlen(swarm->name) +1+ 3 +1 );
         sprintf( filename, "%s/particle%u.%u.%s.dat", outputPath, p_rank, p_id, swarm->name );

      /* THE TEST:
       * check if file exists, and remember if it is */
      p_file = fopen( filename, "a" );
      pcu_check_true( p_file != NULL );

      if( p_file != NULL ) {
         local_p_files++;
         fclose(p_file);
      }

      Memory_Free( filename );
   }

   /* Collect the number of local_p_files found */
   ierr = MPI_Reduce( &local_p_files, &global_p_files, 1, MPI_UNSIGNED, MPI_SUM, 0, MPI_COMM_WORLD );

   if( myRank == 0 )
      pcu_check_true( global_p_files == particleGlobalCount );
   stgMainDestroy( cf );

}

void USOSuite_TestAllSwarm( USOSuiteData* data ) {
   Dictionary*            dictionary;
   UnderworldContext*     context;
   Stg_ComponentFactory*  cf;
   char                   *filename,*outputPath;
   char                   xml_input[PCU_PATH_MAX];
   Underworld_SwarmOutput *uso = NULL;
   Swarm                  *swarm = NULL;
   FILE                   *s_file = NULL;
   int                    myRank;

   pcu_docstring( "This test runs a model with the Underworld_SwarmOutput (uso) component turn on. The uso component is set to ALL_SWARM mode, thus one output file per timestep is expected and this test tests that.");

   /* read in the xml input file */
   pcu_filename_input( "testUSO_ALL_SWARM.xml", xml_input );
   cf = stgMainInitFromXML( xml_input, MPI_COMM_WORLD, NULL );
   context = (UnderworldContext*)LiveComponentRegister_Get( cf->LCRegister, (Name)"context" );
   context->maxTimeSteps = 5;
   uso = (Underworld_SwarmOutput*)LiveComponentRegister_Get( cf->LCRegister, (Name)"uso" );
   pcu_check_true( uso != NULL );

   uso = Stg_CheckType( uso, Underworld_SwarmOutput );
   outputPath = context->outputPath;
   dictionary = context->dictionary;

   stgMainBuildAndInitialise( cf );

   stgMainLoop( cf );

   /* Test is to see if the output file is as expected, i.e. 'xyz.passiveTracerSwarm.00005.dat' */
   swarm = uso->swarm;
   MPI_Comm_rank( swarm->comm, &myRank );

   filename=Memory_Alloc_Array_Unnamed( char, strlen(outputPath) +1 + strlen("xyz.passiveTracerSwarm.00005.dat") );
   sprintf( filename, "%sxyz.passiveTracerSwarm.00005.dat", outputPath );

   s_file = fopen( filename, "r" );
   pcu_check_true( s_file != NULL );
   fclose(s_file);
   
   Memory_Free( filename );

   stgMainDestroy( cf );
}

void USOSuite( pcu_suite_t* suite ) {
   pcu_suite_setData( suite, USOSuiteData );
   pcu_suite_setFixtures( suite, USOSuite_Setup, USOSuite_Teardown );
   pcu_suite_addTest( suite, USOSuite_TestParticleTrace );
   pcu_suite_addTest( suite, USOSuite_TestAllSwarm );
}


