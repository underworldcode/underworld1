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
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
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

#ifndef __PICellerator_Common_PpcFeVariable_h__
#define __PICellerator_Common_PpcFeVariable_h__

	/** @see PpcFeVariable_ValueAtParticle */
	typedef void PpcFeVariable_ValueAtParticleFunction( 
		void*							particleFeVariable, 
		IntegrationPointsSwarm*	swarm, 
		Element_LocalIndex		lElement_I, 
		void*							particle, 
		double*						particleValue );
	
	/** Textual name of this class */
	extern const Type PpcFeVariable_Type;

	/** PpcFeVariable class contents */
	#define __PpcFeVariable \
		/* General info */ \
		__ParticleFeVariable \
		\
		/* Virtual info */ \
		/* PpcFeVariable info */ \
      PpcManager*           ppcMan; \
      int                   ppcTag;
		
	struct PpcFeVariable { __PpcFeVariable };

	/* --- Contstructors / Destructors --- */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPCFEVARIABLE_DEFARGS \
                PARTICLEFEVARIABLE_DEFARGS \

	#define PPCFEVARIABLE_PASSARGS \
                PARTICLEFEVARIABLE_PASSARGS \

   PpcFeVariable* PpcFeVariable_New( Name name, 
      DomainContext *context, 
      FeMesh *mesh,
      IntegrationPointsSwarm *swarm,
      Bool accumulate,
      PpcManager *ppcMan,
      int tag );

	PpcFeVariable* _PpcFeVariable_New(  PPCFEVARIABLE_DEFARGS  );
	
   void* _PpcFeVariable_DefaultNew( Name name );

	void _PpcFeVariable_Init( PpcFeVariable* self, PpcManager *ppcMan, int tag );
	
	void _PpcFeVariable_AssignFromXML( void* variable, Stg_ComponentFactory* cf, void* data );

	void _PpcFeVariable_Build( void* variable, void* data );

	void _PpcFeVariable_Initialise( void* variable, void* data );

	void _PpcFeVariable_Execute( void* variable, void* data );

	void _PpcFeVariable_Destroy( void* variable, void* data );

	/** Returns in particleValue the value represented on this particle */
	#define PpcFeVariable_ValueAtParticle( particleFeVariable, swarm, lElement_I, particle, particleValue ) \
		( (PpcFeVariable*)(particleFeVariable) )->_valueAtParticle(                                     \
			(particleFeVariable), \
			(swarm), \
			(lElement_I), \
			(particle), \
			(particleValue) )

   void _PpcFeVariable_ValueAtParticle( void* _self, IntegrationPointsSwarm *swarm, unsigned lElement_I, void *particle, double *value );

#endif 

