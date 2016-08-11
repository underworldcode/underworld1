#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "PpcFeVariable.h"
#include "PpcManager.h"


/* Textual name of this class */
const Type PpcManager_Type = "PpcManager";

void _PpcManager_Init( void* _self, PICelleratorContext* context, Stg_ComponentFactory* cf, MaterialPointsSwarm* ms, IntegrationPointsSwarm* is, Materials_Register* mr, FeMesh* m, double g, Scaling* sc )
{
   PpcManager* self = (PpcManager*) _self;
   Bool storeAnyPrevious = False;
   Bool storeAnyOnParticles = False;
   Bool storeAnyOnMesh = False;
   Index tag_I;

   self->info_stream = Journal_Register( Info_Type, self->type );
   self->error_stream = Journal_Register( Error_Type, self->type );

   self->context = context;
   self->cf = cf;
   self->integrationSwarm = is;
   self->materialSwarm = ms;
   self->materials_Register = mr;
   self->mesh = m;
   self->scaling = sc;
   self->gravity[0] = 0;
   self->gravity[1] = 0;
   self->gravity[2] = 0;
   self->gravity[context->verticalAxis] = g;

   /* choose the appropriate 'Get' function */
   if( is->mapper && Stg_Class_IsInstance( is->mapper, OneToOneMapper_Type ) ) {
      self->_get = _PpcManager_GetWithOneToOneMap;
   } else {
      self->_get = _PpcManager_GetWithOneToManyMap;
   }

   for ( tag_I = 0; tag_I < self->tagsCount; tag_I++ )
   {
      if ( self->tagList[tag_I].storePreviousValue )
      {
         self->tagList[tag_I].storeOnParticles = True;
         storeAnyPrevious = True;
      }
      if ( self->tagList[tag_I].storeOnParticles )
         storeAnyOnParticles = True;
      if ( self->tagList[tag_I].storeOnMesh )
         storeAnyOnMesh = True;
   }
   /* Sanity checks */
   if ( storeAnyPrevious )
      Journal_Firewall( self->materialSwarm != NULL, self->error_stream, "\n\n\n"
                        "Error in func %s: if you want to store previous values you must provide an material swarm.\n"
                        "Please add\n\t <param name=""MaterialSwarm""> yourSwarmName  </param>\n to the PpcManager component "
                        "\n\n\n", __func__ );
   if ( storeAnyOnParticles )
      Journal_Firewall( self->materialSwarm != NULL, self->error_stream, "\n\n\n"
                        "Error in func %s: if you want to store values on the particles you must provide an material swarm.\n"
                        "Please add\n\t <param name=""MaterialSwarm""> yourSwarmName  </param>\n to the PpcManager component "
                        "\n\n\n", __func__ );
   if ( storeAnyOnMesh )
   {
      Journal_Firewall( self->integrationSwarm != NULL, self->error_stream, "\n\n\n"
                        "Error in func %s: if you want to store values on the mesh you must provide an integration swarm.\n"
                        "Please add\n\t <param name=""IntegrationSwarm""> yourSwarmName  </param>\n to the PpcManager component "
                        "\n\n\n", __func__ );
      Journal_Firewall( self->mesh != NULL, self->error_stream, "\n\n\n"
                        "Error in func %s: if you want to store values on the mesh you must provide the mesh component.\n"
                        "Please add\n\t <param name=""Mesh""> yourMeshName  </param>\n to the PpcManager component "
                        "\n\n\n", __func__ );
   }
}

/** Public constructor */
PpcManager* PpcManager_New( 
      Name                    name,
      PICelleratorContext*    context,
      Stg_ComponentFactory*   cf, 
      MaterialPointsSwarm*    ms, 
      IntegrationPointsSwarm* is,  
      Materials_Register*     mr, 
      FeMesh*                 m,
      double                  g, 
      Scaling*                sc ) {

   PpcManager* self = (PpcManager*) _PpcManager_DefaultNew( name );
   assert( self );

   /* construct the thing */
   _PpcManager_Init( self, context, cf, ms, is, mr, m, HUGE_VAL, sc );
   self->isConstructed = True;

   return self;
}

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
PpcManager* _PpcManager_New(  PPCMANAGER_DEFARGS  )
{
   PpcManager* self;
   assert( _sizeOfSelf >= sizeof(PpcManager) );
   nameAllocationType = NON_GLOBAL;
   self = (PpcManager*) _Stg_Component_New(  STG_COMPONENT_PASSARGS  );
   return self;
}


void* _PpcManager_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof(PpcManager);
   Type                                                      type = PpcManager_Type;
   Stg_Class_DeleteFunction*                              _delete = _PpcManager_Delete;
   Stg_Class_PrintFunction*                                _print = _PpcManager_Print;
   Stg_Class_CopyFunction*                                  _copy = NULL;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _PpcManager_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _PpcManager_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _PpcManager_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _PpcManager_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _PpcManager_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _PpcManager_Destroy;

   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;
   return (void*)_PpcManager_New(  PPCMANAGER_PASSARGS  );
}

