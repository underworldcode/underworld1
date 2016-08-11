/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing 
** (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**   Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**   Australian Computational Earth Systems Simulator - http://www.access.edu.au
**   Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**   Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**   Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**   Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**   Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
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

#include <assert.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>
#include "types.h"
#include "IrregularQuadGenerator.h"
#include "IrregularTriGenerator.h"
#include "ConductivityMaterialManager.h"

const Type ConductivityMaterialManager_Type = (const Type)"ConductivityMaterialManager";

ConductivityMaterialManager* ConductivityMaterialManager_New( Name name ) {
    ConductivityMaterialManager* self = (ConductivityMaterialManager*) _ConductivityMaterialManager_DefaultNew( name );

    ConductivityMaterialManager_InitAll( self );
    return self;
}

ConductivityMaterialManager* _ConductivityMaterialManager_New(  CONDUCTIVITYMATERIALMANAGER_DEFARGS  )
{
    ConductivityMaterialManager* self;

    /* Allocate memory */
    assert( _sizeOfSelf >= sizeof(ConductivityMaterialManager) );
	/* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
	/* This means that any values of these parameters that are passed into this function are not passed onto the parent function
	   and so should be set to ZERO in any children of this class. */
	nameAllocationType = NON_GLOBAL;

    self = (ConductivityMaterialManager*)_Stg_Component_New(  STG_COMPONENT_PASSARGS  );

    return self;
}

void _ConductivityMaterialManager_Init( ConductivityMaterialManager* self ) {
    self->materials  = Stg_ObjectList_New();
    self->heightMaps = Stg_ObjectList_New();
}

void ConductivityMaterialManager_InitAll( void* _self ) {
    ConductivityMaterialManager* self = (ConductivityMaterialManager*)_self;
    Coord center;

    _ConductivityMaterialManager_Init( self );
    self->isConstructed = False;
}

void _ConductivityMaterialManager_Delete( void* _self ) {
    ConductivityMaterialManager* self = (ConductivityMaterialManager*)_self;

    Stg_Class_Delete( self->materials );
    Stg_Class_Delete( self->heightMaps );

    /* Delete parent */
    _Stg_Shape_Delete( self );
}

void _ConductivityMaterialManager_Print( void* _self, Stream* stream ) {
    ConductivityMaterialManager* self = (ConductivityMaterialManager*)_self;
	
    /* Print parent */
    _Stg_Shape_Print( self, stream );
}

void* _ConductivityMaterialManager_Copy( void* _self, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap ) {
    ConductivityMaterialManager*	self = (ConductivityMaterialManager*)_self;
    ConductivityMaterialManager*	newConductivityMaterialManager;
	
    newConductivityMaterialManager = (ConductivityMaterialManager*)_Stg_Shape_Copy( self, dest, deep, nameExt, ptrMap );
	
    return (void*)newConductivityMaterialManager;
}

void* _ConductivityMaterialManager_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(ConductivityMaterialManager);
	Type                                                      type = ConductivityMaterialManager_Type;
	Stg_Class_DeleteFunction*                              _delete = _ConductivityMaterialManager_Delete;
	Stg_Class_PrintFunction*                                _print = _ConductivityMaterialManager_Print;
	Stg_Class_CopyFunction*                                  _copy = _ConductivityMaterialManager_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _ConductivityMaterialManager_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _ConductivityMaterialManager_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _ConductivityMaterialManager_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _ConductivityMaterialManager_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _ConductivityMaterialManager_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _ConductivityMaterialManager_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

    return (void*) _ConductivityMaterialManager_New(  CONDUCTIVITYMATERIALMANAGER_PASSARGS  );
}

