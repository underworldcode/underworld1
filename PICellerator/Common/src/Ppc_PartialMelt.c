#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_PartialMelt.h"


/* Textual name of this class */
const Type Ppc_PartialMelt_Type = "Ppc_PartialMelt";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_PartialMelt* _Ppc_PartialMelt_New(  PPC_LINEARDENSITY_DEFARGS  )
{
   Ppc_PartialMelt* self;

   assert( _sizeOfSelf >= sizeof(Ppc_PartialMelt) );
   nameAllocationType = NON_GLOBAL;
   self = (Ppc_PartialMelt*) _Ppc_New(  PPC_PASSARGS  );
   self->_get = _get;
   return self;
}


void* _Ppc_PartialMelt_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof(Ppc_PartialMelt);
   Type                                                      type = Ppc_PartialMelt_Type;
   Stg_Class_DeleteFunction*                              _delete = _Ppc_PartialMelt_Delete;
   Stg_Class_PrintFunction*                                _print = _Ppc_PartialMelt_Print;
   Stg_Class_CopyFunction*                                  _copy = NULL;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_PartialMelt_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _Ppc_PartialMelt_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _Ppc_PartialMelt_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _Ppc_PartialMelt_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _Ppc_PartialMelt_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _Ppc_PartialMelt_Destroy;
   AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_PartialMelt_Get;

   return (void*)_Ppc_PartialMelt_New(  PPC_LINEARDENSITY_PASSARGS  );
}


void _Ppc_PartialMelt_Init( void* _self, int liquidusTag, int solidusTag, int t )
{
   Ppc_PartialMelt* self = (Ppc_PartialMelt*)_self;

   /* Sanity check */
   self->liquidusTag = liquidusTag;
   self->solidusTag = solidusTag;
   self->temperatureTag = t;
}


void _Ppc_PartialMelt_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data )
{
   Ppc_PartialMelt* self = (Ppc_PartialMelt*)_self;

   /* Construct parent */
   _Ppc_AssignFromXML( self, cf, data );

   _Ppc_PartialMelt_Init(
      self,
      PpcManager_GetPpcFromDict( self->manager, cf, (Name)self->name, "LiquidusTag", "" ),
      PpcManager_GetPpcFromDict( self->manager, cf, (Name)self->name, "SolidusTag", "" ),
      PpcManager_GetPpcFromDict( self->manager, cf, (Name)self->name, "TemperatureTag", "default_temperature_ppc" ) );
}


void _Ppc_PartialMelt_Build( void* _self, void* data )
{
   Ppc_PartialMelt* self = (Ppc_PartialMelt*)_self;

   /* Build parent */
   _Ppc_Build( self, data );
}

void _Ppc_PartialMelt_Initialise( void* _self, void* data )
{
   Ppc_PartialMelt* self = (Ppc_PartialMelt*)_self;

   /* Initialize parent */
   _Ppc_Initialise( self, data );
}

void _Ppc_PartialMelt_Delete( void* _self )
{
   Ppc_PartialMelt* self = (Ppc_PartialMelt*)_self;

   /* Delete parent */
   _Ppc_Delete( self );
}

void _Ppc_PartialMelt_Print( void* _self, Stream* stream )
{
   Ppc_PartialMelt* self = (Ppc_PartialMelt*)_self;

   /* Print parent */
   _Ppc_Print( self, stream );
}

void _Ppc_PartialMelt_Execute( void* _self, void* data )
{
   Ppc_PartialMelt* self = (Ppc_PartialMelt*)_self;

   /* Execute parent */
   _Ppc_Execute( self, data );
}

void _Ppc_PartialMelt_Destroy( void* _self, void* data )
{
   Ppc_PartialMelt* self = (Ppc_PartialMelt*)_self;

   /* Destroy parent */
   _Ppc_Destroy( self, data );
}


Ppc_PartialMelt* Ppc_PartialMelt_New( Name name, Stg_Component* _self )
{
   Ppc_PartialMelt* self = (Ppc_PartialMelt*) _self;
   return self;
}

/*
 * Public functions
 *
 */

int _Ppc_PartialMelt_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result )
{
   Ppc_PartialMelt* self = (Ppc_PartialMelt*) _self;
   double lT, sT, T, ss, tmp;
   int err;

   /** 1st calculate supersolidus temperature **/

   /** Get Liquidus temperature **/
   err = PpcManager_Get( self->manager, lElement_I, particle, self->liquidusTag, &lT );
   if ( err )
      lT = 0;

   /* get thermal expansivity */
   err = PpcManager_Get( self->manager, lElement_I, particle, self->solidusTag, &sT );
   if ( err )
      sT = 0;

   assert( fabs( (lT-sT) ) > 1.0e-15 );

   /** Pressure dependence **/
   err = PpcManager_Get( self->manager, lElement_I, particle, self->temperatureTag, &T );
   if ( err )
      T = 0;

   if ( T < sT )
      ss = 0;
   else if ( T > lT )
      ss = 1;
   else
   {
      ss = ( T - sT ) / ( lT - sT );
   }

   /** error check the supersolidus temperature **/
   if ( ss <= 0 )
   {
      result[0] = 0;
      return 0;
   }

   if ( ss >= 1 )
   {
      result[0] = 1;
      return 0;
   }

   /* calculate and return melt fraction (after McKenzie and Bickle, 1988) */
   ss = ss - 0.5;
   tmp = 0.5 + ss + (ss*ss - 0.25)*(0.4256 + 2.988*ss);

   if ( tmp > 1 )
      result[0] = 1;
   else if ( tmp < 0 )
      result[0] = 0;
   else result[0] = tmp;

   return 0;
}
