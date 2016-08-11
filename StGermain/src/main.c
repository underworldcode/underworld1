/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**   Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**   Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**   AuScope - http://www.auscope.org
**   Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**   Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**   Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**   Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**   Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
**   Kent Humphries, Software Engineer, VPAC. (kenth@vpac.org)
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

#ifdef HAVE_PYTHON
   #include <Python.h>
#endif
#ifdef HAVE_SDL /* On Mac OS X, SDL needs to redefine main() so SDL.h must be included here */
   #include <SDL/SDL.h>
#endif
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const Type StGermain_Type = "StGermain";

static void deleteListArgItem( void* ptr ) {
   /* Correct way to delete result of stgParseList[All]CmdLineArg */
   Memory_Free( ptr );
}

typedef void (ForEachFunc) ( Stream* stream, Type componentType );

static void forEachComponent( Stream* stream, ForEachFunc* forEach ) {
   int                           i;
   Stg_ComponentRegister*        cr;
   Stg_ComponentRegisterElement* cre;

   cr = Stg_ComponentRegister_Get_ComponentRegister();
   for( i = 0; i < Stg_ComponentRegister_GetCount(cr); i++ ) {
      cre = Stg_ComponentRegister_GetByIndex(cr, i);
      Type componentType = Stg_ComponentRegisterElement_GetType( cre );
      forEach( stream, componentType );
   }
}

static void forEachLiveComponent( Stream* stream, ForEachFunc* forEach ) {
   Stg_ObjectList* uniqueComponentTypes;
   Index           i;

   /* Add each instantiated component type to a list, ensuring the list is of unique entries */
   uniqueComponentTypes = Stg_ObjectList_New();
   for( i = 0; i < LiveComponentRegister_GetCount( stgLiveComponentRegister ); i ++ ) {
      Type componentType = Stg_Class_GetType( LiveComponentRegister_At( stgLiveComponentRegister, i ) );
      Index j;
      Bool found;

      found = False;
      for( j = 0; componentType && j < Stg_ObjectList_Count( uniqueComponentTypes ); j++ ) {
         char* added = (char*)Stg_ObjectAdaptor_Object( (Stg_ObjectAdaptor*)Stg_ObjectList_At( uniqueComponentTypes, j ) );
         if( strcmp( componentType, added ) == 0 ) {
            found = True;
         }
      }
      if( componentType && !found ) {
         Stg_ObjectList_PointerAppend( uniqueComponentTypes, componentType, 0, 0, 0, 0 );
      }
   }

   for( i = 0; i < Stg_ObjectList_Count( uniqueComponentTypes ); i ++ ) {
      Type componentType = (char*)Stg_ObjectAdaptor_Object( (Stg_ObjectAdaptor*)Stg_ObjectList_At( uniqueComponentTypes, i ) );
      forEach( stream, componentType );
   }
   
   Stg_Class_Delete( uniqueComponentTypes );
}

static void printComponentListItem( Stream* stream, Type componentType ) {
   Journal_Printf( stream, "\t'%s'\n", componentType );
}

