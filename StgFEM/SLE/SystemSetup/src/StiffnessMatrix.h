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
** Contributors:
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
*/
/** \file
**  Role:
**	Finite Element Stiffness Matrix object.
**
** Assumptions:
**	Assumes the dofs are indexed by local node index, rather than global.
**
** Comments:
**	Note that given both the high level Assembly and Low-level ElStiffMat 
**	assembly functions actually call entry points, these objects are
**	very customisable. They both default to empty, in case we have a matrix
**	that doesn't need element-based information to be assembled.
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgFEM_SLE_SystemSetup_StiffnessMatrix_h__
#define __StgFEM_SLE_SystemSetup_StiffnessMatrix_h__
	

	/* typedefs for virtual functions: Other than  */
	typedef void		(StiffnessMatrix_CalculateNonZeroEntriesFunction)	( void* stiffnessMatrix );

	/* #Defines and variables which allow generalising the assembly alg. for when row and col FeVariables different */
	#define			MAX_FE_VARS	2
	#define			ROW_VAR		0
	#define			COL_VAR		1
	
	/* Textual name of this class */
	extern const Type StiffnessMatrix_Type;
	
	/* StiffnessMatrix information */
	#define __StiffnessMatrix  \
		/* General info */ \
		__Stg_Component \
		\
		FiniteElementContext*				context;			\
		/* Virtual info */ \
		StiffnessMatrix_CalculateNonZeroEntriesFunction* _calculateNonZeroEntries; \
		\
		/* StiffnessMatrix info */  \
		Stream*                                           debug;                          \
		FeVariable*                                       rowVariable;                    \
		FeVariable*                                       columnVariable;                 \
		ForceVector*                                      rhs;                            \
		ForceVector*					  transRHS;			  \
		Mat	                                          matrix;                         \
