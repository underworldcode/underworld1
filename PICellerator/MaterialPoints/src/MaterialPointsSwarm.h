/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**	Melbourne, 3053, Australia.
** Copyright (c) 2005-2010, Monash University, Building 28, Monash University Clayton Campus,
**	Victoria, 3800, Australia
**
** Primary Contributing Organisations:
**	Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**	AuScope - http://www.auscope.org
**	Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**
** Contributors:
**	Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**	Patrick D. Sunter, Software Engineer, VPAC. (patrick@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**	Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
**	Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**	Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**	David Stegman, Postdoctoral Fellow, Monash University. (david.stegman@sci.monash.edu.au)
**	Wendy Sharples, PhD Student, Monash University (wendy.sharples@sci.monash.edu.au)
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
**	Represents a set of points which track materials in the domain. It can be advected during the simulation and
**	can be population controlled for load balancing purposes.
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __PICellerator_MaterialPoints_MaterialPointsSwarm_h__
#define __PICellerator_MaterialPoints_MaterialPointsSwarm_h__

	/* Textual name of this class */
	extern const Type MaterialPointsSwarm_Type;

	/* MaterialPointsSwarm information */
	#define __MaterialPointsSwarm \
		__GeneralSwarm \
		\
		Material*                             material;             /**< For material assignment */ \
		Materials_Register*                   materials_Register;   \
		SwarmVariable*                        materialIndexVariable; \
		Bool                                  geomodHack; \
		Bool                                  overrideMaterialCheck;

	struct MaterialPointsSwarm { __MaterialPointsSwarm };
	
	
	/*---------------------------------------------------------------------------------------------------------------------
	** Constructors
	*/
	
   MaterialPointsSwarm* MaterialPointsSwarm_New(
      Name                                  name,
      AbstractContext*                      context,
      void*                                 cellLayout,
      void*                                 particleLayout,
      Dimension_Index                       dim,
      SizeT                                 particleSize,
      EscapedRoutine*                       escapedRoutine, 
      Material*                             material,
      Variable_Register*                    swarmVariable_Register,
      ExtensionManager_Register*            extensionMgr_Register,
      Materials_Register*                   materials_Register,		
      MPI_Comm                              comm,
      void*                                 ics );
		
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define MATERIALPOINTSSWARM_DEFARGS \
                GENERALSWARM_DEFARGS

	#define MATERIALPOINTSSWARM_PASSARGS \
                GENERALSWARM_PASSARGS

	MaterialPointsSwarm* _MaterialPointsSwarm_New(  MATERIALPOINTSSWARM_DEFARGS  );

	void _MaterialPointsSwarm_Delete( void* swarm );
	void _MaterialPointsSwarm_Print( void* swarm, Stream* stream );
	#define MaterialPointsSwarm_Copy( self ) \
		(MaterialPointsSwarm*) Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define MaterialPointsSwarm_DeepCopy( self ) \
		(MaterialPointsSwarm*) Stg_Class_Copy( self, NULL, True, NULL, NULL )
	void* _MaterialPointsSwarm_Copy( void* swarm, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	
	void* _MaterialPointsSwarm_DefaultNew( Name name ) ;

	void _MaterialPointsSwarm_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) ;
	void _MaterialPointsSwarm_Build( void* swarm, void* data ) ;
	void _MaterialPointsSwarm_Initialise( void* swarm, void* data ) ;
	void _MaterialPointsSwarm_Execute( void* swarm, void* data );
	void _MaterialPointsSwarm_Destroy( void* swarm, void* data ) ;


	void _MaterialPointsSwarm_Init( 
		void*                                 swarm,
		Material*                             material,
		Materials_Register*                   materials_Register );

	/** TimeIntegrator hook, used to update envoke population control routines after advection */
	void _MaterialPointsSwarm_UpdateHook( void* timeIntegrator, void* swarm );

	/* Public functions */

	/** Sets the material instance associated with the given point index */
	void MaterialPointsSwarm_SetMaterialAt( void* swarm, Index point_I, Index materialIndex );

	/** Returns the material instance associated with the given point index */
	Material* MaterialPointsSwarm_GetMaterialAt( void* swarm, Index point_I );

	/** Returns the material instance associated with the given MaterialPoint */
	Material* MaterialPointsSwarm_GetMaterialOn( void* swarm, void* particle );

	/** Returns the material instance associated with the given point index */
	Index MaterialPointsSwarm_GetMaterialIndexAt( void* swarm, Index point_I );

        /** Get the extension on a material. */
        void* MaterialPointsSwarm_GetMaterialExtensionOn( void* swarm, void *matPoint, Index extHandle );

        /** Get the extension on a material index. */
        void* MaterialPointsSwarm_GetMaterialExtensionAt( void* swarm, int matPointInd, Index extHandle );

	/** Returns the particle extension at given point index */
	void* MaterialPointsSwarm_GetExtensionAt( void* swarm, Index point_I, Index extHandle );
	
#endif

