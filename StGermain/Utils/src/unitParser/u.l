
%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "u.h"
#include "u.tab.h"

//#define DEBUG_UNIT_PARSER

extern uyyType theOutput;

double preVal( char* yytext, int yylen );
double lenVal( char* yytext, int yylen );
double timVal( char* yytext, int yylen );
double masVal( char* yytext, int yylen );
double temVal( char* yytext, int yylen );
double curVal( char* yytext, int yylen );
double accVal( char* yytext, int yylen );
double eneVal( char* yytext, int yylen );
double forVal( char* yytext, int yylen );
double freVal( char* yytext, int yylen );
double powVal( char* yytext, int yylen );
double prsVal( char* yytext, int yylen );
double molVal( char* yytext, int yylen );
%}

digit    [0-9]
integer  {digit}+
exponent [eE][+-]?{integer}
real     [+-]?{integer}("."{integer})?{exponent}?


 /* Prefix  Symbol  Value
    ------  ------  -----
    yotta    Y      1e+24
    zetta    Z      1e+21
    exa      E      1e+18
    peta     P      1e+15
    tera     T      1e+12
    giga     G      1e+09 
    mega     M      1e+06 
    kilo     k      1e+03 
    hecto    h      1e+02 
    deca     e      1e+01 
    deci     d      1e−01
    centi    c      1e−02
    milli    m      1e−03 
    micro    u      1e−06 
    nano     n      1e−09 
    pico     p      1e−12 
    femto    f      1e−15 
    atto     a      1e−18 
    zepto    z      1e−21 
    yocto    y      1e−24 
  */
prefix  y|z|a|f|p|n|u|m|c|d|e|h|k|M|G|T|P|E|Z|Y



  /* Length_units  Symbol    conversion_to_SI
     ------------  ------    ----------------
     meter         m         1.0   
     angstrom      angstrom  1e-10
     astron.unit   astrUnit 149597870690
     feet          feet      0.3048
     foot          foot      0.3048
     inch          inch      0.0254     
     mile          mile      1609.344
     light year    ly        9454254955500000
     parsec        parsec    30856774879000000
  */
length_unit  m|angstrom|astrUnit|feet|foot|inch|mile|ly|parsec



  /* Time_units    Symbol     conversion_to_SI
     ------------  ------     ----------------
     seconds        s                     1
     minute         min                  60
     hour           h                  3600
     day            day               86400 
     week           week             604800
     fortnight      fortnight       1209600
     month          month           2628000
     year           yr             31536000
     decade         decade        315360000
     century        century      3153600000
     millennium     millennium  31536000000
  */
time_unit  s|min|h|day|week|fortnight|month|yr|decade|century|millennium



  /* Mass_units     Symbol     conversion_to_SI
     -------------  ---------  ----------------
     gram           g          1e-3
     atomic mass u. AtMaUnit   1.66053873e-27
     ounce          ounce      0.028349523125 
     pound          pound      0.45359237
     ton            ton        1000.0
     tonne          tonne      1000.0     
  */
mass_unit  g|AtMaUnit|ounce|pound|ton|tonne



  /* Temp_units     Symbol     conversion_to_SI
     -------------  ---------  ----------------
     kelvin         K          1.0
  */
temperature_unit  K



  /* Electric_
     current_units  Symbol     conversion_to_SI
     -------------  ---------  ----------------
     Ampere           A          1.0
  */
electric_current_unit  A



  /* Acceleration_units  Symbol  conversion_to_SI
     ------------------  ------  ----------------
     g-unit              G   9.80665
     gal                 gal     0.01
  */
acceleration_unit  G|gal  



  /* Energy_units        Symbol  conversion_to_SI
     ------------------  ------  ----------------
     Joule               J       1.0
     calorie             cal     4.184
     erg                 erg     1e-7     
  */
energy_unit J|cal|erg
  


  /* Force_units         Symbol  conversion_to_SI
     ------------------  ------  ----------------
     Newton              N       1.0
     dyne                dyne    0.00001     
  */
