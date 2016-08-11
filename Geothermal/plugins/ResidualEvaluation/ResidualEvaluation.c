/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University 
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 		* Redistributions of source code must retain the above copyright notice, 
** 			this list of conditions and the following disclaimer.
** 		* Redistributions in binary form must reproduce the above copyright 
**			notice, this list of conditions and the following disclaimer in the 
**			documentation and/or other materials provided with the distribution.
** 		* Neither the name of the Monash University nor the names of its contributors 
**			may be used to endorse or promote products derived from this software 
**			without specific prior written permission.
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
** Contributors:
** 
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include <mpi.h>
#include <assert.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>
#include "Geothermal/Base/Base.h"

const Type Geothermal_ResidualEvaluation_Type = (const Type)"Geothermal_ResidualEvaluation";

typedef struct {
	__Codelet;
} Geothermal_ResidualEvaluation;

void Geothermal_ResidualEvaluation_Eval( void* _context );
void Geothermal_ResidualEvaluation_EvalForBoundingBox( void* _context );

void _Geothermal_ResidualEvaluation_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
	UnderworldContext*  context;

	context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", UnderworldContext, True, data  ); 

	ContextEP_Append( context, AbstractContext_EP_Solve, Geothermal_ResidualEvaluation_EvalForBoundingBox );
}

void _Geothermal_ResidualEvaluation_Build( void* component, void* data ) {
	Geothermal_ResidualEvaluation*	self = (Geothermal_ResidualEvaluation*)component;

	assert( self );
}

void _Geothermal_ResidualEvaluation_Initialise( void* component, void* data ) {
	Geothermal_ResidualEvaluation*	self = (Geothermal_ResidualEvaluation*)component;

	assert( self );
}

void* _Geothermal_ResidualEvaluation_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Geothermal_ResidualEvaluation);
	Type                                                      type = Geothermal_ResidualEvaluation_Type;
	Stg_Class_DeleteFunction*                              _delete = _Codelet_Delete;
	Stg_Class_PrintFunction*                                _print = _Codelet_Print;
	Stg_Class_CopyFunction*                                  _copy = _Codelet_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Geothermal_ResidualEvaluation_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Geothermal_ResidualEvaluation_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Geothermal_ResidualEvaluation_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Geothermal_ResidualEvaluation_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Codelet_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Codelet_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return _Codelet_New(  CODELET_PASSARGS  );
}

Index Geothermal_ResidualEvaluation_Register( PluginsManager* pluginsManager ) {
	return PluginsManager_Submit( pluginsManager, Geothermal_ResidualEvaluation_Type, (Name)"0", _Geothermal_ResidualEvaluation_DefaultNew  );
}

/* make a great big single finite element from the bounding box and integrate this at descreet points */
double InterpolateWithinBoundingBox( FeVariable* feVar, double* min, double* max, double* xi ) {
	double	gCoord[3];
	double	phi;

 	gCoord[0] = 0.5*(max[0] + min[0]) + xi[0]*0.5*(max[0] - min[0]);
 	gCoord[1] = 0.5*(max[1] + min[1]) + xi[1]*0.5*(max[1] - min[1]);
 	gCoord[2] = 0.5*(max[2] + min[2]) + xi[2]*0.5*(max[2] - min[2]);

	FieldVariable_InterpolateValueAt( feVar, gCoord, &phi );

	return phi;
}

double IntegrateBoundingBoxL2Error( FeVariable* feVar, double* min, double* max, double* phiRef ) {
	//double	l		= -1.0 / sqrt( 3.0 );
	//double	r		= +1.0 / sqrt( 3.0 );
	//double	xi[8][3]	= { {l,l,l}, {r,l,l}, {l,r,l}, {r,r,l}, {l,l,r}, {r,l,r}, {l,r,r}, {r,r,r} };
	//double	weight		= 1.0;
	double	abscissa[5];
	double	weight[5];
	double	l2Err		= 0.0;
	double	norm		= 0.0;
	double	phi;
	int	v_i, v_j, v_k;
	double	lCoord[3];
	double	omega;
	int	ii		= 0;

        abscissa[0]  = - sqrt( 245.0 + 14.0 * sqrt( 70.0 ) )/21.0;
        abscissa[1]  = - sqrt( 245.0 - 14.0 * sqrt( 70.0 ) )/21.0;
        abscissa[2]  = 0.0;
        abscissa[3]  = - abscissa[1];
        abscissa[4]  = - abscissa[0];

        weight[0] = ( 322.0 - 13.0 * sqrt( 70.0 ) )/900.0;
        weight[1] = ( 322.0 + 13.0 * sqrt( 70.0 ) )/900.0;
        weight[2] = 128.0/225.0;
        weight[3] = weight[1];
        weight[4] = weight[0];

/*
	for( v_i = 0; v_i < 8; v_i++ ) {
		phi = InterpolateWithinBoundingBox( feVar, min, max, xi[v_i] );
		l2Err += weight * (phi - phiRef[v_i]) * (phi - phiRef[v_i]);
		norm += weight * phiRef[v_i] * phiRef[v_i];
        }
*/
	for( v_i = 0; v_i < 5; v_i++ ) {
		for( v_j = 0; v_j < 5; v_j++ ) {
			for( v_k = 0; v_k < 5; v_k++ ) {
				lCoord[0] = abscissa[v_k];		
				lCoord[1] = abscissa[v_j];		
				lCoord[2] = abscissa[v_i];		
				omega = weight[v_i] * weight[v_j] * weight[v_k];
				phi = InterpolateWithinBoundingBox( feVar, min, max, lCoord );
				l2Err += omega * (phi - phiRef[ii]) * (phi - phiRef[ii]);
				norm += omega * phiRef[ii] * phiRef[ii];
				ii++;
			}
		}
	}

	l2Err /= norm;

	return l2Err;
}

