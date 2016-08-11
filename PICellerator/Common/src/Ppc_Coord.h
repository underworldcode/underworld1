#ifndef __PICellerator_Common_Ppc_Coord_h__
#define __PICellerator_Common_Ppc_Coord_h__

/** Textual name of this class */
extern const Type Ppc_Coord_Type;

/** Ppc_Coord class contents */
#define __Ppc_Coord \
		/* Parent info */ \
		__Ppc \
		/* General data */ \
        unsigned axis;

struct Ppc_Coord
{
   __Ppc_Coord
};


#ifndef ZERO
#define ZERO 0
#endif

#define PPC_CONSTANT_DEFARGS \
                PPC_DEFARGS

#define PPC_CONSTANT_PASSARGS \
                PPC_PASSARGS

Ppc_Coord* _Ppc_Coord_New(  PPC_CONSTANT_DEFARGS  );

void _Ppc_Coord_Delete( void* _self );
void _Ppc_Coord_Print( void* _self, Stream* stream );
void* _Ppc_Coord_DefaultNew( Name name );
void _Ppc_Coord_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
void _Ppc_Coord_Build( void* _self, void* data );
void _Ppc_Coord_Initialise( void* _self, void* data );
void _Ppc_Coord_Execute( void* _self, void* data );
void _Ppc_Coord_Destroy( void* _self, void* data );

/* Public functions */
int _Ppc_Coord_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

