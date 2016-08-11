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
**	Abstract class faciliting how particles of a cell are arranged. 
**	This class is for layouts that define the initial particle positions
**	relation to their elements, rather than globally.
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgDomain_Swarm_PerCellParticleLayout_h__
#define __StgDomain_Swarm_PerCellParticleLayout_h__
	
	/** @see PerCellParticleLayout_InitialCount */
	typedef Particle_InCellIndex (PerCellParticleLayout_InitialCountFunction) ( 
			void* particleLayout, 
			void* cellLayout, 
			Cell_Index cell_I );

	/** @see PerCellParticleLayout_InitialiseParticlesOfCell */
	typedef void (PerCellParticleLayout_InitialiseParticlesOfCellFunction) ( 
			void* particleLayout, 
			void* swarm, 
			Cell_Index cell_I );
	
	/* Textual name of this class */
	extern const Type PerCellParticleLayout_Type;
	
	/* ParticleLayout information */
	#define __PerCellParticleLayout \
		/* General info */ \
		__ParticleLayout \
		\
		/* virtual information */ \
		PerCellParticleLayout_InitialCountFunction*			_initialCount; \
		PerCellParticleLayout_InitialiseParticlesOfCellFunction*	_initialiseParticlesOfCell;

	struct PerCellParticleLayout { __PerCellParticleLayout };
	
	/* No "_New" and "_Init" as this is an abstract class */
	
	/* Creation implementation / Virtual constructor */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PERCELLPARTICLELAYOUT_DEFARGS \
                PARTICLELAYOUT_DEFARGS, \
                PerCellParticleLayout_InitialCountFunction*                            _initialCount, \
                PerCellParticleLayout_InitialiseParticlesOfCellFunction*  _initialiseParticlesOfCell

	#define PERCELLPARTICLELAYOUT_PASSARGS \
                PARTICLELAYOUT_PASSARGS, \
	        _initialCount,              \
	        _initialiseParticlesOfCell

   PerCellParticleLayout* _PerCellParticleLayout_New(  PERCELLPARTICLELAYOUT_DEFARGS  );
	
	/* Initialise implementation */
   void _PerCellParticleLayout_Init(
		void*                  particleLayout );
	
	/* Stg_Class_Delete a PerCellParticleLayout construct */
	void _PerCellParticleLayout_Delete( void* particleLayout );
	
	/* Print a PerCellParticleLayout construct */
	void _PerCellParticleLayout_Print( void* particleLayout, Stream* stream );
	
	/* Copy */
	#define PerCellParticleLayout_Copy( self ) \
		(PerCellParticleLayout*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define PerCellParticleLayout_DeepCopy( self ) \
		(PerCellParticleLayout*)Stg_Class_Copy( self, NULL, True, NULL, NULL )
	
	void* _PerCellParticleLayout_Copy( void* particleLayout, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
   void _PerCellParticleLayout_Destroy( void* particleLayout, void* data );
   void _PerCellParticleLayout_AssignFromXML( void* particleLayout, Stg_ComponentFactory *cf, void* data );
	
	void _PerCellParticleLayout_SetInitialCounts( void* particleLayout, void* _swarm );
	void _PerCellParticleLayout_InitialiseParticles( void* particleLayout, void* _swarm );

	/** Initialise all particles in a given cell */
	void PerCellParticleLayout_InitialiseParticlesOfCell( void* particleLayout, void* swarm, Cell_Index cell_I );

	/** Return the initial number of particles in cell */
	Particle_InCellIndex PerCellParticleLayout_InitialCount( void* particleLayout, void* _swarm, Cell_Index cell_I );
	
#endif /* __StgDomain_Swarm_PerCellParticleLayout_h__ */

