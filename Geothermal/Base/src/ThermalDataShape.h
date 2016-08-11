/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing 
** (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**   Victorian Partnership for Advanced Computing Ltd, Computational Software 
**     Development - http://csd.vpac.org
**   Australian Computational Earth Systems Simulator - 
**     http://www.access.edu.au
**   Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**   Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Robert Turnbull, Research Assistant, Monash University. 
**     (robert.turnbull@sci.monash.edu.au)
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**   Louis Moresi, Associate Professor, Monash University. 
**     (louis.moresi@sci.monash.edu.au)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**   Julian Giordani, Research Assistant, Monash University.
**     (julian.giordani@sci.monash.edu.au)
**   Vincent Lemiale, Postdoctoral Fellow, Monash University. 
**     (vincent.lemiale@sci.monash.edu.au)
**   Kent Humphries, Software Engineer, VPAC. (kenth@vpac.org)
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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __Geothermal_ThermalDataShapeClass_h__
#define __Geothermal_ThermalDataShapeClass_h__

extern const Type ThermalDataShape_Type;

#define __ThermalDataShape				\
    __Stg_Shape						\
    Mesh* mesh;						\
    MaterialPointsSwarm* swarm;				\
    const char* filename;				\
    int step;                                           \
    IndexSet* elSet;

struct ThermalDataShape { __ThermalDataShape };

ThermalDataShape* ThermalDataShape_New( Name name );

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define THERMALDATASHAPE_DEFARGS \
                STG_SHAPE_DEFARGS

	#define THERMALDATASHAPE_PASSARGS \
                STG_SHAPE_PASSARGS

ThermalDataShape* _ThermalDataShape_New(  THERMALDATASHAPE_DEFARGS  );

void _ThermalDataShape_Init( ThermalDataShape* self );

void ThermalDataShape_InitAll( void* _self );


void _ThermalDataShape_Delete( void* _self );
void _ThermalDataShape_Print( void* _self, Stream* stream );
void* _ThermalDataShape_Copy( void* _self, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
void* _ThermalDataShape_DefaultNew( Name name ) ;
void _ThermalDataShape_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) ;
void _ThermalDataShape_Build( void* _self, void* data ) ;
void _ThermalDataShape_Initialise( void* _self, void* data ) ;
void _ThermalDataShape_Execute( void* _self, void* data );
void _ThermalDataShape_Destroy( void* _self, void* data ) ;

Bool ThermalDataShape_IsCoordInside( void* _self, Coord coord ) ;

#endif 