void _PpcManager_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data )
{
   PpcManager* self = (PpcManager*)_self;
   Dictionary*	theDictionary = NULL;
   PICelleratorContext* context = NULL;
   IntegrationPointsSwarm* is = NULL;
   MaterialPointsSwarm* ms = NULL;
   Materials_Register* mr = NULL;
   Scaling* sc = NULL;
   FeMesh* m = NULL;
   double g;
   Dictionary_Entry_Value*	tagList = NULL;
   Dictionary_Entry_Value*	tagEntry = NULL;
   Index tag_I;

   /* The dictionary */
   theDictionary = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name ) );

   /* The context */
   context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", PICelleratorContext, False, data );
   if ( !context )
      context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", PICelleratorContext, True, data  );

   /* The materials register */
   mr = context->materials_Register;
   assert( mr );

   /* Get Gravity, check local xml than global dictionary */
   g = Stg_ComponentFactory_GetDouble( cf, self->name, "Gravity", HUGE_VAL  );
   if( g == HUGE_VAL ) g = Dictionary_GetDouble_WithScopeDefault( cf->rootDict, (Dictionary_Entry_Key)"Gravity", HUGE_VAL );
   
   if( g == HUGE_VAL ) {
      Journal_RPrintf( context->input_verbose_stream,
            "\n\n"
            "******************************************************\n"
            "WARNING - Gravity is not defined, if its value\n"
            "is required there will be problems. To define it put\n"
            "  <param name=\"Gravity\" units=\"m/s^2\">9.8</param>\n"
            "in you input file.\n"
            "******************************************************\n" );
   }

   /* The mesh */
   m = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Mesh", FeMesh, False, data );
   if ( !m )
      m = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_linearMesh", FeMesh, True, data  );

   /* The scaling component */
   sc = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Scaling", Scaling, False, data );
   if ( !sc )
      sc = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_scaling", Scaling, True, data  );

   /* The integration swarm */
   is = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"IntegrationSwarm", IntegrationPointsSwarm, False, data );
   if ( !is )
      is = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_picIntegrationPoints", IntegrationPointsSwarm, True, data  );

   /* The material swarm */
   ms = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"MaterialSwarm", MaterialPointsSwarm, False, data );
   if ( !ms )
      ms = Stg_ComponentFactory_ConstructByName( cf, (Name)"default_materialSwarm", MaterialPointsSwarm, False, data  );


   /* Read the tag list */
   tagList = Dictionary_Get( theDictionary, (Dictionary_Entry_Key)"Properties" );
   //TODO: assert( tagList );
   self->tagsCount = tagList ? Dictionary_Entry_Value_GetCount( tagList ) : 0;
   self->tagList = Memory_Alloc_Array( PpcTag, self->tagsCount, "PpcManager_tagList" );

   for ( tag_I = 0; tag_I < self->tagsCount; tag_I++ )
   {
      tagEntry = Dictionary_Entry_Value_GetElement( tagList, tag_I );
      self->tagList[tag_I].name=StG_Strdup( Dictionary_Entry_Value_AsString( Dictionary_Entry_Value_GetMember( tagEntry, (Dictionary_Entry_Key)"PropertyName") ) );
      self->tagList[tag_I].storeOnParticles = Dictionary_Entry_Value_AsBool( Dictionary_Entry_Value_GetMember( tagEntry, (Dictionary_Entry_Key)"StoreOnParticles") );
      self->tagList[tag_I].storeNLOnParticles = Dictionary_Entry_Value_AsBool( Dictionary_Entry_Value_GetMember( tagEntry, (Dictionary_Entry_Key)"StoreNLOnParticles") );
      self->tagList[tag_I].storeOnMesh = Dictionary_Entry_Value_AsBool( Dictionary_Entry_Value_GetMember( tagEntry, (Dictionary_Entry_Key)"StoreOnMesh") );
      self->tagList[tag_I].lastStoredTimestep = (unsigned) -1;
      self->tagList[tag_I].materialExtHandle = (unsigned) -1;
      self->tagList[tag_I].particleExtHandle = (unsigned) -1;
      /* we initialise solePpc here, this is important for the build phase */
      self->tagList[tag_I].solePpc = NULL;
      self->tagList[tag_I].ppcFeVar = NULL;
      /* storage of the previous value */
      self->tagList[tag_I].storePreviousValue = Dictionary_Entry_Value_AsBool( Dictionary_Entry_Value_GetMember( tagEntry, (Dictionary_Entry_Key)"StorePreviousValue") );
      self->tagList[tag_I].previousParticleExtHandle = (unsigned) -1;
      self->tagList[tag_I].passiveProperty = Dictionary_Entry_Value_AsBool( Dictionary_Entry_Value_GetMember( tagEntry, (Dictionary_Entry_Key)"PassiveProperty") );
      // initialise min & max
      self->tagList[tag_I].min = HUGE_VAL;
      self->tagList[tag_I].max = -1 * HUGE_VAL;
   }

   _PpcManager_Init( self, context, cf, ms, is, mr, m, g, sc );
}


void _PpcManager_Build( void* _self, void* data )
{
   PpcManager* self = (PpcManager*) _self;
   MaterialPointsSwarm* ms=self->materialSwarm;
   SwarmVariable* materialPointsSwarmVariable = NULL;
   PpcManager_MaterialExt* materialExt;
   PpcManager_ParticleExt* particleExt;
   StandardParticle particle;
   Material_Index material_I;
   Material* material = NULL;
   Index numMaterials = 0;
   char* configName = NULL;
   char* newName = NULL;
   Bool storeAnyPrevious = False;
   Bool anyPassiveProperty = False;
   int tag_I;

   /* Add configuration to the materials */
   Journal_Printf( self->info_stream, "Linking materials and ppc... \n" );

   /* make sure things are built */
   Stg_Component_Build( self->scaling, NULL, False );
   Stg_Component_Build( self->mesh, NULL, False );
   Stg_Component_Build( self->integrationSwarm, NULL, False );
   if( ms )
      Stg_Component_Build( ms, NULL, False );

   /* Extend the materials */
   for ( tag_I = 0 ; tag_I < self->tagsCount ; tag_I++ )
   {
      /* if this tag isn't relevant to materials skip this loop, but build if required */
      if ( self->tagList[tag_I].solePpc != NULL  )
      {
         Stg_Component_Build( self->tagList[tag_I].solePpc, NULL, False );
         continue;
      }

      self->tagList[tag_I].materialExtHandle = Materials_Register_AddMaterialExtension( self->materials_Register, self->type, sizeof(PpcManager_MaterialExt) );

      /* Read the material extensions */
      numMaterials = Materials_Register_GetCount( self->materials_Register );

      for ( material_I = 0; material_I < numMaterials; material_I++ )
      {
         material = Materials_Register_GetByIndex( self->materials_Register, material_I );
         /* Extend the material */
         materialExt = ExtensionManager_GetFunc( material->extensionMgr, material, self->tagList[tag_I].materialExtHandle );

         Journal_Printf( self->info_stream, "\nPhysical properties for material %s:\n", material->name );

         configName = Stg_ComponentFactory_GetString( self->cf, material->name, (Dictionary_Entry_Key)self->tagList[tag_I].name, NULL );

         /* if no string can be found, the ppc is not searched for*/
         if ( !configName ) materialExt->ppc = NULL;
         else
         {
            if ( Stg_StringIsNumeric( configName ) )
            {
               /* if the string is numeric, then we must create a Ppc_Constant from scrach
                * using the string "value" */
               double value = strtod( configName, 0 );
               char* ppcNameNew;
               Stg_asprintf( &ppcNameNew, "%s-%s", material->name, self->tagList[tag_I].name );
               materialExt->ppc = (Ppc*)Ppc_Constant_New( ppcNameNew, self, value );
               Memory_Free(ppcNameNew);
            }
            else
            {
               /* else the string is the name of a ppc we look for */
               materialExt->ppc = Stg_ComponentFactory_ConstructByName( self->cf, (Name)configName, Ppc, True, data );
            }
            /* also go ahead and ensure ppc is build */
            Stg_Component_Build( materialExt->ppc, NULL, False );
         }
      }

      /* Is it passive property? */
      if ( self->tagList[tag_I].passiveProperty  )
      {
         anyPassiveProperty = True;
         self->tagList[tag_I].storeOnParticles = True;
      }

      /* Extend the particles */
      if ( self->tagList[tag_I].storeOnParticles )
      {
         self->tagList[tag_I].particleExtHandle = ExtensionManager_Add( ms->particleExtensionMgr, (Name)self->type, sizeof( PpcManager_ParticleExt ) );

         particleExt = ExtensionManager_Get( ms->particleExtensionMgr, &particle, self->tagList[tag_I].particleExtHandle );

         materialPointsSwarmVariable = Swarm_NewScalarVariable( ms, (Name)self->tagList[tag_I].name, (ArithPointer) &particleExt->value - (ArithPointer) &particle, Variable_DataType_Double );
         /* To free memory */
         LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*)materialPointsSwarmVariable->variable );
         LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*)materialPointsSwarmVariable );
      }
      /* Extend the particles to store the previous value */
      if ( self->tagList[tag_I].storePreviousValue  )
      {
         storeAnyPrevious = True;
         self->tagList[tag_I].previousParticleExtHandle = ExtensionManager_Add( ms->particleExtensionMgr, (Name)self->type, sizeof( PpcManager_ParticleExt ) );

         particleExt = ExtensionManager_Get( ms->particleExtensionMgr, &particle, self->tagList[tag_I].previousParticleExtHandle );
         Stg_asprintf( &newName, "previous-%s", self->tagList[tag_I].name );
         materialPointsSwarmVariable = Swarm_NewScalarVariable( ms, (Name)newName, (ArithPointer) &particleExt->value - (ArithPointer) &particle, Variable_DataType_Double );
         /* To free memory */
         LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*)materialPointsSwarmVariable->variable );
         LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*)materialPointsSwarmVariable );
      }

      /* Do we store in on the mesh */
      if ( self->tagList[tag_I].storeOnMesh  )
      {
         char *meshVarName=NULL;
         Stg_asprintf( &meshVarName, "%s-mesh", self->tagList[tag_I].name );
         /* if so create a new PpcFeVariable */
         self->tagList[tag_I].ppcFeVar = PpcFeVariable_New( meshVarName,
                                                 (DomainContext*)self->integrationSwarm->context,
                                                 self->mesh,
                                                 self->integrationSwarm,
                                                 False, /* no accumulation */
                                                 self,
                                                 tag_I );
         Stg_Component_Build( self->tagList[tag_I].ppcFeVar, NULL, False );
         Memory_Free( meshVarName );
      }
   }

   if ( anyPassiveProperty )
   {
      if( Stg_Class_IsInstance( self->integrationSwarm->mapper, OneToOneMapper_Type ) ) {
      /* Add a function to update all passive properties  */
      EP_PrependClassHook( Context_GetEntryPoint( self->context, AbstractContext_EP_DumpClass),//AbstractContext_EP_SaveClass ),
                           _PpcManager_CalculatePassiveProperties,
                           self );
      } else {

      /* Add a function to update all passive properties  */
      EP_PrependClassHook( Context_GetEntryPoint( self->context, AbstractContext_EP_SaveClass ),
                           _PpcManager_CalculatePassivePropertiesOneToMany,
                           self );
      }
   }

   if( ms ) {
      // realloc swarm as new extension may have been created
      Swarm_Realloc( ms );
   }

   Journal_Printf( self->info_stream, "done\n" );
}


