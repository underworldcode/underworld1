/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**	Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**	Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**	AuScope - http://www.auscope.org
**	Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**	Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**	Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**	Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**	Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
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

#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>

const Type StgFEM_Document_Type = "StgFEM_Document";


void _StgFEM_Document_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
   Codelet*           self = (Codelet*)component;
   AbstractContext*   context;
   ConditionFunction* condFunc;
   Dictionary*        pluginDict = Codelet_GetPluginDictionary( component, cf->rootDict );
   DocumentationComponentFactory* dcf;
   Stream* componentListStream;

   context = (AbstractContext*)Stg_ComponentFactory_ConstructByName(
      cf,
      Dictionary_GetString( pluginDict, (Dictionary_Entry_Key)"Context" ),
      AbstractContext,
      True,
      data );


   /* Print list of all components */
   componentListStream = Journal_Register( Info_Type, (Name)"componentList"  );
   Stream_RedirectFile_WithPrependedPath( componentListStream, context->outputPath, "ComponentList.txt" );
   Stg_ComponentRegister_PrintAllTypes( Stg_ComponentRegister_Get_ComponentRegister(), componentListStream );

   /* Print info for one component */
   dcf = DocumentationComponentFactory_New( );
   Stream_RedirectFile_WithPrependedPath( dcf->infoStream, context->outputPath, "Documentation.txt" );
   DocumentationComponentFactory_DocumentType( dcf, Dictionary_GetString( context->dictionary, (Dictionary_Entry_Key)"documentType" ) );

   exit( EXIT_SUCCESS  );
}

void* _StgFEM_Document_DefaultNew( Name name ) {
   return Codelet_New(
           StgFEM_Document_Type,
      _StgFEM_Document_DefaultNew,
      _StgFEM_Document_AssignFromXML,
      _Codelet_Build,
      _Codelet_Initialise,
      _Codelet_Execute,
      _Codelet_Destroy,
      name );
}

Index StgFEM_Document_Register( PluginsManager* pluginsManager ) {

   return PluginsManager_Submit( pluginsManager, StgFEM_Document_Type, (Name)"0", _StgFEM_Document_DefaultNew );
}