void _ConductivityMaterialManager_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
    ConductivityMaterialManager* 	self 		= (ConductivityMaterialManager*)_self;
    Dictionary*				dict;
    Dictionary_Entry_Value*		materialsList;
    Dictionary_Entry_Value*		heightMapsList;
    unsigned				listEntry_i;
    char*				entryName;
    Material*				material;
    FeVariable*				heightMap;

    _ConductivityMaterialManager_Init( self );
    Stg_Component_AssignFromXML( self, cf, data, False );

    self->context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", FiniteElementContext, False, data );
    if( !self->context  )
        self->context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", FiniteElementContext, True, data  );

    self->swarm = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"MaterialSwarm", MaterialPointsSwarm, True, data  );

    dict = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );

    materialsList = Dictionary_Get( dict, (Dictionary_Entry_Key)"Materials"  );
    heightMapsList = Dictionary_Get( dict, (Dictionary_Entry_Key)"HeightMaps"  );

    /* there must be one more material type than there are height maps, and these must be added to the lists in order
     * of descending heights... */
    if( Dictionary_Entry_Value_GetCount( materialsList ) != Dictionary_Entry_Value_GetCount( heightMapsList ) + 1 ) {
        printf( "ERROR: there must be one more material list entry than there are height maps entry\n" );
        abort();
    }

    for( listEntry_i = 0; listEntry_i < Dictionary_Entry_Value_GetCount( materialsList ); listEntry_i++ ) {
        entryName = Dictionary_Entry_Value_AsString( Dictionary_Entry_Value_GetElement( materialsList, listEntry_i ) );
        material = Stg_ComponentFactory_ConstructByName( cf, (Name)entryName, Material, True, data  );
        Stg_ObjectList_Append( self->materials, material );
    }

    for( listEntry_i = 0; listEntry_i < Dictionary_Entry_Value_GetCount( heightMapsList ); listEntry_i++ ) {
        entryName = Dictionary_Entry_Value_AsString( Dictionary_Entry_Value_GetElement( heightMapsList, listEntry_i ) );
        heightMap = Stg_ComponentFactory_ConstructByName( cf, (Name)entryName, FeVariable, True, data  );
        Stg_ObjectList_Append( self->heightMaps, heightMap );
    }

    /* for the parallel implementation of the height fields */
    self->referenceMesh = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"ReferenceMesh", FeMesh, False, data  );
}

void _ConductivityMaterialManager_Build( void* _self, void* data ) {
    ConductivityMaterialManager*	self = (ConductivityMaterialManager*)_self;
    unsigned				listEntry_i;
    
    Stg_Component_Build( self->swarm, data, False );  
    for( listEntry_i = 0; listEntry_i < Stg_ObjectList_Count( self->materials ); listEntry_i++ ) {
	Stg_Component_Build( (Material*)Stg_ObjectList_At( self->materials, listEntry_i ), data, False );
    }
    for( listEntry_i = 0; listEntry_i < Stg_ObjectList_Count( self->heightMaps ); listEntry_i++ ) {
	Stg_Component_Build( (FeVariable*)Stg_ObjectList_At( self->heightMaps, listEntry_i ), data, False );
    }

    /* set the heights here */
    if( self->referenceMesh ) {
        ConductivityMaterialManager_SetHeights_Parallel( self );
    }
    else {
        ConductivityMaterialManager_SetHeights( self );
    }
}

void _ConductivityMaterialManager_Initialise( void* _self, void* data ) {
    ConductivityMaterialManager*	self 		= (ConductivityMaterialManager*)_self;
    unsigned				listEntry_i;

    /* initialise the material before we load it onto the particles.
     * this ensures that the particles which exist in the elements for which the shape has been
     * assigned are set to the relevant material index before we go an mess with them in the 
     * _Load() function. 
     * if we set the initail shape to be NOWHERE, then we can specify which particles we wish to 
     * to have the relevant material property in the _Load() function instead...
     */
    Stg_Component_Initialise( self->swarm, data, False );  
    for( listEntry_i = 0; listEntry_i < Stg_ObjectList_Count( self->materials ); listEntry_i++ ) {
	Stg_Component_Initialise( (Material*)Stg_ObjectList_At( self->materials, listEntry_i ), data, False );
    }
    for( listEntry_i = 0; listEntry_i < Stg_ObjectList_Count( self->heightMaps ); listEntry_i++ ) {
	Stg_Component_Initialise( (FeVariable*)Stg_ObjectList_At( self->heightMaps, listEntry_i ), data, False );
    }

    /* here's where we assign the materials */
    ConductivityMaterialManager_AssignMaterials( self );
}