void _PpcManager_Initialise( void* _self, void* data )
{
   PpcManager* self = (PpcManager*) _self;
   PpcTag*     tagEntry = NULL;
   ExtensionManager *extMgr = NULL;
   MaterialPointsSwarm* ms = NULL;
   MaterialPoint *mp = NULL;
   PpcManager_ParticleExt* current = NULL;

   int tag_i, p_i, mp_local_count;

   ms = self->materialSwarm;

   Stg_Component_Initialise( self->integrationSwarm, data, False );

   if ( ms )
   {
      Stg_Component_Initialise( ms, data, False );
      mp_local_count = ms->particleLocalCount;
      extMgr = ms->particleExtensionMgr;
   }


   /* initialise any particle storage */
   for( tag_i = 0 ; tag_i < self->tagsCount ; tag_i++ ) {
      tagEntry = &(self->tagList[tag_i]);

      /* initialise if sole ppc  */
      if ( tagEntry->solePpc != NULL  )
      {
         Stg_Component_Initialise( tagEntry->solePpc, NULL, False );
      }

      if(tagEntry->storeOnParticles) {
         /* start initialise the particle storage */
         for ( p_i = 0 ; p_i < mp_local_count ; p_i++  ) {
            mp = (MaterialPoint*)Swarm_ParticleAt( ms, p_i );

            current = ExtensionManager_Get( extMgr, mp, tagEntry->particleExtHandle );
            current->value = 0.0;
         }
      }

      if( tagEntry->storePreviousValue ) _PpcManager_UpdatePrevious( self, tagEntry );
   }
}

void _PpcManager_Delete( void* _self ) {
   _Stg_Component_Delete( _self );
}

void _PpcManager_Print( void* _self, Stream* stream ) {}

void _PpcManager_Execute( void* _self, void* data ) {}

void _PpcManager_Destroy( void* _self, void* data )
{
   PpcManager* self = (PpcManager*) _self;
   unsigned ii;

   /* Destroy all ppcFeVar created */
   for ( ii=0; ii<self->tagsCount; ii++ )
   {
      if ( self->tagList[ii].storeOnMesh )
         Stg_Component_Destroy( self->tagList[ii].ppcFeVar, data, False );
      if ( self->tagList[ii].solePpc )
         Stg_Component_Destroy( self->tagList[ii].solePpc, data, False );
      Memory_Free( self->tagList[ii].name );
   }
   if ( self->tagsCount > 0 )
      Memory_Free( self->tagList );
}


int PpcManager_GetPrevious( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, int tagID, double* result )
{
   PpcManager* self = (PpcManager*) _self;
   MaterialPointsSwarm *materialSwarm = NULL;
   IntegrationPointsSwarm* swarm = (IntegrationPointsSwarm*)self->integrationSwarm;
   PpcManager_ParticleExt *particleExt = NULL;
   MaterialPoint *mp = NULL;
   ExtensionInfo_Index previousExtHan;
   PpcTag *tagEntry = NULL;

   /* check the tagID is valid */
   assert( tagID != -1 );
   tagEntry = &(self->tagList[tagID]);
   assert( tagEntry->storePreviousValue == True );

   /* get the previous particle extension: */
   previousExtHan = tagEntry->previousParticleExtHandle;

   /* get material point and material swarm */
   mp = OneToOneMapper_GetMaterialPoint( swarm->mapper, particle, &materialSwarm );
   assert(mp != NULL);

   /* get previous value extension */
   particleExt = ExtensionManager_Get( materialSwarm->particleExtensionMgr, mp, previousExtHan );
   
   *result = particleExt->value;

   return 0;
}

