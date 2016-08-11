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

#include "types.h"
#include "Underworld_SwarmOutput.h"

#include <assert.h>
#include <string.h>

/* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
const Type Underworld_SwarmOutput_Type = "Underworld_SwarmOutput";

/*----------------------------------------------------------------------------------------------------------------------------------
** Constructors
*/
Underworld_SwarmOutput* Underworld_SwarmOutput_New(
		Name                                  name,
		TimeIntegrator*                       timeIntegrator,
		FeVariable*                           velocityField )
{
	Underworld_SwarmOutput* self = (Underworld_SwarmOutput*) _Underworld_SwarmOutput_DefaultNew( name );

	/* 	Underworld_SwarmOutput_InitAll */
	abort();

	return self;
}

Underworld_SwarmOutput* _Underworld_SwarmOutput_New(  UNDERWORLD_SWARMOUTPUT_DEFARGS  )
{
	Underworld_SwarmOutput* self;
	
	/* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
	assert( _sizeOfSelf >= sizeof(Underworld_SwarmOutput) );
	/* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
	/* This means that any values of these parameters that are passed into this function are not passed onto the parent function
	   and so should be set to ZERO in any children of this class. */
	nameAllocationType = NON_GLOBAL;

	self = (Underworld_SwarmOutput*)_Stg_Component_New(  STG_COMPONENT_PASSARGS  );

	
	/* General info */

	/* Virtual Info */
	
	return self;
}


/*------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _Underworld_SwarmOutput_Delete( void* uwSwarmOutput ) {
	Underworld_SwarmOutput* self = (Underworld_SwarmOutput*)uwSwarmOutput;

   if( self->feVariableList ) Memory_Free( self->feVariableList );
   if( self->svList ) Memory_Free( self->svList );
}


void _Underworld_SwarmOutput_Print( void* uwSwarmOutput, Stream* stream ) {
}

void* _Underworld_SwarmOutput_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Underworld_SwarmOutput);
	Type                                                      type = Underworld_SwarmOutput_Type;
	Stg_Class_DeleteFunction*                              _delete = _Underworld_SwarmOutput_Delete;
	Stg_Class_PrintFunction*                                _print = _Underworld_SwarmOutput_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Underworld_SwarmOutput_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Underworld_SwarmOutput_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Underworld_SwarmOutput_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Underworld_SwarmOutput_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = NULL;
	Stg_Component_DestroyFunction*                        _destroy = _Underworld_SwarmOutput_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*) _Underworld_SwarmOutput_New(  UNDERWORLD_SWARMOUTPUT_PASSARGS  );
}

void _Underworld_SwarmOutput_Init( Underworld_SwarmOutput* self,
   PICelleratorContext*  context,
   unsigned int          usoOutputEvery,
   USO_MODE              mode,
   Swarm*                swarm,
   unsigned int          feVarNum,
   FeVariable**          feVariableList,
   unsigned int          svNum,
   SwarmVariable**       svList
   ) {
	self->swarm = swarm;
  self->context = (UnderworldContext*)context;
	self->mode = mode;
	self->feVarNum = feVarNum;
	self->usoOutputEvery = usoOutputEvery;
	self->feVariableList = feVariableList;
	self->svList = svList;
	self->svNum = svNum;

   if( self->mode == USO_PERPARTICLE ) {
      /* in this case we must assign a global id to each particle in the swarm; so we add an extension to the particles */
      self->pidHandle = ExtensionManager_Add( swarm->particleExtensionMgr, "particleID", 2*sizeof(unsigned int) );
      EP_AppendClassHook( Context_GetEntryPoint( context, AbstractContext_EP_UpdateClass ), _Underworld_SwarmOutput_OutputParticleTrace, self );
   } else {
      if( self->feVarNum > 0 || self->svNum > 0 ) {
         /* Swarm output will run on the SaveClass EP - the same time as standard checkpointing */
         EP_AppendClassHook( Context_GetEntryPoint( context, AbstractContext_EP_UpdateClass ), _Underworld_SwarmOutput_OutputAllSwarm, self );
      } else {
         EP_AppendClassHook( Context_GetEntryPoint( context, AbstractContext_EP_UpdateClass ), _Underworld_SwarmOutput_OutputSwarmPos, self );
      }

   }
}

