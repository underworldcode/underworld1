/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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

#include <StgDomain/Geometry/Geometry.h>
#include <StgDomain/Shape/Shape.h>
#include <StgDomain/Mesh/Mesh.h>
#include <StgDomain/Utils/Utils.h>

#include "types.h"
#include "SwarmClass.h"
#include "StandardParticle.h"
#include "SwarmOutput.h"
#include "SwarmVariable.h"

#include <assert.h>
#include <string.h>

const Type SwarmOutput_Type = "SwarmOutput";


SwarmOutput* SwarmOutput_New(		
		Name                                               name,
		void*                                              context,
		Swarm*                                             swarm,
		Name                                               baseFilename,
		unsigned int                                       columnWidth, 
		unsigned int                                       decimalLength,
		char*                                              borderString )		
{
	SwarmOutput* self = _SwarmOutput_DefaultNew( name );

	_SwarmOutput_Init( self, context, swarm, baseFilename, columnWidth, decimalLength, borderString );
	return self;
}

SwarmOutput* _SwarmOutput_New(  SWARMOUTPUT_DEFARGS  ) 
{
	SwarmOutput*		self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(SwarmOutput) );
	/* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
	/* This means that any values of these parameters that are passed into this function are not passed onto the parent function
	   and so should be set to ZERO in any children of this class. */
	nameAllocationType = NON_GLOBAL;

	self = (SwarmOutput*)_Stg_Component_New(  STG_COMPONENT_PASSARGS  );
	
	/* Virtual functions */
	self->_printHeader                = _printHeader;
	self->_printData                  = _printData;

	return self;
}

void _SwarmOutput_Init( 
		SwarmOutput*                                       self,
		void*                                              context,
		Swarm*                                             swarm,
		Name                                               baseFilename,
		unsigned int                                       columnWidth, 
		unsigned int                                       decimalLength,
		char*                                              borderString )
{
	SwarmVariable* swarmVariable;
	ExtensionInfo_Index handle;
	/* Add ourselves to the register for later retrieval by clients */
	self->isConstructed = True;

	self->swarm                  = swarm;
	self->baseFilename           = baseFilename;

	handle = ExtensionManager_Add( swarm->particleExtensionMgr, (Name)SwarmOutput_Type, sizeof( int )  );
	swarmVariable = Swarm_NewScalarVariable( 
			swarm,
         self->name, //"globalIndexVariable",  OK: changed to use unique name
			(ArithPointer) ExtensionManager_Get( swarm->particleExtensionMgr, 0, handle ), 
			Variable_DataType_Int );
	self->globalIndexVariable = swarmVariable->variable;

	self->columnWidth = columnWidth;
	self->borderString = StG_Strdup( borderString );
	Stg_asprintf( &self->doubleFormatString, "%%%d.%dg", columnWidth, decimalLength );

	EP_AppendClassHook( Context_GetEntryPoint( context, AbstractContext_EP_DumpClass ),	SwarmOutput_Execute, self );
}


void _SwarmOutput_Delete( void* swarmOutput ) {
	SwarmOutput* self = (SwarmOutput*) swarmOutput;

	Memory_Free( self->doubleFormatString );
	Memory_Free( self->borderString );
	_Stg_Component_Delete( self );
}

void _SwarmOutput_Print( void* _swarmOutput, Stream* stream ) {
	SwarmOutput* self = (SwarmOutput*) _swarmOutput;

	Journal_Printf( stream, "SwarmOutput - '%s'\n", self->name );
	Stream_Indent( stream );
	_Stg_Component_Print( self, stream );

	Journal_PrintPointer( stream, self->_printHeader );
	Journal_PrintPointer( stream, self->_printData );

	Journal_Printf( stream, "Swarm - '%s'\n", self->swarm->name );
	Journal_Printf( stream, "globalIndexVariable - '%s'\n", self->globalIndexVariable->name );

	Stream_UnIndent( stream );
}

void* _SwarmOutput_Copy( void* swarmOutput, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
	SwarmOutput*	self = (SwarmOutput*)swarmOutput;
	SwarmOutput*	newSwarmOutput;
	PtrMap*			map = ptrMap;
	Bool			ownMap = False;
	
	if( !map ) {
		map = PtrMap_New( 10 );
		ownMap = True;
	}
	
	newSwarmOutput = _Stg_Component_Copy( self, dest, deep, nameExt, map );
	
	newSwarmOutput->_printHeader               = self->_printHeader;
	newSwarmOutput->_printData                 = self->_printData  ;

	newSwarmOutput->swarm                      = self->swarm;
	newSwarmOutput->globalIndexVariable        = self->globalIndexVariable;

	if( ownMap ) {
		Stg_Class_Delete( map );
	}
				
	return (void*)newSwarmOutput;
}


