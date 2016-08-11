/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Monash Cluster Computing, Australia
** (C) 2003-2004 All Rights Reserved
**
** Primary Authors:
** 
** implemetation of the melting model of de Smet, van den Berg and Vlaar, Lithos, vol 48, 153-170, 1999
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>

#include "types.h"
#include "ParticleMelting.h"
#include <assert.h>

const Type ParticleMelting_Type = "ParticleMelting";

ParticleMelting* _ParticleMelting_New(  PARTICLEMELTING_DEFARGS  )
{
	ParticleMelting*		self;
	
	/* Call private constructor of parent - this will set virtual functions of parent and continue up the hierarchy tree. At the beginning of the tree it will allocate memory of the size of object and initialise all the memory to zero. */
	assert( _sizeOfSelf >= sizeof(ParticleMelting) );
	/* The following terms are parameters that have been passed into this function but are being set before being passed onto the parent */
	/* This means that any values of these parameters that are passed into this function are not passed onto the parent function
	   and so should be set to ZERO in any children of this class. */
	nameAllocationType = NON_GLOBAL;

	self = (ParticleMelting*) _Stg_Component_New(  STG_COMPONENT_PASSARGS  );

	return self;
}

void _ParticleMelting_Init( 
   ParticleMelting* self,
   Bool extractMelt,
   FeVariable* temperatureField,
   FeVariable* pressureField,
   MaterialPointsSwarm* materialSwarm,
   Scaling* scaling ) {

	ParticleMelting_ParExt *parExt = NULL;
	StandardParticle particle;

   /* extractMelt:
    * For mantle set to true, according to de Smet, 98
    * For lithosphere set to false according to Patrice Rey
    */
   self->extractMelt              = extractMelt;
   self->temperatureField         = temperatureField;
   self->pressureField            = pressureField;
   self->materialSwarm = materialSwarm;
   self->scaling = scaling;

   /* add to EP, post time step, to save the previous value of melting depletion */
	/*   EntryPoint_AppendClassHook( 
      AbstractContext_GetEntryPoint(self->context, AbstractContext_EP_UpdateClass), 
      "MeltDepletionUpdate", 
      ParticleMelting_MeltFrationUpdate, 
      self->name, 
      self );
	*/

   /*
   EntryPoint_Append
   AbstractContext_EP_UpdateClass
   */

	/* now we extend the particles with 3 doubles: F, prevF and maxTheta */	
	self->particleExtHandle = ExtensionManager_Add( self->materialSwarm->particleExtensionMgr, (Name)self->type, sizeof(ParticleMelting_ParExt) );
	parExt = ExtensionManager_Get( self->materialSwarm->particleExtensionMgr, &particle, self->particleExtHandle );

	self->meltVariable = Swarm_NewScalarVariable( self->materialSwarm, (Name)"Melt", (ArithPointer) &parExt->melt - (ArithPointer) &particle, Variable_DataType_Double );

	self->FVariable = Swarm_NewScalarVariable( self->materialSwarm, (Name)"Depletion", (ArithPointer) &parExt->F - (ArithPointer) &particle, Variable_DataType_Double );
	  
	self->prevFVariable = Swarm_NewScalarVariable( self->materialSwarm, (Name)"PrevDepletion", (ArithPointer) &parExt->prevF - (ArithPointer) &particle, Variable_DataType_Double );

	self->maxThetaVariable = Swarm_NewScalarVariable( self->materialSwarm, (Name)"MaxDegreeOfMelting", (ArithPointer) &parExt->maxTheta - (ArithPointer) &particle, Variable_DataType_Double );

}

