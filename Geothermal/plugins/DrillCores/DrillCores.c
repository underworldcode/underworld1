/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2013, Monash University 
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
**  Steve Quenette (steve.quenette@monash.edu)
**
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

#include <gLucifer/Base/Base.h>
#include <gLucifer/DrawingObjects/DrawingObjects.h>

#include <string.h>
#include <stdlib.h>

const Type Geothermal_DrillCores_Type = (const Type)"Geothermal_DrillCores";

typedef enum { LatLong, EastNorthings } Geothermal_DrillCores_CoordSystem;
typedef enum { HD_Vertical_HD, HD_Vertical_GL, Surface_Vertical_GL, HD_Angle, Surface_Angle } Geothermal_DrillCores_Height_Datum;

typedef struct {
	Geothermal_DrillCores_CoordSystem  coordSystem;
	double                             longEastings;
	double                             latNorthings;
	Geothermal_DrillCores_Height_Datum datum;
	double                             depth;
	double                             elevation;
	double                             inclination; /* angle from horizontal (in degrees) */
	double                             azimuthal;   /* orientation from 0 degrees north, going clockwise */
	double                             top;         /* Effective top - calculated from datum and data provided */
	double                             bottom;      /* Effective bottom - calculated from datum and data provided */
	double                             length;      /* length - may calculated from datum and data provided */
	unsigned                           numSamples;
	char                               filename[FILENAME_MAX];
	void*                              capture;
	lucColour                          colour;
	double                             radius;
	char                               name[FILENAME_MAX];
} DrillCore;

typedef struct {
	__Codelet;
	DrillCore** 	drillCores;
	unsigned	numDrillCores;
	FieldVariable*	temperatureField;
	FieldVariable*	heatFluxField;
	FieldVariable*	conductivityField;
	FieldVariable*	heightMap;
} Geothermal_DrillCores;

void Geothermal_DrillCores_Evaluate( void* _context );