void _ConductivityMaterialManager_Execute( void* _self, void* data ) {
    ConductivityMaterialManager*	self = (ConductivityMaterialManager*)_self;
}

void _ConductivityMaterialManager_Destroy( void* _self, void* data ) {
    ConductivityMaterialManager*	self = (ConductivityMaterialManager*)_self;
}

void ConductivityMaterialManager_SetHeights( ConductivityMaterialManager* cmm ) {
    unsigned			field_i;
    char*			field_fn;
    char*			filename;
    File*			file;
    Dictionary*			dict;
    Dictionary_Entry_Value*	fileNamesList;
    FeVariable*			heightField;
    double			height;
    int				dummyInt[3];
    double			dummyDouble[2];
    unsigned			numNodeVerts;
    unsigned			numEdgeVerts;
    unsigned			numTriVerts;
    unsigned			vert_i;
    double			vertHeights[3];
    double			dist0sq, dist1sq, edgeLenSq;
    double			*vert_0, *vert_1, *vertCentre;
    int				vertInds[3];
    double			triCentre[2];

    dict = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cmm->context->CF->componentDict, (Dictionary_Entry_Key)cmm->name )  );
    fileNamesList = Dictionary_Get( dict, (Dictionary_Entry_Key)"HeightMapFilePrefixes" );

    if( Dictionary_Entry_Value_GetCount( fileNamesList ) != Stg_ObjectList_Count( cmm->heightMaps ) ) {
        printf( "ERROR: number of file names not equal to the number of height maps\n" );
        abort();
    }

    for( field_i = 0; field_i < Stg_ObjectList_Count( cmm->heightMaps ); field_i++ ) {
        heightField = (FeVariable* )Stg_ObjectList_At( cmm->heightMaps, field_i );
        field_fn = Dictionary_Entry_Value_AsString( Dictionary_Entry_Value_GetElement( fileNamesList, field_i ) );

        /* set the heights for the delaunay nodes directly from the triangle file... */
        filename = Memory_Alloc_Array( char, strlen( field_fn ) + 6 /* .node */, "heightmap.node" );
        sprintf( filename, "%s.node", field_fn );
        file = CFile_NewRead( filename );
	Journal_Firewall(
		(long)file,
		Journal_Register( ConductivityMaterialManager_Type, Error_Type ),
		"File \"%s\" not found!\n",
		filename );
	fscanf( CFile_Ptr( file ), "%u  %d  %d  %d", &numNodeVerts, dummyInt, dummyInt+1, dummyInt+2 );
	for( vert_i = 0; vert_i < numNodeVerts; vert_i++ ) {
		fscanf( CFile_Ptr( file ), "%d %lf %lf %lf %d", dummyInt, dummyDouble, dummyDouble+1, &height, dummyInt+1 );
		FeVariable_SetValueAtNode( heightField, vert_i, &height );
	}
        Stg_Class_Delete( file );
        Memory_Free( filename );

	/* only need to interpolate additional nodes for the irregular quad mesh */
	if( !strcmp( heightField->feMesh->generator->type, IrregularTriGenerator_Type ) )
		continue;
		
        /* now interpolate the edge centre nodes */
        filename = Memory_Alloc_Array( char, strlen( field_fn ) + 6 /* .edge */, "heightmap.edge" );
	sprintf( filename, "%s.edge", field_fn );
	file = CFile_NewRead( filename );
	Journal_Firewall(
		(long)file,
		Journal_Register( ConductivityMaterialManager_Type, Error_Type ),
		"File \"%s\" not found!\n",
		filename );
	fscanf( CFile_Ptr( file ), "%d  %d", &numEdgeVerts, dummyInt );
	for( vert_i = 0; vert_i < numEdgeVerts; vert_i++ ) {
		fscanf( CFile_Ptr( file ), "%d  %d  %d  %d", dummyInt, vertInds, vertInds+1, dummyInt+1 );

		FeVariable_GetValueAtNode( heightField, vertInds[0], vertHeights+0 );
		FeVariable_GetValueAtNode( heightField, vertInds[1], vertHeights+1 );

		vert_0 = Mesh_GetVertex( heightField->feMesh, vertInds[0] );
		vert_1 = Mesh_GetVertex( heightField->feMesh, vertInds[1] );
		vertCentre = Mesh_GetVertex( heightField->feMesh, numNodeVerts+vert_i );

		dist0sq = (vert_0[0] - vertCentre[0])*(vert_0[0] - vertCentre[0]) + 
			  (vert_0[1] - vertCentre[1])*(vert_0[1] - vertCentre[1]);
		dist1sq = (vert_1[0] - vertCentre[0])*(vert_1[0] - vertCentre[0]) + 
			  (vert_1[1] - vertCentre[1])*(vert_1[1] - vertCentre[1]);
		edgeLenSq = (vert_0[0] - vert_1[0])*(vert_0[0] - vert_1[0]) + 
			    (vert_0[1] - vert_1[1])*(vert_0[1] - vert_1[1]);

		height = (1.0/sqrt(edgeLenSq))*(sqrt(dist1sq)*vertHeights[0] + sqrt(dist0sq)*vertHeights[1]);
		FeVariable_SetValueAtNode( heightField, numNodeVerts+vert_i, &height );
	}
	Stg_Class_Delete( file );
	Memory_Free( filename );

	/* now interpolate the triangle centre nodes */
	filename = Memory_Alloc_Array( char, strlen( field_fn ) + 5 /* .ele */, "heightmap.ele" );
	sprintf( filename, "%s.ele", field_fn );
	file = CFile_NewRead( filename );
	Journal_Firewall(
		(long)file,
		Journal_Register( ConductivityMaterialManager_Type, Error_Type ),
		"File \"%s\" not found!\n",
		filename );
	fscanf( CFile_Ptr( file ), "%d  %d  %d", &numTriVerts, dummyInt, dummyInt+1 );
	triCentre[0] = triCentre[1] = 0.33333333333333333333333333333333333333333333333;
	for( vert_i = 0; vert_i < numTriVerts; vert_i++ ) {
		fscanf( CFile_Ptr( file ), "%d  %d  %d  %d", dummyInt, vertInds, vertInds+1, vertInds+2 );
		
		FeVariable_GetValueAtNode( heightField, vertInds[0], vertHeights+0 );
		FeVariable_GetValueAtNode( heightField, vertInds[1], vertHeights+1 );
		FeVariable_GetValueAtNode( heightField, vertInds[2], vertHeights+2 );
		
		height = IrregularQuadGenerator_InterpFromLocalCoord_Tri( vertHeights, triCentre );
		FeVariable_SetValueAtNode( heightField, numNodeVerts+numEdgeVerts+vert_i, &height );
	}
	Stg_Class_Delete( file );
	Memory_Free( filename );
    }
}

