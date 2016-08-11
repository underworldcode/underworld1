/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University 
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 		* Redistributions of source code must retain the above copyright notice, 
** 			this list of conditions and the following disclaimer.
** 		* Redistributions in binary form must reproduce the above copyright 
**			notice, this list of conditions and the following disclaimer in the 
**			documentation and/or other materials provided with the distribution.
** 		* Neither the name of the Monash University nor the names of its contributors 
**			may be used to endorse or promote products derived from this software 
**			without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
** THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
** PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS 
** BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
** CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
** SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
** OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
**
** Contact:
*%  Louis.Moresi - Louis.Moresi@monash.edu
*%
** Contributors:
** 
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include <mpi.h>
#include <assert.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>
#include "Geothermal/Base/Base.h"

const Type Geothermal_GOCADOutput_Type = (const Type)"Geothermal_GOCADOutput";

typedef struct {
	__Codelet;
	FeVariable*		temperatureField;
	Swarm*			swarm;
	BuoyancyMaterial* 	materialMgr;
	Name			outputPath;
} Geothermal_GOCADOutput;

void Geothermal_GOCADOutput_Dump( void* _context );

void _Geothermal_GOCADOutput_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
	Geothermal_GOCADOutput*		self		= (Geothermal_GOCADOutput*)component;
	Dictionary*			dict		= Codelet_GetPluginDictionary( component, cf->rootDict );
	Bool				enabled;
	
	self->temperatureField = Stg_ComponentFactory_ConstructByName( cf, Dictionary_GetString( dict, "TemperatureField" ), FeVariable, True, data );
	self->swarm = Stg_ComponentFactory_ConstructByName( cf, Dictionary_GetString( dict, "Swarm" ), Swarm, True, data );
	self->materialMgr = Stg_ComponentFactory_ConstructByName( cf, Dictionary_GetString( dict, "MaterialManager" ), BuoyancyMaterial, False, data );
	self->context = (AbstractContext*)Stg_ComponentFactory_ConstructByName( cf, "context", UnderworldContext, True, data  ); 
	self->outputPath = Dictionary_Entry_Value_AsString( Dictionary_Get( dict, "outputPath" ) );
	enabled = Dictionary_Entry_Value_AsBool( Dictionary_Get( dict, "enabled" ) );
	if( enabled ) {
		ContextEP_Append( self->context, AbstractContext_EP_Dump, Geothermal_GOCADOutput_Dump );
	}

	if( self->context->nproc != 1){
	   sleep(1*(self->context->rank+1));   //bit hackish, but ensures root proc runs firewall first so that error message is printed 
	   Journal_Firewall( self->context->nproc == 1, Journal_Register( Debug_Type, (Name)Geothermal_GOCADOutput_Type ),
			"\n\n\nIn func %s, GOCADOutput does not support work in parallel. \n\n\n",
			__func__  );
	}

}

void _Geothermal_GOCADOutput_Build( void* component, void* data ) {
}

void _Geothermal_GOCADOutput_Initialise( void* component, void* data ) {
}

void _Geothermal_GOCADOutput_Destroy( void* component, void* data ) {
}

void* _Geothermal_GOCADOutput_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Geothermal_GOCADOutput);
	Type                                                      type = Geothermal_GOCADOutput_Type;
	Stg_Class_DeleteFunction*                              _delete = _Codelet_Delete;
	Stg_Class_PrintFunction*                                _print = _Codelet_Print;
	Stg_Class_CopyFunction*                                  _copy = _Codelet_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Geothermal_GOCADOutput_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Geothermal_GOCADOutput_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Geothermal_GOCADOutput_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Geothermal_GOCADOutput_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Codelet_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Geothermal_GOCADOutput_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return _Codelet_New(  CODELET_PASSARGS  );
}

Index Geothermal_GOCADOutput_Register( PluginsManager* pluginsManager ) {
	return PluginsManager_Submit( pluginsManager, Geothermal_GOCADOutput_Type, (Name)"0", _Geothermal_GOCADOutput_DefaultNew  );
}

