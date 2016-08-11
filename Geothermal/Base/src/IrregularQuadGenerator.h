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
** Invariants:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __Geothermal_IrregularQuadGenerator_h__
#define __Geothermal_IrregularQuadGenerator_h__

	/** Textual name of this class */
	extern const Type IrregularQuadGenerator_Type;

	typedef int 	TriIndex[3];
	typedef int 	EdgeIndex[2];

	/** Virtual function types */
	typedef void (IrregularQuadGenerator_SetTopologyParamsFunc)( void* meshGenerator, 
								 unsigned maxDecompDims, unsigned* minDecomp, unsigned* maxDecomp );
	typedef void (IrregularQuadGenerator_GenElementTypesFunc)( void* meshGenerator, Mesh* mesh );

	/** IrregularQuadGenerator class contents */
	#define __IrregularQuadGenerator							\
		/* General info */								\
		__MeshGenerator									\
												\
		/* Virtual info */								\
		IrregularQuadGenerator_SetTopologyParamsFunc*	setTopologyParamsFunc;		\
		IrregularQuadGenerator_GenElementTypesFunc*	genElementTypesFunc;		\
												\
		/* IrregularQuadGenerator info */						\
		Comm*		comm;								\
		unsigned	maxDecompDims;							\
		unsigned*	minDecomp;							\
		unsigned*	maxDecomp;							\
		unsigned	shadowDepth;							\
                int             contactDepth[3][2];                     			\
                double          contactGeom[3];							\
		char*		filename;							\
		/* for the parallel implementation */ \
		FeMesh*		referenceMesh;							\

	struct IrregularQuadGenerator { __IrregularQuadGenerator };

	/*--------------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/



	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define IRREGULARQUADGENERATOR_DEFARGS \
                MESHGENERATOR_DEFARGS, \
                IrregularQuadGenerator_SetTopologyParamsFunc*  setTopologyParamsFunc, \
                IrregularQuadGenerator_GenElementTypesFunc*      genElementTypesFunc

	#define IRREGULARQUADGENERATOR_PASSARGS \
                MESHGENERATOR_PASSARGS, \
	        setTopologyParamsFunc, \
	        genElementTypesFunc  

	IrregularQuadGenerator* IrregularQuadGenerator_New( Name name );
	IrregularQuadGenerator* _IrregularQuadGenerator_New(  IRREGULARQUADGENERATOR_DEFARGS  );
	void _IrregularQuadGenerator_Init( IrregularQuadGenerator* self );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Virtual functions
	*/

	void _IrregularQuadGenerator_Delete( void* meshGenerator );
	void _IrregularQuadGenerator_Print( void* meshGenerator, Stream* stream );
	void _IrregularQuadGenerator_AssignFromXML( void* meshGenerator, Stg_ComponentFactory* cf, void* data );
	void _IrregularQuadGenerator_Build( void* meshGenerator, void* data );
	void _IrregularQuadGenerator_Initialise( void* meshGenerator, void* data );
	void _IrregularQuadGenerator_Execute( void* meshGenerator, void* data );
	void _IrregularQuadGenerator_Destroy( void* meshGenerator, void* data );

	void IrregularQuadGenerator_SetDimSize( void* meshGenerator, unsigned nDims );
	void IrregularQuadGenerator_Generate( void* meshGenerator, void* mesh, void* data );
	void _IrregularQuadGenerator_SetTopologyParams( void* meshGenerator,
						    unsigned maxDecompDims, unsigned* minDecomp, unsigned* maxDecomp );
	void _IrregularQuadGenerator_GenElementTypes( void* meshGenerator, Mesh* mesh );

	#define IrregularQuadGenerator_SetTopologyParams( self, maxDecompDims, minDecomp, maxDecomp )	\
		VirtualCall( self, setTopologyParamsFunc, self, maxDecompDims, minDecomp, maxDecomp )
	#define IrregularQuadGenerator_GenElementTypes( self, mesh )	\
		VirtualCall( self, genElementTypesFunc, self, mesh )

	/*--------------------------------------------------------------------------------------------------------------------------
	** Public functions
	*/

	void IrregularQuadGenerator_SetShadowDepth( void* meshGenerator, unsigned depth );

	double IQG_M_0( double coord[2] );
	double IQG_M_1( double coord[2] );
	double IQG_M_2( double coord[2] );
	double IrregularQuadGenerator_InterpFromLocalCoord_Tri( double phiAtNodes[3], double elCoord[2] );
	double** ReadNodeFile( char* file_prefix, int* num_nodes );
	TriIndex* ReadTriFile( char* file_prefix, int* num_tris );
	EdgeIndex* ReadEdgeFile( char* file_prefix, int* num_edges );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/

	void IrregularQuadGenerator_GenTopoAndGeom( IrregularQuadGenerator* self, Mesh* mesh, IGraph* topo );
	void IrregularQuadGenerator_GenTopoAndGeom_Parallel( IrregularQuadGenerator* self, Mesh* mesh, IGraph* topo );
	void IrregularQuadGenerator_GenBndVerts( IrregularQuadGenerator* self, IGraph* topo );
	void IrregularQuadGenerator_CompleteVertexNeighbours( IrregularQuadGenerator* self, IGraph* topo );
	void IrregularQuadGenerator_MapToDomain( IrregularQuadGenerator* self, Sync* sync, unsigned nIncEls, unsigned* incEls );
	void IrregularQuadGenerator_Destruct( IrregularQuadGenerator* self );
	void IrregularQuadGenerator_DestructTopology( IrregularQuadGenerator* self );
	void IrregularQuadGenerator_DestructGeometry( IrregularQuadGenerator* self );

#endif /* __Geothermal_IrregularQuadGenerator_h__ */

