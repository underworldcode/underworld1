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

#ifndef __PICellerator_MaterialPoints_GeneralSwarm_h__
#define __PICellerator_MaterialPoints_GeneralSwarm_h__

/* Textual name of this class */
extern const Type GeneralSwarm_Type;

/* GeneralSwarm information */
#define __GeneralSwarm \
		__Swarm \
		\
		SwarmAdvector*                        swarmAdvector;        \
		EscapedRoutine*                       escapedRoutine;       \
		SwarmVariable*                        particleCoordVariable; /** Set only if a global coord system swarm. */ \
 
struct GeneralSwarm
{
   __GeneralSwarm
};


/*---------------------------------------------------------------------------------------------------------------------
** Constructors
*/

#ifndef ZERO
#define ZERO 0
#endif

#define GENERALSWARM_DEFARGS \
                SWARM_DEFARGS

#define GENERALSWARM_PASSARGS \
                SWARM_PASSARGS

GeneralSwarm* _GeneralSwarm_New(  GENERALSWARM_DEFARGS  );

void _GeneralSwarm_Delete( void* swarm );
#define GeneralSwarm_Copy( self ) \
		(GeneralSwarm*) Stg_Class_Copy( self, NULL, False, NULL, NULL )
#define GeneralSwarm_DeepCopy( self ) \
		(GeneralSwarm*) Stg_Class_Copy( self, NULL, True, NULL, NULL )

void* _GeneralSwarm_DefaultNew( Name name ) ;

void _GeneralSwarm_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) ;
void _GeneralSwarm_Build( void* swarm, void* data ) ;
void _GeneralSwarm_Initialise( void* swarm, void* data ) ;
void _GeneralSwarm_Execute( void* swarm, void* data );
void _GeneralSwarm_Destroy( void* swarm, void* data ) ;


void _GeneralSwarm_Init(
   void*                                 swarm,
   EscapedRoutine*                       escapedRoutine );

/** TimeIntegrator hook, used to update envoke population control routines after advection */
void _GeneralSwarm_UpdateHook( void* timeIntegrator, void* swarm );

/* Public functions */

/** Returns the particle extension at given point index */
void* GeneralSwarm_GetExtensionAt( void* swarm, Index point_I, Index extHandle );

void GeneralSwarm_AddParticlesFromCoordArray( void* _swarm, Index count, Index dim, double* array );

int GeneralSwarm_AddParticle( void* swarm, Index dim, double xI, double xJ, double xK );


#endif

