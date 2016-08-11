/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**	Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**	Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**	AuScope - http://www.auscope.org
**	Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**	Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**	Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**	Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**	Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
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

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include "types.h"
#include "FeVariable_IrregTri.h"
#include "IrregularTriGenerator.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

const Type FeVariable_IrregTri_Type = (const Type)"FeVariable_IrregTri";

void* _FeVariable_IrregTri_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                         _sizeOfSelf = sizeof(FeVariable);
	Type                                                                 type = FeVariable_IrregTri_Type;
	Stg_Class_DeleteFunction*                                         _delete = _FeVariable_Delete;
	Stg_Class_PrintFunction*                                           _print = _FeVariable_Print;
	Stg_Class_CopyFunction*                                             _copy = _FeVariable_Copy;
	Stg_Component_DefaultConstructorFunction*             _defaultConstructor = (Stg_Component_DefaultConstructorFunction*)_FeVariable_DefaultNew;
	Stg_Component_ConstructFunction*                               _construct = _FeVariable_AssignFromXML;
	Stg_Component_BuildFunction*                                       _build = _FeVariable_Build;
	Stg_Component_InitialiseFunction*                             _initialise = _FeVariable_Initialise;
	Stg_Component_ExecuteFunction*                                   _execute = _FeVariable_Execute;
	Stg_Component_DestroyFunction*                                   _destroy = _FeVariable_Destroy;
	AllocationType                                         nameAllocationType = NON_GLOBAL;
	FieldVariable_InterpolateValueAtFunction*             _interpolateValueAt = _FeVariable_IrregTri_InterpolateValueAt;
	FieldVariable_GetValueFunction*               _getMinGlobalFieldMagnitude = _FeVariable_GetMinGlobalFieldMagnitude;
	FieldVariable_GetValueFunction*               _getMaxGlobalFieldMagnitude = _FeVariable_GetMaxGlobalFieldMagnitude;
	FieldVariable_CacheValuesFunction*       _cacheMinMaxGlobalFieldMagnitude = _FeVariable_CacheMinMaxGlobalFieldMagnitude;
	FieldVariable_GetCoordFunction*                  _getMinAndMaxLocalCoords = _FeVariable_GetMinAndMaxLocalCoords;
	FieldVariable_GetCoordFunction*                 _getMinAndMaxGlobalCoords = _FeVariable_GetMinAndMaxGlobalCoords;
	FeVariable_InterpolateWithinElementFunction*    _interpolateWithinElement = _FeVariable_InterpolateNodeValuesToElLocalCoord;
	FeVariable_GetValueAtNodeFunction*                        _getValueAtNode = _FeVariable_GetValueAtNode;
	FeVariable_SyncShadowValuesFunc*                        _syncShadowValues = _FeVariable_SyncShadowValues;

	return _FeVariable_New(  FEVARIABLE_PASSARGS  ); /* feVariableList */
}

InterpolationResult _FeVariable_IrregTri_InterpolateValueAt( void* variable, double* globalCoord, double* value ) {
	FeVariable*				self = (FeVariable*)variable;
	InterpolationResult	retValue;
	Element_DomainIndex	elementCoordIn = (unsigned)-1;
	Stream*			      errorStream = Journal_Register( ErrorStream_Type, FeVariable_IrregTri_Type );

   if( !Mesh_SearchElements( self->feMesh, globalCoord, &elementCoordIn ) ) {
      Journal_Printf( errorStream, "in func: '%s()': the coordinate: [%.6e,%.6e] is not within FeVariable_IrregTri domain:"
                    " %s. \n", __func__, globalCoord[0], globalCoord[1],
                    self->name );
      assert( 0 );
   }

   retValue = LOCAL;

   *value = IrregularTriGenerator_InterpolateWithinElement( self->feMesh->generator, self, elementCoordIn, globalCoord );

	return retValue;
}
