/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**	Melbourne, 3053, Australia.
** Copyright (c) 2005-2010, Monash University, Building 28, Monash University Clayton Campus,
**	Victoria, 3800, Australia
**
** Primary Contributing Organisations:
**	Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**	AuScope - http://www.auscope.org
**	Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
**	Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**	Patrick D. Sunter, Software Engineer, VPAC. (patrick@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**	Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
**	Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**	Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**	David Stegman, Postdoctoral Fellow, Monash University. (david.stegman@sci.monash.edu.au)
**	Wendy Sharples, PhD Student, Monash University (wendy.sharples@sci.monash.edu.au)
**
**  This library is free software; you can redistribute it and/or
**  modify it under the terms of the GNU Lesser General Public
**  License as published by the Free Software Foundation; either
**  version 2.1 of the License, or (at your option) any later version.
**
**  This library is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
**  Lesser General Public License for more details.
**
**  You should have received a copy of the GNU Lesser General Public
**  License along with this library; if not, write to the Free Software
**  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include "types.h"
#include "PpcManager.h"

const Type PpcFeVariable_Type = "PpcFeVariable";

char* PpcFeVariable_names[10];
int PpcFeVariable_nNames = 0;
int PpcFeVariable_curName = 0;

PpcFeVariable* PpcFeVariable_New( Name name, 
      DomainContext *context, 
      FeMesh *mesh,
      IntegrationPointsSwarm *swarm,
      Bool accumulate,
      PpcManager *ppcMan,
      int tag ) {

   PpcFeVariable     *self = NULL;
   int               componentsCount=1;

   /* painfully define a new ParticleFeVariable, methods first */
   self = _PpcFeVariable_DefaultNew( name );

   /** define basics in data structures */
   self->isConstructed = True;
	_FieldVariable_Init( (FieldVariable*)self, context, componentsCount, context->dim, True, NULL, context->communicator, context->fieldVariable_Register, False );
   _FeVariable_Init( (FeVariable* )self, mesh, NULL, NULL, False, NULL, NULL, NULL, False, False );
	_ParticleFeVariable_Init( (ParticleFeVariable*)self, swarm, accumulate );
	_PpcFeVariable_Init( self, ppcMan, tag );

   LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*) self );

   return self;

}


void* _PpcFeVariable_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                                       _sizeOfSelf = sizeof(PpcFeVariable);
   Type                                                               type = PpcFeVariable_Type;
   Stg_Class_DeleteFunction*                                       _delete = _ParticleFeVariable_Delete;
   Stg_Class_PrintFunction*                                         _print = _ParticleFeVariable_Print;
   Stg_Class_CopyFunction*                                           _copy = _ParticleFeVariable_Copy;
   Stg_Component_DefaultConstructorFunction*           _defaultConstructor = _PpcFeVariable_DefaultNew;
   Stg_Component_ConstructFunction*                             _construct = _PpcFeVariable_AssignFromXML;
   Stg_Component_BuildFunction*                                     _build = _PpcFeVariable_Build;
   Stg_Component_InitialiseFunction*                           _initialise = _PpcFeVariable_Initialise;
   Stg_Component_ExecuteFunction*                                 _execute = _PpcFeVariable_Execute;
   Stg_Component_DestroyFunction*                                 _destroy = _PpcFeVariable_Destroy;
   FieldVariable_InterpolateValueAtFunction*           _interpolateValueAt = _FeVariable_InterpolateValueAt;
   FieldVariable_GetCoordFunction*                _getMinAndMaxLocalCoords = _FeVariable_GetMinAndMaxLocalCoords;
   FieldVariable_GetCoordFunction*               _getMinAndMaxGlobalCoords = _FeVariable_GetMinAndMaxGlobalCoords;
   FeVariable_InterpolateWithinElementFunction*  _interpolateWithinElement = _FeVariable_InterpolateNodeValuesToElLocalCoord;
   FeVariable_GetValueAtNodeFunction*                      _getValueAtNode = _FeVariable_GetValueAtNode;
   ParticleFeVariable_ValueAtParticleFunction*            _valueAtParticle = _PpcFeVariable_ValueAtParticle;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType                             nameAllocationType = ZERO;
   FieldVariable_GetValueFunction*   _getMinGlobalFieldMagnitude = ZERO;
   FieldVariable_GetValueFunction*   _getMaxGlobalFieldMagnitude = ZERO;
   FieldVariable_CacheValuesFunction* _cacheMinMaxGlobalFieldMagnitude = ZERO;
   FeVariable_SyncShadowValuesFunc*            _syncShadowValues = ZERO;

   return (void*) _PpcFeVariable_New(  PPCFEVARIABLE_PASSARGS  );
}

