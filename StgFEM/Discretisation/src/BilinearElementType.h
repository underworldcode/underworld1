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
**	Implements the ElementType FEM requirements for a BiLinear element.
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgFEM_Discretisation_BilinearElementType_h__
#define __StgFEM_Discretisation_BilinearElementType_h__
	
	/* Textual name of this class */
	extern const Type BilinearElementType_Type;

	/* BilinearElementType information */
	#define __BilinearElementType \
		/* General info */ \
		__ElementType \
		\
		/* Virtual info */ \
		\
		/* BilinearElementType info */ \
		double	minElLocalCoord[2]; /** Bottom corner in elLocal mathematical space */ \
		double	maxElLocalCoord[2]; /** Top corner in elLocal mathematical space */ \
		double	elLocalLength[2]; /** Length of element in elLocal space */ \
		\
		unsigned**		triInds; \
		
	struct BilinearElementType { __BilinearElementType };



	/* Create a new BilinearElementType and initialise */
	void* _BilinearElementType_DefaultNew( Name name );
	
	BilinearElementType* BilinearElementType_New( Name name );
	
	/* Creation implementation / Virtual constructor */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define BILINEARELEMENTTYPE_DEFARGS \
                ELEMENTTYPE_DEFARGS

	#define BILINEARELEMENTTYPE_PASSARGS \
                ELEMENTTYPE_PASSARGS

	BilinearElementType* _BilinearElementType_New(  BILINEARELEMENTTYPE_DEFARGS  );
	
	/* Initialise implementation */
	void _BilinearElementType_Init( BilinearElementType* self );
	
	/* Stg_Class_Delete a BilinearElementType construst */
	void _BilinearElementType_Delete( void* elementType );
	
	/* Print the contents of an BilinearElementType construct */
	void _BilinearElementType_Print( void* elementType, Stream* stream );
	
	/* Bilinear element type build implementation */
	void _BilinearElementType_AssignFromXML( void* elementType, Stg_ComponentFactory *cf, void* data );
	
	void _BilinearElementType_Build( void* elementType, void* data );
	
	void _BilinearElementType_Initialise( void* elementType, void *data );
	
	void _BilinearElementType_Execute( void* elementType, void *data );
	
	void _BilinearElementType_Destroy( void* elementType, void *data );
	
	/** ElementType_EvaluateShapeFunctionsAt implementation. */
	void _BilinearElementType_SF_allNodes( void* elementType, const double localCoord[], double* const evaluatedValues );
	
	/** ElementType_EvaluateShapeFunctionDerivsAt implementation. */
	void _BilinearElementType_SF_allLocalDerivs_allNodes( void* elementType, const double localCoord[],
		double** const evaluatedDerivatives );
	
	/** ElementType_ConvertGlobalToElLocalCoord implementation. If the element type is "box" (ParallelPipedQuadEL),
		then calculates this using a faster shortcut. */
#if 0
	void _BilinearElementType_ConvertGlobalCoordToElLocal(
		void*		elementType,
		void*		mesh, 
		unsigned	element, 
		const double*	globalCoord,
		double*		elLocalCoord );
#endif 

	double _BilinearElementType_JacobianDeterminantSurface(
		void*		elementType,
		void*		mesh,
		unsigned	element_I,
		const double	localCoord[],
		unsigned	face_I,
		unsigned	norm );

#endif /* __StgFEM_Discretisation_BilinearElementType_h__ */

