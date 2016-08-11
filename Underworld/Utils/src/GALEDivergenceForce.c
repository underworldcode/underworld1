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
** Copyright (C) 2008, California Institute of Technology
** Modified for GALEDivergenceForce by Walter Landry
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

#include "types.h"
#include "GALEDivergenceForce.h"

#include <assert.h>
#include <string.h>

/* Textual name of this class */
const Type GALEDivergenceForce_Type = "GALEDivergenceForce";

GALEDivergenceForce* GALEDivergenceForce_New(Name name,
                                     FiniteElementContext*	context,
                                     ForceVector* forceVector,
                                     Swarm* integrationSwarm,
                                     Stg_Shape* domainShape,
                                     FeMesh* geometryMesh,
                                     GALEStressBC_Entry force)
{
	GALEDivergenceForce* self = (GALEDivergenceForce*) _GALEDivergenceForce_DefaultNew( name );

	GALEDivergenceForce_InitAll( 
			self,
                        context,
			forceVector,
			integrationSwarm,
                        domainShape,
                        geometryMesh,
                        force);

	return self;
}

/* Creation implementation / Virtual constructor */
GALEDivergenceForce* _GALEDivergenceForce_New( DIVERGENCEFORCE_DEFARGS)
{
	GALEDivergenceForce* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(GALEDivergenceForce) );
	self = (GALEDivergenceForce*) _ForceTerm_New( FORCETERM_PASSARGS);
	
	return self;
}

void _GALEDivergenceForce_Init(GALEDivergenceForce* self,
                           Stg_Shape* domainShape, FeMesh *geometryMesh,
                           GALEStressBC_Entry force)
{
  self->isConstructed    = True;

  self->domainShape=domainShape;
  self->geometryMesh=geometryMesh;
  self->force=force;
}

void GALEDivergenceForce_InitAll( 
		void*                                               forceTerm,
                FiniteElementContext*	context,
		ForceVector*                                        forceVector,
		Swarm*                                              integrationSwarm,
                Stg_Shape* domainShape,
                FeMesh* geometryMesh,
                GALEStressBC_Entry force)
{
	GALEDivergenceForce* self = (GALEDivergenceForce*) forceTerm;

	_ForceTerm_Init( self, context, forceVector, integrationSwarm, NULL );
	_GALEDivergenceForce_Init( self, domainShape, geometryMesh, force);
}

void _GALEDivergenceForce_Delete( void* forceTerm ) {
	GALEDivergenceForce* self = (GALEDivergenceForce*)forceTerm;

	_ForceTerm_Delete( self );
}

void _GALEDivergenceForce_Print( void* forceTerm, Stream* stream ) {
	GALEDivergenceForce* self = (GALEDivergenceForce*)forceTerm;
	
	_ForceTerm_Print( self, stream );

	/* General info */
}

void* _GALEDivergenceForce_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(GALEDivergenceForce);
	Type                                                      type = GALEDivergenceForce_Type;
	Stg_Class_DeleteFunction*                              _delete = _GALEDivergenceForce_Delete;
	Stg_Class_PrintFunction*                                _print = _GALEDivergenceForce_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _GALEDivergenceForce_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _GALEDivergenceForce_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _GALEDivergenceForce_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _GALEDivergenceForce_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _GALEDivergenceForce_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _GALEDivergenceForce_Destroy;
	ForceTerm_AssembleElementFunction*            _assembleElement = _GALEDivergenceForce_AssembleElement;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*)_GALEDivergenceForce_New( DIVERGENCEFORCE_PASSARGS);
}

