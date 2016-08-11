/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University 
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
**       * Redistributions of source code must retain the above copyright notice, 
**          this list of conditions and the following disclaimer.
**       * Redistributions in binary form must reproduce the above copyright 
**         notice, this list of conditions and the following disclaimer in the 
**         documentation and/or other materials provided with the distribution.
**       * Neither the name of the Monash University nor the names of its contributors 
**         may be used to endorse or promote products derived from this software 
**         without specific prior written permission.
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


#ifndef __Underworld_Utils_Context_h__
#define __Underworld_Utils_Context_h__
   
   /* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
   extern const Type UnderworldContext_Type;
   
   #define __UnderworldContext \
      /* Macro defining parent goes here - This means you can cast this class as its parent */ \
      __PICelleratorContext \
      /* Virtual functions go here */ \

   struct UnderworldContext { __UnderworldContext };

   /* Constructors ----------------------------------------------------------------------------------------------------*/
   
   /** Constructor */
   void* _UnderworldContext_DefaultNew( Name name );
   
   UnderworldContext* UnderworldContext_New( 
      Name        name,
      double      start,
      double      stop,
      MPI_Comm    communicator,
      Dictionary* dictionary );

   
   /** Private Constructor: This will accept all the virtual functions for this class as arguments. */
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define UNDERWORLDCONTEXT_DEFARGS \
      PICELLERATORCONTEXT_DEFARGS

   #define UNDERWORLDCONTEXT_PASSARGS \
      PICELLERATORCONTEXT_PASSARGS

   UnderworldContext* _UnderworldContext_New( UNDERWORLDCONTEXT_DEFARGS );
   
   /** Initialisation implementation */
   void _UnderworldContext_Init( UnderworldContext* self );

   /* Virtual Functions -----------------------------------------------------------------------------------------------*/

   void _UnderworldContext_AssignFromXML( void* context, Stg_ComponentFactory* cf, void* data );

   /* Stg_Class_Delete implementation */
   void _UnderworldContext_Delete( void* context );
   
   /* Destroy implmentation  */
   void _UnderworldContext_Destroy( void* component, void* data );

   /* Print implementation */
   void _UnderworldContext_Print( void* context, Stream* stream );
   
   /* Public functions -----------------------------------------------------------------------------------------------*/
   void _UnderworldContext_GenerateOptionsFile( UnderworldContext* context );
   
#endif

