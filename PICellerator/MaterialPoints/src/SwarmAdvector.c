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

#include "types.h"
#include "SwarmAdvector.h"

#include "GeneralSwarm.h"
#include "PeriodicBoundariesManager.h"
#include <assert.h>
#include <string.h>
#include <math.h>


/* Textual name of this class */
const Type SwarmAdvector_Type = "SwarmAdvector";

/*-------------------------------------------------------------------------------------------------------------------------
** Constructors
*/
SwarmAdvector* SwarmAdvector_New(
		Name                                       name,
		DomainContext*                             context,
		TimeIntegrator*                            timeIntegrator,
		FeVariable*                                velocityField,
		Bool                                       allowFallbackToFirstOrder,
		GeneralSwarm*                       swarm,
		PeriodicBoundariesManager*                 periodicBCsManager )
{
	SwarmAdvector* self = (SwarmAdvector*) _SwarmAdvector_DefaultNew( name );

	/* 	SwarmAdvector_InitAll */
	_TimeIntegrand_Init( self, context, timeIntegrator, swarm->particleCoordVariable->variable, 0, NULL,
		allowFallbackToFirstOrder );
	_SwarmAdvector_Init( self, velocityField, swarm, periodicBCsManager );

	return self;
}

SwarmAdvector* _SwarmAdvector_New(  SWARMADVECTOR_DEFARGS  )
{
	SwarmAdvector* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(SwarmAdvector) );
	self = (SwarmAdvector*)_TimeIntegrand_New(  TIMEINTEGRAND_PASSARGS  );
	
	/* General info */

	/* Virtual Info */
	
	return self;
}

void _SwarmAdvector_Init( 
		SwarmAdvector*                             self,
		FeVariable*                                velocityField,
		GeneralSwarm*                       swarm,
		PeriodicBoundariesManager*                 periodicBCsManager )
{	
	self->velocityField = velocityField;
	self->swarm = swarm;

  // check we don't have a GaussSwarm as swarm input (only way to check is by the cellLayout Type)
   Journal_Firewall( !Stg_Class_IsInstance(swarm->cellLayout, SingleCellLayout_Type ),
         global_error_stream,
         "Error in '%s'. Can attach a SwarmAdvector to swarm '%s' because the swarm's cell layout '%s'"
         "is of type '%s' which is incompatible with advection.\nAre you sure you want to advect this swarm?"
         " If you change it's cell layout",
         __func__, swarm->name, swarm->cellLayout->name, swarm->cellLayout->type );

    if( swarm->swarmAdvector == NULL )
	   swarm->swarmAdvector = self;	/* Attach ourselves to the swarm */
    else
	   Journal_Firewall( 0 ,
			global_error_stream,
			"In func - %s.\nSwarm '%s' appears to already have an advector (%s) attached,\n"
			"but now advector (%s) is trying to attach itself.\n"
			"Please check your input file, and ensure there is only one advector per swarm.",
			__func__,
			swarm->name,
			swarm->swarmAdvector->name,
			self->name );

	swarm->isAdvecting = True;
	self->variable = swarm->particleCoordVariable->variable;

   Journal_Firewall(
                    Stg_Class_IsInstance( swarm->cellLayout, ElementCellLayout_Type ),
                    NULL,
                    "Error In func %s: %s expects a materialSwarm with cellLayout of type ElementCellLayout.",
                    __func__, self->type );
   
	 /* if not regular use quicker algorithm for SwarmAdvection */
	 if( ((ElementCellLayout*)swarm->cellLayout)->mesh->isRegular == False && self->type == SwarmAdvector_Type ) {
	    self->_calculateTimeDeriv = _SwarmAdvector_TimeDeriv_Quicker4IrregularMesh;
	 }
	self->periodicBCsManager = periodicBCsManager;
	


	TimeIntegrator_AppendSetupEP( self->timeIntegrator,  
			"SwarmAdvector_AdvectionSetup", SwarmAdvector_AdvectionSetup,  self->name, self );
	TimeIntegrator_PrependFinishEP( self->timeIntegrator,
			"SwarmAdvector_AdvectionFinish", SwarmAdvector_AdvectionFinish,  self->name, self );
	TimeIntegrator_InsertBeforeFinishEP( 
		self->timeIntegrator,
		"IntegrationPointsSwarm_Update", /* Must before this */
		"GeneralSwarm_Update", 
		_GeneralSwarm_UpdateHook, 
		swarm->name, 
		swarm );
			
}


/*------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _SwarmAdvector_Delete( void* swarmAdvector ) {
	SwarmAdvector* self = (SwarmAdvector*)swarmAdvector;

	/* Delete parent */
	_TimeIntegrand_Delete( self );
}


void _SwarmAdvector_Print( void* swarmAdvector, Stream* stream ) {
	SwarmAdvector* self = (SwarmAdvector*)swarmAdvector;
	
	/* Print parent */
	_TimeIntegrand_Print( self, stream );
}


