/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
**       * Redistributions of source code must retain the above copyright notice,
**          this list of conditions and the following disclaimer.
**       * Redistributions in binary form must reproduce the above copyright
**       notice, this list of conditions and the following disclaimer in the
**       documentation and/or other materials provided with the distribution.
**       * Neither the name of the Monash University nor the names of its contributors
**       may be used to endorse or promote products derived from this software
**       without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
** THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
** PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
** BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
** OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**
** Contact:
*%  Louis.Moresi - Louis.Moresi@monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/


#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include "types.h"
#include "Rheology_Register.h"
#include "RheologyClass.h"
#include "ConstitutiveMatrix.h"
#include "RheologyMaterial.h"

#include <assert.h>
#include <string.h>

const Type ConstitutiveMatrix_Type = "ConstitutiveMatrix";

ConstitutiveMatrix* _ConstitutiveMatrix_New(  CONSTITUTIVEMATRIX_DEFARGS  )
{
   ConstitutiveMatrix*  self;

   assert( _sizeOfSelf >= sizeof(ConstitutiveMatrix) );

   /* General info */
   self = (ConstitutiveMatrix*)_StiffnessMatrixTerm_New(  STIFFNESSMATRIXTERM_PASSARGS  );

   /* Virtual functions */
   self->_setValue            = _setValue;
   self->_getViscosity        = _getViscosity;
   self->_isotropicCorrection = _isotropicCorrection;
   self->_setSecondViscosity  = _setSecondViscosity;
   self->_assemble_D_B        = _assemble_D_B;
   self->_calculateStress     = _calculateStress;

   return self;
}

void _ConstitutiveMatrix_Init(
      ConstitutiveMatrix*                     self,
      Dimension_Index                        dim,
      Bool                                   storeConstitutiveMatrix,
      Bool                                   restartUsingPreviousStokesSol )
{
   /* General and Function pointers for this class that are not on the parent class should be set here should already be set */

   /* ConstitutiveMatrix info */
   self->storeConstitutiveMatrix = storeConstitutiveMatrix;
   self->restartUsingPreviousStokesSol = restartUsingPreviousStokesSol;

   self->matrixData = NULL;
   self->dim        = dim;
   self->isSwarmTypeIntegrationPointsSwarm = Stg_Class_IsInstance( self->integrationSwarm, IntegrationPointsSwarm_Type );
   Journal_Firewall(
      self->isSwarmTypeIntegrationPointsSwarm,
      global_error_stream,
      "Error In %s - ConstitutiveMatrix %s cannot use %s. An instance of IntegrationPointsSwarm is required.\n",
      __func__,
      self->name,
      self->integrationSwarm->name );

   Journal_Firewall( ((IntegrationPointsSwarm*)self->integrationSwarm)->mapper != NULL,
      global_error_stream,
      "Error in function %s - ConstitutiveMatrix \'%s\' uses an %s, \'%s\', that doesn't have a mapper associated with it.\n"
      "\'%s\' assumes its IntegrationPointsSwarm has a mapping to a %s via a mapper component\n\n"
      "In the case:\n- The model uses particles (FEM+PIC) for defining multiple materials then the IntegrationPointsSwarm passed to \'%s\' should\n"
      "be updated to the IntegrationPointsSwarm mapped to the principle \'MaterialPointsSwarm\' (most likely the \'picIntegrationPoints\')\n\n"
      "- The model uses no particles (FEM only) then a proxy mapper and MaterialPointsSwarm must be associate with \'%s\'. An example of this can be\n"
      "found at Underworld/InputFiles/Underworld_Components/GaussSwarmWithGaussMaterialSwarm.xml\n",
      __func__, self->name, IntegrationPointsSwarm_Type, MaterialPointsSwarm_Type, self->integrationSwarm->name, self->name, self->name, self->name );

   Journal_Firewall(
      Stg_Class_IsInstance( ((IntegrationPointsSwarm*)self->integrationSwarm)->mapper, OneToOneMapper_Type ) || Stg_Class_IsInstance( ((IntegrationPointsSwarm*)self->integrationSwarm)->mapper, OneToManyMapper_Type ),
      global_error_stream,
      "Error In %s - ConstitutiveMatrix %s cannot use %s. ConstitutiveMatrix only works with IntegrationPointsSwarms"
      " which uses one-to-one mapping\n",
      __func__,
      self->name,
      self->integrationSwarm->name );

   Journal_Firewall( Stg_CheckType( self->context, PICelleratorContext ),
      global_error_stream,
      "Error - in %s(): \n context is type %s,  not type PICelleratorContext as required.\n" , __func__, self->context->type );

   self->materials_Register  = ((PICelleratorContext*)self->context)->materials_Register;
   Journal_Firewall( self->materials_Register != NULL,
      global_error_stream,
      "Error - in %s(): \n no materials_Register found \n" , __func__ );
   
   self->isDiagonal          = False;
   self->columnSize          = 0;
   self->rowSize             = 0;

   /* If we are restarting, there will be an existing valid solution for the velocity, pressure
   etc fields - thus we record this so any yield rheologies will behave correctly */
   if ( True == self->context->loadFieldsFromCheckpoint && restartUsingPreviousStokesSol == True ) {
      self->previousSolutionExists = True;
   }
   else {
      /* Otherwise, we don't want to set this as true till we've done at least one iteration of the
      first solve */
      self->previousSolutionExists = False;
   }

   self->sle = NULL;
   self->sleNonLinearIteration_I = 0;

}

