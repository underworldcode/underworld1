#ifndef __PICellerator_Common_Ppc_JacobianCondition_h__
#define __PICellerator_Common_Ppc_JacobianCondition_h__

/** Textual name of this class */
extern const Type Ppc_JacobianCondition_Type;

/** Ppc_JacobianCondition class contents */
#define __Ppc_JacobianCondition \
		/* Parent info */ \
		__Ppc \
		/* General data */ \
		double	**jacobian; \
		double  **GNi; \

struct Ppc_JacobianCondition
{
   __Ppc_JacobianCondition
};


#ifndef ZERO
#define ZERO 0
#endif

#define PPC_CONSTANT_DEFARGS \
                PPC_DEFARGS

#define PPC_CONSTANT_PASSARGS \
                PPC_PASSARGS

Ppc_JacobianCondition* _Ppc_JacobianCondition_New(  PPC_CONSTANT_DEFARGS  );

void _Ppc_JacobianCondition_Delete( void* _self );
void _Ppc_JacobianCondition_Print( void* _self, Stream* stream );
void* _Ppc_JacobianCondition_DefaultNew( Name name );
void _Ppc_JacobianCondition_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
void _Ppc_JacobianCondition_Build( void* _self, void* data );
void _Ppc_JacobianCondition_Initialise( void* _self, void* data );
void _Ppc_JacobianCondition_Execute( void* _self, void* data );
void _Ppc_JacobianCondition_Destroy( void* _self, void* data );

/* Public functions */
int _Ppc_JacobianCondition_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