/* assumes a OneToOneMap is used for the material and integration points */
int _PpcManager_GetWithOneToOneMap( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, int tagID, double* result )
{
   PpcManager* self = (PpcManager*) _self;
   PpcManager_MaterialExt* materialExt=NULL;
   Material* material = NULL;
   IntegrationPointsSwarm* swarm = (IntegrationPointsSwarm*)self->integrationSwarm;
   ExtensionInfo_Index  materialExtHandle = -1;
   //Index tag_I;
   PpcTag* tagEntry = NULL;
   Bool storeOnParticles = False;
   Ppc *solePpc = NULL;
   unsigned lastStoredTimestep, currentTimestep;
   int err;

   if ( tagID == -1 )
      return -1;

   /* Get the tag in the tag list */
   tagEntry = &(self->tagList[tagID]);
   materialExtHandle = tagEntry->materialExtHandle;
   storeOnParticles = tagEntry->storeOnParticles;
   lastStoredTimestep = tagEntry->lastStoredTimestep;
   solePpc = tagEntry->solePpc;

   currentTimestep = self->context->timeStep;

   if ( storeOnParticles )
   {
      /*
       * Check if we need to update the particle storage.
       * Involves checking if the last stored state was on the same
       * nonLinear iteration and timestep iteration as now
       */
      if ( tagEntry->lastStoredTimestep != currentTimestep )
      {
         _PpcManager_StoreOnParticles( self, tagEntry );
         tagEntry->lastStoredTimestep = currentTimestep;
      }
      else if ( tagEntry->storeNLOnParticles )
      {

         Stg_ObjectList *sle_list = self->context->slEquations;
         SystemLinearEquations *sle = NULL;
         assert( sle_list );

         /* only check if the 'stokesEqn' is nonLinear. This could be improved
          * to search only the sle we're currently iterating in. JG 7Sep10 */
         sle = Stg_ObjectList_Get( sle_list, "stokesEqn" );
         assert(sle);
         if ( sle->isNonLinear )
         {
            if ( tagEntry->lastStoredNLstep != sle->nonLinearIteration_I )
            {
               _PpcManager_StoreOnParticles( self, tagEntry );
               tagEntry->lastStoredNLstep = sle->nonLinearIteration_I;
            }
         }

      }

      /* access particle storage */
      result[0] = _OneToOneMapper_GetDoubleFromExtension( swarm->mapper, particle, tagEntry->particleExtHandle, 0);

      /*
      materialPoint = OneToOneMapper_GetMaterialPoint( swarm->mapper, particle, &materialSwarm );
      particleExt = ExtensionManager_Get( materialSwarm->particleExtensionMgr, materialPoint, tagEntry->particleExtHandle );
      result[0] = particleExt->value;
      */
      return 0;
   }

   /* for ppcs not defined per material */
   if ( solePpc != NULL )
   {
      err = Ppc_Get( solePpc, lElement_I, particle, result );
      return err;
   }

   if ( materialExtHandle == (unsigned)-1 )
      return -1;

   /* Get the material */
   material = IntegrationPointsSwarm_GetMaterialOn( (IntegrationPointsSwarm*) swarm, particle );

   Journal_Firewall( material != NULL,
                     self->error_stream,
                     "No material found.  Please ensure your model contains at least one material\n" );

   /* Get the extension */
   materialExt = ExtensionManager_Get( material->extensionMgr, material, materialExtHandle );

   Journal_Firewall( materialExt->ppc != NULL,
                     self->error_stream, "\n\n\n"
                     "Material <%s> has no configuration for property <%s>..."
                     "\n\n\n", material->name, tagEntry->name );

   /* Get the value */
   err = Ppc_Get( materialExt->ppc, lElement_I, particle, result );

   return err;
}

/* assumes a ManyToOneMap is used for the material and integration points */
int _PpcManager_GetWithOneToManyMap( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, int tagID, double* result )
{
   PpcManager* self = (PpcManager*) _self;
   PpcManager_MaterialExt* materialExt=NULL;
   Material* material = NULL;
   OneToManyMapper* mapper=NULL;
   IntegrationPointsSwarm* swarm = (IntegrationPointsSwarm*)self->integrationSwarm;
   ExtensionInfo_Index  materialExtHandle = -1;
   //Index tag_I;
   PpcTag* tagEntry = NULL;
   Bool storeOnParticles = False;
   Ppc *solePpc = NULL;
   MaterialPoint* mp=NULL;
   OneToManyRef *ref=NULL;
   unsigned lastStoredTimestep, currentTimestep;
   double c;
   int ii;
   int err;

   if ( tagID == -1 )
      return -1;

   /* Get the tag in the tag list */
   tagEntry = &(self->tagList[tagID]);
   materialExtHandle = tagEntry->materialExtHandle;
   storeOnParticles = tagEntry->storeOnParticles;
   lastStoredTimestep = tagEntry->lastStoredTimestep;
   solePpc = tagEntry->solePpc;

   currentTimestep = self->context->timeStep;

   if ( storeOnParticles )
   {
      /*
       * Check if we need to update the particle storage.
       * Involves checking if the last stored state was on the same
       * nonLinear iteration and timestep iteration as now
       */
      if ( tagEntry->lastStoredTimestep != currentTimestep )
      {
         _PpcManager_StoreOnParticlesOneToMany( self, tagEntry );
         tagEntry->lastStoredTimestep = currentTimestep;
      }
      else if ( tagEntry->storeNLOnParticles )
      {

         Stg_ObjectList *sle_list = self->context->slEquations;
         SystemLinearEquations *sle = NULL;
         assert( sle_list );

         /* only check if the 'stokesEqn' is nonLinear. This could be improved
          * to search only the sle we're currently iterating in. JG 7Sep10 */
         sle = Stg_ObjectList_Get( sle_list, "stokesEqn" );
         assert(sle);
         if ( sle->isNonLinear )
         {
            if ( tagEntry->lastStoredNLstep != sle->nonLinearIteration_I )
            {
               _PpcManager_StoreOnParticlesOneToMany( self, tagEntry );
               tagEntry->lastStoredNLstep = sle->nonLinearIteration_I;
            }
         }

      }

      /* access particle storage
       * This magic function should be the all in one function to map the
       * material point values to this intergration point. 
       * Note the final argument, the offset is 0, meaning the extension 
       * is only a double data type. */
      result[0] = _OneToManyMapper_GetDoubleFromExtension( swarm->mapper, particle, tagEntry->particleExtHandle, 0 );

      return 0;
   }

   /* for ppcs not defined per material */
   if ( solePpc != NULL )
   {
      err = Ppc_Get( solePpc, lElement_I, particle, result );
      return err;
   }

   if ( materialExtHandle == (unsigned)-1 )
      return -1;

   /* 
    * ppc value is on material extension.
    * Algorithm is loop over all the material points that are mapped 
    * from this integration point and sum up their Ppc contributions. 
    */

   mapper = (OneToManyMapper*)swarm->mapper;
   ref = OneToManyMapper_GetMaterialRef(mapper, particle);

   result[0] = 0; /* zero as we'll accumulate */
   
   for(ii = 0; ii < ref->numParticles; ii++) {
      /* get the materialPoint */
      mp = (MaterialPoint*)Swarm_ParticleAt(mapper->materialSwarm, ref->particleInds[ii]);
      /* get the material and materialExt */
      material = (Material*)Materials_Register_GetByIndex(
            mapper->materialSwarm->materials_Register, 
            mp->materialIndex );
      materialExt = ExtensionManager_Get( material->extensionMgr, material, materialExtHandle );

      Journal_Firewall( materialExt->ppc != NULL,
                     self->error_stream, "\n\n\n"
                     "Material <%s> has no configuration for property <%s>..."
                     "\n\n\n", material->name, tagEntry->name );

      /* Get the Ppc value */
      err = Ppc_Get( materialExt->ppc, lElement_I, particle, result );

      /* sum contribution */
      c = *result;
      result[0] += ((double)ref->weights[ii])*c;
   }

   return err;
}

