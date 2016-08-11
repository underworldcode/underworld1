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
**
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifndef __StGermain_Base_Automation_LiveComponentRegister_h__
#define __StGermain_Base_Automation_LiveComponentRegister_h__

   extern const Type LiveComponentRegister_Type;
   extern LiveComponentRegister* stgLiveComponentRegister; /* Live components singleton. */
   
   #define __LiveComponentRegister \
      /* General info */ \
      __Stg_Class \
      \
      /* Virtual info */ \
      \
      /* Class info */ \
      Stg_ObjectList         *componentList;
      
   struct LiveComponentRegister { __LiveComponentRegister };
   
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define LIVECOMPONENTREGISTER_DEFARGS \
      STG_CLASS_DEFARGS

   #define LIVECOMPONENTREGISTER_PASSARGS \
      STG_CLASS_PASSARGS

   LiveComponentRegister* _LiveComponentRegister_New( LIVECOMPONENTREGISTER_DEFARGS );
   
   LiveComponentRegister* LiveComponentRegister_New(  );

   void _LiveComponentRegister_Init( LiveComponentRegister *self );
   
   void LiveComponentRegister_Init( LiveComponentRegister *self );

   void _LiveComponentRegister_Delete( void* liveComponentRegister );
   
   void _LiveComponentRegister_Print( void* liveComponentRegister, Stream* stream );
   
   Index LiveComponentRegister_Add( LiveComponentRegister *self, Stg_Component *component );

   /* 
    * If the live component register exists then add this component to it
    * returns 1 on success, 0 on failure to find LiveComponentRegister
    */
   Index LiveComponentRegister_IfRegThenAdd( Stg_Component *component );
   
   Stg_Component *LiveComponentRegister_Get( LiveComponentRegister *self, Name name );

   Stg_Component *LiveComponentRegister_At( void* liveComponentRegister, Index index );
   
   /* 
    * Delete's the entry referring to a component, but not the *component referred to*. This
    * is useful when another class needs to delete a component from the lcRegister before the 
    * lcRegister does it as part of DeleteAll - such as with Codelets which must be deleted
    * before the plugin dlls are closed. 
    */
   Index LiveComponentRegister_RemoveOneComponentsEntry( void* liveComponentRegister, Name name );

   unsigned int LiveComponentRegister_GetCount( LiveComponentRegister *self );

   void LiveComponentRegister_BuildAll( void* liveComponentRegister, void* data );

   void LiveComponentRegister_InitialiseAll( void* liveComponentRegister, void* data );

   void LiveComponentRegister_DeleteAll( void* liveComponentRegister) ;

   LiveComponentRegister* LiveComponentRegister_GetLiveComponentRegister();

   void LiveComponentRegister_PrintSimilar( void* liveComponentRegister, Name name, Stream* stream, unsigned int number );

   void LiveComponentRegister_DestroyAll( void* lcReg );

#endif /* __StGermain_Base_Automation_LiveComponentRegister_h__ */