/* 		Bool						  useShellMatrix;		  \ */ \
		Stg_Component*                                    applicationDepInfo;             \
		Bool                                              isNonLinear;                    \
		Bool                                              allowZeroElementContributions;  \
		EntryPoint_Register*                              entryPoint_Register;            \
		Stg_ObjectList*                                   stiffnessMatrixTermList;        \
		FeEntryPoint*                                     assembleStiffnessMatrix;        \
		Name                                              _assembleStiffnessMatrixEPName; \
		MPI_Comm                                          comm;                           \
		Index                                             rowLocalSize;                   \
		Index                                             colLocalSize;                   \
		Index                                             dim;                            \
		Index                                             nonZeroCount;                   \
		Index                                             diagonalNonZeroCount;           \
		int*                                            diagonalNonZeroIndices;         \
		Index                                             offDiagonalNonZeroCount;        \
		int*                                            offDiagonalNonZeroIndices;      \
												\
		Assembler*					zeroBCsAsm;			\
		Assembler*					bcAsm;				\
		Assembler*					transBCAsm;			\
		Assembler*					diagBCsAsm;			\
		double**					elStiffMat;			\
		double*						bcVals;				\
		unsigned					nRowDofs;			\
		unsigned					nColDofs;			\
									\
		IArray* rowInc;	\
		IArray* colInc; \
                int nModifyCBs;\
                Callback* modifyCBs;

	struct StiffnessMatrix { __StiffnessMatrix };
	
	/* Creation implementation / Virtual constructor */
	void* StiffnessMatrix_DefaultNew( Name name );
	
	StiffnessMatrix* StiffnessMatrix_New(
		Name                                             name,
		void*                                            rowVariable,
		void*                                            columnVariable,
		void*                                            rhs,
		Stg_Component*                                   applicationDepInfo,
		Dimension_Index                                  dim,
		Bool                                             isNonLinear,
		Bool                                             allowZeroElementContributions,
		void*                                            entryPoint_Register,
		MPI_Comm                                         comm );


	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define STIFFNESSMATRIX_DEFARGS \
                STG_COMPONENT_DEFARGS, \
                Bool                                                                   initFlag, \
                StiffnessMatrix_CalculateNonZeroEntriesFunction*       _calculateNonZeroEntries, \
                void*                                                               rowVariable, \
                void*                                                            columnVariable, \
                void*                                                                       rhs, \
                Stg_Component*                                               applicationDepInfo, \
                Dimension_Index                                                             dim, \
                Bool                                                                isNonLinear, \
                Bool                                              allowZeroElementContributions, \
                void*                                                       entryPoint_Register, \
                MPI_Comm                                                                   comm

	#define STIFFNESSMATRIX_PASSARGS \
                STG_COMPONENT_PASSARGS, \
	        initFlag,                      \
	        _calculateNonZeroEntries,      \
	        rowVariable,                   \
	        columnVariable,                \
	        rhs,                           \
	        applicationDepInfo,            \
	        dim,                           \
	        isNonLinear,                   \
	        allowZeroElementContributions, \
	        entryPoint_Register,           \
	        comm                         

	StiffnessMatrix* _StiffnessMatrix_New(  STIFFNESSMATRIX_DEFARGS  );		
		
	void _StiffnessMatrix_Init(
		StiffnessMatrix*                                 self,
		void*                                            rowVariable,
		void*                                            columnVariable,
		void*                                            rhs,
		Stg_Component*                                   applicationDepInfo,
		Dimension_Index                                  dim,
		Bool                                             isNonLinear,
		Bool                                             allowZeroElementContributions,
		void*                                            entryPoint_Register,
		MPI_Comm                                         comm );
	
	/* Stg_Class_Delete a ElementType construst */
	void _StiffnessMatrix_Delete( void* stiffnessMatrix );
	
	/* Print the contents of an ElementType construct */
	void _StiffnessMatrix_Print( void* stiffnessMatrix, Stream* stream );
	
	/* Copy */
	#define StiffnessMatrix_Copy( self ) \
		(StiffnessMatrix*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define StiffnessMatrix_DeepCopy( self ) \
		(StiffnessMatrix*)Stg_Class_Copy( self, NULL, True, NULL, NULL )
	
	void* _StiffnessMatrix_Copy( void* stiffnessMatrix, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	
	/* Build */
	void _StiffnessMatrix_Build( void* stiffnessMatrix, void* data );
	
	/* Construct */
	void _StiffnessMatrix_AssignFromXML( void* stiffnessMatrix, Stg_ComponentFactory* cf, void* data );
	
	/* Initialisation implementation */
	void _StiffnessMatrix_Initialise( void* stiffnessMatrix, void* data );
	
	/* Execution implementation */
	void _StiffnessMatrix_Execute( void* stiffnessMatrix, void* data );
	
	/* Destruction implementation */
	void _StiffnessMatrix_Destroy( void* stiffnessMatrix, void* data );

	/* Calculate the number of non zero entries into each row of the matrix */
	void StiffnessMatrix_CalculateNonZeroEntries( void* stiffnessMatrix );
	void _StiffnessMatrix_CalculateNonZeroEntries( void* stiffnessMatrix );
	
	/** Interface to Build stiffness matrix. Calls an entry point, allowing user to specialise exactly what
	should be assembled at run-time. */
	void StiffnessMatrix_Assemble( void* stiffnessMatrix, Bool bcRemoveQuery, void* _sle, void* _context );

	/* +++ Public Functions +++ */

	/* +++ Private Functions +++ */

	void _StiffnessMatrix_CalcAndUpdateNonZeroEntriesAtRowNode(
		StiffnessMatrix*	self,
		Node_LocalIndex		rowNode_lI,
		Dof_EquationNumber	currMatrixRow,
		Index			activeEqsAtCurrRowNode );

	void _StiffnessMatrix_CalculatedListOfUniqueRelatedColNodes(
		StiffnessMatrix*	self,
		Node_LocalIndex		rowNode_lI,
		Node_DomainIndex*	uniqueRelatedColNodes,
		Node_Index*		uniqueRelatedColNodesCountPtr );
	
	void _StiffnessMatrix_PrintElementStiffnessMatrix(
		StiffnessMatrix*	self,
		Element_LocalIndex	element_lI,
		Dof_EquationNumber**	rowElementLM,
		Dof_EquationNumber**	colElementLM,
		double**		elStiffMatToAdd );

	#define StiffnessMatrix_SetToNonLinear( stiffnessMatrix ) \
		((stiffnessMatrix)->isNonLinear = True)

	void StiffnessMatrix_AssembleElement(
		void* stiffnessMatrix,
		Element_LocalIndex element_lI,
		SystemLinearEquations* sle,
		FiniteElementContext* context,
		double** elStiffMatVecToAdd);


	void StiffnessMatrix_AddStiffnessMatrixTerm( void* stiffnessMatrix, StiffnessMatrixTerm* stiffnessMatrixTerm ) ;

	void StiffnessMatrix_RefreshMatrix( StiffnessMatrix* self );

	void StiffnessMatrix_CalcNonZeros( void* stiffnessMatrix );

void StiffnessMatrix_AddModifyCallback( StiffnessMatrix* self, void* callback, void* object );

#endif /* __StgFEM_SLE_SystemSetup_StiffnessMatrix_h__ */

