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

#ifndef __StgDomain_Mesh_GALESurfaceAdaptor_h__
#define __StgDomain_Mesh_GALESurfaceAdaptor_h__

	/** Textual name of this class */
	extern const Type GALESurfaceAdaptor_Type;

	/** Virtual function types */

	/** GALESurfaceAdaptor class contents */
	typedef enum {
		GALESurfaceAdaptor_SurfaceType_Wedge, 
		GALESurfaceAdaptor_SurfaceType_Plateau, 
		GALESurfaceAdaptor_SurfaceType_Topo_Data, 
		GALESurfaceAdaptor_SurfaceType_Sine, 
		GALESurfaceAdaptor_SurfaceType_Cosine, 
		GALESurfaceAdaptor_SurfaceType_Cylinder,
		GALESurfaceAdaptor_SurfaceType_Invalid
	} GALESurfaceAdaptor_SurfaceType;

	typedef struct {
				/* these vectors represent things in the x-axis and z-axis */
              double	offs[2];
              double  endOffs[2];
              double	grad[2];
	} GALESurfaceAdaptor_WedgeInfo;

	typedef struct {
          double	x1, x2, x3, x4, z1, z2, z3, z4, height;
	} GALESurfaceAdaptor_PlateauInfo;

	typedef struct {
          int nx,nz;
          double minX, maxX, minZ, maxZ, dx, dz;
          double *heights;
	} GALESurfaceAdaptor_Topo_DataInfo;

	typedef struct {
		double	origin[2];
		double	amp;
		double	freq;
	} GALESurfaceAdaptor_TrigInfo;

	typedef struct {
		double	origin[2];
                double  minX, maxX;
		double	r;
                Bool    sign;
	} GALESurfaceAdaptor_CylinderInfo;

	typedef union {
		GALESurfaceAdaptor_WedgeInfo	wedge;
		GALESurfaceAdaptor_PlateauInfo	plateau;
                GALESurfaceAdaptor_Topo_DataInfo    topo_data;
		GALESurfaceAdaptor_TrigInfo		trig;
		GALESurfaceAdaptor_CylinderInfo	cylinder;
	} GALESurfaceAdaptor_SurfaceInfo;

        typedef double (GALESurfaceAdaptor_DeformFunc)( GALESurfaceAdaptor_SurfaceInfo* self,
                                                    Mesh* mesh,
                                                    unsigned* globalSize,
                                                    unsigned vertex,
                                                    unsigned* vertexInds);

	#define __GALESurfaceAdaptor				\
		/* General info */				\
		__MeshAdaptor					\
								\
		/* Virtual info */				\
								\
		/* GALESurfaceAdaptor info */			\
		GALESurfaceAdaptor_SurfaceType	topSurfaceType;	\
		GALESurfaceAdaptor_SurfaceType	bottomSurfaceType;	\
		GALESurfaceAdaptor_SurfaceInfo	top_info;       \
		GALESurfaceAdaptor_SurfaceInfo	bottom_info;    \
                GALESurfaceAdaptor_DeformFunc       *topDeformFunc; \
                GALESurfaceAdaptor_DeformFunc       *bottomDeformFunc;


	struct GALESurfaceAdaptor { __GALESurfaceAdaptor };

	/*--------------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/



	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define GALESURFACEADAPTOR_DEFARGS \
                MESHADAPTOR_DEFARGS

	#define GALESURFACEADAPTOR_PASSARGS \
                MESHADAPTOR_PASSARGS

	GALESurfaceAdaptor* GALESurfaceAdaptor_New( Name name, AbstractContext* context );
	GALESurfaceAdaptor* _GALESurfaceAdaptor_New(  GALESURFACEADAPTOR_DEFARGS  );
	void _GALESurfaceAdaptor_Init( GALESurfaceAdaptor* self );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Virtual functions
	*/

	void _GALESurfaceAdaptor_Delete( void* adaptor );
	void _GALESurfaceAdaptor_Print( void* adaptor, Stream* stream );
	void _GALESurfaceAdaptor_AssignFromXML( void* adaptor, Stg_ComponentFactory* cf, void* data );
	void _GALESurfaceAdaptor_Build( void* adaptor, void* data );
	void _GALESurfaceAdaptor_Initialise( void* adaptor, void* data );
	void _GALESurfaceAdaptor_Execute( void* adaptor, void* data );
	void _GALESurfaceAdaptor_Destroy( void* adaptor, void* data );

	void GALESurfaceAdaptor_Generate( void* adaptor, void* _mesh, void* data );

	/*--------------------------------------------------------------------------------------------------------------------------
	** Public functions
	*/

	/*--------------------------------------------------------------------------------------------------------------------------
	** Private Member functions
	*/

	double GALESurfaceAdaptor_Wedge( GALESurfaceAdaptor_SurfaceInfo *info, Mesh* mesh, 
				     unsigned* globalSize, unsigned vertex, unsigned* vertexInds );
	double GALESurfaceAdaptor_Plateau( GALESurfaceAdaptor_SurfaceInfo *info, Mesh* mesh, 
				     unsigned* globalSize, unsigned vertex, unsigned* vertexInds );
	double GALESurfaceAdaptor_Topo_Data( GALESurfaceAdaptor_SurfaceInfo *info, Mesh* mesh, 
				     unsigned* globalSize, unsigned vertex, unsigned* vertexInds );
	double GALESurfaceAdaptor_Sine( GALESurfaceAdaptor_SurfaceInfo *info, Mesh* mesh, 
				    unsigned* globalSize, unsigned vertex, unsigned* vertexInds );
	double GALESurfaceAdaptor_Cosine( GALESurfaceAdaptor_SurfaceInfo *info, Mesh* mesh, 
				      unsigned* globalSize, unsigned vertex, unsigned* vertexInds );
        double GALESurfaceAdaptor_Cylinder( GALESurfaceAdaptor_SurfaceInfo *info,
                                        Mesh* mesh, 
                                        unsigned* globalSize, unsigned vertex,
                                        unsigned* vertexInds );

#endif /* __StgDomain_Mesh_GALESurfaceAdaptor_h__ */

