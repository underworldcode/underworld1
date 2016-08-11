/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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
/** TODO

* generalise ascii reading to allow different deliminators
* more robust handling of black spaces / lines
* read in header from file
* swarmVariables have have a multilevel tree of parents.  currently only check one above for checkpointing. 
*
*/ 

#ifndef __Underworld_Utils_ASCIIParticleLayout_h__
#define __Underworld_Utils_ASCIIParticleLayout_h__
	

	/* Textual name of this class */
	extern const Type ASCIIParticleLayout_Type;
	
	/* ASCIIParticleLayout information */
	#define __ASCIIParticleLayout \
      __GlobalParticleLayout \
		Name                    filename;            \
		Dictionary_Entry_Value* columnList;          \
      Dictionary_Entry_Value* columnScalingList;   \
      Dictionary_Entry_Value* columnOffsetList;    \
		File*                   file;                \
		Stream*                 errorStream;         \
		unsigned*               swarmVarMapping;     \
		Index                   columnCount;         \
		double*                 rowData;             \
      double*                 columnScaling;       \
      double*                 columnOffset;        \
		char*                   delim;

	struct ASCIIParticleLayout { __ASCIIParticleLayout };
	
   /* Creation implementation / Virtual constructor */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define ASCIIPARTICLELAYOUT_DEFARGS        \
                GLOBALPARTICLELAYOUT_DEFARGS
                
	#define ASCIIPARTICLELAYOUT_PASSARGS        \
                GLOBALPARTICLELAYOUT_PASSARGS

   ASCIIParticleLayout* _ASCIIParticleLayout_New(  ASCIIPARTICLELAYOUT_DEFARGS  );
	
	/* 'Stg_Class' Stuff */
	void _ASCIIParticleLayout_Delete( void* particleLayout );
	void _ASCIIParticleLayout_Print( void* particleLayout, Stream* stream );
	#define ASCIIParticleLayout_Copy( self ) \
		(ASCIIParticleLayout*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define ASCIIParticleLayout_DeepCopy( self ) \
		(ASCIIParticleLayout*)Stg_Class_Copy( self, NULL, True, NULL, NULL )
	void* _ASCIIParticleLayout_Copy( void* particleLayout, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	
	/* 'Stg_Component' Stuff */
	void* _ASCIIParticleLayout_DefaultNew( Name name ) ;
	void _ASCIIParticleLayout_AssignFromXML( void* particleLayout, Stg_ComponentFactory *cf, void* data );
	void _ASCIIParticleLayout_Build( void* particleLayout, void* data );
	void _ASCIIParticleLayout_Initialise( void* particleLayout, void* data );
	void _ASCIIParticleLayout_Execute( void* particleLayout, void* data );
	void _ASCIIParticleLayout_Destroy( void* particleLayout, void* data );
	
	void _ASCIIParticleLayout_SetInitialCounts( void* particleLayout, void* _swarm ) ;
	void _ASCIIParticleLayout_InitialiseParticles( void* particleLayout, void* _swarm ) ;
	void _ASCIIParticleLayout_InitialiseParticle( void* particleLayout, void* swarm, Particle_Index newParticle_I, void* particle);
   void _ASCIIParticleLayout_MapColumnsToSwarmVariables(	void* particleLayout, void* _swarm );

#endif /* __Underworld_Utils_ASCIIParticleLayout_h__ */

