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

#ifndef __StgFEM_Discretisation_FeMesh_h__
#define __StgFEM_Discretisation_FeMesh_h__

	/** Textual name of this class */
	extern const Type FeMesh_Type;

	/** Virtual function types */

	/** Class contents */
	#define __FeMesh				\
		/* General info */			\
		__Mesh					\
							\
		/* Virtual info */			\
							\
		/* FeMesh info */			\
		char*			feElFamily;	\
		ElementType*		feElType;	\
		Bool		elementMesh;		\
                Bool useFeAlgorithms; \
		IArray*	inc; \
		IndexSet*           bndNodeSet;   	  /* IndexSet for mesh boundary nodes */ \
		IndexSet*           bndElementSet;	  /* IndexSet for mesh boundary elements */ \
		

	struct FeMesh { __FeMesh };

	/*--------------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/



	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define FEMESH_DEFARGS \
                MESH_DEFARGS

	#define FEMESH_PASSARGS \
                MESH_PASSARGS

	FeMesh* FeMesh_New( Name name, AbstractContext* context );
	FeMesh* _FeMesh_New(  FEMESH_DEFARGS  );
	void _FeMesh_Init( FeMesh* self, ElementType* elType, const char* family, Bool elementMesh );
   FeMesh* _FeMesh_DefaultNew( Name name );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Virtual functions
	*/

	void _FeMesh_Delete( void* feMesh );
	void _FeMesh_Print( void* feMesh, Stream* stream );
	void _FeMesh_AssignFromXML( void* feMesh, Stg_ComponentFactory* cf, void* data );
	void _FeMesh_Build( void* feMesh, void* data );
	void _FeMesh_Initialise( void* feMesh, void* data );
	void _FeMesh_Execute( void* feMesh, void* data );
	void _FeMesh_Destroy( void* feMesh, void* data );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Public functions
	*/

	void FeMesh_SetElementFamily( void* feMesh, const char* family );
	void FeMesh_SetElementType( void* feMesh, ElementType* elType );

	ElementType* FeMesh_GetElementType( void* feMesh, unsigned element );

	unsigned FeMesh_GetNodeLocalSize( void* feMesh );
	unsigned FeMesh_GetNodeRemoteSize( void* feMesh );
	unsigned FeMesh_GetNodeDomainSize( void* feMesh );
	unsigned FeMesh_GetNodeGlobalSize( void* feMesh );
	unsigned FeMesh_GetElementLocalSize( void* feMesh );
	unsigned FeMesh_GetElementDomainSize( void* feMesh );
	unsigned FeMesh_GetElementRemoteSize( void* feMesh );
	unsigned FeMesh_GetElementGlobalSize( void* feMesh );

	unsigned FeMesh_GetElementNodeSize( void* feMesh, unsigned element );
	unsigned FeMesh_GetNodeElementSize( void* feMesh, unsigned node );
	void FeMesh_GetElementNodes( void* feMesh, unsigned element, IArray* inc );
	void FeMesh_GetNodeElements( void* feMesh, unsigned node, IArray* inc );

	unsigned FeMesh_ElementDomainToGlobal( void* feMesh, unsigned domain );
	Bool FeMesh_ElementGlobalToDomain( void* feMesh, unsigned global, unsigned* domain );
	unsigned FeMesh_NodeDomainToGlobal( void* feMesh, unsigned domain );
	Bool FeMesh_NodeGlobalToDomain( void* feMesh, unsigned global, unsigned* domain );

	void FeMesh_CoordGlobalToLocal( void* feMesh, unsigned element, double* global, double* local );
	void FeMesh_CoordLocalToGlobal( void* feMesh, unsigned element, double* local, double* global );
	void FeMesh_EvalBasis( void* feMesh, unsigned element, double* localCoord, double* basis );
	void FeMesh_EvalLocalDerivs( void* feMesh, unsigned element, double* localCoord, double** derivs );
	void FeMesh_EvalGlobalDerivs( void* feMesh, unsigned element, double* localCoord, double** derivs, double* jacDet );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/

	void FeMesh_Destruct( FeMesh* self );

#endif /* __StgFEM_Discretisaton_FeMesh_h__ */

