/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University 
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
** 		* Redistributions of source code must retain the above copyright notice, 
** 			this list of conditions and the following disclaimer.
** 		* Redistributions in binary form must reproduce the above copyright 
**			notice, this list of conditions and the following disclaimer in the 
**			documentation and/or other materials provided with the distribution.
** 		* Neither the name of the Monash University nor the names of its contributors 
**			may be used to endorse or promote products derived from this software 
**			without specific prior written permission.
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
*%  Louis.Moresi - Louis.Moresi@monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
** 
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

const Type JAMBox_ExamplePlugin_Type = "JAMBox_ExamplePlugin";

typedef struct { 
   __Codelet
   Bool someParameter;
} JAMBox_ExamplePlugin;

void _JAMBox_ExamplePlugin_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
   JAMBox_ExamplePlugin* self = (JAMBox_ExamplePlugin*) component;

	self->context = (AbstractContext*)Stg_ComponentFactory_ConstructByName( cf, (Name)"context", AbstractContext, True, data  ); 
   self->someParameter = Stg_ComponentFactory_PluginGetBool( cf, self, (Dictionary_Entry_Key)"SomeParameter", True );
}

void* _JAMBox_ExamplePlugin_DefaultNew( Name name ) {
	/* Variables set in this function */
   SizeT                                              _sizeOfSelf = sizeof( JAMBox_ExamplePlugin );
   Type                                                      type = JAMBox_ExamplePlugin_Type;
   Stg_Class_DeleteFunction*                              _delete = _Codelet_Delete;
   Stg_Class_PrintFunction*                                _print = _Codelet_Print;
   Stg_Class_CopyFunction*                                  _copy = _Codelet_Copy;
   Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _JAMBox_ExamplePlugin_DefaultNew;
   Stg_Component_ConstructFunction*                    _construct = _JAMBox_ExamplePlugin_AssignFromXML;
   Stg_Component_BuildFunction*                            _build = _Codelet_Build;
   Stg_Component_InitialiseFunction*                  _initialise = _Codelet_Initialise;
   Stg_Component_ExecuteFunction*                        _execute = _Codelet_Execute;
   Stg_Component_DestroyFunction*                        _destroy = _Codelet_Destroy;

   /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
   AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

   return _Codelet_New(  CODELET_PASSARGS  );
}

Index JAMBox_ExamplePlugin_Register( PluginsManager* pluginsManager ) {
   return PluginsManager_Submit( pluginsManager, JAMBox_ExamplePlugin_Type, (Name)"0", _JAMBox_ExamplePlugin_DefaultNew  );
}


