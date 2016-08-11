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
#include "MaterialPointsSwarm.h"
#include <assert.h>
#include <string.h>
#include <math.h>


/* Textual name of this class */
const Type MaterialPointsSwarm_Type = "MaterialPointsSwarm";

MaterialPointsSwarm* MaterialPointsSwarm_New(
      Name                                  name,
      AbstractContext*                      context,
      void*                                 cellLayout,
      void*                                 particleLayout,
      Dimension_Index                       dim,
      SizeT                                 particleSize,
      EscapedRoutine*                       escapedRoutine,
      Material*                             material,
      Variable_Register*                    swarmVariable_Register,
      ExtensionManager_Register*            extensionMgr_Register,
      Materials_Register*                   materials_Register,		
      MPI_Comm                              comm,
      void*                                 ics_dummy ) 
{
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(MaterialPointsSwarm);
	Type                                                      type = MaterialPointsSwarm_Type;
	Stg_Class_DeleteFunction*                              _delete = _MaterialPointsSwarm_Delete;
	Stg_Class_PrintFunction*                                _print = _MaterialPointsSwarm_Print;
	Stg_Class_CopyFunction*                                  _copy = _MaterialPointsSwarm_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _MaterialPointsSwarm_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _MaterialPointsSwarm_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _MaterialPointsSwarm_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _MaterialPointsSwarm_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _MaterialPointsSwarm_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _MaterialPointsSwarm_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;
	void*                          ics = ZERO;

	/* The following terms are parameters that have been passed into or defined in this function but are being set before being passed onto the parent */
	Particle_InCellIndex  cellParticleTblDelta = DEFAULT_CELL_PARTICLE_TBL_DELTA;
	double                extraParticlesFactor = DEFAULT_EXTRA_PARTICLES_FACTOR;

  MaterialPointsSwarm* self = _MaterialPointsSwarm_New(  MATERIALPOINTSSWARM_PASSARGS  );

   _Swarm_Init( 
         (Swarm*)self, context,
         cellLayout,
         particleLayout,
         dim,
         particleSize,
         cellParticleTblDelta,
         extraParticlesFactor,
         extensionMgr_Register,
         swarmVariable_Register,
         False,
         comm, 
         ics_dummy );

   _GeneralSwarm_Init( self, escapedRoutine );

   _MaterialPointsSwarm_Init( 
      self, 
      material,
      materials_Register );

   return self;
}


MaterialPointsSwarm* _MaterialPointsSwarm_New(  MATERIALPOINTSSWARM_DEFARGS  )
{
	MaterialPointsSwarm* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(MaterialPointsSwarm) );
	/* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
	/* This means that any values of these parameters that are passed into this function are not passed onto the parent function
	   and so should be set to ZERO in any children of this class. */
	ics = NULL;

	self = (MaterialPointsSwarm*)_Swarm_New(  SWARM_PASSARGS  );

	return self;
}


void _MaterialPointsSwarm_Init( 
		void*                                 swarm,
		Material*                             material,
		Materials_Register*                   materials_Register )
{
	MaterialPointsSwarm*    self = (MaterialPointsSwarm*)swarm;
	MaterialPoint           particle;
	GlobalParticle          globalParticle;

	self->material           = material;
	self->materials_Register = materials_Register;
	
	self->materialIndexVariable = Swarm_NewScalarVariable( self, (Name)"MaterialIndex", GetOffsetOfMember( particle , materialIndex  ), Variable_DataType_Int ); /* Should be unsigned int */
   LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*)self->materialIndexVariable->variable );
   LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*)self->materialIndexVariable );

}

/*------------------------------------------------------------------------------------------------------------------------
** Virtual functions
*/

void _MaterialPointsSwarm_Delete( void* swarm ) {
	MaterialPointsSwarm* self = (MaterialPointsSwarm*)swarm;

	_Swarm_Delete( self );
}


void _MaterialPointsSwarm_Print( void* swarm, Stream* stream ) {
	MaterialPointsSwarm* self = (MaterialPointsSwarm*)swarm;
	
	_Swarm_Print( self, stream );
}

void* _MaterialPointsSwarm_Copy( void* swarm, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
	MaterialPointsSwarm*	self = (MaterialPointsSwarm*)swarm;
	MaterialPointsSwarm*	newMaterialPointsSwarm;
	
	newMaterialPointsSwarm = (MaterialPointsSwarm*)_Swarm_Copy( self, dest, deep, nameExt, ptrMap );

	newMaterialPointsSwarm->materialIndexVariable = (SwarmVariable*)Stg_Class_Copy(
				self->materialIndexVariable,
				NULL,
				deep,
				nameExt,
				ptrMap );
	
	return (void*)newMaterialPointsSwarm;
}

