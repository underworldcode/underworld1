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

#ifndef __StgDomain_Mesh_SurfaceAdaptor_h__
#define __StgDomain_Mesh_SurfaceAdaptor_h__

	/** Textual name of this class */
	extern const Type SurfaceAdaptor_Type;

	/** Virtual function types */

	/** SurfaceAdaptor class contents */
	typedef enum {
		SurfaceAdaptor_SurfaceType_Wedge, 
		SurfaceAdaptor_SurfaceType_Plateau, 
		SurfaceAdaptor_SurfaceType_Topo_Data, 
		SurfaceAdaptor_SurfaceType_Sine, 
		SurfaceAdaptor_SurfaceType_Cosine, 
		SurfaceAdaptor_SurfaceType_Invalid
	} SurfaceAdaptor_SurfaceType;

	typedef struct {
				/* these vectors represent things in the x-axis and z-axis */
              double	offs[2];
              double  endOffs[2];
              double	grad[2];
	} SurfaceAdaptor_WedgeInfo;

	typedef struct {
          double	x1, x2, x3, x4, z1, z2, z3, z4, height;
	} SurfaceAdaptor_PlateauInfo;

	typedef struct {
          int nx,nz;
          double minX, maxX, minZ, maxZ, dx, dz;
          double *heights;
	} SurfaceAdaptor_Topo_DataInfo;

	typedef struct {
		double	origin[2];
		double	amp;
		double	freq;
	} SurfaceAdaptor_TrigInfo;

	typedef union {
		SurfaceAdaptor_WedgeInfo	wedge;
		SurfaceAdaptor_PlateauInfo	plateau;
                SurfaceAdaptor_Topo_DataInfo    topo_data;
		SurfaceAdaptor_TrigInfo		trig;
	} SurfaceAdaptor_SurfaceInfo;

	#define __SurfaceAdaptor				\
		/* General info */				\
		__MeshAdaptor					\
								\
		/* Virtual info */				\
								\
		/* SurfaceAdaptor info */			\
		SurfaceAdaptor_SurfaceType	surfaceType;	\
		SurfaceAdaptor_SurfaceInfo	info;           \
		int                             contactDepth;


	struct SurfaceAdaptor { __SurfaceAdaptor };

	/*--------------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/



	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define SURFACEADAPTOR_DEFARGS \
                MESHADAPTOR_DEFARGS

	#define SURFACEADAPTOR_PASSARGS \
                MESHADAPTOR_PASSARGS

	SurfaceAdaptor* SurfaceAdaptor_New( Name name, AbstractContext* context );
	SurfaceAdaptor* _SurfaceAdaptor_New(  SURFACEADAPTOR_DEFARGS  );
	void _SurfaceAdaptor_Init( SurfaceAdaptor* self );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Virtual functions
	*/

	void _SurfaceAdaptor_Delete( void* adaptor );
	void _SurfaceAdaptor_Print( void* adaptor, Stream* stream );
	void _SurfaceAdaptor_AssignFromXML( void* adaptor, Stg_ComponentFactory* cf, void* data );
	void _SurfaceAdaptor_Build( void* adaptor, void* data );
	void _SurfaceAdaptor_Initialise( void* adaptor, void* data );
	void _SurfaceAdaptor_Execute( void* adaptor, void* data );
	void _SurfaceAdaptor_Destroy( void* adaptor, void* data );

	void SurfaceAdaptor_Generate( void* adaptor, void* _mesh, void* data );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Public functions
	*/

	/*--------------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/

	double SurfaceAdaptor_Wedge2D( SurfaceAdaptor* self, Mesh* mesh, 
				     unsigned* globalSize, unsigned vertex, unsigned* vertexInds );
	double SurfaceAdaptor_Wedge3D( SurfaceAdaptor* self, Mesh* mesh, 
				     unsigned* globalSize, unsigned vertex, unsigned* vertexInds );
	double SurfaceAdaptor_Plateau( SurfaceAdaptor* self, Mesh* mesh, 
				     unsigned* globalSize, unsigned vertex, unsigned* vertexInds );
	double SurfaceAdaptor_Topo_Data( SurfaceAdaptor* self, Mesh* mesh, 
				     unsigned* globalSize, unsigned vertex, unsigned* vertexInds );
	double SurfaceAdaptor_Sine( SurfaceAdaptor* self, Mesh* mesh, 
				    unsigned* globalSize, unsigned vertex, unsigned* vertexInds );
	double SurfaceAdaptor_Cosine( SurfaceAdaptor* self, Mesh* mesh, 
				      unsigned* globalSize, unsigned vertex, unsigned* vertexInds );

#endif /* __StgDomain_Mesh_SurfaceAdaptor_h__ */