void WriteFieldHeaderFile( Geothermal_GOCADOutput* go ) {
	FeMesh*		mesh	= go->temperatureField->feMesh;
	Grid**		grid	= (Grid**)Mesh_GetExtension( mesh, Grid*, "vertexGrid" );
	unsigned* 	sizes	= (*grid)->sizes;
	FILE* 		fp;
	char		filename[200];

	sprintf( filename, "%s/temp.gocad", go->context->outputPath );
	fp = fopen( filename, "w" );
	fprintf( fp, "GOCAD SGrid 1\n" );
	fprintf( fp, "HEADER {\n" );
	fprintf( fp, "name:temperature_grid\n" );
	fprintf( fp, "ascii:on\n" );
	fprintf( fp, "double_precision_binary:on\n" );
	fprintf( fp, "}\n" );
	fprintf( fp, "GOCAD_ORIGINAL_COORDINATE_SYSTEM\n" );
	fprintf( fp, "NAME Default\n" );
	fprintf( fp, "AXIS_NAME \"X\" \"Y\" \"Z\"\n" );
	fprintf( fp, "AXIS_UNIT \"m\" \"m\" \"m\"\n" );
	fprintf( fp, "ZPOSITIVE Elevation\n" );
	fprintf( fp, "END_ORIGINAL_COORDINATE_SYSTEM\n" );
	fprintf( fp, "AXIS_N %u %u %u\n", sizes[0], sizes[2], sizes[1] );
	fprintf( fp, "PROP_ALIGNMENT POINTS\n" );
	fprintf( fp, "ASCII_DATA_FILE %s\n", "temp.vals.gocad" );
	fprintf( fp, "PROPERTY 1 \"temperature\"\n" );
	fprintf( fp, "PROPERTY_CLASS 1 \"temperature\"\n" );
	fprintf( fp, "PROPERTY_CLASS_HEADER 1 \"temperature\" {\n" );
	fprintf( fp, "}\n" );
	fprintf( fp, "PROPERTY_SUBCLASS 1 QUANTITY Float\n" );
	fprintf( fp, "PROP_ORIGINAL_UNIT 1 none\n" );
	fprintf( fp, "PROP_UNIT 1 none\n" );
	fprintf( fp, "PROP_NO_DATA_VALUE 1 -99999\n" );
	fprintf( fp, "PROP_ESIZE 1 4\n" );
	fprintf( fp, "END\n" );
	fclose( fp );
}

void WriteFieldValuesFile( Geothermal_GOCADOutput* go ) {
	FeVariable*			temp	= go->temperatureField;
	FeMesh*				mesh	= go->temperatureField->feMesh;
	Grid**				grid	= (Grid**)Mesh_GetExtension( mesh, Grid*, "vertexGrid" );
	unsigned*			size	= (*grid)->sizes;
	FILE*				fp;
	unsigned 			gNode, i, j, k, ijk[3];
	double				*coord, T;
	char				filename[200];

	sprintf( filename, "%s/temp.vals.gocad", go->context->outputPath );
	fp = fopen( filename, "w" );
	fprintf( fp, "*\n" );
	fprintf( fp, "*   X\t\tY\t\tZ\t\ttemperature\tI\tJ\tK\n" );
	fprintf( fp, "*\n" );

	/* NOTE only works in serial for now */
	for( j = 0; j < size[1]; j++ ) {
		for( k = 0; k < size[2]; k++ ) {
			for( i = 0; i < size[0]; i++ ) {
				ijk[0] = i;
				ijk[1] = j;
				ijk[2] = k;
				gNode = Grid_Project( *grid, ijk );
				coord = mesh->verts[gNode];
				FeVariable_GetValueAtNode( temp, gNode, &T );
				fprintf( fp, "%f %f %f %f %u %u %u\n", coord[0], coord[2], coord[1], T, ijk[0], ijk[2], ijk[1] );
			}
		}
	}

	fclose( fp );
}