void ParticleMelting_MeltFrationUpdate( ParticleMelting *self, void *data ) {
   MaterialPoint *mat_part=NULL;
   ExtensionManager *part_extMgr=NULL;
   MaterialPointsSwarm *mat_swarm=NULL;
   ParticleMelting_ParExt *parExt=NULL;
   ParticleMelting_MatExt *matExt=NULL;
   FeVariable *temperatureField=self->temperatureField;
   FeVariable *pressureField=self->pressureField;
	FeMesh* mesh = temperatureField->feMesh;
   Scaling *scaling=self->scaling;
	double depth, min[3], max[3]; 
   double temperature, pressure, deepPressure, theta;
   unsigned lpartCount, par_i, part_extHandle, mat_extHandle;

	Mesh_GetGlobalCoordRange( mesh, min, max );

   mat_swarm = self->materialSwarm;
   part_extMgr = mat_swarm->particleExtensionMgr;
   lpartCount = self->materialSwarm->particleLocalCount;
   part_extHandle = self->particleExtHandle;
   mat_extHandle = self->materialExtHandle;

   for( par_i = 0 ; par_i < lpartCount ; par_i++  ) {
      mat_part = (MaterialPoint*)Swarm_ParticleAt( mat_swarm, par_i );

      /* get the particle extension */
      parExt = ExtensionManager_Get( part_extMgr, mat_part, part_extHandle );

      /* get the material extension */
      matExt = MaterialPointsSwarm_GetMaterialExtensionOn( mat_swarm, mat_part, mat_extHandle );
      if( !matExt->TurnMeltOn ) continue;

		/* Get temperature*/
      _FeVariable_InterpolateValueAt( temperatureField, mat_part->coord, &temperature );
		/* Get pressure*/
		if( pressureField )
		  _FeVariable_InterpolateValueAt( pressureField, mat_part->coord, &pressure );
		else {
		  depth = fabs( max[J_AXIS] - mat_part->coord[J_AXIS] );
		  pressure = depth * self->refRho * self->gravity;
		}
		/* Get deep pressure */
		deepPressure = matExt->deepPressure;

		/* Scale all  */
		if( self->scaling ) {
		  temperature = Scaling_Unscale( scaling, temperature, sTemperature );
        pressure = Scaling_Unscale( scaling, pressure, sPressure );
		  /* Safety checks - TODO: put some warnings here */
		  if( temperature < 0 )
			 temperature = 0;
		  if( pressure < 0 )
			 pressure = 0;
		  /*deepPressure = Scaling_Unscale( scaling, matExt->deepPressure, sPressure );*/
		  /* pressure in GPa */
		  pressure /= 1e9;
		  deepPressure /= 1e9;
		} 

      /* Calculate Depletion */
      parExt->prevF = parExt->F;

		if( pressure < deepPressure )
		  theta = ParticleMelting_CalculateSuperSolidusTemperature( pressure, temperature, matExt->sCoeff, matExt->lCoeff );
		else
		  theta = ParticleMelting_CalculateSuperSolidusTemperature( pressure, temperature, matExt->sCoeffDeep, matExt->lCoeffDeep );
		
      if( self->extractMelt == True ) {
         /* F is depletion so it cannot decrease! 
          * Once some melt has been produced and removed, 
          * the residual rock is depleted forever and ever, amen */
         if( theta > parExt->maxTheta ) {
           parExt->F = ParticleMelting_CalculateMeltFraction( theta );

           parExt->melt = parExt->F  -  ParticleMelting_CalculateMeltFraction( parExt->maxTheta );

           parExt->maxTheta = theta;
         } else {
           parExt->melt = 0;
         }		
      } else {

         /* melt isn't extracted so depletion can decrease */
         parExt->F = ParticleMelting_CalculateMeltFraction( theta );

         parExt->melt = parExt->F - parExt->prevF; 
      }

   }

}

/* --- Virtual Function Implementations --- */
void _ParticleMelting_Delete( void* _self ) {
	ParticleMelting* self = (ParticleMelting*) _self;

	_Stg_Component_Delete( self );
}

void _ParticleMelting_Print( void* _self, Stream* stream ) {
}

void* _ParticleMelting_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(ParticleMelting);
	Type                                                      type = ParticleMelting_Type;
	Stg_Class_DeleteFunction*                              _delete = _ParticleMelting_Delete;
	Stg_Class_PrintFunction*                                _print = _ParticleMelting_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _ParticleMelting_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _ParticleMelting_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _ParticleMelting_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _ParticleMelting_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _ParticleMelting_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _ParticleMelting_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return (void*) _ParticleMelting_New(  PARTICLEMELTING_PASSARGS  );
}

void _ParticleMelting_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ){
	ParticleMelting*          self              = (ParticleMelting*) _self;

	self->context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", PICelleratorContext, False, data );
	if( !self->context  )
		self->context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", PICelleratorContext, True, data  );

	_ParticleMelting_Init( 
			self,
			Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"ExtractMelt", False  ),
			Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"TemperatureField", FeVariable, True, data  ),
			Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"PressureField", FeVariable, False, data  ),
			Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"MaterialPointsSwarm", MaterialPointsSwarm, True, data),
			Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Scaling", Scaling, True, data) );
}

