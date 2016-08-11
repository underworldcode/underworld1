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
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PopulationControl/PopulationControl.h>
#include <PICellerator/Weights/Weights.h>

#include "MaterialPoints.h"

#include <stdio.h>

Bool PICellerator_MaterialPoints_Init( int* argc, char** argv[] ) {
	Stg_ComponentRegister* componentsRegister = Stg_ComponentRegister_Get_ComponentRegister();

	Journal_Printf( Journal_Register( DebugStream_Type, (Name)"Context"  ), "In: %s\n", __func__ ); /* DO NOT CHANGE OR REMOVE */

	Stg_ComponentRegister_Add( componentsRegister, PICelleratorContext_Type, (Name)"0", _PICelleratorContext_DefaultNew  );
	
	Stg_ComponentRegister_Add( componentsRegister, MappedParticleLayout_Type, (Name)"0", _MappedParticleLayout_DefaultNew  );
	Stg_ComponentRegister_Add( componentsRegister, IntegrationPointsSwarm_Type, (Name)"0", _IntegrationPointsSwarm_DefaultNew  );
	Stg_ComponentRegister_Add( componentsRegister, MaterialPointsSwarm_Type, (Name)"0", _MaterialPointsSwarm_DefaultNew  );
	
	Stg_ComponentRegister_Add( componentsRegister, MaterialFeVariable_Type, (Name)"0", _MaterialFeVariable_DefaultNew  );
	Stg_ComponentRegister_Add( componentsRegister, Material_Type, (Name)"0", _Material_DefaultNew  );
	
	Stg_ComponentRegister_Add( componentsRegister, CoincidentMapper_Type, (Name)"0", _CoincidentMapper_DefaultNew  );
	Stg_ComponentRegister_Add( componentsRegister, NearestNeighbourMapper_Type, (Name)"0", _NearestNeighbourMapper_DefaultNew  );
	Stg_ComponentRegister_Add( componentsRegister, GaussCoincidentMapper_Type, (Name)"0", _GaussCoincidentMapper_DefaultNew  );
	Stg_ComponentRegister_Add( componentsRegister, GaussMapper_Type, (Name)"0", _GaussMapper_DefaultNew  );
	
	Stg_ComponentRegister_Add( componentsRegister, SwarmAdvector_Type, (Name)"0", _SwarmAdvector_DefaultNew  );
	Stg_ComponentRegister_Add( componentsRegister, SwarmAdvectionIJK_Type, (Name)"0", _SwarmAdvectionIJK_DefaultNew  );

	Stg_ComponentRegister_Add( componentsRegister, PeriodicBoundariesManager_Type, (Name)"0", _PeriodicBoundariesManager_DefaultNew  );
	
	Stg_ComponentRegister_Add( componentsRegister, SwarmVariableField_Type, (Name)"0", _SwarmVariableField_DefaultNew  );

	/* Doing this in alphabetical order to match ls output */
	RegisterParent( CoincidentMapper_Type,          OneToOneMapper_Type );
	RegisterParent( NearestNeighbourMapper_Type,    OneToOneMapper_Type );
	RegisterParent( GaussCoincidentMapper_Type,     OneToOneMapper_Type );
	RegisterParent( PICelleratorContext_Type,       FiniteElementContext_Type );
	RegisterParent( GaussMapper_Type,               OneToOneMapper_Type );
	RegisterParent( IntegrationPointMapper_Type,    Stg_Component_Type );
	RegisterParent( IntegrationPointsSwarm_Type,    Swarm_Type );
	RegisterParent( MappedParticleLayout_Type,      ParticleLayout_Type );
	RegisterParent( ManyToOneMapper_Type,           IntegrationPointMapper_Type );
	RegisterParent( Material_Type,                  Stg_Component_Type );
	RegisterParent( MaterialFeVariable_Type,        ParticleFeVariable_Type );
	RegisterParent( Materials_Register_Type,        NamedObject_Register_Type );
	RegisterParent( MaterialPointsSwarm_Type,       GeneralSwarm_Type );
	RegisterParent( OneToOneMapper_Type,            IntegrationPointMapper_Type );
	RegisterParent(OneToManyMapper_Type, IntegrationPointMapper_Type);
	RegisterParent( ParticleFeVariable_Type,        FeVariable_Type );
	RegisterParent( PeriodicBoundariesManager_Type, Stg_Component_Type );
	RegisterParent( SwarmAdvector_Type,             TimeIntegrand_Type );
	RegisterParent( SwarmAdvectionIJK_Type,         SwarmAdvector_Type );
	
	/* dave, 18.09.07 */
	RegisterParent( SwarmVariableField_Type,        ParticleFeVariable_Type );

  Stg_ComponentRegister_Add( componentsRegister, GeneralSwarm_Type, (Name)"0", _GeneralSwarm_DefaultNew  );
  RegisterParent( GeneralSwarm_Type, Swarm_Type );

	return True;
}


