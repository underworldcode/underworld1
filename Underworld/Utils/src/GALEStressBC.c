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
**  Modified 2006-2010 by Walter Landry (California Institute of Technology)
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
** $Id$
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include "types.h"
#include "GALEStressBC.h"

#include <assert.h>
#include <string.h>

/* Textual name of this class */
const Type GALEStressBC_Type = "GALEStressBC";

extern const char* WallEnumToStr[Wall_Size];

GALEStressBC* GALEStressBC_New( 
		Name                                      name,
                FiniteElementContext*	context,
		ForceVector*                              forceVector,
		Swarm*                                    integrationSwarm,
                ConditionFunction_Register*               conFunc_Register)
{
	GALEStressBC* self = (GALEStressBC*) _GALEStressBC_DefaultNew( name );

	GALEStressBC_InitAll( 
			self,
                        context,
			forceVector,
			integrationSwarm,
                        conFunc_Register);

	return self;
}

/* Creation implementation / Virtual constructor */
GALEStressBC* _GALEStressBC_New( GALESTRESSBC_DEFARGS)
{
	GALEStressBC* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(GALEStressBC) );
	self = (GALEStressBC*) _ForceTerm_New( FORCETERM_PASSARGS);
	
        self->conFunc_Register=condFunc_Register;
	
	return self;
}

void _GALEStressBC_Init( 
		GALEStressBC*                                  self, 
                ConditionFunction_Register*		   conFunc_Register)
{
  self->isConstructed    = True;
  self->numEntries        = 0;
  self->conFunc_Register=conFunc_Register;
}

void GALEStressBC_InitAll( 
		void*                                               forceTerm,
                FiniteElementContext*	context,
		ForceVector*                                        forceVector,
		Swarm*                                              integrationSwarm,
                ConditionFunction_Register*			    conFunc_Register)
{
	GALEStressBC* self = (GALEStressBC*) forceTerm;

	_ForceTerm_Init( self, context, forceVector, integrationSwarm, NULL );
	_GALEStressBC_Init( self, conFunc_Register );
}

void _GALEStressBC_Delete( void* forceTerm ) {
	GALEStressBC* self = (GALEStressBC*)forceTerm;

	_ForceTerm_Delete( self );
}

void _GALEStressBC_Print( void* forceTerm, Stream* stream ) {
	GALEStressBC* self = (GALEStressBC*)forceTerm;
	
	_ForceTerm_Print( self, stream );
}

void* _GALEStressBC_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(GALEStressBC);
	Type                                                      type = GALEStressBC_Type;
	Stg_Class_DeleteFunction*                              _delete = _GALEStressBC_Delete;
	Stg_Class_PrintFunction*                                _print = _GALEStressBC_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _GALEStressBC_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _GALEStressBC_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _GALEStressBC_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _GALEStressBC_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _GALEStressBC_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _GALEStressBC_Destroy;
	ForceTerm_AssembleElementFunction*            _assembleElement = _GALEStressBC_AssembleElement;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*)_GALEStressBC_New( GALESTRESSBC_PASSARGS);
}

void _GALEStressBC_AssignFromXML( void* forceTerm, Stg_ComponentFactory* cf, void* data ) {
	GALEStressBC*          self             = (GALEStressBC*)forceTerm;

	/* Construct Parent */
	_ForceTerm_AssignFromXML( self, cf, data );

	_GALEStressBC_Init( self, condFunc_Register );
        {
          char*	wallStr;
		
          /* Obtain which wall */
          wallStr = Stg_ComponentFactory_GetString( cf, self->name, "wall", "" );
          if (!strcasecmp(wallStr, "back"))
            self->_wall = Wall_Back;
          else if (!strcasecmp(wallStr, "left"))
            self->_wall = Wall_Left;
          else if (!strcasecmp(wallStr, "bottom"))
            self->_wall = Wall_Bottom;
          else if (!strcasecmp(wallStr, "right"))
            self->_wall = Wall_Right;
          else if (!strcasecmp(wallStr, "top"))
            self->_wall = Wall_Top;
          else if (!strcasecmp(wallStr, "front"))
            self->_wall = Wall_Front;
          else {
            assert( 0 );
            self->_wall = Wall_Size; /* invalid entry */
          }
        }
        _GALEStressBC_GetValues(cf,self,"x",data);
        _GALEStressBC_GetValues(cf,self,"y",data);
        _GALEStressBC_GetValues(cf,self,"z",data);
          
}