void _Geothermal_DrillCores_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
	Geothermal_DrillCores*		self		= (Geothermal_DrillCores*)component;
	UnderworldContext*  		context;
	Dictionary*			dict		= Codelet_GetPluginDictionary( component, cf->rootDict );
	Dictionary_Entry_Value*		drillCoreList;
	unsigned			drillCore_i;
	lucPlot* plotObject;
	Stream*				errorStream	= Journal_Register( ErrorStream_Type, Geothermal_DrillCores_Type );
	
	Stream_SetPrintingRank( errorStream, 0 );
	context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", UnderworldContext, True, data  ); 
	ContextEP_Append( context, AbstractContext_EP_Solve, Geothermal_DrillCores_Evaluate );
	self->context = (AbstractContext*)context;

	self->temperatureField 	= Stg_ComponentFactory_ConstructByName( cf, Dictionary_GetString( dict, 
		(Dictionary_Entry_Key)"TemperatureField" ), FieldVariable, True, data );
	self->heatFluxField 	= Stg_ComponentFactory_ConstructByName( cf, Dictionary_GetString( dict, 
		(Dictionary_Entry_Key)"HeatFluxField" ), FieldVariable, False, data );
	self->conductivityField 	= Stg_ComponentFactory_ConstructByName( cf, Dictionary_GetString( dict, 
		(Dictionary_Entry_Key)"ConductivityField" ), FieldVariable, False, data );
	self->heightMap	 	= Stg_ComponentFactory_ConstructByName( cf, Dictionary_GetString( dict, 
		(Dictionary_Entry_Key)"HeightMap" ), FieldVariable, True, data );
        drillCoreList = Dictionary_Get( dict, (Dictionary_Entry_Key)"DrillCores" );
        if( drillCoreList ) {
		self->numDrillCores = Dictionary_Entry_Value_GetCount( drillCoreList );
		self->drillCores = malloc( self->numDrillCores * sizeof(DrillCore*) );
	}
	else {
		self->numDrillCores = 0;
		self->drillCores = NULL;
	}

	for( drillCore_i = 0; drillCore_i < self->numDrillCores; drillCore_i++ ) {
		self->drillCores[drillCore_i] = malloc( sizeof(DrillCore) );
		Dictionary* drillCoreEntryDict = Dictionary_Entry_Value_AsDictionary( 
			Dictionary_Entry_Value_GetElement( drillCoreList, drillCore_i ) );
		char* tmpStr;

		/* Get the name, if given */
		strncpy( self->drillCores[drillCore_i]->name, Dictionary_GetString( drillCoreEntryDict, "name" ), FILENAME_MAX );

		/* Get the position of the drillcores */
		tmpStr = Dictionary_GetString( drillCoreEntryDict, "coordinateSystem" );
		if( strcmp( "latlong", tmpStr ) == 0 ) {
			self->drillCores[drillCore_i]->coordSystem = LatLong;
		}
		else if( strcmp( "eastnorthings", tmpStr ) == 0 ) {
			self->drillCores[drillCore_i]->coordSystem = EastNorthings;
		}
		else {
			Journal_Firewall( 0, errorStream, "Error: Unrecognised \"coordinateSystem\" - \"%s\".\n", tmpStr );
		}
		switch( self->drillCores[drillCore_i]->coordSystem ) {
			case LatLong:
				self->drillCores[drillCore_i]->longEastings = Dictionary_GetDouble( drillCoreEntryDict, "latitude");
				self->drillCores[drillCore_i]->latNorthings = Dictionary_GetDouble(drillCoreEntryDict, "longitude");
				Journal_Firewall(
					Dictionary_Get( drillCoreEntryDict, "latitude" ) != NULL, 
					errorStream, 
					"ERROR: \"latitude\" missing for drill core %u.\n", drillCore_i );
				Journal_Firewall( 
					Dictionary_Get( drillCoreEntryDict, "longitude" ) != NULL, 
					errorStream, 
					"ERROR: \"longitude\" missing for drill core %u.\n", drillCore_i );
				break;
			case EastNorthings:
				self->drillCores[drillCore_i]->longEastings = Dictionary_GetDouble( drillCoreEntryDict, "eastings");
				self->drillCores[drillCore_i]->latNorthings = Dictionary_GetDouble(drillCoreEntryDict, "northings");
				Journal_Firewall(
					Dictionary_Get( drillCoreEntryDict, "eastings" ) != NULL, 
					errorStream, 
					"ERROR: \"eastings\" missing for drill core %u.\n", drillCore_i );
				Journal_Firewall( 
					Dictionary_Get( drillCoreEntryDict, "northings" ) != NULL, 
					errorStream, 
					"ERROR: \"northings\" missing for drill core %u.\n", drillCore_i );
				break;
			default:
				Journal_Firewall( 0, errorStream, "Error: Should not be here!\n" );
				break;
		}

		/* The top and number of samples must be given */
		self->drillCores[drillCore_i]->numSamples = 
			Dictionary_GetUnsignedInt_WithDefault( drillCoreEntryDict, "numSamples", 10 );

		/* Obtain the depth and length of the drill core - dependent on the datum given
		    { RL/elevation (ground level) given as height-datum or the model surface, vertical or angle(d) drill hole, and
		    depth if not angled as hieght datum or from model surface (ground level)*/
		tmpStr = Dictionary_GetString( drillCoreEntryDict, "datum" );
		if( strcmp( "HD,vertical,HD", tmpStr ) == 0 ) {
			self->drillCores[drillCore_i]->datum = HD_Vertical_HD;
		}
		else if( strcmp( "HD,vertical,GL", tmpStr ) == 0 ) {
			self->drillCores[drillCore_i]->datum = HD_Vertical_GL;
		}
		else if( strcmp( "surface,vertical,GL", tmpStr ) == 0 ) {
			self->drillCores[drillCore_i]->datum = Surface_Vertical_GL;
		}
		else if( strcmp( "HD,angle", tmpStr ) == 0 ) {
			self->drillCores[drillCore_i]->datum = HD_Angle;
		}
		else if( strcmp( "surface,angle", tmpStr ) == 0 ) {
			self->drillCores[drillCore_i]->datum = Surface_Angle;
		}
		else {
			Journal_Firewall( 0, errorStream, "Error: Unrecognised \"datum\" - \"%s\" for drill core %u.\n", tmpStr,
				 drillCore_i );
		}
		switch( self->drillCores[drillCore_i]->datum ) {
			case HD_Vertical_HD:{
				double oldLength = Dictionary_GetDouble( drillCoreEntryDict, "length" );
				self->drillCores[drillCore_i]->depth = Dictionary_GetDouble( drillCoreEntryDict, "depth" );
				self->drillCores[drillCore_i]->elevation = Dictionary_GetDouble( drillCoreEntryDict, "elevation" );
				Journal_Firewall(
					Dictionary_Get( drillCoreEntryDict, "depth" ) != NULL, 
					errorStream, 
					"ERROR: \"depth\" missing for drill core %u.\n", drillCore_i );
				Journal_Firewall(
					Dictionary_Get( drillCoreEntryDict, "elevation" ) != NULL, 
					errorStream, 
					"ERROR: \"elevation\" missing for drill core %u.\n", drillCore_i );

				/* Calculate real length from top */
				Journal_Firewall( 
					self->drillCores[drillCore_i]->elevation >= self->drillCores[drillCore_i]->depth, 
					errorStream, 
					"ERROR: \"elevation\" %g must be higher than the \"depth\" %g for drill core %u.\n",
						self->drillCores[drillCore_i]->elevation, self->drillCores[drillCore_i]->depth,
						drillCore_i );
				self->drillCores[drillCore_i]->length = 
					self->drillCores[drillCore_i]->elevation -
					self->drillCores[drillCore_i]->depth;
				self->drillCores[drillCore_i]->top = self->drillCores[drillCore_i]->elevation;
				self->drillCores[drillCore_i]->bottom = 
					self->drillCores[drillCore_i]->top - 
					self->drillCores[drillCore_i]->length;

				/* Length shouldn't be provided ... ignore. */
				if( Dictionary_Get( drillCoreEntryDict, "length" ) ) {
					Journal_Printf( errorStream, 
						"WARNING: Both \"length\" (%g) and (\"depth\" and \"elevation\") provided for "
						"drill core %u. Ignoring \"length\" value (length calculated to be %g).\n",
						oldLength, drillCore_i, self->drillCores[drillCore_i]->length );
				}

				/* Can't have angled dril core */
				if( Dictionary_Get( drillCoreEntryDict, "inclination" ) ) {
					if( fabs( self->drillCores[drillCore_i]->inclination - 90.0 ) > 0.00001 ) {
						Journal_Printf( errorStream, 
							"WARNING: \"vertical\" provided for drill core %u - "
							"\"inclination\" (%g) must be 90. Ignoring \"inclination\" value.\n", 
							drillCore_i, self->drillCores[drillCore_i]->inclination );
					}
				}
				if( Dictionary_Get( drillCoreEntryDict, "azimuthal" ) ) {
					if( fabs( self->drillCores[drillCore_i]->azimuthal - 0.0 ) > 0.00001 ) {
						Journal_Printf( errorStream, 
							"WARNING: \"vertical\" provided for drill core %u - "
							"\"azimuthal\" (%g) must be 0. Ignoring \"azimuthal\" value.\n", 
							drillCore_i, self->drillCores[drillCore_i]->azimuthal );
					}
				}
				self->drillCores[drillCore_i]->inclination = 90.0;
				self->drillCores[drillCore_i]->azimuthal = 0.0;
				break;
			}
			case HD_Vertical_GL: {
				double oldLength = Dictionary_GetDouble( drillCoreEntryDict, "length" );
				self->drillCores[drillCore_i]->depth = Dictionary_GetDouble( drillCoreEntryDict, "depth" );
				self->drillCores[drillCore_i]->elevation = Dictionary_GetDouble( drillCoreEntryDict, "elevation" );
				Journal_Firewall(
					Dictionary_Get( drillCoreEntryDict, "depth" ) != NULL, 
					errorStream, 
					"ERROR: \"depth\" missing for drill core %u.\n", drillCore_i );
				Journal_Firewall(
					Dictionary_Get( drillCoreEntryDict, "elevation" ) != NULL, 
					errorStream, 
					"ERROR: \"elevation\" missing for drill core %u.\n", drillCore_i );

				/* Calculate real length from top */
				Journal_Firewall( 
					self->drillCores[drillCore_i]->depth >= 0, 
					errorStream, 
					"ERROR: \"depth\" %g must be >= 0 for drill core %u.\n",
						self->drillCores[drillCore_i]->depth, drillCore_i );
				self->drillCores[drillCore_i]->length = self->drillCores[drillCore_i]->depth;
				self->drillCores[drillCore_i]->top = self->drillCores[drillCore_i]->elevation;
				self->drillCores[drillCore_i]->bottom = 
					self->drillCores[drillCore_i]->top - 
					self->drillCores[drillCore_i]->length;

				/* Length shouldn't be provided ... ignore. */
				if( Dictionary_Get( drillCoreEntryDict, "length" ) ) {
					Journal_Printf( errorStream, 
						"WARNING: Both \"length\" (%g) and (\"depth\" and \"elevation\") provided for "
						"drill core %u. Ignoring \"length\" value (length calculated to be %g).\n",
						oldLength, drillCore_i, self->drillCores[drillCore_i]->length );
				}

				/* Can't have angled dril core */
				if( Dictionary_Get( drillCoreEntryDict, "inclination" ) ) {
					if( fabs( self->drillCores[drillCore_i]->inclination - 90.0 ) > 0.00001 ) {
						Journal_Printf( errorStream, 
							"WARNING: \"vertical\" provided for drill core %u - "
							"\"inclination\" (%g) must be 90. Ignoring \"inclination\" value.\n", 
							drillCore_i, self->drillCores[drillCore_i]->inclination );
					}
				}
				if( Dictionary_Get( drillCoreEntryDict, "azimuthal" ) ) {
					if( fabs( self->drillCores[drillCore_i]->azimuthal - 0.0 ) > 0.00001 ) {
						Journal_Printf( errorStream, 
							"WARNING: \"vertical\" provided for drill core %u - "
							"\"azimuthal\" (%g) must be 0. Ignoring \"azimuthal\" value.\n", 
							drillCore_i, self->drillCores[drillCore_i]->azimuthal );
					}
				}
				self->drillCores[drillCore_i]->inclination = 90.0;
				self->drillCores[drillCore_i]->azimuthal = 0.0;
				break;
			}
			case Surface_Vertical_GL: {
				double oldLength = Dictionary_GetDouble( drillCoreEntryDict, "length" );
				self->drillCores[drillCore_i]->depth = Dictionary_GetDouble( drillCoreEntryDict, "depth" );
				Journal_Firewall(
					Dictionary_Get( drillCoreEntryDict, "depth" ) != NULL, 
					errorStream, 
					"ERROR: \"depth\" missing for drill core %u.\n", drillCore_i );

				/* Calculate real length from top */
				Journal_Firewall( 
					self->drillCores[drillCore_i]->depth >= 0, 
					errorStream, 
					"ERROR: \"depth\" %g must be >= 0 for drill core %u.\n",
						self->drillCores[drillCore_i]->depth, drillCore_i );
				self->drillCores[drillCore_i]->length = self->drillCores[drillCore_i]->depth;

				/* Length shouldn't be provided ... ignore. */
				if( Dictionary_Get( drillCoreEntryDict, "length" ) ) {
					Journal_Printf( errorStream, 
						"WARNING: Both \"length\" (%g) and \"depth\" provided and \"elevation\" to be "
						"calculated for drill core %u. Ignoring \"length\" value (length calculated to "
						"be %g).\n",
						oldLength, drillCore_i, self->drillCores[drillCore_i]->length );
				}

				/* Can't have angled drill core */
				if( Dictionary_Get( drillCoreEntryDict, "inclination" ) ) {
					if( fabs( self->drillCores[drillCore_i]->inclination - 90.0 ) > 0.00001 ) {
						Journal_Printf( errorStream, 
							"WARNING: \"vertical\" provided for drill core %u - "
							"\"inclination\" (%g) must be 90. Ignoring \"inclination\" value.\n", 
							drillCore_i, self->drillCores[drillCore_i]->inclination );
					}
				}
				if( Dictionary_Get( drillCoreEntryDict, "azimuthal" ) ) {
					if( fabs( self->drillCores[drillCore_i]->azimuthal - 0.0 ) > 0.00001 ) {
						Journal_Printf( errorStream, 
							"WARNING: \"vertical\" provided for drill core %u - "
							"\"azimuthal\" (%g) must be 0. Ignoring \"azimuthal\" value.\n", 
							drillCore_i, self->drillCores[drillCore_i]->azimuthal );
					}
				}
				self->drillCores[drillCore_i]->inclination = 90.0;
				self->drillCores[drillCore_i]->azimuthal = 0.0;
				break;
			}
			case HD_Angle: {
				double oldDepth = Dictionary_GetDouble( drillCoreEntryDict, "depth" );
				self->drillCores[drillCore_i]->length = Dictionary_GetDouble( drillCoreEntryDict, "length" );
				self->drillCores[drillCore_i]->elevation = Dictionary_GetDouble( drillCoreEntryDict, "elevation" );
				Journal_Firewall(
					Dictionary_Get( drillCoreEntryDict, "length" ) != NULL, 
					errorStream, 
					"ERROR: \"length\" missing for drill core %u.\n", drillCore_i );
				Journal_Firewall(
					Dictionary_Get( drillCoreEntryDict, "elevation" ) != NULL, 
					errorStream, 
					"ERROR: \"elevation\" missing for drill core %u.\n", drillCore_i );

				/* Calculate real length from top */
				Journal_Firewall( 
					self->drillCores[drillCore_i]->length >= 0, 
					errorStream, 
					"ERROR: \"length\" %g must be >= 0 for drill core %u.\n",
						self->drillCores[drillCore_i]->length, drillCore_i );
				self->drillCores[drillCore_i]->top = self->drillCores[drillCore_i]->elevation;

				/* Depth shouldn't be provided ... ignore. */
				if( Dictionary_Get( drillCoreEntryDict, "depth" ) ) {
					Journal_Printf( errorStream, 
						"WARNING: Both \"depth\" (%g) and (\"length\" and \"elevation\") provided for "
						"drill core %u. Ignoring \"depth\" value (to be calculated later).\n",
						oldDepth, drillCore_i );
				}

				self->drillCores[drillCore_i]->inclination = Dictionary_GetDouble(drillCoreEntryDict,"inclination");
				self->drillCores[drillCore_i]->azimuthal = Dictionary_GetDouble( drillCoreEntryDict, "azimuthal" );
				break;
			}
			case Surface_Angle: {
				double oldDepth = Dictionary_GetDouble( drillCoreEntryDict, "depth" );
				self->drillCores[drillCore_i]->length = Dictionary_GetDouble( drillCoreEntryDict, "length" );
				Journal_Firewall(
					Dictionary_Get( drillCoreEntryDict, "length" ) != NULL, 
					errorStream, 
					"ERROR: \"length\" missing for drill core %u.\n", drillCore_i );

				/* Calculate real length from top */
				Journal_Firewall( 
					self->drillCores[drillCore_i]->length >= 0, 
					errorStream, 
					"ERROR: \"length\" %g must be >= 0 for drill core %u.\n",
						self->drillCores[drillCore_i]->length, drillCore_i );

				/* Depth shouldn't be provided ... ignore. */
				if( Dictionary_Get( drillCoreEntryDict, "depth" ) ) {
					Journal_Printf( errorStream, 
						"WARNING: Both \"depth\" (%g) and \"length\" provided and \"elevation\" to be "
						"calculated for drill core %u. Ignoring \"depth\" value (to be calculated "
						"later).\n",
						oldDepth, drillCore_i );
				}

				self->drillCores[drillCore_i]->inclination = Dictionary_GetDouble(drillCoreEntryDict,"inclination");
				self->drillCores[drillCore_i]->azimuthal = Dictionary_GetDouble( drillCoreEntryDict, "azimuthal" );
				break;
			}
			default:
				Journal_Firewall( 0, errorStream, "Error: Should not be here!\n" );
				break;
		}

		/* Vis params */
		self->drillCores[drillCore_i]->capture = Stg_ComponentFactory_ConstructByName( cf, 
			Dictionary_GetString(drillCoreEntryDict, 
			(Dictionary_Entry_Key)"drawCapture"), lucCapture, False, data ) ;
		plotObject = Stg_ComponentFactory_ConstructByName( 
			cf, Dictionary_GetString( drillCoreEntryDict, (Dictionary_Entry_Key)"plot"), lucPlot, False, data ) ;
		lucColour_FromString( &self->drillCores[drillCore_i]->colour, 
			Dictionary_GetString(drillCoreEntryDict, (Dictionary_Entry_Key)"colour") );
		self->drillCores[drillCore_i]->radius = Dictionary_GetDouble( drillCoreEntryDict, "radius" );

		if( self->drillCores[drillCore_i]->capture ) {
      /* Set some drawing parameters on capture object */
      lucCapture* capture = (lucCapture*)self->drillCores[drillCore_i]->capture;
      /* Overwrite property string - if multiple drillcores use this only the last one will have any effect on properties
       * if this is a problem, create multiple lucCapture objects for each drill core */
      snprintf(capture->properties, 2048, "scaling=%g\narrowhead=%g\nlineWidth=%g\nflat=%d", 1.0, 0.0, self->drillCores[drillCore_i]->radius, 0); 
		}

		/* Passed in a plot object? Get filename from there, otherwise use default name */
		if( strlen( self->drillCores[drillCore_i]->name ) > 0 ) {
			sprintf( self->drillCores[drillCore_i]->filename, "%s/%s.dat", context->outputPath, self->drillCores[drillCore_i]->name );
		}
		else if( plotObject ) {
			strncpy( self->drillCores[drillCore_i]->filename, plotObject->dataFileName, FILENAME_MAX );
		}
		else {
			sprintf( self->drillCores[drillCore_i]->filename, "%s/drill_core_%u.dat", context->outputPath, drillCore_i );
		}
	}
}

