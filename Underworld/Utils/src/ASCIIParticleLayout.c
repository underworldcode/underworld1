/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
** Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
** Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
** Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
** Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
** Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
** Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include "types.h"
#include "ASCIIParticleLayout.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>

const Type ASCIIParticleLayout_Type = (const Type)"ASCIIParticleLayout";

ASCIIParticleLayout* _ASCIIParticleLayout_New( ASCIIPARTICLELAYOUT_DEFARGS  )
{
   ASCIIParticleLayout* self;
   
   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof( ASCIIParticleLayout ) );
   self = (ASCIIParticleLayout*)_GlobalParticleLayout_New(  GLOBALPARTICLELAYOUT_PASSARGS  );

   /* set default attributes */
   self->filename            = NULL;
   self->mustUseAllParticles = False;
   self->columnList          = NULL;
   self->columnScalingList   = NULL;
   self->columnScaling       = NULL;
   self->columnOffsetList    = NULL;
   self->columnOffset        = NULL;

   return self;
}


void _ASCIIParticleLayout_Delete( void* particleLayout ) {
   ASCIIParticleLayout* self = (ASCIIParticleLayout*)particleLayout;

   /* Stg_Class_Delete parent class */
   _GlobalParticleLayout_Delete( self );
}

void _ASCIIParticleLayout_Print( void* particleLayout, Stream* stream ) {
   ASCIIParticleLayout* self = (ASCIIParticleLayout*)particleLayout;
   
   /* General info */
   Journal_Printf( stream, "ASCIIParticleLayout (ptr): %p:\n", self );
   Stream_Indent( stream );
   
   /* Parent class info */
   _GlobalParticleLayout_Print( self, stream );
   
   /* ASCIIParticleLayout */
   Journal_Printf( stream, "filename: %s\n", self->filename );
   
   Stream_UnIndent( stream );
}


void* _ASCIIParticleLayout_Copy( void* particleLayout, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
   ASCIIParticleLayout*	self = (ASCIIParticleLayout*)particleLayout;
   ASCIIParticleLayout*	newASCIIParticleLayout;
   
   newASCIIParticleLayout = (ASCIIParticleLayout*)_GlobalParticleLayout_Copy( self, dest, deep, nameExt, ptrMap );
   
   newASCIIParticleLayout->filename            = StG_Strdup( self->filename );
   newASCIIParticleLayout->mustUseAllParticles = self->mustUseAllParticles;
   newASCIIParticleLayout->columnList          = self->columnList;
   newASCIIParticleLayout->columnScalingList   = self->columnScalingList;
   newASCIIParticleLayout->columnOffsetList    = self->columnOffsetList;

   return (void*)newASCIIParticleLayout;
}

void* _ASCIIParticleLayout_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                                _sizeOfSelf = sizeof(ASCIIParticleLayout);
	Type                                                                        type = ASCIIParticleLayout_Type;
	Stg_Class_DeleteFunction*                                                _delete = _ASCIIParticleLayout_Delete;
	Stg_Class_PrintFunction*                                                  _print = _ASCIIParticleLayout_Print;
	Stg_Class_CopyFunction*                                                    _copy = _ASCIIParticleLayout_Copy;
	Stg_Component_DefaultConstructorFunction*                    _defaultConstructor = _ASCIIParticleLayout_DefaultNew;
	Stg_Component_ConstructFunction*                                      _construct = _ASCIIParticleLayout_AssignFromXML;
	Stg_Component_BuildFunction*                                              _build = _ASCIIParticleLayout_Build;
	Stg_Component_InitialiseFunction*                                    _initialise = _ASCIIParticleLayout_Initialise;
	Stg_Component_ExecuteFunction*                                          _execute = _ASCIIParticleLayout_Execute;
	Stg_Component_DestroyFunction*                                          _destroy = _ASCIIParticleLayout_Destroy;
	AllocationType                                                nameAllocationType = NON_GLOBAL;
	ParticleLayout_SetInitialCountsFunction*                       _setInitialCounts = _ASCIIParticleLayout_SetInitialCounts;
	ParticleLayout_InitialiseParticlesFunction*                 _initialiseParticles = _ASCIIParticleLayout_InitialiseParticles;
	CoordSystem                                                          coordSystem = GlobalCoordSystem;
	Bool                                                 weightsInitialisedAtStartup = False;
	GlobalParticleLayout_InitialiseParticleFunction*             _initialiseParticle = _ASCIIParticleLayout_InitialiseParticle;
	Particle_Index                                             totalInitialParticles = 0;
	double                                            averageInitialParticlesPerCell = 0.0;

   return (void*)_ASCIIParticleLayout_New(  ASCIIPARTICLELAYOUT_PASSARGS  );
}