#define NUMPOINTS 5
#define EPSILON 1.0E+2

void Geothermal_ResidualEvaluation_EvalForBoundingBox( void* _context ) {
	UnderworldContext*	context		= (UnderworldContext*) _context;
	FeVariable*		tempField	= (FeVariable* )LiveComponentRegister_Get( context->CF->LCRegister, (Name)"TemperatureField"  );
	double			min[3], max[3];
	FILE*			fp;
	int			ii;
	double			referenceTemp[125];
	double			l2Err;
	int			v_i, v_j, v_k;
	double			abscissa[5];
	double			xi[3];

	min[0] = Dictionary_GetDouble_WithDefault( context->dictionary, (Dictionary_Entry_Key)"materialBoxMinX", 0.0 );
	min[1] = Dictionary_GetDouble_WithDefault( context->dictionary, (Dictionary_Entry_Key)"materialBoxMinY", 0.0 );
	min[2] = Dictionary_GetDouble_WithDefault( context->dictionary, (Dictionary_Entry_Key)"materialBoxMinZ", 0.0 );
	max[0] = Dictionary_GetDouble_WithDefault( context->dictionary, (Dictionary_Entry_Key)"materialBoxMaxX", 0.0 );
	max[1] = Dictionary_GetDouble_WithDefault( context->dictionary, (Dictionary_Entry_Key)"materialBoxMaxY", 0.0 );
	max[2] = Dictionary_GetDouble_WithDefault( context->dictionary, (Dictionary_Entry_Key)"materialBoxMaxZ", 0.0 );
	for( ii = 0; ii < 3; ii++ ) {
		min[ii] = min[ii] - EPSILON;
		max[ii] = max[ii] + EPSILON;
	}
	max[1] = 0.0;

	fp = fopen( "reference.temp", "r" );
	if( fp ) {
		for( ii = 0; ii < 125; ii++ )
			fscanf( fp, "%lf", referenceTemp+ii );

		fclose( fp );
	}
	else {
/*
		double	l	 = -1.0 / sqrt( 3.0 );
		double	r	 = +1.0 / sqrt( 3.0 );
		double	xi[8][3] = { {l,l,l}, {r,l,l}, {l,r,l}, {r,r,l}, {l,l,r}, {r,l,r}, {l,r,r}, {r,r,r} };
*/
        	abscissa[0]  = - sqrt( 245.0 + 14.0 * sqrt( 70.0 ) )/21.0;
        	abscissa[1]  = - sqrt( 245.0 - 14.0 * sqrt( 70.0 ) )/21.0;
        	abscissa[2]  = 0.0;
        	abscissa[3]  = - abscissa[1];
        	abscissa[4]  = - abscissa[0];

		fp = fopen( "reference.temp", "w" );
/*
		for( ii = 0; ii < 8; ii++ ) {
			referenceTemp[ii] = InterpolateWithinBoundingBox( tempField, min, max, xi[ii] );
			fprintf( fp, "%20.12lf\n", referenceTemp[ii] );
		}
*/
		ii = 0;
		for( v_i = 0; v_i < 5; v_i++ ) {
			for( v_j = 0; v_j < 5; v_j++ ) {
				for( v_k = 0; v_k < 5; v_k++ ) {
					xi[0] = abscissa[v_k];
					xi[1] = abscissa[v_j];
					xi[2] = abscissa[v_i];
					referenceTemp[ii] = InterpolateWithinBoundingBox( tempField, min, max, xi );
					fprintf( fp, "%20.12lf\n", referenceTemp[ii] );
					ii++;
				}
			}
		}
		fclose( fp );
		return;
	}

	l2Err = IntegrateBoundingBoxL2Error( tempField, min, max, referenceTemp );

	fp = fopen( "residual.temp", "w" );
	fprintf( fp, "%20.12lf\n", l2Err );
	fclose( fp );	
}