void _Underworld_SwarmOutput_AssignFromXML( void* uwSwarmOutput, Stg_ComponentFactory* cf, void* data ) {
   Underworld_SwarmOutput*  self          = (Underworld_SwarmOutput*) uwSwarmOutput;

   PICelleratorContext*    context;
   Swarm*    swarm;
   Dictionary*             dictionary = Dictionary_GetDictionary( cf->componentDict, self->name );
   Dictionary_Entry_Value* list;
   char*                   mode;
   USO_MODE                uso_mode;
   unsigned int            feVarNum, svNum, feVar_I, usoOutputEvery;
   char*                   varName;
   FeVariable**            feVariableList;
   SwarmVariable**         svList;

   context =  Stg_ComponentFactory_ConstructByName( cf, (Name)"context", PICelleratorContext, True, data ) ;
   swarm = (Swarm*)Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Swarm", Swarm, True, data  );

   mode = (char*)Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"mode", "ALL_SWARM" );

   if( !strcmp(mode, "ALL_SWARM") ) {
      uso_mode = USO_PERTIMESTEP;
      usoOutputEvery = Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"usoOutputEvery", 5 );
      if( usoOutputEvery < 0 ) {
         printf("\n\n******** ERROR *********\n"
               "Bad input value for parameter 'usoOutputEvery' in component '%s'\n"
               "Value for usoOutputEvery should be an integer >= 0, current input value is %d\n"
               ,self->name, usoOutputEvery );
         assert(0);
      }
   }
   else if( !strcmp(mode, "PARTICLE_TRACE" ) )
      {uso_mode = USO_PERPARTICLE; usoOutputEvery = 1;}
   else{
      printf("\n\n****** Warning ******\n"
            "Unrecognised param\n  <param name=\"mode\"> %s </param> in '%s'\n"
            "Valid values are ALL_SWARM or PARTICLE_TRACE'\n"
            "Using default mode: 'ALL_SWARM', for '%s', which means an ascii dump of '%s' particles will take place\n"
            "If you want the output format to be a single file, containing a single files values through time add\n"
            "  <param name=\"mode\">PARTICLE_TRACE</param>\n"
            "to the '%s' definition\n\n", mode, self->name, self->swarm->name, self->name, self->name);
      assert(0);
   }
	
	/* Get all FeVariables specified */
	list = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"FeVariables"  );
	
   if( ( list == NULL) 
         || 
     ( (list != NULL) && (Dictionary_Entry_Value_GetCount( list ) == 0) ) ) {
      /* if the list is null or of zero length 
       * that's ok but we zero the corresponding data */
      feVarNum = 0;
      feVariableList = NULL;
   } else {
      feVarNum = Dictionary_Entry_Value_GetCount( list );

      /* Allocate the memory to store pointers to them */
      feVariableList = Memory_Alloc_Array( FeVariable*, feVarNum, "List FeVariables" );

      for( feVar_I = 0 ; feVar_I < feVarNum ; feVar_I++ ) {
         varName = Dictionary_Entry_Value_AsString( Dictionary_Entry_Value_GetElement( list, feVar_I ) );
         feVariableList[ feVar_I ] = Stg_ComponentFactory_ConstructByName( cf, (Name)varName, FeVariable, True, data  );
      }
   }

   /* Get all SwarmVariables specified */
	list = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"SwarmVariables"  );
	
   if( ( list == NULL) 
         || 
     ( (list != NULL) && (Dictionary_Entry_Value_GetCount( list ) == 0) ) ) {
      /* if the list is null or of zero length 
       * that's ok but we zero the corresponding data */
      svNum = 0;
      svList = NULL;
   } else {
      svNum = Dictionary_Entry_Value_GetCount( list );

      /* Allocate the memory to store pointers to them */
      svList = Memory_Alloc_Array( SwarmVariable*, svNum, "List FeVariables" );

      for( feVar_I = 0 ; feVar_I < svNum ; feVar_I++ ) {
         varName = Dictionary_Entry_Value_AsString( Dictionary_Entry_Value_GetElement( list, feVar_I ) );
         svList[ feVar_I ] = Stg_ComponentFactory_ConstructByName( cf, (Name)varName, SwarmVariable, True, data  );
      }
   }

   _Underworld_SwarmOutput_Init( self,
      context, usoOutputEvery, uso_mode, swarm,
      feVarNum, feVariableList,
      svNum, svList );

}

