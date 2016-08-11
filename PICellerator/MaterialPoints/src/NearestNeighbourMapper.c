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
#include <float.h>

const Type NearestNeighbourMapper_Type = "NearestNeighbourMapper";

void* _NearestNeighbourMapper_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                                 _sizeOfSelf = sizeof(NearestNeighbourMapper);
	Type                                                                         type = NearestNeighbourMapper_Type;
	Stg_Class_DeleteFunction*                                                 _delete = _NearestNeighbourMapper_Delete;
	Stg_Class_PrintFunction*                                                   _print = _NearestNeighbourMapper_Print;
	Stg_Class_CopyFunction*                                                     _copy = _NearestNeighbourMapper_Copy;
	Stg_Component_DefaultConstructorFunction*                     _defaultConstructor = _NearestNeighbourMapper_DefaultNew;
	Stg_Component_ConstructFunction*                                       _construct = _NearestNeighbourMapper_AssignFromXML;
	Stg_Component_BuildFunction*                                               _build = _NearestNeighbourMapper_Build;
	Stg_Component_InitialiseFunction*                                     _initialise = _NearestNeighbourMapper_Initialise;
	Stg_Component_ExecuteFunction*                                           _execute = _NearestNeighbourMapper_Execute;
	Stg_Component_DestroyFunction*                                           _destroy = _NearestNeighbourMapper_Destroy;
	AllocationType                                                 nameAllocationType = NON_GLOBAL;
	IntegrationPointMapper_MapFunction*                                          _map = _NearestNeighbourMapper_Map;
	IntegrationPointMapper_GetMaterialPointsSwarmsFunction*  _getMaterialPointsSwarms = _OneToOneMapper_GetMaterialPointsSwarms;
	IntegrationPointMapper_GetMaterialIndexOnFunction*            _getMaterialIndexOn = _OneToOneMapper_GetMaterialIndexOn;
	IntegrationPointMapper_GetExtensionOnFunction*                    _getExtensionOn = _OneToOneMapper_GetExtensionOn;
   IntegrationPointMapper_GetDoubleFromExtension*            _getDoubleFromExtension = _OneToOneMapper_GetDoubleFromExtension;
	IntegrationPointMapper_GetDoubleFromExtension*             _getDoubleFromMaterial = _OneToOneMapper_GetDoubleFromMaterial;

	return _NearestNeighbourMapper_New(  NEARESTNEIGHBOURMAPPER_PASSARGS  );
}

NearestNeighbourMapper* _NearestNeighbourMapper_New(  NEARESTNEIGHBOURMAPPER_DEFARGS  ) {
	NearestNeighbourMapper* result;

	result = (NearestNeighbourMapper*)_OneToOneMapper_New(  ONETOONEMAPPER_PASSARGS  );

	return result;
}

void _NearestNeighbourMapper_AssignFromXML( void* mapper, Stg_ComponentFactory* cf, void* data ) {
	_OneToOneMapper_AssignFromXML( mapper, cf, data );
}

void _NearestNeighbourMapper_Init( void* mapper ) {
}

void _NearestNeighbourMapper_Delete( void* mapper ) {
	NearestNeighbourMapper* self = (NearestNeighbourMapper*)mapper;

	_OneToOneMapper_Delete( self );
}

void _NearestNeighbourMapper_Print( void* mapper, Stream* stream ) {
	_IntegrationPointMapper_Print( mapper, stream );
}

void* _NearestNeighbourMapper_Copy( void* mapper, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
	return _IntegrationPointMapper_Copy( mapper, dest, deep, nameExt, ptrMap );
}

void _NearestNeighbourMapper_Build( void* mapper, void* cf ) {
	NearestNeighbourMapper* self = (NearestNeighbourMapper*)mapper;

	_OneToOneMapper_Build( self, cf );
}

