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
#include <StgFEM/Discretisation/Discretisation.h>
#include <StgFEM/SLE/SystemSetup/SystemSetup.h>

#include "types.h"
#include "UzawaPreconditionerTerm.h"
#include "Stokes_SLE.h"

#include <assert.h>
#include <string.h>

/* Textual name of this class */
const Type UzawaPreconditionerTerm_Type = "UzawaPreconditionerTerm";

UzawaPreconditionerTerm* UzawaPreconditionerTerm_New( 
	Name							name,
	FiniteElementContext*	context,
	StiffnessMatrix*			stiffnessMatrix,
	Swarm*						integrationSwarm )
{
	UzawaPreconditionerTerm* self = (UzawaPreconditionerTerm*) _UzawaPreconditionerTerm_DefaultNew( name );

	self->isConstructed = True;
	_StiffnessMatrixTerm_Init( self, context, stiffnessMatrix, integrationSwarm, NULL );
	_UzawaPreconditionerTerm_Init( self );

	return self;
}

/* Creation implementation / Virtual constructor */
UzawaPreconditionerTerm* _UzawaPreconditionerTerm_New(  UZAWAPRECONDITIONERTERM_DEFARGS  )
{
	UzawaPreconditionerTerm* self;
	
	/* Allocate memory */
	assert( _sizeOfSelf >= sizeof(UzawaPreconditionerTerm) );
	self = (UzawaPreconditionerTerm*) _StiffnessMatrixTerm_New(  STIFFNESSMATRIXTERM_PASSARGS  );
	
	/* Virtual info */
	
	return self;
}

void _UzawaPreconditionerTerm_Init( void* matrixTerm ) {
}

void _UzawaPreconditionerTerm_Delete( void* matrixTerm ) {
	UzawaPreconditionerTerm* self = (UzawaPreconditionerTerm*)matrixTerm;

	_StiffnessMatrixTerm_Delete( self );
}

void _UzawaPreconditionerTerm_Print( void* matrixTerm, Stream* stream ) {
	UzawaPreconditionerTerm* self = (UzawaPreconditionerTerm*)matrixTerm;
	
	_StiffnessMatrixTerm_Print( self, stream );

	/* General info */
}

void* _UzawaPreconditionerTerm_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                                 _sizeOfSelf = sizeof(UzawaPreconditionerTerm);
	Type                                                         type = UzawaPreconditionerTerm_Type;
	Stg_Class_DeleteFunction*                                 _delete = _UzawaPreconditionerTerm_Delete;
	Stg_Class_PrintFunction*                                   _print = _UzawaPreconditionerTerm_Print;
	Stg_Class_CopyFunction*                                     _copy = NULL;
	Stg_Component_DefaultConstructorFunction*     _defaultConstructor = _UzawaPreconditionerTerm_DefaultNew;
	Stg_Component_ConstructFunction*                       _construct = _UzawaPreconditionerTerm_AssignFromXML;
	Stg_Component_BuildFunction*                               _build = _UzawaPreconditionerTerm_Build;
	Stg_Component_InitialiseFunction*                     _initialise = _UzawaPreconditionerTerm_Initialise;
	Stg_Component_ExecuteFunction*                           _execute = _UzawaPreconditionerTerm_Execute;
	Stg_Component_DestroyFunction*                           _destroy = _UzawaPreconditionerTerm_Destroy;
	StiffnessMatrixTerm_AssembleElementFunction*     _assembleElement = _UzawaPreconditionerTerm_AssembleElement;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*)_UzawaPreconditionerTerm_New(  UZAWAPRECONDITIONERTERM_PASSARGS  );
}

void _UzawaPreconditionerTerm_AssignFromXML( void* matrixTerm, Stg_ComponentFactory* cf, void* data ) {
	UzawaPreconditionerTerm*            self             = (UzawaPreconditionerTerm*)matrixTerm;

	/* Construct Parent */
	_StiffnessMatrixTerm_AssignFromXML( self, cf, data );

	_UzawaPreconditionerTerm_Init( self );
}

void _UzawaPreconditionerTerm_Build( void* matrixTerm, void* data ) {
	UzawaPreconditionerTerm*             self             = (UzawaPreconditionerTerm*)matrixTerm;

	_StiffnessMatrixTerm_Build( self, data );
}

void _UzawaPreconditionerTerm_Initialise( void* matrixTerm, void* data ) {
	UzawaPreconditionerTerm*             self             = (UzawaPreconditionerTerm*)matrixTerm;

	_StiffnessMatrixTerm_Initialise( self, data );
}

void _UzawaPreconditionerTerm_Execute( void* matrixTerm, void* data ) {
	_StiffnessMatrixTerm_Execute( matrixTerm, data );
}

