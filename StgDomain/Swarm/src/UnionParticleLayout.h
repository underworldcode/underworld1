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

#ifndef __StgDomain_Swarm_UnionParticleLayout_h__
#define __StgDomain_Swarm_UnionParticleLayout_h__
	

	/* Textual name of this class */
	extern const Type UnionParticleLayout_Type;
	
	/* UnionParticleLayout information */
	#define __UnionParticleLayout \
		__GlobalParticleLayout \
		\
		GlobalParticleLayout**                           particleLayoutList;            \
		Index                                            particleLayoutCount;

	struct UnionParticleLayout { __UnionParticleLayout };
	
   /* Create a new UnionParticleLayout and initialise */
   UnionParticleLayout* UnionParticleLayout_New(
		Name             name, 
      AbstractContext* context, 
      CoordSystem      coordSystem,
      Bool             weightsInitialisedAtStartup,
      unsigned int     totalInitialParticles, 
      double           averageInitialParticlesPerCell,
		GlobalParticleLayout** particleLayoutList,
		Index                  particleLayoutCount );
	
   /* Creation implementation / Virtual constructor */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define UNIONPARTICLELAYOUT_DEFARGS \
                GLOBALPARTICLELAYOUT_DEFARGS, \
                GlobalParticleLayout**   particleLayoutList, \
                Index                   particleLayoutCount

	#define UNIONPARTICLELAYOUT_PASSARGS \
                GLOBALPARTICLELAYOUT_PASSARGS, \
	        particleLayoutList,  \
	        particleLayoutCount

   UnionParticleLayout* _UnionParticleLayout_New(  UNIONPARTICLELAYOUT_DEFARGS  );

	void _UnionParticleLayout_Init( 
		void*                                            unionParticleLayout, 
		GlobalParticleLayout**                           particleLayoutList,
		Index                                            particleLayoutCount );
	
	/* 'Stg_Class' Stuff */
	void _UnionParticleLayout_Delete( void* unionParticleLayout );
	void _UnionParticleLayout_Print( void* unionParticleLayout, Stream* stream );
	#define UnionParticleLayout_Copy( self ) \
		(UnionParticleLayout*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define UnionParticleLayout_DeepCopy( self ) \
		(UnionParticleLayout*)Stg_Class_Copy( self, NULL, True, NULL, NULL )
	void* _UnionParticleLayout_Copy( void* unionParticleLayout, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	
	/* 'Stg_Component' Stuff */
	void* _UnionParticleLayout_DefaultNew( Name name ) ;
	void _UnionParticleLayout_AssignFromXML( void* unionParticleLayout, Stg_ComponentFactory *cf, void* data );
	void _UnionParticleLayout_Build( void* unionParticleLayout, void* data );
	void _UnionParticleLayout_Initialise( void* unionParticleLayout, void* data );
	void _UnionParticleLayout_Execute( void* unionParticleLayout, void* data );
	void _UnionParticleLayout_Destroy( void* unionParticleLayout, void* data );
	
	void _UnionParticleLayout_SetInitialCounts( void* unionParticleLayout, void* _swarm );
	/* Initialises the coordinates of a cell's particle */
	void _UnionParticleLayout_InitialiseParticles( void* unionParticleLayout, void* swarm );
	void _UnionParticleLayout_InitialiseParticle( 
			void*             unionParticleLayout,
			void*             _swarm,
			Particle_Index    newParticle_I,
			void*             particle );


#endif /* __StgDomain_Swarm_UnionParticleLayout_h__ */