int PpcManager_GetGravity( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double result[3] )
{
   PpcManager* self = (PpcManager*) _self;
   int ii;
   /* FIXME: this function should use the ppc */
   for ( ii = 0 ; ii < 3 ; ii++ )
      result[ii] = self->gravity[ii];

   return 0;
}


int PpcManager_GetConstant( void* _self, char* constantName, double* result )
{
   PpcManager* self = (PpcManager*) _self;
   Scaling* S = self->scaling;
   int err = -1;

   Journal_Firewall( (Bool) S,
                     self->error_stream, "\n\n\n"
                     "The PpcManager does not know the scaling component\n"
                     "\n\n\n" );

   result[0] = 0;

   if ( !strcmp( constantName, "R" ) || !strcmp( constantName, "GasConstant" ) )
   {
      result[0] = Scaling_Scale( S, 8.314472, (UnitDefinition_Type)
      {
         2, 1,-2,-1, 0, 1
      }
                               );
      err = 0;
   }
   else if ( !strcmp( constantName, "AtomicMass" ) )
   {
      result[0] = Scaling_Scale( S, 1.660538782e-27, (UnitDefinition_Type)
      {
         0, 1, 0, 0, 0, 1
      }
                               );
      err = 0;
   }
   else if ( !strcmp( constantName, "Avogadro" ) )
   {
      result[0] = Scaling_Scale( S, 6.02214179e23, (UnitDefinition_Type)
      {
         0, 0, 0, 0, 0, 1
      }
                               );
      err = 0;
   }
   else if ( !strcmp( constantName, "Boltzmann" ) )
   {
      result[0] = Scaling_Scale( S, 1.3806504e-23, (UnitDefinition_Type)
      {
         2, 1,-2,-1, 0, 1
      }
                               );
      err = 0;
   }
   else if ( !strcmp( constantName, "ElectricConstant" ) )
   {
      result[0] = Scaling_Scale( S, 8.854187817e-12, (UnitDefinition_Type)
      {
         -3,-1, 4, 0, 2, 1
      }
                               );
      err = 0;
   }
   else if ( !strcmp( constantName, "ElectronMass" ) )
   {
      result[0] = Scaling_Scale( S, 9.10938215e-31, (UnitDefinition_Type)
      {
         0, 1, 0, 0, 0, 1
      }
                               );
      err = 0;
   }
   else if ( !strcmp( constantName, "ElectronVolt" ) )
   {
      result[0] = Scaling_Scale( S, 1.602176487e-19, (UnitDefinition_Type)
      {
         2, 1,-2, 0, 0, 1
      }
                               );
      err = 0;
   }
   else if ( !strcmp( constantName, "ElementaryCharge" ) )
   {
      result[0] = Scaling_Scale( S, 1.602176487e-19, (UnitDefinition_Type)
      {
         0, 0, 1, 0, 1, 1
      }
                               );
      err = 0;
   }
   else if ( !strcmp( constantName, "Faraday" ) )
   {
      result[0] = Scaling_Scale( S, 96485.3399, (UnitDefinition_Type)
      {
         0, 0, 1, 0, 1, 1
      }
                               );
      err = 0;
   }
   else if ( !strcmp( constantName, "MagneticConstant" ) )
   {
      result[0] = Scaling_Scale( S, 12.566370614e-7, (UnitDefinition_Type)
      {
         1, 1,-2, 0,-2, 1
      }
                               );
      err = 0;
   }
   else if ( !strcmp( constantName, "G" ) || !strcmp( constantName, "GravitationConstant" ) )
   {
      result[0] = Scaling_Scale( S, 6.67428e-11, (UnitDefinition_Type)
      {
         3,-1,-2, 0, 0, 1
      }
                               );
      err = 0;
   }
   else if ( !strcmp( constantName, "Planck" ) )
   {
      result[0] = Scaling_Scale( S, 6.62606896e-34, (UnitDefinition_Type)
      {
         2, 1,-1, 0, 0, 1
      }
                               );
      err = 0;
   }
   else if ( !strcmp( constantName, "VacuumLightSpeed" ) )
   {
      result[0] = Scaling_Scale( S, 299792458, (UnitDefinition_Type)
      {
         1, 0,-1, 0, 0, 1
      }
                               );
      err = 0;
   }
   else
   {
      result[0] = 0;
      err = -1;
   }
   return err;
}

int PpcManager_SearchTagList( void* _self, char* entryString )
{
   PpcManager* self = (PpcManager*) _self;
   PpcTag *tagEntry = NULL;
   int tag_I;

   for ( tag_I = 0 ; tag_I < self->tagsCount ; tag_I++ )
   {
      tagEntry = &(self->tagList[tag_I]);
      if ( strcmp( entryString, tagEntry->name ) == 0 )
      {
         Journal_Printf( self->info_stream,
                         "Found Ppc '%s' in the '%s' Properties list.\n",
                         entryString, self->name );
         return tag_I;
      }
   }
   return -1;
}

void PpcManager_AddSolePpcToList( PpcManager* mgr, Ppc* ppc, char* newName )
{
   int ppcID = mgr->tagsCount;
   char* savedName  = StG_Strdup( newName ); /* store this name incase newName dies when reallocarray is called */

   ppc->ppcMgrID = ppcID; 

   mgr->tagsCount++;
   mgr->tagList = ReallocArray( mgr->tagList, PpcTag, mgr->tagsCount );

   /* store the ppc as a solePpc, not a 'Property' on material extensions */
   mgr->tagList[ ppcID ].name = StG_Strdup( savedName );
   mgr->tagList[ ppcID ].solePpc = ppc;
   mgr->tagList[ ppcID ].storeOnParticles = False;
   mgr->tagList[ ppcID ].storeNLOnParticles = False;
   mgr->tagList[ ppcID ].storeOnMesh = False;
   mgr->tagList[ ppcID ].materialExtHandle = (unsigned)-1;
   mgr->tagList[ ppcID ].lastStoredTimestep = (unsigned) -1;
   mgr->tagList[ ppcID ].particleExtHandle = (unsigned)-1;
   mgr->tagList[ ppcID ].storePreviousValue = False;
   mgr->tagList[ ppcID ].previousParticleExtHandle = (unsigned)-1;
   mgr->tagList[ ppcID ].passiveProperty = False;

   Journal_Printf( mgr->info_stream,
                   "Creating Ppc '%s' as a solePpc not related to the Properties list defined in '%s'.\n",
                   newName, mgr->name );
   Memory_Free( savedName );
}

