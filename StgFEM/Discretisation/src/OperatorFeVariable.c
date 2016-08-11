/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**   Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**   Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**   AuScope - http://www.auscope.org
**   Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**   Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**   Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**   Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**   Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
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

#include "types.h"
#include "FeVariable.h"
#include "OperatorFeVariable.h"
#include "FeMesh.h"

#include <assert.h>

const Type OperatorFeVariable_Type = "OperatorFeVariable";

OperatorFeVariable* OperatorFeVariable_NewUnary( Name name, DomainContext* context, void* _operand, Name operatorName ) {
   FeVariable* operand = (FeVariable*)_operand;
   Stream*     errorStream = Journal_Register( Error_Type, (Name)OperatorFeVariable_Type );

   Journal_Firewall( operand != NULL, errorStream,
      "In func %s: Trying to operate on NULL field.\n", __func__ );

   return OperatorFeVariable_New( 
      name,
      context,
      operand->feMesh,
      operand->dofLayout,
      operand->bcs,
      operand->ics,
      operand->linkedDofInfo,
      operand->templateFeVariable,
      OperatorFeVariable_UnaryInterpolationFunc, 
      OperatorFeVariable_UnaryValueAtNodeFunc,
      operatorName,
      NULL,
      1,
      &operand,
      operand->dim,
      operand->isCheckpointedAndReloaded,
      operand->communicator,
      operand->fieldVariable_Register ); 
}

OperatorFeVariable* OperatorFeVariable_NewUnary_OwnOperator( Name name, DomainContext* context, void* _operand, Operator* ownOperator ) {
   FeVariable* operand = (FeVariable*)_operand;
   Stream*     errorStream = Journal_Register( Error_Type, (Name)OperatorFeVariable_Type );

   Journal_Firewall( operand != NULL, errorStream,
      "In func %s: Trying to operate on NULL field.\n", __func__ );
          
   return OperatorFeVariable_New( 
      name,
      context,
      operand->feMesh,
      operand->dofLayout,
      operand->ics,
      operand->bcs,
      operand->linkedDofInfo,
      operand->templateFeVariable,
      OperatorFeVariable_UnaryInterpolationFunc, 
      OperatorFeVariable_UnaryValueAtNodeFunc,
      ownOperator->name,
      ownOperator,
      1,
      &operand,
      operand->dim,
      operand->isCheckpointedAndReloaded,
      operand->communicator,
      operand->fieldVariable_Register ); 
}

OperatorFeVariable* OperatorFeVariable_NewBinary( Name name, DomainContext* context, void* _operand0, void* _operand1, Name operatorName ) {
   FeVariable* operands[2];
   Stream*     errorStream = Journal_Register( Error_Type, (Name)OperatorFeVariable_Type );
   
   Journal_Firewall( _operand0 != NULL, errorStream,
      "In func %s: First operand to operate on is NULL.\n", __func__ );

   Journal_Firewall( _operand1 != NULL, errorStream,
      "In func %s: Second operand to operate on is NULL.\n", __func__ );
       
   operands[0] = (FeVariable*)_operand0;
   operands[1] = (FeVariable*)_operand1;
   
   return OperatorFeVariable_New( 
      name,
      context,
      operands[0]->feMesh,
      operands[0]->dofLayout,
      operands[0]->bcs,
      operands[0]->ics,
      operands[0]->linkedDofInfo,
      operands[0]->templateFeVariable,
      OperatorFeVariable_BinaryInterpolationFunc,
      OperatorFeVariable_BinaryValueAtNodeFunc,
      operatorName,
      NULL,
      2, 
      operands,
      operands[0]->dim,
      operands[0]->isCheckpointedAndReloaded,
      operands[0]->communicator,
      operands[0]->fieldVariable_Register );
}

OperatorFeVariable* OperatorFeVariable_New( 
   Name                                         name,
   DomainContext*                               context,
   void*                                        feMesh,
   DofLayout*                                   dofLayout,
   void*                                        bcs,
   void*                                        ics,
   void*                                        linkedDofInfo,
   void*                                        templateFeVariable,
   FeVariable_InterpolateWithinElementFunction* interpolateWithinElement,
   FeVariable_GetValueAtNodeFunction*           getValueAtNode,
   Name                                         operatorName,
   Operator*                                    ownOperator,
   Index                                        operandCount,
   FeVariable**                                 operands,
   Dimension_Index                              dim,
   Bool                                         isCheckpointedAndReloaded,
   MPI_Comm                                     communicator,
   FieldVariable_Register*                      fieldVariable_Register )
{
   OperatorFeVariable* self = _OperatorFeVariable_DefaultNew( name );

   self->isConstructed = True;

   _FieldVariable_Init( (FieldVariable*)self, context, operandCount, dim,
      isCheckpointedAndReloaded, NULL, communicator, fieldVariable_Register, False );
   
   _FeVariable_Init( (FeVariable*)self, feMesh, dofLayout, bcs,False,
      ics, linkedDofInfo, templateFeVariable, True, False ); 

   _OperatorFeVariable_Init( self, operatorName, operandCount, operands, ownOperator );

   return self;
}

