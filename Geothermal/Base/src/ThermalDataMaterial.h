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

#ifndef __Geothermal_ThermalDataMaterialClass_h__
#define __Geothermal_ThermalDataMaterialClass_h__

extern const Type ThermalDataMaterial_Type;

#define __ThermalDataMaterial		\
    __Stg_Component			\
    FiniteElementContext* context;	\
    FeMesh* mesh;			\
    MaterialPointsSwarm* swarm;		\
    const char* filename;		\
    int step;                           \
    Material* material;			\
    int nSamplePoints;			\
    Bool loadByCell;			\
    double min[3];			\
    double max[3];			\
    Material* basinMat;			\
    const char* basinMatFilename;	\

struct ThermalDataMaterial { __ThermalDataMaterial };

ThermalDataMaterial* ThermalDataMaterial_New( Name name );

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define THERMALDATAMATERIAL_DEFARGS \
                STG_COMPONENT_DEFARGS

	#define THERMALDATAMATERIAL_PASSARGS \
                STG_COMPONENT_PASSARGS

ThermalDataMaterial* _ThermalDataMaterial_New(  THERMALDATAMATERIAL_DEFARGS  );

void _ThermalDataMaterial_Init( ThermalDataMaterial* self );

void ThermalDataMaterial_InitAll( void* _self );


void _ThermalDataMaterial_Delete( void* _self );
void _ThermalDataMaterial_Print( void* _self, Stream* stream );
void* _ThermalDataMaterial_Copy( void* _self, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
void* _ThermalDataMaterial_DefaultNew( Name name ) ;
void _ThermalDataMaterial_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) ;
void _ThermalDataMaterial_Build( void* _self, void* data ) ;
void _ThermalDataMaterial_Initialise( void* _self, void* data ) ;
void _ThermalDataMaterial_Execute( void* _self, void* data );
void _ThermalDataMaterial_Destroy( void* _self, void* data ) ;

void ThermalDataMaterial_FindMaterialBoundingBox( ThermalDataMaterial* self, double* min, double* max );

#endif 