int main( int argc, char* argv[] ) {
   /* StGermain standard bits & pieces */
   MPI_Comm              CommWorld;
   int                   rank;
   int                   numProcessors;
   XML_IO_Handler*       ioHandler = NULL;
   Stream*               stream = NULL;
   Stg_ObjectList*       inputPaths = NULL;
   Stg_ObjectList*       listAllTopics = NULL;
   char*                 helpTopic = NULL;
   char*                 listAllTopic = NULL;
   char*                 listTopic = NULL;
   char*                 inputPath = NULL;
   Stg_ObjectList*       listTopics;
   Stg_ComponentFactory* cf;
   Index                 i;
   PluginsManager*       plugins = NULL;
   Dictionary*           dictionary = NULL;
   Dictionary*           sources = NULL;

   /* Initialise PETSc, get world info */
   MPI_Init( &argc, &argv );
   MPI_Comm_dup( MPI_COMM_WORLD, &CommWorld );
   MPI_Comm_size( CommWorld, &numProcessors );
   MPI_Comm_rank( CommWorld, &rank );
   StGermain_Init( &argc, &argv );
   stream = Journal_Register( Info_Type, StGermain_Type );

   #ifdef HAVE_PYTHON
      Py_Initialize();
   #endif   
   /* Ensures copyright info always come first in output */
   MPI_Barrier( CommWorld ); 

   #ifdef NOSHARED
      register_static_modules();
   #endif
   
   /* 
    * Parse the input path command line argument... needed before we start parsing the input.
    * And add the path to the global xml path dictionary. 
    */
   inputPaths = stgParseInputPathCmdLineArg( &argc, &argv );
   for( i = 0; i < Stg_ObjectList_Count( inputPaths ); i++ ) {
      inputPath = (char*)Stg_ObjectAdaptor_Object( (Stg_ObjectAdaptor*)Stg_ObjectList_At( inputPaths, i ) );
      XML_IO_Handler_AddDirectory( (Name)"--inputPath", inputPath );
      File_AddPath( inputPath );
   }

   /* Create the application's dictionary & read input. */
   dictionary = Dictionary_New();
   sources = Dictionary_New();
   ioHandler = XML_IO_Handler_New();
   IO_Handler_ReadAllFromCommandLine( ioHandler, argc, argv, dictionary, sources );
   Journal_ReadFromDictionary( dictionary );

   /* now dereference aliases */
   DictionaryUtils_AliasDereferenceDictionary( dictionary );
   
   /* Parse the additional (StGermain exe specific) commandline arguments (that require dictionary already filled). */
   helpTopic = stgParseHelpCmdLineArg( &argc, &argv );
   listAllTopics = Stg_ObjectList_New();

   while( ( listAllTopic = stgParseListAllCmdLineArg( &argc, &argv ) ) != 0 ) {
      Stg_ObjectList_Append( listAllTopics, Stg_ObjectAdaptor_NewOfPointer( listAllTopic, 0, True, False, deleteListArgItem, 0, 0 ) );
   }

   listTopics = Stg_ObjectList_New();
   while( ( listTopic = stgParseListCmdLineArg( &argc, &argv ) ) != 0 ) {
      Stg_ObjectList_PointerAppend( listTopics, listTopic, 0, deleteListArgItem, 0, 0 );
   }

   /*
    * If the command line arguments ask for "help" or "list-all", then load the toolboxes and plugins (to include all 
    * symbols/components, and print the help for the selected component, but don't run the wholse app. There can be
    * multiple "list-all"s..
    */
   if( helpTopic || Stg_ObjectList_Count( listAllTopics ) ) {
      plugins = PluginsManager_New();

      ModulesManager_Load( stgToolboxesManager, dictionary, (Name)"" );
      ModulesManager_Load( plugins, dictionary, (Name)"context" );

      /* "--help" parameter */
      if( helpTopic ) {
         Journal_Printf( stream, "Help topic not implemented .\n" );
         assert(0);
         Memory_Free( helpTopic );
         helpTopic = 0;
      }

      /* "--list-all" parameters */
      for( i = 0; i < Stg_ObjectList_Count( listAllTopics ); i++ ) {
         listAllTopic = (char*)Stg_ObjectAdaptor_Object( (Stg_ObjectAdaptor*)Stg_ObjectList_At( listAllTopics, i ) );

         if( strcmp( listAllTopic, "components" ) == 0 ) {
            Journal_Printf( stream, "Registered components are:\n" );
            forEachComponent( stream, printComponentListItem );
         }
         else {
            Journal_Printf( stream, "List-all topic '%s' not found.\n", listAllTopic );
            Journal_Printf( stream, "Available topics are:\n\t'components'\n\t'references'\n\t'equations'\n\t'rights'\n", listAllTopic );
         }
      }

      Stg_Class_Delete( plugins );
   }
   else {  /* ... run the app */
      ModulesManager_Load( stgToolboxesManager, dictionary, (Name)"" );

      /* Magic happens here! */
      cf = stgMainConstruct( dictionary, sources, CommWorld, NULL );
      stgMainBuildAndInitialise( cf );
      stgMainLoop( cf );

      /* "--list" parameters */
      for( i = 0; i < Stg_ObjectList_Count( listTopics ); i++ ) {
         listTopic = (char*)Stg_ObjectAdaptor_Object( (Stg_ObjectAdaptor*)Stg_ObjectList_At( listTopics, i ) );

         if( strcmp( listTopic, "components" ) == 0 ) {
            Journal_Printf( stream, "Instantiated/used components are:\n" );
            forEachLiveComponent( stream, printComponentListItem );
         }
         else {
            Journal_Printf( stream, "List topic '%s' not found.\n", listTopic );
            Journal_Printf( stream, "Available topics are:\n\t'components'\n\t'references'\n\t'equations'\n\t'rights'\n", listTopic );
         }
      }
      stgMainDestroy( cf );
   }

   /* Close off everything */
   Stg_Class_Delete( listAllTopics );
   Stg_Class_Delete( listTopics );
   Stg_Class_Delete( inputPaths );
   #ifdef HAVE_PYTHON
      Py_Finalize();
   #endif
   StGermain_Finalise();
   MPI_Finalize();
   
   return 0; /* success */
}
