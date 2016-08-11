/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**   Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**   Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**   AuScope - http://www.auscope.org
**   Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**   Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
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
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <gLucifer/gLucifer.h>
#include "Toolbox.h"

const Type gLucifer_Toolbox_Type = "gLucifer_Toolbox";

void _gLucifer_Toolbox_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data )  {
}

void* _gLucifer_Toolbox_DefaultNew( Name name ) {
   return Codelet_New(
      gLucifer_Toolbox_Type,
      _gLucifer_Toolbox_DefaultNew,
      _gLucifer_Toolbox_AssignFromXML,
      _Codelet_Build,
      _Codelet_Initialise,
      _Codelet_Execute,
      _Codelet_Destroy,
      name );
}

void gLucifer_Toolbox_Initialise( ToolboxesManager* toolboxesManager, int* argc, char*** argv ) {
   gLucifer_Init( argc, argv );
}

void gLucifer_Toolbox_Finalise( ToolboxesManager* toolboxesManager ) {
   gLucifer_Finalise();
   
   Journal_RPrintf(
      Journal_Register( Info_Type, (Name)gLucifer_Toolbox_Type  ),
         "Finalised: gLucifer (Visualisation Framework) Revision %s.\n", VERSION );
}

Index gLucifer_Toolbox_Register( ToolboxesManager* toolboxesManager ) {
   return ToolboxesManager_Submit( toolboxesManager, gLucifer_Toolbox_Type, (Name)"0", _gLucifer_Toolbox_DefaultNew );
}



