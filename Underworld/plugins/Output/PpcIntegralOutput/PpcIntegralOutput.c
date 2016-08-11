/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (c) 2005-2010, Monash University, Building 28, Monash University Clayton Campus,
**	Victoria, 3800, Australia
**
** Primary Contributing Organisations:
**	Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
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
/** \file 
\details Prints out the value calculated by the PpcIntegral(s) to a file ("PpcIntegrals.dat"). Can add values to file if restarting from a checkpoint.  
**/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>
#include <assert.h>

const Type Underworld_PpcIntegralOutput_Type = "Underworld_PpcIntegralOutput";
static Bool PpcIntegralOutputBeenHere;
static Stream* PpcIntegralOutputStream;

void PpcIntegralOutput( UnderworldContext* context ) {
   static unsigned     ppcIntegralCount;
   static int          ppcIntegralIndex[30];
   unsigned     ii;
   
	if (!PpcIntegralOutputBeenHere ) {
		Name                 filename;
      Bool                 fileOpened;
      Stream*              errorStream  = Journal_Register( Error_Type, (Name)CURR_MODULE_NAME  );
      PpcIntegralOutputStream = Journal_Register( Info_Type, (Name)"PpcIntegralOutput" );

		/* Set up stream */
		Stg_asprintf( &filename, "PpcIntegrals.dat" );
      /* Open File */
      if ( context->rank == 0 ) {
         if ( context->loadFromCheckPoint == False ) {
            /* Always overwrite the file if starting a new run */
            fileOpened = Stream_RedirectFile_WithPrependedPath( PpcIntegralOutputStream, context->outputPath, filename );
         } else {
            /* Just append to the file if doing a restart from checkpoint */
            fileOpened = Stream_AppendFile_WithPrependedPath( PpcIntegralOutputStream, context->outputPath, filename );
         }
         Journal_Firewall( fileOpened, errorStream, 
               "Could not open file %s/%s. Possibly directory %s does not exist or is not writable.\n"
               "Check 'outputPath' in input file.\n", context->outputPath, filename, context->outputPath );
      }
		Memory_Free( filename );
		Stream_SetAutoFlush( PpcIntegralOutputStream, True );
      /* get all PpcIntegral components */
      ppcIntegralCount=0;
      for(ii=0; ii< LiveComponentRegister_GetCount( context->CF->LCRegister ); ii++){
         if( Stg_Class_IsInstance( LiveComponentRegister_At( context->CF->LCRegister, ii ), PpcIntegral_Type) ){
            ppcIntegralIndex[ppcIntegralCount] = ii;
            ppcIntegralCount++;
         }
      }

		/* Print header to stream */
      if(context->loadFromCheckPoint == False) {
		   Journal_Printf( PpcIntegralOutputStream, 
			   	"#       Timestep            Time");
         for(ii=0; ii< ppcIntegralCount; ii++) {
            Stg_Component* comp = LiveComponentRegister_At( context->CF->LCRegister, ppcIntegralIndex[ii] );
            Journal_Printf( PpcIntegralOutputStream, "%16s", comp->name );
         }
         Journal_Printf( PpcIntegralOutputStream, "\n" );
      }
		PpcIntegralOutputBeenHere = True;
	}
		
	Journal_Printf( PpcIntegralOutputStream, "    %12.6g    %12.6g",	(double)context->timeStep, context->currentTime );
   for(ii=0; ii< ppcIntegralCount; ii++) {
      Stg_Component* comp = LiveComponentRegister_At( context->CF->LCRegister, ppcIntegralIndex[ii] );
      Journal_Printf( PpcIntegralOutputStream, "    %12.6g", PpcIntegral_Integrate(comp) );
   }
   Journal_Printf( PpcIntegralOutputStream, "\n" );

}


void _Underworld_PpcIntegralOutput_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
	AbstractContext* context;
   PpcIntegralOutputBeenHere = False;

	context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", AbstractContext, True, data  ); 
	ContextEP_Append( context, AbstractContext_EP_FrequentOutput, PpcIntegralOutput );
}


void* _Underworld_PpcIntegralOutput_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof( Codelet );
	Type                                                      type = Underworld_PpcIntegralOutput_Type;
	Stg_Class_DeleteFunction*                              _delete = _Codelet_Delete;
	Stg_Class_PrintFunction*                                _print = _Codelet_Print;
	Stg_Class_CopyFunction*                                  _copy = _Codelet_Copy;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Underworld_PpcIntegralOutput_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Underworld_PpcIntegralOutput_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Codelet_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Codelet_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Codelet_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Codelet_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;

	return _Codelet_New(  CODELET_PASSARGS  );
}


Index Underworld_PpcIntegralOutput_Register( PluginsManager* pluginsManager ) {
	Index result;

	result = PluginsManager_Submit( pluginsManager, Underworld_PpcIntegralOutput_Type, (Name)"0", _Underworld_PpcIntegralOutput_DefaultNew  );

	return result;
}


