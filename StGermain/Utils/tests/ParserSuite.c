#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "pcu/pcu.h"
#include <StGermain/StGermain.h>

/* silly stgermain, I must define this */
#define CURR_MODULE_NAME "ParserSuite"

#define ParserSuite_Power_Tolerance  1e-14
#define ParserSuite_Number_Tolerance  1e-10

typedef struct {
} ParserSuiteData;

void ParserSuite_Setup( ParserSuiteData* data ) { 
}

void ParserSuite_Teardown( ParserSuiteData* data ) {
}


Bool ParserSuite_Equal( UnitDefinition_Type a, UnitDefinition_Type b  ) {
  return (Bool)( fabs( a.lengPower - b.lengPower ) < ParserSuite_Power_Tolerance &&
			 fabs( a.massPower - b.massPower ) < ParserSuite_Power_Tolerance &&
			 fabs( a.timePower - b.timePower ) < ParserSuite_Power_Tolerance &&
			 fabs( a.tempPower - b.tempPower ) < ParserSuite_Power_Tolerance &&
			 fabs( a.currPower - b.currPower ) < ParserSuite_Power_Tolerance );
}

void ParserSuite_TestPref( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  double expected;
  
  pcu_docstring( "This test parses all prefixes defined in the unit-parser");
  
  /* yocto */
  sprintf( str, "ym" );
  expected = 1e-24;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* zepto */
  sprintf( str, "zm" );
  expected = 1e-21;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* atto */
  sprintf( str, "am" );
  expected = 1e-18;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* femto */
  sprintf( str, "fm" );
  expected = 1e-15;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* pico */
  sprintf( str, "pm" );
  expected = 1e-12;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* nano */
  sprintf( str, "nm" );
  expected = 1e-9;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* micro */
  sprintf( str, "um" );
  expected = 1e-6;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* milli */
  sprintf( str, "mm" );
  expected = 1e-3;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* centi */
  sprintf( str, "cm" );
  expected = 1e-2;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* deci */
  sprintf( str, "dm" );
  expected = 1e-1;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* deca */
  sprintf( str, "em" );
  expected = 10;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* hecto */
  sprintf( str, "hm" );
  expected = 1e2;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* kilo */
  sprintf( str, "km" );
  expected = 1e3;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* mega */
  sprintf( str, "Mm" );
  expected = 1e6;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* giga */
  sprintf( str, "Gm" );
  expected = 1e9;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* tera */
  sprintf( str, "Tm" );
  expected = 1e12;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* peta */
  sprintf( str, "Pm" );
  expected = 1e15;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* exa */
  sprintf( str, "Em" );
  expected = 1e18;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* zetta */
  sprintf( str, "Zm" );
  expected = 1e21;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* yotta */
  sprintf( str, "Ym" );
  expected = 1e24;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
}


void ParserSuite_TestLeng( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  double expected;
  UnitDefinition_Type expectedUnit = {1, 0, 0, 0, 0, 1};
  
  pcu_docstring( "This test parses all length units defined in the unit-parser");
  
  /* meter */
  sprintf( str, "m" );
  expected = 1;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* angstrom */
  sprintf( str, "angstrom" );
  expected = 1e-10;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* astronomical unit */
  sprintf( str, "astrUnit" );
  expected = 149597870690.0;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* feet */
  sprintf( str, "feet" );
  expected = 0.3048;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* foot */
  sprintf( str, "foot" );
  expected = 0.3048;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  /* inch */
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  sprintf( str, "inch" );
  expected = 0.0254;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* mile */
  sprintf( str, "mile" );
  expected = 1609.344;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* light year */
  sprintf( str, "ly" );
  expected = 9454254955500000.0;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* parsec */
  sprintf( str, "parsec" );
  expected = 30856774879000000.0;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
}

void ParserSuite_TestTime( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  double expected;
  UnitDefinition_Type expectedUnit = {0, 0, 1, 0, 0, 1};
  
  pcu_docstring( "This test parses all time units defined in the unit-parser");
  
  /* second */
  sprintf( str, "s" );
  expected = 1;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* minute */
  sprintf( str, "min" );
  expected = res.c * 60;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* hour */
  sprintf( str, "h" );
  expected = res.c * 60;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* day */
  sprintf( str, "day" );
  expected = res.c * 24;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* week */
  sprintf( str, "week" );
  expected = res.c * 7;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* fortnight */
  sprintf( str, "fortnight" );
  expected = res.c * 2;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* month */
  sprintf( str, "month" );
  expected = 2628000;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* year */
  sprintf( str, "yr" );
  expected = res.c * 12;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* decade */
  sprintf( str, "decade" );
  expected = res.c * 10;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* century */
  sprintf( str, "century" );
  expected = res.c * 10;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* millennium */
  sprintf( str, "millennium" );
  expected = res.c * 10;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
}


