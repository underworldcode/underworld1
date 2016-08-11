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

#ifndef __Geothermal_IrregularTriGenerator_h__
#define __Geothermal_IrregularTriGenerator_h__

	/** Textual name of this class */
	extern const Type IrregularTriGenerator_Type;

	/** Virtual function types */
	typedef void (IrregularTriGenerator_SetTopologyParamsFunc)( void* meshGenerator, 
								 unsigned maxDecompDims, unsigned* minDecomp, unsigned* maxDecomp );
	typedef void (IrregularTriGenerator_GenElementTypesFunc)( void* meshGenerator, Mesh* mesh );

	/** IrregularTriGenerator class contents */
	#define __IrregularTriGenerator							\
		/* General info */								\
		__MeshGenerator									\
												\
		/* Virtual info */								\
		IrregularTriGenerator_SetTopologyParamsFunc*	setTopologyParamsFunc;		\
		IrregularTriGenerator_GenElementTypesFunc*	genElementTypesFunc;		\
												\
		/* IrregularTriGenerator info */						\
		Comm*		comm;								\
		unsigned	maxDecompDims;							\
		unsigned*	minDecomp;							\
		unsigned*	maxDecomp;							\
		unsigned	shadowDepth;							\
                int             contactDepth[3][2];                     			\
                double          contactGeom[3];							\
		char*		filename;							\
		IArray*		incVerts;							\
		IArray*		incEls;								\
		/* for the parallel implementation */ \
		FeMesh*		referenceMesh;							\

	struct IrregularTriGenerator { __IrregularTriGenerator };

	/*--------------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define IRREGULARTRIGENERATOR_DEFARGS \
                MESHGENERATOR_DEFARGS, \
                IrregularTriGenerator_SetTopologyParamsFunc*  setTopologyParamsFunc, \
                IrregularTriGenerator_GenElementTypesFunc*      genElementTypesFunc

	#define IRREGULARTRIGENERATOR_PASSARGS \
                MESHGENERATOR_PASSARGS, \
	        setTopologyParamsFunc, \
	        genElementTypesFunc  

	IrregularTriGenerator* IrregularTriGenerator_New( Name name );
	IrregularTriGenerator* _IrregularTriGenerator_New(  IRREGULARTRIGENERATOR_DEFARGS  );
	void _IrregularTriGenerator_Init( IrregularTriGenerator* self );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Virtual functions
	*/

	void _IrregularTriGenerator_Delete( void* meshGenerator );
	void _IrregularTriGenerator_Print( void* meshGenerator, Stream* stream );
	void _IrregularTriGenerator_AssignFromXML( void* meshGenerator, Stg_ComponentFactory* cf, void* data );
	void _IrregularTriGenerator_Build( void* meshGenerator, void* data );
	void _IrregularTriGenerator_Initialise( void* meshGenerator, void* data );
	void _IrregularTriGenerator_Execute( void* meshGenerator, void* data );
	void _IrregularTriGenerator_Destroy( void* meshGenerator, void* data );

	void IrregularTriGenerator_SetDimSize( void* meshGenerator, unsigned nDims );
	void IrregularTriGenerator_Generate( void* meshGenerator, void* mesh, void* data );
	void _IrregularTriGenerator_SetTopologyParams( void* meshGenerator,
						    unsigned maxDecompDims, unsigned* minDecomp, unsigned* maxDecomp );
	void _IrregularTriGenerator_GenElementTypes( void* meshGenerator, Mesh* mesh );

	#define IrregularTriGenerator_SetTopologyParams( self, maxDecompDims, minDecomp, maxDecomp )	\
		VirtualCall( self, setTopologyParamsFunc, self, maxDecompDims, minDecomp, maxDecomp )
	#define IrregularTriGenerator_GenElementTypes( self, mesh )	\
		VirtualCall( self, genElementTypesFunc, self, mesh )

	/*--------------------------------------------------------------------------------------------------------------------------
	** Public functions
	*/

	void IrregularTriGenerator_SetShadowDepth( void* meshGenerator, unsigned depth );
	double IrregularTriGenerator_InterpolateWithinElement( void* generator, FeVariable* feVariable, unsigned element, double* gCoord );
	double IrregularTriGenerator_InterpolateValueAt( void* generator, FeVariable* feVariable, double* gCoord );
	Bool IrregularTriGenerator_ElementHasPoint( void* generator, Mesh* mesh, unsigned element, double* point );
	Bool IrregularTriGenerator_SearchElements( void* generator, Mesh* mesh, double* point, unsigned* element );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/

	void IrregularTriGenerator_GenTopoAndGeom( IrregularTriGenerator* self, Mesh* mesh, IGraph* topo );
	void IrregularTriGenerator_GenTopoAndGeom_Parallel( IrregularTriGenerator* self, Mesh* mesh, IGraph* topo );
	void IrregularTriGenerator_GenBndVerts( IrregularTriGenerator* self, IGraph* topo );
	void IrregularTriGenerator_CompleteVertexNeighbours( IrregularTriGenerator* self, IGraph* topo );
	void IrregularTriGenerator_Destruct( IrregularTriGenerator* self );
	void IrregularTriGenerator_DestructTopology( IrregularTriGenerator* self );
	void IrregularTriGenerator_DestructGeometry( IrregularTriGenerator* self );

#endif /* __Geothermal_IrregularTriGenerator_h__ */

