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
*%  Owen Kaluza - Owen.Kaluza(at)monash.edu
*%
*% Development Team :
*%  http://www.underworldproject.org/aboutus.html
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>

#include "Base.h"

Stream* lucInfo  = NULL;
Stream* lucDebug = NULL;
Stream* lucError = NULL;

/* Alias placeholders */
const Type lucDefaultWindow_Type = "lucDefaultWindow";
/* Dummy objects */
const Type lucRenderingEngineGL_Type = "lucRenderingEngineGL";
const Type lucDefaultImageOutput_Type = "lucDefaultImageOutput";
const Type lucOutputPNG_Type = "lucOutputPNG";
const Type lucOutputJPEG_Type = "lucOutputJPEG";
const Type lucEncoderLibfame_Type = "lucEncoderLibfame";
const Type lucEncoderLibavcodec_Type = "lucEncoderLibavcodec";

Bool lucBase_Init()
{
   Stg_ComponentRegister* componentRegister = Stg_ComponentRegister_Get_ComponentRegister();

   Journal_Printf( Journal_Register( Debug_Type, (Name)"Context"  ), "In: %s\n", __func__ ); /* DO NOT CHANGE OR REMOVE */

   /* Set up streams */
   lucInfo  = Journal_Register( Info_Type, (Name)"lucInfo"  );
   lucDebug = Journal_Register( Debug_Type, (Name)"lucDebug"  );
   lucError = Journal_Register( Error_Type, (Name)"lucError" );

   Stg_ComponentRegister_Add( componentRegister, lucCamera_Type, (Name)"0", _lucCamera_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucColourMap_Type, (Name)"0", _lucColourMap_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucDatabase_Type, (Name)"0", _lucDatabase_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucViewport_Type, (Name)"0", _lucViewport_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucWindow_Type, (Name)"0", _lucWindow_DefaultNew  );

   /* Register Parents for type checking */
   RegisterParent( lucCamera_Type,            Stg_Component_Type );
   RegisterParent( lucColourMap_Type,         Stg_Component_Type );
   RegisterParent( lucDatabase_Type,          Stg_Component_Type );
   RegisterParent( lucDrawingObject_Type,     Stg_Component_Type );
   RegisterParent( lucViewport_Type,          Stg_Component_Type );
   RegisterParent( lucWindow_Type,            Stg_Component_Type );

   RegisterParent( lucDrawingObject_Register_Type, NamedObject_Register_Type );

   /* For backward compatibility */
   Stg_ComponentRegister_Add( componentRegister, lucDefaultWindow_Type, (Name)"0", _lucWindow_DefaultNew  );
   /* Dummy components */
   Stg_ComponentRegister_Add( componentRegister, lucRenderingEngineGL_Type, (Name)"0", _DummyComponent_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucDefaultImageOutput_Type, (Name)"0", _DummyComponent_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucOutputPNG_Type, (Name)"0", _DummyComponent_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucOutputJPEG_Type, (Name)"0", _DummyComponent_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucEncoderLibfame_Type, (Name)"0", _DummyComponent_DefaultNew  );
   Stg_ComponentRegister_Add( componentRegister, lucEncoderLibavcodec_Type, (Name)"0", _DummyComponent_DefaultNew  );

   return True;
}



