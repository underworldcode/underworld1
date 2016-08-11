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
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include "StgFEM/Discretisation/Discretisation.h"
#include "units.h"
#include "types.h"
#include "shortcuts.h"
#include "FiniteElementContext.h"
#include "SolutionVector.h"
#include "ForceVector.h"
#include "ForceTerm.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "EntryPoint.h"
#include "Assembler.h"


Bool ForceVector_BCAsm_RowR( void* forceVec, Assembler* assm );


/* Textual name of this class */
const Type ForceVector_Type = "ForceVector";

/** Name of this class' entry points */
static const char	ForceVector_assembleForceVectorStr[] = "assembleForceVector";

ForceVector* ForceVector_New(
	Name							name,
	FiniteElementContext*	context,
	FeVariable*					feVariable,
	Dimension_Index			dim,
	void*							entryPoint_Register,
	MPI_Comm						comm )		
{
	ForceVector* self = _ForceVector_DefaultNew( name );

	self->isConstructed = True;
	_SolutionVector_Init( (SolutionVector*)self, context, comm, feVariable ); 
	_ForceVector_Init( self, dim, entryPoint_Register );

	return self;
}

void* _ForceVector_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(ForceVector);
	Type                                                      type = ForceVector_Type;
	Stg_Class_DeleteFunction*                              _delete = _ForceVector_Delete;
	Stg_Class_PrintFunction*                                _print = _ForceVector_Print;
	Stg_Class_CopyFunction*                                  _copy = _ForceVector_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _ForceVector_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _ForceVector_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _ForceVector_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _ForceVector_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _ForceVector_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _ForceVector_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;

	return _ForceVector_New(  FORCEVECTOR_PASSARGS  );
}

ForceVector* _ForceVector_New(  FORCEVECTOR_DEFARGS  ) {
	ForceVector* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(ForceVector) );
	self = (ForceVector*)_SolutionVector_New(  SOLUTIONVECTOR_PASSARGS  );
	
	return self;
}


void _ForceVector_Init( void* forceVector, Dimension_Index dim, EntryPoint_Register* entryPoint_Register ) {
	ForceVector* self = (ForceVector*)  forceVector;
	
	/* ForceVector info */
	self->dim = dim;
	self->entryPoint_Register = entryPoint_Register;
	
	/* Create Stream */
	self->debug = Stream_RegisterChild( StgFEM_SLE_SystemSetup_Debug, self->type );
	
	/* Create Entry Point for assembleForceVector */
	Stg_asprintf( &self->_assembleForceVectorEPName, "%s-%s", self->name, ForceVector_assembleForceVectorStr );
	self->assembleForceVector = FeEntryPoint_New( self->_assembleForceVectorEPName, FeEntryPoint_AssembleForceVector_CastType );
	EntryPoint_Register_Add( self->entryPoint_Register, self->assembleForceVector );	

	/* Add default hook to assembleForceVector entry point */
	EP_ReplaceAll( self->assembleForceVector, ForceVector_GlobalAssembly_General );

	self->forceTermList = Stg_ObjectList_New();

	self->bcAsm = Assembler_New();
	self->inc = IArray_New();

	self->nModifyCBs = 0;
 	self->modifyCBs = NULL;
}

void _ForceVector_Delete( void* forceVector ) {
	ForceVector* self = (ForceVector*)forceVector;
	
	Journal_DPrintf( self->debug, "In %s - for %s\n", __func__, self->name );

	/* Stg_Class_Delete parent*/
	_SolutionVector_Delete( self );
}

void _ForceVector_Print( void* forceVector, Stream* stream ) {
	ForceVector* self = (ForceVector*)forceVector;
	
	/* General info */
	Journal_Printf( stream, "ForceVector (ptr): %p\n", self );
	
	/* Print parent */
	_SolutionVector_Print( self, stream );
	
	/* Virtual info */
	
	/* ForceVector info */
	Journal_Printf( stream, "\tassembleForceVector e.p. (ptr): %p\n", self->assembleForceVector );
	EntryPoint_PrintConcise( self->assembleForceVector, stream );

	Journal_Printf( stream, "\tVector (ptr): %p\n", self->vector );
	Journal_Printf( stream, "\tComm: %u\n", self->comm );
	Journal_Printf( stream, "\tLocalSize: %u\n", self->localSize );
}