void _Underworld_SwarmOutput_Build( void* uwSwarmOutput, void* data ) {
	Underworld_SwarmOutput*	self = (Underworld_SwarmOutput*) uwSwarmOutput;
	unsigned int feVar_I, feVarNum;
	
	/* Build all StGermain based data structure this component uses */
	feVarNum = self->feVarNum;
	for( feVar_I = 0 ; feVar_I < feVarNum; feVar_I++ ) 
		Stg_Component_Build( self->feVariableList[feVar_I], data, False );

	Stg_Component_Build( self->swarm, data, False );
}
void _Underworld_SwarmOutput_Initialise( void* uwSwarmOutput, void* data ) {
   Underworld_SwarmOutput*	self = (Underworld_SwarmOutput*) uwSwarmOutput;
   PICelleratorContext*    context = (PICelleratorContext*)self->context;
   Swarm *swarm = self->swarm;
   GlobalParticle *g_point = NULL;
   FeVariable* feVar=NULL;
   SwarmVariable *sv=NULL;
   FILE *outputFile=NULL;
   int feVar_i, dof_i, lengthNP, lengthID;
   unsigned int particle_id, feVar_I, feVarNum;
   unsigned int *part_ext;
   int particle_rank;
   char* filename, *outputPath;

   outputPath = context->outputPath;

   /* Initialise all StGermain based data structure this component uses */
   feVarNum = self->feVarNum;
   for( feVar_I = 0 ; feVar_I < feVarNum; feVar_I++ ) 
      Stg_Component_Initialise( self->feVariableList[feVar_I], data, False );

   feVarNum = self->svNum;
   for( feVar_I = 0 ; feVar_I < feVarNum; feVar_I++ ) 
      Stg_Component_Initialise( self->svList[feVar_I], data, False );

   Stg_Component_Initialise( swarm, data, False );

   /* if mode is PARTICLE_TRACE, now initialise the particleIDs */
   if( self->mode == USO_PERPARTICLE ) {
      MPI_Comm_rank( swarm->comm, (int*)&particle_rank );
      for( particle_id = 0 ; particle_id < swarm->particleLocalCount ; particle_id++ ) {
         g_point = (GlobalParticle*)Swarm_ParticleAt( swarm, particle_id );

         part_ext = (unsigned int*)ExtensionManager_Get( swarm->particleExtensionMgr, g_point, self->pidHandle );
         part_ext[0] = particle_rank;
         part_ext[1] = particle_id;

         /* if starting from a checkpoint step, don't creat output file */
         if( context->loadSwarmsFromCheckpoint ) continue;

         /* create output file using the part_ext values as an id */
         if( particle_rank == 0 ) lengthNP = 1;
         else lengthNP = (int)log10((double)particle_rank)+1;

         if( particle_id == 0 ) lengthID = 1;
         else lengthID = (int)log10((double)particle_id)+1;

         filename = Memory_Alloc_Array_Unnamed( char,
         /*  OutputPath      .      particle       .   nproc  .  particle_id  .        swarm           .  dat\0 */
         strlen(outputPath) +1+ strlen("particle") + lengthNP +1 +lengthID + 1 + strlen(swarm->name) +1+ 3 +1 );
         sprintf( filename, "%s/particle%u.%u.%s.dat", outputPath, particle_rank, particle_id, swarm->name );
         outputFile = fopen( filename, "w" );

         fprintf( outputFile, "#timeStep time Xpos Ypos Zpos" );
         /* print out the other field names */
         for( feVar_i = 0 ; feVar_i < self->feVarNum ; feVar_i++ ) {
            feVar = self->feVariableList[feVar_i];
            for( dof_i = 0 ; dof_i < (feVar->fieldComponentCount); dof_i++ ) {
               fprintf( outputFile, ", %s-%d", feVar->name, dof_i );
            }
         }
         for( feVar_i = 0 ; feVar_i < self->svNum ; feVar_i++ ) {
            sv = self->svList[feVar_i];
            for( dof_i = 0 ; dof_i < (sv->dofCount); dof_i++ ) {
               fprintf( outputFile, ", %s-%d", sv->name, dof_i );
            }
         }
         fprintf( outputFile, "\n" );
         fprintf( outputFile, "%d %g %g %g %g\n", context->timeStep, context->currentTime, g_point->coord[0], g_point->coord[1], g_point->coord[2] );
         fclose(outputFile);
         Memory_Free(filename);

      }
   }
}

