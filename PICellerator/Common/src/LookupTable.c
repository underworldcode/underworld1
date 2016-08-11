#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "types.h"
#include "LookupTable.h"

/* Textual name of this class */
const Type LookupTable_Type = "LookupTable";

#ifndef myAbs
#define myAbs( x ) ((x > 0) ? x : -x)
#endif

#define MAXLINESIZE 512

/* #define FAST_NO_INTERPOLATION */


/* Private Constructor */
LookupTable* _LookupTable_New(  LOOKUPTABLE_DEFARGS  )
{
	LookupTable* self;
	
	assert( _sizeOfSelf >= sizeof(LookupTable) );
	nameAllocationType = NON_GLOBAL;
	self = (LookupTable*) _Stg_Component_New(  STG_COMPONENT_PASSARGS  );
	return self;
}


void* _LookupTable_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(LookupTable);
	Type                                                      type = LookupTable_Type;
	Stg_Class_DeleteFunction*                              _delete = _LookupTable_Delete;
	Stg_Class_PrintFunction*                                _print = _LookupTable_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _LookupTable_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _LookupTable_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _LookupTable_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _LookupTable_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _LookupTable_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _LookupTable_Destroy;

	/* Variables that are set to ZERO are variables that will be set either by the current _New function or another parent _New function further up the hierachy */
	AllocationType  nameAllocationType = NON_GLOBAL /* default value NON_GLOBAL */;


	return (void*)_LookupTable_New(  LOOKUPTABLE_PASSARGS  );
}


void _LookupTable_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {

	LookupTable* self = (LookupTable*)_self;
	Stream* info = Journal_Register( Info_Type, (Name)self->type  );
	Dictionary* theDictionary;
	FILE* fh;
	char buffer[MAXLINESIZE];
	int i;
	double aux;
	char* strUnit;

	/* Read the configuration */
	theDictionary = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );

	self->context = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Context", FiniteElementContext, False, data );
	if( !self->context  )
		self->context = Stg_ComponentFactory_ConstructByName( cf, (Name)"context", FiniteElementContext, True, data  );

	/* Check for scaling stuff */
	self->scaling = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"Scaling", Scaling, True, data  );

	strUnit = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"ColumnUnits", "" );
	self->columnUnits = Scaling_Parse( strUnit );

	strUnit = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"RowUnits", "" );
	self->rowUnits = Scaling_Parse( strUnit );

	strUnit = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"TableUnits", "" );
	self->tableUnits = Scaling_Parse( strUnit );

	/* File name */
	self->fileName = Stg_ComponentFactory_GetString( cf, self->name, (Dictionary_Entry_Key)"FileName", "table.dat"  );

	Journal_Printf( info, "\t\t\tConstructing lookup table from file %s\n", self->fileName );

	/* File open */
	fh = fopen( self->fileName, "r" );

	Journal_Firewall( (Bool)fh, 
			  Journal_Register( Error_Type, (Name)LookupTable_Type  ), 
			  "Error: file %s not found\n",
			  self->fileName );

	/* Read header */
	fscanf( fh, "%s", (char*)&buffer );
	self->tableName = StG_Strdup( buffer );
	fscanf( fh, "%d %d %lf %lf %lf %lf", &(self->nRow), &(self->nCol), 
		&(self->minValueRow), &(self->minValueCol), 
		&(self->ARow), &(self->ACol) );

	Journal_Firewall( (Bool)self->nCol > 0, 
			  Journal_Register( Error_Type, (Name)LookupTable_Type  ), 
			  "Error: number of columns is %i in file %s\n",
  			  self->nCol, self->fileName );

	Journal_Firewall( (Bool)self->nRow > 0, 
			  Journal_Register( Error_Type, (Name)LookupTable_Type  ), 
			  "Error: number of rows is %i in file %s\n",
			  self->nRow, self->fileName );

	self->maxValueRow = self->minValueRow + self->nRow * self->ARow;
	self->maxValueCol = self->minValueCol + self->nCol * self->ACol;

	/* Alloc memory */
	self->nCoeff = self->nRow * self->nCol;
	self->table = 0x0;
	self->table = Memory_Alloc_Array( valueType, self->nCoeff, self->tableName );

	Journal_Firewall( (Bool)self->table, 
			  Journal_Register( Error_Type, (Name)LookupTable_Type  ), 
			  "Not enough memory to load lookup table.\n" );

	fscanf( fh, "%s", (char*)&buffer );
	fscanf( fh, "%s", (char*)&buffer );

	/* Read the table */
	i = 0;
	while( !feof( fh ) && i <  self->nRow * self->nCol ) {
	  fscanf( fh, "%lf", &aux );
	  self->table[i] = aux;
	  i++;
	}
	if( i != self->nRow*self->nCol ) {
	  assert( 0 );
	}
	Journal_Firewall( i == self->nCoeff, 
			  Journal_Register( Error_Type, (Name)LookupTable_Type  ), 
			  "Number of rows (%i) in file %s does not match the size of the table (%ix%i)\n",
			  i, self->fileName, self->nRow, self->nCol );	  
	
	fclose( fh );

	Journal_Printf( info, "\t\t\t\t...done\n" );
}