void* _SwarmAdvector_Copy( void* swarmAdvector, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
	SwarmAdvector*	self = (SwarmAdvector*)swarmAdvector;
	SwarmAdvector*	newSwarmAdvector;
	
	newSwarmAdvector = (SwarmAdvector*)_TimeIntegrand_Copy( self, dest, deep, nameExt, ptrMap );

	newSwarmAdvector->velocityField = self->velocityField;
	newSwarmAdvector->swarm         = self->swarm;
	newSwarmAdvector->periodicBCsManager = self->periodicBCsManager;
	
	return (void*)newSwarmAdvector;
}

void* _SwarmAdvector_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                               _sizeOfSelf = sizeof(SwarmAdvector);
	Type                                                       type = SwarmAdvector_Type;
	Stg_Class_DeleteFunction*                               _delete = _SwarmAdvector_Delete;
	Stg_Class_PrintFunction*                                 _print = _SwarmAdvector_Print;
	Stg_Class_CopyFunction*                                   _copy = _SwarmAdvector_Copy;
	Stg_Component_DefaultConstructorFunction*   _defaultConstructor = _SwarmAdvector_DefaultNew;
	Stg_Component_ConstructFunction*                     _construct = _SwarmAdvector_AssignFromXML;
	Stg_Component_BuildFunction*                             _build = _SwarmAdvector_Build;
	Stg_Component_InitialiseFunction*                   _initialise = _SwarmAdvector_Initialise;
	Stg_Component_ExecuteFunction*                         _execute = _SwarmAdvector_Execute;
	Stg_Component_DestroyFunction*                         _destroy = _SwarmAdvector_Destroy;
	TimeIntegrand_CalculateTimeDerivFunction*  _calculateTimeDeriv = _SwarmAdvector_TimeDeriv;
	TimeIntegrand_IntermediateFunction*              _intermediate = _SwarmAdvector_Intermediate;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*) _SwarmAdvector_New(  SWARMADVECTOR_PASSARGS  );
}


void _SwarmAdvector_AssignFromXML( void* swarmAdvector, Stg_ComponentFactory* cf, void* data ) {
	SwarmAdvector*	            self          = (SwarmAdvector*) swarmAdvector;
	FeVariable*                 velocityField;
	GeneralSwarm*        swarm;
	PeriodicBoundariesManager*  periodicBCsManager;

	_TimeIntegrand_AssignFromXML( self, cf, data );

	velocityField      = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"VelocityField", FeVariable, True, data  );
	swarm              = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Swarm", GeneralSwarm, True, data  );
	periodicBCsManager = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"PeriodicBCsManager", PeriodicBoundariesManager, False, data  );

	_SwarmAdvector_Init( self, velocityField, swarm, periodicBCsManager );
}

void _SwarmAdvector_Build( void* swarmAdvector, void* data ) {
   SwarmAdvector*	self = (SwarmAdvector*) swarmAdvector;
   GeneralSwarm *swarm=NULL;

   Stg_Component_Build( self->velocityField, data, False );
   Stg_Component_Build( self->swarm, data, False );

   swarm=self->swarm;

   /* Test if mesh is periodic and a periodic boundaries manager hasn't been given */
   if ( !self->periodicBCsManager && Stg_Class_IsInstance( ((ElementCellLayout*)swarm->cellLayout)->mesh->generator, CartesianGenerator_Type ) ) {
      CartesianGenerator* cartesianGenerator = (CartesianGenerator*) ((ElementCellLayout*)swarm->cellLayout)->mesh->generator;
      if ( cartesianGenerator->periodic[ I_AXIS ] ||
           cartesianGenerator->periodic[ J_AXIS ] || 
           cartesianGenerator->periodic[ K_AXIS ] ) {
         /* Create a periodicBCsManager if there isn't one already */
         self->periodicBCsManager = PeriodicBoundariesManager_New( "periodicBCsManager", (PICelleratorContext*)self->context, (Mesh*)((ElementCellLayout*)swarm->cellLayout)->mesh, (Swarm*)swarm, NULL );
   }


}
	if ( self->periodicBCsManager )
		Stg_Component_Build( self->periodicBCsManager, data, False );
   _TimeIntegrand_Build( self, data );

}

void _SwarmAdvector_Initialise( void* swarmAdvector, void* data ) {
	SwarmAdvector*	self = (SwarmAdvector*) swarmAdvector;
	
   Stg_Component_Initialise( self->velocityField, data, False );
   Stg_Component_Initialise( self->swarm, data, False );
	if ( self->periodicBCsManager )
		Stg_Component_Initialise( self->periodicBCsManager, data, False );
	_TimeIntegrand_Initialise( self, data );
}

void _SwarmAdvector_Execute( void* swarmAdvector, void* data ) {
	SwarmAdvector*	self = (SwarmAdvector*)swarmAdvector;
	
	_TimeIntegrand_Execute( self, data );
}

