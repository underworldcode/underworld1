#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
#include "Ppc_Operation.h"


/* Textual name of this class */
const Type Ppc_Operation_Type = "Ppc_Operation";


void _Ppc_Operation_Init( void* _self, char* op ) {
  Ppc_Operation* self = (Ppc_Operation*)_self;

  Journal_Firewall( op[0] == '*' || op[0] == '/' ||
						  op[0] == '+' || op[0] == '-' ||
						  strcmp( op, "min" ) == 0 ||
						  strcmp( op, "max" ) == 0 ||
						  strcmp( op, "max" ) == 0 ||
						  strcmp( op, "geometricMean" ) == 0 ||
						  strcmp( op, "harmonicMean" ) == 0 ||
						  strcmp( op, "mean" ) == 0,
						  self->error_stream, "\n\n\n"
						  "Error in configuration of Ppc_Operation %s\n"
						  "unknown operation <%s>"
						  "\n\n\n", self->name, op );

						  
  if( op[0] == '/' || op[0] == '-' ) 
  	Journal_Firewall( self->propertyTagCount == 2, 
						  self->error_stream, "\n\n\n"
						  "Error in configuration of Ppc_Operation %s\n"
						  "The operations '-' and '/' need exacly TWO properties."
						  "\n\n\n", self->name );
						  	  
	self->operation = op;
}


Ppc_Operation* Ppc_Operation_New( Name name, PpcManager* mgr, int* tagList, int nPpc, char* operation ) {
  Ppc_Operation* self = _Ppc_Operation_DefaultNew( name );
  int I;

  self->isConstructed = True;
  _Ppc_Init( self, mgr, HUGE_VAL, HUGE_VAL, 0 );

  self->propertyTagCount = nPpc;
  self->propertyTagList = Memory_Alloc_Array( int, self->propertyTagCount, "Ppc_Operation_propertyTagList" );
  for( I = 0 ; I < nPpc ; I++ ) {
	 self->propertyTagList[I] = tagList[I];
  }

  _Ppc_Operation_Init( self, operation );

  LiveComponentRegister_IfRegThenAdd( (Stg_Component*)self );

  return self;
}


/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_Operation* _Ppc_Operation_New(  PPC_OPERATION_DEFARGS  )
{
	Ppc_Operation* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_Operation) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_Operation*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_Operation_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_Operation);
	Type                                                      type = Ppc_Operation_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_Operation_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_Operation_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_Operation_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_Operation_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_Operation_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_Operation_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_Operation_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_Operation_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_Operation_Get;

	return (void*)_Ppc_Operation_New(  PPC_OPERATION_PASSARGS  );
}


void _Ppc_Operation_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
  Ppc_Operation* self = (Ppc_Operation*)_self;
  Dictionary*	theDictionary;
  Dictionary_Entry_Value*	tagList = NULL;
  Dictionary_Entry_Value*	tagEntry = NULL;
  Index tag_I;
  char* ppcName;
  
  /* Construct parent */
  _Ppc_AssignFromXML( self, cf, data );
  
  /* The dictionary */ 
  theDictionary = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );
  
  /* Read the properties list */	
  tagList = Dictionary_Get( theDictionary, (Dictionary_Entry_Key)"Properties" );
  assert( tagList );
  self->propertyTagCount = Dictionary_Entry_Value_GetCount( tagList );
  self->propertyTagList = Memory_Alloc_Array( int, self->propertyTagCount, "Ppc_Operation_propertiesList" );
  
  for( tag_I = 0; tag_I < self->propertyTagCount; tag_I++ ){
	 tagEntry = Dictionary_Entry_Value_GetElement( tagList, tag_I );
	 ppcName = Dictionary_Entry_Value_AsString( tagEntry );
	 self->propertyTagList[tag_I] = PpcManager_GetPpcByName( self->manager, cf, (Name)ppcName );

	 Journal_Firewall( self->propertyTagList[tag_I] != -1, 
							 self->error_stream, "\n\n\n"
							 "Error in configuration of Ppc_Operation %s\n"
							 "Property <%s> not found\n"
							 "\n\n\n", self->name, ppcName );
  }	
  
  /* Init */
  _Ppc_Operation_Init( 
     self,
	  Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"Operation", "" ) );
}


