/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
** Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
** Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
** AuScope - http://www.auscope.org
** Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
** Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
** Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
** Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
** Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
** David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
** Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
** Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
** Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
** Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
** Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
** Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
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
*/


#ifndef __Geothermal_Base_GeothermalAdvDiffResidualForceTerm_h__
#define __Geothermal_Base_GeothermalAdvDiffResidualForceTerm_h__

   #define ISQRT15 0.25819888974716112567

   /** Textual name of this class */
   extern const Type GeothermalAdvDiffResidualForceTerm_Type;

   /** GeothermalAdvDiffResidualForceTerm class contents */
   #define __GeothermalAdvDiffResidualForceTerm \
      /* General info */ \
      __AdvDiffResidualForceTerm \
      \
      double                                                densityFluid; \
      double                                                densitySolid; \
      double                                                specificHeatFluid; \
      double                                                specificHeatSolid; \
      double                                                thermalConductivityFluid;  \
      double                                                thermalConductivitySolid;  \
      double                                                porosity;          \
      double                                                specificVelocity;  \
      BuoyancyMaterial*                                     diffMaterial;

   struct GeothermalAdvDiffResidualForceTerm { __GeothermalAdvDiffResidualForceTerm };

   #ifndef ZERO
   #define ZERO 0
   #endif

   #define GEOTHERMALADVDIFFRESIDUALFORCETERM_DEFARGS \
                ADVDIFFRESIDUALFORCETERM_DEFARGS

   #define GEOTHERMALADVDIFFRESIDUALFORCETERM_PASSARGS \
                ADVDIFFRESIDUALFORCETERM_PASSARGS

   GeothermalAdvDiffResidualForceTerm* _GeothermalAdvDiffResidualForceTerm_New(  ADVDIFFRESIDUALFORCETERM_DEFARGS  );

   void _GeothermalAdvDiffResidualForceTerm_Init(
      void*                                        self,
      double                                  densityFluid,
      double                                  densitySolid,
      double                                  specificHeatFluid,
      double                                  specificHeatSolid,
      double                                  thermalConductivityFluid,
      double                                  thermalConductivitySolid,
      double                                  porosity,
      BuoyancyMaterial*                       diffMaterial,
      Bool                                    specificVelocity );

   void* _GeothermalAdvDiffResidualForceTerm_DefaultNew( Name name ) ;
   void  _GeothermalAdvDiffResidualForceTerm_AssignFromXML( void* residual, Stg_ComponentFactory* cf, void* data ) ;
   void  _GeothermalAdvDiffResidualForceTerm_Build( void* residual, void* data ) ;
   void  _GeothermalAdvDiffResidualForceTerm_Initialise( void* residual, void* data ) ;
   void  _GeothermalAdvDiffResidualForceTerm_Execute( void* residual, void* data ) ;
   void  _GeothermalAdvDiffResidualForceTerm_Destroy( void* residual, void* data ) ;
   void  _GeothermalAdvDiffResidualForceTerm_AssembleElement( void* forceTerm, ForceVector* forceVector, Element_LocalIndex lElement_I, double* elementResidual );
   double _GeothermalAdvDiffResidualForceTerm_UpwindDiffusivity( 
         GeothermalAdvDiffResidualForceTerm* self, 
         AdvectionDiffusionSLE* sle, 
         Swarm* swarm, 
         FeMesh* mesh, 
         Element_LocalIndex lElement_I, 
         Dimension_Index dim );

#endif