void _SwarmAdvector_Destroy( void* swarmAdvector, void* data ) {
	SwarmAdvector*	self = (SwarmAdvector*)swarmAdvector;

	_TimeIntegrand_Destroy( self, data );	
   Stg_Component_Destroy( self->velocityField, data, False );
   Stg_Component_Destroy( self->swarm, data, False );
	if ( self->periodicBCsManager )
		Stg_Component_Destroy( self->periodicBCsManager, data, False );
}

Bool _SwarmAdvector_TimeDeriv( void* swarmAdvector, Index array_I, double* timeDeriv ) {
	SwarmAdvector*      self          = (SwarmAdvector*) swarmAdvector;
	FieldVariable*      velocityField = (FieldVariable*) self->velocityField;
	double*             coord;
	InterpolationResult result;

	/* Get Coordinate of Object using Variable */
	coord = Variable_GetPtrDouble( self->variable, array_I );

	result = FieldVariable_InterpolateValueAt( velocityField, coord, timeDeriv );

	if ( result == OTHER_PROC || result == OUTSIDE_GLOBAL || isinf(timeDeriv[0]) || isinf(timeDeriv[1]) || 
			( self->swarm->dim == 3 && isinf(timeDeriv[2]) ) ) 
	{
		#if 0
		Journal_Printf( Journal_Register( Error_Type, (Name)self->type  ),
			"Error in func '%s' for particle with index %u.\n\tPosition (%g, %g, %g)\n\tVelocity here is (%g, %g, %g)."
			"\n\tInterpolation result is %s.\n",
			__func__, array_I, coord[0], coord[1], coord[2], 
			timeDeriv[0], timeDeriv[1], ( self->swarm->dim == 3 ? timeDeriv[2] : 0.0 ),
			InterpolationResultToStringMap[result]  );
		#endif	
		return False;	
	}

	return True;
}

Bool _SwarmAdvector_TimeDeriv_Quicker4IrregularMesh( void* swarmAdvector, Index array_I, double* timeDeriv ) {
   SwarmAdvector*      self          = (SwarmAdvector*) swarmAdvector;
   FeVariable*      velocityField = (FeVariable*) self->velocityField;
   GlobalParticle* particle=NULL;
   FeMesh_ElementType* eType=NULL;
   double xi[3];
   unsigned cellID;

   particle = (GlobalParticle*)Swarm_ParticleAt( self->swarm, array_I );
   assert( particle );

   // get particle's cell/element id - ASSUMES 1-to-1 cellID to elementID
   // if not local return False
   cellID = CellLayout_CellOf( self->swarm->cellLayout, particle );
   if( cellID >= CellLayout_CellLocalCount( self->swarm->cellLayout ) )
      return False;

   /* below is some funky code to optimise time in retrieving :
      Why? for an irregular mesh the function CellLayout_CellOf() will actually
      evaulate the local coordinate and store it on a temporary valiable (on 'FeMesh_ElementType->local[]') 
      we will access below, instead of re-evaluating the GlobaToLocal loop (i.e. Newton-Raphson iteration)
      with a function like FeMesh_CoordGlobalToLocal().
   
      This ASSUMES that the temporary storage isn't modified between CellLayout_CellOf() and here.
   */

   // get particle's local coordinate
   eType = (FeMesh_ElementType*)velocityField->feMesh->elTypes[velocityField->feMesh->elTypeMap[cellID]];
   FeMesh_ElementTypeGetLocal( eType, xi );
   
   // do interpolation
   FeVariable_InterpolateWithinElement( velocityField, cellID, xi, timeDeriv );

   return True;
}


void _SwarmAdvector_Intermediate( void* swarmAdvector, Index lParticle_I ) {
	SwarmAdvector*      self          = (SwarmAdvector*) swarmAdvector;

	if ( self->periodicBCsManager ) {
      Journal_Firewall( (Swarm*)self->swarm == (Swarm*)self->periodicBCsManager->swarm, global_error_stream,
            "Error in %s. The SwarmAdvector %s and periodBCsManager %s don't use the same swarm, erroroneos condition\n",
            self->swarm->name, self->periodicBCsManager->swarm->name );
             
		// use the polymorphic function not a static one!
		self->periodicBCsManager->_updateParticle( self->periodicBCsManager, lParticle_I );
	}
}
/*-------------------------------------------------------------------------------------------------------------------------
** Private Functions
*/
/*---------------------------------------------------------------------------------------------------------------------
** Entry Point Hooks
*/
void SwarmAdvector_AdvectionSetup( TimeIntegrator* timeIntegrator, SwarmAdvector* self ) {
	FeVariable_SyncShadowValues( self->velocityField );
}

void SwarmAdvector_AdvectionFinish( TimeIntegrator* timeIntegrator, SwarmAdvector* self ) {
	#if DEBUG
		Swarm_CheckCoordsAreFinite( self->swarm );
	#endif
	
	/* Move particles across processors because they've just been advected */
	Swarm_UpdateAllParticleOwners( self->swarm );
}


/*-------------------------------------------------------------------------------------------------------------------------
** Public Functions
*/