force_unit N|dyne
  


  /* Frequency_units     Symbol  conversion_to_SI
     ------------------  ------  ----------------
     Hertz               Hz      1.0     
  */
frequency_unit Hz



  /* Power_units         Symbol  conversion_to_SI
     ------------------  ------  ----------------
     Watt                W       1.0
  */
power_unit W



  /* Pressure_units      Symbol  conversion_to_SI
     ------------------  ------  ----------------
     Pascal              Pa           1.0
     atmosphere          atm     101325.01
     bar                 bar     100000.0
     cm of hg            cmHg      1333.2239
     mm of hg            mmHg       133.32239     
  */
pressure_unit Pa|atm|bar|cmHg|mmHg



  /* Amount_subs_units   Symbol  conversion_to_SI
     ------------------  ------  ----------------
     mole                mol     1.0
  */
substance_unit mol
    

%%

{real}      { 
              yylval.c = atof(yytext); 
              return TOKEN_NUMBER; 
            }

{length_unit}  { 
              yylval.pl = 1;
              yylval.pm = 0; 
              yylval.pt = 0; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = lenVal( yytext, yyleng );
              return TOKEN_UNIT; 
            }

{time_unit}    { 
              yylval.pl = 0;
              yylval.pm = 0; 
              yylval.pt = 1; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = timVal( yytext, yyleng );
              return TOKEN_UNIT; 
            }

{mass_unit}    { 
              yylval.pl = 0;
              yylval.pm = 1; 
              yylval.pt = 0; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = masVal( yytext, yyleng ); 
              return TOKEN_UNIT; 
            }

{temperature_unit}    { 
              yylval.pl = 0;
              yylval.pm = 0; 
              yylval.pt = 0; 
              yylval.pT = 1; 
              yylval.pA = 0; 
              yylval.c = temVal( yytext, yyleng ); 
              return TOKEN_UNIT; 
            }

{electric_current_unit}  { 
              yylval.pl = 0;
              yylval.pm = 0; 
              yylval.pt = 0; 
              yylval.pT = 0; 
              yylval.pA = 1; 
              yylval.c = curVal( yytext, yyleng ); 
              return TOKEN_UNIT; 
            }

{acceleration_unit}  { 
              yylval.pl = 1;
              yylval.pm = 0; 
              yylval.pt = -2; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = accVal( yytext, yyleng ); 
              return TOKEN_UNIT; 
            }

{energy_unit}  { 
              yylval.pl = 2;
              yylval.pm = 1; 
              yylval.pt = -2; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = eneVal( yytext, yyleng ); 
              return TOKEN_UNIT; 
            }

{force_unit}  { 
              yylval.pl = 1;
              yylval.pm = 1; 
              yylval.pt = -2; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = forVal( yytext, yyleng ); 
              return TOKEN_UNIT; 
            }

{frequency_unit}  { 
              yylval.pl = 0;
              yylval.pm = 0; 
              yylval.pt = -1; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = freVal( yytext, yyleng ); 
              return TOKEN_UNIT; 
            }

{power_unit}  { 
              yylval.pl = 2;
              yylval.pm = 1; 
              yylval.pt = -3; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = powVal( yytext, yyleng ); 
              return TOKEN_UNIT; 
            }

{pressure_unit}  { 
              yylval.pl = -1;
              yylval.pm = 1; 
              yylval.pt = -2; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = prsVal( yytext, yyleng ); 
              return TOKEN_UNIT; 
            }

{substance_unit}  { 
              yylval.pl = 0;
              yylval.pm = 0; 
              yylval.pt = 0; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = molVal( yytext, yyleng ); 
              return TOKEN_UNIT; 
            }

({prefix}{length_unit}) { 
              yylval.pl = 1;
              yylval.pm = 0; 
              yylval.pt = 0; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = preVal( yytext, yyleng );
              yylval.c *= lenVal( yytext+1, yyleng-1 );
              return TOKEN_UNIT; 
            }