int PpcManager_GetField( PpcManager* mgr,
                     Stg_ComponentFactory* cf,
                     Stg_Component* component,
                     Dictionary_Entry_Key key,
                     Bool essential ) {
   /**
    * Role:
    *   Returns the 'Ppc' id, for referencing the Ppc later through the PpcManager
    *
    * Detail:
    *   This function is designed to be a temporary replacement for the 
    *   Stg_ComponentFactory_ConstructByKey() workflow in Underworld coding.
    *   Allowing all Underworld code to use Ppc workflow instead of FieldVariables
    *   and SwarmVariables. Thus this function returns the id of a Ppc, found
    *   in XML that can be used later through the PpcManager.
    *   
    *   This cunning function can handle either finding an FieldVariable or a Ppc
    *   as XML input. 
    *   If a FieldVariable is found, a Ppc_Variable of that FieldVariable is created
    *   and used to reference the FieldVariable.
    *
    * Input:
    *   mgr          PpcManager used.
    *   cf           component factory of simulation
    *   component    the Stg_Component whose dictionary to parse
    *   key          the textual name of the XML param
    *   essential    when true: error if not key not resolved
    *
    */ 
   Ppc_Variable* pcVar=NULL;
   Stg_Component* thething=NULL;
   Name thethingsname=NULL;
   int pcID=-1;

   Dictionary *thisDictionary = NULL;
   Dictionary_Entry_Value *dev = NULL;

   thisDictionary = Dictionary_GetDictionary( cf->componentDict, component->name );

   Journal_Firewall( thisDictionary != NULL, mgr->error_stream,
                     "Error in func %s: Can't find the sub dictionary for component '%s'.\n",
                     __func__, component->name );

   dev = Dictionary_Get( thisDictionary, key );

   /* error */
   Journal_Firewall( ( dev == NULL && essential )==0, mgr->error_stream, 
               "Component '%s' cannot find essential input with key '%s'.\n"
               "Update the XML input to include\n\t<param name=\"%s\"> </param>\n\n", 
               component->name, key, key );

   if( dev == NULL && !essential ) {
      Journal_RPrintf( cf->infoStream, 
            "Warning: Component '%s' cannot find non-essential input with key '%s'.\n",
           component->name, key );
      return -1;
   }

   thethingsname = Dictionary_Entry_Value_AsString( dev );
   
   thething = LiveComponentRegister_Get( cf->LCRegister, thethingsname );

   if ( Stg_StringIsNumeric( thethingsname ) && thething == NULL ) // must be a constant value input file
   {
      /* then we must create a Ppc_Constant from scrach using the string "value" */
      double value = strtod( thethingsname, 0 );
      char *newName = NULL;
      Ppc* ppc;

      /* create a new name if a new ppc must be created from scratch */
      Stg_asprintf( &newName, "%s-solePpcNumber%03i", thethingsname, mgr->tagsCount);

      ppc = (Ppc*)Ppc_Constant_New( newName, mgr, value );
      return ppc->ppcMgrID;

   }

   Journal_Firewall( thething != NULL, mgr->error_stream,
                     "Stg_Component '%s' cannot find essential component '%s' (key '%s').\n", 
                     component->name, thethingsname, key );

   
   if( Stg_Class_IsInstance( thething, Ppc_Type ) ) {
      /* If we're here, then try find the ppc */
      pcID = PpcManager_GetPpcFromDict( mgr, mgr->cf, (Name)component->name, key, "" );
      assert( pcID > -1 ); //TODO Improve error handling
      return pcID;

   }
   
   thething = (Stg_Component*)Stg_ComponentFactory_ConstructByKey( mgr->cf, component->name, key, FieldVariable, essential, NULL );

   if( Stg_Class_IsInstance( thething, FieldVariable_Type ) ) {
      FieldVariable* fieldVar = (FieldVariable*)thething;

      /* so fieldVar is found,
       * search if this FieldVariable is already cast to a Ppc.
       * if so, return that ppc
       * if not create a pcVar (Ppc_Variable) from it and return the pcVar
       */
      PpcTag* tagEntry=NULL;
      int tag_i;
      char* ppcName=NULL;

      /* search in PpcManager for Ppc_Variables that represent fieldVar */
      for( tag_i = 0 ; tag_i < mgr->tagsCount ; tag_i++ ) {
         tagEntry = &(mgr->tagList[tag_i]);
         /* ASSUMPTION: check ONLY solePpc types */
         if( tagEntry->solePpc ) {
            if( Stg_Class_IsInstance( tagEntry->solePpc, Ppc_Variable_Type ) ) {
               Ppc_Variable* pcVar=(Ppc_Variable*)tagEntry->solePpc;
               if( pcVar->fieldVar == fieldVar ) { return tag_i; } /* if found fieldVar, return tag_i */
            }
         } 
#if 0
     Will do later if required, but for now it seems overkill and we can assume all previously
        fieldVar casted into Ppc_Variables are 'solePpc', not associated with materials

         else {
            /* assume ppc exists as a material extension */
            PpcManager_MaterialExt materialExt = 
               ExtensionManager_GetFunc( material->extensionMgr, material, tagEntry->materialExtHandle );
            ...}
#endif
      }

      /* it wasn't found, so create the cast */
      Stg_asprintf( &ppcName, "%s-ppc", fieldVar->name );
      pcVar = Ppc_Variable_New( ppcName,
                        mgr,
                        fieldVar,
                        NULL );
      Memory_Free( ppcName );
      return pcVar->ppcMgrID;
   }
   else {
      assert(0); /* shouldn't hit this */
   }
}

int PpcManager_GetPpcFromDict( PpcManager* mgr,
                               Stg_ComponentFactory* cf,
                               Name componentName,
                               Dictionary_Entry_Key key,
                               Name defaultName )
{
   /**
    * Role:
    *  This function returns a tag to a Ppc, given a specific dictionary entry
    *  key (ppcName) in a componentName's dicitionary.
    *  If an appropriate Ppc is not found, PpcManager_GetPpcByName() is used to
    *  search for the key in the entire dictionary. If that search doesn't succeed
    *  the defaultName is then used as the entry key in PpcManager_GetPpcByName()
    *
    * It's assumed this function runs before _PpcManager_Build()
    *
    * Returns: the tag name for this ppc.
    */
   Dictionary *thisDictionary = NULL;
   Dictionary_Entry_Value *dev = NULL;
   char *ppcName = NULL;
   int tag = -1;

   Journal_Firewall( mgr != NULL, Journal_Register( Error_Type, "Error_Type" ), "Error in func %s: No PpcManager provided", __func__ );

   thisDictionary = Dictionary_GetDictionary( cf->componentDict, componentName );

   /** if this failed, check to see if called from a plugin */
   if(!thisDictionary){
      Dictionary* pluginDict = (Dictionary*)Dictionary_Get( cf->rootDict, "plugins" );
      Dictionary*               thisPluginDict = NULL;
      Index pluginIndex;
      Name  pluginType;

      if(pluginDict){
         /* search for plugins Dictionary */
         for( pluginIndex = 0; pluginIndex < Dictionary_Entry_Value_GetCount( (Dictionary_Entry_Value*)pluginDict ); pluginIndex++ ) {
            thisPluginDict = Dictionary_Entry_Value_AsDictionary( Dictionary_Entry_Value_GetElement( (Dictionary_Entry_Value*)pluginDict, pluginIndex ) );
            pluginType = StG_Strdup( Dictionary_GetString( thisPluginDict, "Type" ) );

            if( !strcmp( componentName, pluginType ) ){
               Memory_Free( pluginType );
               thisDictionary = thisPluginDict;
               break;
            }
            Memory_Free( pluginType );
         }
      }
   }

   Journal_Firewall( thisDictionary != NULL, mgr->error_stream,
                     "Error in func %s: Can't find the sub dictionary for component '%s'.\n",
                     __func__, componentName );

   dev = Dictionary_Get( thisDictionary, key );

   if ( dev != NULL )
   {
      ppcName = Dictionary_Entry_Value_AsString( dev );
      tag = PpcManager_GetPpcByName( mgr, cf, ppcName );
   }

   if ( tag == -1 && strcmp( defaultName, "" ) != 0 )
   {
      tag = PpcManager_GetPpcByName( mgr, cf, defaultName );
   }

   return tag;
}

