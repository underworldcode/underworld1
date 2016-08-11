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
#include "StgFEM/Discretisation/Discretisation.h"
#include "StgFEM/SLE/SystemSetup/SystemSetup.h"
#include "types.h"
#include "Stokes_SLE.h"

#include <stdio.h>
#include <stdlib.h>

double Stokes_SLE_UpdateDt( Stokes_SLE* self, FiniteElementContext* context ) {
	double                  velMax        = 0;
	double                  minSeparation = 0;
	double                  minSeparationEachDim[3] = { 0, 0, 0 };
	FeVariable*             velFeVar      = NULL;
	double			localDt;
	double			globalDt;
	
	velFeVar = self->uSolnVec->feVariable;
	velMax = FieldVariable_GetMaxGlobalFieldMagnitude( velFeVar );

	Journal_DPrintf( self->debug, "In func %s for SLE '%s' - Maximum global magnitude for field '%s' = %g.\n",
		__func__, self->name, velFeVar->name, velMax );

	FeVariable_GetMinimumSeparation( velFeVar, &minSeparation, minSeparationEachDim );

	localDt = 0.5 * minSeparation / velMax;
	(void)MPI_Allreduce( &localDt, &globalDt, 1, MPI_DOUBLE, MPI_MIN, MPI_COMM_WORLD );

	return globalDt;
}


