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

#ifndef __Geothermal_Base_IrregularMeshGaussLayout_h__
#define __Geothermal_Base_IrregularMeshGaussLayout_h__
	

	/* Textual name of this class */
	extern const Type IrregularMeshGaussLayout_Type;
	
	/* IrregularMeshGaussLayout information */
	#define __IrregularMeshGaussLayout \
		__GaussParticleLayout \
		\
		double* abscissa; \
		FeMesh* feMesh; \

	struct IrregularMeshGaussLayout { __IrregularMeshGaussLayout };
	
	/* Create a new IrregularMeshGaussLayout and initialise */
	IrregularMeshGaussLayout* IrregularMeshGaussLayout_New( Name name, Dimension_Index dim, Particle_InCellIndex* particlesPerDim, FeMesh* feMesh ) ;
	
	/* Creation implementation / Virtual constructor */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define IRREGULARMESHGAUSSLAYOUT_DEFARGS \
                GAUSSPARTICLELAYOUT_DEFARGS

	#define IRREGULARMESHGAUSSLAYOUT_PASSARGS \
                GAUSSPARTICLELAYOUT_PASSARGS

	IrregularMeshGaussLayout* _IrregularMeshGaussLayout_New(  IRREGULARMESHGAUSSLAYOUT_DEFARGS  );
		
	/* Stg_Class_Delete implementation */
	void _IrregularMeshGaussLayout_Delete( void* gaussParticleLayout );
	
	/* Print implementation */
	void _IrregularMeshGaussLayout_Print( void* gaussParticleLayout, Stream* stream );
	
	/* Copy */
	#define IrregularMeshGaussLayout_Copy( self ) \
		(IrregularMeshGaussLayout*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define IrregularMeshGaussLayout_DeepCopy( self ) \
		(IrregularMeshGaussLayout*)Stg_Class_Copy( self, NULL, True, NULL, NULL )
	
	void* _IrregularMeshGaussLayout_Copy( void* gaussParticleLayout, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	
	void* _IrregularMeshGaussLayout_DefaultNew( Name name );
	void  _IrregularMeshGaussLayout_AssignFromXML( void* gaussParticleLayout, Stg_ComponentFactory* cf, void* data );
	void  _IrregularMeshGaussLayout_Build( void* gaussParticleLayout, void* data );
	void  _IrregularMeshGaussLayout_Initialise( void* gaussParticleLayout, void* data );
	void  _IrregularMeshGaussLayout_Execute( void* gaussParticleLayout, void* data );
	void  _IrregularMeshGaussLayout_Destroy( void* gaussParticleLayout, void* data );
	
	void _IrregularMeshGaussLayout_InitialiseParticlesOfCell( void* gaussParticleLayout, void* swarm, Cell_Index cell_I );

#endif /* __Geothermal_Base_IrregularMeshGaussLayout_h__ */