void* _OperatorFeVariable_DefaultNew( Name name ) {
   /* Variables set in this function */
   SizeT                                                      _sizeOfSelf = sizeof(OperatorFeVariable);
   Type                                                              type = OperatorFeVariable_Type;
   Stg_Class_DeleteFunction*                                      _delete = _OperatorFeVariable_Delete;
   Stg_Class_PrintFunction*                                        _print = _OperatorFeVariable_Print;
   Stg_Class_CopyFunction*                                          _copy = _OperatorFeVariable_Copy;
   Stg_Component_DefaultConstructorFunction*          _defaultConstructor = (Stg_Component_DefaultConstructorFunction*)_OperatorFeVariable_DefaultNew;
   Stg_Component_ConstructFunction*                            _construct = _OperatorFeVariable_AssignFromXML;
   Stg_Component_BuildFunction*                                    _build = _OperatorFeVariable_Build;
   Stg_Component_InitialiseFunction*                          _initialise = _OperatorFeVariable_Initialise;
   Stg_Component_ExecuteFunction*                                _execute = _OperatorFeVariable_Execute;
   Stg_Component_DestroyFunction*                                _destroy = _OperatorFeVariable_Destroy;
   AllocationType                                      nameAllocationType = NON_GLOBAL;
   FieldVariable_InterpolateValueAtFunction*          _interpolateValueAt = _OperatorFeVariable_InterpolateValueAt;
   FieldVariable_GetValueFunction*            _getMinGlobalFieldMagnitude = _OperatorFeVariable_GetMinGlobalFieldMagnitude;
   FieldVariable_GetValueFunction*            _getMaxGlobalFieldMagnitude = _OperatorFeVariable_GetMaxGlobalFieldMagnitude;
   FieldVariable_CacheValuesFunction*    _cacheMinMaxGlobalFieldMagnitude = _OperatorFeVariable_CacheMinMaxGlobalFieldMagnitude;
   FieldVariable_GetCoordFunction*               _getMinAndMaxLocalCoords = _OperatorFeVariable_GetMinAndMaxLocalCoords;
   FieldVariable_GetCoordFunction*              _getMinAndMaxGlobalCoords = _OperatorFeVariable_GetMinAndMaxGlobalCoords;
   FeVariable_InterpolateWithinElementFunction* _interpolateWithinElement = _OperatorFeVariable_InterpolateWithinElement;
   FeVariable_GetValueAtNodeFunction*                     _getValueAtNode = _OperatorFeVariable_GetValueAtNode;
   FeVariable_SyncShadowValuesFunc*                     _syncShadowValues = _OperatorFeVariable_SyncShadowValues;

   return _OperatorFeVariable_New( FEOPERATOR_PASSARGS );
}

OperatorFeVariable* _OperatorFeVariable_New( FEOPERATOR_DEFARGS ) {
   OperatorFeVariable* self;
   
   /* Allocate memory */
   assert( _sizeOfSelf >= sizeof(OperatorFeVariable) );
   self = (OperatorFeVariable*)_FeVariable_New( FEVARIABLE_PASSARGS );

   return self;
}

void _OperatorFeVariable_Init( void* feOperator, Name operatorName, Index operandCount, FeVariable** operands, Operator* ownOperator ) {
   OperatorFeVariable* self = (OperatorFeVariable*)feOperator;
   Index               operand_I;
   Stream*             errorStream = Journal_Register( Error_Type, (Name)self->type );

   self->debug = Stream_RegisterChild( StgFEM_Discretisation_Debug, self->type );

   /* Initialise operands container. */
   self->operands = Stg_ObjectList_New();

   /* Assign values to object */
   self->operandCount = operandCount;
   self->operatorName = operatorName;
   self->_operator = ownOperator;
   self->dictionary = self->context->CF->componentDict;
   self->myDict = Dictionary_Entry_Value_AsDictionary(
      Dictionary_Get( self->dictionary, (Dictionary_Entry_Key)self->name ) );

   /* Add each operands to the operands list for this OperatorFeVariable. */
   for( operand_I = 0; operand_I < operandCount; operand_I++ ) {
      Journal_Firewall( operands[operand_I] != NULL, errorStream,
         "In func %s: Operand %u in list is NULL.\n", __func__, operand_I );

      Stg_ObjectList_Append( self->operands, operands[operand_I] );
   }
}

