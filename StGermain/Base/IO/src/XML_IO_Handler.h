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
*/
/** \file
**  Role:
**   Allows input/output from XML files. \see IO_Handler.h
**
** Assumptions:
**   Maximum string length in raw data output is limited to 100 characters. I
**   could make this dynamic, however I can't foresee a need for it.
**
** Comments:
**   TODO: include files of different types (eg nastran etc)
**   TODO: validation (more a dictionary feature actually)
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StGermain_Base_IO_XML_IO_Handler_h__
#define __StGermain_Base_IO_XML_IO_Handler_h__
   
   #include <libxml/tree.h>
   
   /* Enum to determine type of list format. */
   typedef enum { PerList, AllRawASCII, AllRawBinary, AllXML } ListEncoding;
   
   /* Textual class name. */
   extern const Type XML_IO_Handler_Type;
   
   extern const xmlChar* APPEND_TAG;
   extern const xmlChar* MERGE_TAG;
   extern const xmlChar* REPLACE_TAG;
   /* A useful map for getting xml versions of Dictionary_MergeTypes. */
   extern const xmlChar* XML_IO_Handler_MergeTypeMap[3];

   /* The Xml search path list. */
   extern Stg_ObjectList* Project_XMLSearchPaths;
   
   /* Additional class function ptr typedefs. */
   typedef Bool (XML_IO_Handler_WriteEntryToFileFunction) ( void* xml_io_handler, const char* filename,
      Dictionary_Entry_Key name, Dictionary_Entry_Value* value, Dictionary_Entry_Source source );
   typedef void (XML_IO_Handler_SetListEncodingFunction) ( void* xml_io_handler, ListEncoding listEncoding );
   typedef void (XML_IO_Handler_SetWritingPrecisionFunction) ( void* xml_io_handler, int dictionaryEntryType,
      int value );
   typedef void (XML_IO_Handler_SetWriteExplicitTypesFunction) ( void* xml_io_handler, Bool writeExplicitTypes);
   
   /* Data struct to store namespaces. */
   typedef struct NameSpaceInfo {
      char*  location;
      char*  version;
      struct NameSpaceInfo* next;
   } NameSpaceInfo;   
   
   #define __XML_IO_Handler  \
      /* General info */ \
      __IO_Handler \
      \
      /* Virtual info */ \
      XML_IO_Handler_WriteEntryToFileFunction*      _writeEntryToFile; \
      XML_IO_Handler_SetListEncodingFunction*       _setListEncoding; \
      XML_IO_Handler_SetWritingPrecisionFunction*   _setWritingPrecision; \
      XML_IO_Handler_SetWriteExplicitTypesFunction* _setWriteExplicitTypes; \
      \
      /* XML_IO_Handler */ \
      NameSpaceInfo*                                nameSpacesList; /*< Allows chain of namespaces. */\
      xmlDocPtr                                     currDoc; \
      xmlNsPtr                                      currNameSpace; \
      int                                           tokeniserCalls; \
      ListEncoding                                  listEncoding; \
      Stg_ObjectList*                               typeKeywords; /*< Maps strings to dictionary types. */\
      char*                                         TYPE_KEYWORDS[Dictionary_Entry_Value_Num_Types + 1]; \
      unsigned int                                  writingPrecision[Dictionary_Entry_Value_Num_Types + 1]; \
      unsigned int                                  writingFieldWidth[Dictionary_Entry_Value_Num_Types + 1]; \
      unsigned int                                  WRITING_FIELD_EXTRAS[Dictionary_Entry_Value_Num_Types + 1]; \
      Bool                                          writeExplicitTypes; \
      Bool                                          writeSources; \
      \
      unsigned                                      searchPathsSize; \
      char**                                        searchPaths; \

   struct _XML_IO_Handler { __XML_IO_Handler };
   
   
   /* Create a new XML_IO_Handler and initialise. */
   XML_IO_Handler* XML_IO_Handler_New( void );
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define XML_IO_HANDLER_DEFARGS \
      IO_HANDLER_DEFARGS, \
      XML_IO_Handler_WriteEntryToFileFunction*      _writeEntryToFile, \
      XML_IO_Handler_SetListEncodingFunction*       _setListEncoding, \
      XML_IO_Handler_SetWritingPrecisionFunction*   _setWritingPrecision, \
      XML_IO_Handler_SetWriteExplicitTypesFunction* _setWriteExplicitTypes

   #define XML_IO_HANDLER_PASSARGS \
      IO_HANDLER_PASSARGS, \
      _writeEntryToFile, \
      _setListEncoding, \
      _setWritingPrecision, \
      _setWriteExplicitTypes

   XML_IO_Handler* _XML_IO_Handler_New( XML_IO_HANDLER_DEFARGS );
   
   /* Initialise a XML_IO_Handler construct. */
   void XML_IO_Handler_Init( XML_IO_Handler* self );
   
   /* Initialisation implementation. */
   void _XML_IO_Handler_Init( XML_IO_Handler* self );
   
   /* Stg_Class_Delete instantiation. */
   extern void _XML_IO_Handler_Delete( void* xml_io_handler ); 
   
   /* Print instantiation. */
   extern void _XML_IO_Handler_Print( void* xml_io_handler, Stream* stream );
   
   /* Set the list encoding that controls whether lists are stored as xml, ascii or binary. */
   extern void XML_IO_Handler_SetListEncoding( void* xml_io_handler, ListEncoding listEncoding );
   extern void _XML_IO_Handler_SetListEncoding( void* xml_io_handler, ListEncoding listEncoding );
   
   extern void XML_IO_Handler_SetWriteExplicitTypes( void* xml_io_handler, Bool writeExplicitTypes );
   extern void _XML_IO_Handler_SetWriteExplicitTypes( void* xml_io_handler, Bool writeExplicitTypes );
   
   /* Set the precision with which each dictionary type is output. */
   extern void XML_IO_Handler_SetWritingPrecision( void* xml_io_handler, int dictionaryEntryType, int value );
   extern void _XML_IO_Handler_SetWritingPrecision( void* xml_io_handler, int dictionaryEntryType, int value );
   
   void _XML_IO_Handler_AddNameSpace( void* io_handler, char* location, char* version );
   
   extern Bool _XML_IO_Handler_CheckNameSpace( XML_IO_Handler* self, xmlNodePtr curNode );
   
   /* Read a dictionary entry of a given name from file. */
   extern Bool _XML_IO_Handler_ReadAllFromFile(
      void*       xml_io_handler,
      const char* filename, 
      Dictionary* dictionary,
      Dictionary* sources );
   
   /* 
    * Read a dictionary entry of a given name from file and force the source file
    * info to be added to the Dictionary_Entry.
    */
   extern Bool _XML_IO_Handler_ReadAllFromFileForceSource(
      void*       xml_io_handler,
      const char* filename,
      Dictionary* dictionary,
      Dictionary* sources );

   /* Read a dictionary entry of a given name from buffer. */
   extern Bool _XML_IO_Handler_ReadAllFromBuffer(
      void*       xml_io_handler,
      const char* buffer,
      Dictionary* dictionary,
      const char* tag );
   
   /* Write a given dictionary entry to file. */
   /* TODO: option to choose between overwrite / update ? */
   extern Bool _XML_IO_Handler_WriteAllToFile(
      void*       xml_io_handler,
      const char* filename,
      Dictionary* dictionary,
      Dictionary* sources,
      Dictionary* revisions,
      Dictionary* branches,
      Dictionary* paths );
   
   /* TODO: option to choose between overwrite / update ? */
   extern Bool XML_IO_Handler_WriteEntryToFile(
      void*                   xml_io_handler,
      const char*             filename,
      Dictionary_Entry_Key    name, 
      Dictionary_Entry_Value* value,
      Dictionary_Entry_Source source );
   
   extern Bool _XML_IO_Handler_WriteEntryToFile(
      void*                   xml_io_handler,
      const char*             filename,
      Dictionary_Entry_Key    name,
      Dictionary_Entry_Value* value,
      Dictionary_Entry_Source source );

   void XML_IO_Handler_AddDirectory( Name name, char* directory ); 

   /* 
    * A function to handle errors produced by libXML through the Stg Stream system.
    * Should be hooked up using xmlSetGenericErrorFunc(), e.g. at the Init() stage. 
    */
   void XML_IO_Handler_LibXMLErrorHandler( void* ctx, const char* msg, ... );
   /* little helper function which writes flattened xml dictionary to memory (returned char*) */
   char* _XML_IO_Handler_WriteAllMem(
      void*       xml_io_handler,
      Dictionary* dictionary,
      Dictionary* sources,
      Dictionary* versions,
      Dictionary* branches,
      Dictionary* paths );


#endif /* __StGermain_Base_IO_XML_IO_Handler_h__ */

