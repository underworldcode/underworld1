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
**	Build the binding table(s).
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __Domain_Mesh_Python_bindings_h__
#define __Domain_Mesh_Python_bindings_h__
	
	extern struct PyMethodDef Mesh_Python_Methods[];

#if 0	/*.... requires bindings to DofLayout and MeshLayout first*/
	extern char Mesh_Python_New__name__[];
	extern char Mesh_Python_New__doc__[];
	PyObject* Mesh_Python_New( PyObject* self, PyObject* args );
#endif
	
	extern char Mesh_Python_Print__name__[];
	extern char Mesh_Python_Print__doc__[];
	PyObject* Mesh_Python_Print( PyObject* self, PyObject* args );
	
	extern char Mesh_Python_Delete__name__[];
	extern char Mesh_Python_Delete__doc__[];
	PyObject* Mesh_Python_Delete( PyObject* self, PyObject* args );
	
	extern char Mesh_Python_GetDictionary__name__[];
	extern char Mesh_Python_GetDictionary__doc__[];
	PyObject* Mesh_Python_GetDictionary( PyObject* self, PyObject* args );
	
#endif /* __Domain_Mesh_Python_bindings_h__ */