void _OperatorFeVariable_Delete( void* feOperator ) {
   OperatorFeVariable* self = (OperatorFeVariable*)feOperator;

   _FeVariable_Delete( self );
}

void _OperatorFeVariable_Print( void* feOperator, Stream* stream ) {
   OperatorFeVariable* self = (OperatorFeVariable*)feOperator;
   Index       operand_I;

   _FeVariable_Print( self, stream );
   Journal_PrintValue( stream, self->operandCount );

   for( operand_I = 0; operand_I < self->operandCount; operand_I++ ) {
      Journal_Printf( stream, "\tFeVariable %u - '%s'\n", operand_I,
         ((FeVariable*)Stg_ObjectList_At( self->operands, operand_I ))->name );
   }   
}

void* _OperatorFeVariable_Copy( void* feOperator, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
   OperatorFeVariable* self = (OperatorFeVariable*)feOperator;
   OperatorFeVariable* newOperatorFeVariable;
   FeVariable**        operands;
   Index               operand_I;

   operands = Memory_Alloc_Array( FeVariable*, self->operandCount, "FOperands-Copy" );

   for( operand_I = 0; operand_I < self->operandCount; operand_I++ ) 
      operands[ operand_I ] = (FeVariable*)Stg_ObjectList_At( self->operands, operand_I );

   newOperatorFeVariable = OperatorFeVariable_New(
      self->name,
      self->context,
      self->feMesh,
      self->dofLayout,
      self->bcs,
      self->ics,
      self->linkedDofInfo,
      self->templateFeVariable,
      OperatorFeVariable_UnaryInterpolationFunc,
      OperatorFeVariable_UnaryValueAtNodeFunc,
      self->operatorName,
      self->_operator,
      self->operandCount,
      operands,
      self->dim,
      self->isCheckpointedAndReloaded,
      self->communicator,
      self->fieldVariable_Register );
   
   return (void*)newOperatorFeVariable;
}

void _OperatorFeVariable_AssignFromXML( void* feOperator, Stg_ComponentFactory* cf, void* data ) {
   OperatorFeVariable*     self = (OperatorFeVariable*)feOperator;
   Dictionary*             dictionary = Dictionary_GetDictionary( cf->componentDict, self->name );
   Dictionary_Entry_Value* list;
   Index                   operandCount = 0;
   Index                   operand_I;
   Name                    operandName;
   Name                    operatorName;
   FeVariable*             operand;
   FeVariable**            operands;
   Stream*                 errorStream = Journal_Register( Error_Type, (Name)self->type );

   /* 
    * NOTE:
    * This component is a special case where it calls its ancestor's *_AssignFromXML
    * instead of its immediate parent's. This is because we don't want to re-declare 
    * the required FeVariable parameters in this component's XML block.
    */
   _FieldVariable_AssignFromXML( self, cf, data );

   /* Get the operator name from the dictionary. */
   operatorName = Stg_ComponentFactory_GetString( cf, self->name,
      (Dictionary_Entry_Key)"Operator", "" );

   /* Get the list ond number of operands if it exist. */
   list = Dictionary_Get( dictionary, (Dictionary_Entry_Key)"Operands" );
   operandCount = ( list ? Dictionary_Entry_Value_GetCount(list) : 1 );

   /* Allocate memory for the operands. */
   operands = Memory_Alloc_Array( FeVariable*, operandCount, "FOperands" );

   /* Loop through the operands and initialise them. */
   for( operand_I = 0; operand_I < operandCount; operand_I++ ) {
      /* Get the name of the current operand. */ 
      operandName = (list ?
         Dictionary_Entry_Value_AsString(
            Dictionary_Entry_Value_GetElement( list, operand_I ) ) :
            Dictionary_GetString( dictionary, (Dictionary_Entry_Key)"Operand" ) );

      /* Get the field if it exists in the LiveComponentRegister. */
      operand = (FeVariable*)LiveComponentRegister_Get( cf->LCRegister, operandName );

      /* If the operand is not found, try to construct it. */
      if( !operand ) {
         /* Make sure that the operand is constructed. */
         operand = Stg_ComponentFactory_ConstructByName( cf, (Name)operandName,
            FeVariable, True, data ); 

         Journal_Firewall( operand->fieldComponentCount <= MAX_FIELD_COMPONENTS, errorStream,
            "In func %s: Field Variable '%s' has too many components.\n", __func__, operand->name );
      }
      operands[ operand_I ] = (FeVariable*)operand;
   }

   /* 
    * NOTE:
    * The parent's *_Init function needs to be called to initialise
    * FeVariable-specific attributes.
    */
   _FeVariable_Init( (FeVariable*)self, operands[0]->feMesh, operands[0]->dofLayout,
      NULL, False, NULL, NULL, NULL, True, False );

   _OperatorFeVariable_Init( self, operatorName, operandCount, operands, NULL );
}

