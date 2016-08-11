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
#include "Ppc.h"
#include "PpcManager.h"

const Type Ppc_Variable_Type = "Ppc_Variable";

void _Ppc_Variable_Init( Ppc_Variable* self, FieldVariable* fieldVar, char* swarmVarName ) {
   char* name=NULL; /* name of Ppc_Variable */
  int length=-1;

   /* check if both an fieldVariable and swarmVariable are defined, it's an error */
  if( swarmVarName != NULL ) length = strlen(swarmVarName);

   if( fieldVar != NULL && ( swarmVarName != NULL && length > 0 ) ) {
      Journal_Firewall( 0, self->error_stream,
            "Error: Component '%s' has both an FieldVariable '%s and a SwarmVariableName '%s'. It can only use one "
            "of these, so please remove one\n.",
            self->name, fieldVar->name, swarmVarName );
   } 
   /* check if neither a fieldVariable nor a swarnVariable are defined */
   if( fieldVar == NULL && ( swarmVarName == NULL && length < 1 ) ) {
      Journal_Firewall( 0, self->error_stream,
            "Error: Component '%s' has neither an FieldVariable nor a SwarmVariableName. It needs one "
            "of these to work\n.", self->name );
   }

   /* assign type: FieldVariable or SwarmVariable */
   if( fieldVar != NULL ) {
      self->fieldVar = fieldVar;
      self->swarmVarName = NULL;
      if(Stg_Class_IsInstance( fieldVar, FieldVariable_Type ) ){
         self->myVarType = PPCVAR_FIELDVARIABLE;
         if (Stg_Class_IsInstance( fieldVar, FeVariable_Type ) && ( ((FeVariable*)fieldVar)->feMesh == self->manager->mesh ) )
            self->myVarType = PPCVAR_FEVARIABLE;
      } else
         Journal_Firewall( 0, self->error_stream, "\n\nError: Passed in component '%s' is not a child of FieldVariable as required.\n\n", fieldVar->name );
      name = self->fieldVar->name;
   } else {
      self->fieldVar = NULL;
      self->swarmVarName = swarmVarName;
      self->myVarType = PPCVAR_SWARMVARIABLE;
      name = swarmVarName;
   }

   PpcManager_AddSolePpcToList( self->manager, (Ppc*)self, name );
}

/** Public constructor, simple interface */
Ppc_Variable* Ppc_Variable_New(  Name name,
                                 PpcManager* mgr,
                                 FieldVariable* fieldVar,
                                 Name swarmVarName )
{

   Ppc_Variable* self = _Ppc_Variable_DefaultNew( name );

   self->isConstructed = True;
   _Ppc_Init( self, mgr, HUGE_VAL, HUGE_VAL, 0 );
   _Ppc_Variable_Init( self, fieldVar, swarmVarName );

   /* Register the component for build and initialise phase */
   LiveComponentRegister_IfRegThenAdd( (Stg_Component*)self );

   return self;
}

Ppc_Variable* _Ppc_Variable_New(  PPCVARIABLE_DEFARGS  ) {
	Ppc_Variable* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_Variable) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_Variable*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}

void* _Ppc_Variable_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_Variable);
	Type                                                      type = Ppc_Variable_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_Variable_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_Variable_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_Variable_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_Variable_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_Variable_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_Variable_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_Variable_Get;

	return (void*)_Ppc_Variable_New(  PPCVARIABLE_PASSARGS  );
}

/* --- Virtual Function Implementations --- */
void _Ppc_Variable_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ){
   Ppc_Variable*        self             = (Ppc_Variable*)_self;
   FieldVariable*       fieldVar         = NULL;
   char*                swarmVarName     = NULL;

   _Ppc_AssignFromXML( _self, cf, data );

   fieldVar = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"FieldVariable", FieldVariable, False, data );
   swarmVarName = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"SwarmVariableName", NULL );

   _Ppc_Variable_Init( self, fieldVar, swarmVarName );

}