/* Gets the actual values used by the GALEStressBC (e.g. a float or a function). */
void _GALEStressBC_GetValues(Stg_ComponentFactory* cf, void *stressBC,
                         char *direction, void *data)
{
          GALEStressBC*          self             = (GALEStressBC*)stressBC;
          char temp_str[20];
          char *type;

          switch(*direction)
            {
            case 'x':
              self->_entryTbl[self->numEntries].axis=I_AXIS;
              break;
            case 'y':
              self->_entryTbl[self->numEntries].axis=J_AXIS;
              break;
            case 'z':
              self->_entryTbl[self->numEntries].axis=K_AXIS;
              break;
            }

          strcat(strcpy(temp_str,direction),"_type");
          type=Stg_ComponentFactory_GetString( cf, self->name, temp_str, "");
          strcat(strcpy(temp_str,direction),"_value");

          if(!strcasecmp(type,"double") || !strcasecmp(type,"float"))
            {
              self->_entryTbl[self->numEntries].type = GALEStressBC_Double;
              self->_entryTbl[self->numEntries].DoubleValue =
                Stg_ComponentFactory_GetDouble( cf, self->name, temp_str, 0.0);
              (self->numEntries)++;
            }
          else if(!strcasecmp(type,"func"))
            {
              char *funcName =
                Stg_ComponentFactory_GetString( cf, self->name, temp_str, "");
              Index	cfIndex;

              cfIndex = ConditionFunction_Register_GetIndex
                ( self->conFunc_Register, funcName);
              self->_entryTbl[self->numEntries].type =
                GALEStressBC_ConditionFunction;
              if ( cfIndex == (unsigned)-1 ) {	
                Stream*	errorStr = Journal_Register( Error_Type, self->type );
                
                Journal_Printf( errorStr, "Error- in %s: While parsing "
                                "definition of GALEStressBC (applies to wall \"%s\"), the cond. func. applied to "
                                "direction \"%s\" - \"%s\" - wasn't found in the c.f. register.\n",
                                __func__, WallEnumToStr[self->_wall],
                                direction, funcName );
                Journal_Printf( errorStr, "(Available functions in the C.F. register are: ");	
                ConditionFunction_Register_PrintNameOfEachFunc
                  ( self->conFunc_Register, errorStr );
                Journal_Printf( errorStr, ")\n");	
                assert(0);
              }	
              self->_entryTbl[self->numEntries].CFIndex = cfIndex;
              (self->numEntries)++;
            }
          else if(!strcasecmp(type,"HydrostaticTerm"))
            {
              self->_entryTbl[self->numEntries].type = GALEStressBC_HydrostaticTerm;
              self->_entryTbl[self->numEntries].hydrostaticTerm = Stg_ComponentFactory_ConstructByKey( cf, self->name, temp_str, HydrostaticTerm, True, data ) ;
              (self->numEntries)++;
            }
          else if(strlen(type)!=0)
            {
              Stream* errorStr = Journal_Register( Error_Type, self->type );
              Journal_Printf( errorStr, "Error- in %s: While parsing "
                              "definition of GALEStressBC (applies to wall \"%s\"), the type of condition \"%s\"\nis not supported.  Supported types are \"double\" and \"function\".",
                                __func__, WallEnumToStr[self->_wall],
                                type );
              assert(0);
            }
}

void _GALEStressBC_Build( void* forceTerm, void* data ) {
	GALEStressBC*               self               = (GALEStressBC*)forceTerm;
	Name                             name;

	_ForceTerm_Build( self, data );
}

void _GALEStressBC_Initialise( void* forceTerm, void* data ) {
	GALEStressBC*             self             = (GALEStressBC*)forceTerm;
	Index                          i;

	_ForceTerm_Initialise( self, data );

}

void _GALEStressBC_Execute( void* forceTerm, void* data ) {
	_ForceTerm_Execute( forceTerm, data );
}

void _GALEStressBC_Destroy( void* forceTerm, void* data ) {
	_ForceTerm_Destroy( forceTerm, data );
}

