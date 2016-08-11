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
**	Instantiates the CellLayout abstract class to a cell layout that has the same / is geometry as a mesh's elements.
**
** Assumptions:
**	Element layout (node interconnects) does not change. Element node coordinates do not change (for IsInCell and CellOf).
**	Element nodes are a subclass of Advectable (coord is the first variable in the structure).
**
** Comments:
**	Created with Snac in mind.
**	"Mesh*"  in the constuctor can be set to 0, but must then be set manually before the ensemble is created.
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgDomain_Swarm_CLLCellLayout_h__
#define __StgDomain_Swarm_CLLCellLayout_h__
	

	/* Textual name of this class */
	extern const Type CLLCellLayout_Type;
	
	/* CLLCellLayout information */
	#define __CLLCellLayout \
		/* General info */ \
      __ElementCellLayout \
		\
		/* Virtual info */ \
		\
      /* CLLCellLayout info */       \
      Mesh*			geometryMesh;      \
      double	   cellSize;          \
      unsigned    meshDim;           \
      Bool        parallelDecompose; \
      unsigned    expandFactor;

	struct CLLCellLayout { __CLLCellLayout };


	
	/* Create a new CLLCellLayout and initialise */
	void* _CLLCellLayout_DefaultNew( Name name );
	
   CLLCellLayout* CLLCellLayout_New( Name name, AbstractContext* context, void* mesh, double cellSize, unsigned meshDim, Bool parallelDecompose, unsigned expandFactor );
	
	/* Creation implementation / Virtual constructor */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define CLLCELLLAYOUT_DEFARGS \
                ELEMENTCELLLAYOUT_DEFARGS

	#define CLLCELLLAYOUT_PASSARGS \
                ELEMENTCELLLAYOUT_PASSARGS

	CLLCellLayout* _CLLCellLayout_New(  CLLCELLLAYOUT_DEFARGS  ); 

	/* Initialise implementation */
   void _CLLCellLayout_Init( CLLCellLayout* self, void* mesh, double cellSize, unsigned meshDim, Bool parallelDecompose, unsigned expandFactor );

	/* Copy */
	#define CLLCellLayout_Copy( self ) \
		(CLLCellLayout*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define CLLCellLayout_DeepCopy( self ) \
		(CLLCellLayout*)Stg_Class_Copy( self, NULL, True, NULL, NULL )
	
	void _CLLCellLayout_AssignFromXML( void *cLLCellLayout, Stg_ComponentFactory *cf, void* data );
	
	void _CLLCellLayout_Build( void *cLLCellLayout, void *data );
	
	void _CLLCellLayout_Initialise( void *cLLCellLayout, void *data );
	
	void _CLLCellLayout_Destroy( void *cLLCellLayout, void *data );
		
#endif /* __StgDomain_Swarm_CLLCellLayout_h__ */