void _UzawaPreconditionerTerm_Destroy( void* matrixTerm, void* data ) {
	_StiffnessMatrixTerm_Destroy( matrixTerm, data );
}


void _UzawaPreconditionerTerm_AssembleElement( 
		void*                                              matrixTerm,
		StiffnessMatrix*                                   stiffnessMatrix, 
		Element_LocalIndex                                 lElement_I, 
		SystemLinearEquations*                             _sle,
		FiniteElementContext*                              context,
		double**                                           elPreconditioner ) 
{
	Stokes_SLE*            sle              = Stg_DCheckType( _sle, Stokes_SLE );
	StiffnessMatrix*       gMatrix          = sle->gStiffMat;
	FeVariable*            gFeVariable_col  = gMatrix->columnVariable;
	ElementType*           gElementType_col = FeMesh_GetElementType( gFeVariable_col->feMesh, lElement_I );
	Node_ElementLocalIndex gColCount        = gElementType_col->nodeCount;
	double**               gElementMatrix;

	StiffnessMatrix*       kMatrix          = sle->kStiffMat;
	FeVariable*            kFeVariable_row  = kMatrix->rowVariable;
	ElementType*           kElementType_row = FeMesh_GetElementType( kFeVariable_row->feMesh, lElement_I );
	Node_ElementLocalIndex kRowCount        = kElementType_row->nodeCount;

	Index                  velocityDofCount;
	Index                  pressureDofCount;
	double**               kElementMatrix;

	double**               kInverseG;
	Index                  row_I;
	Index                  col_I;
	Index                  vel_I;

	pressureDofCount = gColCount;
	velocityDofCount = kRowCount * kMatrix->dim;

	/* Allocate Memory */
	gElementMatrix = Memory_Alloc_2DArray( double, velocityDofCount, pressureDofCount, (Name)"g element matrix"  );
	memset( gElementMatrix[0], 0, sizeof(double) * velocityDofCount * pressureDofCount );
	kElementMatrix = Memory_Alloc_2DArray( double, velocityDofCount, velocityDofCount, (Name)"k element matrix"  );
	memset( kElementMatrix[0], 0, sizeof(double) * velocityDofCount * velocityDofCount );
	kInverseG      = Memory_Alloc_2DArray( double, velocityDofCount, pressureDofCount, (Name)"[ diag(K) ]^-1 G"  );

	/* Assemble Element G */
	StiffnessMatrix_AssembleElement( gMatrix, lElement_I, _sle, context, gElementMatrix );

	/* Assemble Element K */
	StiffnessMatrix_AssembleElement( kMatrix, lElement_I, _sle, context, kElementMatrix );

	/* Assemble [ diag(K) ]^-1 G */
	for ( row_I = 0 ; row_I < velocityDofCount ; row_I++ ) {
		for ( col_I = 0 ; col_I < pressureDofCount ; col_I++ ) {
			kInverseG[ row_I ][ col_I ] = gElementMatrix[ row_I ][ col_I ] / kElementMatrix[ row_I ][ row_I ];
		}
	}

	/* Assemble Gt [ diag(K) ]^-1 G */
	for ( row_I = 0 ; row_I < pressureDofCount ; row_I++ ) {
		for ( col_I = 0 ; col_I < pressureDofCount ; col_I++ ) {

			for ( vel_I = 0 ; vel_I < velocityDofCount ; vel_I++ ) 
				elPreconditioner[ row_I ][ col_I ] += gElementMatrix[ vel_I ][ col_I ] * kInverseG[ vel_I ][ row_I ];
		}
	}
	Memory_Free( kInverseG );
	Memory_Free( gElementMatrix );
	Memory_Free( kElementMatrix );


	/* Correct for Compressibility */
	if ( sle->cStiffMat ) {
		double**               mElementMatrix;
		StiffnessMatrix*       cMatrix        = sle->cStiffMat;
		
		mElementMatrix = Memory_Alloc_2DArray( double, pressureDofCount, pressureDofCount, (Name)"m element matrix"  );
		memset( mElementMatrix[0], 0, sizeof(double) * pressureDofCount * pressureDofCount );

		StiffnessMatrix_AssembleElement( cMatrix, lElement_I, _sle, context, mElementMatrix );

		for ( row_I = 0 ; row_I < pressureDofCount ; row_I++ ) {
			for ( col_I = 0 ; col_I < pressureDofCount ; col_I++ ) {
				elPreconditioner[ row_I ][ col_I ] -= mElementMatrix[ row_I ][ col_I ];
			}
		}

		Memory_Free( mElementMatrix );
	}
}