void _GALEStressBC_AssembleElement( void* forceTerm, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elForceVec ) {
  GALEStressBC*               self               = (GALEStressBC*) forceTerm;
  Dimension_Index                  dim                = forceVector->dim;
  FeMesh*              mesh               = forceVector->feVariable->feMesh;
  Grid*		grid;
  Node_ElementLocalIndex           eNode_I;
  ElementType*                     elementType;
  Dof_Index                        nodeDofCount;
  double                           stress, area;
  IJK			ijk;
  int overcount;
  IArray *incidence;
  int elementNodeCount;
  int *elementNodes;

  FeVariable* velocityField = NULL;
  velocityField = (FeVariable*)FieldVariable_Register_GetByName
    ( self->context->fieldVariable_Register, "VelocityField" );

  elementType       = FeMesh_GetElementType( mesh, lElement_I );
  nodeDofCount      = dim;
  
  grid=*(Grid**)ExtensionManager_Get(mesh->info, mesh, 
                                     ExtensionManager_GetHandle(mesh->info,
                                                                "vertexGrid"));
  
  incidence=IArray_New();
  area=GALEStressBC_compute_face_area(self->_wall,mesh,lElement_I,dim,incidence);
  elementNodeCount=IArray_GetSize(incidence);
  elementNodes=IArray_GetPtr(incidence);

  if(dim==2)
    {
      overcount=2;
    }
  else
    {
      overcount=4;
    }
  /* Apply the stress */
  for( eNode_I = 0 ; eNode_I < elementNodeCount; eNode_I++ ) {
    /* Make sure that we are on the boundary */
    int condition, entry_I;
    ConditionFunction* cf;
    RegularMeshUtils_Node_1DTo3D
      (mesh,Mesh_DomainToGlobal(mesh,MT_VERTEX,elementNodes[eNode_I]),ijk);
    
    switch(self->_wall)
      {
      case Wall_Left:
        condition=(ijk[0] == 0);
        break;
      case Wall_Right:
        condition=(ijk[0] == ( grid->sizes[0] - 1 ));
        break;
      case Wall_Bottom:
        condition=(ijk[1] == 0);
        break;
      case Wall_Top:
        condition=(ijk[1] == ( grid->sizes[1] - 1 ));
        break;
      case Wall_Front:
        condition=(ijk[2] == 0);
        break;
      case Wall_Back:
        condition=(ijk[2] == ( grid->sizes[2] - 1 ));
        break;
      }
    
    if(condition)
      {
        for(entry_I=0; entry_I<self->numEntries; ++entry_I)
          {
            switch(self->_entryTbl[entry_I].type)
              {
              case GALEStressBC_Double:
                stress=self->_entryTbl[entry_I].DoubleValue;
                break;
              case GALEStressBC_ConditionFunction:
                cf=self->conFunc_Register->
                  _cf[self->_entryTbl[entry_I].CFIndex];
                
                /* We use a variable number of zero "0", because
                   we don't use the variable number and that one
                   is always going to exist. */
                ConditionFunction_Apply(cf,elementNodes[eNode_I],
                                        0,self->context,&stress);
                break;
              case GALEStressBC_HydrostaticTerm:
                stress=
                  -HydrostaticTerm_Pressure(self->_entryTbl[entry_I].hydrostaticTerm,
                                            Mesh_GetVertex(mesh,elementNodes[eNode_I]));
                if(self->_wall!=Wall_Top)
                  {
                    Stream* errorStr=Journal_Register( Error_Type, self->type );
                    Journal_Firewall(0,errorStr,"You can only apply a HydrostaticTerm GALEStressBC to the top wall.\nYou applied it to the %s wall",WallVC_WallEnumToStr[self->_wall]);
                  }

                if(dim==2)
                  {
                    double dx, dy, *coord0, *coord1;
                    /* StGermain uses the ordering

                       0:x,y
                       1:x+,y
                       2:x+,y+
                       3:x,y+

                       So the top two vertices are 2 and 3 */

                    coord0=Mesh_GetVertex(mesh,elementNodes[3]);
                    coord1=Mesh_GetVertex(mesh,elementNodes[2]);

                    dx=coord1[0]-coord0[0];
                    dy=-(coord1[1]-coord0[1]);

                    elForceVec[eNode_I*nodeDofCount + I_AXIS]+=
                      stress*dy/overcount;
                    elForceVec[eNode_I*nodeDofCount + J_AXIS]+=
                      stress*dx/overcount;
                  }
                else
                  {
                    double *coord0,*coord1,*coord2,*coord3,
                      vector0[3],vector1[3],normal[3];
                    
                    /* Decompose the quadrilateral into two triangles.
                       For each triangle, take the cross product and
                       apply the force in that direction. */

                    coord0=Mesh_GetVertex(mesh,elementNodes[2]);
                    coord1=Mesh_GetVertex(mesh,elementNodes[3]);
                    coord2=Mesh_GetVertex(mesh,elementNodes[7]);
                    coord3=Mesh_GetVertex(mesh,elementNodes[6]);
                    
                    StGermain_VectorSubtraction( vector0, coord1, coord0, dim) ;
                    StGermain_VectorSubtraction( vector1, coord2, coord1, dim) ;
                    StGermain_VectorCrossProduct( normal, vector0, vector1 );

                    elForceVec[eNode_I*nodeDofCount + I_AXIS]+=
                      stress*normal[0]/(2*overcount);
                    elForceVec[eNode_I*nodeDofCount + J_AXIS]+=
                      stress*normal[1]/(2*overcount);
                    elForceVec[eNode_I*nodeDofCount + K_AXIS]+=
                      stress*normal[2]/(2*overcount);

                    StGermain_VectorSubtraction( vector0, coord2, coord1, dim) ;
                    StGermain_VectorSubtraction( vector1, coord3, coord2, dim) ;
                    StGermain_VectorCrossProduct( normal, vector0, vector1 );

                    elForceVec[eNode_I*nodeDofCount + I_AXIS]+=
                      stress*normal[0]/(2*overcount);;
                    elForceVec[eNode_I*nodeDofCount + J_AXIS]+=
                      stress*normal[1]/(2*overcount);;
                    elForceVec[eNode_I*nodeDofCount + K_AXIS]+=
                      stress*normal[2]/(2*overcount);;
                  }
                break;
              }
            /* We have to divide by an overcount_factor, because
               otherwise different elements will count the same node
               more than once.  In 2D, nodes are counted twice, in 3D,
               nodes are counted four times. */
            if(self->_entryTbl[entry_I].type!=GALEStressBC_HydrostaticTerm)
              elForceVec[eNode_I*nodeDofCount + self->_entryTbl[entry_I].axis]+=
                stress*area/overcount;
          }
      }
  }
  Stg_Class_Delete(incidence);
}


