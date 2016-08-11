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
*%  Owen Kaluza - Owen.Kaluza(at)monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifndef __lucDrawingObject_Register_h__
#define __lucDrawingObject_Register_h__

extern const Type lucDrawingObject_Register_Type;

#define __lucDrawingObject_Register \
		/* Macro defining parent goes here - This means you can cast this class as its parent */ \
		__NamedObject_Register \
		\
		/* Virtual functions go here */ \
		\
		/* Class info */ \
 
struct lucDrawingObject_Register
{
   __lucDrawingObject_Register
};


/*--------------------------------------------------------------------------------------------------------------------------
** Constructor
*/

lucDrawingObject_Register*	lucDrawingObject_Register_New( void );

/*-----------------------------------------------------------------------------------------------------------------
** General virtual functions
*/

/*-----------------------------------------------------------------------------------------------------------------
** Public functions
*/
#define lucDrawingObject_Register_Add NamedObject_Register_Add

#define lucDrawingObject_Register_GetIndex NamedObject_Register_GetIndex

#define lucDrawingObject_Register_GetByName( self, materialName ) \
		( (lucDrawingObject*) NamedObject_Register_GetByName( self, materialName ) )

#define lucDrawingObject_Register_GetByIndex( self, materialIndex ) \
		( (lucDrawingObject*) NamedObject_Register_GetByIndex( self, materialIndex ) )

#define lucDrawingObject_Register_GetCount( self ) \
		(self)->objects->count

#define lucDrawingObject_Register_DeleteAllObjects( self ) \
		Stg_ObjectList_DeleteAllObjects( (self)->objects )
#define lucDrawingObject_Register_PrintAllObjects( self, stream ) \
		Stg_ObjectList_PrintAllObjects( (self)->objects, stream )

/* +++ Public Functions +++ */
void lucDrawingObject_Register_SetupAll( void* drawingObject_Register, lucDatabase* database) ;
void lucDrawingObject_Register_DrawAll( void* drawingObject_Register, lucDatabase* database) ;
void lucDrawingObject_Register_CleanUpAll( void* drawingObject_Register, void* context ) ;


#endif