PpcFeVariable* _PpcFeVariable_New(  PPCFEVARIABLE_DEFARGS  ) {
	PpcFeVariable* self;
	
   /* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree.
   At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
   assert( _sizeOfSelf >= sizeof(PpcFeVariable) );
   self = (PpcFeVariable*)_ParticleFeVariable_New(  PARTICLEFEVARIABLE_PASSARGS );
   self->ppcMan = NULL;
   self->ppcTag = -1;

	return self;
}

void _PpcFeVariable_Init( PpcFeVariable* self, PpcManager *PpcMan, int tag ) {
   self->ppcMan = PpcMan;
   self->ppcTag = tag;
}

/* --- Virtual Function Implementations --- */
void _PpcFeVariable_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ){
   PpcFeVariable* self = (PpcFeVariable*) _self;
   PpcManager* ppcMan;
   int tag;

   _ParticleFeVariable_AssignFromXML( _self, cf, data );

   ppcMan     = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"PpcManager", PpcManager, True, data );
   tag        = PpcManager_GetPpcFromDict( ppcMan, cf, self->name, (Dictionary_Entry_Key)"functionLabel", "functionLabel" );

   _PpcFeVariable_Init( self, ppcMan, tag );
}

void _PpcFeVariable_Build( void* _self, void* data ) {
   PpcFeVariable* self = (PpcFeVariable*) _self;
   Name                 tmpName;

   Stg_Component_Build( self->ppcMan, data, False );

   /* Create Dof Layout */
   tmpName = Stg_Object_AppendSuffix( self, (Name)"Variable" );
   self->dataVariable = Variable_NewScalar( tmpName, (AbstractContext*)self->context, Variable_DataType_Double, (Index*)&((IGraph*)self->feMesh->topo)->remotes[MT_VERTEX]->nDomains, NULL, (void**)&self->data, self->context->variable_Register );
   Memory_Free( tmpName  );
   self->fieldComponentCount = 1;

   tmpName = Stg_Object_AppendSuffix( self, (Name)"DOF" );
   self->dofLayout = DofLayout_New( tmpName, self->context, self->context->variable_Register, 0, self->feMesh );
   self->dofLayout->_numItemsInLayout = FeMesh_GetNodeDomainSize( self->feMesh );
   DofLayout_AddAllFromVariableArray( self->dofLayout, 1, &self->dataVariable );
   Memory_Free( tmpName );
   self->eqNum->dofLayout = self->dofLayout;

   _ParticleFeVariable_Build( self, data );
}

void _PpcFeVariable_Initialise( void* _self, void* data ) {
	PpcFeVariable*      self = (PpcFeVariable*) _self;

   Stg_Component_Initialise( self->ppcMan, data, False );

   _ParticleFeVariable_Initialise( self, data );
}

void _PpcFeVariable_Execute( void* _self, void* _ctx ) {
   _ParticleFeVariable_Execute( _self, _ctx );
}

void _PpcFeVariable_Destroy( void* _self, void* data ) {
	PpcFeVariable* self = (PpcFeVariable*) _self;

   _ParticleFeVariable_Destroy( self, data );
}

void _PpcFeVariable_ValueAtParticle( void* _self, IntegrationPointsSwarm *swarm, unsigned lElement_I, void *particle, double *value ) {
   PpcFeVariable *self = (PpcFeVariable*)_self;
   PpcManager *ppcMan = self->ppcMan;
   int err;

   err = PpcManager_Get( ppcMan, lElement_I, particle, self->ppcTag, value );

   if( err ) {
      assert(0);
   }

}
