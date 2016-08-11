#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#include "ViscousCreep.h"

#include <float.h>
#include <math.h>
#include <assert.h>


/* Textual name of this class */
const Type ViscousCreep_Type = "ViscousCreep";

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
ViscousCreep* _ViscousCreep_New(  VISCOUSCREEP_DEFARGS  )
{
   ViscousCreep* self;

   /* Call private constructor of parent */
   assert( _sizeOfSelf >= sizeof(ViscousCreep) );
   self = (ViscousCreep*) _Rheology_New(  RHEOLOGY_PASSARGS  );

   return self;
}

void _ViscousCreep_Build( void* _self, void* data )
{
   ViscousCreep*  self = (ViscousCreep*)_self;

   _Rheology_Build( self, data );

}

void _ViscousCreep_Initialise( void* _self, void* data )
{
   ViscousCreep*  self = (ViscousCreep*)_self;

   _Rheology_Initialise( self, data );
}

void _ViscousCreep_Destroy( void* _self, void* data )
{
   ViscousCreep*  self = (ViscousCreep*)_self;

   _Rheology_Destroy( self, data );
}


void* _ViscousCreep_DefaultNew( Name name )
{
   /* Variables set in this function */
   SizeT                                                     _sizeOfSelf = sizeof(ViscousCreep);
   Type                                                             type = ViscousCreep_Type;
   Stg_Class_DeleteFunction*                                     _delete = _Rheology_Delete;
   Stg_Class_PrintFunction*                                       _print = _Rheology_Print;
   Stg_Class_CopyFunction*                                         _copy = _Rheology_Copy;
   Stg_Component_DefaultConstructorFunction*         _defaultConstructor = _ViscousCreep_DefaultNew;
   Stg_Component_ConstructFunction*                           _construct = _ViscousCreep_AssignFromXML;
   Stg_Component_BuildFunction*                                   _build = _ViscousCreep_Build;
   Stg_Component_InitialiseFunction*                         _initialise = _ViscousCreep_Initialise;
   Stg_Component_ExecuteFunction*                               _execute = _Rheology_Execute;
   Stg_Component_DestroyFunction*                               _destroy = _ViscousCreep_Destroy;
   Rheology_ModifyConstitutiveMatrixFunction*  _modifyConstitutiveMatrix = _ViscousCreep_ModifyConstitutiveMatrix;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return (void*) _ViscousCreep_New(  VISCOUSCREEP_PASSARGS  );
}


/* XML exmaple

   ****
   This component is not thouroughly tested and may break use with caution
   ****

   <struct name="rheology">
		<param name="Type">ViscousCreep</param>
		<param name="StrainRateInvariantField">StrainRateInvariantField</param>
		<param name="TemperatureField">offset_temp</param>
		<param name="PressureField">PressureField</param>
		<param name="GrainSize">1e-3</param>
		<param name="shear_modulus" units="Pa">1</param>
		<param name="burgers_vector" units="m">1</param>
		<param name="DefaultStrainRateInvariant">1</param>
		<param name="enable_limit_eta">False</param>
		<param name="max_limit_eta" units="Pa*s">0</param>
		<param name="min_limit_eta" units="Pa*s">0</param>

		<param name="enable_diffusion">True</param>
		<param name="diffusion_material_constant" units="s^-1">0.5e6</param>
		<param name="diffusion_activation_energy" units="kJ*mol^-1">229.74</param>
		<param name="diffusion_grainsize_exponent">0</param>

		<param name="enable_dislocation">False</param>
		<param name="dislocation_material_constant" units="s^-1">3.5e22</param>
		<param name="dislocation_activation_energy" units="kJ*mol^-1">510</param>
		<param name="dislocation_stress_exponent">3.5</param>
	        <param name="dislocation_grainsize_exponent">0</param>

		<param name="enable_peierls">False</param>
		<param name="peierls_exponent_s">1</param>
		<param name="peierls_exponent_q">1</param>
		<param name="peierls_lower_threshold_pressure" units="Pa">1e8</param>
		<param name="peierls_lower_threshold_temperature" units="K">613</param>
		<param name="peierls_upper_threshold_temperature" units="K">1200</param>
		<param name="peierls_material_constant" units="Pa^-2*s^-1">1e-4</param>
		<param name="peierls_stress" units="Pa">1.5e9</param>

		<!-- UNCOMMENT THIS TO USE THE STRAIN_RATE FORMULATION 
		<param name="peierls_strainrate" units="s^-1">5.7e11</param>
		-->
	</struct>

   */