({prefix}{time_unit}) { 
              yylval.pl = 0;
              yylval.pm = 0; 
              yylval.pt = 1; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = preVal( yytext, yyleng );
              yylval.c *= timVal( yytext+1, yyleng-1 );
              return TOKEN_UNIT; 
            }

({prefix}{mass_unit}) { 
              yylval.pl = 0;
              yylval.pm = 1; 
              yylval.pt = 0; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = preVal( yytext, yyleng );
              yylval.c *= masVal( yytext+1, yyleng-1 );
              return TOKEN_UNIT; 
            }

({prefix}{temperature_unit}) { 
              yylval.pl = 0;
              yylval.pm = 0; 
              yylval.pt = 0; 
              yylval.pT = 1; 
              yylval.pA = 0; 
              yylval.c = preVal( yytext, yyleng );
              yylval.c *= temVal( yytext+1, yyleng-1 );
              return TOKEN_UNIT; 
            }

({prefix}{electric_current_unit}) { 
              yylval.pl = 0;
              yylval.pm = 0; 
              yylval.pt = 0; 
              yylval.pT = 0; 
              yylval.pA = 1; 
              yylval.c = preVal( yytext, yyleng );
              yylval.c *= curVal( yytext+1, yyleng-1 );
              return TOKEN_UNIT; 
            }

({prefix}{acceleration_unit}) { 
              yylval.pl = 1;
              yylval.pm = 0; 
              yylval.pt = -2; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = preVal( yytext, yyleng );
              yylval.c *= accVal( yytext+1, yyleng-1 );
              return TOKEN_UNIT; 
            }

({prefix}{energy_unit}) { 
              yylval.pl = 2;
              yylval.pm = 1; 
              yylval.pt = -2; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = preVal( yytext, yyleng );
              yylval.c *= eneVal( yytext+1, yyleng-1 );
              return TOKEN_UNIT; 
            }

({prefix}{force_unit}) { 
              yylval.pl = 1;
              yylval.pm = 1; 
              yylval.pt = -2; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = preVal( yytext, yyleng );
              yylval.c *= forVal( yytext+1, yyleng-1 );
              return TOKEN_UNIT; 
            }

({prefix}{frequency_unit}) { 
              yylval.pl = 0;
              yylval.pm = 0; 
              yylval.pt = -1; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = preVal( yytext, yyleng );
              yylval.c *= freVal( yytext+1, yyleng-1 );
              return TOKEN_UNIT; 
            }

({prefix}{power_unit}) { 
              yylval.pl = 2;
              yylval.pm = 1; 
              yylval.pt = -3; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = preVal( yytext, yyleng );
              yylval.c *= powVal( yytext+1, yyleng-1 );
              return TOKEN_UNIT; 
            }

({prefix}{pressure_unit}) { 
              yylval.pl = -1;
              yylval.pm = 1; 
              yylval.pt = -2; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = preVal( yytext, yyleng );
              yylval.c *= prsVal( yytext+1, yyleng-1 );
              return TOKEN_UNIT; 
            }

({prefix}{substance_unit}) { 
              yylval.pl = 0;
              yylval.pm = 0; 
              yylval.pt = 0; 
              yylval.pT = 0; 
              yylval.pA = 0; 
              yylval.c = preVal( yytext, yyleng );
              yylval.c *= molVal( yytext+1, yyleng-1 );
              return TOKEN_UNIT; 
            }


[*/^)(]       { return *yytext; }

[ \t]       ; /* skip whitespace */

.           yyerror( "invalid character" );

%%

int yywrap(void) {  return 1; } 