void _ASCIIParticleLayout_AssignFromXML( void* particleLayout, Stg_ComponentFactory *cf, void* data ) {
   ASCIIParticleLayout*    self     = (ASCIIParticleLayout*) particleLayout;
   Name                    filename;

   _GlobalParticleLayout_AssignFromXML( self, cf, data );
   /** The name of the file containing the global layout definitions */
   filename                  = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"filename", NULL );
   /** list for which swarmvariable corresponds to which column of the ascii file.  see meta file example.  all required swarmvariables must be represented. */
   self->columnList          = Dictionary_Get( Dictionary_GetDictionary( cf->componentDict, self->name ), (Dictionary_Entry_Key)"columnList" );
   /** list for how to scale each column within ascii file */
   self->columnScalingList   = Dictionary_Get( Dictionary_GetDictionary( cf->componentDict, self->name ), (Dictionary_Entry_Key)"columnScaling" );
   /** list for how to offset each column within ascii file.  applied after scaling */
   self->columnOffsetList    = Dictionary_Get( Dictionary_GetDictionary( cf->componentDict, self->name ), (Dictionary_Entry_Key)"columnOffset" );
   /** deliminators seperating ascii row data */
   self->delim               = StG_Strdup( Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"fileDeliminator", " ,\t" ));
   
   self->filename            = StG_Strdup( filename );
   self->errorStream         = Journal_MyStream( Error_Type, self );
   self->file                = (File*)CFile_NewRead( filename );
   Journal_Firewall( self->file != NULL,
        self->errorStream,
        "Error in %s for %s '%s' - Cannot find or open file '%s'.",
        __func__,
        self->type,
        self->name,
        filename );
}
   
void _ASCIIParticleLayout_Build( void* particleLayout, void* data ) {
}

void _ASCIIParticleLayout_Initialise( void* particleLayout, void* data ) {
}

void _ASCIIParticleLayout_Execute( void* particleLayout, void* data ) {
}

void _ASCIIParticleLayout_Destroy( void* particleLayout, void* data ) {
   ASCIIParticleLayout*  self = (ASCIIParticleLayout*)particleLayout;
   Memory_Free( self->filename );
   Stg_Class_Delete( self->file );

   _GlobalParticleLayout_Destroy( self, data );
}

#define ASCIIMAX_LINE_LENGTH_DEFINE 1024

void _ASCIIParticleLayout_SetInitialCounts( void* particleLayout, void* _swarm ) {
   ASCIIParticleLayout* self      = (ASCIIParticleLayout*)particleLayout;
   Swarm*               swarm     = (Swarm*)_swarm;
   int                  count     = 0;
   char                 check[ASCIIMAX_LINE_LENGTH_DEFINE];
   char                 temp[ASCIIMAX_LINE_LENGTH_DEFINE];
   
   Journal_DPrintf( self->debug, "In %s(): for ParticleLayout \"%s\", of type %s\n",
      __func__, self->name, self->type );
   Stream_IndentBranch( Swarm_Debug ); 

   /** count number of lines in file */
   while (fgets(temp, ASCIIMAX_LINE_LENGTH_DEFINE, CFile_Ptr( self->file ) ) != NULL){
      fscanf( CFile_Ptr( self->file ), "%s", check );
      /** do not include empty lines */
      if ( strcmp( check, "\n") )
         count++;
   }
   rewind( CFile_Ptr( self->file ) );
   self->totalInitialParticles = count;
   
   _GlobalParticleLayout_SetInitialCounts( self, swarm );

   Stream_UnIndentBranch( Swarm_Debug );  
   Journal_DPrintf( self->debug, "...finished %s() for ParticleLayout \"%s\".\n",
      __func__, self->name );
}

