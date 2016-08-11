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
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
** 
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/** \file 
\details Use Underworld_ShapeFemIC. Sets up Initial Conditions for Finite Element shapes.
**/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include <string.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>

#define STD   1
#define PM    2
#define PM_2  3

const Type Underworld_ShapeFemIC_Type = "Underworld_ShapeFemIC";
typedef struct {
	__Codelet
} Underworld_ShapeFemIC;



void Underworld_LinearShapeIC( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ) {

  UnderworldContext*       context            = (UnderworldContext*)_context;

  Dictionary*              theDictionary      = context->dictionary;
  FeVariable*    tempField   = (FeVariable*)LiveComponentRegister_Get( context->CF->LCRegister, (Name)"TemperatureField" );
  FeMesh*	           theMesh            = NULL;
  double*                  result             = (double* ) _result;
  Stg_Shape*               shape;
  Name                     shapeName;
  double*                  coord;
  Dictionary_Entry_Value*  shapeSpecs;	
  Index                    numSpecs = 0;
  Index	                   shapeSpec_I;
  Dictionary_Entry_Value*  shapeSpec = NULL;
  Dictionary*	           shapeSpecDict;

  int setup;
  /* STD config */
  double gz, p1x, p1y, p1z, p2x, p2y, p2z, z, ux, uy, uz, nu, d;
  double gx, gy, T0, x, y, az, xp, yp, zp, xpp, ypp, zpp;
  double ox, oy;
  /* PM config */
  double ox1,oy1,T1,H1,gy1;
  double ox2,oy2,T2,H2,gy2;
  double T1u, T1l;
  double T2u, T2l;
  double Tmu, Tml;
  double o1ux, o1uy;
  double o1ly;
  double o2ux, o2uy;
  double o2ly;
  double omuy, omly;
  double alpha, beta, W;
  double tol = 1e-5, a, b, c;

  theMesh = tempField->feMesh;

  shapeSpecs = Dictionary_Get( theDictionary, (Dictionary_Entry_Key)"linearShapeIC" );
  numSpecs = Dictionary_Entry_Value_GetCount( shapeSpecs );

  for( shapeSpec_I = 0; shapeSpec_I < numSpecs; shapeSpec_I++  ) {
    shapeSpec = Dictionary_Entry_Value_GetElement( shapeSpecs, shapeSpec_I );
    shapeSpecDict = Dictionary_Entry_Value_AsDictionary( shapeSpec );
    shapeName = Dictionary_Entry_Value_AsString( Dictionary_Get( shapeSpecDict, (Dictionary_Entry_Key)"Shape" ) );

    /* Get the shape */
    shape = (Stg_Shape* ) LiveComponentRegister_Get( context->CF->LCRegister, (Name)shapeName );

    Journal_Firewall( (Bool ) shape, 
							 Journal_Register( Error_Type, (Name)Underworld_ShapeFemIC_Type  ), 
							 "Shape %s not found.\n", shapeName );

    /* Find coordinate of node */
    coord = Mesh_GetVertex( theMesh, node_lI );

    if( Stg_Shape_IsCoordInside( shape, coord ) ) {

      setup = Dictionary_GetInt_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"setup", STD );

      switch( setup  ) {
	case PM_2:
	      /* Must be a box otherwise, so' cazzi */
	      assert( shape->type == Box_Type );

		Box *box = (Box*)shape;
		 double age_at_minX = Dictionary_Entry_Value_AsDouble(Dictionary_Get( shapeSpecDict, (Dictionary_Entry_Key)"age_at_minX" ) );
		 double age_at_maxX = Dictionary_Entry_Value_AsDouble( Dictionary_Get( shapeSpecDict, (Dictionary_Entry_Key)"age_at_maxX" ) );
		 double surfaceTemperature = Dictionary_Entry_Value_AsDouble(Dictionary_Get( shapeSpecDict, (Dictionary_Entry_Key)"SurfaceTemperature" ) );
		 double lithosphereBaseTemperature = Dictionary_Entry_Value_AsDouble(Dictionary_Get( shapeSpecDict, (Dictionary_Entry_Key)"LithosphereBaseTemperature" ) );
		 double thermalDiffusivity = Dictionary_Entry_Value_AsDouble(Dictionary_Get( shapeSpecDict, (Dictionary_Entry_Key)"ThermalDiffusivity" ) );

		 double box_min[3], box_max[3]; /* the min max coords of the box shape */
		 double bottom_y, surface_y, plateThickness;

		 int i; 
		 for( i = 0 ; i<context->dim; i++ ) {
		    box_min[i] = box->centre[i] - 0.5*box->width[i];
		    box_max[i] = box->centre[i] + 0.5*box->width[i];
		 }

		 bottom_y = box_min[1];
		 surface_y = box_max[1];
		 plateThickness = box->width[1];

		 /* check if ages are less than 1Kyr old, if so set it to 1000 for formula happyiness */
		 if( age_at_minX * Scaling_ParseDimCoeff( context->scaling, "yr" ) < 1000 ) {
		    age_at_minX = 1000 /  Scaling_ParseDimCoeff( context->scaling, "yr" );
		 }
		 if( age_at_maxX * Scaling_ParseDimCoeff( context->scaling, "yr" ) < 1000 ) {
		    age_at_maxX = 1000 /  Scaling_ParseDimCoeff( context->scaling, "yr" );
		 } 

		 /* This formula is taken from
		  * H.S. Carslaw, J.C. Jaeger, Conduction of Heat in Solids, 1984 
		  */

		double aux, theSum, expTerm, sinTerm;
		int I, numberOfTerms;

		/* do linear interpolation to calculate lithosphericAge: it's a function of x coord only */
		double phi = (coord[0] - box_min[0]) / box->width[0];

		double lithosphericAge = (1 - phi)*age_at_minX + phi*age_at_maxX;

		double depth = surface_y - coord[J_AXIS]; 
		aux = -thermalDiffusivity*M_PI*M_PI*lithosphericAge/(plateThickness*plateThickness);
		
		numberOfTerms = 200;
		theSum = 0;
		for( I = 1; I < numberOfTerms; I++ ) { 
		  expTerm = exp( aux*I*I );
		  sinTerm = sin( I*M_PI*depth/plateThickness );
		  theSum += expTerm * sinTerm / I; 
		}
		*result =  surfaceTemperature + (lithosphereBaseTemperature - surfaceTemperature) * (depth/plateThickness + 2.0/M_PI * theSum);
		break;

      case STD:
		  /* rotation angle */
		  az = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"rotation", 0.0  );
		  /* gradient in each direction */
		  gx = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"gradientx", 0.0  );
		  gy = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"gradienty", 0.0  );
		  /* value of the field at origin */
		  T0 = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"valueAtOrigin", 0.0 );

		  x = coord[I_AXIS];
		  y = coord[J_AXIS];
		  az = az/180.0*M_PI;

		  switch( theMesh->topo->nDims  ) {
		  case 2: 
			 /* origin */
			 ox = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"originx", 0.0  );
			 oy = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"originy", 0.0 );
			 /* rotations and translations */
			 xp = (x-ox)*cos(az) - (y-oy)*sin(az);
			 yp = (x-ox)*sin(az) + (y-oy)*cos(az );
			 /* compute value at the point */
			 *result = T0 + xp*gx + yp*gy; 
			 break;
	
		  case 3: 
			 gz = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"gradientz", 0.0 );
			 /* two points defining the rotational axis (3D case ) First point is the origin */
			 p1x = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"p1x", 0.0  );
			 p1y = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"p1y", 0.0  );
			 p1z = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"p1z", 0.0  );

			 z   = coord[K_AXIS];

			 /* 1) translation of the rotation axis to pass through the origin */
			 a = x - p1x;
			 b = y - p1y;
			 c = z - p1z;
          xp = a;
			 yp = b;
			 zp = c;

			 if( az != 0 ) {
				p2x = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"p2x", 0.0  );
				p2y = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"p2y", 0.0  );
				p2z = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"p2z", 1.0 );
				/* unit axis of rotation */
				ux = p1x-p2x;
				uy = p1y-p2y;
				uz = p1z-p2z;
				nu = sqrt(ux*ux + uy*uy + uz*uz);
				
				ux /= nu;
				uy /= nu;
				uz /= nu;

				if( fabs(fabs(ux)-1.0) < tol ) {
				  /* Rotation axis is X */
				  xp = xp;
				  zp = c*cos(az) - b*sin(az);
				  yp = c*sin(az) + b*cos(az);
				} else { 
				  if( fabs(fabs(uz)-1.0) < tol ) {
				  /* Rotation axis is Z */
					 xp = a*cos(az) - b*sin(az);
					 yp = a*sin(az) + b*cos(az);
					 zp = zp;
				  } else {
					 if( fabs(fabs(uy)-1.0) < tol ) {
						/* Rotation axis is Y */
						xp = a*cos(az) - c*sin(az);
						zp = a*sin(az) + c*cos(az);
						yp = yp;
					 } else {
						/* Rotation not aligned with any axis */
						d = sqrt( uy*uy + uz*uz );
						/* 2) Rotate V about X axis to put V in XZ plane */
						xp = 0;
						yp = b*c/d - c*b/a;
						zp = b*b/a + c*c/a;
						/* 3) Rotate V about Y to align with Z */
						xpp = xp*d - zp*a;
						ypp = yp;
						zpp = xp*a + zp*d;
						/* 4) Rotate az */
						xp = xpp*cos(az) - ypp*sin(az);
						yp = xpp*sin(az) + ypp*cos(az);
						zp = zpp;
					 }
				  }
				}
			 }
				/*
			 xp = cos(az)*( -uy*(y-p1y) - uz*(z-p1z) ) - sin(az)*( uz/d*(y-p1y) - uy/ux*(z-p1z) );
			 yp = sin(az)*( -uy*(y-p1y) - uz*(z-p1z) ) + cos(az)*( uz/d*(y-p1y) - uy/ux*(z-p1z) );
			 zp = d*uy/ux*(y-p1y) + d*uz/ux*(z-p1z );
				*/

			 /* compute value at the point */
			 *result = T0 + xp*gx + yp*gy + zp*gz; 

			 assert( !isnan( result[0] ) );

			 /*assert( result[0] >= 0 );
				assert( result[0] <= 1 );*/
			 break;	
		  }
		  break;

      case PM:
		  /* Advanced config */
		  ox1 = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"originx1", 0.0  );
		  oy1 = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"originy1", 0.0  );
		  gy1 = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"gradienty1", 0.0  );
		  T1 = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"valueAtOrigin1", 0.0  );
		  H1 = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"height1", 0.0  );

		  ox2 = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"originx2", 0.0  );
		  oy2 = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"originy2", 0.0  );
		  gy2 = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"gradienty2", 0.0  );
		  T2 = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"valueAtOrigin2", 0.0  );
		  H2 = Dictionary_GetDouble_WithDefault( shapeSpecDict, (Dictionary_Entry_Key)"height2", 0.0 );

		  x = coord[I_AXIS];
		  y = coord[J_AXIS];

		  T1u = T1;
		  T1l = T1 + H1 * gy1;
		  T2u = T2;
		  T2l = T2 + H2 * gy2;

		  o1ux = ox1;
		  o1uy = oy1;
		  o1ly = oy1 + H1;
            
		  o2ux = ox2;
		  o2uy = oy2;
		  o2ly = oy2 + H2;

		  W = o2ux - o1ux;
		  alpha = (x - o1ux) / W;
            
		  Tmu = (1-alpha)*T1u + alpha*T2u;
		  Tml = (1-alpha)*T1l + alpha*T2l;            
		  omuy = (1-alpha)*o1uy + alpha*o2uy;
		  omly = (1-alpha)*o1ly + alpha*o2ly;
            
		  beta = (y - omuy) / (omly - omuy);

		  *result = (1-beta )*Tmu + beta*Tml;
		  assert( !isnan( result[0] ) );
		  break;
      }
    }
  }
}


