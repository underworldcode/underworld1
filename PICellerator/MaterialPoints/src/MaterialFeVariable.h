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

#ifndef __PICellerator_MaterialPoints_MaterialFeVariable_h__
#define __PICellerator_MaterialPoints_MaterialFeVariable_h__
	
	/** Textual name of this class */
	extern const Type MaterialFeVariable_Type;

	/** MaterialFeVariable class contents */
	#define __MaterialFeVariable \
		/* General info */ \
		__ParticleFeVariable \
		\
		Material*                         material;                       \
		IntegrationPointsSwarm*           picIntegrationPoints;                 
		
	struct MaterialFeVariable { __MaterialFeVariable };
	
	/* --- Contstructors / Destructors --- */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define MATERIALFEVARIABLE_DEFARGS \
                PARTICLEFEVARIABLE_DEFARGS

	#define MATERIALFEVARIABLE_PASSARGS \
                PARTICLEFEVARIABLE_PASSARGS

	MaterialFeVariable* _MaterialFeVariable_New(  MATERIALFEVARIABLE_DEFARGS  );
	
	void _MaterialFeVariable_Init( MaterialFeVariable* self, Material* material ) ;
	
	/** Print the contents of an MaterialFeVariable construct */
	void* _MaterialFeVariable_DefaultNew( Name name );
	void _MaterialFeVariable_Delete( void* variable );
	void _MaterialFeVariable_Print( void* variable, Stream* stream );
	void* _MaterialFeVariable_Copy( void* feVariable, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );

	void _MaterialFeVariable_AssignFromXML( void* variable, Stg_ComponentFactory* cf, void* data );
	void _MaterialFeVariable_Build( void* variable, void* data );
	void _MaterialFeVariable_Initialise( void* variable, void* data );
	void _MaterialFeVariable_Execute( void* variable, void* data );
	void _MaterialFeVariable_Destroy( void* variable, void* data );
	void _MaterialFeVariable_ValueAtParticle( 
			void*                   materialFeVariable, 
			IntegrationPointsSwarm* swarm,
			Element_LocalIndex      lElement_I,
			void*                   particle,
			double*                 particleValue );

#endif 