void _ParticleMelting_Build( void* _self, void* data ) {
	ParticleMelting* self = (ParticleMelting*) _self;
	Materials_Register* matReg = self->materialSwarm->materials_Register;
	ParticleMelting_MatExt *matExt = NULL;
	unsigned materialCount, mat_i;
	Material* material = NULL;

	Stg_Component_Build( self->temperatureField, data, False );
	Stg_Component_Build( self->materialSwarm, data, False );
	if( self->pressureField )
	  Stg_Component_Build( self->pressureField, data, False );
	if( self->scaling ) {
	  Stg_Component_Build( self->scaling, data, False );
	  self->refRho = Scaling_Scale( self->scaling, 3400, sDensity );
	  self->gravity = Scaling_Scale( self->scaling, 9.81, sAcceleration );
	} else {
	  self->refRho = 1.0;
	  self->gravity = 1.0;
	}

	/* now we extend the materials with ParticleMelting_MatExt */
	self->materialExtHandle = Materials_Register_AddMaterialExtension( matReg, self->type, sizeof(ParticleMelting_MatExt) );

	materialCount = Materials_Register_GetCount( matReg );
	for( mat_i = 0 ; mat_i < materialCount ; mat_i++ ) {
		material = Materials_Register_GetByIndex( matReg, mat_i );
		matExt = ExtensionManager_Get( material->extensionMgr, material, self->materialExtHandle );

		/* get latent heat and specific heat from the material dictionary */
		matExt->lhf = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"latentHeat_Fusion", 0.0  );
		matExt->Cp = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"Cp", 1.0  );

		matExt->TurnMeltOn = Dictionary_GetBool_WithDefault( material->dictionary, (Dictionary_Entry_Key)"TurnMeltOn", 0.0  );

      /* read in values from xml file for solidus polynomial */
		matExt->sCoeff[0] = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"solidus_coeff0", 0.0  );
		matExt->sCoeff[1] = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"solidus_coeff1", 0.0  );
		matExt->sCoeff[2] = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"solidus_coeff2", 0.0  );
		matExt->sCoeff[3] = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"solidus_coeff3", 0.0  );

      /* read in values from xml file for liquidus polynomial */
		matExt->lCoeff[0] = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"liquidus_coeff0", 0.0  );
		matExt->lCoeff[1] = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"liquidus_coeff1", 0.0  );
		matExt->lCoeff[2] = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"liquidus_coeff2", 0.0  );
		matExt->lCoeff[3] = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"liquidus_coeff3", 0.0  );

		matExt->deepPressure = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"deepPressure", 0.0  );

      /* read in values from xml file for solidus polynomial (deep)*/
		matExt->sCoeffDeep[0] = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"solidus_coeff0_deep", matExt->sCoeff[0]  );
		matExt->sCoeffDeep[1] = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"solidus_coeff1_deep", matExt->sCoeff[1]  );
		matExt->sCoeffDeep[2] = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"solidus_coeff2_deep", matExt->sCoeff[2]  );
		matExt->sCoeffDeep[3] = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"solidus_coeff3_deep", matExt->sCoeff[3]  );

      /* read in values from xml file for liquidus polynomial */
		matExt->lCoeffDeep[0] = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"liquidus_coeff0_deep", matExt->lCoeff[0]  );
		matExt->lCoeffDeep[1] = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"liquidus_coeff1_deep", matExt->lCoeff[1]  );
		matExt->lCoeffDeep[2] = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"liquidus_coeff2_deep", matExt->lCoeff[2]  );
		matExt->lCoeffDeep[3] = Dictionary_GetDouble_WithDefault( material->dictionary, (Dictionary_Entry_Key)"liquidus_coeff3_deep", matExt->lCoeff[3]  );
	}

   EP_PrependClassHook_AlwaysFirst( 
         Context_GetEntryPoint( self->context, AbstractContext_EP_UpdateClass ),
         ParticleMelting_MeltFrationUpdate, 
         self );

   /*
	EP_InsertClassHookBefore( 
		Context_GetEntryPoint( self->context, AbstractContext_EP_UpdateClass ), 
		"_ParticleFeVariable_Execute",
		ParticleMelting_MeltFrationUpdate, 
		self );
      */
}

void _ParticleMelting_Initialise( void* _self, void* data ) {
	ParticleMelting* self = (ParticleMelting*) _self;
   unsigned par_i, lpartCount;
   MaterialPoint *particle;
   ParticleMelting_ParExt *parExt;

	Stg_Component_Initialise( self->temperatureField, data, False );
	Stg_Component_Initialise( self->materialSwarm, data, False );
	if( self->pressureField )
	  Stg_Component_Initialise( self->pressureField, data, False );
	if( self->scaling )
	  Stg_Component_Initialise( self->scaling, data, False );

	
	if( !self->context->loadFromCheckPoint ) {

	  lpartCount = self->materialSwarm->particleLocalCount;
	  
	  for( par_i = 0 ; par_i < lpartCount ; par_i++  ) {
		 particle = (MaterialPoint*)Swarm_ParticleAt( self->materialSwarm, par_i );
		 parExt = ExtensionManager_Get( self->materialSwarm->particleExtensionMgr, particle, self->particleExtHandle );
		 
		 /* initialise all */
		 parExt->melt = 0;
		 parExt->F = 0;
		 parExt->prevF = 0;
		 parExt->maxTheta = 0;
	  }
	}
}

