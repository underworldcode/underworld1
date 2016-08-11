%{
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "u.h"

uyyType theOutput;
%}

%token TOKEN_UNIT
%token TOKEN_NUMBER

%left '*' '/'

%%
start:  exp                {
                             uyyType i;
                             i = $1;
									  /*printf( "\n\tleng: %f\n\tmass: %f\n\ttime: %f\n\ttemp: %f\n\tcurr: %f\n\ttoSI: %g\n", i.pl, i.pm, i.pt, i.pT, i.pA, i.c );*/
									  theOutput = i;
                           }
        ;

exp:    /* empty */        {
                             uyyType o;
                             o.pl = 0;
                             o.pm = 0;
                             o.pt = 0;
                             o.pT = 0;
                             o.pA = 0;
                             o.c = 1;
									  $$ = o;
                           }
        | units            {
									  $$ = $1;
                           }
        ;

units:  units '*' units    {
                             uyyType o,i1,i3;
                             i1 = $1;
                             i3 = $3;
                             o.pl = i1.pl + i3.pl;
                             o.pm = i1.pm + i3.pm;
                             o.pt = i1.pt + i3.pt;
                             o.pT = i1.pT + i3.pT;
                             o.pA = i1.pA + i3.pA;
                             o.c = i1.c * i3.c;
									  $$ = o;
                           }
        | units '/' units  {
                             uyyType o,i1,i3;
                             i1 = $1;
                             i3 = $3;
                             o.pl = i1.pl - i3.pl;
                             o.pm = i1.pm - i3.pm;
                             o.pt = i1.pt - i3.pt;
                             o.pT = i1.pT - i3.pT;
                             o.pA = i1.pA - i3.pA;
                             o.c = i1.c / i3.c;
									  $$ = o;
                           }
        | units '^' number {
                             uyyType o,i1,i3;
                             i1 = $1;
                             i3 = $3;
                             o.pl = i1.pl * i3.c;
                             o.pm = i1.pm * i3.c;
                             o.pt = i1.pt * i3.c;
                             o.pT = i1.pT * i3.c;
                             o.pA = i1.pA * i3.c;
                             o.c = pow( i1.c, i3.c );
									  $$ = o;
                           }
        | '(' units ')'    {
                             uyyType o,i2;
                             i2 = $2;
                             o.pl = i2.pl;
                             o.pm = i2.pm;
                             o.pt = i2.pt;
                             o.pT = i2.pT;
                             o.pA = i2.pA;
                             o.c = i2.c;
									  $$ = o;
                           }
        | unit             {
                             uyyType o,i;
                             i = $1;
                             o.pl = i.pl;
                             o.pm = i.pm;
                             o.pt = i.pt;
                             o.pT = i.pT;
                             o.pA = i.pA;
                             o.c = i.c;
									  $$ = o;
                           }
        ;

unit:   TOKEN_UNIT         {
                             uyyType o,i;
                             i = $1;
                             o.pl = i.pl;
                             o.pm = i.pm;
                             o.pt = i.pt;
                             o.pT = i.pT;
                             o.pA = i.pA;
                             o.c = i.c;
									  $$ = o;
                           }
        ;

number: TOKEN_NUMBER       {
                             uyyType o,i;
                             i = $1;
                             o.pl = 0;
                             o.pm = 0;
                             o.pt = 0;
                             o.pT = 0;
                             o.pA = 0;
                             o.c = i.c;
									  $$ = o;
                           }
        ;

%%

/*
int main() {
  return yyparse();
}
*/

int yyerror(char* errstr) {
	 printf("Error: %s\n", errstr);
	 return -1;
}