void _ConstitutiveMatrix_Delete( void* constitutiveMatrix ) {
   ConstitutiveMatrix* self = (ConstitutiveMatrix*)constitutiveMatrix;

   Journal_DPrintf( self->debug, "In %s - for matrix %s\n", __func__, self->name );

   /* Stg_Class_Delete parent*/
   _StiffnessMatrixTerm_Delete( self );

}

void _ConstitutiveMatrix_Print( void* constitutiveMatrix, Stream* stream ) {
   ConstitutiveMatrix* self = (ConstitutiveMatrix*)constitutiveMatrix;

   /* General info */
   Journal_PrintPointer( stream, constitutiveMatrix );
   Stream_Indent( stream );

   /* Print parent */
   _StiffnessMatrixTerm_Print( self, stream );

   /* Function pointers for this class that are not on the parent class should be set here */
   Journal_PrintPointer( stream, self->_setValue );
   Journal_PrintPointer( stream, self->_getViscosity );
   Journal_PrintPointer( stream, self->_isotropicCorrection );
   Journal_PrintPointer( stream, self->_setSecondViscosity );
   Journal_PrintPointer( stream, self->_assemble_D_B );
   Journal_PrintPointer( stream, self->_calculateStress );

   /* Regular Info */
   Journal_PrintPointer( stream, self->debug );
   ConstitutiveMatrix_PrintContents( self, stream );

   Journal_PrintBool( stream, self->isDiagonal );
   Journal_PrintValue( stream, self->dim );
   Journal_PrintValue( stream, self->columnSize );
   Journal_PrintValue( stream, self->rowSize );
   Journal_PrintBool( stream, self->previousSolutionExists );

   Stream_UnIndent( stream );
}


void* _ConstitutiveMatrix_Copy( void* constitutiveMatrix, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
   ConstitutiveMatrix*  self = (ConstitutiveMatrix*)constitutiveMatrix;
   ConstitutiveMatrix*  newConstitutiveMatrix;

   /* TODO */ abort();
   if (deep) {
      newConstitutiveMatrix->matrixData = Memory_Alloc_2DArray( double, self->columnSize, self->rowSize, (Name)self->name );
   }
   return (void* )newConstitutiveMatrix;
}

void _ConstitutiveMatrix_AssignFromXML( void* constitutiveMatrix, Stg_ComponentFactory* cf, void* data ) {
   ConstitutiveMatrix*	self = (ConstitutiveMatrix*)constitutiveMatrix;
   Dimension_Index		dim;
   Bool						storeConstitutiveMatrix, restartUsingPreviousStokesSol;

   _StiffnessMatrixTerm_AssignFromXML( self, cf, data );

   dim = Stg_ComponentFactory_GetRootDictUnsignedInt( cf, (Dictionary_Entry_Key)"dim", 0  );

   restartUsingPreviousStokesSol = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"restartUsingPreviousStokesSol", True  );

   storeConstitutiveMatrix = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"storeConstitutiveMatrix", False  );

   _ConstitutiveMatrix_Init( self, dim, storeConstitutiveMatrix, restartUsingPreviousStokesSol );
}

