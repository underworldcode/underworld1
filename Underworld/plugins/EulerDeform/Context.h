/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Victorian Partnership for Advanced Computing (VPAC) Ltd, Australia
** (C) 2003-2005 All Rights Reserved
**
** Authors:
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	David May, PhD Student Monash University, VPAC. (david.may@sci.maths.monash.edu.au)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**
**
** Role:
**	Defines any header info, such as new structures, needed by this plugin
**
** Assumptions:
**
** Comments:
**
** $Id $
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __Underworld_plugins_EulerDeform_Context_h__
#define __Underworld_plugins_EulerDeform_Context_h__

	struct EulerDeform_Context {
		unsigned		nSystems;
		FeVariable*             artDField; /* artificial displacement field */
		EulerDeform_System*	systems;
		AbstractContext*        ctx;
                TimeIntegrator*         timeIntegrator;
                void (*displacementCallback)();
	};

	struct EulerDeform_System {
		Mesh*		mesh;
		double*		verts;
	        FeVariable*     dispField;
		Remesher*	remesher;
		int             interval;
		FieldVariable*	velField;
		unsigned	nFields;
		FieldVariable**	fields;
		Variable**	vars;
		Bool		wrapTop;
		Bool		wrapBottom;
		Bool		wrapLeft;
                int             surfaceIdx;

		Bool		staticTop;
		Bool		staticBottom;
		Bool		staticLeft;
		Bool		staticRight;
		Bool		staticFront;
		Bool		staticBack;
		Bool		staticSides;
		double**	sideCoords;

		Bool contactRight;
		Bool contactLeft;
	};

#endif
