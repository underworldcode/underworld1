/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**	Melbourne, 3053, Australia.
** Copyright (c) 2005-2010, Monash University, Building 28, Monash University Clayton Campus,
**	Victoria, 3800, Australia
**
** Primary Contributing Organisations:
**	Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**	AuScope - http://www.auscope.org
**	Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**
** Contributors:
**	Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**	Patrick D. Sunter, Software Engineer, VPAC. (patrick@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**	Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
**	Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**	Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**	David Stegman, Postdoctoral Fellow, Monash University. (david.stegman@sci.monash.edu.au)
**	Wendy Sharples, PhD Student, Monash University (wendy.sharples@sci.monash.edu.au)
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

#include <PICellerator/PopulationControl/PopulationControl.h>
#include <PICellerator/Weights/Weights.h>

#include "MaterialPoints.h"

#include <assert.h>
#include <string.h>
#include <math.h>

/*I am generalising this so that I can use it- Wendy S*/


/* Textual name of this class */
const Type SwarmAdvectionIJK_Type = "SwarmAdvectionIJK";

/*-------------------------------------------------------------------------------------------------------------------------
** Constructors
*/
SwarmAdvectionIJK* SwarmAdvectionIJK_New(
		Name                                       name,
		DomainContext*                             context,
		TimeIntegrator*                            timeIntegrator,
		FeVariable*                                velocityField,
		Bool                                       allowFallbackToFirstOrder,
		MaterialPointsSwarm*                       swarm,
		PeriodicBoundariesManager*                 periodicBCsManager )
{
	SwarmAdvectionIJK* self = (SwarmAdvectionIJK*) _SwarmAdvectionIJK_DefaultNew( name );
	Bool suppressaxis[3];
	
	/* 	SwarmAdvectionIJK_InitAll */
	_TimeIntegrand_Init( self, context, timeIntegrator, swarm->particleCoordVariable->variable, 0, NULL,
		allowFallbackToFirstOrder );
	_SwarmAdvector_Init( (SwarmAdvector*)self, velocityField, swarm, periodicBCsManager);
	_SwarmAdvectionIJK_Init( self, suppressaxis );

	return self;
}

SwarmAdvectionIJK* _SwarmAdvectionIJK_New(  SWARMADVECTIONIJK_DEFARGS  )
{
	SwarmAdvectionIJK* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(SwarmAdvectionIJK) );
	self = (SwarmAdvectionIJK*)_SwarmAdvector_New(  SWARMADVECTOR_PASSARGS  );
	
	/* General info */

	/* Virtual Info */
	
	return self;
}

void _SwarmAdvectionIJK_Init( SwarmAdvectionIJK* self, Bool *suppressaxis )
{
	/* Adding parameter which axis as the user can choose to prevent
	   the movement of a tracer in any / all of the X (0), Y (1) or Z (2) directions */
	
	int i;
	
	for(i=0;i<self->swarm->dim;i++)	
		self->suppressaxis[i] = suppressaxis[i];
}


/*------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void* _SwarmAdvectionIJK_Copy( void* swarmAdvector, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
	SwarmAdvectionIJK*	self = (SwarmAdvectionIJK*)swarmAdvector;
	SwarmAdvectionIJK*	newSwarmAdvectionIJK;
	
	newSwarmAdvectionIJK = (SwarmAdvectionIJK*)_SwarmAdvector_Copy( self, dest, deep, nameExt, ptrMap );

	newSwarmAdvectionIJK->velocityField = self->velocityField;
	newSwarmAdvectionIJK->swarm         = self->swarm;
	newSwarmAdvectionIJK->periodicBCsManager = self->periodicBCsManager;
	
	return (void*)newSwarmAdvectionIJK;
}

void* _SwarmAdvectionIJK_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                               _sizeOfSelf = sizeof(SwarmAdvectionIJK);
	Type                                                       type = SwarmAdvectionIJK_Type;
	Stg_Class_DeleteFunction*                               _delete = _SwarmAdvector_Delete;
	Stg_Class_PrintFunction*                                 _print = _SwarmAdvector_Print;
	Stg_Class_CopyFunction*                                   _copy = _SwarmAdvectionIJK_Copy;
	Stg_Component_DefaultConstructorFunction*   _defaultConstructor = _SwarmAdvectionIJK_DefaultNew;
	Stg_Component_ConstructFunction*                     _construct = _SwarmAdvectionIJK_AssignFromXML;
	Stg_Component_BuildFunction*                             _build = _SwarmAdvector_Build;
	Stg_Component_InitialiseFunction*                   _initialise = _SwarmAdvector_Initialise;
	Stg_Component_ExecuteFunction*                         _execute = _SwarmAdvector_Execute;
	Stg_Component_DestroyFunction*                         _destroy = _SwarmAdvector_Destroy;
	TimeIntegrand_CalculateTimeDerivFunction*  _calculateTimeDeriv = _SwarmAdvectionIJK_TimeDeriv;
	TimeIntegrand_IntermediateFunction*              _intermediate = _SwarmAdvector_Intermediate;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*) _SwarmAdvectionIJK_New(  SWARMADVECTIONIJK_PASSARGS  );
}


void _SwarmAdvectionIJK_AssignFromXML( void* swarmAdvector, Stg_ComponentFactory* cf, void* data) {
	SwarmAdvectionIJK*	            self          = (SwarmAdvectionIJK*) swarmAdvector;
	Bool	suppressaxis[3];
	_SwarmAdvector_AssignFromXML( self, cf, data );
	
	/* Everything except suppressaxis constructed by parent  */
	
	suppressaxis[0] = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"suppressAxisI", False  );
	suppressaxis[1] = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"suppressAxisJ", False  );
	suppressaxis[2] = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"suppressAxisK", False  );

	_SwarmAdvectionIJK_Init( self, suppressaxis );
}


Bool _SwarmAdvectionIJK_TimeDeriv( void* swarmAdvector, Index array_I, double* timeDeriv ) {
	SwarmAdvectionIJK*      self          = (SwarmAdvectionIJK*) swarmAdvector;
	FieldVariable*      velocityField = (FieldVariable*) self->velocityField;
	double*             coord;
	InterpolationResult result;
	int i;

	/* Get Coordinate of Object using Variable */
	coord = Variable_GetPtrDouble( self->variable, array_I );

	result = FieldVariable_InterpolateValueAt( velocityField, coord, timeDeriv );
	
	
	/* This prevents advection in the X, Y or Z direction */
	
	for( i=0; i<self->swarm->dim; i++)
		if ( self->suppressaxis[i] )
			timeDeriv[ i ] = 0.0;


	if ( result == OTHER_PROC || result == OUTSIDE_GLOBAL || isinf(timeDeriv[0]) || isinf(timeDeriv[1]) || 
			( self->swarm->dim == 3 && isinf(timeDeriv[2]) ) ) 
	{
		#if 0
		Journal_Printf( Journal_Register( Error_Type, (Name)self->type  ),
			"Error in func '%s' for particle with index %u.\n\tPosition (%g, %g, %g)\n\tVelocity here is (%g, %g, %g)."
			"\n\tInteropolation result is %u.\n",
			__func__, array_I, coord[0], coord[1], coord[2], 
			timeDeriv[0], timeDeriv[1], ( self->swarm->dim == 3 ? timeDeriv[2] : 0.0 ),
			result );
		abort();
		#endif

		return False;
	}

	return True;
}

/*-------------------------------------------------------------------------------------------------------------------------
** Private Functions
*/
/*---------------------------------------------------------------------------------------------------------------------
** Entry Point Hooks
*/
/*-------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/




