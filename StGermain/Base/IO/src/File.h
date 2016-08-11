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
**	Abstract representation of a file.
**
** <b>Assumptions:</b>
**	None
**
** <b>Comments:</b>
**	The abstraction of this class has come from a number of reasons:
**	- Other than using traditional C files, StGermain will use HDF5 and potentially other alternatives for writing data to file.
**	  These use different functions other than fopen(), fclose()...etc. 
**
**	  Hence a level of abstract is created to allow for various implementations.
**
**	- The concept of 'File' is separated from 'Stream' to allow more flexibility. It allows users to direct output of a
**	  child stream to a different location from the parent.
**
**	  However, this will introduce a potential of mis-use when uses accidentally attempts to set a HDF5 file to a normal
**	  CStream or vice-versa. Stream_SetFile will catch this error and will print a warning message.
**
** <b>Description:</b>
**	Parent class for all files. Sub classes should implement the open, close and flush functions.
**	File is a sub-class of NamedObject, where its name is the name of the file it has opened if any.
**
**
**/

#ifndef __StGermain_Base_IO_File_h__
#define __StGermain_Base_IO_File_h__

	/** Textual name for File class. */
	extern const Type File_Type;

	/** Search paths for these files */
	extern Stg_ObjectList* _stgFilePaths;

	typedef File* (File_Constructor) (void);
	typedef Bool (File_ReadFunction) (void* file, const char* const fileName );
	typedef Bool (File_WriteFunction) (void* file, const char* const fileName );
	typedef Bool (File_AppendFunction) (void* file, const char* const fileName );
	typedef Bool (File_CloseFunction) (void* file );
	typedef Bool (File_FlushFunction) (void* file );
		
	/** \def __File See File */
	#define __File \
		__Stg_Object					\
								\
		/* Virtual functions */				\
		File_ReadFunction*	_read; 		\
		File_WriteFunction*	_write; 	\
		File_AppendFunction*	_append; 	\
		File_CloseFunction*	_close; 	\
		File_FlushFunction*	_flush; 	\
								\
		/* File members */			\
		void*				fileHandle;	/* Generic file handle for any file. */ \
		Bool				_opened; \
		int				_lastOpenedAs;
	struct File { __File };


	/** Constructor interface. */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define JOURNALFILE_DEFARGS \
                STG_CLASS_DEFARGS, \
                File_ReadFunction*     _read, \
                File_WriteFunction*    _write, \
                File_AppendFunction*   _append, \
                File_CloseFunction*    _close, \
                File_FlushFunction*    _flush

	#define JOURNALFILE_PASSARGS \
                STG_CLASS_PASSARGS, \
	        _read,   \
	        _write,   \
	        _append, \
	        _close,  \
	        _flush 

	File* _File_New(  JOURNALFILE_DEFARGS  );
	
	/** Init interface. */
	void _File_Init(
		File*			self,
		File_ReadFunction*	_read,
		File_WriteFunction*	_write,
		File_AppendFunction*	_append,
		File_CloseFunction*	_close,
		File_FlushFunction*	_flush );
	
	/** Stg_Class_Delete interface. */
	void _File_Delete( void* file );

	/** Print interface. */
	void _File_Print( void* file, Stream* stream );		
	

	/** Open a file for reading. If successful, sets the name for this instance to the file name and returns True.
	 **
	 ** @param file An instance of a File sub-class.
	 **/
	Bool File_Read( void* file, const char* const fileName );
	
	
	/** Open a file for writing. If successful, sets the name for this instance to the file name and returns True.
	 **
	 ** @param file An instance of a File sub-class.
	 **/
	Bool File_Write( void* file, const char* const fileName );
	
	
	/** Reopen a file for writing. It will use the previous name it was opened with.
	 **
	 ** @param file An instance of a File sub-class.
	 **/
	Bool File_Reopen( void* file );
	
	/** Macro to improve readability */
	#define File_ReopenIfClosed File_Reopen
	
	/** Opens a file for writing, if the file exists then append future print statements to this file.
	 ** If successful, sets the name for this instance to the file name and returns True.
	 **
	 ** @param file An instance of a File sub-class.
	 **/
	Bool File_Append( void* file, const char* const fileName );

	/** Close a file. If successful, returns True.
	 **
	 ** @param file An instance of a File sub-class.
	 **/
	Bool File_Close( void* file );

	/** Checks whether a file is opened.
	 **
	 ** @param file An instance of a File sub-class.
	 **/
	Bool File_IsOpened( void* file );

	/** Flush a file, outputting all un-written data from input buffers to device/file. If successful, returns True.
	 **
	 ** @param file An instance of a File sub-class.
	 **/
	Bool File_Flush( void* file );

	/** Add a default lookup fail - where read-opens fail because file is not found, will lookup.
	 **
	 ** @param file An instance of a File sub-class.
	 ** @param directory Directory/path to add to look up
	 **/
	void File_AddPath( char* directory );


#endif /* __StGermain_Base_IO_File_h__ */