void ParserSuite_TestMass( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  double expected;
  UnitDefinition_Type expectedUnit = {0, 1, 0, 0, 0, 1};
  
  pcu_docstring( "This test parses all mass units defined in the unit-parser");
  
  /* gram */
  sprintf( str, "g" );
  expected = 1e-3;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* atomic mass unnit */
  sprintf( str, "AtMaUnit" );
  expected = 1.66053873e-27;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* ounce */
  sprintf( str, "ounce" );
  expected = 0.028349523125;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* pound */
  sprintf( str, "pound" );
  expected = 0.45359237;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* ton */
  sprintf( str, "ton" );
  expected = 1000;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* tonne */
  sprintf( str, "tonne" );
  expected = 1000;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
}


void ParserSuite_TestTemp( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  double expected;
  UnitDefinition_Type expectedUnit = {0, 0, 0, 1, 0, 1};
  
  pcu_docstring( "This test parses all temperature units defined in the unit-parser");
  
  /* kelvin */
  sprintf( str, "K" );
  expected = 1;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
}
 

void ParserSuite_TestElCu( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  double expected;
  UnitDefinition_Type expectedUnit = {0, 0, 0, 0, 1, 1};
  
  pcu_docstring( "This test parses all electric current units defined in the unit-parser");
  
  /* ampere */
  sprintf( str, "A" );
  expected = 1;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
}


void ParserSuite_TestAcce( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  double expected;
  UnitDefinition_Type expectedUnit = {1, 0,-2, 0, 0, 1};
  
  pcu_docstring( "This test parses all acceleration units defined in the unit-parser");
  
  /* G */
  sprintf( str, "G" );
  expected = 9.80665;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* gal */
  sprintf( str, "gal" );
  expected = 0.01;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
}


void ParserSuite_TestEner( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  double expected;
  UnitDefinition_Type expectedUnit = {2, 1,-2, 0, 0, 1};
  
  pcu_docstring( "This test parses all energy units defined in the unit-parser");
  
  /* Joule */
  sprintf( str, "J" );
  expected = 1.0;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* calorie */
  sprintf( str, "cal" );
  expected = 4.184;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* erg */
  sprintf( str, "erg" );
  expected = 1e-7;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
}


void ParserSuite_TestForc( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  double expected;
  UnitDefinition_Type expectedUnit = {1, 1, -2, 0, 0, 1};
  
  pcu_docstring( "This test parses all force units defined in the unit-parser");
  
  /* newton */
  sprintf( str, "N" );
  expected = 1.0;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* dyne */
  sprintf( str, "dyne" );
  expected = 0.00001;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
}


void ParserSuite_TestFreq( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  double expected;
  UnitDefinition_Type expectedUnit = {0, 0,-1, 0, 0, 1};
  
  pcu_docstring( "This test parses all frequency units defined in the unit-parser");
  
  /* hertz */
  sprintf( str, "Hz" );
  expected = 1;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
}


void ParserSuite_TestPowe( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  double expected;
  UnitDefinition_Type expectedUnit = {2, 1,-3, 0, 0, 1};
  
  pcu_docstring( "This test parses all power units defined in the unit-parser");
  
  /* watt */
  sprintf( str, "W" );
  expected = 1;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
}


void ParserSuite_TestPres( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  double expected;
  UnitDefinition_Type expectedUnit = {-1, 1,-2, 0, 0, 1};
  
  pcu_docstring( "This test parses all pressure units defined in the unit-parser");
  
  /* pascal */
  sprintf( str, "Pa" );
  expected = 1;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* atmosphere */
  sprintf( str, "atm" );
  expected = 101325.01;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* bar */
  sprintf( str, "bar" );
  expected = 1e5;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* cm of hg */
  sprintf( str, "cmHg" );
  expected = 1333.2239;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
  /* mm of hg */
  sprintf( str, "mmHg" );
  expected = 133.32239;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
}


void ParserSuite_TestSubs( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  double expected;
  UnitDefinition_Type expectedUnit = {0, 0, 0, 0, 0, 1};
  
  pcu_docstring( "This test parses all amount of substance units defined in the unit-parser");
  
  /* mole */
  sprintf( str, "mol" );
  expected = 1;
  res = Scaling_Parse( str );
  pcu_check_true( fabs( res.c - expected ) < ParserSuite_Number_Tolerance );
  pcu_check_true( ParserSuite_Equal( res, expectedUnit ) );
}