void ConductivityMaterialManager_AssignMaterials( ConductivityMaterialManager* cmm ) {
    unsigned 			el_i;
    unsigned 			cell_i;
    unsigned			part_i;
    MaterialPoint*		matPoint;
    int				layer_i;
    double			layerHeight;
    double			layerCoord[2];
    Material*			material;
    Grid*			elGrid;
    unsigned			sizes[2];
    double			min3d[3], max3d[3];
    double			min2d[2], max2d[2];
    CartesianGenerator* 	gen2d;
    int				maxDecomp[2] = { 0, 0 };
    FeMesh*			mesh2d;
    FeVariable**		regHeightMaps;
    Variable*			regHeightVar;
    DofLayout*			regHeightDofs;
    static double*		arrayPtr;
    unsigned			numVerts2d;
    unsigned			vert_i;
    double			height;
    FeVariable*			basement;
    FeMesh*			irregQuadMesh;
    unsigned			element;
    unsigned			nearestVertex;
    ElementType*		elType;
    double			lCoord[2];
    double			basementMaxDepth;
    FeVariable*			feVar;

    /* store the fields on a single regular mesh */
    elGrid = *(Grid**)ExtensionManager_Get( ((ElementCellLayout*)cmm->swarm->cellLayout)->mesh->info, ((ElementCellLayout*)cmm->swarm->cellLayout)->mesh, ExtensionManager_GetHandle( ((ElementCellLayout*)cmm->swarm->cellLayout)->mesh->info, (Name)"elementGrid" )  );
    sizes[0] = elGrid->sizes[I_AXIS];
    sizes[1] = elGrid->sizes[K_AXIS];
    Mesh_GetGlobalCoordRange( ((ElementCellLayout*)cmm->swarm->cellLayout)->mesh , min3d, max3d );
    min2d[0] = min3d[I_AXIS];
    min2d[1] = min3d[K_AXIS];
    max2d[0] = max3d[I_AXIS];
    max2d[1] = max3d[K_AXIS];
    gen2d = CartesianGenerator_New( "gen2d", NULL );
    if( cmm->referenceMesh ) {
        MeshGenerator_SetMPIComm( gen2d, MPI_COMM_WORLD );
    }
    else {
        /* we want this mesh to be the same on every proc (its only 2d, so should be fine...) */
        MeshGenerator_SetMPIComm( gen2d, MPI_COMM_SELF );
    }
    CartesianGenerator_SetDimSize( gen2d, 2 );
    CartesianGenerator_SetTopologyParams( gen2d, sizes, 0, NULL, maxDecomp );
    CartesianGenerator_SetGeometryParams( gen2d, min2d, max2d );
    CartesianGenerator_SetShadowDepth( gen2d, 0 );
    mesh2d = FeMesh_New( "mesh2d", NULL );
    Mesh_SetGenerator( mesh2d, gen2d );
    FeMesh_SetElementFamily( mesh2d, "linear" );
    mesh2d->useFeAlgorithms = False;
    Stg_Component_Build( mesh2d, NULL, False );
   
    printf( "interpolating irregular height fields to regular ones for %d fields...\n", Stg_ObjectList_Count( cmm->heightMaps ) );
 
    numVerts2d = Mesh_GetDomainSize( mesh2d, MT_VERTEX );
    regHeightMaps = Memory_Alloc_Array( FeVariable*, Stg_ObjectList_Count( cmm->heightMaps ), "regularHeightMaps" );
    for( layer_i = 0; layer_i < Stg_ObjectList_Count( cmm->heightMaps ); layer_i++ ) {
        arrayPtr = Memory_Alloc_Array( double, numVerts2d, "arrayPointer" );
        regHeightVar = Variable_NewScalar( "", (AbstractContext*)cmm->context, Variable_DataType_Double, (Index*)&numVerts2d, NULL, (void**)&arrayPtr, cmm->context->variable_Register  );
        regHeightDofs = DofLayout_New( "", (AbstractContext*)cmm->context, cmm->context->variable_Register, numVerts2d, mesh2d );
        DofLayout_AddAllFromVariableArray( regHeightDofs, 1, &regHeightVar );
        Stg_Component_Build( regHeightDofs, NULL, False );
        Stg_Component_Initialise( regHeightDofs, NULL, False );
        regHeightMaps[layer_i] = FeVariable_New( "", NULL, mesh2d, regHeightDofs, NULL, NULL, NULL, 2, True, False, False, 
                                                 cmm->context->fieldVariable_Register );

        /* if we are doing this in parallel, then we need to make sure the vertex is with a local element. otherwise
         * just assign the value at the closest vertex */
	/* NOTE: this seems to be giving different results in serial for the particle assignment, so disable for now... */
/*
        if( cmm->referenceMesh ) {
            irregQuadMesh = ((FeVariable*)Stg_ObjectList_At( cmm->heightMaps, layer_i ))->feMesh;
            for( vert_i = 0; vert_i < numVerts2d; vert_i++ ) {
                if( !Mesh_SearchElements( irregQuadMesh, mesh2d->verts[vert_i], &element ) ) {
                    nearestVertex = Mesh_NearestVertex( irregQuadMesh, mesh2d->verts[vert_i] );
		    FeVariable_GetValueAtNode( Stg_ObjectList_At( cmm->heightMaps, layer_i ), nearestVertex, &height );
		}
                else {
                    elType = FeMesh_GetElementType( irregQuadMesh, element );
                    ElementType_ConvertGlobalCoordToElLocal( elType, irregQuadMesh, element, mesh2d->verts[vert_i], lCoord );
                    FeVariable_InterpolateWithinElement( Stg_ObjectList_At( cmm->heightMaps, layer_i ), element, lCoord, &height );
                }
                FeVariable_SetValueAtNode( regHeightMaps[layer_i], vert_i, &height );
            }
        }
	else {
*/
            feVar = (FeVariable*)Stg_ObjectList_At( cmm->heightMaps, layer_i );
            if( !strcmp( feVar->feMesh->generator->type, IrregularTriGenerator_Type ) ) {
                for( vert_i = 0; vert_i < numVerts2d; vert_i++ ) {
                    height = IrregularTriGenerator_InterpolateValueAt( feVar->feMesh->generator, feVar, mesh2d->verts[vert_i] );
                    FeVariable_SetValueAtNode( regHeightMaps[layer_i], vert_i, &height );
                }
            }
            else {
                for( vert_i = 0; vert_i < numVerts2d; vert_i++ ) {
                    FieldVariable_InterpolateValueAt( Stg_ObjectList_At( cmm->heightMaps, layer_i ), mesh2d->verts[vert_i], &height );
                    FeVariable_SetValueAtNode( regHeightMaps[layer_i], vert_i, &height );
                }
            }
//	}
        printf( "\t...done for field %d\n", layer_i );
    }

    for( el_i = 0; el_i < Mesh_GetLocalSize( ((ElementCellLayout*)cmm->swarm->cellLayout)->mesh, MT_VOLUME ); el_i++ ) {
        cell_i = CellLayout_MapElementIdToCellId( cmm->swarm->cellLayout, el_i );
	for( part_i = 0; part_i < cmm->swarm->cellParticleCountTbl[cell_i]; part_i++ ) {
            matPoint = (MaterialPoint*) Swarm_ParticleInCellAt( cmm->swarm, cell_i, part_i );

            /* now find which layers the particle lies between... assumes material layers are
             * ordered by descending values of height */
            layerCoord[0] = matPoint->coord[I_AXIS];
            layerCoord[1] = matPoint->coord[K_AXIS];
            for( layer_i = 0; layer_i < Stg_ObjectList_Count( cmm->heightMaps ) - 1; layer_i++ ) {
                FieldVariable_InterpolateValueAt( regHeightMaps[layer_i], layerCoord, &layerHeight );
                if( matPoint->coord[J_AXIS] > layerHeight )
                    break;
            }

            /* now assign the material value for this layer... NOTE: there must be one more material than
             * there are height maps */
            material = (Material*)Stg_ObjectList_At( cmm->materials, layer_i );
            matPoint->materialIndex = material->index;
        }
    }

    /* now assign the basement... everything underneath this layer to be reassigned as such */
    basement = regHeightMaps[Stg_ObjectList_Count( cmm->heightMaps ) - 1];
    material = (Material*)Stg_ObjectList_At( cmm->materials, Stg_ObjectList_Count( cmm->materials ) - 1 );
    basementMaxDepth = _FeVariable_GetMinGlobalFieldMagnitude( basement );
    for( el_i = 0; el_i < Mesh_GetLocalSize( ((ElementCellLayout*)cmm->swarm->cellLayout)->mesh, MT_VOLUME ); el_i++ ) {
        cell_i = CellLayout_MapElementIdToCellId( cmm->swarm->cellLayout, el_i );
	for( part_i = 0; part_i < cmm->swarm->cellParticleCountTbl[cell_i]; part_i++ ) {
            matPoint = (MaterialPoint*) Swarm_ParticleInCellAt( cmm->swarm, cell_i, part_i );

            if( matPoint->coord[J_AXIS] < basementMaxDepth ) {
                matPoint->materialIndex = material->index;
                continue;
            }

            layerCoord[0] = matPoint->coord[I_AXIS];
            layerCoord[1] = matPoint->coord[K_AXIS];
            FieldVariable_InterpolateValueAt( basement, layerCoord, &layerHeight );
            if( matPoint->coord[J_AXIS] < layerHeight )
                matPoint->materialIndex = material->index;
        }
    }

    for( layer_i = 0; layer_i < Stg_ObjectList_Count( cmm->heightMaps ); layer_i++ ) {
	Stg_ObjectList_Remove( cmm->context->fieldVariable_Register->objects, regHeightMaps[layer_i]->name, DELETE );
        Stg_Component_Destroy( regHeightMaps[layer_i], NULL, False );
    }
    Memory_Free( regHeightMaps );

    MPI_Barrier( MPI_COMM_WORLD );
}

