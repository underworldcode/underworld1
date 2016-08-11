/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**	Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**	Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**	AuScope - http://www.auscope.org
**	Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**	Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**	Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**	Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**	Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
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

#include <string.h>
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <assert.h>

#include "FieldVariableConditionFunction.h"

const Type FieldVariableConditionFunction_Type = "Underworld_FieldVariableConditionFunction";

void _FieldVariableConditionFunction_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
	FieldVariableConditionFunction* self = (FieldVariableConditionFunction*)component;
	Dictionary*		pluginDict	= Codelet_GetPluginDictionary( component, cf->rootDict );

	/** field variable which we will interrogate to determine values */
   self->fieldVariable = Stg_ComponentFactory_PluginConstructByKey( cf, self, (Dictionary_Entry_Key)"FieldVariable", FieldVariable, True, data );
	/** target fevariable */
   self->targetfeVariable = Stg_ComponentFactory_PluginConstructByKey( cf, self, (Dictionary_Entry_Key)"TargetFeVariable", FeVariable, True, data  );
	self->failTolerance = Dictionary_GetDouble_WithDefault( pluginDict, (Dictionary_Entry_Key)"FailureTolerance" , 0.001 );
	
	ConditionFunction_Register_Add( condFunc_Register, ConditionFunction_New( FieldVariableConditionFunction_getVal, (Name)self->type, NULL ) );

}

void _FieldVariableConditionFunction_Build( void* component, void* data ) {
	FieldVariableConditionFunction* self = (FieldVariableConditionFunction*)component;

   Stg_Component_Build( self->fieldVariable, data, False );
   Stg_Component_Build( self->targetfeVariable, data, False );
   /** also initialise now */
   Stg_Component_Initialise( self->fieldVariable, data, False );
   /** just set this large enough for a 3d vector */
   self->value = Memory_Alloc_Array( double, 3, "interValue" );         

}

void _FieldVariableConditionFunction_Destroy( void* component, void* data ) {
	FieldVariableConditionFunction* self = (FieldVariableConditionFunction*)component;
   /** our work is done, so we clean up after ourselves */
   Stg_Component_Destroy(self->fieldVariable, NULL, False);
   Memory_Free( self->value );

}

void* _FieldVariableConditionFunction_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(FieldVariableConditionFunction);
	Type                                                      type = FieldVariableConditionFunction_Type;
	Stg_Class_DeleteFunction*                              _delete = _Codelet_Delete;
	Stg_Class_PrintFunction*                                _print = _Codelet_Print;
	Stg_Class_CopyFunction*                                  _copy = _Codelet_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _FieldVariableConditionFunction_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _FieldVariableConditionFunction_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _FieldVariableConditionFunction_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Codelet_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Codelet_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _FieldVariableConditionFunction_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return _Codelet_New(  CODELET_PASSARGS  );
}

Index Underworld_FieldVariableConditionFunction_Register( PluginsManager* pluginsManager ) {

	return PluginsManager_Submit( pluginsManager, FieldVariableConditionFunction_Type, (Name)"0", _FieldVariableConditionFunction_DefaultNew  );
}

void FieldVariableConditionFunction_getVal( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ){
   DomainContext*  context = (DomainContext*)_context;
   double*                 coord;
	InterpolationResult	   retValue;
   Stream*                 errorStr = Journal_Register( Error_Type, FieldVariableConditionFunction_Type  );
	
   FieldVariableConditionFunction* self = (FieldVariableConditionFunction*)LiveComponentRegister_Get( context->CF->LCRegister, (Name) FieldVariableConditionFunction_Type );

   /* if the node is not on the current, local mesh don't interpolate the solution */
   if( node_lI >= Mesh_GetLocalSize( self->targetfeVariable->feMesh, MT_VERTEX ) )
     return; 
      
   coord = Mesh_GetVertex( self->targetfeVariable->feMesh, node_lI );

   /** interpolating the required values from the provided feVariable */
   retValue = FieldVariable_InterpolateValueAt( self->fieldVariable, coord, self->value);

   if(retValue == OTHER_PROC || retValue == OUTSIDE_GLOBAL){
      /* due to differences in machine precision, points of interrogation can sometimes appear to be outside the domain. 
         we perturb the interpolation coordinate to see if this fixes the issue. default tolerance is 0.001 the local domain size. 
         sometimes, where a deformed mesh is used, a greater tolerance may be required (as deformation may depend on resolution). */
      double localSizeMin[3], localSizeMax[3], deltaCoord[3];
      double testCoord[3];
      unsigned ii,jj;
      int updown[2] = {-1,1};

      /* determine perturbation */
      Mesh_GetLocalCoordRange( self->targetfeVariable->feMesh, localSizeMin, localSizeMax );      
      for( ii=0; ii< Mesh_GetDimSize(self->targetfeVariable->feMesh); ii++) deltaCoord[ii] = self->failTolerance*(localSizeMax[ii] - localSizeMin[ii]);
      
      /* step coord through +/- perturbation, one dimension at a time, and test */
      ii = 0;
      while( (retValue == OTHER_PROC || retValue == OUTSIDE_GLOBAL) && ii < Mesh_GetDimSize(self->targetfeVariable->feMesh) ){
         for( jj=0; jj< 2; jj++){
            memcpy( testCoord, coord, Mesh_GetDimSize(self->targetfeVariable->feMesh)*sizeof(double) );
            testCoord[ii] += updown[jj]*deltaCoord[ii];
            retValue = FieldVariable_InterpolateValueAt( self->fieldVariable, testCoord, self->value);
            if(!(retValue == OTHER_PROC || retValue == OUTSIDE_GLOBAL)) break;  /* if success, break */
         }
         ii++;
      }
      if(retValue == OTHER_PROC || retValue == OUTSIDE_GLOBAL){                  /* if still no success, report and bail */
         FieldVariable_GetMinAndMaxLocalCoords( self->fieldVariable, localSizeMin, localSizeMax );
         if(Mesh_GetDimSize(self->targetfeVariable->feMesh) == 2)
            Journal_Firewall(!(retValue == OTHER_PROC || retValue == OUTSIDE_GLOBAL), 
               errorStr,"\n\nError while trying to interpolate field for FieldVariable Initial Condition.\n"
               "Query location appears outside local domain min or max:\n\n   Location:  %f,%f\n\n   min:  %f,%f\n   max:  %f,%f.\n\n"
               "A tolerance of 0.001*(max-min) is used by default, though you may increase the factor by setting \'FailureTolerance\' within the plugin dictionary.\n\n",
               coord[0], coord[1], localSizeMin[0], localSizeMin[1],localSizeMax[0], localSizeMax[1] );
         else
            Journal_Firewall(!(retValue == OTHER_PROC || retValue == OUTSIDE_GLOBAL), 
               errorStr,"\n\nError while trying to interpolate field for FieldVariable Initial Condition.\n"
               "Query location appears outside local domain min or max:\n\n   Query Location:  %f,%f,%f\n\n   min:  %f,%f,%f\n   max:  %f,%f,%f.\n\n"
               "A tolerance of 0.001*(max-min) is used by default, though you may increase the factor by setting \'FailureTolerance\' within the plugin dictionary.\n\n",
               coord[0], coord[1], coord[2], localSizeMin[0], localSizeMin[1], localSizeMin[2], localSizeMax[0], localSizeMax[1], localSizeMax[2] );            
      }
      
   }
   FeVariable_SetValueAtNode( self->targetfeVariable, node_lI, self->value);
}