void ParserSuite_TestMultiplication( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  UnitDefinition_Type expected;
  
  pcu_docstring( "This test parses the multiplication of some units defined in the unit-parser");
  
  /* test 1 */
  sprintf( str, "m*kg*s*K*A" );
  expected = (UnitDefinition_Type){ 1, 1, 1, 1, 1, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 2 */
  sprintf( str, "s*Hz" );
  expected = (UnitDefinition_Type){ 0, 0, 0, 0, 0, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 3 */
  sprintf( str, "W*N*Pa*J" );
  expected = (UnitDefinition_Type){ 4, 4,-9, 0, 0, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 4 */
  sprintf( str, "km*mm" );
  expected = (UnitDefinition_Type){ 2, 0, 0, 0, 0, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
}


void ParserSuite_TestDivision( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  UnitDefinition_Type expected;
  
  pcu_docstring( "This test parses the division of some units defined in the unit-parser");
  
  /* test 1 */
  sprintf( str, "m/kg/s/K/A" );
  expected = (UnitDefinition_Type){ 1,-1,-1,-1,-1, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 2 */
  sprintf( str, "s/Hz" );
  expected = (UnitDefinition_Type){ 0, 0, 2, 0, 0, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 3 */
  sprintf( str, "W/N/Pa/J" );
  expected = (UnitDefinition_Type){ 0,-2, 3, 0, 0, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 4 */
  sprintf( str, "km/mm" );
  expected = (UnitDefinition_Type){ 0, 0, 0, 0, 0, 1e6};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
}


void ParserSuite_TestPower( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  UnitDefinition_Type expected;
  
  pcu_docstring( "This test parses the power of some units defined in the unit-parser");
  
  /* test 1 */
  sprintf( str, "kg^1" );
  expected = (UnitDefinition_Type){ 0, 1, 0, 0, 0, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 2 */
  sprintf( str, "m^2" );
  expected = (UnitDefinition_Type){ 2, 0, 0, 0, 0, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 3 */
  sprintf( str, "K^-1" );
  expected = (UnitDefinition_Type){ 0, 0, 0,-1, 0, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 4 */
  sprintf( str, "s^-2" );
  expected = (UnitDefinition_Type){ 0, 0,-2, 0, 0, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 5 */
  sprintf( str, "W^5" );
  expected = (UnitDefinition_Type){10, 5,-15, 0, 0, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 6 */
  sprintf( str, "m^1.5" );
  expected = (UnitDefinition_Type){1.5, 0, 0, 0, 0, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 7 */
  sprintf( str, "K^-1.5" );
  expected = (UnitDefinition_Type){ 0, 0, 0,-1.5, 0, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 8 */
  sprintf( str, "km^2" );
  expected = (UnitDefinition_Type){ 2, 0, 0, 0, 0, 1e6};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 8 */
  sprintf( str, "mm^-2" );
  expected = (UnitDefinition_Type){ -2, 0, 0, 0, 0, 1e6};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
}


void ParserSuite_TestBrackets( ParserSuiteData* data ) {
  char str[512];
  UnitDefinition_Type res;
  UnitDefinition_Type expected;
  
  pcu_docstring( "This test parses expresion involving brackets of some units defined in the unit-parser");
  
  /* test 1 */
  sprintf( str, "m/(s*K)" );
  expected = (UnitDefinition_Type){ 1, 0,-1,-1, 0, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 2 */
  sprintf( str, "A/(s/m)" );
  expected = (UnitDefinition_Type){ 1, 0,-1, 0, 1, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 3 */
  sprintf( str, "(kg*min)^2" );
  expected = (UnitDefinition_Type){ 0, 2, 2, 0, 0, 60*60};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
  /* test 4 */
  sprintf( str, "(m*s)^3/(kg*A)" );
  expected = (UnitDefinition_Type){ 3, -1, 3, 0,-1, 1};
  res = Scaling_Parse( str );
  pcu_check_true( res.c == expected.c );
  pcu_check_true( ParserSuite_Equal( res, expected ) );
}


void ParserSuite( pcu_suite_t* suite ) {
   pcu_suite_setData( suite, ParserSuiteData );
   pcu_suite_setFixtures( suite, ParserSuite_Setup, ParserSuite_Teardown );
   pcu_suite_addTest( suite, ParserSuite_TestPref );
   pcu_suite_addTest( suite, ParserSuite_TestLeng );
   pcu_suite_addTest( suite, ParserSuite_TestTime );
   pcu_suite_addTest( suite, ParserSuite_TestMass );
   pcu_suite_addTest( suite, ParserSuite_TestTemp );
   pcu_suite_addTest( suite, ParserSuite_TestElCu );
   pcu_suite_addTest( suite, ParserSuite_TestAcce );
   pcu_suite_addTest( suite, ParserSuite_TestEner );
   pcu_suite_addTest( suite, ParserSuite_TestForc );
   pcu_suite_addTest( suite, ParserSuite_TestFreq );
   pcu_suite_addTest( suite, ParserSuite_TestPowe );
   pcu_suite_addTest( suite, ParserSuite_TestPres );
   pcu_suite_addTest( suite, ParserSuite_TestMultiplication );
   pcu_suite_addTest( suite, ParserSuite_TestDivision );
   pcu_suite_addTest( suite, ParserSuite_TestPower );
   pcu_suite_addTest( suite, ParserSuite_TestBrackets );
}