double preVal( char* yytext, int yylen ) {
  double p = 1;
  if( yylen > 1 ) {
     switch( yytext[0] ) {
       case 'Y': p = 1e+24; break;
       case 'Z': p = 1e+21; break;
       case 'E': p = 1e+18; break;
       case 'P': p = 1e+15; break;
       case 'T': p = 1e+12; break;
       case 'G': p = 1e+09; break;
       case 'M': p = 1e+06; break;
       case 'k': p = 1e+03; break;
       case 'h': p = 1e+02; break;
       case 'e': p = 1e+01; break;
       case 'd': p = 1e-01; break;
       case 'c': p = 1e-02; break;
       case 'm': p = 1e-03; break;
       case 'u': p = 1e-06; break;
       case 'n': p = 1e-09; break;
       case 'p': p = 1e-12; break;
       case 'f': p = 1e-15; break;
       case 'a': p = 1e-18; break;
       case 'z': p = 1e-21; break;
       case 'y': p = 1e-24; break;
     }
  }
#ifdef DEBUG_UNIT_PARSER
  printf("prefix %c value: %g\n", yytext[0], p );
#endif
  return p;
}

double lenVal( char* yytext, int yylen ) {
  double l = 1;
  if( strcmp( yytext, "m" ) == 0 )               l = 1.0;
  else if( strcmp( yytext, "angstrom" ) == 0 )   l = 1e-10;
  else if( strcmp( yytext, "astrUnit" ) == 0 )   l = 149597870690.0;
  else if( strcmp( yytext, "feet" ) == 0 )       l = 0.3048;
  else if( strcmp( yytext, "foot" ) == 0 )       l = 0.3048;
  else if( strcmp( yytext, "inch" ) == 0 )       l = 0.0254;
  else if( strcmp( yytext, "mile" ) == 0 )       l = 1609.344;
  else if( strcmp( yytext, "ly" ) == 0 )         l = 9454254955500000.0;
  else if( strcmp( yytext, "parsec" ) == 0 )     l = 30856774879000000.0;
#ifdef DEBUG_UNIT_PARSER
  printf("length %s value: %g\n", yytext, l );
#endif
  return l;
}


double timVal( char* yytext, int yylen ) {
  double t = 1.0;
  if( strcmp( yytext, "s" ) == 0 )               t = 1.0;
  else if( strcmp( yytext, "min" ) == 0 )        t = 60.0;
  else if( strcmp( yytext, "h" ) == 0 )          t = 3600.0;
  else if( strcmp( yytext, "day" ) == 0 )        t = 86400.0;
  else if( strcmp( yytext, "week" ) == 0 )       t = 604800.0;
  else if( strcmp( yytext, "fortnight" ) == 0 )  t = 1209600.0;
  else if( strcmp( yytext, "month" ) == 0 )      t = 2628000.0;
  else if( strcmp( yytext, "yr" ) == 0 )         t = 31536000.0;
  else if( strcmp( yytext, "decade" ) == 0 )     t = 315360000.0;
  else if( strcmp( yytext, "century" ) == 0 )    t = 3153600000.0;
  else if( strcmp( yytext, "millennium" ) == 0 ) t = 31536000000.0;
#ifdef DEBUG_UNIT_PARSER
  printf("time %s value: %g\n", yytext, t );
#endif
  return t;
}

double masVal( char* yytext, int yylen ) {
  double m = 1.0;
  if( strcmp( yytext, "g" ) == 0 )               m = 1.0e-3;
  else if( strcmp( yytext, "AtMaUnit" ) == 0 )   m = 1.66053873e-27;
  else if( strcmp( yytext, "ounce" ) == 0 )      m = 0.028349523125;
  else if( strcmp( yytext, "pound" ) == 0 )      m = 0.45359237;
  else if( strcmp( yytext, "ton" ) == 0 )        m = 1000.0;
  else if( strcmp( yytext, "tonne" ) == 0 )      m = 1000.0;
#ifdef DEBUG_UNIT_PARSER
  printf("mass %s value: %g\n", yytext, m );
#endif
  return m;
}

