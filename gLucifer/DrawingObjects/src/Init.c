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
*%  Owen Kaluza - Owen.Kaluza(at)monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <gLucifer/Base/Base.h>

#include "DrawingObjects.h"

/* Alias placeholders */
const Type lucSwarmSquares_Type = "lucSwarmSquares";
Dictionary* lucSwarmSquares_MetaAsDictionary() {return Dictionary_New();}
Dictionary* lucSwarmSquares_Type_MetaAsDictionary() {return lucSwarmSquares_MetaAsDictionary();}

Bool lucDrawingObjects_Init()
{
   Stg_ComponentRegister* componentRegister = Stg_ComponentRegister_Get_ComponentRegister();

   Journal_Printf( Journal_Register( Debug_Type, (Name)"Context"  ), "In: %s\n", __func__ ); /* DO NOT CHANGE OR REMOVE */

   Stg_ComponentRegister_Add( componentRegister, lucCapture_Type, (Name)"0", _lucCapture_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucColourBar_Type, (Name)"0", _lucColourBar_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucFieldVariableBorder_Type, (Name)"0", _lucFieldVariableBorder_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucFieldSampler_Type, (Name)"0", _lucFieldSampler_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucMeshSampler_Type, (Name)"0", _lucMeshSampler_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucIsosurface_Type, (Name)"0", _lucIsosurface_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucIsosurfaceCrossSection_Type, (Name)"0", _lucIsosurfaceCrossSection_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucCrossSection_Type, (Name)"0", _lucCrossSection_DefaultNew );
   Stg_ComponentRegister_Add( componentRegister, lucMeshCrossSection_Type, (Name)"0", _lucMeshCrossSection_DefaultNew );
   Stg_ComponentRegister_Add( componentRegister, lucScalarFieldCrossSection_Type, (Name)"0", _lucScalarFieldCrossSection_DefaultNew );
   Stg_ComponentRegister_Add( componentRegister, lucScalarField_Type, (Name)"0", _lucScalarField_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucVectorArrowCrossSection_Type, (Name)"0", _lucVectorArrowCrossSection_DefaultNew );
   Stg_ComponentRegister_Add( componentRegister, lucVectorArrows_Type, (Name)"0", _lucVectorArrows_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucTextureMap_Type, (Name)"0", _lucTextureMap_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucContour_Type, (Name)"0", _lucContour_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucContourCrossSection_Type, (Name)"0", _lucContourCrossSection_DefaultNew );
   Stg_ComponentRegister_Add( componentRegister, lucFeVariableSurface_Type, (Name)"0", _lucFeVariableSurface_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucSwarmViewer_Type, (Name)"0", _lucSwarmViewer_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucSwarmVectors_Type, (Name)"0", _lucSwarmVectors_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucSwarmShapes_Type, (Name)"0", _lucSwarmShapes_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucSwarmSquares_Type, (Name)"0", _lucSwarmShapes_DefaultNew  );  /* Alias for back compat */
   Stg_ComponentRegister_Add( componentRegister, lucHistoricalSwarmTrajectory_Type, (Name)"0", _lucHistoricalSwarmTrajectory_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucEigenvectorsCrossSection_Type, (Name)"0", _lucEigenvectorsCrossSection_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucEigenvectors_Type, (Name)"0", _lucEigenvectors_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucSwarmRGBColourViewer_Type, (Name)"0", _lucSwarmRGBColourViewer_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucMeshViewer_Type, (Name)"0", _lucMeshViewer_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucPlot_Type, (Name)"0", _lucPlot_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucTitle_Type, (Name)"0", _lucTitle_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucAxis_Type, (Name)"0", _lucAxis_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucTimeStep_Type, (Name)"0", _lucTimeStep_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucScalarFieldOnMeshCrossSection_Type, (Name)"0", _lucScalarFieldOnMeshCrossSection_DefaultNew );
   Stg_ComponentRegister_Add( componentRegister, lucScalarFieldOnMesh_Type, (Name)"0", _lucScalarFieldOnMesh_DefaultNew );
   Stg_ComponentRegister_Add( componentRegister, lucVectorArrowMeshCrossSection_Type, (Name)"0", _lucVectorArrowMeshCrossSection_DefaultNew );
   Stg_ComponentRegister_Add( componentRegister, lucVectorArrowsOnMesh_Type, (Name)"0", _lucVectorArrowsOnMesh_DefaultNew  );


   /* Register Parents for type checking */
   RegisterParent( lucCrossSection_Type,                    lucDrawingObject_Type );
   RegisterParent( lucContourCrossSection_Type,             lucCrossSection_Type );
   RegisterParent( lucIsosurfaceCrossSection_Type,          lucCrossSection_Type );
   RegisterParent( lucScalarFieldCrossSection_Type,         lucCrossSection_Type );
   RegisterParent( lucMeshCrossSection_Type,                lucCrossSection_Type );
   RegisterParent( lucVectorArrowCrossSection_Type,         lucCrossSection_Type );
   RegisterParent( lucEigenvectorsCrossSection_Type,        lucCrossSection_Type );
   RegisterParent( lucVectorArrowMeshCrossSection_Type,     lucMeshCrossSection_Type );
   RegisterParent( lucScalarFieldOnMeshCrossSection_Type,   lucMeshCrossSection_Type );
   RegisterParent( lucContour_Type,                         lucContourCrossSection_Type );
   RegisterParent( lucScalarField_Type,                     lucScalarFieldCrossSection_Type );
   RegisterParent( lucScalarFieldOnMesh_Type,               lucScalarFieldOnMeshCrossSection_Type );
   RegisterParent( lucVectorArrows_Type,                    lucVectorArrowCrossSection_Type );
   RegisterParent( lucVectorArrowsOnMesh_Type,              lucVectorArrowMeshCrossSection_Type );
   RegisterParent( lucEigenvectors_Type,                    lucEigenvectorsCrossSection_Type );
   RegisterParent( lucCapture_Type,                         lucDrawingObject_Type );
   RegisterParent( lucColourBar_Type,                       lucDrawingObject_Type );
   RegisterParent( lucFieldVariableBorder_Type,             lucDrawingObject_Type );
   RegisterParent( lucFieldSampler_Type,                    lucCrossSection_Type );
   RegisterParent( lucMeshSampler_Type,                     lucMeshCrossSection_Type );
   RegisterParent( lucIsosurface_Type,                      lucDrawingObject_Type );
   RegisterParent( lucTextureMap_Type,                      lucDrawingObject_Type );
   RegisterParent( lucFeVariableSurface_Type,               lucMeshCrossSection_Type );
   RegisterParent( lucSwarmViewer_Type,               lucDrawingObject_Type );
   RegisterParent( lucSwarmVectors_Type,              lucSwarmViewer_Type );
   RegisterParent( lucSwarmShapes_Type,               lucSwarmViewer_Type );
   RegisterParent( lucHistoricalSwarmTrajectory_Type, lucDrawingObject_Type );
   RegisterParent( lucSwarmRGBColourViewer_Type,      lucSwarmViewer_Type );
   RegisterParent( lucMeshViewer_Type,                lucDrawingObject_Type );
   RegisterParent( lucPlot_Type,                      lucDrawingObject_Type );
   RegisterParent( lucTitle_Type,                     lucDrawingObject_Type );
   RegisterParent( lucAxis_Type,                      lucDrawingObject_Type );
   RegisterParent( lucTimeStep_Type,                  lucDrawingObject_Type );




   return True;
}


