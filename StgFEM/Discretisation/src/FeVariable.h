/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**   Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**	Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**     AuScope - http://www.auscope.org
**     Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**	Computational Infrastructure for Geodynamics - http://www.geodynamics.org**
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
*/
/* \file
**  Role:
**   Brings together and manages the life cycle of all the components required by the
**   Finite Element Method about a variable to be solved for.
**
** Assumptions:
**
** Comments:
**
** $Id $
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgFEM_Discretisation_FeVariable_h__
#define __StgFEM_Discretisation_FeVariable_h__
   
   /* Textual name of this class */
   extern const Type FeVariable_Type;

   typedef void (FeVariable_InterpolateWithinElementFunction)
      ( void* fieldVariable, Element_DomainIndex dElement_I, double* xi, double* value );

   typedef void (FeVariable_GetValueAtNodeFunction)
      ( void* feVariable, Node_DomainIndex dNode_I, double* value );

   typedef void (FeVariable_SyncShadowValuesFunc)( void* feVariable );

   /* Function prototypes for import / export */
   typedef void (FeVariable_ReadNodalValuesFromFile_Function)
      ( void* feVariable, const char* prefixStr, unsigned int timeStep );

   typedef void (FeVariable_IO_File_Function) ( void* feVariable, const char* fileName, Bool saveCoords );

   /* 
    * A global list of import/export info objects - can be added to later by plugins. Needs to be initialised in
    * FeDiscretisation_Init().
    */
   /* TODO: maybe should move this to the StgFEM_Context later???  */
   extern Stg_ObjectList* FeVariable_FileFormatImportExportList;

   /* FeVariable class contents */
   #define __FeVariable \
      /* General info */ \
      __FieldVariable \
      \
      /* Virtual info */ \
      FeVariable_InterpolateWithinElementFunction* _interpolateWithinElement; \
      FeVariable_GetValueAtNodeFunction*           _getValueAtNode; \
      FeVariable_SyncShadowValuesFunc*             _syncShadowValues; \
      FeVariable_SyncShadowValuesFunc*             _calibrateBCValues; /* function for when nonAABCs is True */\
      FeVariable_IO_File_Function                  *_saveToFile; \
      FeVariable_IO_File_Function                  *_readToFile; \
      /* FeVariable info */ \
      Stream*                                      debug; \
      /* Mesh that this variable is discretised over */ \
      FeMesh*                                      feMesh; \
      /* DofLayout for this variable: relates each mesh node to the Variable's */ \
      DofLayout*                                   dofLayout; \
      /* Temporary GNx for operations */ \
      double                                       **GNx; \
      /* Boundary conditions applied to this variable - Compulsory, so the eq num table can be worked out*/ \
      VariableCondition*                           bcs; \
      DynamicVC*                                   dynamicBCs[3]; /* Temporary hack */   \
      Bool                                         removeBCs; \
      Bool                                         periodic[3]; \
      /* Boundary conditions applied to this variable - Optional, may be NULL */ \
      VariableCondition*                           ics; \
      /* Info on which dofs are linked together: optional, may be NULL */ \
      LinkedDofInfo*                               linkedDofInfo; \
      /* Equation number array: maps where each dof of this Variable goes in any matrices based off it. */ \
      FeEquationNumber*                            eqNum;  \
      /* Records whether the user has sync'd shadow values yet. */ \
      Bool                                         shadowValuesSynchronised;  \
      /* A "template" feVariable this one is based on - ie this one's mesh and BCs is based off that one */ \
      FeVariable*                                  templateFeVariable; \
      /* 
       * Records whether this FeVariable is a reference solution -
       * and should be loaded from a file, regardless of checkpointing status 
       */ \
      Bool                                         isReferenceSolution; \
      /* if self->isReferenceSolution is true, t
       * his param determines if it's loaded once at the start, or every timestep. 
       */ \
      Bool                                         loadReferenceEachTimestep; \
      Bool                                         buildEqNums; \
      \
      IArray*                                      inc; \
      /* currently this variable is only used for the FeVariable_ReadFromFileWithMesh function  */ \
      Variable*                                    variable; \
			/* boolean is true if the FeVariable contains non axis-aligned bc, i.e. spherical mesh */ \
			Bool         nonAABCs; \

   /* 
    * Brings together and manages the life cycle of all the components required by the
    * Finite Element Method about a variable to be solved for - see FeVariable.h 
    */
   struct FeVariable { __FeVariable };

   /* Checkpoint file version enum */
   typedef enum FeCheckpointFileVersion {
      FeCHECKPOINT_V1 = 1, /* Original checkpointing format   */
      /* 
       * No longer store nodes within checkpoint files, and now store attributes including 
       * checkpoint version, number of dimensions and cartesion mesh size. 
       */
      FeCHECKPOINT_V2 
   } FeCheckpointFileVersion;
   
   /* --- Contstructors / Destructors --- */
   
   /* Create a new FeVariable and initialises it. The default one - no template. */
   
   void* _FeVariable_DefaultNew( Name name );
   
   FeVariable* FeVariable_New(
      Name                    name,
      DomainContext*          context,
      void*                   feMesh,
      DofLayout*              dofLayout, 
      void*                   bcs,
      void*                   ics,
      void*                   linkedDofInfo,
      Dimension_Index         dim,
      Bool                    isCheckpointedAndReloaded,
      Bool                    referenceSoulution,
      Bool                    loadReferenceEachTimestep,
      FieldVariable_Register* fieldVariable_Register );

   /* 
    * Create a new FeVariable and initialises it. Mesh, bcs and eqNum table is based off a template one.
    * User is required to provide new ICs (we figured this would be the case in the vast majority of
    * implementations. 
    */
   FeVariable* FeVariable_New_FromTemplate(
      Name                    name,
      DomainContext*          context,
      void*                   templateFeVariable,
      DofLayout*              dofLayout, 
      void*                   ics,
      Bool                    isReferenceSolution,
      Bool                    loadReferenceEachTimestep,
      FieldVariable_Register* fV_Register );
   
   /*
    * Create a new FeVariable and initialises it. 
    * User chooses whether to pass a template or not. 
    */
   FeVariable* FeVariable_New_Full(
      Name                    name,
      DomainContext*          context,
      void*                   feMesh,
      DofLayout*              dofLayout, 
      void*                   bcs,
      void*                   ics,
      void*                   linkedDofInfo,
      void*                   templateFeVariable,
      Index                   fieldComponentCount,
      Dimension_Index         dim,
      Bool                    isCheckpointedAndReloaded,
      Bool                    referenceSoulution,
      Bool                    loadReferenceEachTimestep,
      MPI_Comm                communicator,
      FieldVariable_Register* fV_Register );
   
   /* Creation implementation / Virtual constructor */
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define FEVARIABLE_DEFARGS \
      FIELDVARIABLE_DEFARGS, \
      FeVariable_InterpolateWithinElementFunction* _interpolateWithinElement, \
      FeVariable_GetValueAtNodeFunction*           _getValueAtNode, \
      FeVariable_SyncShadowValuesFunc*             _syncShadowValues 

   #define FEVARIABLE_PASSARGS \
      FIELDVARIABLE_PASSARGS, \
      _interpolateWithinElement, \
      _getValueAtNode, \
      _syncShadowValues

   FeVariable* _FeVariable_New( FEVARIABLE_DEFARGS );
   
   /* Initialise implementation */
   void _FeVariable_Init( 
      FeVariable* self,
      void*       feMesh,
      DofLayout*  dofLayout, 
      void*       bcs,
      Bool        nonAABCs,
      void*       ics,
      void*       linkedDofInfo,
      void*       templateFeVariable,
      Bool        referenceSoulution,
      Bool        loadReferenceEachTimestep );
   
   /* Stg_Class_Delete a FeVariable construst */
   void _FeVariable_Delete( void* variable );
   
   /* --- Virtual Function Implementations --- */
   #define FeVariable_InterpolateWithinElement( feVariable, dElement_I, xi, value ) \
      ( ((FeVariable*)feVariable)->_interpolateWithinElement( feVariable, dElement_I, xi, value ) )

   #define FeVariable_GetValueAtNode( feVariable, dNode_I, value ) \
      ( ((FeVariable*)feVariable)->_getValueAtNode( feVariable, dNode_I, value ) )

   #define FeVariable_SyncShadowValues( feVariable ) \
      ( ((FeVariable*)feVariable)->_syncShadowValues( feVariable ) )

   /* Print the contents of an FeVariable construct */
   void _FeVariable_Print( void* variable, Stream* stream );
   
   /* Copy */
   #define FeVariable_Copy( self ) \
      (FeVariable*)Stg_Class_Copy( self, NULL, Fale, NULL, NULL )
   #define FeVariable_DeepCopy( self ) \
      (FeVariable*)Stg_Class_Copy( self, NULL, True, NULL, NULL )
   
   void* _FeVariable_Copy( void* feVariable, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
   
   /* Stg_Component_Build() implementation */
   void _FeVariable_Build( void* variable, void* data );
   
   /* Stg_Component_AssignFromXML() implementation */
   void _FeVariable_AssignFromXML( void* variable, Stg_ComponentFactory* cf, void* data );
   
   /* Stg_Component_Initialise() implementation */
   void _FeVariable_Initialise( void* variable, void* data );
   
   /* Stg_Component_Execute() implementation */
   void _FeVariable_Execute( void* variable, void* data );
   
   /* Stg_Component_Destroy() implementation */
   void _FeVariable_Destroy( void* variable, void* data );
   
   /* Apply BCs for this variable */
   void FeVariable_ApplyBCs( void* variable, void* data );

   Bool FeVariable_IsBC( void* variable, int node, int dof );
   
   /* Interpolate the value of the FE variable at a particular coord **/
   InterpolationResult _FeVariable_InterpolateValueAt( void* variable, double* coord, double* value );

   /* Implementations of the min and max val functions */
   double _FeVariable_GetMinGlobalFieldMagnitude( void* feVariable );

   double _FeVariable_GetMaxGlobalFieldMagnitude( void* feVariable );

   void _FeVariable_CacheMinMaxGlobalFieldMagnitude( void* feVariable );
   
   /* Implementations of the coord-getting functions */
   void _FeVariable_GetMinAndMaxLocalCoords( void* feVariable, double* min, double* max );

   void _FeVariable_GetMinAndMaxGlobalCoords( void* feVariable, double* min, double* max );

   /* Prints out the value at each DOF for this FeVariable */
   void FeVariable_PrintLocalDiscreteValues( void* variable, Stream* stream );

   void _FeVariable_GetValueAtNode( void* feVariable, Node_DomainIndex dNode_I, double* value );

   /* --- Public Functions --- */

   /* 
    * Finds the value of the field at the node and broadcasts it to the rest of the processors 
    * It calls MPI_Allreduce - so this function must be called by all processors for it to work.
    */
   void FeVariable_GetValueAtNodeGlobal( void* feVariable, Node_GlobalIndex gNode_I, double* value );

   /* 
    * Finds the coordinate of the node and broadcasts it to the rest of the processors
    * It calls MPI_Allreduce - so this function must be called by all processors for it to work.
    */
   void FeVariable_GetCoordAtNodeGlobal( void* feVariable, Node_GlobalIndex gNode_I, double* coord );

   /* Zeros the value of the field at every nodal position */
   void FeVariable_ZeroField( void* feVariable );

   /* 
    * Calculates the domain element & element local coord that a particular global coord lives in.
    * Same return status conventions as for the InterpolateValueAt function. 
    */
   InterpolationResult FeVariable_GetElementLocalCoordAtGlobalCoord(
      void*                feVariable,
      double*              globalCoord,
      double*              elLocalCoord,
      Element_DomainIndex* elementCoordInPtr );

   /* Updates a single component of the value at a certain node */
   #define FeVariable_SetComponentAtNode( feVariable, dNode_I, dof_I, componentVal ) \
      DofLayout_SetValueDouble( (feVariable)->dofLayout, dNode_I, dof_I, componentVal );
      
   /* Updates all the components at a given node that user passes in with the componentValues array */
   void FeVariable_SetValueAtNode( void* feVariable, Node_DomainIndex dNode_I, double* componentValues );
   
   /* 
    * Gets the value of the FeVariable at a given node.
    * if a scalar, just returns the value.
    * if a vector, gives the magnitude.
    * Probably should split these into separate functions.
    */   
   double FeVariable_GetScalarAtNode( void* feVariable, Node_LocalIndex lNode_I );   

   /* Prints the values at the nodes in a manner that's easier to interpret if the geometry
   used is a 2D box. */
   void FeVariable_PrintLocalDiscreteValues_2dBox( void* variable, Stream* stream );

   /* Saves the current mesh coordinates, and value of each dof in the feVariable, to file */
   void FeVariable_SaveToFile( void* feVariable, const char* filename, Bool saveCoords );

   /* Reads in everything to initialise a built FeVariable from a file */
   void FeVariable_ReadFromFile( void* feVariable, const char* filename );
   
   /* Reads in everything to initialise and built FeVariable from a file,
    * where a corresponding meshfile has also been provided.  
    * you must provide the filename of the checkpointed feVariable
    * from which interpolation is being performed, and its corresponding mesh filename.
    * a templateFeVariable is also currently required which provides auxilliary information, so
    * for example if you are loading a velocityField, you will want to pass in another velocityField
    * with all the same attributes (except resolution). 
    * It is the users responsibility to destroy this newly created FeVariable, along with its dofLayout and the mesh.
    * See usage in FeVariable_InterpolateFromFile for an example.
    */
   FeVariable* FeVariable_ReadFromFileWithMesh(
      void*       templateFeVariable,
      const char* feVarFilename,
      const char* meshFilename,
      Bool partitioned );

   /* 
    * Reads in everything to initialise and built FeVariable from a file. 
    * This function also performs required interpolation where simulation 
    * mesh resolution does not match file mesh resolution
    * you must provide the feVariable which is being initialised, the filename of the checkpointed feVariable
    * from which interpolation is being performed, and its corresponding mesh filename.
    */
   void FeVariable_InterpolateFromFile( void* feVariable, const char* feVarFilename, const char* meshFilename );

   /* Evaluates Spatial Derivatives using shape functions */
   Bool FeVariable_InterpolateDerivativesAt( void* variable, double* globalCoord, double* value );
   
   void FeVariable_InterpolateDerivativesToElLocalCoord(
      void*              _feVariable,
      Element_LocalIndex lElement_I,
      double*            elLocalCoord,
      double*            value );
   
   void FeVariable_InterpolateDerivatives_WithGNx(
      void*              _feVariable,
      Element_LocalIndex lElement_I,
      double**           GNx,
      double*            value );

   void FeVariable_InterpolateValue_WithNi( void* _feVariable, Element_LocalIndex lElement_I, double* Ni, double* value );
   
   void FeVariable_GetMinimumSeparation( void* feVariable, double* minSeparationPtr, double minSeparationEachDim[3] );

   /* 
    * Synchronises each processor's shadow dof values to be the same as the values on their "home" processors.
    * Collective.
    */
   void _FeVariable_SyncShadowValues( void* feVariable );

   /* Perhaps should be moved into feVariable interface? */
   void FeVariable_PrintDomainDiscreteValues( void* feVariable, Stream* stream );

   void FeVariable_PrintCoordsAndValues( void* _feVariable, Stream* stream );

   /*
    * Use this function when you want the value of the field but the local coordinates
    * you are using may not be appropriate for the mesh of this FeVariable.
    */
   InterpolationResult FeVariable_InterpolateFromMeshLocalCoord(
      void*               feVariable,
      FeMesh*             mesh,
      Element_DomainIndex dElement_I,
      double*             localCoord,
      double*             value );
   
   #define FeVariable_IntegrateElement( feVariable, swarm, dElement_I ) \
      FeVariable_IntegrateElement_AxisIndependent( \
         feVariable, swarm, dElement_I, ((FeVariable*)feVariable)->dim, \
         I_AXIS, J_AXIS, K_AXIS ) 

   double FeVariable_IntegrateElement_AxisIndependent( 
      void*               feVariable,
      void*               _swarm,
      Element_DomainIndex dElement_I,
      Dimension_Index     dim, 
      Axis                axis0,
      Axis                axis1,
      Axis                axis2 );

   double FeVariable_Integrate( void* feVariable, void* _swarm );

   /* Functions assumes IJK Topology Elements */
   double FeVariable_AverageTopLayer( void* feVariable, void* swarm, Axis layerAxis );

   double FeVariable_AverageBottomLayer( void* feVariable, void* swarm, Axis layerAxis );

   double FeVariable_AverageLayer( void* feVariable, void* swarm, Axis layerAxis, Index layerIndex );

   #define FeVariable_IntegrateLayer( feVariable, swarm, layerAxis, layerIndex ) \
      FeVariable_IntegrateLayer_AxisIndependent( \
         feVariable, swarm, layerAxis, layerIndex, ((FeVariable*) feVariable)->dim, I_AXIS, J_AXIS, K_AXIS ) 

   double FeVariable_IntegrateLayer_AxisIndependent( 
      void* feVariable, void* _swarm,
      Axis layerAxis, Index layerIndex, Dimension_Index dim, 
      Axis axis0, Axis axis1, Axis axis2 );

   double FeVariable_AveragePlane( void* feVariable, Axis planeAxis, double planeHeight );

   double FeVariable_IntegratePlane( void* feVariable, Axis planeAxis, double planeHeight );

   /* --- Private Functions --- */

   /* 
    * Evaluates the value at a point within a given element, 
    * based on the current values of the nodes in that element.
    */
   void _FeVariable_InterpolateNodeValuesToElLocalCoord(
      void*               self,
      Element_DomainIndex element_lI,
      double*             elLocalCoord,
      double*             value );

   /* Utility function:- saves duplication of the print local and print domain functions */
   void _FeVariable_PrintLocalOrDomainValues( void* variable, Index localOrDomainCount, Stream* stream );

#endif /* __StgFEM_Discretisation_FeVariable_h__ */