void _ASCIIParticleLayout_InitialiseParticles( void* particleLayout, void* _swarm ) {
   ASCIIParticleLayout*    self = (ASCIIParticleLayout*)particleLayout;
	Stream*                 errorStream = Journal_Register( Error_Type, (Name)self->type );
    unsigned                ii;

	/** determine mapping from ascii columns to swarmVariable indexs */ 
	_ASCIIParticleLayout_MapColumnsToSwarmVariables( particleLayout, _swarm );
	
	/** allocate space for each row of ascii data */
   self->rowData       = Memory_Alloc_Array( double, self->columnCount, "ASCII perRowData" );
   self->columnScaling = Memory_Alloc_Array( double, self->columnCount, "ASCII column scaling" );
   self->columnOffset  = Memory_Alloc_Array( double, self->columnCount, "ASCII column offset"  );

   /** default scaling to 1 */
   for( ii=0; ii< self->columnCount; ii++ ){
      self->columnScaling[ii] = 1;
      self->columnOffset[ii] = 0;
   }

   /** now set values according to dictionary values (if available) */
   if( self->columnScalingList ) {
      unsigned scalingCount = Dictionary_Entry_Value_GetCount( self->columnScalingList );
      Journal_Firewall( scalingCount==self->columnCount, self->errorStream, "Error in %s for %s '%s'.\nScaling list count (%d) is not equal to data list count (%d).",
           __func__, self->type, self->name, scalingCount, self->columnCount );
      for( ii = 0; ii < scalingCount; ii++ )
         self->columnScaling[ii] = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetElement( self->columnScalingList, ii ) );
   }
   if( self->columnOffsetList ) {
      unsigned offsetCount = Dictionary_Entry_Value_GetCount( self->columnOffsetList );
      Journal_Firewall( offsetCount==self->columnCount, self->errorStream, "Error in %s for %s '%s'.\nOffset list count (%d) is not equal to data list count (%d).",
           __func__, self->type, self->name, offsetCount, self->columnCount );
      for( ii = 0; ii < offsetCount; ii++ )
         self->columnOffset[ii]  = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetElement( self->columnOffsetList,  ii ) );
   }
    
    _GlobalParticleLayout_InitialiseParticles( self, _swarm );
    
}
	
void _ASCIIParticleLayout_InitialiseParticle( void* particleLayout, void* _swarm, Particle_Index newParticle_I, void* particle ) {
   ASCIIParticleLayout*	self = (ASCIIParticleLayout*)particleLayout;
   Swarm*				swarm = (Swarm*)_swarm;
   SwarmVariable*       swarmVar;
   SwarmVariable*       parentSwarmVar;
   Index                swarmVar_I;
   Index                jj;
   int                  testChar;
   Bool                 blankLine = True;
   char                 currentString[ASCIIMAX_LINE_LENGTH_DEFINE];
   Bool                 isCheckpointedAndReloaded;
   char*                pch;
   
   /** first check if line is empty.  keep skipping until non-empty line is found */
   while( blankLine == True ){
      testChar = fgetc( CFile_Ptr( self->file ) );
      if ( testChar != '\n' )
         blankLine = False;
   }
   /** we must now rewind a character */
   fseek( CFile_Ptr( self->file ), -1, SEEK_CUR );
      
   /** get string from file */
   fgets( currentString, ASCIIMAX_LINE_LENGTH_DEFINE, CFile_Ptr( self->file ) );

   /** tokenize string */
   pch = strtok(currentString, self->delim );
   jj  = 0;
   while ((pch != NULL) && (jj<self->columnCount) ) {
      sscanf( pch, "%lg", self->rowData+jj);
      *(self->rowData+jj) *= *(self->columnScaling+jj);        /* apply scaling */
      *(self->rowData+jj) += *(self->columnOffset +jj);        /* apply offset  */
      jj++;
      pch = strtok (NULL, self->delim);
   }

   for( swarmVar_I = 0; swarmVar_I < swarm->swarmVariable_Register->objects->count; swarmVar_I++ ) {
      /** set default for checkpointing/reloading to false */
      isCheckpointedAndReloaded = False;
      /** get swarm variable */
      swarmVar = SwarmVariable_Register_GetByIndex( swarm->swarmVariable_Register, swarmVar_I );
      /** check checkpoint/reload flag */
      isCheckpointedAndReloaded = swarmVar->isCheckpointedAndReloaded;
      /** if swarmVar has a parent, then it may have its checkpoint flag set to determine checkpoint for children */
      if( swarmVar->variable->parent != NULL ){
         parentSwarmVar = SwarmVariable_Register_GetByName( swarm->swarmVariable_Register, swarmVar->variable->parent->name );
         isCheckpointedAndReloaded = parentSwarmVar->isCheckpointedAndReloaded;
      }
      /** given we reload data via the children variables, which always have dofCount==1, we no longer wish to reload it for the parent */
      if( swarmVar->dofCount !=1 ) isCheckpointedAndReloaded = False;
      
      if( isCheckpointedAndReloaded ) {
         /* Treat the data differently depending on its type */

         if(      swarmVar->variable->dataTypes[0] == Variable_DataType_Int   ){
            int   data  = (int)   self->rowData[self->swarmVarMapping[swarmVar_I]];
            Variable_SetValue( swarmVar->variable, swarm->particleLocalCount, &data );
		//	fprintf(stdout,"%s IData %d\n",swarmVar->name,data);
         }
         else if( swarmVar->variable->dataTypes[0] == Variable_DataType_Char  ){
            char  data  = (char)  self->rowData[self->swarmVarMapping[swarmVar_I]];
            Variable_SetValue( swarmVar->variable, swarm->particleLocalCount, &data );
		//	fprintf(stdout,"%s CData %c\n",swarmVar->name,data);
         }
         else if( swarmVar->variable->dataTypes[0] == Variable_DataType_Float ){
            float data  = (float) self->rowData[self->swarmVarMapping[swarmVar_I]];
            Variable_SetValue( swarmVar->variable, swarm->particleLocalCount, &data );
		//	fprintf(stdout,"%s FData %f\n",swarmVar->name,data);
         }
         else{
            double data = (double) self->rowData[self->swarmVarMapping[swarmVar_I]];
            Variable_SetValue( swarmVar->variable, swarm->particleLocalCount, &data );
		//	fprintf(stdout,"%s DData %lf\n",swarmVar->name,data);
         }
      }
   }
      
}

