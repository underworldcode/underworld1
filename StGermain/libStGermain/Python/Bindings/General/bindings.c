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
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <Python.h>

#include <mpi.h>
#include "StGermain/StGermain.h"
#include <limits.h>
#include "bindings.h"
#include "misc.h"

struct PyMethodDef General_Python_Methods[] = {
	{ General_Python_copyright__name__,	General_Python_copyright,		METH_VARARGS, General_Python_copyright__doc__	},
	{ General_Python_Initialise__name__,	General_Python_Initialise,	METH_VARARGS, General_Python_Initialise__doc__	},
	{ 0, 0, 0, 0 }
};

/* "Initialise" member */
char General_Python_Initialise__doc__[] = "Initialise all StGermain C modules";
char General_Python_Initialise__name__[] = "Initialise";
PyObject* General_Python_Initialise( PyObject* self, PyObject* args ) {
	int result = 0;
	int argc = 0;
	char **argv = NULL;
	
	/*
	TODO: grab the argc and argv values to pass in correctly
	*/
	/* Obtain arguements */
	#if 0
	if( !PyArg_ParseTuple( args, "iO:", &, &len ) ) {
		return NULL;
	}
	#endif

	result = StGermain_Init( &argc, &argv );
	return Py_BuildValue("i", result);
}


