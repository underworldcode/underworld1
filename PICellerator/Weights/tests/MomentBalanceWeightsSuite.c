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
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pcu/pcu.h"
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include "ConstantWeightsSuite.h"
#include "MomentBalanceWeightsSuite.h"

typedef struct {
   PICelleratorContext*  context;
   Stg_ComponentFactory* cf;
} MomentBalanceWeightsSuiteData;

void MomentBalanceWeightsSuite_Setup( MomentBalanceWeightsSuiteData* data ) {
   char xmlInputFilename[PCU_PATH_MAX];

   pcu_filename_input( "testMomentBalanceWeights.xml", xmlInputFilename );
   data->cf = stgMainInitFromXML( xmlInputFilename, MPI_COMM_WORLD, NULL );
   data->context = (PICelleratorContext*) LiveComponentRegister_Get( data->cf->LCRegister, (Name)"context" );
   stgMainBuildAndInitialise( data->cf );
} 

void MomentBalanceWeightsSuite_Teardown( MomentBalanceWeightsSuiteData* data ) {
   stgMainDestroy( data->cf );
}

void MomentBalanceWeightsSuite_TestConstantFunction( MomentBalanceWeightsSuiteData* data  ) {
   WeightsSuite_TestElementIntegral( data->context, "ConstantFunction", 1000,
      1e-10, /* --mean-tolerance */
      1e-10, /* --standardDeviation-tolerance */
      0.0, /* --mean-expectedValue */
      0.0 /* --standardDeviation-expectedValue */ );
}
void MomentBalanceWeightsSuite_TestLinearFunction ( MomentBalanceWeightsSuiteData* data ) {
   WeightsSuite_TestElementIntegral( data->context, "LinearFunction", 1000,
      0.000001, /* --mean-tolerance */
      0.000001, /* --standardDeviation-tolerance */
      0.00363688, /* --mean-expectedValue */
      0.029866 /* --standardDeviation-expectedValue */ );
}
void MomentBalanceWeightsSuite_TestQuadraticFunction ( MomentBalanceWeightsSuiteData* data ) {
   WeightsSuite_TestElementIntegral( data->context, "QuadraticFunction", 1000,
      0.000001, /* --mean-tolerance */
      0.000001, /* --standardDeviation-tolerance */
      0.0446896, /* --mean-expectedValue */
      0.0351938 /* --standardDeviation-expectedValue */ );
}

void MomentBalanceWeightsSuite_TestPolynomialFunction( MomentBalanceWeightsSuiteData* data ) {
   WeightsSuite_TestElementIntegral( data->context, "PolynomialFunction", 1000,
      0.000001, /* --mean-tolerance */
      0.000001, /* --standardDeviation-tolerance */
      0.0166968, /* --mean-expectedValue */
      0.0130415 /* --standardDeviation-expectedValue */ );
}

void MomentBalanceWeightsSuite_TestCircleInterface( MomentBalanceWeightsSuiteData* data ) {
   WeightsSuite_TestElementIntegral( data->context, "CircleInterface", 1000,
      0.000001, /* --mean-tolerance */
      0.000001, /* --standardDeviation-tolerance */
      0.0978135, /* --mean-expectedValue */
      0.0686067 /* --standardDeviation-expectedValue */ );
}

void MomentBalanceWeightsSuite_TestExponentialInterface( MomentBalanceWeightsSuiteData* data ) {
   WeightsSuite_TestElementIntegral( data->context, "ExponentialInterface", 1000,
      0.000001, /* --mean-tolerance */
      0.000001, /* --standardDeviation-tolerance */
      0.0946707, /* --mean-expectedValue */
      0.0752869 /* --standardDeviation-expectedValue */ );
}

void MomentBalanceWeightsSuite( pcu_suite_t* suite ) {
   pcu_suite_setData( suite, MomentBalanceWeightsSuiteData );
   pcu_suite_setFixtures( suite, MomentBalanceWeightsSuite_Setup, MomentBalanceWeightsSuite_Teardown );
   pcu_suite_addTest( suite, MomentBalanceWeightsSuite_TestConstantFunction );
   pcu_suite_addTest( suite, MomentBalanceWeightsSuite_TestLinearFunction );
   pcu_suite_addTest( suite, MomentBalanceWeightsSuite_TestQuadraticFunction );
   pcu_suite_addTest( suite, MomentBalanceWeightsSuite_TestPolynomialFunction );
   pcu_suite_addTest( suite, MomentBalanceWeightsSuite_TestCircleInterface );
   pcu_suite_addTest( suite, MomentBalanceWeightsSuite_TestExponentialInterface );
}