void _OperatorFeVariable_Build( void* feOperator, void* data ) {
   OperatorFeVariable* self = (OperatorFeVariable*)feOperator;
   Index               operand_I;
   Stream*             errorStream = Journal_Register( Error_Type, (Name)self->type );
   FeVariable*         operand0;
   FeVariable*         operand1;

   _FeVariable_Build( self, data );

   for( operand_I = 0; operand_I < self->operandCount; operand_I++ ) {
      operand0 = (FeVariable*)Stg_ObjectList_At( self->operands, operand_I );
      Stg_Component_Build( operand0, data, False );
   }

   operand0 = (FeVariable*)Stg_ObjectList_At( self->operands, 0 );

   if( !self->_operator ) {
      /* Check if we are using a gradient operator. */
      if( strcasecmp( self->operatorName, "gradient" ) == 0 ) {
         self->useGradient = True;
         assert( self->operandCount == 1 );
         self->fieldComponentCount = operand0->fieldComponentCount * self->dim;
      }
      else {
         /* Just use normal operator. */
         self->useGradient = False;

         /* 
          * Added 5 May 2006 by P Sunter: in the case of VectorScale, 
          * the fieldComponentCount should be based on the 2nd operator. 
          * Also make sure the 2nd operator has at least as may dofs per node as the first. 
          */
         if( self->operandCount == 2 ) {
            operand1 = (FeVariable*)Stg_ObjectList_At( self->operands, 1 );

            /* Make sure that the component counts of the 2 operands match. */ 
            Journal_Firewall(
               operand1->fieldComponentCount >= operand0->fieldComponentCount, errorStream,
               "Error - in %s: tried to create a %s operator from feOperators \"%s\" "
               "and \"%s\" - who have fieldCounts %d and %d - unsupported since operations "
               "such as VectorScale require the 2nd feOperator to have >= the first's field count.\n",
               __func__, self->operatorName,
               operand0->name, operand1->name,
               operand0->fieldComponentCount, operand1->fieldComponentCount );

            self->_operator = Operator_NewFromName( self->operatorName, operand1->fieldComponentCount, self->dim );
         }
         else 
            self->_operator = Operator_NewFromName( self->operatorName, operand0->fieldComponentCount, self->dim );

         /* Reset this value from that which is from operator. */
         self->fieldComponentCount = self->_operator->resultDofs;
      }
   }
   else {
      self->useGradient = False;
      /* Reset this value from that which is from operator. */
      self->fieldComponentCount = self->_operator->resultDofs;
   }

   /* 
    * Update the dictionary with the latest fieldComponentCount values. 
    * Perhaps, a more elegant solution is for the dictionary to be updated based on any on the fly
    * overwriting of parameters from the code itself. JR 09272011.
    */
   if( self->myDict ) {
      Dictionary_Set( self->myDict, (Dictionary_Entry_Key)"fieldComponentCount",
         Dictionary_Entry_Value_FromUnsignedInt( self->fieldComponentCount ) );
   }
      
   _OperatorFeVariable_SetFunctions( self );
}

