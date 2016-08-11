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
**     An IntegrationPointMapper which maps one MaterialPointsSwarm to one IntegrationPointsSwarm.
**     Each material point will then have a corresponding integration point generated during mapping.
**
** Assumptions:
**
** Comments:
**     Reverse mapping between integration point to material can be done through the IntegrationPointsSwarm or
**     IntegrationPointMapper interface. It does this by extending each IntegrationPoint with a MaterialPointRef
**     struct, which has enough information to fetch a particular point out of a particular swarm again.
**     
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __PICellerator_MaterialPoints_OneToManyMapper_h__
#define __PICellerator_MaterialPoints_OneToManyMapper_h__

	extern const Type OneToManyMapper_Type;

        struct OneToManyRef {
	    int numParticles;
	    int *particleInds;
	    float *weights;
        };

	/* OneToManyMapper information */
	#define __OneToManyMapper \
		__IntegrationPointMapper \
		\
		Stream*				errorStream; \
		MaterialPointsSwarm*		materialSwarm; \
		Index                           refHandle; /**< Extension handle to reference struct of material points */

	struct OneToManyMapper { __OneToManyMapper };

	#ifndef ZERO
	#define ZERO 0
	#endif

	#define ONETOMANYMAPPER_DEFARGS \
                INTEGRATIONPOINTMAPPER_DEFARGS

	#define ONETOMANYMAPPER_PASSARGS \
                INTEGRATIONPOINTMAPPER_PASSARGS
	
OneToManyMapper* _OneToManyMapper_New( ONETOMANYMAPPER_DEFARGS );

void _OneToManyMapper_Init( void* mapper, MaterialPointsSwarm* materialSwarm );

	void _OneToManyMapper_Delete( void* mapper );
	void _OneToManyMapper_Print( void* mapper, Stream* stream );
	#define OneToManyMapper_Copy( self ) \
		(OneToManyMapper*) Stg_Class_Copy( self, NULL, False, NULL, NULL )
	#define OneToManyMapper_DeepCopy( self ) \
		(OneToManyMapper*) Stg_Class_Copy( self, NULL, True, NULL, NULL )
	void* _OneToManyMapper_Copy( void* mapper, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	
	void _OneToManyMapper_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data );
	void _OneToManyMapper_Build( void* mapper, void* data ) ;
	void _OneToManyMapper_Initialise( void* mapper, void* data );
	void _OneToManyMapper_Execute( void* mapper, void* data );
	void _OneToManyMapper_Destroy( void* mapper, void* data );

	MaterialPointsSwarm** _OneToManyMapper_GetMaterialPointsSwarms( void* mapper, Index* count );	
	Material_Index _OneToManyMapper_GetMaterialIndexOn( void* mapper, void* point );
	void* _OneToManyMapper_GetExtensionOn( void* mapper, void* point, ExtensionInfo_Index extHandle );
        double _OneToManyMapper_GetDoubleFromExtension(void* mapper, void* intPoint, ExtensionInfo_Index extHandle, int offs);
        double _OneToManyMapper_GetDoubleFromMaterial(void* mapper, void* intPoint, ExtensionInfo_Index extHandle, int offs);

        OneToManyRef* OneToManyMapper_GetMaterialRef( void* mapper, void* intPoint );

#endif
