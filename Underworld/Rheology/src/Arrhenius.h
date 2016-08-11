/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
**       * Redistributions of source code must retain the above copyright notice,
**          this list of conditions and the following disclaimer.
**       * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**       * Neither the name of the Monash University nor the names of its contributors
**       may be used to endorse or promote products derived from this software
**       without specific prior written permission.
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


#ifndef __Underworld_Rheology_Arrhenius_h__
#define __Underworld_Rheology_Arrhenius_h__

   /** Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
   extern const Type Arrhenius_Type;

   /** Class contents */
   #define __Arrhenius \
      /* Parent macro */ \
      __Rheology \
      /* Virtual functions go here */ \
      /* Other Info */\
      int                  temperature_id;        \
      double               eta0;                    \
      double               activationEnergy;        \
      double               activationVolume;        \
      double               referenceTemp;

   struct Arrhenius { __Arrhenius };

   /** Public Constructor */
   Arrhenius* Arrhenius_New(
      Name              name,
      AbstractContext*  context,
      int               temperature_id,
      double            eta0,
      double            activationEnergy,
      double            activationVolume,
      double            referenceTemp);

   /** Private Constructor: This will accept all the virtual functions for this class as arguments. */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define ARRHENIUS_DEFARGS \
                RHEOLOGY_DEFARGS

	#define ARRHENIUS_PASSARGS \
                RHEOLOGY_PASSARGS

   Arrhenius* _Arrhenius_New(  ARRHENIUS_DEFARGS  );

   /* 'Stg_Component' implementations */
   void* _Arrhenius_DefaultNew( Name name ) ;
   void _Arrhenius_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data );
   void _Arrhenius_Destroy( void* _self, void* data );
   void _Arrhenius_Build( void* _self, void* data );
   void _Arrhenius_Initialise( void* _self, void* data );
   void _Arrhenius_Init( Arrhenius* self, int temperature_id, double eta0, double activationEnergy, double activationVolume, double referenceTemp );

   void _Arrhenius_ModifyConstitutiveMatrix(
      void*                                              rheology,
      ConstitutiveMatrix*                                constitutiveMatrix,
      MaterialPointsSwarm*                               swarm,
      Element_LocalIndex                                 lElement_I,
      MaterialPoint*                                     materialPoint,
      Coord                                              xi );

#endif