void* _MaterialPointsSwarm_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                 _sizeOfSelf = sizeof(MaterialPointsSwarm);
	Type                                                         type = MaterialPointsSwarm_Type;
	Stg_Class_DeleteFunction*                                 _delete = _MaterialPointsSwarm_Delete;
	Stg_Class_PrintFunction*                                   _print = _MaterialPointsSwarm_Print;
	Stg_Class_CopyFunction*                                     _copy = _MaterialPointsSwarm_Copy;
	Stg_Component_DefaultConstructorFunction*     _defaultConstructor = _MaterialPointsSwarm_DefaultNew;
	Stg_Component_ConstructFunction*                       _construct = _MaterialPointsSwarm_AssignFromXML;
	Stg_Component_BuildFunction*                               _build = _MaterialPointsSwarm_Build;
	Stg_Component_InitialiseFunction*                     _initialise = _MaterialPointsSwarm_Initialise;
	Stg_Component_ExecuteFunction*                           _execute = _MaterialPointsSwarm_Execute;
	Stg_Component_DestroyFunction*                           _destroy = _MaterialPointsSwarm_Destroy;
	SizeT                                                particleSize = sizeof(MaterialPoint);

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;
	void*                          ics = ZERO;

	return _MaterialPointsSwarm_New(  MATERIALPOINTSSWARM_PASSARGS  );
}


void _MaterialPointsSwarm_AssignFromXML( void* swarm, Stg_ComponentFactory* cf, void* data ) {
	MaterialPointsSwarm*	        self          = (MaterialPointsSwarm*) swarm;
	EscapedRoutine*                 escapedRoutine;
	Material*                       material;
	Materials_Register*             materials_Register;
	PICelleratorContext*		context;

	_GeneralSwarm_AssignFromXML( self, cf, data );

	material = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Material", Material, False, data );

	context = (PICelleratorContext* )self->context;
	assert( Stg_CheckType( context, PICelleratorContext ) );
	materials_Register = context->materials_Register; 
	assert( materials_Register );

	_MaterialPointsSwarm_Init(
			self,
			material,
			materials_Register );

	self->overrideMaterialCheck = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"overrideMaterialCheck", False );
	self->geomodHack = Dictionary_GetBool_WithDefault( cf->rootDict, (Dictionary_Entry_Key)"geomodHacks", False  );
}

void _MaterialPointsSwarm_Build( void* swarm, void* data ) {
	MaterialPointsSwarm*	self = (MaterialPointsSwarm*) swarm;
	int			commHandler_I;
	Bool                    movementCommHandlerFound = False;
	Stream*                 errorStream = Journal_Register( Error_Type, (Name)self->type  );
	int var_I;

	_GeneralSwarm_Build( self, data );

	if( self->material       != NULL) Stg_Component_Build( self->material,       data , False );
}