void _ViscousCreep_AssignFromXML( void* rheology, Stg_ComponentFactory* cf, void* data )
{
   ViscousCreep*  self = (ViscousCreep*)rheology;
   PpcManager *mgr=NULL;

   /* Construct Parent */
   _Rheology_AssignFromXML( self, cf, data );

   mgr = self->mgr;

   Journal_Printf( global_info_stream,
        "*** Warning - you are using the ViscousCreep rheology. This hasn't been well tested and may break your model. If you encounter problems please try use a simpler rheology model\n" );

   self->strainRateInvTag = PpcManager_GetField( self->mgr, cf, (Stg_Component*)self, "StrainRateInvariantField", True );
   self->temperatureLabel = PpcManager_GetField( self->mgr, cf, (Stg_Component*)self, "TemperatureField", True );
   self->pressureLabel = PpcManager_GetField( self->mgr, cf, (Stg_Component*)self, "PressureField", True );
   self->grainSizeLabel = PpcManager_GetField( self->mgr, cf, (Stg_Component*)self,"GrainSize", True );        // grain size variable

   self->A_diff = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"diffusion_material_constant", 1.0 ); // material constant for dislocation creep
   self->m_diff = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"diffusion_grainsize_exponent", 0.0 ); // grain size exponent
   self->E_diff = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"diffusion_activation_energy", 0.0 ); // activation energy for dislocation process
   self->V_diff = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"diffusion_activation_volume", 0.0 );
   self->enable_diffusion = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"enable_diffusion", False ); 

   self->n_dis = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"dislocation_stress_exponent", 1.0 );
   self->A_dis = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"dislocation_material_constant", 1.0 ); // material constant for diffusion creep
   self->E_dis = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"dislocation_activation_energy", 0.0 ); // activation energy for diffusion process
   self->V_dis = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"dislocation_activation_volume", 0.0 );
   self->enable_dislocation = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"enable_dislocation", False ); 

   self->enable_peierls = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"enable_peierls", False ); 
   self->peierls_q = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"peierls_exponent_q", 1 );
   self->peierls_p = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"peierls_exponent_p", 1 );
   self->peierls_lower_threshold_pressure = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"peierls_lower_threshold_pressure", 2e8 );
   self->peierls_lower_threshold_temperature = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"peierls_lower_threshold_temperature", -1 );
   self->peierls_upper_threshold_temperature = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"peierls_upper_threshold_temperature", -1 );
   self->A_p = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"peierls_material_constant", 0 );
   self->stress_peierls = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"peierls_stress", -1 ); // limiting peierls stress
   self->burgers_vector = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"burgers_vector", 1 );
   self->shear_modulus = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"shear_modulus", 1 );
   self->defaultStrainRateInvariant = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"DefaultStrainRateInvariant", -1 );

   // read in limiting parameters
   self->no_peierls_NR = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"no_peierls_NR", False ); 
   self->enable_eta_limit = Stg_ComponentFactory_GetBool( cf, self->name, (Dictionary_Entry_Key)"enable_limit_eta", True ); 
   self->limit_max = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"max_limit_eta", 1e26 );
   self->limit_min = Stg_ComponentFactory_GetDouble( cf, self->name, (Dictionary_Entry_Key)"min_limit_eta", 1e15 );

   Journal_Firewall(  (self->enable_diffusion || self->enable_dislocation ), 
         self->mgr->error_stream,
         "Error, in component %s.\n"
         "Neither the diffusion of dislocation mechanisms are enabled, this is erroneous\n"
         "Add one or both of the following lines to this component to enable mechanisms\n"
         "\n<param name=\"enable_diffusion\">True</param>\n"
         "\n<param name=\"enable_dislocation\">True</param>\n", self->name
         );

   if( self->enable_peierls ) {
      Journal_Firewall( self->stress_peierls > DBL_MIN , self->mgr->error_stream,
            "Error, in component %s.\n"
            "Input values for peierls_stress, it must be set\n"
            "\n<param name=\"peierls_stress\">...</param>\n", self->name
            );
      Rheology_SetToNonLinear( self );
   }

   if( self->enable_dislocation && !self->enable_diffusion && self->defaultStrainRateInvariant == -1 ) {
      // input check: if only dislocation active we need a DefaultStrainRateInvariant parameter
      Journal_Firewall( 0 , self->mgr->error_stream,
            "Error, in component %s.\n"
            "As only the dislocation creep mechanism is enabled you must supply a DefaultStrainRateInvariant for the initial nonlinear iteration\n"
            "\ni.e\t<param name=\"DefaultStrainRateInvariant\">...</param>\n", self->name
	);
   }

   if( self->n_dis > 1 && self->enable_dislocation )
      Rheology_SetToNonLinear( self );

   /* test if extension already exists */
   self->particleExtHandle = ExtensionManager_GetHandle( mgr->materialSwarm->particleExtensionMgr, (Name)"ViscoMechanism" );
   if( self->particleExtHandle == (unsigned)-1 ) {
      MaterialPoint materialPoint;
      int *mechanismExtension;
      /* add the extension for a int */
      self->particleExtHandle = ExtensionManager_Add( mgr->materialSwarm->particleExtensionMgr, "ViscoMechanism", sizeof(int) );
      mechanismExtension = (int*)ExtensionManager_Get( mgr->materialSwarm->particleExtensionMgr, &materialPoint, self->particleExtHandle );

      /* Setup extension variable to analyse dominate viscous mechanism: diffusion, dislocation, peierls */
      self->mechanismType = Swarm_NewScalarVariable( mgr->materialSwarm, (Name)"ViscousCreep_Mechanism", (ArithPointer) mechanismExtension - (ArithPointer) &materialPoint, Variable_DataType_Int );
      LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*)self->mechanismType->variable );
      LiveComponentRegister_Add( LiveComponentRegister_GetLiveComponentRegister(), (Stg_Component*)self->mechanismType );
   } else {
      self->mechanismType = SwarmVariable_Register_GetByName( mgr->materialSwarm->swarmVariable_Register, "ViscousCreep_Mechanism" );
   }
}


