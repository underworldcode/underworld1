#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_Switch.h"


/* Textual name of this class */
const Type Ppc_Switch_Type = "Ppc_Switch";


void _Ppc_Switch_Init( void* _self, char* op, int field ) {
  Ppc_Switch* self = (Ppc_Switch*)_self;
  Index case_I;

  Journal_Firewall( op[0] == 'l' || op[0] == 'n' ||
						  op[0] == 'c' || op[0] == 'f', 
						  self->error_stream, "\n\n\n"
						  "Error in configuration of Ppc_Switch %s\n"
						  "unknown interpolation <%s>"
						  "Valid options are: linear, nearest, ceil, floor"
						  "\n\n\n", self->name, op );
						  	  
  self->interpolation = op;
  self->fieldTag = field;

	/* check */
	for( case_I = 0; case_I < self->caseCount; case_I++ ){
	  Journal_Firewall( self->caseList[case_I].valueTag >= 0, 
							  self->error_stream, "\n\n\n"
							  "Error in configuration of Ppc_Switch %s\n"
							  "Case number %i, (with case %g) has no assosiated Value.\n"
							  "\n\n\n", self->name, case_I, self->caseList[case_I].constant );
  }	
}


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_Switch* _Ppc_Switch_New(  PPC_SWITCH_DEFARGS  )
{
	Ppc_Switch* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_Switch) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_Switch*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_Switch_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_Switch);
	Type                                                      type = Ppc_Switch_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_Switch_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_Switch_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_Switch_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_Switch_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_Switch_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_Switch_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_Switch_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_Switch_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_Switch_Get;

	return (void*)_Ppc_Switch_New(  PPC_SWITCH_PASSARGS  );
}


void _Ppc_Switch_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
  Ppc_Switch* self = (Ppc_Switch*)_self;
  Dictionary*	theDictionary = NULL;
  Dictionary_Entry_Value* caseEntry = NULL;
  Dictionary_Entry_Value* tagList = NULL;
  Index case_I;
  char* ppcName;
  
  /* Construct parent */
  _Ppc_AssignFromXML( self, cf, data );
  
  /* The dictionary */ 
  theDictionary = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name ) );
  
  /* Read the properties list */	
  tagList = Dictionary_Get( theDictionary, (Dictionary_Entry_Key)"CaseList" );
  assert( tagList );
  self->caseCount = Dictionary_Entry_Value_GetCount( tagList );
  self->caseList = Memory_Alloc_Array( Ppc_Switch_Case, self->caseCount, "Ppc_Switch_caseList" );
  
  for( case_I = 0; case_I < self->caseCount; case_I++ ){
	 caseEntry = Dictionary_Entry_Value_GetElement( tagList, case_I );
	 /* get case value */
	 self->caseList[case_I].constant = Dictionary_Entry_Value_AsDouble( Dictionary_Entry_Value_GetMember( caseEntry, (Dictionary_Entry_Key)"Case") );
	 /* get ppc associated with the case*/
	 ppcName = Dictionary_Entry_Value_AsString( Dictionary_Entry_Value_GetMember( caseEntry, (Dictionary_Entry_Key)"Value") );
	 self->caseList[case_I].valueTag = PpcManager_GetPpcByName( self->manager, cf, (Name)ppcName );
  }	
  
  /* Init */
  _Ppc_Switch_Init( 
     self,
	  Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"Interpolate", "" ),
	  PpcManager_GetPpcFromDict( self->manager, cf, self->name, (Dictionary_Entry_Key)"Field", "" ) );
}


void _Ppc_Switch_Build( void* _self, void* data ) {
  Ppc_Switch* self = (Ppc_Switch*)_self;

  /* Build parent */
  _Ppc_Build( self, data );
}

void _Ppc_Switch_Initialise( void* _self, void* data ) {
  Ppc_Switch* self = (Ppc_Switch*)_self;

  /* Initialize parent */
  _Ppc_Initialise( self, data );
}

void _Ppc_Switch_Delete( void* _self ) {
  Ppc_Switch* self = (Ppc_Switch*)_self;

  /* Delete parent */
  _Ppc_Delete( self );
}

void _Ppc_Switch_Print( void* _self, Stream* stream ) {
  Ppc_Switch* self = (Ppc_Switch*)_self;

  /* Print parent */
  _Ppc_Print( self, stream );
}

void _Ppc_Switch_Execute( void* _self, void* data ) {
  Ppc_Switch* self = (Ppc_Switch*)_self;

  /* Execute parent */
  _Ppc_Execute( self, data );
}

void _Ppc_Switch_Destroy( void* _self, void* data ) {
  Ppc_Switch* self = (Ppc_Switch*)_self;

  if( self->caseCount )
	 Memory_Free( self->caseList );
   
  /* Destroy parent */
  _Ppc_Destroy( self, data );
}




int _Ppc_Switch_InterpolateLinear( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double fieldValue, double* result ) { 
  Ppc_Switch* self = (Ppc_Switch*) _self;
  Ppc_Switch_Case* caseList = self->caseList;
  double val1, val2, case1, case2;
  int tag = -1, tag1 = -1, tag2 = -1;
  int case_I, err;

  /* fieldValue less than the minimum */
  if( fieldValue <= caseList[0].constant ) {
	 tag = caseList[0].valueTag;
	 /* get value */
	 err = PpcManager_Get( self->manager, lElement_I, particle, tag, result );
	 return err;
  }	 
  
  /* fieldValue in some case */
  for( case_I = 0 ; case_I < self->caseCount - 1 ; case_I++ ) {
	 case1 = caseList[case_I].constant;
	 case2 = caseList[case_I+1].constant;
	 if( case1 <= fieldValue && fieldValue <= case2 ) {
		tag1 = caseList[case_I].valueTag;
		tag2 = caseList[case_I+1].valueTag;
		err = PpcManager_Get( self->manager, lElement_I, particle, tag1, &val1 );
		assert( !err );
		err = PpcManager_Get( self->manager, lElement_I, particle, tag2, &val2 );
		assert( !err );
		result[0] = (fieldValue - case1)/(case2 - case1) * (val2 - val1) + val1;
		return 0;
	 }
  }
  
  /* fieldValue greater that the maximum */
  tag = caseList[self->caseCount-1].valueTag;
  err = PpcManager_Get( self->manager, lElement_I, particle, tag, result );
  return err;
}