void* _ForceVector_Copy( void* forceVector, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
	ForceVector*	self = (ForceVector*)forceVector;
	ForceVector*	newForceVector;
	PtrMap*		map = ptrMap;
	Bool		ownMap = False;
	
	if( !map ) {
		map = PtrMap_New( 10 );
		ownMap = True;
	}
	
	newForceVector = _SolutionVector_Copy( self, dest, deep, nameExt, map );
	
	/* TODO: copy vector? */
	newForceVector->entryPoint_Register = self->entryPoint_Register;
	newForceVector->localSize = self->localSize;
	
	if( deep ) {
		newForceVector->assembleForceVector = (FeEntryPoint*)Stg_Class_Copy( self->assembleForceVector, NULL, deep, nameExt, map );
		if( self->_assembleForceVectorEPName ) {
			if( nameExt ) {
				unsigned	nameLen = strlen( self->_assembleForceVectorEPName );
				
				newForceVector->_assembleForceVectorEPName = Memory_Alloc_Bytes_Unnamed( nameLen + strlen( nameExt ) + 1, "FV->vecEPName" );
				memcpy( newForceVector->_assembleForceVectorEPName, self->_assembleForceVectorEPName, nameLen );
				strcpy( newForceVector->_assembleForceVectorEPName + nameLen, nameExt );
			}
			else {
				newForceVector->_assembleForceVectorEPName = StG_Strdup( self->_assembleForceVectorEPName );
			}
		}
		else {
			newForceVector->_assembleForceVectorEPName = NULL;
		}

	}
	else {
		newForceVector->debug = self->debug;
		newForceVector->_assembleForceVectorEPName = self->_assembleForceVectorEPName;
		newForceVector->assembleForceVector = self->assembleForceVector;
	}
	
	if( ownMap ) {
		Stg_Class_Delete( map );
	}
	
	return (void*)newForceVector;
}


void _ForceVector_AssignFromXML( void* forceVector, Stg_ComponentFactory* cf, void* data ) {
	ForceVector*    self = (ForceVector*)forceVector;
	Dimension_Index dim = 0;
	void*           entryPointRegister = NULL;

	_SolutionVector_AssignFromXML( self, cf, data );
	
	dim = Stg_ComponentFactory_GetRootDictUnsignedInt( cf, (Dictionary_Entry_Key)"dim", 0 );

	entryPointRegister = (void*)self->context->entryPoint_Register;
	assert( entryPointRegister  );
	
	_ForceVector_Init( self, dim, entryPointRegister );
}

void _ForceVector_Build( void* forceVector, void* data ) {
   ForceVector* self = (ForceVector*)forceVector;

   _SolutionVector_Build( self, data );

   /* update the size depending on our now built feVariable */
   self->localSize = self->feVariable->eqNum->localEqNumsOwnedCount;

   Stream_IndentBranch( StgFEM_Debug );
   Journal_DPrintfL( self->debug, 2, "Allocating the L.A. Force Vector with %d local entries.\n", self->localSize );
   Stream_UnIndentBranch( StgFEM_Debug );

   Assembler_SetVariables( self->bcAsm, self->feVariable, NULL );
   Assembler_SetCallbacks( self->bcAsm, 
      NULL, 
      ForceVector_BCAsm_RowR, NULL, 
      NULL, NULL, 
      self );
}


void _ForceVector_Initialise( void* forceVector, void* data ) {
	ForceVector* self = (ForceVector*)forceVector;
	
	_SolutionVector_Initialise( self, data );
}

void _ForceVector_Execute( void* forceVector, void* data ) {
}

void _ForceVector_Destroy( void* forceVector, void* data ) {
	ForceVector* self = (ForceVector*)forceVector;

	Memory_Free( self->_assembleForceVectorEPName );

	/* Don't delete entry point: E.P register will delete it automatically */
	Stg_Class_Delete( self->forceTermList );

	Stg_Class_Delete( self->inc );

	_SolutionVector_Destroy( self, data );
}

void ForceVector_Assemble( void* forceVector ) {
	ForceVector* self = (ForceVector*)forceVector;
        int ii;
	
	Journal_DPrintf( self->debug, "In %s - for vector \"%s\" - calling the \"%s\" E.P.\n", __func__, self->name,
			 self->assembleForceVector->name );

	/* Call the Entry point directly from the base class */
	/* Note that it may be empty: this is deliberate. */
	if ( 0 == self->assembleForceVector->hooks->count ) {
		Journal_DPrintf( self->debug, "(E.P. has no hooks loaded -> no assembly required.)\n" ); 
	}	
	
	((FeEntryPoint_AssembleForceVector_CallFunction*)EntryPoint_GetRun( self->assembleForceVector ))(
		self->assembleForceVector,
		self );

        /* Run all the modify callbacks. */
        for( ii = 0; ii < self->nModifyCBs; ii++ ) {
           void* callback = self->modifyCBs[ii].callback;
           void* object = self->modifyCBs[ii].object;
           ((void(*)(void*))callback)( object );
        }
}