void _ConstitutiveMatrix_Build( void* constitutiveMatrix, void* data ) {
   ConstitutiveMatrix* self = (ConstitutiveMatrix*)constitutiveMatrix;
   Material_Index      material_I;
   Material_Index      materialCount = Materials_Register_GetCount( self->materials_Register );
   RheologyMaterial*   material;
   Stream*             errorStream = Journal_Register( Error_Type, (Name)self->type  );

   _StiffnessMatrixTerm_Build( self, data );

   Journal_DPrintf( self->debug, "In %s - for matrix %s\n", __func__, self->name );

   self->matrixData = Memory_Alloc_2DArray( double, self->columnSize, self->rowSize, (Name)self->name  );
/*
        self->deriv = AllocArray2D( double, (self->dim + 1), (self->dim + 1) );
*/

   for ( material_I = 0 ; material_I < materialCount ; material_I++ ) {
      material = (RheologyMaterial*) Materials_Register_GetByIndex( self->materials_Register, material_I );
      Journal_Firewall( Stg_Class_IsInstance( material, RheologyMaterial_Type ),
         errorStream,
         "Error - in %s(): while checking if each "
            "RheologyMaterial is non-linear, found a material of type %s, which is "
            "not a subclass of RheologyMaterial. Currently, if you wish to use the "
            "underworld constitutive matrix to assemble over a swarm, _every_ material "
            "must be a RheologyMaterial.\n", __func__, material->type );

      if ( RheologyMaterial_IsNonLinear( material ) )
         ConstitutiveMatrix_SetToNonLinear( self );
   }
}


void _ConstitutiveMatrix_Initialise( void* constitutiveMatrix, void* data ) {
   ConstitutiveMatrix* self          = (ConstitutiveMatrix*)constitutiveMatrix;

   Journal_DPrintf( self->debug, "In %s - for matrix %s\n", __func__, self->name );


   _StiffnessMatrixTerm_Initialise( self, data );

   ConstitutiveMatrix_ZeroMatrix( self ) ;
}


void _ConstitutiveMatrix_Execute( void* constitutiveMatrix, void* data ) {
   _StiffnessMatrixTerm_Execute( constitutiveMatrix, data );
}

void _ConstitutiveMatrix_Destroy( void* constitutiveMatrix, void* data ) {
   ConstitutiveMatrix* self          = (ConstitutiveMatrix*)constitutiveMatrix;

   Memory_Free( self->matrixData );

   _StiffnessMatrixTerm_Destroy( self, data );
}

/* +++ Private Functions +++ */

/* +++ Public Functions +++ */

