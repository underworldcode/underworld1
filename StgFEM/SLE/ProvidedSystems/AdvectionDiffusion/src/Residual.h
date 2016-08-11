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


#ifndef __StgFEM_SLE_ProvidedSystems_AdvectionDiffusion_Residual_h__
#define __StgFEM_SLE_ProvidedSystems_AdvectionDiffusion_Residual_h__

	typedef double (AdvDiffResidualForceTerm_UpwindParamFunction)( void* residual, double pecletNumber );
	
	typedef double (AdvDiffResidualForceTerm_GetDiffusivityFromIntPoint)( void* self, void* lParticle_I );

	/** Textual name of this class */
	extern const Type AdvDiffResidualForceTerm_Type;

	/** AdvDiffResidualForceTerm class contents */
	#define __AdvDiffResidualForceTerm \
		/* General info */ \
		__ForceTerm \
		\
		/* Virtual info */ \
		AdvDiffResidualForceTerm_UpwindParamFunction*			_upwindParam; \
		AdvDiffResidualForceTerm_GetDiffusivityFromIntPoint*	_getDiffusivityFromIntPoint; \
		double*																phiGrad; \
		double**																GNx; \
		double*																Ni; \
		double*																SUPGNi; \
		IArray																*incarray; \
		\
		/* AdvDiffResidualForceTerm info */ \
		FeVariable*															velocityField; \
		double																defaultDiffusivity; \
		Variable*															diffusivityVariable; \
		AdvDiffResidualForceTerm_UpwindParamFuncType				upwindParamType; \
      int                                                   last_maxNodeCount; /* behaves like a static variable to record max node count per element */ \

	struct AdvDiffResidualForceTerm { __AdvDiffResidualForceTerm };	

	AdvDiffResidualForceTerm* AdvDiffResidualForceTerm_New( 
		Name															name,
		FiniteElementContext*									context,
		ForceVector*												forceVector,
		Swarm*														integrationSwarm,
		Stg_Component*												sle, 
		FeVariable*													velocityField,
		Variable*													diffusivityVariable,
		double														defaultDiffusivity,
		AdvDiffResidualForceTerm_UpwindParamFuncType		upwindFuncType );

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define ADVDIFFRESIDUALFORCETERM_DEFARGS \
                FORCETERM_DEFARGS, \
                AdvDiffResidualForceTerm_UpwindParamFunction*  _upwindParam

	#define ADVDIFFRESIDUALFORCETERM_PASSARGS \
                FORCETERM_PASSARGS, \
	        _upwindParam

	AdvDiffResidualForceTerm* _AdvDiffResidualForceTerm_New(  ADVDIFFRESIDUALFORCETERM_DEFARGS  );

	void _AdvDiffResidualForceTerm_Init(
   	void*                                        residual,
   	FeVariable*                                  velocityField,
   	Variable*                                    diffusivityVariable,
   	double                                       defaultDiffusivity,
   	AdvDiffResidualForceTerm_UpwindParamFuncType upwindFuncType );
	
	void _AdvDiffResidualForceTerm_Delete( void* residual );

	void _AdvDiffResidualForceTerm_Print( void* residual, Stream* stream );

	void* _AdvDiffResidualForceTerm_DefaultNew( Name name );

	void _AdvDiffResidualForceTerm_AssignFromXML( void* residual, Stg_ComponentFactory* cf, void* data );

	void _AdvDiffResidualForceTerm_Build( void* residual, void* data );

	void _AdvDiffResidualForceTerm_Initialise( void* residual, void* data );

	void _AdvDiffResidualForceTerm_Execute( void* residual, void* data );

	void _AdvDiffResidualForceTerm_Destroy( void* residual, void* data );

	void _AdvDiffResidualForceTerm_AssembleElement( void* forceTerm, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elementResidual );

	/** Virtual Function Implementations */
	#define AdvDiffResidualForceTerm_UpwindParam( residual, pecletNumber ) \
		( ((AdvDiffResidualForceTerm*) residual)->_upwindParam( residual, pecletNumber ) )

	double _AdvDiffResidualForceTerm_UpwindParam( void* residual, double pecletNumber ) ;

#endif