void _LookupTable_Build( void* _self, void* data ) {
  LookupTable*        self = (LookupTable*) _self;
  Index               tableSize;
  Index               i;
  double              aux;

  if( self->scaling ) {
    tableSize = self->nCoeff;
    for( i = 0; i < tableSize; i++ ) {
      self->table[i] = Scaling_Scale( self->scaling, self->table[i], self->tableUnits );
    }
    
	 self->minValueRow = Scaling_Scale( self->scaling, self->minValueRow, self->rowUnits );
    self->maxValueRow = Scaling_Scale( self->scaling, self->maxValueRow, self->rowUnits );
    self->ARow = Scaling_Scale( self->scaling, self->ARow, self->rowUnits );
    
    self->minValueCol = Scaling_Scale( self->scaling, self->minValueCol, self->columnUnits );
    self->maxValueCol = Scaling_Scale( self->scaling, self->maxValueCol, self->columnUnits );
    self->ACol = Scaling_Scale( self->scaling, self->ACol*aux, self->columnUnits );   
  }
}

void _LookupTable_Initialise( void* _self, void* data ) {}

void _LookupTable_Delete( void* _self ) {

   LookupTable*        self = (LookupTable*) _self;

   Memory_Free( self->table );
}

void _LookupTable_Print( void* _self, Stream* stream ) {}

void _LookupTable_Execute( void* _self, void* data ) {}

void _LookupTable_Destroy( void* _self, void* data ) {}


valueType LookupTable_Search( void* _self, valueType rowValue, valueType colValue ) {

  LookupTable*                 self = (LookupTable*)_self;
  Index                        rowNumber, colNumber, tableSize;
  Index                        ixr1c1, ixr2c1, ixr1c2, ixr2c2; 
  valueType                    var1c1, var2c1, var1c2, var2c2, result;
  double                       dr, dc;

  /* Find row and col number */
  rowNumber = floor( myAbs(rowValue - self->minValueRow) / self->ARow );
  colNumber = floor( myAbs(colValue - self->minValueCol) / self->ACol );

  tableSize = self->nCoeff;

#ifdef FAST_NO_INTERPOLATION

  ixr1c1 = colNumber * self->nRow + rowNumber;

  if( ixr1c1 >= tableSize ) {
    ixr1c1 = tableSize;
  }

  result = self->table[ixr1c1];

#else  

  if( rowNumber > self->nRow - 2 ) {
    rowNumber = self->nRow - 2 ;
    rowValue = self->maxValueRow;
  }
  if( colNumber > self->nCol - 2 ) {
    colNumber = self->nCol - 2 ;
    colValue = self->maxValueCol;
  }

  /* Find indexes in table */
  ixr1c1 = colNumber * self->nRow + rowNumber;
  ixr2c1 = ixr1c1 + 1;
  ixr1c2 = ixr1c1 + self->nRow;
  ixr2c2 = ixr2c1 + self->nRow;

  /* Grab table values */
  assert( ixr1c1 < self->nCoeff );
  assert( ixr2c1 < self->nCoeff );
  assert( ixr1c2 < self->nCoeff );
  assert( ixr2c2 < self->nCoeff );
  var1c1 = self->table[ixr1c1];
  var2c1 = self->table[ixr2c1];
  var1c2 = self->table[ixr1c2];
  var2c2 = self->table[ixr2c2];

  /* Compute distances inside the cell */
  dr = (rowValue - (rowNumber*self->ARow + self->minValueRow)) / self->ARow;
  dc = (colValue - (colNumber*self->ACol + self->minValueCol)) / self->ACol;

  /* Average the value */
  result = (var1c1*dr + var2c1*(1-dr)) *    dc +
           (var1c2*dr + var2c2*(1-dr)) * (1-dc);
#endif		

  return result;
}