void _Geothermal_DrillCores_Build( void* component, void* data ) {
	Geothermal_DrillCores*	self = (Geothermal_DrillCores*)component;

	assert( self );
}

void _Geothermal_DrillCores_Initialise( void* component, void* data ) {
	Geothermal_DrillCores*	self 		= (Geothermal_DrillCores*)component;
	unsigned		drillCore_i;
	double			min[3], max[3];
	DrillCore*		drillCore;
	Stream*			errorStream	= Journal_Register( ErrorStream_Type, Geothermal_DrillCores_Type );

	assert( self );

	FieldVariable_GetMinAndMaxGlobalCoords( self->temperatureField, min, max );

	/* check to make sure the drill cores are all within the domain */
	for( drillCore_i = 0; drillCore_i < self->numDrillCores; drillCore_i++ ) {
		double 		x_length, y_length, z_length;
		double 		theta, phi;
		double		height;
		double		coord[2];
		double		bottom[3];

		drillCore = self->drillCores[drillCore_i];

		coord[0] = drillCore->longEastings;
		coord[1] = drillCore->latNorthings;

		FieldVariable_InterpolateValueAt( self->heightMap, coord, &height );

		theta 	= M_PI*drillCore->inclination/180.0; 		/* angle of inclination, in degrees */
		phi 	= M_PI*(90.0 - drillCore->azimuthal)/180.0; 	/* angle of orientation, in degrees, with north = 0 degrees
									    going clockwise */

		x_length = drillCore->length*cos( phi )*cos( theta );
		y_length = drillCore->length*sin( theta );
		z_length = drillCore->length*sin( phi )*cos( theta );

		/* Complete info for cases that require the knowledge of the model surface */
		switch( self->drillCores[drillCore_i]->datum ) {
			case HD_Vertical_HD:
			case HD_Vertical_GL:
				/* All info already known/set */
				break;
			case Surface_Vertical_GL:
				self->drillCores[drillCore_i]->elevation = height;
				self->drillCores[drillCore_i]->top = self->drillCores[drillCore_i]->elevation;
				self->drillCores[drillCore_i]->bottom = 
					self->drillCores[drillCore_i]->top - 
					self->drillCores[drillCore_i]->length;
				break;
			case HD_Angle:
				self->drillCores[drillCore_i]->bottom = self->drillCores[drillCore_i]->top - y_length;
				self->drillCores[drillCore_i]->depth = self->drillCores[drillCore_i]->bottom;
				break;
			case Surface_Angle: 
				self->drillCores[drillCore_i]->elevation = height;
				self->drillCores[drillCore_i]->top = self->drillCores[drillCore_i]->elevation;
				self->drillCores[drillCore_i]->bottom = self->drillCores[drillCore_i]->top - y_length;
				self->drillCores[drillCore_i]->depth = self->drillCores[drillCore_i]->bottom;
				break;
			default:
				Journal_Firewall( 0, errorStream, "Error: Should not be here!\n" );
				break;
		}

		bottom[0] = drillCore->longEastings + x_length;
		bottom[2] = drillCore->latNorthings + z_length;
		bottom[1] = self->drillCores[drillCore_i]->bottom;

		Journal_Firewall( 
			!( drillCore->longEastings < min[I_AXIS] || drillCore->longEastings > max[I_AXIS] ),
			errorStream, 
			"ERROR: drill core %u's initial longitude/eastings [%.8e] is outside the domain [%.8e,%.8e].\n",
			drillCore_i, drillCore->longEastings, min[I_AXIS], max[I_AXIS] );
		Journal_Firewall( 
			!( drillCore->latNorthings < min[K_AXIS] || drillCore->latNorthings > max[K_AXIS] ),
			errorStream, 
			"ERROR: drill core %u's initial latitude/northings [%.8e] is outside the domain [%.8e,%.8e].\n",
			drillCore_i, drillCore->latNorthings, min[K_AXIS], max[K_AXIS] );
		Journal_Firewall(
			!( bottom[I_AXIS] < min[I_AXIS] || bottom[I_AXIS] > max[I_AXIS] ),
			errorStream, 
			"ERROR: drill core %u's bottom longitude/eastings [%.8e] is outside the domain [%.8e,%.8e].\n",
			drillCore_i, bottom[I_AXIS], min[I_AXIS], max[I_AXIS] );
		Journal_Firewall(
			!( bottom[K_AXIS] < min[K_AXIS] || bottom[K_AXIS] > max[K_AXIS] ),
			errorStream, 
			"ERROR: drill core %u's bottom latitude/northings [%.8e] is outside the domain [%.8e,%.8e].\n",
			drillCore_i, bottom[K_AXIS], min[K_AXIS], max[K_AXIS] );
		Journal_Firewall(
			!( bottom[J_AXIS] < min[J_AXIS] ),
			errorStream, 
			"ERROR: drill core %u's depth [%.8e] is below the domain [%.8e].\n",
			drillCore_i, bottom[J_AXIS], min[J_AXIS] );

		/* Remove old files */
		remove(drillCore->filename);
	}
}

