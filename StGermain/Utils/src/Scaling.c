#include <mpi.h>
#include <math.h>
#include <StGermain/StGermain.h>

#include "types.h"
#include "Scaling.h"
#include "u.h"
#include "u.tab.h"

UnitDefinition_Type parse( char* strUnit );

/* Textual name of this class */
const Type Scaling_Type = "Scaling";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Scaling* _Scaling_New(  SCALING_DEFARGS  )
{
  Scaling* self;  
  assert( _sizeOfSelf >= sizeof(Scaling) );
  nameAllocationType = NON_GLOBAL;
  self = (Scaling*) _Stg_Component_New(  STG_COMPONENT_PASSARGS  );	
  return self;
}


void* _Scaling_DefaultNew( Name name ) {
  SizeT                                              _sizeOfSelf = sizeof(Scaling);
  Type                                                      type = Scaling_Type;
  Stg_Class_DeleteFunction*                              _delete = _Stg_Component_Delete;
  Stg_Class_PrintFunction*                                _print = _Stg_Component_Print;
  Stg_Class_CopyFunction*                                  _copy = _Stg_Component_Copy;
  Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Scaling_DefaultNew;
  Stg_Component_ConstructFunction*                    _construct = _Scaling_AssignFromXML;
  Stg_Component_BuildFunction*                            _build = _Scaling_Build;
  Stg_Component_InitialiseFunction*                  _initialise = _Scaling_Initialise;
  Stg_Component_ExecuteFunction*                        _execute = NULL;
  Stg_Component_DestroyFunction*                        _destroy = _Scaling_Destroy;

  AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

  return (void*)_Scaling_New(  SCALING_PASSARGS  );
}

void _Scaling_Build( void* _self, void* data ) {}

void _Scaling_Initialise( void* _self, void* data ) {}

void _Scaling_Destroy( void* _self, void* data ) {}

void _Scaling_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
  Scaling* self = (Scaling*)_self;
  Dictionary*	dict;
  double l, m, t, T, ec;

  dict = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name ) );

  l  = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"spaceCoefficient_meters", 1.0 );
  t  = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"timeCoefficient_seconds", 1.0 );
  m  = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"massCoefficient_kilograms", 1.0 );
  T  = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"temperatureCoefficient_kelvin", 1.0 );
  ec = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"electricCurrentCoefficient_amperes", 1.0 );

  _Scaling_Init( self, l, m, t, T, ec );
}


void _Scaling_Init( void* _self, double l, double m, double t, double T, double ec ) {
  Scaling*                      self = (Scaling* )_self;

  self->length = l;;
  self->time = t;
  self->mass = m;
  self->temperature = T;
  self->electricCurrent = ec;

  self->info_stream = Journal_Register( Info_Type, (Name)self->type  );
  self->error_stream = Journal_Register( Error_Type, (Name)self->type  );
}


Scaling* Scaling_New( 
							Name                                                name,
							double                                              space,
							double                                              time,
							double                                              mass,
							double                                              temperature,
							double                                              ec ) {

  Scaling*        self = (Scaling*) _Scaling_DefaultNew( name );

  _Scaling_Init( self, space, mass, time, temperature, ec );

  self->isConstructed = True;

  return self;
}


double scalingFactor( void* _self, UnitDefinition_Type theUnit ) {
  Scaling* self = (Scaling*)_self;
  double scalingFactor = 1.0;

  if( theUnit.lengPower != 0 )
	 scalingFactor *= pow( self->length, theUnit.lengPower );
  if( theUnit.timePower != 0 )
	 scalingFactor *= pow( self->time, theUnit.timePower );
  if( theUnit.massPower != 0 )
	 scalingFactor *= pow( self->mass, theUnit.massPower );
  if( theUnit.tempPower != 0 )
	 scalingFactor *= pow( self->temperature, theUnit.tempPower );
  if( theUnit.currPower != 0 )
	 scalingFactor *= pow( self->electricCurrent, theUnit.currPower );

  return scalingFactor;
}

double Scaling_Unscale( void* _self, double scaledValue, UnitDefinition_Type theUnit ) {
  Scaling* self = (Scaling*)_self;  
  double dimensionalValue = scaledValue / theUnit.c * scalingFactor( self, theUnit );
  return  dimensionalValue;
}

double Scaling_Scale( void* _self, double dimensionalValue, UnitDefinition_Type theUnit ) {
  Scaling* self = (Scaling*)_self;
  double  scaledValue = dimensionalValue * theUnit.c / scalingFactor( self, theUnit );
  return  scaledValue;
}


UnitDefinition_Type Scaling_Parse( char* strUnit ) {
  UnitDefinition_Type out;
  int err;

  err = myParser( strUnit, (uyyType*) &out );

  Journal_Firewall( !err, 
						  Journal_Register( Error_Type, (Name)"Scaling_Parse"  ), 
						  "\n\n\nWrong units: %s\n"
						  "Please check your intput file.\n\n\n", strUnit );
  
  return out;
}