void* _SwarmOutput_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(SwarmOutput);
	Type                                                      type = SwarmOutput_Type;
	Stg_Class_DeleteFunction*                              _delete = _SwarmOutput_Delete;
	Stg_Class_PrintFunction*                                _print = _SwarmOutput_Print;
	Stg_Class_CopyFunction*                                  _copy = _SwarmOutput_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _SwarmOutput_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _SwarmOutput_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _SwarmOutput_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _SwarmOutput_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _SwarmOutput_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _SwarmOutput_Destroy;
	SwarmOutput_PrintHeaderFunction*                  _printHeader = _SwarmOutput_PrintHeader;
	SwarmOutput_PrintDataFunction*                      _printData = _SwarmOutput_PrintData;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

		return (void*) _SwarmOutput_New(  SWARMOUTPUT_PASSARGS  );
}
void _SwarmOutput_AssignFromXML( void* swarmOutput, Stg_ComponentFactory* cf, void* data ) {
	SwarmOutput*	        self         = (SwarmOutput*)swarmOutput;
	Swarm*                  swarm;
	Name                    baseFilename;

	self->context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", AbstractContext, False, data );
	if( !self->context  )
		self->context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", AbstractContext, True, data  );

	swarm        =  Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Swarm", Swarm, True, data   ) ;
	baseFilename = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"baseFilename", self->name  );

	_SwarmOutput_Init( 
			self,
			(AbstractContext*)self->context,
			swarm, 
			baseFilename, 
			Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"columnWidth", 12  ), 
			Stg_ComponentFactory_GetUnsignedInt( cf, self->name, (Dictionary_Entry_Key)"decimalLength", 6  ), 
			Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"borderString", "    " )  );
}

void _SwarmOutput_Build( void* swarmOutput, void* data ) {
	SwarmOutput*	 self                = (SwarmOutput*)     swarmOutput;

	Stg_Component_Build( self->globalIndexVariable, data, False );
}

void _SwarmOutput_Initialise( void* swarmOutput, void* data ) {
	SwarmOutput*	 self                = (SwarmOutput*)     swarmOutput;
	AbstractContext* context             = self->context;
	Swarm*           swarm               = self->swarm;
	Variable*        globalIndexVariable = self->globalIndexVariable;
	Particle_Index   lParticle_I;
	Name             filename;
	Stream*          stream              = Journal_Register( Info_Type, (Name)SwarmOutput_Type  );

   /* re-enable printing from current rank process */
	Stream_SetPrintingRank( stream, context->rank );
   
	Stg_Component_Initialise( self->globalIndexVariable, context, False );

   if( !context->loadSwarmsFromCheckpoint ) _SwarmOutput_SetGlobalIndicies( self, context );

	for ( lParticle_I = 0 ; lParticle_I < swarm->particleLocalCount ; lParticle_I++ ) {
		/* Open file */
		Stg_asprintf( &filename, "%s.%05d.dat", self->baseFilename, Variable_GetValueInt( globalIndexVariable, lParticle_I ) );
		if( context->loadSwarmsFromCheckpoint )
			Stream_AppendFile_WithPrependedPath( stream, context->outputPath, filename );
		else
			Stream_RedirectFile_WithPrependedPath( stream, context->outputPath, filename );

		/* Print to file */
		SwarmOutput_PrintHeader( self, stream, lParticle_I, data );
		Journal_Printf( stream, "\n" );
		Stream_CloseAndFreeFile( stream );
	}
}

void _SwarmOutput_Execute( void* swarmOutput, void* data ) {
	SwarmOutput*	  self                = (SwarmOutput*)     swarmOutput;
	AbstractContext*  context             = (AbstractContext*) data;
	Swarm*            swarm               = self->swarm;
	Variable*         globalIndexVariable = self->globalIndexVariable;
	Particle_Index    lParticle_I;
	Name              filename;
	Stream*           stream              = Journal_Register( Info_Type, (Name)SwarmOutput_Type  );

   /* re-enable printing from current rank process */
	Stream_SetPrintingRank( stream, context->rank );

	Variable_Update( globalIndexVariable );

	for ( lParticle_I = 0 ; lParticle_I < swarm->particleLocalCount ; lParticle_I++ ) {
		/* Open file */
		Stg_asprintf( &filename, "%s.%05d.dat", self->baseFilename, Variable_GetValueInt( globalIndexVariable, lParticle_I ) );
		Stream_AppendFile_WithPrependedPath( stream, context->outputPath, filename );

		/* Print to file */
		SwarmOutput_PrintData( self, stream, lParticle_I, data );
		Journal_Printf( stream, "\n" );
		Stream_Flush( stream );
		Stream_CloseAndFreeFile( stream );
	}
}

void _SwarmOutput_Destroy( void* swarmOutput, void* data ) {
}