void _Underworld_SwarmOutput_OutputParticleTrace( void* uwSwarmOutput, void* data ) {
   Underworld_SwarmOutput*	self = (Underworld_SwarmOutput*)uwSwarmOutput;
   PICelleratorContext*    context = self->context;
   Swarm*                  swarm;
   GlobalParticle*         g_point;
   char*                   filename = NULL;
   char*                   outputPath = context->outputPath;
   unsigned int            particle_i, lParticleCount;
   unsigned int            *part_ext;

   FILE*              outputFile;
   MPI_Comm	   comm;
   int                myRank, nProcs;
   unsigned           particle_rank, particle_id;
   MPI_Status         status;
   const int          FINISHED_WRITING_TAG = 100;
   int                canExecute = 0;
   int                ierr, lengthID, lengthNP; 

   swarm = self->swarm;
   lParticleCount = swarm->particleLocalCount;

   for( particle_i = 0 ; particle_i < lParticleCount; particle_i++ ) {
      g_point = (GlobalParticle*)Swarm_ParticleAt( swarm, particle_i );

      part_ext = (unsigned int*) ExtensionManager_Get( swarm->particleExtensionMgr, g_point, self->pidHandle );

      particle_rank = part_ext[0];
      particle_id = part_ext[1];

      if( particle_rank == 0 ) lengthNP = 1;
      else lengthNP = (int)log10((double)particle_rank)+1;

      if( particle_id == 0 ) lengthID = 1;
      else lengthID = (int)log10((double)particle_id)+1;

      filename = Memory_Alloc_Array_Unnamed( char,
      /*  OutputPath      .      particle       .   nproc  .  particle_i  .        swarm           .  dat\0 */
      strlen(outputPath) +1+ strlen("particle") + lengthNP +1 +lengthID + 1 + strlen(swarm->name) +1+ 3 +1 );

      sprintf( filename, "%s/particle%u.%u.%s.dat", outputPath, particle_rank, particle_id, swarm->name );
      /* open in append mode */
      outputFile = fopen( filename, "a" );

      assert(outputFile); 

      /* print out the particle coords */
      fprintf( outputFile, "%d %g %g %g %g ", context->timeStep, context->currentTime, g_point->coord[0], g_point->coord[1], g_point->coord[2] );
      _Underworld_SwarmOutput_ForEachParticlePrintFeVars( self, context, particle_i, outputFile );
      fclose( outputFile );
      Memory_Free( filename );
   }
}

void _Underworld_SwarmOutput_ForEachParticlePrintFeVars( Underworld_SwarmOutput* self, PICelleratorContext* context, unsigned int particle_i, FILE* outputFile ) {
   FeVariable *feVar;
   SwarmVariable *sv;
   int feVarCount, feVar_i, dof_i;
   double value[10]; /* tmp array for field values */
   GlobalParticle* g_point = (GlobalParticle*)Swarm_ParticleAt( self->swarm, particle_i );

   /* print out the fields */
   feVarCount = self->feVarNum;
   for( feVar_i = 0 ; feVar_i < feVarCount ; feVar_i++ ) {
      feVar = self->feVariableList[feVar_i];
      _FeVariable_InterpolateValueAt( feVar, g_point->coord, value );

      for( dof_i = 0 ; dof_i < feVar->fieldComponentCount ; dof_i++ ) {
         fprintf( outputFile, "%g ", value[dof_i] );
      }
   }

   /* print out swarm vars */
   feVarCount = self->svNum;
   for( feVar_i = 0 ; feVar_i < feVarCount ; feVar_i++ ) {
      sv = self->svList[feVar_i];
			sv->_valueAt( sv, particle_i, value );

      for( dof_i = 0 ; dof_i < sv->dofCount ; dof_i++ ) {
         fprintf( outputFile, "%g ", value[dof_i] );
      }
   }

   /* print new line */
   fprintf( outputFile, "\n" );
}