void Geothermal_ResidualEvaluation_Eval( void* _context ) {
	UnderworldContext*	context		= (UnderworldContext*) _context;
	FeVariable*		tempField	= (FeVariable* )LiveComponentRegister_Get( context->CF->LCRegister, (Name)"TemperatureField"  );
	double			min[3], max[3];
	double			coord[3];
	double			temp[NUMPOINTS];
	unsigned		ii;
	FILE*			reference_fp;
	FILE*			out_fp;
	double			dummy[4];
	double			prevTemp[NUMPOINTS], prevTempNorm, tempNormSq, tempNorm;
	double			err, numerator, denomenator;

	min[0] = Dictionary_GetDouble_WithDefault( context->dictionary, (Dictionary_Entry_Key)"materialBoxMinX", 0.0 );
	min[1] = Dictionary_GetDouble_WithDefault( context->dictionary, (Dictionary_Entry_Key)"materialBoxMinY", 0.0 );
	min[2] = Dictionary_GetDouble_WithDefault( context->dictionary, (Dictionary_Entry_Key)"materialBoxMinZ", 0.0 );
	max[0] = Dictionary_GetDouble_WithDefault( context->dictionary, (Dictionary_Entry_Key)"materialBoxMaxX", 0.0 );
	max[1] = Dictionary_GetDouble_WithDefault( context->dictionary, (Dictionary_Entry_Key)"materialBoxMaxY", 0.0 );
	max[2] = Dictionary_GetDouble_WithDefault( context->dictionary, (Dictionary_Entry_Key)"materialBoxMaxZ", 0.0 );
	for( ii = 0; ii < 3; ii++ ) {
		min[ii] = min[ii] - EPSILON;
		max[ii] = max[ii] + EPSILON;
	}
	max[1] = 0.0;

	reference_fp = fopen( "values.reference", "r" );
	if( reference_fp ) {
		for( ii = 0; ii < NUMPOINTS; ii++ ) {
			fscanf( reference_fp, "%lf %lf %lf %lf %lf", dummy+0, dummy+1, dummy+2, prevTemp+ii, dummy+3 );
        	}
		fscanf( reference_fp, "%lf", &prevTempNorm );
		fclose( reference_fp );

		out_fp = fopen( "values.residual", "w" );
	}
	/* create the reference file */
	else {
		for( ii = 0; ii < NUMPOINTS; ii++ )
			prevTemp[ii] = 1.0E-6;

		out_fp = fopen( "values.reference", "w" );
        }

	/* x-min face (left) */
	coord[0] = min[0];
	coord[1] = 0.5 * ( max[1] + min[1] );
	coord[2] = 0.5 * ( max[2] + min[2] );
	FieldVariable_InterpolateValueAt( tempField, coord, &temp[0] );
	err = fabs( temp[0] - prevTemp[0] ) / prevTemp[0];
	fprintf( out_fp, "%20.12lf %20.12lf %20.12lf %20.12lf %20.12lf\n", coord[0], coord[1], coord[2], temp[0], err );

	/* x-max face (right) */
	coord[0] = max[0];
	coord[1] = 0.5 * ( max[1] + min[1] );
	coord[2] = 0.5 * ( max[2] + min[2] );
	FieldVariable_InterpolateValueAt( tempField, coord, &temp[1] );
	err = fabs( temp[1] - prevTemp[1] ) / prevTemp[1];
	fprintf( out_fp, "%20.12lf %20.12lf %20.12lf %20.12lf %20.12lf\n", coord[0], coord[1], coord[2], temp[1], err );

	/* y-min face (bottom) */
	coord[0] = 0.5 * ( max[0] + min[0] );
	coord[1] = min[1];
	coord[2] = 0.5 * ( max[2] + min[2] );
	FieldVariable_InterpolateValueAt( tempField, coord, &temp[2] );
	err = fabs( temp[2] - prevTemp[2] ) / prevTemp[2];
	fprintf( out_fp, "%20.12lf %20.12lf %20.12lf %20.12lf %20.12lf\n", coord[0], coord[1], coord[2], temp[2], err );

	/* z-min face (back) */
	coord[0] = 0.5 * ( max[0] + min[0] );
	coord[1] = 0.5 * ( max[1] + min[1] );
	coord[2] = min[2];
	FieldVariable_InterpolateValueAt( tempField, coord, &temp[3] );
	err = fabs( temp[3] - prevTemp[3] ) / prevTemp[3];
	fprintf( out_fp, "%20.12lf %20.12lf %20.12lf %20.12lf %20.12lf\n", coord[0], coord[1], coord[2], temp[3], err );
	
	/* z-max face (front) */
	coord[0] = 0.5 * ( max[0] + min[0] );
	coord[1] = 0.5 * ( max[1] + min[1] );
	coord[2] = max[2];
	FieldVariable_InterpolateValueAt( tempField, coord, &temp[4] );
	err = fabs( temp[4] - prevTemp[4] ) / prevTemp[4];
	fprintf( out_fp, "%20.12lf %20.12lf %20.12lf %20.12lf %20.12lf\n", coord[0], coord[1], coord[2], temp[4], err );

	numerator = denomenator = 0.0;
	for( ii = 0; ii < NUMPOINTS; ii++ ) {
		numerator += ( temp[ii] - prevTemp[ii] ) * ( temp[ii] - prevTemp[ii] );
		denomenator += prevTemp[ii] * prevTemp[ii];
	}
	tempNormSq = numerator / denomenator;
	tempNorm = sqrt( tempNormSq );
	fprintf( out_fp, "%20.12lf", tempNorm );

	fclose( out_fp );
}