void _Ppc_Variable_Build( void* _self, void* data ) {
	Ppc_Variable* self = (Ppc_Variable*) _self;

   _Ppc_Build( _self, data );

   if( self->fieldVar ) Stg_Component_Build( self->fieldVar, data, False );
}

void _Ppc_Variable_Initialise( void* _self, void* data ) {
	Ppc_Variable*      self = (Ppc_Variable*) _self;
   PpcManager*       manager = NULL;
   MaterialPointsSwarm* ms   = NULL;

   _Ppc_Initialise( _self, data );

   manager = self->manager;
   ms      = manager->materialSwarm;

   if( self->fieldVar ) Stg_Component_Initialise( self->fieldVar, data, False );
   else {
    /* check the swarm variable from the construct func exists */
    self->swarmVar = Stg_ObjectList_Get( ms->swarmVariable_Register->objects, (Name)self->swarmVarName  );
    Journal_Firewall( self->swarmVar != NULL , self->error_stream,
        "\nCan't find the swarm variable called %s attached to the swarm, %s. Please ensure this swarm variable "
        "is created on the correct swarm. To find out which swarm variables are in your simulation use the StgFEM_SwarmVariableList "
        "plugin\n", self->swarmVar, ms );

    /* if the swarmVar exists, ensure it's been build and initialised */
    Stg_Component_Build( self->swarmVar, data, False );
    Stg_Component_Initialise( self->swarmVar, data, False );
  }
}

void _Ppc_Variable_Execute( void* _self, void* _ctx ) {
}

void _Ppc_Variable_Destroy( void* _self, void* data ) {
   Ppc_Variable* self = (Ppc_Variable*) _self;

   if( self->fieldVar ) Stg_Component_Destroy( self->fieldVar, data, False );

   _Ppc_Destroy( _self, data );
}

int _Ppc_Variable_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
   Ppc_Variable *self = (Ppc_Variable*)_self;
   PpcManager *manager = self->manager;
   Coord globalCoord;
   InterpolationResult res;
   
   switch( self->myVarType ) {
      case PPCVAR_FIELDVARIABLE:
         FeMesh_CoordLocalToGlobal( manager->mesh, lElement_I, particle->xi, globalCoord );
         res = FieldVariable_InterpolateValueAt( self->fieldVar, globalCoord, result );
         return !( (res == LOCAL) || (res == SHADOW) );
      case PPCVAR_FEVARIABLE:
         /* note: we interpolate with the manager's mesh. Assume FeVariable_InterpolateFromMeshLocalCoord() can
          * handle interpolation between meshes */
         res = FeVariable_InterpolateFromMeshLocalCoord( self->fieldVar, manager->mesh, lElement_I, particle->xi, result );
         return !( (res == LOCAL) || (res == SHADOW) );
      case PPCVAR_SWARMVARIABLE:
         {
            MaterialPointsSwarm*     ms  = (MaterialPointsSwarm*)manager->materialSwarm;
            MaterialPointsSwarm*     tmp = NULL; 
            IntegrationPointsSwarm*  is = (IntegrationPointsSwarm*)manager->integrationSwarm; 
            int lParticleId;

            /* get the material point in charge of this integrationPoint */
            MaterialPoint* materialPoint = OneToOneMapper_GetMaterialPoint( 
                              ((IntegrationPointsSwarm*)is)->mapper, 
                              particle, 
                              (MaterialPointsSwarm**)&tmp );

            /* now some magic:
            * to correctly reference the SwarmVariable we need to know the local particle
            * index in the variable array. This can be worked out through some pointer
            * madness */
            lParticleId = ( ms->particleExtensionMgr == NULL ) ?
            /* there is no extension, just use the particleSize as the spacing */
            ( (ArithPointer)materialPoint-(ArithPointer)ms->particles )/ms->particleSize :
            /* there is an extension, use the finalParticleSize as the spacing */
            ( (ArithPointer)materialPoint-(ArithPointer)ms->particles )/ms->particleExtensionMgr->finalSize;

            SwarmVariable_ValueAt( self->swarmVar, lParticleId, result );
            return 0;
         }
      default:
         assert(0);
   }
}