void _Underworld_SwarmOutput_OutputSwarmPos( void* uwSwarmOutput, void* data ) {
   Underworld_SwarmOutput*	self = (Underworld_SwarmOutput*)uwSwarmOutput;
   PICelleratorContext*    context = self->context;
   Swarm*    swarm;
   char*                   filename = NULL;
   char*                   outputPath = context->outputPath;
   unsigned int            timeStep;

   FILE*              outputFile;
   MPI_Comm	   comm;
   int                myRank;
   int                nProcs;
   MPI_Status         status;
   const int          FINISHED_WRITING_TAG = 100;
   int                ierr,canExecute = 0;


   timeStep = context->timeStep;
   comm = self->swarm->comm;
   MPI_Comm_size( comm, (int*)&nProcs );
   MPI_Comm_rank( comm, (int*)&myRank );
   /* 
    * IF timeStep is zero or self->useOutputEvery is 0 or if timeStep doesn't fall on an self->usoOutputEvery timestep 
    *    THEN do nothing 
    */ 
   if( (timeStep==0) || (self->usoOutputEvery==0) || (timeStep % self->usoOutputEvery != 0) ) return;

   swarm = self->swarm;
	
   filename = Memory_Alloc_Array_Unnamed( char,
   /*           OutputPath     .      xyz       .      SwarmName                . time . dat\0 */
   strlen(outputPath) +1+ strlen("xyz") +1+ strlen(swarm->name) +1+ 5 + 1+ 3 +1 );
   sprintf( filename, "%s/%s.%s.%.5u.dat", outputPath, "xyz", swarm->name, timeStep );
   /* wait for go-ahead from process ranked lower than me, to avoid competition writing to file */
   if ( myRank != 0  && canExecute == 0 ) {
      ierr=MPI_Recv( &canExecute, 1, MPI_INT, myRank - 1, FINISHED_WRITING_TAG, comm, &status );
   }	

      /* if myRank is 0, create or append file, otherwise append to file */
   if (myRank == 0) {
      /* append to file if restarting from checkpoint */
      if( context->loadSwarmsFromCheckpoint )
         outputFile = fopen( filename, "a" );
      else
         outputFile = fopen( filename, "w" );

      fprintf( outputFile, "# FORMAT is:\n#Xpos, Ypos, Zpos\n" );
   } else {
      outputFile = fopen( filename, "a" );
   }

   _Underworld_SwarmOutput_GetFeVariableValues( self, NULL, swarm, outputFile );
   fclose( outputFile );

	/* confirms this processor is finshed */
	canExecute = 1;
	/* send go-ahead from process ranked lower than me, to avoid competition writing to file */
	if ( myRank != nProcs - 1 ) {
		MPI_Ssend( &canExecute, 1, MPI_INT, myRank + 1, FINISHED_WRITING_TAG, comm );
	}

   if( filename ) Memory_Free( filename );
}

