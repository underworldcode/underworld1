#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>
#include <float.h>

#include "types.h"
#include "Ppc_JacobianCondition.h"


/* Textual name of this class */
const Type Ppc_JacobianCondition_Type = "Ppc_JacobianCondition";


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_JacobianCondition* _Ppc_JacobianCondition_New(  PPC_CONSTANT_DEFARGS  )
{
   Ppc_JacobianCondition* self;

   assert( _sizeOfSelf >= sizeof(Ppc_JacobianCondition) );
   nameAllocationType = NON_GLOBAL;
   self = (Ppc_JacobianCondition*) _Ppc_New(  PPC_PASSARGS  );
   self->_get = _get;
   return self;
}


void* _Ppc_JacobianCondition_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof(Ppc_JacobianCondition);
   Type                                                      type = Ppc_JacobianCondition_Type;
   Stg_Class_DeleteFunction*                              _delete = _Ppc_JacobianCondition_Delete;
   Stg_Class_PrintFunction*                                _print = _Ppc_JacobianCondition_Print;
   Stg_Class_CopyFunction*                                  _copy = NULL;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_JacobianCondition_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _Ppc_JacobianCondition_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _Ppc_JacobianCondition_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _Ppc_JacobianCondition_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _Ppc_JacobianCondition_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _Ppc_JacobianCondition_Destroy;
   AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_JacobianCondition_Get;

   return (void*)_Ppc_JacobianCondition_New(  PPC_CONSTANT_PASSARGS  );
}


void _Ppc_JacobianCondition_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data )
{
   Ppc_JacobianCondition* self = (Ppc_JacobianCondition*)_self;

   /* Construct parent */
   _Ppc_AssignFromXML( self, cf, data );

}


void _Ppc_JacobianCondition_Build( void* _self, void* data )
{
   Ppc_JacobianCondition* self = (Ppc_JacobianCondition*)_self;

   /* Build parent */
   _Ppc_Build( self, data );

   self->jacobian = Memory_Alloc_2DArray( double, 3, 3, (Name)"Temporary jacobian"  );
   self->GNi = Memory_Alloc_2DArray( double, 3, 9, (Name)"Temporary GNi"  );

}

void _Ppc_JacobianCondition_Initialise( void* _self, void* data )
{
   Ppc_JacobianCondition* self = (Ppc_JacobianCondition*)_self;

   /* Initialize parent */
   _Ppc_Initialise( self, data );
}

void _Ppc_JacobianCondition_Delete( void* _self )
{
   Ppc_JacobianCondition* self = (Ppc_JacobianCondition*)_self;

   Memory_Free( self->GNi );
   Memory_Free( self->jacobian );

   /* Delete parent */
   _Ppc_Delete( self );
}

void _Ppc_JacobianCondition_Print( void* _self, Stream* stream )
{
   Ppc_JacobianCondition* self = (Ppc_JacobianCondition*)_self;

   /* Print parent */
   _Ppc_Print( self, stream );
}

void _Ppc_JacobianCondition_Execute( void* _self, void* data )
{
   Ppc_JacobianCondition* self = (Ppc_JacobianCondition*)_self;

   /* Execute parent */
   _Ppc_Execute( self, data );
}

void _Ppc_JacobianCondition_Destroy( void* _self, void* data )
{
   Ppc_JacobianCondition* self = (Ppc_JacobianCondition*)_self;

   /* Destroy parent */
   _Ppc_Destroy( self, data );
}

/*
 * Public functions
 *
 */

int _Ppc_JacobianCondition_Get( void* _self, unsigned lElement_I, IntegrationPoint* particle, double* result )
{
   Ppc_JacobianCondition* self = (Ppc_JacobianCondition*) _self;
   ElementType* elType=NULL;
   double **GNi = self->GNi;
   double **jac = self->jacobian;
   double a,b,c,d, sroot, root, e1m, e2m;

   // get element type
   elType = FeMesh_GetElementType( self->manager->mesh , lElement_I );

   // evaluate jacobian
   ElementType_EvaluateShapeFunctionLocalDerivsAt( elType, particle->xi, GNi );
   ElementType_Jacobian( elType, self->manager->mesh, lElement_I, particle->xi, Mesh_GetDimSize(self->manager->mesh), jac, GNi );

   /* calculate the condition number of the jacobian,
    * do this taking the sq. root of the ratios of
    * eigenvalues from the jacobian * transpose_jacobian.( because we need a
    * symmetric real matrix ) */

   a = jac[0][0]*jac[0][0]+jac[0][1]*jac[0][1];
   b = jac[0][0]*jac[1][0] + jac[0][1]*jac[1][1];
   c = b;
   d = jac[1][0]*jac[1][0] + jac[1][1]*jac[1][1];

   sroot = pow((a+d),2) - 4*(a*d-b*c);
   // just calculate the moduli of the eigenvalues
   if( sroot < DBL_MIN )
   {
      // we have complex eigenvalues
      assert(0); //mirko, will i ever get complex roots?
      root = -1 * sroot;
      e1m = 0.5 * sqrt( pow( (a+d),2) + root );
      e2m = 0.5 * sqrt( pow( (a+d),2) + root );
   }
   else
   {
      // real eignevalues
      root = sqrt( sroot );
      e1m = 0.5 * ( (a+d) + root ) ;
      e2m = 0.5 * ( (a+d) - root ) ;
   }

   if( e1m > e2m )
      *result = sqrt( fabs(e1m/e2m) );
   else
      *result = sqrt( fabs(e2m/e1m) );

   *result = result[0];

   return 0;
}