/*** Default Implementations ***/
void _SwarmOutput_PrintHeader( void* swarmOutput, Stream* stream, Particle_Index lParticle_I, void* data ) {
	SwarmOutput*	  self                = (SwarmOutput*)     swarmOutput;
	char*             firstBorderString;

	/* Print First Border with '#' in the front */
	firstBorderString = StG_Strdup( self->borderString );
	firstBorderString[0] = '#';
	Journal_Printf( stream, firstBorderString );
	Memory_Free( firstBorderString );

	Journal_PrintString_WithLength( stream, "Timestep", self->columnWidth );

	SwarmOutput_PrintString( self, stream, "Time" );
	SwarmOutput_PrintString( self, stream, "CoordX" );
	SwarmOutput_PrintString( self, stream, "CoordY" );
	SwarmOutput_PrintString( self, stream, "CoordZ" );
}

void _SwarmOutput_PrintData( void* swarmOutput, Stream* stream, Particle_Index lParticle_I, void* data ) {
	SwarmOutput*	  self                = (SwarmOutput*)     swarmOutput;
	AbstractContext*  context             = (AbstractContext*) data;
	Swarm*            swarm               = self->swarm;
	GlobalParticle*   particle            = (GlobalParticle*)Swarm_ParticleAt( swarm, lParticle_I );
	double*           coord               = particle->coord;
		
	SwarmOutput_PrintValue( self, stream, context->timeStep );
	SwarmOutput_PrintValue( self, stream, context->currentTime + AbstractContext_Dt( context ) );
	SwarmOutput_PrintValue( self, stream, coord[ I_AXIS ] );
	SwarmOutput_PrintValue( self, stream, coord[ J_AXIS ] );
	SwarmOutput_PrintValue( self, stream, coord[ K_AXIS ] );
}


/** Virtual Function Wrappers */
void SwarmOutput_Execute( void* swarmOutput, void* context ) {
	SwarmOutput*	  self                = (SwarmOutput*)     swarmOutput;

	self->_execute( self, context );
}
void SwarmOutput_PrintHeader( void* swarmOutput, Stream* stream, Particle_Index lParticle_I, void* context ) {
	SwarmOutput*	  self                = (SwarmOutput*)     swarmOutput;
	self->_printHeader( self, stream, lParticle_I, context );
}
void SwarmOutput_PrintData( void* swarmOutput, Stream* stream, Particle_Index lParticle_I, void* context ) {
	SwarmOutput*	  self                = (SwarmOutput*)     swarmOutput;
	self->_printData( self, stream, lParticle_I, context );
}

void _SwarmOutput_SetGlobalIndicies( void* swarmOutput, void* data ) {
	SwarmOutput*	  self                = (SwarmOutput*)     swarmOutput;
	AbstractContext*  context             = (AbstractContext*) data;
	Swarm*            swarm               = self->swarm;
	Variable*         globalIndexVariable = self->globalIndexVariable;
	Particle_Index    lParticle_I;
	Particle_Index    particleCount       = 0;
	int               rank                = context->rank;
	int               nproc               = context->nproc;
	MPI_Status        status;

	Variable_Update( globalIndexVariable );

	/* Receive total number of particles from previous processor */
	if ( rank != 0 ) {
		(void)MPI_Recv( &particleCount, 1, MPI_UNSIGNED, rank - 1, 0, context->communicator, &status );
	}
	
	/* Assign value to particle */
	for ( lParticle_I = 0 ; lParticle_I < swarm->particleLocalCount ; lParticle_I++ ) {
		Variable_SetValueInt( globalIndexVariable, lParticle_I, particleCount );
		particleCount++;
	}

	/* Send total number of particles next processor */
	if ( rank != nproc - 1 ) {
		(void)MPI_Send( &particleCount, 1, MPI_UNSIGNED, rank + 1, 0, context->communicator );
	}
}

void SwarmOutput_PrintString( void* swarmOutput, Stream* stream, char* string ) {
	SwarmOutput*	  self                = (SwarmOutput*)     swarmOutput;

	Journal_Printf( stream, "%s", self->borderString );
	Journal_PrintString_WithLength( stream, string, self->columnWidth );
}

void SwarmOutput_PrintDouble( void* swarmOutput, Stream* stream, double value ) {
	SwarmOutput*	  self                = (SwarmOutput*)     swarmOutput;

	Journal_Printf( stream, "%s", self->borderString );
	Journal_Printf( stream, self->doubleFormatString, value );
}

void SwarmOutput_PrintTuple( void* swarmOutput, Stream* stream, double* value, Index size ) {
	SwarmOutput*	  self                = (SwarmOutput*)     swarmOutput;
	Index             count;

	Journal_Printf( stream, "%s", self->borderString );
	for( count = 0 ; count < size ; count++ ) {
		Journal_Printf( stream, self->doubleFormatString, value[count] );
	}
}


