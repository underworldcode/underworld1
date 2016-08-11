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
**	Describes special "linked" degrees of freedom.
**
** Assumptions:
**
** Comments:
**
**	
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgFEM_Discretisation_LinkedDofInfo_h__
#define __StgFEM_Discretisation_LinkedDofInfo_h__

	extern const Type LinkedDofInfo_Type;
	
	#define __LinkedDofInfo \
		__Stg_Component \
		\
		DomainContext*	context; \
		Dictionary*		dictionary; \
		/** A mesh that this indo is based around */ \
		Mesh*				mesh; \
		/** A DofLayout that this info is based around */ \
		DofLayout*		dofLayout; \
		/** an array of ints, specifying for each local dof whether it is linked, and if so where to */ \
		int**				linkedDofTbl; \
		/** count of how many linked dof sets are active */ \
		Index				linkedDofSetsCount; \
		Index				linkedDofSetsSize; \
		Index				linkedDofSetsDelta; \
		/** For each linked dof set, records the eq num they all map to */ \
		int*				eqNumsOfLinkedDofs; \

	struct LinkedDofInfo { __LinkedDofInfo };



	/* +++ Constructors / Destructors +++ */
	
	/** Create a linkedDofInfo */
	void* _LinkedDofInfo_DefaultNew( Name name );
	
	LinkedDofInfo* LinkedDofInfo_New( 
		Name				name,
		DomainContext*	context,
		void*				mesh,
		DofLayout*		dofLayout,
		Dictionary*		dictionary );
	
	/** Creation implementation */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define LINKEDDOFINFO_DEFARGS \
                STG_COMPONENT_DEFARGS

	#define LINKEDDOFINFO_PASSARGS \
                STG_COMPONENT_PASSARGS

	LinkedDofInfo* _LinkedDofInfo_New(  LINKEDDOFINFO_DEFARGS  );
	
	/** Initialisation implementation functions */
	void _LinkedDofInfo_Init( void* linkedDofInfo, DomainContext* context, void* mesh, DofLayout* dofLayout, Dictionary* dictionary );
	
	/** Stg_Class_Delete implementation. */
	void _LinkedDofInfo_Delete( void* linkedDofInfo );
	
	/* +++ Virtual Function Interfaces & Implementations +++ */
	
	/** Print implementation */
	void _LinkedDofInfo_Print( void* linkedDofInfo, Stream* stream );
	
	void _LinkedDofInfo_AssignFromXML( void* linkedDofInfo, Stg_ComponentFactory *cf, void* data );
	
	void _LinkedDofInfo_Execute( void* linkedDofInfo, void *data );
	
	void _LinkedDofInfo_Destroy( void* linkedDofInfo, void *data );

	/* Copy */
	#define LinkedDofInfo_Copy( self ) \
		(LinkedDofInfo*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define LinkedDofInfo_DeepCopy( self ) \
		(LinkedDofInfo*)Stg_Class_Copy( self, NULL, True, NULL, NULL )
	
	void* _LinkedDofInfo_Copy( void* linkedDofInfo, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );

	void _LinkedDofInfo_Build( void* linkedDofInfo, void* data );

	void _LinkedDofInfo_Initialise( void* linkedDofInfo, void* data );

	Index LinkedDofInfo_AddDofSet( void* linkedDofInfo );
	
	void LinkedDofInfo_AddDofToSet( void* linkedDofInfo, Index linkedDofSet_I, Node_Index node_I, Dof_Index nodeLocalDof_I );
	
	void LinkedDofInfo_AddDofsToSet_FromIndexSet( void* linkedDofInfo, Index linkedDofSet_I, IndexSet* nodeSet, Dof_Index nodeLocalDof_I );
		
#endif