double temVal( char* yytext, int yylen ) {
  double t = 1.0;
  if( strcmp( yytext, "K" ) == 0 )               t = 1.0;
#ifdef DEBUG_UNIT_PARSER
  printf("temp %s value: %g\n", yytext, t );
#endif
  return t;
}

double curVal( char* yytext, int yylen ) {
  double t = 1.0;
  if( strcmp( yytext, "A" ) == 0 )               t = 1.0;
#ifdef DEBUG_UNIT_PARSER
  printf("e.c. %s value: %g\n", yytext, t );
#endif
  return t;
}

double accVal( char* yytext, int yylen ) {
  double t = 1.0;
  if( strcmp( yytext, "G" ) == 0 )               t = 9.80665;
  else if( strcmp( yytext, "gal" ) == 0 )        t = 0.01;
#ifdef DEBUG_UNIT_PARSER
  printf("acc. %s value: %g\n", yytext, t );
#endif
  return t;
}

double eneVal( char* yytext, int yylen ) {
  double t = 1.0;
  if( strcmp( yytext, "J" ) == 0 )               t = 1.0;
  else if( strcmp( yytext, "cal" ) == 0 )        t = 4.184;
  else if( strcmp( yytext, "erg" ) == 0 )        t = 1e-7;
#ifdef DEBUG_UNIT_PARSER
  printf("ener %s value: %g\n", yytext, t );
#endif
  return t;
}

double forVal( char* yytext, int yylen ) {
  double t = 1.0;
  if( strcmp( yytext, "N" ) == 0 )               t = 1.0;
  else if( strcmp( yytext, "dyne" ) == 0 )       t = 0.00001;
#ifdef DEBUG_UNIT_PARSER
  printf("forc %s value: %g\n", yytext, t );
#endif
  return t;
}

double freVal( char* yytext, int yylen ) {
  double t = 1.0;
  if( strcmp( yytext, "Hz" ) == 0 )              t = 1.0;
#ifdef DEBUG_UNIT_PARSER
  printf("freq %s value: %g\n", yytext, t );
#endif
  return t;
}

double powVal( char* yytext, int yylen ) {
  double t = 1.0;
  if( strcmp( yytext, "W" ) == 0 )              t = 1.0;
#ifdef DEBUG_UNIT_PARSER
  printf("pow. %s value: %g\n", yytext, t );
#endif
  return t;
}

double prsVal( char* yytext, int yylen ) {
  double t = 1.0;
  if( strcmp( yytext, "Pa" ) == 0 )              t = 1.0;
  else if( strcmp( yytext, "atm" ) == 0 )        t = 101325.01;
  else if( strcmp( yytext, "bar" ) == 0 )        t = 100000.0;
  else if( strcmp( yytext, "cmHg" ) == 0 )       t = 1333.2239;
  else if( strcmp( yytext, "mmHg" ) == 0 )       t = 133.32239;
#ifdef DEBUG_UNIT_PARSER
  printf("press %s value: %g\n", yytext, t );
#endif
  return t;
}

double molVal( char* yytext, int yylen ) {
  double t = 1.0;
  if( strcmp( yytext, "mol" ) == 0 )              t = 1.0;
#ifdef DEBUG_UNIT_PARSER
  printf("a.s. %s value: %g\n", yytext, t );
#endif
  return t;
}


/* int main() */
int myParser( char* buff, uyyType* out )
{
  YY_BUFFER_STATE str_buffer = NULL;
  int err;

  /* printf("main: scaning buf:\n%s\nOutput:",buff); */

  str_buffer = yy_scan_string( buff );
  err = yyparse();

  out[0] = theOutput;

/*  printf("err: %i\n",err);
  printf( "\n\tleng: %f\n\tmass: %f\n\ttime: %f\n\ttemp: %f\n\tcurr: %f\n\ttoSI: %g\n", 
			 theOutput.pl, theOutput.pm, theOutput.pt, theOutput.pT, theOutput.pA, theOutput.c );
*/  
  
  yy_delete_buffer(str_buffer);

  return err;
}


