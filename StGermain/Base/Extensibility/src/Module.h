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
**
*/
/** \file
**  Role:
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StGermain_Base_Extensibility_Module_h__
#define __StGermain_Base_Extensibility_Module_h__
   
   /* The prototype for the virtual functions in a module */
   typedef char*       (Module_MangleNameFunction)  ( char* name );

   typedef const char* (Module_GetDepsFunction)     ( void );
   
   /** Textual name of this class */
   extern const Type Module_Type;

   /* Modules info */
   #define __Module \
      /* General info */ \
      __Stg_Object \
      \
      /* Virtual info */ \
      Module_MangleNameFunction*  MangleName; \
      \
      /* Module info */ \
      DLL_Handle                  dllPtr; \
      Module_GetDepsFunction*     GetDeps;    \
      char*                       mangledName;
      
   struct Module { __Module };

   /* Creation implementation / Virtual constructor */
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define MODULE_DEFARGS \
      STG_OBJECT_DEFARGS, \
      Module_MangleNameFunction* MangleName, \
      Stg_ObjectList*            directories

   #define MODULE_PASSARGS \
      STG_OBJECT_PASSARGS, \
      MangleName,  \
      directories

   Module* _Module_New( MODULE_DEFARGS );
   
   /* Initialisation implementation */
   void _Module_Init(
      Module*                   self,
      Module_MangleNameFunction MangleName,
      Stg_ObjectList*           directories );
   
   /* Stg_Class_Delete implementation */
   void _Module_Delete( void* plugin );
   
   /* Print implementation */
   void _Module_Print( void* plugin, Stream* stream );

   /** Return the mangled (symbol and file) name. Note: result needs to be freed */
   char* Module_MangledName( void* module );

   /** Load a specific symbol of the module, where the symbol is prefixed by the module name */
   void* Module_LoadSymbol( void* module, const char* suffix );

   /** Un load the module */
   void Module_UnLoad( void* module );
   
#endif /* __StGermain_Base_Extensibility_Module_h__ */