void ForceVector_PrintElementForceVector(
	ForceVector* self,
	Element_LocalIndex element_lI,
	Dof_EquationNumber** elementLM,
	double* elForceVecToAdd )
{
	DofLayout*		dofLayout = self->feVariable->dofLayout;
	FeMesh*			feMesh = self->feVariable->feMesh;
	unsigned		nInc, *inc;
	Dof_Index		dofsPerNode;
	Node_LocalIndex		nodesThisEl;
	Node_LocalIndex		node_I;
	Dof_Index		dof_I;
	Index			vec_I;
	IArray*			incArray;

	incArray = IArray_New();
	FeMesh_GetElementNodes( feMesh, element_lI, incArray );
	nInc = IArray_GetSize( incArray );
	inc = IArray_GetPtr( incArray );
	dofsPerNode = dofLayout->dofCounts[inc[0]];
	nodesThisEl = nInc;

	for ( node_I=0; node_I < nodesThisEl; node_I++ ) {
		for ( dof_I = 0; dof_I < dofsPerNode; dof_I++ ) {
			vec_I = node_I * dofsPerNode + dof_I;

			Journal_DPrintf( self->debug, "Entry[%d][%d] (LM (%4d)) = %.3f\n",
					 node_I, dof_I,
					 elementLM[node_I][dof_I],
					 elForceVecToAdd[vec_I] ); 
		}			
	}

	Stg_Class_Delete( incArray );
}

/* from the depreciated Vector class */
void _ForceVector_VectorView( Vec v, Stream* stream ) {
	unsigned	entry_i;
	PetscInt	size;
	PetscScalar*	array;

	VecGetSize( v, &size );
	VecGetArray( v, &array );

	Journal_Printf( stream, "%p = [", v );
	for( entry_i = 0; entry_i < size; entry_i++ ) 
		Journal_Printf( stream, "\t%u: \t %.12g\n", entry_i, array[entry_i] );
	Journal_Printf( stream, "];\n" );

	VecRestoreArray( v, &array );
}

void ForceVector_GlobalAssembly_General( void* forceVector ) {
	ForceVector*            self                 = (ForceVector*) forceVector;
	FeVariable*             feVar                = self->feVariable;
	Element_LocalIndex      element_lI;
	Element_LocalIndex      elementLocalCount;
	Node_ElementLocalIndex  nodeCountCurrElement = 0;
	Element_Nodes           nodeIdsInCurrElement = 0;
	Dof_Index               totalDofsThisElement = 0;
	Dof_Index               totalDofsPrevElement = 0;
	Dof_Index               dofCountLastNode     = 0;
	Dof_EquationNumber**    elementLM            = NULL;
	double*                 elForceVecToAdd      = NULL;
	/* For output printing */
	double                  outputPercentage=10;	/* Controls how often to give a status update of assembly progress */
	int                     outputInterval;

	Journal_DPrintf( self->debug, "In %s - for vector \"%s\"\n", __func__, self->name );
	
	Stream_IndentBranch( StgFEM_Debug );
	
	if ( Stg_ObjectList_Count( self->forceTermList ) > 0 ) {
		elementLocalCount = FeMesh_GetElementLocalSize( feVar->feMesh );

		/* Initialise Vector */
		outputInterval = (int)( (outputPercentage/100.0)*(double)(elementLocalCount) );
		if( outputInterval == 0 ) { outputInterval = elementLocalCount; }
	
		for( element_lI = 0; element_lI < elementLocalCount; element_lI++ ) {  
			unsigned	nInc, *inc;
		
			FeMesh_GetElementNodes( feVar->feMesh, element_lI, self->inc );
			nInc = IArray_GetSize( self->inc );
			inc = IArray_GetPtr( self->inc );
			nodeCountCurrElement = nInc;
			/* Get the local node ids */
			nodeIdsInCurrElement = inc;

			/* Set value of elementLM: will automatically just index into global LM table if built */
			elementLM = FeEquationNumber_BuildOneElementLocationMatrix( feVar->eqNum, element_lI );

			/* work out number of dofs at the node, using LM */
			/* Since: Number of entries in LM table for this element = (by defn.) Number of dofs this element */
			dofCountLastNode = feVar->dofLayout->dofCounts[nodeIdsInCurrElement[nodeCountCurrElement-1]]; 
			totalDofsThisElement = &elementLM[nodeCountCurrElement-1][dofCountLastNode-1] - &elementLM[0][0] + 1;

			if ( totalDofsThisElement > totalDofsPrevElement ) {
				if (elForceVecToAdd) Memory_Free( elForceVecToAdd );
				Journal_DPrintfL( self->debug, 2, "Reallocating elForceVecToAdd to size %d\n", totalDofsThisElement );
				elForceVecToAdd = Memory_Alloc_Array( double, totalDofsThisElement, "elForceVecToAdd" );
			}

			/* Initialise Values to Zero */
			memset( elForceVecToAdd, 0, totalDofsThisElement * sizeof(double) );
		
			/* Assemble this element's element force vector: going through each force term in list */
			ForceVector_AssembleElement( self, element_lI, elForceVecToAdd );


	        /* When keeping BCs in we come across a bit of a problem in parallel. We're not
	           allowed to add entries to the force vector here and then clobber it later with
	           an insert in order to set the BC. So, what we'll do is just add zero here, that
	           way later we can add the BC and it will be the same as inserting it.
	           --- Luke, 20 May 2008 */
	        if( !self->feVariable->eqNum->removeBCs ) {
	           DofLayout* dofs;
	           int nDofs, curInd;
	           int ii, jj;

	           dofs = self->feVariable->dofLayout; /* shortcut to the dof layout */
	           curInd = 0; /* need a counter to track where we are in the element force vector */
	           for( ii = 0; ii < nodeCountCurrElement; ii++ ) {
	              nDofs = dofs->dofCounts[inc[ii]]; /* number of dofs on this node */
	              for( jj = 0; jj < nDofs; jj++ ) {
	                 if( !FeVariable_IsBC( self->feVariable, inc[ii], jj ) ) {
	                    curInd++;
	                    continue; /* only need to clear it if it's a bc */
	                 }
	                 elForceVecToAdd[curInd] = 0.0;
	                 curInd++;
	              }
	           }
	        }

			/* Ok, assemble into global matrix */
			//Vector_AddEntries( self->vector, totalDofsThisElement, (Index*)(elementLM[0]), elForceVecToAdd );
			VecSetValues( self->vector, totalDofsThisElement, (PetscInt*)elementLM[0], elForceVecToAdd, ADD_VALUES );

#if DEBUG
			if( element_lI % outputInterval == 0 ) {
				Journal_DPrintfL( self->debug, 2, "done %d percent of global force vector assembly (general) \n",
						  (int)(100.0*((double)element_lI/(double)elementLocalCount)) );
			}
#endif

			/* Cleanup: If we haven't built the big LM for all elements, free the temporary one */
			if ( False == feVar->eqNum->locationMatrixBuilt ) {
				Memory_Free( elementLM );
			}
			totalDofsPrevElement = totalDofsThisElement;
		}

		Memory_Free( elForceVecToAdd );
	}
	else {
		Journal_DPrintf( self->debug, "No ForceTerms registered - returning.\n" );
	}

	/* If we're keeping BCs, insert them into the force vector. */
	if( !feVar->eqNum->removeBCs )
		Assembler_LoopVector( self->bcAsm );

	//Vector_AssemblyBegin( self->vector );
	//Vector_AssemblyEnd( self->vector ); 
	VecAssemblyBegin( self->vector );
	VecAssemblyEnd( self->vector );

#if DEBUG
	if ( Stream_IsPrintableLevel( self->debug, 3 ) ) {
		Journal_DPrintf( self->debug, "Completely built vector %s is:\n", self->name );
		_ForceVector_VectorView( self->vector, self->debug );
	}
#endif

	Stream_UnIndentBranch( StgFEM_Debug );
}
		