void _Geothermal_DrillCores_Destroy( void* component, void* data ) {
	Geothermal_DrillCores*	self = (Geothermal_DrillCores*)component;
	unsigned		drillCore_i;

	assert( self );

	for( drillCore_i = 0; drillCore_i < self->numDrillCores; drillCore_i++ ) {
		free( self->drillCores[drillCore_i] );
	}

	if( self->drillCores ) {
		free( self->drillCores );
	}
}

void* _Geothermal_DrillCores_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Geothermal_DrillCores);
	Type                                                      type = Geothermal_DrillCores_Type;
	Stg_Class_DeleteFunction*                              _delete = _Codelet_Delete;
	Stg_Class_PrintFunction*                                _print = _Codelet_Print;
	Stg_Class_CopyFunction*                                  _copy = _Codelet_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Geothermal_DrillCores_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Geothermal_DrillCores_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Geothermal_DrillCores_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Geothermal_DrillCores_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Codelet_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Geothermal_DrillCores_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return _Codelet_New(  CODELET_PASSARGS  );
}

Index Geothermal_DrillCores_Register( PluginsManager* pluginsManager ) {
	return PluginsManager_Submit( pluginsManager, Geothermal_DrillCores_Type, (Name)"0", _Geothermal_DrillCores_DefaultNew  );
}

