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
** Copyright (C) 2008, California Institute of Technology
** Modified for GALEDivergenceForce by Walter Landry
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


#ifndef __Gale_Utils_GALEDivergenceForce_h__
#define __Gale_Utils_GALEDivergenceForce_h__

	/** Textual name of this class */
	extern const Type GALEDivergenceForce_Type;

	/** GALEDivergenceForce class contents */
	#define __GALEDivergenceForce \
		/* General info */ \
		__ForceTerm \
		Stg_Shape*                                 domainShape;    \
                FeMesh*                                    geometryMesh; \
                GALEStressBC_Entry                             force;

	struct GALEDivergenceForce { __GALEDivergenceForce };

	#ifndef ZERO
	#define ZERO 0
	#endif

	#define DIVERGENCEFORCE_DEFARGS \
                FORCETERM_DEFARGS

	#define DIVERGENCEFORCE_PASSARGS \
                FORCETERM_PASSARGS

        GALEDivergenceForce* _GALEDivergenceForce_New( DIVERGENCEFORCE_DEFARGS);
	
	void GALEDivergenceForce_InitAll( 
		void*                                               forceTerm,
                FiniteElementContext*	context,
		ForceVector*                                        forceVector,
		Swarm*                                              integrationSwarm,
                Stg_Shape* domainShape,
                FeMesh* geometryMesh,
                GALEStressBC_Entry force);

	void _GALEDivergenceForce_Delete( void* forceTerm );
	void _GALEDivergenceForce_Print( void* forceTerm, Stream* stream );

	void* _GALEDivergenceForce_DefaultNew( Name name ) ;
        void _GALEDivergenceForce_AssignFromXML( void* forceTerm, Stg_ComponentFactory* cf, void* data ) ;
	void _GALEDivergenceForce_Build( void* forceTerm, void* data ) ;
	void _GALEDivergenceForce_Initialise( void* forceTerm, void* data ) ;
	void _GALEDivergenceForce_Execute( void* forceTerm, void* data ) ;
	void _GALEDivergenceForce_Destroy( void* forceTerm, void* data ) ;

	void _GALEDivergenceForce_AssembleElement( void* forceTerm, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elForceVec ) ;

#endif
