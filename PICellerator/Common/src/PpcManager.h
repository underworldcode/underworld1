#ifndef __PICellerator_Common_PpcManager_h__
#define __PICellerator_Common_PpcManager_h__

/** Textual name of this class */
extern const Type PpcManager_Type;

/* Material extension */
typedef struct
{
   Ppc*  ppc;
} PpcManager_MaterialExt;

/* Particle extension */
typedef struct
{
   double  value;
} PpcManager_ParticleExt;

/* Tag - quantity list */
typedef struct
{
   /* id to identify the property */
   char*                name;
   /* material extension */
   ExtensionInfo_Index  materialExtHandle;
   /* Particle storage */
   Bool                 storeOnParticles;
   unsigned             lastStoredTimestep;
   Bool                 storeNLOnParticles;
   unsigned             lastStoredNLstep;
   ExtensionInfo_Index  particleExtHandle;
   /* Particle storage for previous step value*/
   Bool                 storePreviousValue;
   ExtensionInfo_Index  previousParticleExtHandle;
   /* Mesh storage */
   Bool                 storeOnMesh;
   PpcFeVariable*       ppcFeVar;
   /* ppc storage for ppc not on materials */
   Ppc*                 solePpc;
   /* Passive properties: properties that are not used anywhere the code - only for output and post processing */
   Bool                 passiveProperty;
   double               min; // hold the min value
   double               max; // hold the max value
} PpcTag;

/* declare polymorphic function for 'Get' method */
typedef int (PpcManager_GetFuncPtr) (void*, Element_LocalIndex, IntegrationPoint*, int, double* );

/** PpcManager class contents */
#define __PpcManager \
		/* Parent info */ \
		__Stg_Component \
      /* function Pointers */ \
      PpcManager_GetFuncPtr*   _get; \
      /* General data */ \
		Stg_ComponentFactory*                               cf; \
      PICelleratorContext*                                context; \
		IntegrationPointsSwarm*                             integrationSwarm; \
		MaterialPointsSwarm*                                materialSwarm; \
		FeMesh*                                             mesh; \
		Scaling*                                            scaling; \
		/* Material extension */ \
		Materials_Register*                                 materials_Register;	\
		/* Tag list */ \
		PpcTag*                                             tagList; \
		int                                                 tagsCount; \
		/* Gravity: unused implementation */ \
		double                                              gravity[3]; \
      /* Setup streams */ \
      Stream*                                             info_stream; \
      Stream*                                             error_stream; \
 

struct PpcManager
{
   __PpcManager
};

/** Public constructor: takes a lot of args */
PpcManager* PpcManager_New( 
      Name                    name,
      PICelleratorContext*    context,
      Stg_ComponentFactory*   cf, 
      MaterialPointsSwarm*    ms, 
      IntegrationPointsSwarm* is,  
      Materials_Register*     mr, 
      FeMesh*                 m,
      double                  g, 
      Scaling*                sc );

#ifndef ZERO
#define ZERO 0
#endif

#define PPCMANAGER_DEFARGS \
                STG_COMPONENT_DEFARGS

#define PPCMANAGER_PASSARGS \
                STG_COMPONENT_PASSARGS

PpcManager* _PpcManager_New(  PPCMANAGER_DEFARGS  );

void _PpcManager_Delete( void* _self );
void _PpcManager_Print( void* _self, Stream* stream );

void* _PpcManager_DefaultNew( Name name );
void _PpcManager_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
void _PpcManager_Build( void* _self, void* data );
void _PpcManager_Initialise( void* _self, void* data );
void _PpcManager_Execute( void* _self, void* data );
void _PpcManager_Destroy( void* _self, void* data );

/* Private functions */
/* stores all the ppc (tagEntry) values on the particle's in one hit */
void _PpcManager_StoreOnParticles( PpcManager* self, PpcTag* tagEntry );
void _PpcManager_StoreOnParticlesOneToMany( PpcManager* self, PpcTag* tagEntry );
void _PpcManager_UpdatePrevious( PpcManager* self, PpcTag* tagEntry );
void _PpcManager_CalculatePassiveProperties( PpcManager* self, void* data );
void _PpcManager_CalculatePassivePropertiesOneToMany( PpcManager* self, void* data );

// The 2 functions below so far only work if the ppc is stored on particles
void PpcManager_GetMaxValue( PpcManager* self, int tagEntry, double *result );
void PpcManager_GetMinValue( PpcManager* self, int tagEntry, double *result );


/** Public functions  */
/* Get the current ppc value. This function is a wrapper and should always be called to access current property value */
#define PpcManager_Get( mgr, lElement_I, particle, tagID, result ) \
   ( mgr->_get( mgr, lElement_I, particle, tagID, result ) )


/** 
 * the different functions the virtual function 'PpcManager_Get' can be set to
 * */
 /* for use with a one-to-one mapping btw Int. point and Material points. */
int _PpcManager_GetWithOneToOneMap( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, int tagID, double* result );

 /* for use with a one-to-many mapping btw Int. point and Material points. */
int _PpcManager_GetWithOneToManyMap( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, int tagID, double* result );

/* Get the previous time step's ppc value. */
int PpcManager_GetPrevious( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, int tagID, double* result );
int PpcManager_SearchTagList( void* _self, char* entryString );

int PpcManager_GetConstant( void* _self, char* constantName, double* result );
int PpcManager_GetGravity( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double result[3] );

/* Register the input ppc as a solePpc in the 'mgr' list of tags */
void PpcManager_AddSolePpcToList( PpcManager* mgr, Ppc* ppc, char* newName );


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
int PpcManager_GetPpcFromDict( PpcManager* mgr,
                               Stg_ComponentFactory* cf,
                               Name componentName,
                               Dictionary_Entry_Key key,
                               Name defaultName );
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
int PpcManager_GetPpcByName( PpcManager* mgr,
                             Stg_ComponentFactory* cf,
                             char* ppcName );

/*   Returns the 'Ppc' id, for referencing the Ppc later through the
 *   PpcManager. Can handle finding a FeVariable and casting it to a
 *   Ppc_Variable. (tres cool)
 */
int PpcManager_GetField( PpcManager* mgr,
                         Stg_ComponentFactory* cf,
                         Stg_Component* component,
                         Dictionary_Entry_Key key,
                         Bool essential );

#endif