int PpcManager_GetPpcByName( PpcManager* mgr,
                             Stg_ComponentFactory* cf,
                             char* ppcName )
{

   /**
    * Role:
    *    This function returns a tag to a Ppc, given a dictionary entry key (ppcName)
    *    and component factory cf.
    *
    *    If the ppcName is recognised as a numeric entry
    *       a new Ppc_Constant is created as a solePpc, its tag is returned
    *    else if the ppcName entry is recognised from the labels in the Property list
    *       the appropriate tag is returned
    *    else
    *       the entry is assumed to be the name of a solePpc defined elsewhere in the
    *       dicionary, this function attempts the solePpc and return its tag.
    *
    * Returns: the tag name for a ppc.
    *
    * It's assumed this function runs before _PpcManager_Build()
    */

   Ppc* ppc = NULL;
   int returnTag = -1;

   if ( Stg_StringIsNumeric( ppcName ) )
   {
      /* if the string is numeric, then we must create a Ppc_Constant from scrach
      * using the string "value" */
      double value = strtod( ppcName, 0 );
      char *newName = NULL;

      /* create a new name if a new ppc must be created from scratch */
      Stg_asprintf( &newName, "%s-solePpcNumber%03i", ppcName, mgr->tagsCount);

      ppc = (Ppc*)Ppc_Constant_New( newName, mgr, value );
      returnTag = ppc->ppcMgrID;

   }
   else
   {

      /* else we assume the ppc is already defined in the manager's tagList */
      returnTag = PpcManager_SearchTagList( mgr, ppcName );
      if ( returnTag == -1 )
      {
         /* if the ppc name wasn't found in the taglist we see
          * if it is defined elsewhere and store as a solePpc */
         ppc = Stg_ComponentFactory_ConstructByName( cf, (Name)ppcName, Ppc, True, NULL );
         returnTag = ppc->ppcMgrID;
      }

   }
   return returnTag;
}

void PpcManager_GetMinValue( PpcManager* self, int tagID, double *result ) {
   PpcTag* tagEntry = &(self->tagList[tagID]);
   assert(self->tagList[tagID].storeOnParticles);

   *result = tagEntry->min;
}

void PpcManager_GetMaxValue( PpcManager* self, int tagID, double *result ) {
   PpcTag* tagEntry = &(self->tagList[tagID]);
   assert(self->tagList[tagID].storeOnParticles);

   *result = tagEntry->max;
}

void _PpcManager_StoreOnParticles( PpcManager* self, PpcTag* tagEntry )
{
   FeMesh* mesh = self->mesh;
   IntegrationPointsSwarm* is = self->integrationSwarm;
   MaterialPointsSwarm *ms = self->materialSwarm;
   Material* material = NULL;
   MaterialPoint *mp = NULL;
   IntegrationPoint *ip = NULL;
   ExtensionInfo_Index  materialExtHandle = tagEntry->materialExtHandle;
   PpcManager_MaterialExt *materialExt = NULL;
   PpcManager_ParticleExt *particleExt = NULL;
   Cell_Index cell_I;
   Particle_InCellIndex cParticle_I;
   Particle_InCellIndex cellParticleCount;
   double result;
   unsigned e_i;
   int nDims = Mesh_GetDimSize( mesh );
   int nElements = Mesh_GetLocalSize( mesh, nDims );
   int err;

   /* if we are storing the previous timestep value, check we're on a new timestep and save it */
   if( ( ( self->context->timeStep > tagEntry->lastStoredTimestep ) || tagEntry->lastStoredTimestep == (unsigned)-1 ) && tagEntry->storePreviousValue == True ) {
      _PpcManager_UpdatePrevious( self, tagEntry );
   }

   /* loop over every particle and assign the ppc value it */

   for ( e_i = 0; e_i < nElements; e_i++ )
   {
      cell_I = CellLayout_MapElementIdToCellId( is->cellLayout, e_i );
      cellParticleCount = is->cellParticleCountTbl[cell_I];

      for ( cParticle_I = 0 ; cParticle_I < cellParticleCount ; cParticle_I++ )
      {
         ip = (IntegrationPoint*) Swarm_ParticleInCellAt( is, cell_I, cParticle_I );

         /* Get the material */
         material = IntegrationPointsSwarm_GetMaterialOn( (IntegrationPointsSwarm*) is, ip );

         /* Get the  material extension */
         materialExt = ExtensionManager_Get( material->extensionMgr, material, materialExtHandle );

         Journal_Firewall( materialExt->ppc != NULL,
                           self->error_stream, "\n\n\n"
                           "Material <%s> has no configuration for property <%s>..."
                           "\n\n\n", material->name, tagEntry->name );

         /* Get the value */
         err = Ppc_Get( materialExt->ppc, e_i, ip, &result );
         assert( !err );

         /* Get the material point */
         mp = OneToOneMapper_GetMaterialPoint( is->mapper, ip, &ms );

	 if( result < tagEntry->min )
	    tagEntry->min = result;
	 if( result > tagEntry->max ) 
	    tagEntry->max = result;

         /* Store the result on the material point extension */
         particleExt = ExtensionManager_Get( ms->particleExtensionMgr, mp, tagEntry->particleExtHandle );
         particleExt->value = result;
         
      }
   }
}