void _Underworld_SwarmOutput_OutputAllSwarm( void* uwSwarmOutput, void* data ) {
   Underworld_SwarmOutput*	self = (Underworld_SwarmOutput*)uwSwarmOutput;
   PICelleratorContext*    context = self->context;
   Swarm*    swarm;
   FeVariable*             feVar;
   SwarmVariable*          sv;
   char*                   filename = NULL;
   char*                   outputPath = context->outputPath;
   unsigned int            feVar_I, feVarNum, svNum, timeStep;

   FILE*              outputFile;
   MPI_Comm	   comm;
   int                myRank;
   int                nProcs;
   MPI_Status         status;
   const int          FINISHED_WRITING_TAG = 100;
   int                ierr,canExecute = 0;


   timeStep = context->timeStep;
   /* 
    * IF timeStep is zero or self->useOutputEvery is 0 or if timeStep doesn't fall on an self->usoOutputEvery timestep 
    *    THEN do nothing 
    */ 
   if( (timeStep==0) || (self->usoOutputEvery==0) || (timeStep % self->usoOutputEvery != 0) ) return;

   feVarNum = self->feVarNum;
   svNum = self->svNum;
   swarm = self->swarm;
   comm = swarm->comm;

   MPI_Comm_size( comm, (int*)&nProcs );
   MPI_Comm_rank( comm, (int*)&myRank );

   /* wait for go-ahead from process ranked lower than me, to avoid competition writing to file */
   if ( myRank != 0  && canExecute == 0 ) {
      ierr=MPI_Recv( &canExecute, 1, MPI_INT, myRank - 1, FINISHED_WRITING_TAG, comm, &status );
   }

   /* for each field create file and then write */
   for( feVar_I = 0 ; feVar_I < feVarNum; feVar_I++ ) {
      feVar = self->feVariableList[feVar_I];

      filename = Memory_Alloc_Array_Unnamed( char,
      /*           OutputPath     .      FieldName        .      SwarmName                . time . dat\0 */
      strlen(outputPath) +1+ strlen(feVar->name) +1+ strlen(swarm->name) +1+ 5 + 1+ 3 +1 );
      sprintf( filename, "%s/%s.%s.%.5u.dat", outputPath, feVar->name, swarm->name, timeStep );

      /* if myRank is 0, create or append file, otherwise append to file */
      if (myRank == 0) {
         int dof_i;
         /* append to file if restarting from checkpoint */
         if( context->loadSwarmsFromCheckpoint )
            outputFile = fopen( filename, "a" );
         else
            outputFile = fopen( filename, "w" );

         fprintf( outputFile, "# FORMAT is:\n#Xpos, Ypos, Zpos" );
         /* print out the other field names */
         for( dof_i = 0 ; dof_i < (feVar->fieldComponentCount); dof_i++ ) {
            fprintf( outputFile, ", %s-%d", feVar->name, dof_i );
         }
         fprintf( outputFile, "\n" );
      } else {
         outputFile = fopen( filename, "a" );
      }

      _Underworld_SwarmOutput_GetFeVariableValues( self, feVar, swarm, outputFile );
      fclose( outputFile );
      if( filename ) Memory_Free( filename );
   }

   /* swarm variable writing time */
   for( feVar_I = 0 ; feVar_I < svNum; feVar_I++ ) {
      sv = self->svList[feVar_I];

      filename = Memory_Alloc_Array_Unnamed( char,
      /*           OutputPath     .      FieldName        .      SwarmName                . time . dat\0 */
      strlen(outputPath) +1+ strlen(sv->name) +1+ strlen(swarm->name) +1+ 5 + 1+ 3 +1 );
      sprintf( filename, "%s/%s.%s.%.5u.dat", outputPath, sv->name, swarm->name, timeStep );

      /* if myRank is 0, create or append file, otherwise append to file */
      if (myRank == 0) {
         int dof_i;
         /* append to file if restarting from checkpoint */
         if( context->loadSwarmsFromCheckpoint )
            outputFile = fopen( filename, "a" );
         else
            outputFile = fopen( filename, "w" );

         fprintf( outputFile, "# FORMAT is:\n#Xpos, Ypos, Zpos" );
         /* print out the other field names */
         for( dof_i = 0 ; dof_i < (sv->dofCount); dof_i++ ) {
            fprintf( outputFile, ", %s-%d", sv->name, dof_i );
         }
         fprintf( outputFile, "\n" );
      } else {
         outputFile = fopen( filename, "a" );
      }

      _Underworld_SwarmOutput_GetSVValues( self, sv, swarm, outputFile );
      fclose( outputFile );
      if( filename ) Memory_Free( filename );

   }

  /* confirms this processor is finshed */
	canExecute = 1;
	/* send go-ahead from process ranked lower than me, to avoid competition writing to file */
	if ( myRank != nProcs - 1 ) {
		MPI_Ssend( &canExecute, 1, MPI_INT, myRank + 1, FINISHED_WRITING_TAG, comm );
	}
}

void _Underworld_SwarmOutput_Destroy( void* uwSwarmOutput, void* data ) {
	Underworld_SwarmOutput*	self = (Underworld_SwarmOutput*)uwSwarmOutput;
	unsigned int feVar_I, feVarNum;

	/* Destroy all StGermain based data structure this component uses */
	feVarNum = self->feVarNum;	
	for( feVar_I = 0 ; feVar_I < feVarNum; feVar_I++ ) 
		Stg_Component_Destroy( self->feVariableList[feVar_I], data, False );

  feVarNum = self->svNum;	
	for( feVar_I = 0 ; feVar_I < feVarNum; feVar_I++ ) 
		Stg_Component_Destroy( self->svList[feVar_I], data, False );

	Stg_Component_Destroy( self->swarm, data, False );
	
}