void _MaterialPointsSwarm_Initialise( void* swarm, void* data ) {
	MaterialPointsSwarm*	self 	= (MaterialPointsSwarm*) swarm;
	AbstractContext* 	context = (AbstractContext*)self->context;
	Index            	var_I	= 0;
	Particle_Index          lParticle_I=0;
	MaterialPoint*		matPoint=NULL;

	_GeneralSwarm_Initialise( self, data );

	if( self->material       != NULL) Stg_Component_Initialise( self->material,       data , False );

	/* Now setup the material properties */
   if(  (False == context->loadFromCheckPoint) || False == (context->loadSwarmsFromCheckpoint) ) {

      /* Beforehand, set each particle to have UNDEFINED_MATERIAL */
      for ( lParticle_I = 0; lParticle_I < self->particleLocalCount; lParticle_I++ ) {
         matPoint = (MaterialPoint*)Swarm_ParticleAt( self, lParticle_I );
         matPoint->materialIndex = UNDEFINED_MATERIAL;
      }
		if( self->material == NULL ) {
			/* Do it by the layout of all known materials */
			Materials_Register_SetupSwarm( self->materials_Register, self );
		}
		else {
			Material_Layout( self->material, self );
			Materials_Register_AssignParticleProperties( 
					self->materials_Register, 
					self, 
					self->swarmVariable_Register->variable_Register );
		}
	}

   if( self->overrideMaterialCheck == False ) {
      /* Check to ensure all particles have a valid material  */
      for ( lParticle_I = 0; lParticle_I < self->particleLocalCount; lParticle_I++ ) {
         matPoint = (MaterialPoint*)Swarm_ParticleAt( self, lParticle_I );
         Journal_Firewall( (matPoint->materialIndex != UNDEFINED_MATERIAL), Journal_MyStream( Error_Type, self ),
               "Error in function %s for swarm '%s'. Material point at (%.5g, %.5g, %.5g), has no material assigned to it.\n"
               "This is most likely because the material geometries don't cover the entire domain - check material/geometry definitions\n\n"
               "To check: visualise the material index field initialisation by running your model with the commandline options\n"
               "\t\'--maxTimeSteps=-1 --components.%s.overrideMaterialCheck=True\'\n", __func__, self->name, matPoint->coord[0], matPoint->coord[1], matPoint->coord[2], self->name);  
      }
   }
}
void _MaterialPointsSwarm_Execute( void* swarm, void* data ) {
	MaterialPointsSwarm*	self = (MaterialPointsSwarm*)swarm;
	
	_GeneralSwarm_Execute( self, data );
}
void _MaterialPointsSwarm_Destroy( void* swarm, void* data ) {
	MaterialPointsSwarm*	self = (MaterialPointsSwarm*)swarm;
   int var_I;
	
	if( self->material != NULL) Stg_Component_Destroy( self->material, data , False );

	_GeneralSwarm_Destroy( self, data );
}

void _MaterialPointsSwarm_UpdateHook( void* timeIntegrator, void* swarm ) {
   _GeneralSwarm_UpdateHook( timeIntegrator, swarm );
}

void MaterialPointsSwarm_SetMaterialAt( void* swarm, Index point_I, Index materialIndex ) {
	MaterialPointsSwarm* self  = (MaterialPointsSwarm*)swarm;
	MaterialPoint*       point;

	point = (MaterialPoint*)Swarm_ParticleAt( self, point_I );
	point->materialIndex = materialIndex;
}


Material* MaterialPointsSwarm_GetMaterialOn( void* swarm, void* particle ) {
	MaterialPointsSwarm* self  = (MaterialPointsSwarm*)swarm;
	MaterialPoint*       materialPoint = (MaterialPoint*)particle;

	return Materials_Register_GetByIndex( self->materials_Register, materialPoint->materialIndex );
}


Material* MaterialPointsSwarm_GetMaterialAt( void* swarm, Index point_I ) {
	MaterialPointsSwarm* self  = (MaterialPointsSwarm*)swarm;
	MaterialPoint*       point;

	point = (MaterialPoint*)Swarm_ParticleAt( self, point_I );
	return Materials_Register_GetByIndex( self->materials_Register, point->materialIndex );
}

Index MaterialPointsSwarm_GetMaterialIndexAt( void* swarm, Index point_I ) {
	MaterialPointsSwarm* self  = (MaterialPointsSwarm*)swarm;
	MaterialPoint*       point;

	point = (MaterialPoint*)Swarm_ParticleAt( self, point_I );
	return point->materialIndex;
}

void* MaterialPointsSwarm_GetMaterialExtensionOn( void* swarm, void *matPoint, Index extHandle ) {
	MaterialPointsSwarm *self  = (MaterialPointsSwarm*)swarm;
	Material *mat;

	mat = MaterialPointsSwarm_GetMaterialOn(self, matPoint);
	return ExtensionManager_Get(mat->extensionMgr, mat, extHandle);
}

void* MaterialPointsSwarm_GetMaterialExtensionAt( void* swarm, int matPointInd, Index extHandle ) {
	MaterialPointsSwarm *self  = (MaterialPointsSwarm*)swarm;
	Material *mat;

	mat = MaterialPointsSwarm_GetMaterialAt(self, matPointInd);
	return ExtensionManager_Get(mat->extensionMgr, mat, extHandle);
}

void* MaterialPointsSwarm_GetExtensionAt( void* swarm, Index point_I, Index extHandle ) {
	MaterialPointsSwarm* self  = (MaterialPointsSwarm*)swarm;
	MaterialPoint*       point;

	point = (MaterialPoint*)Swarm_ParticleAt( self, point_I );
	return ExtensionManager_Get( self->particleExtensionMgr, point, extHandle );
	
}