void _PpcManager_StoreOnParticlesOneToMany( PpcManager* self, PpcTag* tagEntry )
{
   /* Role:
    *    To store ppc values on material points when a OneToMany mapper is used
    */

   FeMesh* mesh = self->mesh;
   MaterialPointsSwarm *ms = self->materialSwarm;
   Material* material = NULL;
   MaterialPoint *mp = NULL;
   IntegrationPoint ip;
   ExtensionInfo_Index  materialExtHandle = tagEntry->materialExtHandle;
   ExtensionInfo_Index  particleExtHandle = tagEntry->materialExtHandle;
   PpcManager_MaterialExt *materialExt = NULL;
   PpcManager_ParticleExt *particleExt = NULL;
   Cell_Index cell_I;
   Particle_InCellIndex cellParticleCount;
   double result;
   int nDims = Mesh_GetDimSize( mesh );
   int nElements = Mesh_GetLocalSize( mesh, nDims );
   int err, e_i, p_i;

   /* if we are storing the previous timestep value, check we're on a new timestep and save it */
   if( ( ( self->context->timeStep > tagEntry->lastStoredTimestep ) || tagEntry->lastStoredTimestep == (unsigned)-1 ) && tagEntry->storePreviousValue == True ) {
      _PpcManager_UpdatePrevious( self, tagEntry );
   }

   particleExtHandle = tagEntry->particleExtHandle;

   /* for each element do particle operation */
   for( e_i = 0 ; e_i < nElements ; e_i++ ) {
      cell_I = CellLayout_MapElementIdToCellId( ms->cellLayout, e_i );
      cellParticleCount = ms->cellParticleCountTbl[cell_I];

      /* loop over particles in element and assign the ppc value it */
      for(p_i = 0;p_i < cellParticleCount;p_i++ ) {

         /* for each particle cast it to 'an integration point' (to allow the
          * _Get() function to work and then save the value on the particles */
         mp = (MaterialPoint*)Swarm_ParticleInElementAt( ms, e_i, p_i );

         /* cast material point to an fake integration point. Note: fake the weight */
         FeMesh_CoordGlobalToLocal( mesh, mp->owningCell, mp->coord, ip.xi );
         ip.owningCell = mp->owningCell;
         ip.weight = 0;


         /* the the material */
         material = Materials_Register_GetByIndex( ms->materials_Register, mp->materialIndex ); 
         /* Get the  material extension */
         materialExt = ExtensionManager_Get( material->extensionMgr, material, materialExtHandle );
         Journal_Firewall( materialExt->ppc != NULL,
                           self->error_stream, "\n\n\n"
                           "Material <%s> has no configuration for property <%s>..."
                           "\n\n\n", material->name, tagEntry->name );

         /* Get the value */
         err = Ppc_Get( materialExt->ppc, e_i, &ip, &result );
         assert( !err );

          /* Store the result on the material point extension */
         particleExt = ExtensionManager_Get( ms->particleExtensionMgr, mp, particleExtHandle );
         particleExt->value = result;
      }
   }
}

void _PpcManager_UpdatePrevious( PpcManager* self, PpcTag* tagEntry )
{
   MaterialPointsSwarm *materialSwarm = NULL;
   MaterialPoint *particle = NULL;
   ExtensionManager *extMgr = NULL;
   ExtensionInfo_Index  currentExtHan, previousExtHan;
   PpcManager_ParticleExt *current = NULL, *previous = NULL;
   unsigned par_I;

   materialSwarm = self->materialSwarm;
   extMgr = materialSwarm->particleExtensionMgr;

   currentExtHan = tagEntry->particleExtHandle;
   previousExtHan = tagEntry->previousParticleExtHandle;

   for ( par_I = 0 ; par_I < materialSwarm->particleLocalCount ; par_I++  )
   {
      particle = (MaterialPoint*)Swarm_ParticleAt( materialSwarm, par_I );

      current = ExtensionManager_Get( extMgr, particle, currentExtHan );
      previous = ExtensionManager_Get( extMgr, particle, previousExtHan );

      previous->value = current->value;
   }
}


void _PpcManager_CalculatePassiveProperties( PpcManager* self, void* data )
{
   MaterialPointsSwarm *materialSwarm = NULL;
   IntegrationPoint *particle = NULL;
   ExtensionManager *extMgr = NULL;
   ExtensionInfo_Index currentExtHan;
   PpcManager_ParticleExt *current = NULL;
   PpcTag *tagEntry = NULL;
   double result;
   int tag_I, err;
   FeMesh* mesh = self->mesh;
   int nDims = Mesh_GetDimSize( mesh );
   int nElements = Mesh_GetLocalSize( mesh, nDims );
   Element_LocalIndex e_i;
   Cell_Index cell_I;
   Particle_InCellIndex cParticle_I;
   Particle_InCellIndex cellParticleCount;

   materialSwarm = self->materialSwarm;
   extMgr = materialSwarm->particleExtensionMgr;

   for ( tag_I = 0 ; tag_I < self->tagsCount ; tag_I++ )
   {
      tagEntry = &(self->tagList[tag_I]);

      if ( tagEntry->passiveProperty )
      {
			_PpcManager_StoreOnParticles(self, tagEntry);
      }
   }
}

void _PpcManager_CalculatePassivePropertiesOneToMany( PpcManager* self, void* data )
{
   MaterialPointsSwarm *ms = NULL;
   Material* material=NULL;
   IntegrationPoint ip;
   MaterialPoint *mp=NULL;
   ExtensionManager *extMgr = NULL;
   ExtensionInfo_Index particleExtHandle;
   PpcManager_ParticleExt *particleExt = NULL;
   PpcManager_MaterialExt *materialExt = NULL;
   PpcTag *tagEntry = NULL;
   double result;
   int tag_I, err, p_i, e_i;
   FeMesh* mesh = self->mesh;
   int nDims = Mesh_GetDimSize( mesh );
   int nElements = Mesh_GetLocalSize( mesh, nDims );
   Cell_Index cell_I;
   Particle_InCellIndex cellParticleCount;
   ExtensionInfo_Index  materialExtHandle;
   

   ms = self->materialSwarm;
   extMgr = ms->particleExtensionMgr;

   /* for each element do particle operation */
   for( e_i = 0 ; e_i < nElements ; e_i++ ) {
      cell_I = CellLayout_MapElementIdToCellId( ms->cellLayout, e_i );
      cellParticleCount = ms->cellParticleCountTbl[cell_I];

      /* loop over particles in element and assign the ppc value it */
      for(p_i = 0;p_i < cellParticleCount;p_i++ ) {

         /* for each particle cast it to 'an integration point' (to allow the
          * _Get() function to work and then save the value on the particles */
         mp = (MaterialPoint*)Swarm_ParticleInElementAt( ms, e_i, p_i );

         /* cast material point to an fake integration point. Note: fake the weight */
         FeMesh_CoordGlobalToLocal( mesh, mp->owningCell, mp->coord, ip.xi );
         ip.owningCell = mp->owningCell;
         ip.weight = 0;

         /* for all ppc, check if it's a passiveProperty, if true store it */
         for( tag_I = 0 ; tag_I < self->tagsCount ; tag_I++ ) {
            tagEntry = &(self->tagList[tag_I]);

            if ( tagEntry->passiveProperty )
            {
               particleExtHandle = tagEntry->particleExtHandle;
               materialExtHandle = tagEntry->materialExtHandle;

               /* the the material */
               material = Materials_Register_GetByIndex( ms->materials_Register, mp->materialIndex ); 
               /* Get the  material extension */
               materialExt = ExtensionManager_Get( material->extensionMgr, material, materialExtHandle );
               Journal_Firewall( materialExt->ppc != NULL,
                                 self->error_stream, "\n\n\n"
                                 "Material <%s> has no configuration for property <%s>..."
                                 "\n\n\n", material->name, tagEntry->name );

               /* Get the value */
               err = Ppc_Get( materialExt->ppc, e_i, &ip, &result );
               assert( !err );

                /* Store the result on the material point extension */
               particleExt = ExtensionManager_Get( ms->particleExtensionMgr, mp, particleExtHandle );
               particleExt->value = result;
            }
         }
      }
   }
}