double GALEStressBC_compute_face_area(Wall wall, FeMesh *mesh, Index lElement_I, 
                                  Dimension_Index dim,
                                  IArray *incidence)
{
  /* Compute the area of the face. */
  if(dim==2)
    {
      double *coord1, *coord2;
      int lower,upper,direction;
      int *elementNodes;
      switch(wall)
        {
        case Wall_Left:
          lower=0;
          upper=3;
          direction=1;
          break;
        case Wall_Right:
          lower=1;
          upper=2;
          direction=1;
          break;
        case Wall_Bottom:
          lower=0;
          upper=1;
          direction=0;
          break;
        case Wall_Top:
          lower=3;
          upper=2;
          direction=0;
          break;
        }

      Mesh_GetIncidence(mesh, Mesh_GetDimSize(mesh), lElement_I,
                        MT_VERTEX,incidence);
      elementNodes=IArray_GetPtr(incidence);
                              
      coord1=Mesh_GetVertex(mesh,elementNodes[lower]);
      coord2=Mesh_GetVertex(mesh,elementNodes[upper]);
      return coord2[direction]-coord1[direction];
    }
  else
    {
      double *coord1, *coord2, *coord3, *coord4;
      int first, second, third, fourth;
      int *elementNodes;
                              
      /* StGermain uses the ordering
         0: x,y,z
         1: x+,y,z
         2: x,y+,z
         3: x+,y+,z
         4: x,y,z+
         5: x+,y,z+
         6: x,y+,z+
         7: x+,y+,z+
      */

      /* Get the indices for which wall we want to get the area of. */
      switch(wall)
        {
        case Wall_Left:
          first=0;
          second=2;
          third=6;
          fourth=4;
          break;
        case Wall_Right:
          first=1;
          second=3;
          third=7;
          fourth=5;
          break;
        case Wall_Bottom:
          first=0;
          second=1;
          third=5;
          fourth=4;
          break;
        case Wall_Top:
          first=2;
          second=3;
          third=7;
          fourth=6;
          break;
        case Wall_Front:
          first=0;
          second=1;
          third=4;
          fourth=3;
          break;
        case Wall_Back:
          first=4;
          second=5;
          third=7;
          fourth=6;
          break;
        }

      Mesh_GetIncidence(mesh, Mesh_GetDimSize(mesh), lElement_I,
                        MT_VERTEX,incidence);
      elementNodes=IArray_GetPtr(incidence);

      coord1=Mesh_GetVertex(mesh,elementNodes[first]);
      coord2=Mesh_GetVertex(mesh,elementNodes[second]);
      coord3=Mesh_GetVertex(mesh,elementNodes[third]);
      coord4=Mesh_GetVertex(mesh,elementNodes[fourth]);

      return StGermain_ConvexQuadrilateralArea(coord1,coord2,coord3,coord4,
                                               dim);
    }
}