void Geothermal_DrillCores_Evaluate( void* _context ) {
	UnderworldContext*	context		= (UnderworldContext*)_context;
	Geothermal_DrillCores*	self;
	unsigned		codelet_i;
	unsigned		drillCore_i;

	/* classically obfuscated StGermain way of getting the self ptr */
	for( codelet_i = 0; codelet_i < context->plugins->codelets->count; codelet_i++ ) {
		Codelet*	codelet = (Codelet*)context->plugins->codelets->data[codelet_i];
		if( !strcmp( codelet->type, Geothermal_DrillCores_Type ) ) {
			self = (Geothermal_DrillCores*)codelet;
			break;
		}
	}
	
	for( drillCore_i = 0; drillCore_i < self->numDrillCores; drillCore_i++ ) {
		FILE*		fp;
		DrillCore*	drillCore;
		double		delta_x, delta_y, delta_z;
		double		theta, phi;
		double		delta_length;
		double 		x_length, y_length, z_length;
		unsigned	step_i;
		unsigned        stepCount;
		double		origin[3], samplePoint[3], temp, hf[3], k;
		int             lCount;
                unsigned int*   lIndices;
		double*         lTemps;
		double*         lHfs0;
		double*         lHfs1;
		double*         lHfs2;
		double*         lKs;
                int*            gCounts;
                int*            gDispl;
                unsigned int*   gIndices;
		double*         gTemps;
		double*         gHfs0;
		double*         gHfs1;
		double*         gHfs2;
		double*         gKs;
		//DBG//double*         lHfs;
                //DBG//int*            gCounts3;
		//DBG//double*         gHfs;
		//DBG//int             proc_i;
		
		drillCore = self->drillCores[drillCore_i];
		delta_length = drillCore->length/((double)(drillCore->numSamples));
		stepCount = drillCore->numSamples + 1;  /* +1 is the surface */
		if( context->rank == 0 ) {
			int i;

			fp = fopen( drillCore->filename, "w" );

			gCounts = (int*)malloc( sizeof(int) * context->nproc );
			//DBG//gCounts3 = (int*)malloc( sizeof(int) * context->nproc );
			gDispl = (int*)malloc( sizeof(int) * context->nproc );
			for( i = 0; i < context->nproc; i++ ) {
				gDispl[i] = i * stepCount;
			}
			gIndices = (unsigned int*)malloc( sizeof(unsigned int) * stepCount * context->nproc );
			gTemps = (double*)malloc( sizeof(double) * stepCount * context->nproc );
			//DBG//gHfs = (double*)malloc( sizeof(double) * 3 * stepCount * context->nproc );
			gHfs0 = (double*)malloc( sizeof(double) * stepCount * context->nproc );
			gHfs1 = (double*)malloc( sizeof(double) * stepCount * context->nproc );
			gHfs2 = (double*)malloc( sizeof(double) * stepCount * context->nproc );
			gKs = (double*)malloc( sizeof(double) * stepCount * context->nproc );
		}
		else {
			fp = 0;
			gCounts = 0;
			//DBG//gCounts3 = 0;
			gDispl = 0;
			gIndices = 0;
			gTemps = 0;
			//DBG//gHfs = 0;
			gHfs0 = 0;
			gHfs1 = 0;
			gHfs2 = 0;
			gKs = 0;
		}
		lIndices = (unsigned int*)malloc( sizeof(unsigned int) * stepCount );
		lTemps = (double*)malloc( sizeof(double) * stepCount );
		//DBG//lHfs = (double*)malloc( sizeof(double) * 3 * stepCount );
		lHfs0 = (double*)malloc( sizeof(double) * stepCount );
		lHfs1 = (double*)malloc( sizeof(double) * stepCount );
		lHfs2 = (double*)malloc( sizeof(double) * stepCount );
		lKs = (double*)malloc( sizeof(double) * stepCount );

		theta 	= M_PI*drillCore->inclination/180.0; 		/* angle of inclination, in degrees */
		phi 	= M_PI*(90.0 - drillCore->azimuthal)/180.0; 	/* angle of orientation, in degrees, with north = 0 degrees
									    going clockwise */

		delta_x = delta_length*cos( phi )*cos( theta );
		delta_y = delta_length*sin( theta );
		delta_z = delta_length*sin( phi )*cos( theta );

		origin[0] = drillCore->longEastings;
		origin[2] = drillCore->latNorthings;
		origin[1] = drillCore->top;

		/* A note about this implementation...
		    If the drill core goes above the surface, "ir" will not be "LOCAL" for that point of the drillcore. The result
		    is that the points of the drill core above the surface DO NOT appear in the "local list" and consequently in
		    the output .dat file (results) */
		lCount = 0;
		for( step_i = 0; step_i < stepCount; step_i++ ) {
			InterpolationResult ir;
			InterpolationResult hfIr = LOCAL + 1; /* i.e any but LOCAL */
			InterpolationResult kIr = LOCAL + 1; /* i.e any but LOCAL */

			samplePoint[0] = origin[0] + ((double)step_i)*delta_x;
			samplePoint[1] = origin[1] - ((double)step_i)*delta_y; /* we are going down in y! */
			samplePoint[2] = origin[2] + ((double)step_i)*delta_z;
			ir = FieldVariable_InterpolateValueAt( self->temperatureField, samplePoint, &temp );
			if( self->heatFluxField ) {
				hfIr = FieldVariable_InterpolateValueAt( self->heatFluxField, samplePoint, hf );
			}
			else {
				hf[0] = 0.0f;
				hf[1] = 0.0f;
				hf[2] = 0.0f;
			}
			if( self->conductivityField ) {
				kIr = FieldVariable_InterpolateValueAt( self->conductivityField, samplePoint, &k );
			}
			else {
				k = 0.0f;
			}

			switch( ir ) {
				case LOCAL:
					lIndices[lCount] = step_i;
					lTemps[lCount] = temp;
					//DBG//lHfs[lCount*3+0] = hf[0];
					//DBG//lHfs[lCount*3+1] = hf[1];
					//DBG//lHfs[lCount*3+2] = hf[2];
					lHfs0[lCount] = hf[0];
					lHfs1[lCount] = hf[1];
					lHfs2[lCount] = hf[2];
					lKs[lCount] = k;
					lCount += 1;
					break;
				case OTHER_PROC:
				case SHADOW:
				case OUTSIDE_GLOBAL:
				default:
					break;
			}
		}
		MPI_Gather( &lCount, 1, MPI_INT, gCounts, 1, MPI_INT, 0, context->communicator );
		//DBG//for( proc_i = 0; context->rank == 0 && proc_i < context->nproc; proc_i++ ) {
		//DBG//	gCounts3[proc_i] = gCounts[proc_i] * 3;
		//DBG//}
		MPI_Gatherv( lIndices, lCount, MPI_UNSIGNED, gIndices, gCounts, gDispl, MPI_UNSIGNED, 0, context->communicator );
		MPI_Gatherv( lTemps, lCount, MPI_DOUBLE, gTemps, gCounts, gDispl, MPI_DOUBLE, 0, context->communicator );
		if( self->heatFluxField ) {
			//DBG//MPI_Gatherv( lHfs, lCount*3, MPI_DOUBLE, gHfs, gCounts3, gDispl, MPI_DOUBLE,0,context->communicator);
			MPI_Gatherv( lHfs0, lCount, MPI_DOUBLE, gHfs0, gCounts, gDispl, MPI_DOUBLE, 0, context->communicator );
			MPI_Gatherv( lHfs1, lCount, MPI_DOUBLE, gHfs1, gCounts, gDispl, MPI_DOUBLE, 0, context->communicator );
			MPI_Gatherv( lHfs2, lCount, MPI_DOUBLE, gHfs2, gCounts, gDispl, MPI_DOUBLE, 0, context->communicator );
		}
		if( self->conductivityField ) {
			MPI_Gatherv( lKs, lCount, MPI_DOUBLE, gKs, gCounts, gDispl, MPI_DOUBLE, 0, context->communicator );
		}
		if( context->rank == 0 ) {
			for( step_i = 0; step_i < stepCount; step_i++ ) {
				int rank;
				unsigned int rankIndex;

				for( rank = 0; rank < context->nproc; rank++ ) {
					for( rankIndex = 0; rankIndex < gCounts[rank]; rankIndex++ ) {
						unsigned int index = gDispl[rank] + rankIndex;

						if( step_i == gIndices[index] ) {
							samplePoint[0] = origin[0] + ((double)step_i)*delta_x;
							samplePoint[1] = origin[1] - ((double)step_i)*delta_y; /* we are going down
														   in y! */
							samplePoint[2] = origin[2] + ((double)step_i)*delta_z;
							fprintf( fp, "%.8e\t%.8e\t%.8e\t%.8e\t%u", samplePoint[0], samplePoint[1],
								samplePoint[2], gTemps[index], rank );
							if( self->heatFluxField ) {
								//DBG//fprintf( fp, "\t%.8e\t%.8e\t%.8e", gHfs[index*3+0], gHfs
								//DBG//	[index*3+1], gHfs[index*3+2] );
								fprintf( fp, "\t%.8e\t%.8e\t%.8e", gHfs0[index], gHfs1[index], 
									gHfs2[index] );
							}
							else {
								fprintf( fp, "\t-\t-\t-" );
							}
							if( self->conductivityField ) {
								fprintf( fp, "\t%.8e", gKs[index] );
							}
							else {
								fprintf( fp, "\t-" );
							}
							fprintf( fp, "\n" );
						}
					}
				}
			}

			fclose( fp );

			/* Visualisation of drill core position */
			if (drillCore->capture) {
				lucCapture* capture = (lucCapture*)drillCore->capture;
				x_length = delta_x * stepCount; 
				y_length = delta_y * stepCount;
				z_length = delta_z * stepCount;
				/* Get centre coords of drill core */
				float pos[3];
				pos[0] = drillCore->longEastings + x_length * 0.5;
				pos[2] = drillCore->latNorthings + z_length * 0.5;
				pos[1] = drillCore->top - y_length * 0.5;
				/* Get vector of drill core */
				float vector[3] = {x_length, -y_length, z_length};

				/* Create geometry */
				lucDatabase_AddVertices(capture->database, 1, lucVectorType, pos);
				lucDatabase_AddVectors(capture->database, 1, lucVectorType, 0, 0, vector);
				lucDatabase_AddRGBA(capture->database, lucVectorType, 1.0, &drillCore->colour);
			}
		}
	}
}