void _Ppc_Operation_Build( void* _self, void* data ) {
   Ppc_Operation* self = (Ppc_Operation*)_self;

   /* Build parent */
   _Ppc_Build( self, data );

   /* assign storage for values */
   self->values = Memory_Alloc_Array( double, self->propertyTagCount, "Ppc_Operation_values" ); 
}

void _Ppc_Operation_Initialise( void* _self, void* data ) {
  Ppc_Operation* self = (Ppc_Operation*)_self;

  /* Initialize parent */
  _Ppc_Initialise( self, data );
}

void _Ppc_Operation_Delete( void* _self ) {
  Ppc_Operation* self = (Ppc_Operation*)_self;

  /* Delete parent */
  _Ppc_Delete( self );
}

void _Ppc_Operation_Print( void* _self, Stream* stream ) {
  Ppc_Operation* self = (Ppc_Operation*)_self;

  /* Print parent */
  _Ppc_Print( self, stream );
}

void _Ppc_Operation_Execute( void* _self, void* data ) {
  Ppc_Operation* self = (Ppc_Operation*)_self;

  /* Execute parent */
  _Ppc_Execute( self, data );
}

void _Ppc_Operation_Destroy( void* _self, void* data ) {
   Ppc_Operation* self = (Ppc_Operation*)_self;

   if( self->propertyTagCount )
      Memory_Free( self->propertyTagList );

   Memory_Free( self->values );

   /* Destroy parent */
   _Ppc_Destroy( self, data );
}


/* 
 * Public functions 
 *
 */

int _Ppc_Operation_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
  Ppc_Operation* self = (Ppc_Operation*) _self;
  double aux = 0;
  char* op = self->operation;
  double* values = self->values;
  int I, err;
  
	for( I = 0 ; I < self->propertyTagCount ; I++ ) {
	  err = PpcManager_Get( self->manager, lElement_I, particle, self->propertyTagList[I], &values[I] );

	  Journal_Firewall( err == 0,
							  self->error_stream, "\n\n\n"
							  "Error in configuration of Ppc_Operation %s\n"
							  "\n\n\n", self->name );
	}
  
  switch( op[0] ) {
  case '*':
  	aux = 1;
		for( I = 0 ; I < self->propertyTagCount ; I++ )
	 		aux *= values[I];
		break;
  case '/':
		aux = values[0] / values[1];
		break;
  case '+':
 		aux = 0;
		for( I = 0 ; I < self->propertyTagCount ; I++ )
			aux += values[I];
		break;
  case '-':
		aux = values[0] - values[1];
		break;
  default:
		if( strcmp( op, "min" ) == 0 ) {
			aux = values[0];
			for( I = 1 ; I < self->propertyTagCount ; I++ )
				aux = values[I] < aux ? values[I] : aux;
				
		} else if( strcmp( op, "max" ) == 0 ) {
			aux = values[0];
			for( I = 1 ; I < self->propertyTagCount ; I++ )
				aux = values[I] > aux ? values[I] : aux;
				
		} else if( strcmp( op, "mean" ) == 0 ) {
	 		aux = 0;
			for( I = 0 ; I < self->propertyTagCount ; I++ )
				aux += values[I];
			aux /= self->propertyTagCount;
			 	
		} else if( strcmp( op, "geometricMean" ) == 0 ) {
	  	aux = 1;
			for( I = 0 ; I < self->propertyTagCount ; I++ )
	 			aux *= values[I];
			aux = sqrt( aux );
			
		} else if( strcmp( op, "harmonicMean" ) == 0 ) {
	  	aux = 0;
			for( I = 0 ; I < self->propertyTagCount ; I++ )
	 			aux += 1.0/values[I];
			aux = self->propertyTagCount / aux;
		}
	}

  result[0] = aux;
  
  return 0;
}