void _OperatorFeVariable_Initialise( void* feOperator, void* data ) {
   OperatorFeVariable*     self = (OperatorFeVariable*)feOperator;
   DomainContext*          context = self->context;
   Index                   operand_I;
   Dictionary_Entry_Value* operands = NULL;
   FeVariable*             operand = NULL;

   for( operand_I = 0; operand_I < self->operandCount; operand_I++ ) {
      /* Get the first operand of the current OperatorFeVariable. */
      operand = (FeVariable*)Stg_ObjectList_At( self->operands, operand_I );
      Stg_Component_Initialise( (FeVariable*)operand, data, False );
   }

   /*
    * Also include check to see if this fevariable should be checkpointed,
    * just incase it didn't go through the fieldvariable construct phase 
    */ 
   operands = Dictionary_Get(
      context->dictionary,
      (Dictionary_Entry_Key)"fieldVariablesToCheckpoint" );

   if( NULL == operands ) 
      operands = Dictionary_Get( context->dictionary, (Dictionary_Entry_Key)"FieldVariablesToCheckpoint" );

   if( operands != NULL ) {
      Index                   listLength = Dictionary_Entry_Value_GetCount( operands );
      Index                   var_I = 0;
      Dictionary_Entry_Value* operandDictValue = NULL;
      char*                   fieldVariableName;
   
      for( var_I = 0; var_I < listLength; var_I++ ) {
         operandDictValue = Dictionary_Entry_Value_GetElement( operands, var_I );
         fieldVariableName = Dictionary_Entry_Value_AsString( operandDictValue ); 

         if( 0 == strcmp( self->name, fieldVariableName ) ) {
            self->isCheckpointedAndReloaded = True;
            break;
         }
      }
   }

   operands = NULL;
   /* Also include check to see if this fevariable should be saved for analysis purposes */ 
   operands = Dictionary_Get( context->dictionary, (Dictionary_Entry_Key)"fieldVariablesToSave" );

   if( NULL == operands ) 
      operands = Dictionary_Get( context->dictionary, (Dictionary_Entry_Key)"FieldVariablesToSave" );

   if( operands != NULL ) {
      Index                   listLength = Dictionary_Entry_Value_GetCount( operands );
      Index                   var_I = 0;
      Dictionary_Entry_Value* operandDictValue = NULL;
      char*                   fieldVariableName;
   
      for( var_I = 0; var_I < listLength; var_I++ ) {
         operandDictValue = Dictionary_Entry_Value_GetElement( operands, var_I );
         fieldVariableName = Dictionary_Entry_Value_AsString( operandDictValue ); 

         if( 0 == strcmp( self->name, fieldVariableName ) ) {
            self->isSavedData = True;
            break;
         }
      }
   }
}

void _OperatorFeVariable_Execute( void* feOperator, void* data ) {
}

void _OperatorFeVariable_Destroy( void* feOperator, void* data ) {
   OperatorFeVariable* self = (OperatorFeVariable*) feOperator;

   if( self->operands ) {
      Stg_Class_Delete( self->operands );
      self->operands = NULL;
   }

   _FeVariable_Destroy( self, data );
}

void _OperatorFeVariable_SetFunctions( void* feOperator ) {
   OperatorFeVariable* self = (OperatorFeVariable*) feOperator;
   Stream*     error = Journal_Register( Error_Type, (Name)self->type );

   if( self->useGradient ) {
      Journal_Firewall( self->operandCount == 1, error, "Cannot use gradient operators for multiple variables.\n" );

      self->_interpolateWithinElement = OperatorFeVariable_GradientInterpolationFunc;
      self->_getValueAtNode = OperatorFeVariable_GradientValueAtNodeFunc;
   }
   else {
      switch( self->operandCount ) {
         case 1:
            self->_interpolateWithinElement = OperatorFeVariable_UnaryInterpolationFunc; 
            self->_getValueAtNode = OperatorFeVariable_UnaryValueAtNodeFunc;
            break;
         case 2:
            self->_interpolateWithinElement = OperatorFeVariable_BinaryInterpolationFunc; 
            self->_getValueAtNode = OperatorFeVariable_BinaryValueAtNodeFunc;
            break;
         default: 
            Journal_Firewall( False, error, "Cannot use '%s' with operandCount = %u.\n", __func__, self->operandCount );
            break;
      }
   }
}

/** Private Functions */
Bool _OperatorFeVariable_CheckInterpolateShadowSpaceReady( OperatorFeVariable* self ) {
   int         operand_I;
   FeVariable* operand;
   Bool        parentIsValid;

   for( operand_I = 0; operand_I < self->operandCount; operand_I++ ) {
      /* Get the first operand of the current OperatorFeVariable. */
      operand = (FeVariable*)Stg_ObjectList_At( self->operands, operand_I );

      if( Stg_Class_IsInstance( operand, FeVariable_Type ) ) {
         if( False == operand->shadowValuesSynchronised ) 
            return False;
         else 
            return True;
      }
      else {
         parentIsValid = _OperatorFeVariable_CheckInterpolateShadowSpaceReady( (OperatorFeVariable*)operand );
         if( False == parentIsValid ) 
            return False;
      } 
   }
   return True;
}

