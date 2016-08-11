#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pcu/pcu.h"
#include <StGermain/StGermain.h>

/* silly stgermain, I must define this */
#define CURR_MODULE_NAME "ScalingSuite"

#define ScalingSuite_Number_Tolerance  1e-10

typedef struct {
} ScalingSuiteData;

void ScalingSuite_Setup( ScalingSuiteData* data ) { 
}

void ScalingSuite_Teardown( ScalingSuiteData* data ) {
}


Bool ScalingSuite_Equal( double a, double b  ) {
  return (Bool) ( fabs( a - b) < ScalingSuite_Number_Tolerance );
}

void ScalingSuite_Test( ScalingSuiteData* data ) {
   Scaling *scaling = NULL;
   double space = 2;
   double time = 3;
   double mass = 5;
   double temperature = 7;
   double ec = 9;
   Bool condition = False;

   scaling = Scaling_New( "theScaling",space,time,mass,temperature,ec);
   char str[512];
   double result, scalingCoeff;

   pcu_docstring( "Tests the scaling operations");

   /**
    * The scaling works as:
    *   dimensional_value = scalingCoeff * scaled_value
    */

   sprintf( str, "m/(s^2)" );
   result = Scaling_ParseDimCoeff( scaling, str );
   pcu_check_true( ScalingSuite_Equal( result, space/(time*time) ) ); 

   
   scalingCoeff = mass/(space*space*space);
   condition = ScalingSuite_Equal( Scaling_Scale( scaling, 3400, sDensity ), (3400/scalingCoeff) );
   pcu_check_true( condition );

   scalingCoeff = space/(time*time);
   condition = ScalingSuite_Equal( Scaling_Scale( scaling, 10.0, sAcceleration ), (10.0/scalingCoeff) );
   pcu_check_true( condition );

   scalingCoeff = space*space;
   condition = ScalingSuite_Equal( Scaling_Scale( scaling, 10.0, sArea ), (10.0/scalingCoeff) );
   pcu_check_true( condition );

   scalingCoeff = mass*space/(time*time);
   condition = ScalingSuite_Equal( Scaling_Scale( scaling, 9.81, sForce ), (9.81/scalingCoeff) );
   pcu_check_true( condition );
}


void ScalingSuite( pcu_suite_t* suite ) {
   pcu_suite_setData( suite, ScalingSuiteData );
   pcu_suite_setFixtures( suite, ScalingSuite_Setup, ScalingSuite_Teardown );
   
   pcu_suite_addTest( suite, ScalingSuite_Test );
}