void WritePropertiesFile( Geothermal_GOCADOutput* go ) {
	FeMesh*				mesh	= go->temperatureField->feMesh;
	Swarm*				swarm	= go->swarm;
	BuoyancyMaterial*		matMgr	= go->materialMgr;
	FILE*				fp;
	unsigned 			el_i, cell_i, part_i;
	double				k, Q, gCoord[3];
	IntegrationPoint*		particle;
	RheologyMaterial*		mat;
	BuoyancyMaterial_MaterialExt*	matExt;
	unsigned 			pCount	= 0;
	char				filename[200];

	sprintf( filename, "%s/properties.gocad", go->context->outputPath );
	fp = fopen( filename, "w" );
	fprintf( fp, "GOCAD VSet 1\n" );
	fprintf( fp, "HEADER {\n" );
	fprintf( fp, "name:properties\n" );
	fprintf( fp, "}\n" );
	fprintf( fp, "GOCAD_ORIGINAL_COORDINATE_SYSTEM\n" );
	fprintf( fp, "NAME Default\n" );
	fprintf( fp, "AXIS_NAME \"X\" \"Y\" \"Z\"\n" );
	fprintf( fp, "AXIS_UNIT \"m\" \"m\" \"m\"\n" );
	fprintf( fp, "ZPOSITIVE Elevation\n" );
	fprintf( fp, "END_ORIGINAL_COORDINATE_SYSTEM\n" );
	fprintf( fp, "PROPERTIES k Q\n" );
	fprintf( fp, "PROP_LEGAL_RANGES **none** **none** **none** **none**\n" );
	fprintf( fp, "NO_DATA_VALUES -99999 -99999\n" );
	fprintf( fp, "PROPERTY_CLASSES k Q\n" );
	fprintf( fp, "PROPERTY_KINDS unknown unknown\n" );
	fprintf( fp, "PROPERTY_SUBCLASSES QUANTITY Float QUANTITY Float\n" );
	fprintf( fp, "ESIZES 1 1\n" );
	fprintf( fp, "UNITS none none\n" );
	fprintf( fp, "PROPERTY_CLASS_HEADER k {\n" );
	fprintf( fp, "name:k\n" );
	fprintf( fp, "}\n" );
	fprintf( fp, "PROPERTY_CLASS_HEADER Q {\n" );
	fprintf( fp, "name:Q\n" );
	fprintf( fp, "}\n" );

	/* write the mateiral properties (thermal conductivity and heat production) */
	for( el_i = 0; el_i < Mesh_GetLocalSize( mesh, MT_VOLUME ); el_i++ ) {
		cell_i = CellLayout_MapElementIdToCellId( swarm->cellLayout, el_i );
		
		for( part_i = 0; part_i < swarm->cellParticleCountTbl[cell_i]; part_i++ ) {
			particle = (IntegrationPoint*)Swarm_ParticleInCellAt( swarm, cell_i, part_i );
			mat = (RheologyMaterial*)IntegrationPointsSwarm_GetMaterialOn( (IntegrationPointsSwarm*)swarm, particle );
			matExt = ExtensionManager_Get( mat->extensionMgr, mat, matMgr->materialExtHandle );
			k = matExt->extensions[0];
			Q = matExt->extensions[1];

			FeMesh_CoordLocalToGlobal( mesh, el_i, particle->xi, gCoord );
			fprintf( fp, "PVRTX %u %f %f %f %f %f\n", pCount, gCoord[0], gCoord[2], gCoord[1], k, Q );
			pCount++;
		}
	}
	fprintf( fp, "END\n" );
	close( fp );
}

void Geothermal_GOCADOutput_Dump( void* _context ) {
	UnderworldContext*		context		= (UnderworldContext*)_context;
	Geothermal_GOCADOutput*		self;
	unsigned			codelet_i;

	/* classically obfuscated StGermain way of getting the self ptr */
	for( codelet_i = 0; codelet_i < context->plugins->codelets->count; codelet_i++ ) {
		Codelet*	codelet = (Codelet*)context->plugins->codelets->data[codelet_i];
		if( !strcmp( codelet->type, Geothermal_GOCADOutput_Type ) ) {
			self = (Geothermal_GOCADOutput*)codelet;
			break;
		}
	}

	WriteFieldHeaderFile( self );
	WriteFieldValuesFile( self );
	if(self->materialMgr) WritePropertiesFile( self );
}