void ForceVector_AssembleElement( void* forceVector, Element_LocalIndex element_lI, double* elForceVecToAdd ) {
	ForceVector*            self                 = (ForceVector*) forceVector;
	Index                   forceTermCount       = Stg_ObjectList_Count( self->forceTermList );
	Index                   forceTerm_I;
	ForceTerm*              forceTerm;

	for ( forceTerm_I = 0 ; forceTerm_I < forceTermCount ; forceTerm_I++ ) {
		forceTerm = (ForceTerm*) Stg_ObjectList_At( self->forceTermList, forceTerm_I );

		ForceTerm_AssembleElement( forceTerm, self, element_lI, elForceVecToAdd );
	}
}

void ForceVector_AddForceTerm( void* forceVector, ForceTerm* forceTerm ) {
	ForceVector*            self                 = (ForceVector*) forceVector;

	Stg_ObjectList_Append( self->forceTermList, Stg_CheckType( forceTerm, ForceTerm ) );
}

Bool ForceVector_BCAsm_RowR( void* forceVec, Assembler* assm ) {
	double	bc;

	bc = DofLayout_GetValueDouble( assm->rowVar->dofLayout, 
				       assm->rowNodeInd, 
				       assm->rowDofInd );
	//Vector_AddEntries( ((ForceVector*)forceVec)->vector, 1, &assm->rowEq, &bc );
	VecSetValues( ((ForceVector*)forceVec)->vector, 1, &assm->rowEq, &bc, ADD_VALUES );
	return True;
}

void ForceVector_AddModifyCallback( ForceVector* self, void* callback, void* object ) {
   self->nModifyCBs++;
   self->modifyCBs = ReallocArray( self->modifyCBs, Callback, self->nModifyCBs );
   self->modifyCBs[self->nModifyCBs - 1].callback = callback;
   self->modifyCBs[self->nModifyCBs - 1].object = object;
}


