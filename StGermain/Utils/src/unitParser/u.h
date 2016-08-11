#ifndef u_h
#define u_h

typedef struct uyyTypeTag {
  double pl;
  double pm;
  double pt;
  double pT;
  double pA;
  double c;
} uyyType;

#define YYSTYPE uyyType


   typedef struct {
     double lengPower;
     double massPower;
     double timePower;
     double tempPower;
     double currPower;
     double c;
   } UnitDefinition_Type;

#endif