void ConstitutiveMatrix_Assemble(
      void*                                              constitutiveMatrix,
      Element_LocalIndex                                 lElement_I,
      int                                                particleIndex,
      IntegrationPoint*                                  particle )
{
   ConstitutiveMatrix*     self          = (ConstitutiveMatrix*)constitutiveMatrix;
   IntegrationPointsSwarm* swarm         = (IntegrationPointsSwarm*)self->integrationSwarm;
   RheologyMaterial*       material;
   MaterialPointsSwarm*    materialSwarm;
   MaterialPoint*          materialPoint;

   /* Big fat assumption!
    * Because of Rheology framework vs PIC IP mapping change, ConstitutiveMatrix assumes that
    * we are using a one-to-one mapping.
    * This is because the Rheology stuff was made based on operating on MaterialPoints
    * rather than IntegrationPoints even though its assembling. However it does require xi
    * which we are passing from the integration point for speed (rather than re-computing it)
    *
    * _Init() firewalls this assumption.
    *
    * Proper fix for this is to reassess the Rheology/ConstitutiveMatrix design
    *
    * -- Alan 20060421
    */
   material = (RheologyMaterial*) IntegrationPointsSwarm_GetMaterialOn( swarm, particle );

   if(!material){
      double gCoord[3];
      FeMesh_CoordLocalToGlobal( swarm->mesh, lElement_I, particle->xi, &gCoord );
      Journal_Firewall(
         NULL,
         Journal_MyStream( Error_Type, self ),
         "Error In %s - ConstitutiveMatrix %s\n"
         "No material available at this location (%f,%f,%f).\n"
         "Check to ensure that your materials cover the entire domain.\n"
         "You can set --maxTimeSteps=-1 to output the initial configuration of your model.",
         __func__,
         self->name,
         gCoord[0], gCoord[1], gCoord[2] );
   }


   materialPoint = OneToOneMapper_GetMaterialPoint( swarm->mapper, particle, &materialSwarm );
   self->currentParticleIndex = particleIndex;

   RheologyMaterial_RunRheologies( material, self, materialSwarm, lElement_I, materialPoint, particle->xi );

  if( self->storeConstitutiveMatrix ) {
    /* copy the recently calculated self->matrixData, the constitutive matrix, onto the particle extension */
    double* cMatrix = (double*)ExtensionManager_Get( materialSwarm->particleExtensionMgr, materialPoint, self->storedConstHandle );
    Index row_I, rowSize = self->rowSize;
    Index columnSize = self->columnSize;

    /* flatten the matrix into a 1D array */
    for( row_I = 0 ; row_I < rowSize ; row_I++ )
      memcpy( &cMatrix[columnSize*row_I], self->matrixData[row_I], columnSize*sizeof(double) );
  }
   Journal_DPrintfL( self->debug, 3, "Viscosity = %g\n", ConstitutiveMatrix_GetIsotropicViscosity( self ) );
}

void ConstitutiveMatrix_AssembleMaterialPoint(void *constitutiveMatrix, int element,
	MaterialPointsSwarm *matSwarm, int matPointInd)
{
	ConstitutiveMatrix *self = (ConstitutiveMatrix*)constitutiveMatrix;
	RheologyMaterial *material;
	MaterialPoint *matPoint;
	double xi[3];

	matPoint = (MaterialPoint*)Swarm_ParticleAt(matSwarm, matPointInd);
	material = (RheologyMaterial*)MaterialPointsSwarm_GetMaterialOn(matSwarm, matPoint);
	FeMesh_CoordGlobalToLocal(((IntegrationPointsSwarm*)self->integrationSwarm)->mesh, element, matPoint->coord, xi);
	self->currentParticleIndex = matPointInd;
	RheologyMaterial_RunRheologies(material, self, matSwarm, element, matPoint, xi);

  if( self->storeConstitutiveMatrix ) {
    /* copy the recently calculated self->matrixData, the constitutive matrix, onto the particle extension */
    double* cMatrix = (double*)ExtensionManager_Get( matSwarm->particleExtensionMgr, matPoint, self->storedConstHandle );
    Index row_I, rowSize = self->rowSize;
    Index columnSize = self->columnSize;

    /* flatten the matrix into a 1D array */
    for( row_I = 0 ; row_I < rowSize ; row_I++ ) 
      memcpy( &cMatrix[columnSize*row_I], self->matrixData[row_I], columnSize*sizeof(double) );
  }
	Journal_DPrintfL( self->debug, 3, "Viscosity = %g\n", ConstitutiveMatrix_GetIsotropicViscosity( self ) );
}

void ConstitutiveMatrix_ZeroMatrix( void* constitutiveMatrix ) {
   ConstitutiveMatrix* self   = (ConstitutiveMatrix*)constitutiveMatrix;

   memset( self->matrixData[0], 0, (self->columnSize * self->rowSize)*sizeof(double) );
        memset( self->derivs, 0, 3 * 3 * sizeof(double) );
}

void ConstitutiveMatrix_SetIsotropicViscosity( void* constitutiveMatrix, double viscosity ) {
   ConstitutiveMatrix* self = (ConstitutiveMatrix*)constitutiveMatrix;

   ConstitutiveMatrix_ZeroMatrix( self );
   ConstitutiveMatrix_IsotropicCorrection( self, viscosity );
}