void _Underworld_SwarmOutput_PrintStandardFormat( GlobalParticle* particle, PICelleratorContext* context, double* result, unsigned fieldComponentCount, FILE* outputFile ) {
   unsigned dof_I;
   fprintf( outputFile, "%d %g %g %g %g ", context->timeStep, context->currentTime,  particle->coord[0], particle->coord[1], particle->coord[2] ); 
   for ( dof_I = 0; dof_I < fieldComponentCount; dof_I++ ) 
      fprintf( outputFile, "%g ", result[dof_I] );

   fprintf( outputFile, "\n" );
}
	
void _Underworld_SwarmOutput_GetSVValues(Underworld_SwarmOutput* uwSwarmOutput, SwarmVariable* sv, Swarm* swarm, FILE* outputFile) {
	Underworld_SwarmOutput*	self = (Underworld_SwarmOutput*)uwSwarmOutput;
	GlobalParticle*         particle;
   int particleLocalCount, p_i;

   particleLocalCount = self->swarm->particleLocalCount;
   for( p_i = 0 ; p_i < particleLocalCount ; p_i++ ) {
      particle = (GlobalParticle*)Swarm_ParticleAt( swarm, p_i );

      fprintf( outputFile, "%g %g %g ", particle->coord[0], particle->coord[1], particle->coord[2] );
      if( sv != NULL ) {
         double result[27];
         unsigned int dof_i,fieldComponentCount = sv->dofCount;
			   sv->_valueAt( sv, p_i, result );
         for ( dof_i = 0; dof_i < fieldComponentCount; dof_i++ ) 
            fprintf( outputFile, "%g ", result[dof_i] );

      }
      fprintf( outputFile, "\n");
   }
}

void _Underworld_SwarmOutput_GetFeVariableValues(Underworld_SwarmOutput* uwSwarmOutput, FeVariable* feVariable, Swarm* swarm, FILE* outputFile) {
	Underworld_SwarmOutput*	self = (Underworld_SwarmOutput*)uwSwarmOutput;
	GlobalParticle*         particle;
   int particleLocalCount, p_i;

   particleLocalCount = self->swarm->particleLocalCount;
   for( p_i = 0 ; p_i < particleLocalCount ; p_i++ ) {
      particle = (GlobalParticle*)Swarm_ParticleAt( swarm, p_i );

      fprintf( outputFile, "%g %g %g ", particle->coord[0], particle->coord[1], particle->coord[2] );
      if( feVariable != NULL ) {
         double result[27];
         unsigned int dof_i,fieldComponentCount = feVariable->fieldComponentCount;
			_FeVariable_InterpolateValueAt( feVariable, particle->coord, result );
         for ( dof_i = 0; dof_i < fieldComponentCount; dof_i++ ) 
            fprintf( outputFile, "%g ", result[dof_i] );

      }
      fprintf( outputFile, "\n");
   }
#if 0
	/* Loop over local elements in problem */
	for( lElement_I = 0 ; lElement_I < localElementCount ; lElement_I++ ) {
		cellID            = CellLayout_MapElementIdToCellId( swarm->cellLayout, lElement_I );
		cellParticleCount = swarm->cellParticleCountTbl[ cellID ];

		/* Loop over materialPoints in element */
		for ( cParticle_I = 0 ; cParticle_I < cellParticleCount ; cParticle_I++ ) {
			particle = (GlobalParticle*)Swarm_ParticleInCellAt( swarm, cellID, cParticle_I );
			/* Interpolate field to particle location (is Global because it uses the
			 * material Points Swarm, could be a bit slow ???*/
			_FeVariable_InterpolateValueAt( feVariable, particle->coord, result );

			_Underworld_SwarmOutput_PrintStandardFormat( particle, context, result, fieldComponentCount, outputFile );

		}
	}
	Memory_Free( result );
#endif
}

/*-------------------------------------------------------------------------------------------------------------------------
** Private Functions
*/
/*---------------------------------------------------------------------------------------------------------------------
** Entry Point Hooks
*/

/*-------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/



