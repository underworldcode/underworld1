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
 ** $Id: RegRemesh.h 2225 1970-01-02 13:48:23Z LukeHodkinson $
 **
 **~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgDomain_Utils_RegRemesh_h__
#define __StgDomain_Utils_RegRemesh_h__

#include <mpi.h>
#include "PETScCompatibility.h"

/* Textual name of this class. */
extern const Type RegRemesh_Type;

/* Virtual function types. */

/* Class contents. */
#define __RegRemesh                             \
    /* General info */                          \
    __Remesher                                  \
                                                \
    /* Virtual info */                          \
                                                \
    /* RegRemesh info ... */                    \
    int surfaceIdx;

struct RegRemesh { __RegRemesh };

/*-----------------------------------------------------------------------------------------------------------------------------
** Constructors
*/

/* Create a RegRemesh */
RegRemesh* RegRemesh_New( Name name, AbstractContext* context, Mesh* mesh, int surfaceIdx );

/* Creation implementation */
	
#ifndef ZERO
#define ZERO 0
#endif

#define REGREMESH_DEFARGS             \
    REMESHER_DEFARGS

#define REGREMESH_PASSARGS            \
    REMESHER_PASSARGS

RegRemesh* _RegRemesh_New(  REGREMESH_DEFARGS  );

/* Initialisation implementation functions */
void _RegRemesh_Init( void* remesher, int surfaceIdx );

/*-----------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _RegRemesh_Delete( void* remesher );

void _RegRemesh_Print( void* remesher, Stream* stream );

RegRemesh* _RegRemesh_DefaultNew( Name name );

void _RegRemesh_AssignFromXML( void* remesher, Stg_ComponentFactory* cf, void* data );

void _RegRemesh_Build( void* remesher, void* data );

void _RegRemesh_Initialise( void* remesher, void* data );

void _RegRemesh_Execute( void* remesher, void* data );

void _RegRemesh_Destroy( void* remesher, void* data );


/*-----------------------------------------------------------------------------------------------------------------------------
** Public functions
*/

void RegRemesh_BuildEqs( RegRemesh* self, int** _eqs );
void RegRemesh_BuildSystem( RegRemesh* self, int* eqs, Mat* _A, Vec* _b );
void RegRemesh_Solve( RegRemesh* self, Mat A, Vec b );

/*-----------------------------------------------------------------------------------------------------------------------------
** Private Member functions
*/

#endif