void ConductivityMaterialManager_SetHeights_Parallel( ConductivityMaterialManager* cmm ) {
    unsigned			field_i;
    char*			field_fn;
    char*			filename;
    File*			file;
    Dictionary*			dict;
    Dictionary_Entry_Value*	fileNamesList;
    FeVariable*			heightField;
    double			height;
    int				dummyInt[3];
    double			dummyDouble[2];
    unsigned			numNodeVerts;
    unsigned			numEdgeVerts;
    unsigned			numTriVerts;
    unsigned			vert_i;
    double			vertHeights[3];
    double			dist0sq, dist1sq, edgeLenSq;
    double			*vert_0, *vert_1, *vertCentre;
    int				vertInds[3];
    double			triCentre[2];
    double			min[3], max[3];
    unsigned			localNode_i;
    int*			nodeMap;
    unsigned			numLocalNodes, numLocalEdges;

    Mesh_GetDomainCoordRange( cmm->referenceMesh, min, max );

    dict = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cmm->context->CF->componentDict, (Dictionary_Entry_Key)cmm->name )  );
    fileNamesList = Dictionary_Get( dict, (Dictionary_Entry_Key)"HeightMapFilePrefixes" );

    if( Dictionary_Entry_Value_GetCount( fileNamesList ) != Stg_ObjectList_Count( cmm->heightMaps ) ) {
        printf( "ERROR: number of file names not equal to the number of height maps\n" );
        abort();
    }

