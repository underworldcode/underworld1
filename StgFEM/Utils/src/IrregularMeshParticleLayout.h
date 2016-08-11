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
**	Instantiates the ParticleLayout abstract class to a spaceFillerly distributed particle layout.
**      Uses SobolGenerator by default
**
** Assumptions:
**	Cell is a right-angled cuboid.
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __Geothermal_Base_IrregularMeshParticleLayout_h__
#define __Geothermal_Base_IrregularMeshParticleLayout_h__
	

	/* Textual name of this class */
	extern const Type IrregularMeshParticleLayout_Type;
	
	/* IrregularMeshParticleLayout information */
	#define __IrregularMeshParticleLayout \
		__PerCellParticleLayout \
		\
		Dimension_Index         dim;                      \
		SobolGenerator*         sobolGenerator[3];        \
		unsigned		cellParticleCount;	  \
		FeMesh*			feMesh;			  \

	struct IrregularMeshParticleLayout { __IrregularMeshParticleLayout };
	
	/* Create a new IrregularMeshParticleLayout and initialise */
	IrregularMeshParticleLayout* IrregularMeshParticleLayout_New( Name name, Dimension_Index dim, unsigned cellParticleCount );
	
	/* Creation implementation / Virtual constructor */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define IRREGULARMESHPARTICLELAYOUT_DEFARGS \
                PERCELLPARTICLELAYOUT_DEFARGS, \
                Dimension_Index                dim, \
                unsigned         cellParticleCount

	#define IRREGULARMESHPARTICLELAYOUT_PASSARGS \
                PERCELLPARTICLELAYOUT_PASSARGS, \
	        dim,               \
	        cellParticleCount

	IrregularMeshParticleLayout* _IrregularMeshParticleLayout_New(  IRREGULARMESHPARTICLELAYOUT_DEFARGS  );
	
   void _IrregularMeshParticleLayout_Init( void* perCellLayout, FeMesh* feMesh, Dimension_Index dim, unsigned cellParticleCount );
	
	/* 'Stg_Class' Stuff */
	void _IrregularMeshParticleLayout_Delete( void* perCellLayout );
	void _IrregularMeshParticleLayout_Print( void* perCellLayout, Stream* stream );
	#define IrregularMeshParticleLayout_Copy( self ) \
		(IrregularMeshParticleLayout*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define IrregularMeshParticleLayout_DeepCopy( self ) \
		(IrregularMeshParticleLayout*)Stg_Class_Copy( self, NULL, True, NULL, NULL )
	void* _IrregularMeshParticleLayout_Copy( void* perCellLayout, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	
	/* 'Stg_Component' Stuff */
	void* _IrregularMeshParticleLayout_DefaultNew( Name name ) ;
	void _IrregularMeshParticleLayout_AssignFromXML( void* perCellLayout, Stg_ComponentFactory *cf, void* data );
	void _IrregularMeshParticleLayout_Build( void* perCellLayout, void* data );
	void _IrregularMeshParticleLayout_Initialise( void* perCellLayout, void* data );
	void _IrregularMeshParticleLayout_Execute( void* perCellLayout, void* data );
	void _IrregularMeshParticleLayout_Destroy( void* perCellLayout, void* data );
	
	Particle_InCellIndex _IrregularMeshParticleLayout_InitialCount( void* perCellLayout, void* cellLayout, Cell_Index cell_I );

	/* Initialises the coordinates of a cell's particle */
	void _IrregularMeshParticleLayout_InitialiseParticles( void* perCellLayout, void* swarm );
	//void _IrregularMeshParticleLayout_InitialiseParticle( void* perCellLayout, void* _swarm, unsigned cellIndex, void* particle );
	void _IrregularMeshParticleLayout_InitialiseParticlesOfCell( void* meshParticleLayout, void* _swarm, Cell_Index cell_I );

#endif /* __Geothermal_Base_IrregularMeshParticleLayout_h__ */

