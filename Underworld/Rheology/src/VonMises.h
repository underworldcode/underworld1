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


#ifndef __Underworld_Rheology_VonMises_h__
#define __Underworld_Rheology_VonMises_h__

   /*
    * Textual name of this class - 
    * This is a global pointer which is used for times when you need to 
    * refer to class and not a particular instance of a class. 
    */
   extern const Type VonMises_Type;
   
   /*
    * Rheology class contents -
    * this is defined as a macro so that sub-classes of this class can use this
    * macro at the start of the definition of their struct.
    */
   #define __VonMises \
      /* Macro defining parent goes here -
         This means you can cast this class as its parent */ \
      __YieldRheology \
      /* Virtual functions go here */ \
      /* Material Parameters */\
      int             strainRateTag;                        \
      double          cohesion;                             \
      double          cohesionAfterSoftening;               \
      Bool            strainRateSoftening;   \
      SymmetricTensor stress; \
      double          stressInv;

   struct VonMises { __VonMises };

   /* Public Constructor */
   VonMises* VonMises_New(
      Name                  name,
      AbstractContext*      context,
      StrainWeakening*      strainWeakening, 
      MaterialPointsSwarm*  materialPointsSwarm, 
      double                minVisc, 
      int                   strainRateTag,
      double                cohesion,
      double                cohesionAfterSoftening,
      Bool                  strainRateSoftening );
   
   /** Private Constructor: This will accept all the virtual functions for this class as arguments. */
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define VONMISES_DEFARGS \
      YIELDRHEOLOGY_DEFARGS

   #define VONMISES_PASSARGS \
      YIELDRHEOLOGY_PASSARGS

   VonMises* _VonMises_New( VONMISES_DEFARGS );
   
   /* 'Stg_Component' implementations */
   void* _VonMises_DefaultNew( Name name ) ;

   void _VonMises_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data );

   void _VonMises_Init(
      VonMises*      self,
      int strainRateTag,
      double         cohesion,
      double         cohesionAfterSoftening,
      Bool           strainRateSoftening );

   void _VonMises_Destroy( void* rheology, void* data );

   void _VonMises_Build( void* rheology, void* data );

   void _VonMises_Initialise( void* rheology, void* data );

   /* 'YieldRheology' implementations */
   double _VonMises_GetYieldCriterion( 
      void*                vonMises,
      ConstitutiveMatrix*  constitutiveMatrix,
      MaterialPointsSwarm* materialPointsSwarm,
      Element_LocalIndex   lElement_I,
      MaterialPoint*       materialPoint,
      Coord                xi );
   
   double _VonMises_GetYieldIndicator( 
      void*                vonMises,
      ConstitutiveMatrix*  constitutiveMatrix,
      MaterialPointsSwarm* materialPointsSwarm,
      Element_LocalIndex   lElement_I,
      MaterialPoint*       materialPoint,
      Coord                xi );

   void _VonMises_HasYielded( 
      void*                vonMises,
      ConstitutiveMatrix*  constitutiveMatrix,
      MaterialPointsSwarm* materialPointsSwarm,
      Element_LocalIndex   lElement_I,
      MaterialPoint*       materialPoint,
      double               yieldCriterion,
      double               yieldIndicator );

#endif