void _GALEDivergenceForce_AssignFromXML( void* forceTerm, Stg_ComponentFactory* cf, void* data ) {
	GALEDivergenceForce*          self             = (GALEDivergenceForce*)forceTerm;
	Dictionary*		dict;
        Stg_Shape* domainShape=NULL;
        FeMesh* geometryMesh=NULL;
        GALEStressBC_Entry force;
        char *type;

	/* Construct Parent */
	_ForceTerm_AssignFromXML( self, cf, data );

	dict = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, self->name ) );
	domainShape =  Stg_ComponentFactory_ConstructByKey( cf, self->name, "DomainShape", Stg_Shape, True, data ) ;
        type = Stg_ComponentFactory_GetString( cf, self->name, "force_type", "" );

        if(!strcasecmp(type,"double") || !strcasecmp(type,"float"))
          {
            force.type = GALEStressBC_Double;
            force.DoubleValue = Stg_ComponentFactory_GetDouble( cf, self->name, "force_value", 0.0 );
          }
        else if(!strcasecmp(type,"func"))
          {
            char *funcName = Stg_ComponentFactory_GetString( cf, self->name, "force_value", "" );
            
            Index cfIndex;
            cfIndex = ConditionFunction_Register_GetIndex
              ( condFunc_Register, funcName);
            force.type = GALEStressBC_ConditionFunction;
            if ( cfIndex == (unsigned)-1 ) {	
              Stream*	errorStr = Journal_Register( Error_Type, self->type );
              
              Journal_Printf( errorStr, "Error- in %s: While parsing "
                              "definition of GALEDivergenceForce, the cond. func. "
                              " \"%s\" - wasn't found in the c.f. register.\n",
                              __func__, funcName );
              Journal_Printf( errorStr, "(Available functions in the C.F. register are: ");	
              ConditionFunction_Register_PrintNameOfEachFunc
                ( condFunc_Register, errorStr );
              Journal_Printf( errorStr, ")\n");	
              assert(0);
            }
            force.CFIndex = cfIndex;
          }
        else if(strlen(type)==0)
          {
            Stream* errorStr = Journal_Register( Error_Type, self->type );
            Journal_Printf( errorStr, "Error- in %s: While parsing "
                            "definition of GALEDivergenceForce, force_type is not specified.\nSupported types are \"double\" and \"function\".\n",
                            __func__);
            assert(0);
          }
        else
          {
            Stream* errorStr = Journal_Register( Error_Type, self->type );
            Journal_Printf( errorStr, "Error- in %s: While parsing "
                            "definition of GALEDivergenceForce, the type of condition \"%s\"\nis not supported.  Supported types are \"double\" and \"function\".\n",
                            __func__, type );
            assert(0);
          }
        
        geometryMesh=Stg_ComponentFactory_ConstructByKey( cf, self->name, "GeometryMesh", FeMesh, True, data ) ;
        
	_GALEDivergenceForce_Init( self, domainShape, geometryMesh, force);
}

void _GALEDivergenceForce_Build( void* forceTerm, void* data ) {
	GALEDivergenceForce*               self               = (GALEDivergenceForce*)forceTerm;
	_ForceTerm_Build( self, data );
}

void _GALEDivergenceForce_Initialise( void* forceTerm, void* data ) {
	GALEDivergenceForce*             self             = (GALEDivergenceForce*)forceTerm;
	_ForceTerm_Initialise( self, data );
}

void _GALEDivergenceForce_Execute( void* forceTerm, void* data ) {
	_ForceTerm_Execute( forceTerm, data );
}

void _GALEDivergenceForce_Destroy( void* forceTerm, void* data ) {
	_ForceTerm_Destroy( forceTerm, data );
}


void _GALEDivergenceForce_AssembleElement( void* forceTerm,
                                       ForceVector* forceVector, 
                                       Element_LocalIndex lElement_I, 
                                       double* elForceVec ) {
  GALEDivergenceForce* self=(GALEDivergenceForce*) forceTerm;
  FeMesh* mesh=forceVector->feVariable->feMesh;
  Node_ElementLocalIndex           eNode_I;
  Element_NodeIndex                elementNodeCount;
  Node_DomainIndex *elementNodes=NULL;

  double xi[3], force, factor;
  ElementType* geometryElementType;
  
  IArray *incidence;

  xi[0]=0;
  xi[1]=0;
  xi[2]=0;
  geometryElementType=FeMesh_GetElementType(self->geometryMesh,lElement_I);
  factor=ElementType_JacobianDeterminant( geometryElementType,
                                          self->geometryMesh,
                                          lElement_I,
                                          xi, forceVector->dim );
  incidence=IArray_New();
  Mesh_GetIncidence(mesh, Mesh_GetDimSize(mesh), lElement_I,
                    MT_VERTEX,incidence);
  elementNodeCount=IArray_GetSize(incidence);
  elementNodes=IArray_GetPtr(incidence);
  
  for( eNode_I = 0 ; eNode_I < elementNodeCount; eNode_I++ ) {
    if(Stg_Shape_IsCoordInside(self->domainShape,
                               Mesh_GetVertex(mesh,elementNodes[eNode_I])))
      {
        switch(self->force.type)
          {
          case GALEStressBC_Double:
            force=self->force.DoubleValue;
            break;
          case GALEStressBC_ConditionFunction:
            
            /* We use a variable number of zero "0", because
               we don't use the variable number and that one
               is always going to exist. */
            ConditionFunction_Apply
              (condFunc_Register->_cf[self->force.CFIndex],
               elementNodes[eNode_I],0,self->context,&force);
            break;
          }
        elForceVec[ eNode_I] += force*factor;
      }
  }
  Stg_Class_Delete(incidence);
}