void _ViscousCreep_ModifyConstitutiveMatrix( void* _self, ConstitutiveMatrix* constitutiveMatrix, MaterialPointsSwarm* swarm, Element_LocalIndex lElement_I, MaterialPoint* materialPoint, Coord xi )
{
   ViscousCreep* self = (ViscousCreep*) _self;
   int err;
   double eII, P, temp;
   double n_dis, A_diff, A_dis, h, m_diff, E_diff, E_dis, V_diff, V_dis, R, stress_peierls;
   double expFrac_diff, expFrac_dis, p_ltP;
   double eta_dis, eta_diff, eta_eff, b;
   double q,p, shear_modulus, p_utT, p_ltT;
   double tauII, eta_pei, A_p;
   int *mechanismType=NULL, nl_safe;

   // get error report stream
   Stream* error_stream = self->mgr->error_stream;
   assert(error_stream);

   /* stupid way to recover the integration point from the materialPoint
    * TODO: FIXCONSTITUTIVE use integration point only */
   IntegrationPoint* integrationPoint = (IntegrationPoint*) Swarm_ParticleAt( constitutiveMatrix->integrationSwarm,
                                        constitutiveMatrix->currentParticleIndex );

   /** get static parameters from dictionary input - easier names **/
   n_dis = self->n_dis;
   m_diff = self->m_diff;
   stress_peierls = self->stress_peierls;
   q = self->peierls_q;
   p = self->peierls_p;
   b = self->burgers_vector;
   shear_modulus = self->shear_modulus;
   p_ltP = self->peierls_lower_threshold_pressure;
   p_ltT = self->peierls_lower_threshold_temperature;
   p_utT = self->peierls_upper_threshold_temperature;
   A_p = self->A_p;

   A_diff = self->A_diff;
   A_dis = self->A_dis;
   E_diff = self->E_diff;
   E_dis = self->E_dis;
   V_diff = self->V_diff;
   V_dis = self->V_dis;

   /* get the pointer for viscosity mechanism flag */
   mechanismType = (int*)ExtensionManager_Get( swarm->particleExtensionMgr, materialPoint, self->particleExtHandle );
   assert(mechanismType);
   *mechanismType=-1;

   /* get gas constant - a bit magic */
   PpcManager_GetConstant( self->mgr, "R", &R );

   /* get the 2nd invariant of the strain rate */
   nl_safe=0; // turn off non linear bits
   if ( constitutiveMatrix->previousSolutionExists ) {
      err = PpcManager_Get( self->mgr, lElement_I, integrationPoint, self->strainRateInvTag, &eII );
      err = PpcManager_Get( self->mgr, lElement_I, integrationPoint, self->pressureLabel, &P );
      assert(!err);
      nl_safe=1; // safe to enter non linear parts because previous solution exists
   } else {
      if (self->defaultStrainRateInvariant > 0 ) {
         eII = self->defaultStrainRateInvariant; // just to be safe if anything needs it
         P=0;
         nl_safe=1; // safe to enter non linear parts because we assume values for strain rate invariant and pressure (eII, P).
      }
   }

   /* get grain size */
   err = PpcManager_Get( self->mgr, lElement_I, integrationPoint, self->grainSizeLabel, &h );
   if( err ) h = 0;

   /* get temperature */
   err = PpcManager_Get( self->mgr, lElement_I, integrationPoint, self->temperatureLabel, &temp );
   assert( !err );
   temp = Scaling_Unscale(self->mgr->scaling,temp,sTemperature);
   Journal_Firewall( temp>0, error_stream, 
         "temperature at point (%g, %g, %g) is %g."
         "A temperature < 0 is unexpected and an error. Find out why you have negative temperature\n",
         materialPoint->coord[0], materialPoint->coord[1], materialPoint->coord[2], temp );


   // save this fraction for later use in diffusion and peierls
   expFrac_diff = exp( (E_diff + V_diff * P) / (R*temp) );
   double phi = (E_dis+V_dis*P)/(R*temp*n_dis);
   expFrac_dis = exp( phi );

   // max values out now
   eta_eff=-DBL_MAX;
   eta_diff=DBL_MAX;
   eta_dis=DBL_MAX;
   eta_pei=DBL_MAX;

   /********** DIFFUSION CREEP CALCULATION ***********/
   if( self->enable_diffusion ) {
      eta_diff = 0.5 / A_diff;

      // grain size dependency - diffusion creep only
      if( h>-1*DBL_EPSILON )
         eta_diff = eta_diff * pow( b/h, -m_diff );

      // calculate final diffusion viscosiy, where shear_modulus is the mu in Karato and Wu, 1993.
      eta_diff = eta_diff * shear_modulus * expFrac_diff;
      eta_eff = eta_diff;

      // sanity checks
      Journal_Firewall( !isnan(eta_diff), error_stream, "diff viscosity is nan\n" );
      Journal_Firewall( eta_diff>0, error_stream, "diff viscosity is less than 0\n" );

      *mechanismType=1;
   }

   /********** DISLOCATION CREEP CALCULATION ***********/
   if( self->enable_dislocation && nl_safe ) {
      eta_dis = 0.5 * pow( A_dis, -1.0/n_dis );

      // strain rate dependency 
      if( fabs( n_dis - 1.0 ) > 1.0e-5 )
         eta_dis = eta_dis * pow( eII, 1.0/n_dis - 1.0 );

      eta_dis = eta_dis * shear_modulus * expFrac_dis;

      // sanity check eta_dis
      Journal_Firewall( !isnan(eta_dis), error_stream, "dis viscosity is nan\n" );
      Journal_Firewall( eta_dis>0, error_stream, "dis viscosity < zero\n" );
   }

   // record mechanism type
   if( eta_dis < eta_diff ) *mechanismType=2;

   // calculate harmonic average of viscosities
   eta_eff = 1 / ( 1/eta_diff + 1/eta_dis );

   // check if we should apply Peierls mechanism (low temperature, high pressure), we need a pressure solution so make sure previous sol exists
   if( self->enable_peierls && nl_safe && 
        P > p_ltP && ( temp<p_utT && temp>p_ltT ) ) {

      double z1, z2, z3, z4, phi;

      /* calculate a stress inv based on dislocation creep 
         estimate the peierls strain-rate, if it is larger than the given
         strain-rate we must calculate a peierls viscosity
        */ 
      tauII = 2 * eta_dis * eII;

      // clip tauII 
      if(tauII > stress_peierls) 
         tauII = stress_peierls;

      z1 = tauII / stress_peierls;
      z2 = pow( z1, p );
      z3 = 1-z2;
      z4 = pow( z3, q );   // z4 = [ 1 - (tauII/stress_peierls)^p ]^q
      phi = (E_dis + V_dis * P)/(R*temp);

      if( eII < (A_p * tauII * tauII * exp( -phi * z4 )) ) {
         /* old implementation - using no NR  */
         if( self->no_peierls_NR ) {
            eta_pei = 1 / (2 * A_p * tauII) * exp( -phi * z4 ) ;
         } else {

            /* use NR iteration to find a stress_inv that could cause the given strain-rate */
            double f, df_dtau, delta, delta_old;
            int i;

            /* initialise for iterations */
            delta_old = HUGE_VAL;

            for(i=0; i<200; i++ ) { // randomly limit the NR iteration to 200

               // evalutate f
               f = eII - A_p * tauII * tauII * exp( -phi * z4 );

               Journal_Firewall( isnan(f)==0, NULL, 
                     "Error in %s: Blow up in newton-raphson iteration for peierls stress calculation\n");

               // check success condition
               if ( fabs( f/eII ) < 1e-2 ) break;

               // evaluate change in tau
               df_dtau = -A_p * tauII * exp( -phi * z4 ) * ( 2 + phi*q*p*z2*pow(z3, (p-1)) );
               delta = f / df_dtau;

               // check delta not growing, iteration is unstable
               Journal_Firewall( fabs(delta/delta_old) < 10, NULL, 
                     "Error in %s:  Blow up in newton-raphson iteration for peierls stress calculation\n", __func__);
               delta_old = delta;

               // update tau dependent bits
               tauII = tauII - delta;
               z1 = tauII / stress_peierls;
               z2 = pow( z1, p );
               z3 = 1-z2;
               z4 = pow( z3, q );

               if( tauII > stress_peierls ) {
                  printf("tau was greater than yield stress\n");
                  assert(0);
                  break;
               }
            }

            Journal_Firewall( i<199, NULL, "Error in %s: NR iteration didn't converge ... why?\n", __func__ );
         }

         eta_pei = tauII/(2*eII);
      } 

      // sanity check eta_pei
      Journal_Firewall( !isnan(eta_pei), error_stream, "peierls viscosity is nan\n" );
      Journal_Firewall( eta_pei>0, error_stream, "peierls viscosity < zero\n" );

      // harmonic average of mechanisms + record mechanism type
      if( eta_pei < eta_dis ) { 
         *mechanismType=3;
         eta_eff = 1 / ( 1/eta_diff + 1/eta_pei ) ;
      } else {
         eta_eff = 1 / ( 1/eta_diff + 1/eta_dis );
      }
   }

   Journal_Firewall( !isnan(eta_eff), error_stream, "eta_eff viscosity is nan\n" );
   Journal_Firewall( eta_eff>0, error_stream, "eta_eff viscosity < zero\n" );

   // test for clipping eta_eff
   if(self->enable_eta_limit) {
      if( eta_eff > self->limit_max ) eta_eff=self->limit_max;
      if( eta_eff < self->limit_min ) eta_eff=self->limit_min;
   }

   ConstitutiveMatrix_SetIsotropicViscosity( constitutiveMatrix, eta_eff );
}


