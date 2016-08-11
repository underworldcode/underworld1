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


#ifdef HAVE_PYTHON
	#include <Python.h>
#endif
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const Type Underworld_Type = "Underworld";

int main( int argc, char* argv[] ) {
	/* StGermain standard bits & pieces */
	MPI_Comm			CommWorld;
	int				rank;
	int				numProcessors;
	Dictionary*			dictionary;
	XML_IO_Handler*			ioHandler;
	Stream*                         stream;
	Stg_ObjectList*                 inputPaths;
	Index                           i;

	/* Initialise PETSc, get world info */
	MPI_Init( &argc, &argv );
	MPI_Comm_dup( MPI_COMM_WORLD, &CommWorld );
	MPI_Comm_size( CommWorld, &numProcessors );
	MPI_Comm_rank( CommWorld, &rank );
	StGermain_Init( &argc, &argv );
	stream = Journal_Register( Info_Type, (Name)Underworld_Type );
	#ifdef HAVE_PYTHON
		Py_Initialize();
	#endif	
	MPI_Barrier( CommWorld ); /* Ensures copyright info always come first in output */
	
	/* Parse the input path command line argument... needed before we start parsing the input */
	/* And add the path to the global xml path dictionary */
	inputPaths = stgParseInputPathCmdLineArg( &argc, &argv );
	for( i = 0; i < Stg_ObjectList_Count( inputPaths ); i++ ) {
		char* inputPath = (char*)Stg_ObjectAdaptor_Object( (Stg_ObjectAdaptor*)Stg_ObjectList_At( inputPaths, i ) );
		XML_IO_Handler_AddDirectory( "--inputPath", inputPath );
	}

	/* Create the application's dictionary & read input */
	dictionary = Dictionary_New();
	ioHandler = XML_IO_Handler_New( );
	IO_Handler_ReadAllFromCommandLine( ioHandler, argc, argv, dictionary );
	Journal_ReadFromDictionary( dictionary );

	/* Error check for input parameters in the dictionary */
	if( dictionary->count == 0 ) {
		printf("********************************************\n"
		       "Error in function %s\n"
		       "%d input parameters have been defined.\n"
		       "check what you are passing to the executable\n"
		       "********************************************\n",
			 __func__, dictionary->count );
		abort();
	}

	stgImportToolbox( dictionary, "Underworld" );
	 
	stgMainLoop( dictionary, CommWorld );
	
	Stg_Class_Delete( dictionary );
	
	/* Close off everything */
	Stg_Class_Delete( inputPaths );
	#ifdef HAVE_PYTHON
		Py_Finalize();
	#endif
	StGermain_Finalise();
	Journal_RPrintf( stream, "Finalised: StGermain Framework.\n");
	MPI_Finalize();
	
	return 0; /* success */
}