/*
 * Needed to over-ride the standard _FeVariable_InterpolateValueAt() since for operator fe variables, 
 * as long as the base fe variable has been shadowed, 
 * this operatorFeVar can be calculated in shadow space. 
 */
InterpolationResult _OperatorFeVariable_InterpolateValueAt( void* feOperator, Coord globalCoord, double* value ) {
   OperatorFeVariable*         self = (OperatorFeVariable*)feOperator;
   Element_DomainIndex elementCoordIn = (unsigned)-1;
   Coord               elLocalCoord = {0,0,0};
   InterpolationResult retValue;

   retValue = FeVariable_GetElementLocalCoordAtGlobalCoord( self, globalCoord, elLocalCoord, &elementCoordIn );

   /* Now interpolate the value at that coordinate, using shape functions. */
   if( retValue == LOCAL ) 
      self->_interpolateWithinElement( self, elementCoordIn, elLocalCoord, value );

   else if( retValue == SHADOW ) {
      /* Check that interpolation on shadow space is ready. */   
      if( !_OperatorFeVariable_CheckInterpolateShadowSpaceReady( self ) ) {
         Stream* warningStr = Journal_Register( Error_Type, (Name)self->type );
         Journal_Printf( warningStr,
            "Warning - in %s, for OperatorFeVariable \"%s\": user asking to "
            "interpolate a value at "
            "coord(%g,%g,%g), which is in shadow space, but "
            "FeVariable_SyncShadowValues() hasn't been called on FeVariables this one is "
            "derived from yet.\n",
            __func__,
            self->name,
            globalCoord[0], globalCoord[1], globalCoord[2] );

         return retValue;
      }
      /* Now interpolate the value at that coordinate, using shape functions. */
      self->_interpolateWithinElement( self, elementCoordIn, elLocalCoord, value );
   }
   return retValue;
}

double _OperatorFeVariable_GetMinGlobalFieldMagnitude( void* feOperator ) {
   OperatorFeVariable* self = (OperatorFeVariable*)feOperator;

   return FieldVariable_GetMinGlobalFieldMagnitude( self );
}

double _OperatorFeVariable_GetMaxGlobalFieldMagnitude( void* feOperator ) {
   OperatorFeVariable* self = (OperatorFeVariable*)feOperator;

   return FieldVariable_GetMaxGlobalFieldMagnitude( self );
}

void _OperatorFeVariable_CacheMinMaxGlobalFieldMagnitude( void* feOperator ) {
   _FeVariable_CacheMinMaxGlobalFieldMagnitude( feOperator );
}

void _OperatorFeVariable_GetMinAndMaxLocalCoords( void* feOperator, double* min, double* max ) {
   _FeVariable_GetMinAndMaxLocalCoords( feOperator, min, max );
}

void _OperatorFeVariable_GetMinAndMaxGlobalCoords( void* feOperator, double* min, double* max ) {
   _FeVariable_GetMinAndMaxGlobalCoords( feOperator, min, max );
}

void _OperatorFeVariable_InterpolateWithinElement( void* feOperator, Element_DomainIndex dElement_I, Coord coord, double* value ) {
   OperatorFeVariable* self = (OperatorFeVariable*)feOperator;

   _OperatorFeVariable_SetFunctions( self );
   FeVariable_InterpolateWithinElement( self, dElement_I, coord, value );
}

void _OperatorFeVariable_GetValueAtNode( void* feOperator, Node_DomainIndex dNode_I, double* value ) {
   OperatorFeVariable* self = (OperatorFeVariable*)feOperator;

   _OperatorFeVariable_SetFunctions( self );
   FeVariable_GetValueAtNode( self, dNode_I, value );
}

void _OperatorFeVariable_SyncShadowValues( void* feOperator ) {
   OperatorFeVariable* self = (OperatorFeVariable*)feOperator;
   FeVariable* operand;
   int         operand_I;

   assert( self );
   for( operand_I = 0; operand_I < self->operandCount; operand_I++ ) {
      operand = (FeVariable*)Stg_ObjectList_At( self->operands, operand_I );
      FeVariable_SyncShadowValues( operand );
   }
   self->shadowValuesSynchronised = True;
}

