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
**  Role:
**   The OperatorFeVariable class is used for performing some kind of useful operation on 1 or more FeVariables, 
**   to produce another FeVariable. E.G. Taking a gradient, additions, subtraction, etc.
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgFEM_Discretisation_OperatorFeVariable_h__
#define __StgFEM_Discretisation_OperatorFeVariable_h__

   #define MAX_FIELD_COMPONENTS 9

   /** Textual name of this class */
   extern const Type OperatorFeVariable_Type;

   /** Class contents */
   #define __OperatorFeVariable \
      /* Parent info */ \
      __FeVariable \
      \
      /* Virtual info */ \
      char*           operatorName; \
      Operator*       _operator; \
      Index           operandCount; \
      Stg_ObjectList* operands; \
      FeVariable*     baseFeVariable; \
      Bool            useGradient; \
      Dictionary*     dictionary; \
      Dictionary*     myDict;

   struct OperatorFeVariable { __OperatorFeVariable };   

   /** Shortcut constructors */
   OperatorFeVariable* OperatorFeVariable_NewUnary( Name name, DomainContext* context, void* _operand, Name operatorName );

   OperatorFeVariable* OperatorFeVariable_NewBinary( Name name, DomainContext* context, void* _operand0, void* _operand1, Name operatorName );
   
   OperatorFeVariable* OperatorFeVariable_NewUnary_OwnOperator( Name name, DomainContext* context, void* _operand, Operator* ownOperator );
   
   /* Public Constructor */
   void* _OperatorFeVariable_DefaultNew( Name name );

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
      FieldVariable_Register*                      fieldVariable_Register );

   /** Private Constructor */
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define FEOPERATOR_DEFARGS \
      FEVARIABLE_DEFARGS

   #define FEOPERATOR_PASSARGS \
      FEVARIABLE_PASSARGS

   OperatorFeVariable* _OperatorFeVariable_New( FEOPERATOR_DEFARGS );

   void _OperatorFeVariable_Init( void* feOperator, Name operatorName, Index operandCount, FeVariable** operands, Operator* ownOperator );

   /* 'Stg_Class' Virtual Implementations */
   void _OperatorFeVariable_Delete( void* feOperator );

   void _OperatorFeVariable_Print( void* feOperator, Stream* stream );

   void* _OperatorFeVariable_Copy( void* feOperator, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
   
   /* 'Stg_Component' Virtual Implementations */
   void _OperatorFeVariable_AssignFromXML( void* feOperator, Stg_ComponentFactory* cf, void* data );

   void _OperatorFeVariable_Build( void* feOperator, void* data );

   void _OperatorFeVariable_Execute( void* feOperator, void* data );

   void _OperatorFeVariable_Destroy( void* feOperator, void* data );

   void _OperatorFeVariable_Initialise( void* feOperator, void* data );

   /** Private Functions */
   InterpolationResult _OperatorFeVariable_InterpolateValueAt( void* feOperator, Coord globalCoord, double* value );

   double _OperatorFeVariable_GetMinGlobalFieldMagnitude( void* feOperator );

   double _OperatorFeVariable_GetMaxGlobalFieldMagnitude( void* feOperator );

   void _OperatorFeVariable_CacheMinMaxGlobalFieldMagnitude( void* feOperator );

   void _OperatorFeVariable_GetMinAndMaxLocalCoords( void* feOperator, double* min, double* max );

   void _OperatorFeVariable_GetMinAndMaxGlobalCoords( void* feOperator, double* min, double* max );

   void _OperatorFeVariable_InterpolateWithinElement( void* feOperator, Element_DomainIndex dElement_I, Coord coord, double* value );

   void _OperatorFeVariable_GetValueAtNode( void* feOperator, Node_DomainIndex dNode_I, double* value );

   void _OperatorFeVariable_SyncShadowValues( void* feOperator );

   Bool _OperatorFeVariable_CheckInterpolateShadowSpaceReady( OperatorFeVariable* self );

   void _OperatorFeVariable_SetFunctions( void* feOperator );

   /** Public Functions */
   void OperatorFeVariable_UnaryInterpolationFunc( void* feOperator, Element_DomainIndex dElement_I, Coord coord, double* value );

   void OperatorFeVariable_BinaryInterpolationFunc( void* feOperator, Element_DomainIndex dElement_I, Coord coord, double* value );

   void OperatorFeVariable_GradientInterpolationFunc( void* feOperator, Element_DomainIndex dElement_I, Coord coord, double* value );

   void OperatorFeVariable_UnaryValueAtNodeFunc( void* feOperator, Node_DomainIndex dNode_I, double* value );

   void OperatorFeVariable_BinaryValueAtNodeFunc( void* feOperator, Node_DomainIndex dNode_I, double* value );

   void OperatorFeVariable_GradientValueAtNodeFunc( void* feOperator, Node_DomainIndex dNode_I, double* value );


#endif /* __StgFEM_Discretisation_OperatorFeVariable_h__ */