void _ASCIIParticleLayout_MapColumnsToSwarmVariables(	void* particleLayout, void* _swarm ) {
   ASCIIParticleLayout*	self = (ASCIIParticleLayout*)particleLayout;
   Swarm*					swarm = (Swarm*)_swarm;
   Index                ii, swarmVar_I;
   Bool                 foundMatchingColumn;
   Bool                 isCheckpointedAndReloaded;
   SwarmVariable*       swarmVar;
   SwarmVariable*       parentSwarmVar;
   
   /** determine number of columns in file */
   self->columnCount = Dictionary_Entry_Value_GetCount( self->columnList );

   self->swarmVarMapping = Memory_Alloc_Array( unsigned, swarm->swarmVariable_Register->objects->count, "swarmVarMapping" );

   for( swarmVar_I = 0; swarmVar_I < swarm->swarmVariable_Register->objects->count; swarmVar_I++ ) {
      /** set default for checkpointing/reloading to false */
      isCheckpointedAndReloaded = False;
      /** get swarm variable */
      swarmVar = SwarmVariable_Register_GetByIndex( swarm->swarmVariable_Register, swarmVar_I );
      /** check checkpoint/reload flag */
      isCheckpointedAndReloaded = swarmVar->isCheckpointedAndReloaded;
      /** if swarmVar has a parent, then it may have its checkpoint flag set to determine checkpoint for children */
      if( swarmVar->variable->parent != NULL ){
         parentSwarmVar = SwarmVariable_Register_GetByName( swarm->swarmVariable_Register, swarmVar->variable->parent->name );
         isCheckpointedAndReloaded = parentSwarmVar->isCheckpointedAndReloaded;
      }
      /** given we reload data via the children variables, which always have dofCount==1, we no longer wish to reload it for the parent */
      if( swarmVar->dofCount != 1 ) isCheckpointedAndReloaded = False;
      
      if( isCheckpointedAndReloaded ) {
         foundMatchingColumn = False;
         for(ii = 0; ii< self->columnCount ; ii++){
            if(!strcmp(swarmVar->name, Dictionary_Entry_Value_AsString( Dictionary_Entry_Value_GetElement( self->columnList, ii ) ) ) ){
               self->swarmVarMapping[swarmVar_I] = ii;
               foundMatchingColumn = True;
               break;
            }
         }
         
         Journal_Firewall( foundMatchingColumn,
              self->errorStream,
              "Error in %s for %s '%s' - Cannot find required ASCII data for swarmVariable %s.\n Check definitions within columnList. They must match exactly. ",
              __func__,
              self->type,
              self->name,
              swarmVar->name );

      }
   }

}


