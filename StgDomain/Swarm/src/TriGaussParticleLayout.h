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
**	Instantiates the ParticleLayout abstract class to a particle layout at points designed for optimal gaussian integration.
**
** Assumptions:
**	Cell is a right-angled cuboid.
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgDomain_Swarm_TriGaussParticleLayout_h__
#define __StgDomain_Swarm_TriGaussParticleLayout_h__

	
	/* Textual name of this class */
	extern const Type TriGaussParticleLayout_Type;
	
	#define __TriGaussParticleLayout \
		__PerCellParticleLayout \
		\
		unsigned int dim;              /**< The number of dimensions to use */ \
	   unsigned int particlesPerCell; /**< The number of points to use per cell */

	struct TriGaussParticleLayout { __TriGaussParticleLayout };
	
	/* Create a new TriGaussParticleLayout and initialise */
   TriGaussParticleLayout* TriGaussParticleLayout_New( 
      Name name, 
      AbstractContext* context,
      CoordSystem      coordSystem,
      Bool             weightsInitialisedAtStartup,
      unsigned int dim, unsigned int particlesPerCell );
	
	/* Creation implementation / Virtual constructor */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define TRIGAUSSPARTICLELAYOUT_DEFARGS \
                PERCELLPARTICLELAYOUT_DEFARGS

	#define TRIGAUSSPARTICLELAYOUT_PASSARGS \
                PERCELLPARTICLELAYOUT_PASSARGS

   TriGaussParticleLayout* _TriGaussParticleLayout_New(  TRIGAUSSPARTICLELAYOUT_DEFARGS  );

	/* Initialise implementation */
	void _TriGaussParticleLayout_Init( 
			TriGaussParticleLayout* self, 
			unsigned int            dim, 
			unsigned int            particlesPerCell );
	
	/* Stg_Class_Delete implementation */
	void _TriGaussParticleLayout_Delete( void* triGaussParticleLayout );
	
	/* Print implementation */
	void _TriGaussParticleLayout_Print( void* triGaussParticleLayout, Stream* stream );
	
	/* Copy */
	#define TriGaussParticleLayout_Copy( self ) \
		(TriGaussParticleLayout*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define TriGaussParticleLayout_DeepCopy( self ) \
		(TriGaussParticleLayout*)Stg_Class_Copy( self, NULL, True, NULL, NULL )
	void* _TriGaussParticleLayout_Copy( void* triGaussParticleLayout, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );

	void* _TriGaussParticleLayout_DefaultNew( Name name );
	void  _TriGaussParticleLayout_AssignFromXML( void* triGaussParticleLayout, Stg_ComponentFactory* cf, void* data );
	void  _TriGaussParticleLayout_Build( void* triGaussParticleLayout, void* data );
	void  _TriGaussParticleLayout_Initialise( void* triGaussParticleLayout, void* data );
	void  _TriGaussParticleLayout_Execute( void* triGaussParticleLayout, void* data );
	void  _TriGaussParticleLayout_Destroy( void* triGaussParticleLayout, void* data );
	
	Particle_InCellIndex _TriGaussParticleLayout_InitialCount( 
			void* triGaussParticleLayout, 
			void* celllayout, 
			Cell_Index cell_I );
	
	void _TriGaussParticleLayout_InitialiseParticlesOfCell( void* triGaussParticleLayout, void* swarm, Cell_Index cell_I );
	
#endif /* __StgDomain_Swarm_TriGaussParticleLayout_h__ */

