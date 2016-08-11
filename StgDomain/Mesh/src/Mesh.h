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
**	External header file to this library.
**
** Assumptions:
**	None so far.
**
** Comments:
**	None so far.
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgDomain_Mesh_Mesh_h__
#define __StgDomain_Mesh_Mesh_h__
	
#include "types.h"
#include "shortcuts.h"

#include "Grid.h"
#include "Decomp.h"
#include "Sync.h"
#include "MeshTopology.h"
#include "IGraph.h"
/*
	#include "Decomposer.h"
	#include "DecompTransfer.h"
*/
#include "Mesh_ElementType.h"
#include "Mesh_HexType.h"
#include "Mesh_CentroidType.h"
#include "Mesh_Algorithms.h"
#include "Mesh_HexAlgorithms.h"
#include "Mesh_RegularAlgorithms.h"
#include "Mesh_CentroidAlgorithms.h"
#include "MeshClass.h"
#include "MeshGenerator.h"
#include "CartesianGenerator.h"
#include "MeshAdaptor.h"
#include "SurfaceAdaptor.h"
#include "GALESurfaceAdaptor.h"
#include "CompressionAdaptor.h"
#include "linearSpaceAdaptor.h"
#include "NodeBunching.h"
#include "MeshVariable.h"
#include "SpatialTree.h"
#include "Remesher.h"

#include "Init.h"
#include "Finalise.h"

#endif /* __StgDomain_Mesh_Mesh_h__ */
