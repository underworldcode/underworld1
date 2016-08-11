/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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
** Role:
**   Tests the SourceTermsSuite
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdio.h>
#include <stdlib.h>

#include "pcu/pcu.h"
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>
#include <PDERework/PDERework.h>
#include <Geothermal/Geothermal.h>
#include "SourceTermsSuite.h"

typedef struct {
} SourceTermsSuiteData;

#define TOLERANCE 0.001
#define Q 1.0
#define TV_1 1.5
#define TV_2 1.33333333333
#define TV_3 0.58571

double T_heatFluxTest( double y ) { return 2.0 - y; }
double T_heatProductionTest( double y ) { return 0.5*Q*y*y + 1.0 - 0.5*Q; }
double T_conductivity1( double y ) { return y; }

double IntegrateField( FeVariable* tempField, IntegrationPointsSwarm* swarm ) {
	FeMesh*			mesh	= tempField->feMesh;
	unsigned		el_i;
	unsigned		cell_i;
	unsigned		point_i;
	double			lTemp	= 0.0;
	double			gTemp;
	double			temp;
	double			detJac;
	IntegrationPoint*	point;
	ElementType*		elType;
	unsigned		dim	= Mesh_GetDimSize( mesh );
	
	for( el_i = 0; el_i < Mesh_GetLocalSize( mesh, dim ); el_i++ ) {
		cell_i = CellLayout_MapElementIdToCellId( swarm->cellLayout, el_i );
		elType = FeMesh_GetElementType( mesh, el_i );
		
		for( point_i = 0; point_i < swarm->cellParticleCountTbl[cell_i]; point_i++ ) {
			point = (IntegrationPoint*)Swarm_ParticleInCellAt( swarm, cell_i, point_i );

			FeVariable_InterpolateWithinElement( tempField, el_i, point->xi, &temp );
			detJac = ElementType_JacobianDeterminant( elType, mesh, el_i, point->xi, dim );

			lTemp += detJac * point->weight * temp;
		}
	}

	MPI_Allreduce( &lTemp, &gTemp, 1, MPI_DOUBLE, MPI_SUM, MPI_COMM_WORLD );

	return gTemp;
}

void SourceTermsSuite_Setup( SourceTermsSuiteData* data ) {
}

void SourceTermsSuite_Teardown( SourceTermsSuiteData* data ) {
}


void SourceTermsSuite_TestHeatFlux( SourceTermsSuiteData* data ) {
	Stg_ComponentFactory*		cf;
	FeVariable*			tempField;
	IntegrationPointsSwarm*		swarm;
	double				temp;

	cf = stgMainInitFromXML( "Geothermal/Base/input/testSourceTerms_HeatFlux.xml", MPI_COMM_WORLD, NULL );
	stgMainBuildAndInitialise( cf );

	swarm = (IntegrationPointsSwarm*)LiveComponentRegister_Get( cf->LCRegister, "picIntegrationPoints" );
	tempField = (FeVariable*)LiveComponentRegister_Get( cf->LCRegister, "TemperatureField" );

	stgMainLoop( cf );

	temp = IntegrateField( tempField, swarm );
	pcu_check_true( fabs( temp - TV_1 ) < TOLERANCE );
	
	stgMainDestroy( cf );

}

void SourceTermsSuite_TestHeatProduction( SourceTermsSuiteData* data ) {
	Stg_ComponentFactory*		cf;
	FeVariable*			tempField;
	IntegrationPointsSwarm*		swarm;
	double				temp;

	cf = stgMainInitFromXML( "Geothermal/Base/input/testSourceTerms_HeatProduction.xml", MPI_COMM_WORLD, NULL );
	stgMainBuildAndInitialise( cf );

	swarm = (IntegrationPointsSwarm*)LiveComponentRegister_Get( cf->LCRegister, "picIntegrationPoints" );
	tempField = (FeVariable*)LiveComponentRegister_Get( cf->LCRegister, "TemperatureField" );

	stgMainLoop( cf );

	temp = IntegrateField( tempField, swarm );
	pcu_check_true( fabs( temp - TV_2 ) < TOLERANCE );
	
	stgMainDestroy( cf );
}

void SourceTermsSuite_TestConductivity1( SourceTermsSuiteData* data ) {
	Stg_ComponentFactory*		cf;
	FeVariable*			tempField;
	IntegrationPointsSwarm*		swarm;
	double				temp;

	cf = stgMainInitFromXML( "Geothermal/Base/input/testSourceTerms_Conductivity1.xml", MPI_COMM_WORLD, NULL );
	stgMainBuildAndInitialise( cf );

	swarm = (IntegrationPointsSwarm*)LiveComponentRegister_Get( cf->LCRegister, "picIntegrationPoints" );
	tempField = (FeVariable*)LiveComponentRegister_Get( cf->LCRegister, "TemperatureField" );

	stgMainLoop( cf );

	temp = IntegrateField( tempField, swarm );
	pcu_check_true( fabs( temp - TV_3 ) < TOLERANCE );
	
	stgMainDestroy( cf );
}

void SourceTermsSuite( pcu_suite_t* suite ) {
   pcu_suite_setData( suite, SourceTermsSuiteData );
   pcu_suite_setFixtures( suite, SourceTermsSuite_Setup, SourceTermsSuite_Teardown );
   pcu_suite_addTest( suite, SourceTermsSuite_TestHeatFlux );
   pcu_suite_addTest( suite, SourceTermsSuite_TestHeatProduction );
   pcu_suite_addTest( suite, SourceTermsSuite_TestConductivity1 );
}