void Underworld_SimpleShapeIC( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ) {
	UnderworldContext* context    = (UnderworldContext*)_context;
	Dictionary* dictionary = context->dictionary;
	MeshVariable* meshVar    = NULL;
	FeMesh* mesh       = NULL;
	double* result     = (double*) _result;
	Stg_Shape* shape;
	Name shapeName;
	double* coord;

	meshVar = (MeshVariable*)Variable_Register_GetByIndex( context->variable_Register, var_I );
	mesh = (FeMesh*)meshVar->mesh; assert( mesh != NULL );

	shapeName = Dictionary_GetString( dictionary, (Dictionary_Entry_Key)"ShapeFemIC" );
	shape = (Stg_Shape* ) LiveComponentRegister_Get( context->CF->LCRegister, (Name)shapeName );
	assert( shape  );

	/* Find coordinate of node */
	coord = Mesh_GetVertex( mesh, node_lI );

	if ( Stg_Shape_IsCoordInside( shape, coord ) ) 
		*result = 1.0;
	else 
		*result = 0.0;
}

void Underworld_GaussianIC( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result ) {
	UnderworldContext*      context            = (UnderworldContext*)_context;
	Dictionary*             dictionary         = context->dictionary;
  FeVariable*    tempField   = (FeVariable*)LiveComponentRegister_Get( context->CF->LCRegister, (Name)"TemperatureField" );
	FeMesh*			mesh               = NULL;
	double*                 result             = (double* ) _result;
	Stg_Shape*              shape;
	Name                    shapeName;
	double*                 coord;
	double                  disVec[3];
	double                  amplitude, width;
	double                  rSq;
	
	mesh       = tempField->feMesh;

	amplitude = Dictionary_GetDouble_WithDefault( dictionary, (Dictionary_Entry_Key)"GaussianIC-Amplitude", 1.0  );
	width = Dictionary_GetDouble_WithDefault( dictionary, (Dictionary_Entry_Key)"GaussianIC-Width", 1e-2  );

	shapeName = Dictionary_GetString( dictionary, (Dictionary_Entry_Key)"ShapeFemIC" );
	shape = (Stg_Shape* ) LiveComponentRegister_Get( context->CF->LCRegister, (Name)shapeName );
	assert( shape  );
	Journal_Firewall( !strcmp(shape->type, "Sphere") || !strcmp(shape->type, "Cylinder"),
			Journal_Register( Error_Type, (Name)Underworld_ShapeFemIC_Type  ),
			"Error in %s: You're applying the GaussianIC to a shape of type %s, which can't be done."
			" It can only work on Sphere\' or \'Cylinder\' shapes\n", __func__,  shape->type );
	/* Find coordinate of node */
	coord = Mesh_GetVertex( mesh, node_lI );

	if( !strcmp(shape->type, "Sphere") ) {
		_Sphere_DistanceFromCenterAxis( shape, coord, disVec );

		rSq = disVec[0]*disVec[0]+disVec[1]*disVec[1];
		*result = amplitude * exp( -1 * rSq / (2 * width) );
	} 
	else if(  !strcmp(shape->type, "Cylinder") )  {
		_Cylinder_DistanceFromCenterAxis( shape, coord, disVec );

		rSq = disVec[0]*disVec[0]+disVec[1]*disVec[1];
		if( shape->dim == 3 ) rSq += disVec[2]*disVec[2];

		*result = amplitude * exp( -1 * rSq / (2 * width) );
	}

}