int _Ppc_Switch_InterpolateNearest( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double fieldValue, double* result ) { 
  Ppc_Switch* self = (Ppc_Switch*) _self;
  Ppc_Switch_Case* caseList = self->caseList;
  double case1, case2;
  int tag = -1;
  int case_I, err;

  /* fieldValue less than the minimum */
  if( fieldValue <= caseList[0].constant ) {
	 tag = caseList[0].valueTag;
	 /* get value */
	 err = PpcManager_Get( self->manager, lElement_I, particle, tag, result );
	 return err;
  }	 
  
  /* fieldValue in some case */
  for( case_I = 0 ; case_I < self->caseCount - 1 ; case_I++ ) {
	 case1 = caseList[case_I].constant;
	 case2 = caseList[case_I+1].constant;
	 if( case1 <= fieldValue && fieldValue <= case2 ) {
		if( fieldValue - case1  < case2 -  case2 ) {
		  tag = caseList[case_I].valueTag;
		  err = PpcManager_Get( self->manager, lElement_I, particle, tag, result );
		  return err;
		} else {
		  tag = caseList[case_I+1].valueTag;
		  err = PpcManager_Get( self->manager, lElement_I, particle, tag, result );
		  return err;
		}
	 }
  }

  /* fieldValue greater that the maximum */
  tag = caseList[self->caseCount-1].valueTag;
  err = PpcManager_Get( self->manager, lElement_I, particle, tag, result );
  return err;
}


int _Ppc_Switch_InterpolateFloor( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double fieldValue, double* result ) { 
  Ppc_Switch* self = (Ppc_Switch*) _self;
  Ppc_Switch_Case* caseList = self->caseList;
  double case1, case2;
  int tag = -1;
  int case_I, err;

  /* fieldValue less than the minimum */
  if( fieldValue <= caseList[0].constant ) {
	 tag = caseList[0].valueTag;
	 /* get value */
	 err = PpcManager_Get( self->manager, lElement_I, particle, tag, result );
	 return err;
  }	 
  
  /* fieldValue in some case */
  for( case_I = 0 ; case_I < self->caseCount - 1 ; case_I++ ) {
	 case1 = caseList[case_I].constant;
	 case2 = caseList[case_I+1].constant;
	 if( case1 <= fieldValue && fieldValue <= case2 ) {
		tag = caseList[case_I].valueTag;
		err = PpcManager_Get( self->manager, lElement_I, particle, tag, result );
		return err;
	 }
  }

  /* fieldValue greater that the maximum */
  tag = caseList[self->caseCount-1].valueTag;
  err = PpcManager_Get( self->manager, lElement_I, particle, tag, result );
  return err;
}


int _Ppc_Switch_InterpolateCeil( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double fieldValue, double* result ) { 
  Ppc_Switch* self = (Ppc_Switch*) _self;
  Ppc_Switch_Case* caseList = self->caseList;
  double case1, case2;
  int tag = -1;
  int case_I, err;

  /* fieldValue less than the minimum */
  if( fieldValue <= caseList[0].constant ) {
	 tag = caseList[0].valueTag;
	 /* get value */
	 err = PpcManager_Get( self->manager, lElement_I, particle, tag, result );
	 return err;
  }	 
  
  /* fieldValue in some case */
  for( case_I = 0 ; case_I < self->caseCount - 1 ; case_I++ ) {
	 case1 = caseList[case_I].constant;
	 case2 = caseList[case_I+1].constant;
	 if( case1 <= fieldValue && fieldValue <= case2 ) {
		tag = caseList[case_I+1].valueTag;
		err = PpcManager_Get( self->manager, lElement_I, particle, tag, result );
		return err;
	 }
  }

  /* fieldValue greater that the maximum */
  tag = caseList[self->caseCount-1].valueTag;
  err = PpcManager_Get( self->manager, lElement_I, particle, tag, result );
  return err;
}

/* 
 * Public functions 
 *
 */
int _Ppc_Switch_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
  Ppc_Switch* self = (Ppc_Switch*) _self;
  double fieldValue;
  int err;

  /* get field value */
  err = PpcManager_Get( self->manager, lElement_I, particle, self->fieldTag, &fieldValue );
  assert( !err );

  switch( self->interpolation[0] ) {
  case 'l':
	 err = _Ppc_Switch_InterpolateLinear( self, lElement_I, particle, fieldValue, result );
	 break;
  case 'n':
	 err = _Ppc_Switch_InterpolateNearest( self, lElement_I, particle, fieldValue, result );
	 break;
  case 'c':
	 err = _Ppc_Switch_InterpolateCeil( self, lElement_I, particle, fieldValue, result );
	 break;
  case 'f':
	 err = _Ppc_Switch_InterpolateFloor( self, lElement_I, particle, fieldValue, result );
	 break;
  default:
	 err = _Ppc_Switch_InterpolateNearest( self, lElement_I, particle, fieldValue, result );
	 break;
  }

  return err;
}


