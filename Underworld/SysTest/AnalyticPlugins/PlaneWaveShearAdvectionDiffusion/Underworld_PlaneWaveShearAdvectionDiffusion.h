/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
**       * Redistributions of source code must retain the above copyright notice,
**          this list of conditions and the following disclaimer.
**       * Redistributions in binary form must reproduce the above copyright
**         notice, this list of conditions and the following disclaimer in the
**         documentation and/or other materials provided with the distribution.
**       * Neither the name of the Monash University nor the names of its contributors
**         may be used to endorse or promote products derived from this software
**         without specific prior written permission.
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
*%  Louis Moresi - Louis.Moresi@monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

#ifndef __Underworld_PlaneWaveShearAdvectionDiffusion_h__
#define __Underworld_PlaneWaveShearAdvectionDiffusion_h__

extern const Type Underworld_PlaneWaveShearAdvectionDiffusion_Type;

typedef struct {
   __Codelet;
   double kx; /* Input parameters: horizontal wavenumber  */
   double ky; /* Input parameters: vertical wavenumber  */
   double Pe; /* Input parameters: peclet number */
   double dt; /* Input parameters: timestep size */
} Underworld_PlaneWaveShearAdvectionDiffusion;

Index Underworld_PlaneWaveShearAdvectionDiffusion_Register( PluginsManager* pluginsManager );

void* _Underworld_PlaneWaveShearAdvectionDiffusion_DefaultNew( Name name );

void _Underworld_PlaneWaveShearAdvectionDiffusion_AssignFromXML( void* analyticSolution, Stg_ComponentFactory* cf, void* data );

void _Underworld_PlaneWaveShearAdvectionDiffusion_Init( Underworld_PlaneWaveShearAdvectionDiffusion* self, double kx, double ky, double Pe, double dt );

void Underworld_PlaneWaveShearAdvectionDiffusion_TemperatureFunction( void* _context, double* coord, double* temp );

void _PlaneWaveShearAdvectionDiffusion( double* pos, double time, double* temp );

void Underworld_PlaneWaveShearAdvectionDiffusion_InitTemperature( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result );
void Underworld_PlaneWaveShearAdvectionDiffusion_Temperature( Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result );
void Underworld_PlaneWaveShearAdvectionDiffusion_ShearVelocityX(  Node_LocalIndex node_lI, Variable_Index var_I, void* _context, void* data, void* _result );

double _PlaneWaveShearAdvectionDiffusion_Solution(double x, double y, double t);

double Underworld_PlaneWaveShearAdvectionDiffusion_dt( void* _class, PICelleratorContext* context );
#endif