void _NearestNeighbourMapper_Initialise( void* mapper, void* cf ) {
	NearestNeighbourMapper* self = (NearestNeighbourMapper*)mapper;

	_OneToOneMapper_Initialise( self, cf );

   Journal_Firewall(
                    Stg_Class_IsInstance( self->materialSwarm->cellLayout, ElementCellLayout_Type ),
                    NULL,
                    "Error In func %s: %s expects a materialSwarm with cellLayout of type ElementCellLayout.",
                    __func__, self->type );

    Journal_Firewall(
            self->integrationSwarm->mesh==(FeMesh*)((ElementCellLayout*)self->materialSwarm->cellLayout)->mesh,
            Journal_Register( Error_Type, (Name)self->type  ),
            "Error - in %s(): NearestNeighbourMapper requires both the MaterialSwarm and\n"
            "the IntegrationSwarm to live on the same mesh.\n"
            "Here the MaterialSwarm %s lives in the mesh %s\n"
            "and the IntegrationSwarm %s lives in the mesh %s.",
            self->materialSwarm->name, ((ElementCellLayout*)self->materialSwarm->cellLayout)->mesh->name,
            self->integrationSwarm->name, self->integrationSwarm->mesh->name);

}

void _NearestNeighbourMapper_Execute( void* mapper, void* data ) {
}

void _NearestNeighbourMapper_Destroy( void* mapper, void* data ) {
	NearestNeighbourMapper* self = (NearestNeighbourMapper*)mapper;

	_OneToOneMapper_Destroy( self, data );
}

void _NearestNeighbourMapper_Map( void* mapper ) {
	NearestNeighbourMapper*			self = (NearestNeighbourMapper*)mapper;
	IntegrationPointsSwarm*	integrationSwarm = self->integrationSwarm;
	MaterialPointsSwarm*		materialSwarm = self->materialSwarm;
	IntegrationPoint*			integrationPoint;
	MaterialPoint*				materialPoint;
	MaterialPointRef*			ref;
	Mesh*						   mesh = ((ElementCellLayout*)self->materialSwarm->cellLayout)->mesh;
	Particle_Index				particle_M;
	Particle_Index				particle_I;
	Cell_Index					cell_I;
	Cell_Index					cell_M;
	Stream*						debugStream = Swarm_Debug;
   double                  global[3];
   unsigned                lElement_I;
   double                  distance2;
   double                  distance2_min;
	
	Stream_IndentBranch( debugStream );

   for( lElement_I = 0; lElement_I < FeMesh_GetElementLocalSize( mesh ); lElement_I++ ) {
   
      cell_I = CellLayout_MapElementIdToCellId( integrationSwarm->cellLayout, lElement_I );
      cell_M = CellLayout_MapElementIdToCellId( materialSwarm->cellLayout, lElement_I );

      for ( particle_I = 0; particle_I < integrationSwarm->cellParticleCountTbl[ cell_I ]; particle_I++ ) {
         integrationPoint = (IntegrationPoint*)Swarm_ParticleInCellAt( integrationSwarm, cell_I, particle_I );
         
         ref = OneToOneMapper_GetMaterialRef( self, integrationPoint );
         ref->swarm_I = materialSwarm->swarmReg_I;
         
         /* Convert integration point local to global coordinates */
         FeMesh_CoordLocalToGlobal( mesh, lElement_I, integrationPoint->xi, (double*) &global );

         /* now lets sweep material points to find our closest friend */
         distance2_min = DBL_MAX;
         for ( particle_M = 0; particle_M < materialSwarm->cellParticleCountTbl[ cell_M ]; particle_M++ ) {
            materialPoint = (MaterialPoint*)Swarm_ParticleInCellAt( materialSwarm, cell_M, particle_M );
            distance2 = pow( global[0] - materialPoint->coord[0], 2 ) + pow( global[1] - materialPoint->coord[1], 2 );
            if( materialSwarm->dim == 3 ) distance2 += pow( global[2] - materialPoint->coord[2], 2 );
            if (distance2<distance2_min ){
               distance2_min = distance2;
               ref->particle_I = Swarm_ParticleCellIDtoLocalID( materialSwarm, cell_M, particle_M );
            }
         }
      }
   }

	Stream_UnIndentBranch( debugStream );
	Journal_DPrintfL( debugStream, 1, "...%s(): Done.\n", __func__ );

}