void OperatorFeVariable_UnaryInterpolationFunc( void* feOperator, Element_DomainIndex dElement_I, Coord localCoord, double* value ) {
   OperatorFeVariable* self = (OperatorFeVariable*)feOperator;
   double      fieldValue0[ MAX_FIELD_COMPONENTS ];
   FeVariable* operand;

   /* Get the first operand of the current OperatorFeVariable. */
   operand = (FeVariable*)Stg_ObjectList_At( self->operands, 0 );
   operand->_interpolateWithinElement( operand, dElement_I, localCoord, fieldValue0 );
   Operator_CarryOutUnaryOperation( self->_operator, fieldValue0, value );

#ifdef DEBUG
   if( Stream_IsPrintableLevel( self->debug, 3 ) ) {
      Dimension_Index dim_I;
      Dof_Index       dof_I;
      Journal_DPrintf( self->debug, "%s Unary Op: El %d, xi = ( ", self->name, dElement_I );

      for( dim_I = 0; dim_I < self->dim; dim_I++ ) 
         Journal_DPrintf( self->debug, "%g ", localCoord[ dim_I ] );

      /* Field 0 */
      Journal_DPrintf( self->debug, ") - %s = [ ",operand->name );
      for( dof_I = 0; dof_I < operand->fieldComponentCount; dof_I++ ) 
         Journal_DPrintf( self->debug, "%g ", fieldValue0[ dof_I ] );

      /* Result */
      Journal_DPrintf( self->debug, "] - Result = [ " );
      for( dof_I = 0; dof_I < self->fieldComponentCount; dof_I++ ) 
         Journal_DPrintf( self->debug, "%g ", value[ dof_I ] );
      Journal_DPrintf( self->debug, "]\n" );
   }
#endif
}

void OperatorFeVariable_BinaryInterpolationFunc( void* feOperator, Element_DomainIndex dElement_I, Coord localCoord, double* value ) {
   OperatorFeVariable* self = (OperatorFeVariable*)feOperator;
   FeVariable* operand0 = NULL;
   FeVariable* operand1 = NULL;
   double      fieldValue0[ MAX_FIELD_COMPONENTS ]; 
   double      fieldValue1[ MAX_FIELD_COMPONENTS ]; 

   /* Get the operands of the current OperatorFeVariable. */
   operand0 = (FeVariable*)Stg_ObjectList_At( self->operands, 0 );
   operand1 = (FeVariable*)Stg_ObjectList_At( self->operands, 1 ); 

   /* Interpolate using the first field's mesh. */ 
   FeVariable_InterpolateFromMeshLocalCoord( operand0, self->feMesh, dElement_I, localCoord, fieldValue0 );
   FeVariable_InterpolateFromMeshLocalCoord( operand1, self->feMesh, dElement_I, localCoord, fieldValue1 );

   Operator_CarryOutBinaryOperation( self->_operator, fieldValue0, fieldValue1, value );

#ifdef DEBUG
   if( Stream_IsPrintableLevel( self->debug, 3 ) ) {
      Dimension_Index dim_I;
      Dof_Index       dof_I;
      Journal_DPrintf( self->debug, "%s Binary Op: El %d, xi = ( ", self->name, dElement_I );

      for( dim_I = 0; dim_I < self->dim; dim_I++ ) 
         Journal_DPrintf( self->debug, "%g ", localCoord[ dim_I ] );

      /* Field 0 */
      Journal_DPrintf( self->debug, ") - %s = [ ", operand0->name );
      for( dof_I = 0; dof_I < operand0->fieldComponentCount; dof_I++ ) 
         Journal_DPrintf( self->debug, "%g ", fieldValue0[ dof_I ] );

      /* Field 1 */
      Journal_DPrintf( self->debug, "] - %s = [ ", operand1->name );
      for( dof_I = 0; dof_I < operand1->fieldComponentCount; dof_I++ ) 
         Journal_DPrintf( self->debug, "%g ", fieldValue1[ dof_I ] );

      /* Result */
      Journal_DPrintf( self->debug, "] - Result = [ " );
      for( dof_I = 0; dof_I < self->fieldComponentCount; dof_I++ ) 
         Journal_DPrintf( self->debug, "%g ", value[ dof_I ] );
      Journal_DPrintf( self->debug, "]\n" );
   }
#endif
}

void OperatorFeVariable_GradientInterpolationFunc( void* feOperator, Element_DomainIndex dElement_I, Coord localCoord, double* value ) {
   OperatorFeVariable* self = (OperatorFeVariable*)feOperator;
   FeVariable* operand = (FeVariable*)Stg_ObjectList_At( self->operands, 0 );

   FeVariable_InterpolateDerivativesToElLocalCoord( operand, dElement_I, localCoord, value );
}