void _Underworld_ShapeFemIC_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
	ConditionFunction*      condFunc;
	UnderworldContext*      context;

	context = (UnderworldContext*)Stg_ComponentFactory_ConstructByName( cf, (Name)"context", UnderworldContext, True, data  ); 
	
	condFunc = ConditionFunction_New( Underworld_SimpleShapeIC, (Name)"Inside1_Outside0_ShapeIC", NULL );
	ConditionFunction_Register_Add( condFunc_Register, condFunc );
	condFunc = ConditionFunction_New( Underworld_GaussianIC, (Name)"GaussianIC", NULL );
	ConditionFunction_Register_Add( condFunc_Register, condFunc );
	condFunc = ConditionFunction_New( Underworld_LinearShapeIC, (Name)"linearShapeIC", NULL );
	ConditionFunction_Register_Add( condFunc_Register, condFunc );
}

void* _Underworld_ShapeFemIC_DefaultNew( Name name ) {
	return Codelet_New(
		Underworld_ShapeFemIC_Type,
		_Underworld_ShapeFemIC_DefaultNew,
		_Underworld_ShapeFemIC_AssignFromXML,
		_Codelet_Build,
		_Codelet_Initialise,
		_Codelet_Execute,
		_Codelet_Destroy,
		name );
}

Index Underworld_ShapeFemIC_Register( PluginsManager* pluginsManager ) {
	Journal_DPrintf( Underworld_Debug, "In: %s( void* )\n", __func__ );

	return PluginsManager_Submit( pluginsManager, Underworld_ShapeFemIC_Type, (Name)"0", _Underworld_ShapeFemIC_DefaultNew  );
}



