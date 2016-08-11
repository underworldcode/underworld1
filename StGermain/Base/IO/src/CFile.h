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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/** \file
** <b>Role:</b>
**	Encapsulation of a C file.
**
** <b>Assumptions:</b>
**	None.
**
** <b>Comments:</b>
**	Be careful in parallel situations where more than one processor may try to open the same file due
**	to executing the same piece of code. When writing to files other than stdout and stderr, only
**	allow one process to perform output.
**
** <b>Description</b>
**	CFile wraps around traditional C file operations such as fopen() and fclose() to alloc output
**	for a file.
**
**
**/

#ifndef __StGermain_Base_IO_CFile_h__
#define __StGermain_Base_IO_CFile_h__

	#include <stdio.h>
	#include "File.h"
	/** Textual name for CFile class. */
	extern const Type CFile_Type;
	
	/** \def __CFile See CFile. */
	#define __CFile \
		/* General info */ \
		__File  \
		/* open a binary file? */ \
		Bool   binary;
	struct CFile { __CFile };


	/** Creates a new CFile with no opened file. File_Read/Write/Append() must be called. */
	File* CFile_New();

	/** Creates a new CFile for reading, opening the file given by fileName. Returns NULL if system is unable to open the given file. */
	File* CFile_NewRead( char* fileName );

	/** Creates a new CFile for writing, opening the file given by fileName. Returns NULL if system is unable to open the given file. */
	File* CFile_NewWrite( char* fileName );

	/** Creates a new binary CFile with no opened file. File_Open() must be called. */
	File* CFileBinary_New();
	
	/** Initialises an instance of CFile. Will not open any files. */
	void CFile_Init( CFile* self );


	/** Constructor interface. */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define CFILE_DEFARGS \
                STG_CLASS_DEFARGS, \
                Bool  binary

	#define CFILE_PASSARGS \
                STG_CLASS_PASSARGS, \
	        binary

	CFile* _CFile_New(  CFILE_DEFARGS  );
		
	/** Init interface. */
	void _CFile_Init( CFile* self );
	
	/** Stg_Class_Delete interface. */
	void _CFile_Delete( void* cfile );

	/** Print interface. */
	void _CFile_Print( void* cfile, Stream* stream );
	
	
	/** Implementation for Read function. */
	Bool _CFile_Read( void* file, char* fileName );

	/** Implementation for Write function. */
	Bool _CFile_Write( void* file, char* fileName );

	/** Implementation for Append function. */
	Bool _CFile_Append( void* file, char* fileName );

	/** Implementation for Close function. */
	Bool _CFile_Close( void* file );

	/** Implementation for Flush function. */
	Bool _CFile_Flush( void* file );
	
	/** Obtain the FILE pointer */
	FILE* CFile_Ptr( void* file );


#endif /* __StGermain_Base_IO_CFile_h__ */