int rank;
MPI_Comm_rank(MPI_COMM_WORLD,&rank);
printf("%d: parallel material layer height assignment routine\n", rank);

    for( field_i = 0; field_i < Stg_ObjectList_Count( cmm->heightMaps ); field_i++ ) {
        heightField = (FeVariable* )Stg_ObjectList_At( cmm->heightMaps, field_i );
        field_fn = Dictionary_Entry_Value_AsString( Dictionary_Entry_Value_GetElement( fileNamesList, field_i ) );

        /* set the heights for the delaunay nodes directly from the triangle file... */
        filename = Memory_Alloc_Array( char, strlen( field_fn ) + 6 /* .node */, "heightmap.node" );
        sprintf( filename, "%s.node", field_fn );
        file = CFile_NewRead( filename );
	Journal_Firewall(
		(long)file,
		Journal_Register( ConductivityMaterialManager_Type, Error_Type ),
		"File \"%s\" not found!\n",
		filename );
	fscanf( CFile_Ptr( file ), "%u  %d  %d  %d", &numNodeVerts, dummyInt, dummyInt+1, dummyInt+2 );
	nodeMap = Memory_Alloc_Array( int, numNodeVerts, "global to local node map" );
	memset( nodeMap, -1, numNodeVerts * sizeof( int ) );
	localNode_i = 0;
	for( vert_i = 0; vert_i < numNodeVerts; vert_i++ ) {
            fscanf( CFile_Ptr( file ), "%d %lf %lf %lf %d", dummyInt, dummyDouble, dummyDouble+1, &height, dummyInt+1 );
            if( dummyDouble[0] < min[I_AXIS] || dummyDouble[0] > max[I_AXIS] || 
                dummyDouble[1] < min[K_AXIS] || dummyDouble[1] > max[K_AXIS] ) {
                continue;
            }

	    nodeMap[vert_i] = localNode_i;
            FeVariable_SetValueAtNode( heightField, localNode_i++, &height );
	}
	numLocalNodes = localNode_i;
        Stg_Class_Delete( file );
        Memory_Free( filename );
	
        /* now interpolate the edge centre nodes */
        filename = Memory_Alloc_Array( char, strlen( field_fn ) + 6 /* .edge */, "heightmap.edge" );
	sprintf( filename, "%s.edge", field_fn );
	file = CFile_NewRead( filename );
	Journal_Firewall(
		(long)file,
		Journal_Register( ConductivityMaterialManager_Type, Error_Type ),
		"File \"%s\" not found!\n",
		filename );
	fscanf( CFile_Ptr( file ), "%d  %d", &numEdgeVerts, dummyInt );
	localNode_i = 0;
	numLocalEdges = 0;
	for( vert_i = 0; vert_i < numEdgeVerts; vert_i++ ) {
            fscanf( CFile_Ptr( file ), "%d  %d  %d  %d", dummyInt, vertInds, vertInds+1, dummyInt+1 );
            if( nodeMap[vertInds[0]] == -1 || nodeMap[vertInds[1]] == -1 )
                continue;

	    vertInds[0] = nodeMap[vertInds[0]];
	    vertInds[1] = nodeMap[vertInds[1]];
            FeVariable_GetValueAtNode( heightField, vertInds[0], vertHeights+0 );
            FeVariable_GetValueAtNode( heightField, vertInds[1], vertHeights+1 );

            vert_0 = Mesh_GetVertex( heightField->feMesh, vertInds[0] );
            vert_1 = Mesh_GetVertex( heightField->feMesh, vertInds[1] );
            vertCentre = Mesh_GetVertex( heightField->feMesh, numLocalNodes+localNode_i );

            dist0sq = (vert_0[0] - vertCentre[0])*(vert_0[0] - vertCentre[0]) + (vert_0[1] - vertCentre[1])*(vert_0[1] - vertCentre[1]);
            dist1sq = (vert_1[0] - vertCentre[0])*(vert_1[0] - vertCentre[0]) + (vert_1[1] - vertCentre[1])*(vert_1[1] - vertCentre[1]);
            edgeLenSq = (vert_0[0] - vert_1[0])*(vert_0[0] - vert_1[0]) + (vert_0[1] - vert_1[1])*(vert_0[1] - vert_1[1]);

            height = (1.0/sqrt(edgeLenSq))*(sqrt(dist1sq)*vertHeights[0] + sqrt(dist0sq)*vertHeights[1]);
            FeVariable_SetValueAtNode( heightField, numLocalNodes+localNode_i, &height );
	    localNode_i++;
	}
	numLocalEdges = localNode_i;
	Stg_Class_Delete( file );
	Memory_Free( filename );

	/* now interpolate the triangle centre nodes */
	filename = Memory_Alloc_Array( char, strlen( field_fn ) + 5 /* .ele */, "heightmap.ele" );
	sprintf( filename, "%s.ele", field_fn );
	file = CFile_NewRead( filename );
	Journal_Firewall(
		(long)file,
		Journal_Register( ConductivityMaterialManager_Type, Error_Type ),
		"File \"%s\" not found!\n",
		filename );
	fscanf( CFile_Ptr( file ), "%d  %d  %d", &numTriVerts, dummyInt, dummyInt+1 );
	triCentre[0] = triCentre[1] = 0.33333333333333333333333333333333333333333333333;
	localNode_i = 0;
	for( vert_i = 0; vert_i < numTriVerts; vert_i++ ) {
            fscanf( CFile_Ptr( file ), "%d  %d  %d  %d", dummyInt, vertInds, vertInds+1, vertInds+2 );
	    if( nodeMap[vertInds[0]] == -1 || nodeMap[vertInds[1]] == -1 || nodeMap[vertInds[2]] == -1 )
                continue;
		
	    vertInds[0] = nodeMap[vertInds[0]];
	    vertInds[1] = nodeMap[vertInds[1]];
	    vertInds[2] = nodeMap[vertInds[2]];
            FeVariable_GetValueAtNode( heightField, vertInds[0], vertHeights+0 );
            FeVariable_GetValueAtNode( heightField, vertInds[1], vertHeights+1 );
            FeVariable_GetValueAtNode( heightField, vertInds[2], vertHeights+2 );
		
            height = IrregularQuadGenerator_InterpFromLocalCoord_Tri( vertHeights, triCentre );
            FeVariable_SetValueAtNode( heightField, numLocalNodes+numLocalEdges+localNode_i, &height );
	    localNode_i++;
	}
	Stg_Class_Delete( file );
	Memory_Free( filename );
	Memory_Free( nodeMap );
    }
}

