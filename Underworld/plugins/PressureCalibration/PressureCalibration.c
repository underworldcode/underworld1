/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
** Copyright (c) 2005-2010, Monash University 
** All rights reserved.
** Redistribution and use in source and binary forms, with or without modification,
** are permitted provided that the following conditions are met:
**
**              * Redistributions of source code must retain the above copyright notice, 
**                      this list of conditions and the following disclaimer.
**              * Redistributions in binary form must reproduce the above copyright 
**                      notice, this list of conditions and the following disclaimer in the 
**                      documentation and/or other materials provided with the distribution.
**              * Neither the name of the Monash University nor the names of its contributors 
**                      may be used to endorse or promote products derived from this software 
**                      without specific prior written permission.
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
#include <assert.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Underworld/Underworld.h>

const Type Underworld_PressureCalibration_Type = "Underworld_PressureCalibration";

typedef struct {
    __Codelet
} Underworld_PressureCalibration;

static double fixedReferenceHeight;

void Underworld_PressureCalibration_UsingMinPressure( void* sle,  UnderworldContext* context ){
    FeVariable*         pressureField    = (FeVariable*) LiveComponentRegister_Get( context->CF->LCRegister, (Name)"PressureField"  );
    Node_LocalIndex     node_I;
    Node_LocalIndex     nodeCount;
    double              pressure;
    double              minVal;
    Variable*           currVariable;

    assert( pressureField );

    minVal = _FeVariable_GetMinGlobalFieldMagnitude( pressureField );

    nodeCount = FeMesh_GetNodeDomainSize( pressureField->feMesh );

    Journal_RPrintf( Journal_Register( Info_Type, (Name)Underworld_PressureCalibration_Type  ), 
          "In plugin %s: Calibrating the pressure solution to be positive, raw min value used to normalise %g\n",
          /*at coord (%g, %g, %g)\n",*/
          Underworld_PressureCalibration_Type, minVal /*, 1,2,3*/ );



    for ( node_I = 0 ; node_I < nodeCount ; node_I++ ) {
        /* Get Pressure Value */
        currVariable = DofLayout_GetVariable( pressureField->dofLayout, node_I, 0 );
        pressure = Variable_GetValueDouble( currVariable, node_I );

        /* Do correction */
        pressure = pressure - minVal;

        /* Set value */
        Variable_SetValueDouble( currVariable, node_I, pressure );
    }
}

void Underworld_PressureCalibration_Normal( void* sle,  UnderworldContext* context ){
    FeVariable*         velocityField    = (FeVariable*) LiveComponentRegister_Get( context->CF->LCRegister, (Name)"VelocityField" );
    FeVariable*         pressureField    = (FeVariable* ) LiveComponentRegister_Get( context->CF->LCRegister, (Name)"PressureField"  );
    Node_LocalIndex     node_I;
    Node_LocalIndex     nodeCount;
    double              pressure;
    Dictionary*         dictionary       = context->dictionary;
    double              referenceHeight;
    double              averagePressureAtRefHeight;
    Variable*           currVariable;
    Coord               min, max;

    assert( pressureField );

    averagePressureAtRefHeight = FeVariable_AveragePlane( pressureField, context->verticalAxis, fixedReferenceHeight ) ;

    nodeCount = FeMesh_GetNodeDomainSize( pressureField->feMesh );

    for ( node_I = 0 ; node_I < nodeCount ; node_I++ ) {
        /* Get Pressure Value */
        currVariable = DofLayout_GetVariable( pressureField->dofLayout, node_I, 0 );
        pressure = Variable_GetValueDouble( currVariable, node_I );

        /* Do correction */
        pressure -= averagePressureAtRefHeight;

        /* Set value */
        Variable_SetValueDouble( currVariable, node_I, pressure );
    }
}



