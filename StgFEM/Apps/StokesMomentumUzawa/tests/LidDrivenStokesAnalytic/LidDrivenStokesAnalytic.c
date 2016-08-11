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
#include <StgFEM/StgFEM.h>

#include <assert.h>
#include <math.h>

const Type StgFEM_LidDrivenStokesAnalytic_Type = "StgFEM_LidDrivenStokesAnalytic";

typedef struct {
	__Codelet;
	unsigned int n;
	double       A, B, C, D;
} StgFEM_LidDrivenStokesAnalytic;

void StgFEM_LidDrivenStokesAnalytic_CalculateConstants( void *analyticSolution ) {
	StgFEM_LidDrivenStokesAnalytic* self = (StgFEM_LidDrivenStokesAnalytic*)analyticSolution;
	double                          E;
	double                          e_nPI;
	double                          e_2nPI;
	double                          e_4nPI;
	double                          n; 
	
	n = (double)self->n;

	e_nPI = exp( n * M_PI );
	e_2nPI = e_nPI * e_nPI;
	e_4nPI = e_2nPI * e_2nPI;

	E = (4.0 * n * n * M_PI * M_PI + 2.0 ) * e_2nPI - e_4nPI - 1.0;

	self->A = ( e_2nPI - 1.0 )* e_nPI / E;
	self->B = self->A;

	self->C = ( 2.0 * n * M_PI - e_2nPI + 1.0 ) * e_nPI / E;
	self->D = - ( 2.0 * n * M_PI * e_2nPI - e_2nPI + 1.0 ) * e_nPI / E;
}

void StgFEM_LidDrivenStokesAnalytic_VelocityFunction( void* data, double* coord, double* velocity ) {
	StgFEM_LidDrivenStokesAnalytic* self = (StgFEM_LidDrivenStokesAnalytic*)data;
	double                          x,y;
	double                          n;
	double                          A, B, C, D;
	
	n = (double)self->n;
	A = self->A;
	B = self->B;
	C = self->C;
	D = self->D;

	/* get copy of coords */
	x = coord[I_AXIS];
	y = coord[J_AXIS];
	
	velocity[ I_AXIS ] = sin( n * M_PI * x ) * 
		( ( A * n * M_PI + C + C * n * M_PI * y) *exp( n * M_PI * y ) 
		- ( B * n * M_PI - D + D * n * M_PI * y ) * exp( - n * M_PI * y ) );
	velocity[ J_AXIS ] = - n * M_PI * cos( n * M_PI * x ) * 
		( ( A + C * y ) * exp( n * M_PI * y ) 
		+ ( B + D * y ) * exp( - n * M_PI * y ) );
}

void StgFEM_LidDrivenStokesAnalytic_PressureFunction( void* data, double* coord, double* pressure ) {
	StgFEM_LidDrivenStokesAnalytic* self = (StgFEM_LidDrivenStokesAnalytic*)data;
	double                          x,y;
	double                          n;
	double                          A, B, C, D;
	
	n = (double)self->n;
	A = self->A;
	B = self->B;
	C = self->C;
	D = self->D;

	/* get copy of coords */
	x = coord[I_AXIS];
	y = coord[J_AXIS];
	
	*pressure = - 2.0 * n * M_PI * cos( n * M_PI * x ) * ( C * exp( n * M_PI * y ) + D * exp( - n * M_PI * y ) );
}

void _StgFEM_LidDrivenStokesAnalytic_AssignFromXML( void* codelet, Stg_ComponentFactory* cf, void* data ) {
	StgFEM_LidDrivenStokesAnalytic* self = (StgFEM_LidDrivenStokesAnalytic*)codelet;
	unsigned int*                   waveSpeed;
 
	/* Set constants */
	*waveSpeed = Stg_ComponentFactory_GetRootDictUnsignedInt( cf, (Dictionary_Entry_Key)"sinusoidalLidWavenumber", 1 );
	self->n = *waveSpeed;
	
	StgFEM_LidDrivenStokesAnalytic_CalculateConstants( self );
}

void _StgFEM_LidDrivenStokesAnalytic_Build( void* codelet, void* data ) {
	StgFEM_LidDrivenStokesAnalytic* self = (StgFEM_LidDrivenStokesAnalytic*)codelet;

	_Codelet_Build( self, data );

   /* Add this plugin's analytic functions into the register. */
   AnalyticFunction_Add( StgFEM_LidDrivenStokesAnalytic_VelocityFunction, (Name)"StgFEM_LidDrivenStokesAnalytic_VelocityFunction" );
   AnalyticFunction_Add( StgFEM_LidDrivenStokesAnalytic_PressureFunction, (Name)"StgFEM_LidDrivenStokesAnalytic_PressureFunction" );
}

void _StgFEM_LidDrivenStokesAnalytic_Initialise( void* codelet, void* data ) {
	_FieldTest_Initialise( codelet, data );
}

void* _StgFEM_LidDrivenStokesAnalytic_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                             _sizeOfSelf = sizeof(StgFEM_LidDrivenStokesAnalytic);
	Type                                                     type = StgFEM_LidDrivenStokesAnalytic_Type;
	Stg_Class_DeleteFunction*                             _delete = _FieldTest_Delete;
	Stg_Class_PrintFunction*                               _print = _FieldTest_Print;
	Stg_Class_CopyFunction*                                 _copy = _FieldTest_Copy;
	Stg_Component_DefaultConstructorFunction* _defaultConstructor = _StgFEM_LidDrivenStokesAnalytic_DefaultNew;
	Stg_Component_ConstructFunction*                   _construct = _StgFEM_LidDrivenStokesAnalytic_AssignFromXML;
	Stg_Component_BuildFunction*                           _build = _StgFEM_LidDrivenStokesAnalytic_Build;
	Stg_Component_InitialiseFunction*                 _initialise = _FieldTest_Initialise;
	Stg_Component_ExecuteFunction*                       _execute = _FieldTest_Execute;
	Stg_Component_DestroyFunction*                       _destroy = _FieldTest_Destroy;

	/*
    * Variables that are set to ZERO are variables that will be set either by the 
    * current _New function or another parent _New function further up the hierachy.
    */

   /* default value NON_GLOBAL */
	AllocationType nameAllocationType = NON_GLOBAL;

	return (void*) _FieldTest_New( FIELDTEST_PASSARGS );
}

Index StgFEM_LidDrivenStokesAnalytic_Register( PluginsManager* pluginsManager ) {
	Journal_DPrintf( StgFEM_Debug, "In: %s( void* )\n", __func__ ); 

	return PluginsManager_Submit( pluginsManager, StgFEM_LidDrivenStokesAnalytic_Type, (Name)"0", _StgFEM_LidDrivenStokesAnalytic_DefaultNew );
}
