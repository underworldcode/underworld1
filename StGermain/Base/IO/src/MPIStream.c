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

#include "Base/Foundation/Foundation.h"

#include "types.h"
#include "shortcuts.h"
#include "File.h"
#include "MPIFile.h"
#include "Stream.h"
#include "MPIStream.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <stdarg.h>   /* Subsequent files need this for variable argument lists. */

#include "Journal.h"


const Type MPIStream_Type = "MPIStream";


Stream* MPIStream_New( Name name )
{
	/* Variables set in this function */
	SizeT                      _sizeOfSelf = sizeof(MPIStream);
	Type                              type = MPIStream_Type;
	Stg_Class_DeleteFunction*      _delete = _MPIStream_Delete;
	Stg_Class_PrintFunction*        _print = _MPIStream_Print;
	Stg_Class_CopyFunction*          _copy = _Stream_Copy;
	Stream_PrintfFunction*         _printf = _MPIStream_Printf;
	Stream_WriteFunction*           _write = _MPIStream_Write;
	Stream_DumpFunction*             _dump = _MPIStream_Dump;
	Stream_SetFileFunction*       _setFile = _MPIStream_SetFile;

	return (Stream*)_MPIStream_New(  MPISTREAM_PASSARGS  );
}

void MPIStream_Init( MPIStream* self, Name name )
{
	
}


MPIStream* _MPIStream_New(  MPISTREAM_DEFARGS  )
{
	MPIStream* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(MPIStream) );
	self = (MPIStream*)_Stream_New(  STREAM_PASSARGS  );
	
	_MPIStream_Init( self );
	
	return self;
}

void _MPIStream_Init( MPIStream* self )
{
	self->defaultFileType = MPIFile_New;
}
	
void _MPIStream_Delete( void* cStream )
{
	MPIStream* self = (MPIStream*)cStream;
	
	/* Stg_Class_Delete parent */
	_Stream_Delete( self );
}

void _MPIStream_Print( void* mpiStream, Stream* stream ) {

	MPIStream* self = (MPIStream*)mpiStream;
	
	/* General info */
	Journal_Printf( stream, "MPIStream (ptr): %p\n", mpiStream );
	
	/* Print parent */
	_Stream_Print( self, stream );
		
}
	
SizeT _MPIStream_Printf( Stream* stream, char *fmt, va_list args )
{
	MPIStream* self = (MPIStream*)stream;
	MPI_Status status;
	char* buffer;
	SizeT numChars;
	int   writeResult;
	
	if ( self->_file == NULL )
	{
		return 0;
	}

	numChars = Stg_vasprintf( &buffer, fmt, args );

	writeResult = MPI_File_write( *(MPI_File*)(self->_file->fileHandle), buffer, numChars, MPI_BYTE, &status );

	if (writeResult != MPI_SUCCESS) {
		char         errorString[2000];
		int          errorStringLength = 0;
		Stream*      errorStream = Journal_Register( Error_Type, MPIFile_Type );
		int          myRank = 0;

		MPI_Comm_rank( MPI_COMM_WORLD, &myRank );
		MPI_Error_string( writeResult, errorString, &errorStringLength);
		Journal_Printf( errorStream, "%3d: %s\n", myRank, errorString );
		File_Close( self->_file );
		MPI_Abort(MPI_COMM_WORLD, writeResult );
	}

	Memory_Free( buffer );
	
	return 0;
}
	
SizeT _MPIStream_Write( Stream* stream, void *data, SizeT elem_size, SizeT num_elems )
{
	MPIStream* self = (MPIStream*)stream;
	MPI_Status status;
	int   writeResult;
	
	writeResult = MPI_File_write( *(MPI_File*)(self->_file->fileHandle), data, num_elems * elem_size,
		 			MPI_BYTE, &status );

	if (writeResult != MPI_SUCCESS) {
		char         errorString[2000];
		int          errorStringLength = 0;
		Stream*      errorStream = Journal_Register( Error_Type, MPIFile_Type );
		int          myRank = 0;

		MPI_Comm_rank( MPI_COMM_WORLD, &myRank );
		MPI_Error_string( writeResult, errorString, &errorStringLength);
		Journal_Printf( errorStream, "%3d: %s\n", myRank, errorString );
		File_Close( self->_file );
		MPI_Abort(MPI_COMM_WORLD, writeResult );
	}

	return num_elems;
}
	
Bool _MPIStream_Dump( Stream* stream, void *data )
{
	/* No specific dumping mechanism, can create in derived classes */
	return False;
}

Bool _MPIStream_SetFile( Stream* stream, File* file )
{
	if ( file->type == MPIFile_Type )
	{
		stream->_file = file;
		return True;
	}
	return False;
}

#define MPIStream_OffsetTag 167

Bool MPIStream_SetOffset( Stream* stream, SizeT sizeToWrite, MPI_Comm communicator ) {
	MPI_Offset    offset    = 0;
	int           rank;
	int           nproc;
	unsigned int  localSizeToWrite;
	unsigned int  sizePartialSum;
	
	if ( stream->_file == NULL ) {
		return False;
	}

	if ( stream->_file->type != MPIFile_Type ) {
		return False;
	}
	
	MPI_Comm_rank( communicator, &rank );
	MPI_Comm_size( communicator, &nproc );

	/* Sum up the individual sizeToWrites for processors lower than this one */
	localSizeToWrite = sizeToWrite;
	MPI_Scan( &localSizeToWrite, &sizePartialSum, 1, MPI_UNSIGNED, MPI_SUM, communicator ); 
	/* Now, just subtract the sizeToWrite of current processor to get our start point */
	offset = sizePartialSum - localSizeToWrite;
	
	MPI_File_seek( *(MPI_File*)stream->_file->fileHandle, offset, MPI_SEEK_SET ); 
	
	return True;
}

SizeT MPIStream_WriteAllProcessors( Stream* stream, void *data, SizeT elem_size, SizeT num_elems, MPI_Comm communicator ) {
	SizeT sizeToWrite = elem_size * num_elems;

	MPIStream_SetOffset( stream, sizeToWrite, communicator );

	return Stream_Write( stream, data, elem_size, num_elems );
}