void Underworld_PressureCalibration_UpperSurface( void* sle,  UnderworldContext* context ){
    FeVariable*         velocityField    = (FeVariable*) LiveComponentRegister_Get( context->CF->LCRegister, (Name)"VelocityField" );
    FeVariable*         pressureField    = (FeVariable* ) LiveComponentRegister_Get( context->CF->LCRegister, (Name)"PressureField"  );
    Node_LocalIndex     node_I;
    Node_LocalIndex     nodeCount;
    double              pressure;
    Dictionary*         dictionary       = context->dictionary;
    double              referenceHeight;
    double              averagePressureAtRefHeight;
    Variable*           currVariable;
    Coord               min, max;

    assert( pressureField );

    FieldVariable_GetMinAndMaxGlobalCoords( velocityField, min, max );
    referenceHeight  = max[ context->verticalAxis ];
    averagePressureAtRefHeight = FeVariable_AveragePlane( pressureField, context->verticalAxis, referenceHeight ) ;

    nodeCount = FeMesh_GetNodeDomainSize( pressureField->feMesh );

    for ( node_I = 0 ; node_I < nodeCount ; node_I++ ) {
        /* Get Pressure Value */
        currVariable = DofLayout_GetVariable( pressureField->dofLayout, node_I, 0 );
        pressure = Variable_GetValueDouble( currVariable, node_I );

        /* Do correction */
        pressure -= averagePressureAtRefHeight;

        /* Set value */
        Variable_SetValueDouble( currVariable, node_I, pressure );
    }
}

void _Underworld_PressureCalibration_AssignFromXML( void* component, Stg_ComponentFactory* cf, void* data ) {
    Underworld_PressureCalibration* self = (Underworld_PressureCalibration*)component;
    UnderworldContext* context;
    Bool useMinValue = False;
    Bool useTopSurface = False;

    context = (UnderworldContext*)Stg_ComponentFactory_ConstructByName( cf, (Name)"context", UnderworldContext, True, data );
    self->context = (AbstractContext* )context;

    useMinValue = Stg_ComponentFactory_PluginGetBool( cf, self, (Dictionary_Entry_Key)"UsingMinPressure", False );
    useTopSurface = Stg_ComponentFactory_PluginGetBool( cf, self, (Dictionary_Entry_Key)"UsingTopPressure", False );
    fixedReferenceHeight = Stg_ComponentFactory_PluginGetDouble( cf, self, (Dictionary_Entry_Key)"ReferenceHeight", 0.0 );

    if( useMinValue  )
        ContextEP_Append( context, "stokesEqn-execute", Underworld_PressureCalibration_UsingMinPressure );
    else if ( useTopSurface )
        ContextEP_Append( context, "stokesEqn-execute", Underworld_PressureCalibration_UpperSurface );
	else
        ContextEP_Append( context, "stokesEqn-execute", Underworld_PressureCalibration_Normal );
}


void* _Underworld_PressureCalibration_DefaultNew( Name name ) {
    /* Variables set in this function */
    SizeT                                              _sizeOfSelf = sizeof(Underworld_PressureCalibration);
    Type                                                      type = Underworld_PressureCalibration_Type;
    Stg_Class_DeleteFunction*                              _delete = _Codelet_Delete;
    Stg_Class_PrintFunction*                                _print = _Codelet_Print;
    Stg_Class_CopyFunction*                                  _copy = _Codelet_Copy;
    Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Underworld_PressureCalibration_DefaultNew;
    Stg_Component_ConstructFunction*                    _construct = _Underworld_PressureCalibration_AssignFromXML;
    Stg_Component_BuildFunction*                            _build = _Codelet_Build;
    Stg_Component_InitialiseFunction*                  _initialise = _Codelet_Initialise;
    Stg_Component_ExecuteFunction*                        _execute = _Codelet_Execute;
    Stg_Component_DestroyFunction*                        _destroy = _Codelet_Destroy;

    /* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
    AllocationType  nameAllocationType = ZERO;

    return _Codelet_New(  CODELET_PASSARGS  );
}


Index Underworld_PressureCalibration_Register( PluginsManager* pluginsManager ) {
    return PluginsManager_Submit( pluginsManager, Underworld_PressureCalibration_Type, (Name)"0", _Underworld_PressureCalibration_DefaultNew  );
}
