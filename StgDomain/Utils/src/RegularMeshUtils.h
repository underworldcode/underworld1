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
**	Utilities to get the sides of a regular mesh etc.
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgDomain_Utils_RegularMeshUtils_h__
#define __StgDomain_Utils_RegularMeshUtils_h__

	extern Index RegularMeshUtils_ascendingIJK_ToHughesNodeNumberMap[8];

	/*--------------------------------------------------------------------------------------------------------------------------
	** Mapping functions
	*/

	void RegularMeshUtils_Node_1DTo3D( void* mesh, unsigned global, unsigned* inds );
	unsigned RegularMeshUtils_Node_3DTo1D( void* mesh, unsigned* inds );

	void RegularMeshUtils_Element_1DTo3D( void* mesh, unsigned global, unsigned* inds );
	unsigned RegularMeshUtils_Element_3DTo1D( void* mesh, unsigned* inds );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Set functions
	*/
   /** Creates a set based on provided string */
   IndexSet* RegularMeshUtils_CreateBoundaryFeatureSet( void* _mesh, MeshTopology_Dim TOPO, char* whichSet );
   /** Creates a new set, based on domain node indices, of all boundary nodes */
   IndexSet* RegularMeshUtils_CreateGlobalSet( void* _mesh );

	/** Creates a new set of problem domain boundary element, on this processor's domain space **/
	IndexSet* RegularMeshUtils_CreateBoundaryElementDomainSet( void* _mesh );

	/** Create a new set, based on node indices, of nodes on the MaxJ side of the global regular mesh */
	IndexSet* RegularMeshUtils_CreateGlobalMaxJSet( void* _mesh );
	
	/** Create a new set, based on node indices, of nodes on the MinJ side of the global regular mesh */
	IndexSet* RegularMeshUtils_CreateGlobalMinJSet( void* _mesh );
	
	/** Create a new set, based on node indices, of nodes on the MinI side of the global regular mesh */
	IndexSet* RegularMeshUtils_CreateGlobalMinISet( void* _mesh );
	
	/** Create a new set, based on node indices, of nodes on the MaxI side of the global regular mesh */
	IndexSet* RegularMeshUtils_CreateGlobalMaxISet( void* _mesh );
	
	/** Create a new set, based on node indices, of nodes on the MaxK side of the global regular mesh */
	IndexSet* RegularMeshUtils_CreateGlobalMaxKSet( void* _mesh );
	
	/** Create a new set, based on node indices, of nodes on the MinK side of the global regular mesh */
	IndexSet* RegularMeshUtils_CreateGlobalMinKSet( void* _mesh );


	/** Create a new set, based on node indices, of nodes on the MaxJ side without the corners of the global regular mesh */
	IndexSet* RegularMeshUtils_CreateGlobalInnerMaxJSet( void* _mesh );
	
	/** Create a new set, based on node indices, of nodes on the MinJ side without the corners of the global regular mesh */
	IndexSet* RegularMeshUtils_CreateGlobalInnerMinJSet( void* _mesh );
	
	/** Create a new set, based on node indices, of nodes on the MinI side without the corners of the global regular mesh */
	IndexSet* RegularMeshUtils_CreateGlobalInnerMinISet( void* _mesh );
	
	/** Create a new set, based on node indices, of nodes on the MaxI side without the corners of the global regular mesh */
	IndexSet* RegularMeshUtils_CreateGlobalInnerMaxISet( void* _mesh );
	
	/** Create a new set, based on node indices, of nodes on the MaxK side without the corners of the global regular mesh */
	IndexSet* RegularMeshUtils_CreateGlobalInnerMaxKSet( void* _mesh );
	
	/** Create a new set, based on node indices, of nodes on the MinK side without the corners of the global regular mesh */
	IndexSet* RegularMeshUtils_CreateGlobalInnerMinKSet( void* _mesh );
	

	/** Create a new set, based on node indices, of the node on the MinIMinJMaxK corner */
	IndexSet* RegularMeshUtils_CreateGlobalMinIMinJMaxKSet( void* _mesh );

	/** Create a new set, based on node indices, of the node on the MaxIMinJMaxK corner */
	IndexSet* RegularMeshUtils_CreateGlobalMaxIMinJMaxKSet( void* _mesh );

	/** Create a new set, based on node indices, of the node on the MinIMaxJMaxK corner */
	IndexSet* RegularMeshUtils_CreateGlobalMinIMaxJMaxKSet( void* _mesh );

	/** Create a new set, based on node indices, of the node on the MaxIMaxJMaxK corner */
	IndexSet* RegularMeshUtils_CreateGlobalMaxIMaxJMaxKSet( void* _mesh );

	/** Create a new set, based on node indices, of the node on the MinIMinJMinK corner */
	IndexSet* RegularMeshUtils_CreateGlobalMinIMinJMinKSet( void* _mesh );

	/** Create a new set, based on node indices, of the node on the MaxIMinJMinK corner */
	IndexSet* RegularMeshUtils_CreateGlobalMaxIMinJMinKSet( void* _mesh );

	/** Create a new set, based on node indices, of the node on the MinIMaxJMinK corner */
	IndexSet* RegularMeshUtils_CreateGlobalMinIMaxJMinKSet( void* _mesh );

	/** Create a new set, based on node indices, of the node on the MaxIMaxJMinK corner */
	IndexSet* RegularMeshUtils_CreateGlobalMaxIMaxJMinKSet( void* _mesh );


	/** Create a new set, based on node indices, of nodes on the MaxJ side of the local regular mesh */
	IndexSet* RegularMeshUtils_CreateLocalInGlobalMaxJSet( void* _mesh );
	
	/** Create a new set, based on node indices, of nodes on the MinJ side of the local regular mesh */
	IndexSet* RegularMeshUtils_CreateLocalInGlobalMinJSet( void* _mesh );
	
	/** Create a new set, based on node indices, of nodes on the MinI side of the local regular mesh */
	IndexSet* RegularMeshUtils_CreateLocalInGlobalMinISet( void* _mesh );
	
	/** Create a new set, based on node indices, of nodes on the MaxI side of the local regular mesh */
	IndexSet* RegularMeshUtils_CreateLocalInGlobalMaxISet( void* _mesh );
	
	/** Create a new set, based on node indices, of nodes on the MaxK side of the local regular mesh */
	IndexSet* RegularMeshUtils_CreateLocalInGlobalMaxKSet( void* _mesh );
	
	/** Create a new set, based on node indices, of nodes on the MinK side of the local regular mesh */
	IndexSet* RegularMeshUtils_CreateLocalInGlobalMinKSet( void* _mesh );

	/** A utility function for error checking and data retrieval that is used by other functions in this file **/
	void RegularMeshUtils_ErrorCheckAndGetDetails( Mesh* mesh, MeshTopology_Dim INPUT, unsigned *nDomainSize, Grid** grid );

IndexSet* RegularMeshUtils_CreateContactMaxJSet( void* _mesh, int lowDepth, int uppDepth );
IndexSet* RegularMeshUtils_CreateContactMinJSet( void* _mesh, int lowDepth, int uppDepth, int inDepth );
IndexSet* RegularMeshUtils_CreateContactMinISet( void* _mesh, int lowDepth, int uppDepth );
IndexSet* RegularMeshUtils_CreateContactMaxISet( void* _mesh, int lowDepth, int uppDepth, int inDepth );

	Node_DomainIndex RegularMeshUtils_GetDiagOppositeAcrossElementNodeIndex( void* _mesh, 
										 Element_DomainIndex refElement_dI, 
										 Node_DomainIndex refNode_dI );

IndexSet* RegularMeshUtils_CreateGlobalMaxIMinJSet( void* _mesh );
IndexSet* RegularMeshUtils_CreateGlobalMinIMinJSet( void* _mesh );

#endif /* __StgDomain_Utils_RegularMeshUtils_h__ */
