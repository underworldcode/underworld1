/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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
**
** Assumptions:
**
** Invariants:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __Geothermal_FieldVariableSurfaceAdaptor_h__
#define __Geothermal_FieldVariableSurfaceAdaptor_h__

	/** Textual name of this class */
	extern const Type FieldVariableSurfaceAdaptor_Type;

	/** Virtual function types */

	#define __FieldVariableSurfaceAdaptor			\
		/* General info */				\
		__MeshAdaptor					\
								\
		/* Virtual info */				\
								\
		/* FieldVariableSurfaceAdaptor info */		\
		int                             contactDepth;	\
		FeVariable*			heightField;	\
		Name				filename;	\
		double				elDilatePower;	\
		double				minElSize[3];	\
		Bool				adaptForMat;	\
		Bool				elBunching;	\


	struct FieldVariableSurfaceAdaptor { __FieldVariableSurfaceAdaptor };

	/*--------------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/



	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define FIELDVARIABLESURFACEADAPTOR_DEFARGS \
                MESHGENERATOR_DEFARGS

	#define FIELDVARIABLESURFACEADAPTOR_PASSARGS \
                MESHGENERATOR_PASSARGS

	FieldVariableSurfaceAdaptor* FieldVariableSurfaceAdaptor_New( Name name );
	FieldVariableSurfaceAdaptor* _FieldVariableSurfaceAdaptor_New(  FIELDVARIABLESURFACEADAPTOR_DEFARGS  );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Virtual functions
	*/

	void _FieldVariableSurfaceAdaptor_Delete( void* adaptor );
	void _FieldVariableSurfaceAdaptor_Print( void* adaptor, Stream* stream );
	void _FieldVariableSurfaceAdaptor_AssignFromXML( void* adaptor, Stg_ComponentFactory* cf, void* data );
	void _FieldVariableSurfaceAdaptor_Build( void* adaptor, void* data );
	void _FieldVariableSurfaceAdaptor_Initialise( void* adaptor, void* data );
	void _FieldVariableSurfaceAdaptor_Execute( void* adaptor, void* data );
	void _FieldVariableSurfaceAdaptor_Destroy( void* adaptor, void* data );

	void FieldVariableSurfaceAdaptor_MeshAdapt_Height( FieldVariableSurfaceAdaptor* self, FeMesh* mesh, double* min, double* max );
	void FieldVariableSurfaceAdaptor_MeshAdapt_Sides( FieldVariableSurfaceAdaptor* self, FeMesh* mesh, double* min, double* max, unsigned dim );
	void FieldVariableSurfaceAdaptor_Generate( void* adaptor, void* _mesh, void* data );
	void FieldVariableSurfaceAdaptor_SurfaceAdapt( void* adaptor, void* _mesh, void* data );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Public functions
	*/

	/*--------------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/

	double FieldVariableSurfaceAdaptor_InterpolateFromHeightMap( FieldVariableSurfaceAdaptor* self, Mesh* mesh, unsigned vertex );
	void FieldVariableSurfaceAdaptor_Build_Parallel( FieldVariableSurfaceAdaptor* self, Mesh* mesh );

#endif /* __Geothermal_FieldVariableSurfaceAdaptor_h__ */

