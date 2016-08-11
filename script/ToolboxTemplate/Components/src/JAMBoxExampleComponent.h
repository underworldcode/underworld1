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
*%  Louis Moresi - Louis.Moresi(at)monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __JAMBoxExampleComponent_h__
#define __JAMBoxExampleComponent_h__

extern const Type JAMBoxExampleComponent_Type;

#define __JAMBoxExampleComponent \
   __Stg_Component \
   AbstractContext*	context; \
   Bool		         someParameter;

struct JAMBoxExampleComponent
{
   __JAMBoxExampleComponent
};

/** Constructors */
JAMBoxExampleComponent* JAMBoxExampleComponent_New(
   Name  name,
   Bool  someParametr );

#ifndef ZERO
#define ZERO 0
#endif

#define JAMBoxEXAMPLECOMPONENT_DEFARGS \
                STG_COMPONENT_DEFARGS

#define JAMBoxEXAMPLECOMPONENT_PASSARGS \
                STG_COMPONENT_PASSARGS

JAMBoxExampleComponent* _JAMBoxExampleComponent_New(  JAMBoxEXAMPLECOMPONENT_DEFARGS  );

/** Virtual Functions */
void _JAMBoxExampleComponent_Init(
   void*  _self,
   Bool   someParameter );

void  _JAMBoxExampleComponent_Delete( void* _self );
void* _JAMBoxExampleComponent_DefaultNew( Name name );
void  _JAMBoxExampleComponent_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
void  _JAMBoxExampleComponent_Build( void* _self, void* data );
void  _JAMBoxExampleComponent_Initialise( void* _self, void* data );
void  _JAMBoxExampleComponent_Execute( void* _self, void* data );
void  _JAMBoxExampleComponent_Destroy( void* _self, void* data );

#endif

