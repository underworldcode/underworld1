/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**      Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**      Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**      Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**      Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**      Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**      Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**
** This file may be distributed under the terms of the VPAC Public License
** as defined by VPAC of Australia and appearing in the file
** LICENSE.VPL included in the packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __Underworld_Utils_RBFManager_h__
#define __Underworld_Utils_RBFManager_h__

      /* Textual name of this class - This is a global pointer which is used for times when you need to refer to class and not a particular instance of a class */
      extern const Type RBFManager_Type;

      #define __RBFParticle \
         __GlobalParticle             \
         double density; 

         typedef struct { __RBFParticle } RBFParticle;

        /* RBFManager information */
        #define __RBFManager                                                                                    \
                /* Macro defining parent goes here - This means you can cast this class as its parent */      \
                __Stg_Component                                                                               \
                /* Virtual Info */                                                                            \
                PICelleratorContext*        context;                                                          \
                double                      particleMass;                                                     \
                Swarm*                      rbfParticleSwarm;                                                 \
                SwarmVariable*              particleDensity;                                                  \
                SwarmVariable*              particleCoordVariable;                                            \
                Dimension_Index             RBFdim;                                                           \
                double                      particleSupportRadius;

        struct RBFManager { __RBFManager };

        /*---------------------------------------------------------------------------------------------------------------------
        ** Constructors
        */

        #ifndef ZERO
        #define ZERO 0
        #endif

        #define RBFMANAGER_DEFARGS \
                STG_COMPONENT_DEFARGS

        #define RBFMANAGER_PASSARGS \
                STG_COMPONENT_PASSARGS

        RBFManager* _RBFManager_New(  RBFMANAGER_DEFARGS  );

        /* Stg_Class_Delete RBFManager implementation */
        void _RBFManager_Delete( void* RBFManager );
        void _RBFManager_Print( void* RBFManager, Stream* stream );
        #define RBFManager_Copy( self ) \
                (RBFManager*) Stg_Class_Copy( self, NULL, False, NULL, NULL )
        #define RBFManager_DeepCopy( self ) \
                (RBFManager*) Stg_Class_Copy( self, NULL, True, NULL, NULL )
        void* _RBFManager_Copy( void* RBFManager, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );

        void* _RBFManager_DefaultNew( Name name ) ;
        void _RBFManager_AssignFromXML( void* shape, Stg_ComponentFactory* cf, void* data ) ;
        void _RBFManager_Build( void* RBFManager, void* data ) ;
        void _RBFManager_Initialise( void* RBFManager, void* data ) ;
        void _RBFManager_Execute( void* RBFManager, void* data );
        void _RBFManager_Destroy( void* RBFManager, void* data ) ;
        void _RBFManager_Init(
            void*                  RBFManager,
            PICelleratorContext*   context,
            Dimension_Index        RBFdim,
            double                 particleSupportRadius,
            Swarm*                 rbfParticleSwarm );
        
        void RBFManager_CalculateParticleDensities( void* RBFManager );
        double RBFManager_2D_CubicSplineKernel( double distance, double h );
        double RBFManager_2D_CubicSplineDerivative( double distance, double projected_distance, double h );
        double RBFManager_3D_CubicSplineKernel( double distance, double h );
        double RBFManager_3D_CubicSplineDerivative( double distance, double projected_distance, double h );

        /*---------------------------------------------------------------------------------------------------------------------
        ** Private functions
        */

        /*---------------------------------------------------------------------------------------------------------------------
        ** Entry Point Hooks
        */

        /*---------------------------------------------------------------------------------------------------------------------
        ** Public functions
        */

#endif