/* given parsable SI units, return the dimensionalising coefficient */
double Scaling_ParseDimCoeff( void* _self, char* strUnit) {
   Scaling* self = (Scaling*)_self;
   UnitDefinition_Type theUnit = Scaling_Parse( strUnit );

   return scalingFactor( self, theUnit ) / theUnit.c;
}

void _Scaling_ScaleList( void* _self, Dictionary_Entry* entryPtr, Dictionary_Entry_Value_List* listPtr ) {
  /* this iterates on each of its Dictionary_Entry and tries to scale it
	* this can be recursive if the Dictionary_Entry is a complex type
	*/
  Scaling* self = (Scaling*)_self;
  Dictionary_Entry_Value* anode=NULL;
  unsigned int ii;
  
  /* if the list is empty do nothing */
  if( listPtr->count == 0  ) return ;

  anode = listPtr->first;
  for( ii=0;ii<listPtr->count;ii++) { 
	 _Scaling_ScaleEntry( self, entryPtr, anode ); 
	 anode = anode->next; 
  }
}

void _Scaling_ScaleEntry( void* _self, Dictionary_Entry* entryPtr, Dictionary_Entry_Value* valuePtr ) {
  /* This is a recursive function, but not a nice one
   * Entries can have serveral value types and require different
   * scaling algorithms depending on the value type
   */ 
  Scaling* self = (Scaling*)_self;

  switch( valuePtr->type ) {
  case Dictionary_Entry_Value_Type_Struct:
	 /* recursive case - to scale a dictionary */
	 Scaling_ScaleDictionary( self, valuePtr->as.typeStruct );
	 break;
  case Dictionary_Entry_Value_Type_List:
	 /* recursive case - to scale a list */
	 _Scaling_ScaleList( self, entryPtr, valuePtr->as.typeList );
	 break;
  case Dictionary_Entry_Value_Type_String:
	 /* if the dicionary entry needs to be scaled... scale it */
	 if( entryPtr->units != NULL ) {
		/* 1st grab the string from the dictionary and 
		 * parse the unit string to a UnitDefinition_Type*/
		char *valueStr, *unitsStr;
      double value;

      valueStr=valuePtr->as.typeString;
		unitsStr=entryPtr->units;

       /* check if string is numeric otherwise error */
       Journal_Firewall( Stg_StringIsNumeric( valueStr ), self->error_stream,
             "\n\n\nError: The param '%s' with value '%s' has a units string '%s' "
             "attributed with it but the value is not numeric."
             "\nEither remove the unit attribute or change the value\n\n", 
             entryPtr->key, valueStr, unitsStr);

		/* 2nd atof that string */
		value = atof( valueStr );

		/* being very careful that the value is a number */
		if( !isnan(value) ) {
		  UnitDefinition_Type udt = Scaling_Parse( unitsStr );

		  /* 3rd do the Scaling */
		  double scaledValue = Scaling_Scale( self, value, udt );

		  /*4th convert the existing Dictionary_Entry
			* to be of type double and store the value appropriately
			* then free the old string and prey */
		  valuePtr->as.typeDouble = scaledValue;
		  valuePtr->type=Dictionary_Entry_Value_Type_Double;
		  /*free( valuePtr->as.typeString );*/

	 Journal_Printf( self->info_stream, "Scaling parameter %10s: %g %s --> %g\n", entryPtr->key, value, entryPtr->units, scaledValue );
		}
	 }
	 break;
  default:
	 /* if the dicionary entry needs to be scaled... scale it */
	 if( entryPtr->units != NULL && entryPtr->value->type == Dictionary_Entry_Value_Type_Double ) {
		/* 1st grab the value from the dictionary.
		 * and the parse the unit string to a UnitDefinition_Type*/
		double value = entryPtr->value->as.typeDouble;

		UnitDefinition_Type udt = Scaling_Parse( entryPtr->units );

		/* 2nd do the Scaling */
		double scaledValue = Scaling_Scale( self, value, udt );

		/*3rd overwrite the existing value in the Dictionary */
		entryPtr->value->as.typeDouble = scaledValue;

	 Journal_Printf( self->info_stream, "Scaling parameter %10s: %g %s --> %g\n", entryPtr->key, value, entryPtr->units, scaledValue );
	 }
  }
}

void Scaling_ScaleDictionary( void* _self, Dictionary* dick ) {
  Scaling* self = (Scaling*)_self;
  Dictionary_Entry* entryPtr=NULL;
  unsigned int index;
  
  /* find the toolboxes tag, and write it first */
  for (index=0; index < dick->count; index++) {
    entryPtr = dick->entryPtr[index]; 
    _Scaling_ScaleEntry( self, entryPtr, entryPtr->value );
  }
}
