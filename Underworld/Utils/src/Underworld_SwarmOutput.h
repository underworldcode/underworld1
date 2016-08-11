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
*%  Louis.Moresi - Louis.Moresi@monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
** 
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#ifndef __Underworld_Utils_Underworld_SwarmOutput_h__
#define __Underworld_Utils_Underworld_SwarmOutput_h__

   /* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
   extern const Type Underworld_SwarmOutput_Type;

   typedef enum _USO_MODE { USO_PERPARTICLE, USO_PERTIMESTEP } USO_MODE;

   /* Underworld_SwarmOutput information */
   #define __Underworld_SwarmOutput \
      /* Macro defining parent goes here - This means you can cast this class as its parent */ \
      __Stg_Component \
      UnderworldContext*        context; \
      /* Virtual Info */\
      Swarm*                    swarm; \
      FeVariable**              feVariableList; \
      SwarmVariable**           svList; \
      Index                     svNum; \
      Index                     feVarNum; \
      unsigned int              usoOutputEvery;\
      USO_MODE                  mode; \
      unsigned int              pidHandle; \
      Stream*                   infoStream; \
      Stream*                   errorStream; \

      struct Underworld_SwarmOutput { __Underworld_SwarmOutput };
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define UNDERWORLD_SWARMOUTPUT_DEFARGS \
                STG_COMPONENT_DEFARGS

	#define UNDERWORLD_SWARMOUTPUT_PASSARGS \
                STG_COMPONENT_PASSARGS

	Underworld_SwarmOutput* _Underworld_SwarmOutput_New(  UNDERWORLD_SWARMOUTPUT_DEFARGS  );

	/* Stg_Class_Delete Underworld_SwarmOutput implementation */
	void _Underworld_SwarmOutput_Delete( void* swarmOutput );
	void _Underworld_SwarmOutput_Print( void* swarmOutput, Stream* stream );
	
	void* _Underworld_SwarmOutput_DefaultNew( Name name ) ;
   void _Underworld_SwarmOutput_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) ;
   
	void _Underworld_SwarmOutput_Build( void* swarmOutput, void* data ) ;
	void _Underworld_SwarmOutput_Initialise( void* swarmOutput, void* data ) ;
	void _Underworld_SwarmOutput_Destroy( void* swarmOutput, void* data ) ;
	
		
	/*---------------------------------------------------------------------------------------------------------------------
	** Functions for ALL_SWARM mode
	*/
	
	void _Underworld_SwarmOutput_OutputSwarmPos( void* swarmOutput, void* data );
	void _Underworld_SwarmOutput_OutputAllSwarm( void* swarmOutput, void* data );
	void _Underworld_SwarmOutput_GetFeVariableValues(Underworld_SwarmOutput* uwSwarmOutput, FeVariable* feVariable, Swarm* swarm, FILE* outputFile );
	void _Underworld_SwarmOutput_PrintStandardFormat( GlobalParticle* particle, PICelleratorContext* context, double* result, unsigned fieldComponentCount, FILE* outputFile  );
	/*---------------------------------------------------------------------------------------------------------------------
	** Functions for PARTICLE_TRACE mode
	*/
   void _Underworld_SwarmOutput_OutputParticleTrace( void* uwSwarmOutput, void* data );
   void _Underworld_SwarmOutput_ForEachParticlePrintFeVars( Underworld_SwarmOutput* self, PICelleratorContext* context, unsigned int p_i, FILE* outputFile );
   void _Underworld_SwarmOutput_GetSVValues(Underworld_SwarmOutput* uwSwarmOutput, SwarmVariable* sv, Swarm* swarm, FILE* outputFile);
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Public functions
	*/
	
#endif 