void ConstitutiveMatrix_MultiplyByValue( void* constitutiveMatrix, double factor ) {
   ConstitutiveMatrix* self       = (ConstitutiveMatrix*)constitutiveMatrix;
   Index               row_I;
   Index               col_I;
   Index               columnSize = self->columnSize;
   Index               rowSize    = self->rowSize;
   double*             columnValue;

   for ( col_I = 0 ; col_I < columnSize ; col_I++ ) {
      columnValue = self->matrixData[ col_I ];

      for ( row_I = 0 ; row_I < rowSize ; row_I++ ) {
         columnValue[ row_I ] *= factor;
      }

   }
}

void ConstitutiveMatrix_PrintContents( void* constitutiveMatrix, Stream* stream ) {
   ConstitutiveMatrix* self   = (ConstitutiveMatrix*)constitutiveMatrix;
   Index               row_I;
   Index               col_I;

   for ( col_I = 0 ; col_I < self->columnSize ; col_I++ ) {
      for ( row_I = 0 ; row_I < self->rowSize ; row_I++ ) {
         Journal_Printf( stream, "matrixData[ %u ][ %u ] = %.4g; \t", col_I, row_I, self->matrixData[ col_I ][ row_I ] );
      }
      Journal_Printf( stream, "\n" );
   }
}

Index ConstitutiveMatrix_GetParticleConstExtHandle( void* constitutiveMatrix ) {
  /* Function definition:
   * gets the handle that defines the "constitutive matrix on particles" extension */
   ConstitutiveMatrix* self   = (ConstitutiveMatrix*)constitutiveMatrix;
  return self->storedConstHandle;
}

void ConstitutiveMatrix_GetStoredMatrixOnParticle(
    void* constitutiveMatrix,
    IntegrationPoint* particle,
    double** cm ) {
  /* Function definition:
   * given a integration point the function returns the stored constitutiveMatrix,
   * if it's defined on the materialPoints, which maps to the int. particle
   */
   ConstitutiveMatrix* self   = (ConstitutiveMatrix*)constitutiveMatrix;
  ExtensionInfo_Index handle = self->storedConstHandle;

  double* ext = (double*)_OneToOneMapper_GetExtensionOn(
      ((IntegrationPointsSwarm*)self->integrationSwarm)->mapper,
      particle, handle );

   if( self->dim == 2 ) {
      cm[0][0] = ext[0]; cm[0][1] = ext[1]; cm[0][2] = ext[2];
      cm[1][0] = ext[3]; cm[1][1] = ext[4]; cm[1][2] = ext[5];
      cm[2][0] = ext[6]; cm[2][1] = ext[7]; cm[2][2] = ext[8];
   } else {
      cm[0][0] = ext[0]; cm[0][1] = ext[1]; cm[0][2] = ext[2]; cm[0][3] = ext[3] ; cm[0][4] = ext[4]; cm[0][5] = ext[5];
      cm[1][0] = ext[6]; cm[1][1] = ext[7]; cm[1][2] = ext[8]; cm[1][3] = ext[9] ; cm[1][4] = ext[10]; cm[1][5] = ext[11];
      cm[2][0] = ext[12]; cm[2][1] = ext[13]; cm[2][2] = ext[14]; cm[2][3] = ext[15] ; cm[2][4] = ext[16]; cm[2][5] = ext[17];
      cm[3][0] = ext[18]; cm[3][1] = ext[19]; cm[3][2] = ext[20]; cm[3][3] = ext[21] ; cm[3][4] = ext[22]; cm[3][5] = ext[23];
      cm[4][0] = ext[24]; cm[4][1] = ext[25]; cm[4][2] = ext[26]; cm[4][3] = ext[27] ; cm[4][4] = ext[28]; cm[4][5] = ext[29];
      cm[5][0] = ext[30]; cm[5][1] = ext[31]; cm[5][2] = ext[32]; cm[5][3] = ext[33] ; cm[5][4] = ext[34]; cm[5][5] = ext[35];
   }
}



