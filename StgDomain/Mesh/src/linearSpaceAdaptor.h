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

#ifndef __StgDomain_Mesh_LinearSpaceAdaptor_h__
#define __StgDomain_Mesh_LinearSpaceAdaptor_h__

	typedef struct {
	  double x, y, p, y0;
	} linearSpaceAdaptor_Segment;

	 /** Textual name of this class */
	 extern const Type LinearSpaceAdaptor_Type;

	 /** Virtual function types */
#define __LinearSpaceAdaptor								\
  /* General info */											\
  __MeshAdaptor												\
  unsigned                     nSegmentsx;			\
  unsigned                     nSegmentsy;			\
  unsigned                     nSegmentsz;			\
  linearSpaceAdaptor_Segment*  tablex;					\
  linearSpaceAdaptor_Segment*  tabley;					\
  linearSpaceAdaptor_Segment*  tablez;					\
  double                       minX;					\
  double                       maxX;					\
  double                       minY;					\
  double                       maxY;					\
  double                       minZ;					\
  double                       maxZ;					\
  Bool                         loadFromCheckPoint;	\


	struct LinearSpaceAdaptor { __LinearSpaceAdaptor };

	/*---------------------------------------------------------------------------------
	** Constructors
	*/
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define LINEARSPACEADAPTOR_DEFARGS \
                MESHADAPTOR_DEFARGS

	#define LINEARSPACEADAPTOR_PASSARGS \
                MESHADAPTOR_PASSARGS

	LinearSpaceAdaptor* LinearSpaceAdaptor_New( Name name, AbstractContext* context );
	LinearSpaceAdaptor* _LinearSpaceAdaptor_New(  LINEARSPACEADAPTOR_DEFARGS  );

	/*---------------------------------------------------------------------------------
	** Virtual functions
	*/
	void _LinearSpaceAdaptor_Delete( void* _self );
	void _LinearSpaceAdaptor_Print( void* _self, Stream* stream );
	void _LinearSpaceAdaptor_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _LinearSpaceAdaptor_Build( void* _self, void* data );
	void _LinearSpaceAdaptor_Initialise( void* _self, void* data );
	void _LinearSpaceAdaptor_Execute( void* _self, void* data );
	void _LinearSpaceAdaptor_Destroy( void* _self, void* data );

	/*---------------------------------------------------------------------------------
	** Public functions
	*/
	void LinearSpaceAdaptor_Generate( void* _self, void* _mesh, void* data );

	/*---------------------------------------------------------------------------------
	** Private Member functions
	*/
   void _LinearSpaceAdaptor_Init( void* _self );

   void   LinearSpaceAdaptor_FillTable( linearSpaceAdaptor_Segment* table, unsigned size  );
   double LinearSpaceAdaptor_MapPoint( linearSpaceAdaptor_Segment* table, unsigned size, double x );


#endif /* __StgDomain_Mesh_LinearSpaceAdaptor_h__ */