void _ParticleMelting_Execute( void* _self, void* data ) {
}

void _ParticleMelting_Destroy( void* _self, void* data ) {
   ParticleMelting* self = (ParticleMelting*) _self;

	Stg_Component_Destroy( self->temperatureField, data, False );
	Stg_Component_Destroy( self->materialSwarm, data, False );
	if( self->pressureField )
	  Stg_Component_Destroy( self->pressureField, data, False );
	if( self->scaling )
	  Stg_Component_Destroy( self->scaling, data, False );
}

double ParticleMelting_CalculateSuperSolidusTemperature( double p , double t, double *sc, double *lc ) {
   double solidus, liquidus, ss;

   /* evaluate polynomials for solidus temperature */
   solidus = sc[0] + sc[1]*p + sc[2]*p*p + sc[3]*p*p*p;

   /* evaluate polynomials for liquidus temperature */
   liquidus = lc[0] + lc[1]*p + lc[2]*p*p + lc[3]*p*p*p;

   if( t <= solidus ) 
	  ss = 0;
   else if( t >= liquidus ) 
	  ss = 1;
	else {   
	  if( fabs(liquidus-solidus) < 1e-15 )  
		 assert(0);
	  ss = ( t - solidus ) / (liquidus - solidus );
	}

	return ss;
}

double ParticleMelting_CalculateMeltFraction( double ss  ) {
  double tmp, frac;

  if( ss <= 0 )
	 return 0;

  if( ss >= 1 )
	 return 1;

   /* calculate and return melt fraction (after McKenzie and Bickle, 1988) */
   ss = ss - 0.5;
   tmp = 0.5 + ss + (ss*ss - 0.25)*(0.4256 + 2.988*ss);

   if( tmp > 1 ) 
	  frac = 1;
   else if ( tmp < 0 ) 
	  frac = 0;
   else frac = tmp;

   return frac;
}


int ParticleMelting_Get( void* _self, MaterialPoint* mp, ParticleMeltingProps prop, double *value )
{
   ParticleMelting* self = (ParticleMelting*) _self;
   ParticleMelting_MatExt *matExt;
   ParticleMelting_ParExt *parExt;

   /* Get the material of the particle */ 
   Material *material = Materials_Register_GetByIndex( self->materialSwarm->materials_Register, mp->materialIndex );

   switch( prop ) {
      case MELT_LATENTHEAT_FUSION:
         matExt = ExtensionManager_Get( material->extensionMgr, material, self->materialExtHandle );
         *value = matExt->lhf;
         break;
      case MELT_HEATCAPACITY:
         matExt = ExtensionManager_Get( material->extensionMgr, material, self->materialExtHandle );
         *value = matExt->Cp;
         break;
      case MELT_FRACTION:
         parExt = ExtensionManager_Get( self->materialSwarm->particleExtensionMgr, mp, self->particleExtHandle );
         *value = parExt->melt;
         break;
      case MELT_DEPLETION:
         parExt = ExtensionManager_Get( self->materialSwarm->particleExtensionMgr, mp, self->particleExtHandle );
         *value = parExt->F;
         break;
      case MELT_PREVDEPLETION:
         parExt = ExtensionManager_Get( self->materialSwarm->particleExtensionMgr, mp, self->particleExtHandle );
         *value = parExt->prevF;
         break;
      case MELT_ON:
         matExt = ExtensionManager_Get( material->extensionMgr, material, self->materialExtHandle );
         *value = matExt->TurnMeltOn;
         break;
      case MELT_SOLIDUS_POLY:
         matExt = ExtensionManager_Get( material->extensionMgr, material, self->materialExtHandle );
         value = matExt->sCoeff;
         break;
      case MELT_LIQUIDUS_POLY:
         matExt = ExtensionManager_Get( material->extensionMgr, material, self->materialExtHandle );
         value = matExt->lCoeff;
         break;
      default:
         printf("Error in %s; no valid property argument passed in\n", __func__ );
         return -1;
   }
   return 0;
}