void OperatorFeVariable_UnaryValueAtNodeFunc( void* feOperator, Node_DomainIndex dNode_I, double* value ) {
   OperatorFeVariable* self = (OperatorFeVariable*)feOperator;
   FeVariable* operand = (FeVariable*)Stg_ObjectList_At( self->operands, 0 );
   double      fieldValue0[ MAX_FIELD_COMPONENTS ]; 

   FeVariable_GetValueAtNode( operand, dNode_I, fieldValue0 );
   Operator_CarryOutUnaryOperation( self->_operator, fieldValue0, value );
}

void OperatorFeVariable_BinaryValueAtNodeFunc( void* feOperator, Node_DomainIndex dNode_I, double* value ) {
   OperatorFeVariable*         self = (OperatorFeVariable*)feOperator;
   FeVariable*         operand0 = NULL;
   FeVariable*         operand1 = NULL;
   double              fieldValue0[ MAX_FIELD_COMPONENTS ]; 
   double              fieldValue1[ MAX_FIELD_COMPONENTS ]; 
   double*             coord;
   Element_DomainIndex operand0Element;
   Element_DomainIndex operand1Element;
   Coord               operand0LocalCoord;
   Coord               operand1LocalCoord;

   /* Get the operands of the current OperatorFeVariable. */
   operand0 = (FeVariable*)Stg_ObjectList_At( self->operands, 0 );
   operand1 = (FeVariable*)Stg_ObjectList_At( self->operands, 1 );

   /* Check the variable's feMesh. */
   if( operand0->feMesh && operand1->feMesh ) {
      FeVariable_GetValueAtNode( operand0, dNode_I, fieldValue0 );
      FeVariable_GetValueAtNode( operand1, dNode_I, fieldValue1 );
   }
   else {  
      /* 
       * Field variables are using different meshes, 
       * so interpolate the values of each based on the global coord. 
       */
      coord = Mesh_GetVertex( self->feMesh, dNode_I );

      assert( Mesh_SearchElements( operand0->feMesh, coord, &operand0Element ) );
      FeMesh_CoordGlobalToLocal( operand0->feMesh, operand0Element, coord, operand0LocalCoord );
      FeVariable_InterpolateWithinElement( operand0, operand0Element, operand0LocalCoord, fieldValue0 );
      
      assert( Mesh_SearchElements( operand1->feMesh, coord, &operand1Element ) ); 
      FeMesh_CoordGlobalToLocal( operand1->feMesh, operand1Element, coord, operand1LocalCoord );
      FeVariable_InterpolateWithinElement( operand1, operand1Element, operand1LocalCoord, fieldValue1 );
   }
   Operator_CarryOutBinaryOperation( self->_operator, fieldValue0, fieldValue1, value );
}

/* 
 * TODO: This interface needs to be changed, as this component won't
 * be storing the mesh, geometry mesh, doflayout any more.
 * These information should be extracted from the FeVariable/OperatorFeVariable's operands now.
 */
void OperatorFeVariable_GradientValueAtNodeFunc( void* feOperator, Node_DomainIndex dNode_I, double* value ) {
   OperatorFeVariable* self = (OperatorFeVariable*)feOperator;
   Mesh*       mesh = (Mesh*)self->feMesh;
   IArray*     inc = IArray_New(); 
   int         *nbrElList;    
   Index       nbrElCount;
   Coord       elLocalCoord;
   double*     coord = Mesh_GetVertex( mesh, dNode_I ); 
   FeVariable* operand = (FeVariable*)Stg_ObjectList_At( self->operands, 0 );

   /* Find the elements around the node and point to them via the nbrElList. */
   FeMesh_GetNodeElements( mesh, dNode_I, inc );
   nbrElCount = IArray_GetSize( inc );
   nbrElList = IArray_GetPtr( inc );

   /* Use last element in list, get local coords then interpolate value. */
   FeMesh_CoordGlobalToLocal( mesh, nbrElList[nbrElCount-1], coord, elLocalCoord );
   memset( value, 0, self->fieldComponentCount * sizeof(double) );

   /* Get value at node for this element. */
   FeVariable_InterpolateDerivativesToElLocalCoord( operand, nbrElList[nbrElCount-1], elLocalCoord, value );

   Stg_Class_Delete( inc );
}
