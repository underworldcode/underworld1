#include <mpi.h>
#include <StGermain/StGermain.h>
#include <StgDomain/StgDomain.h>
#include <StgFEM/StgFEM.h>
#include <PICellerator/PICellerator.h>
#include <Common/Common.h>

#include <string.h>
#include <math.h>

#include "types.h"
//#include "MineralPhase.h"
//#include "LookupTable.h"
#include "Ppc_Table.h"


/* Textual name of this class */
const Type Ppc_Table_Type = "Ppc_Table";

#ifndef myAbs
#define myAbs( x ) ((x > 0) ? x : -x)
#endif

#define MAXLINESIZE 1024

/* Private Constructor: This will accept all the virtual functions for this class as arguments. */
Ppc_Table* _Ppc_Table_New(  PPC_TABLE_DEFARGS  )
{
	Ppc_Table* self;
	
	assert( _sizeOfSelf >= sizeof(Ppc_Table) );
	nameAllocationType = NON_GLOBAL;
	self = (Ppc_Table*) _Ppc_New(  PPC_PASSARGS  );	
	self->_get = _get;
	return self;
}


void* _Ppc_Table_DefaultNew( Name name ) {
	/* Variables set in this function */
	SizeT                                              _sizeOfSelf = sizeof(Ppc_Table);
	Type                                                      type = Ppc_Table_Type;
	Stg_Class_DeleteFunction*                              _delete = _Ppc_Table_Delete;
	Stg_Class_PrintFunction*                                _print = _Ppc_Table_Print;
	Stg_Class_CopyFunction*                                  _copy = NULL;
	Stg_Component_DefaultConstructorFunction*  _defaultConstructor = _Ppc_Table_DefaultNew;
	Stg_Component_ConstructFunction*                    _construct = _Ppc_Table_AssignFromXML;
	Stg_Component_BuildFunction*                            _build = _Ppc_Table_Build;
	Stg_Component_InitialiseFunction*                  _initialise = _Ppc_Table_Initialise;
	Stg_Component_ExecuteFunction*                        _execute = _Ppc_Table_Execute;
	Stg_Component_DestroyFunction*                        _destroy = _Ppc_Table_Destroy;
	AllocationType                              nameAllocationType = NON_GLOBAL;
   Ppc_GetFunction*                                          _get = _Ppc_Table_Get;

	return (void*)_Ppc_Table_New(  PPC_TABLE_PASSARGS  );
}


void _Ppc_Table_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data ) {
	Ppc_Table* self = (Ppc_Table*)_self;
	Dictionary*	theDictionary = NULL;
	Stream* info = Journal_Register( Info_Type, (Name)self->type  );
	FILE* fh = NULL;
	char buffer[MAXLINESIZE];
	int i;
	double aux;
	char* strUnit = NULL;

	/* Construct parent */
	_Ppc_AssignFromXML( self, cf, data );

	/* The dictionary */ 
	theDictionary = Dictionary_Entry_Value_AsDictionary( Dictionary_Get( cf->componentDict, (Dictionary_Entry_Key)self->name )  );

	/* The values to index the table comes from these variables */	
	self->columnVariable = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"ColumnData", FeVariable, True, data );
	self->rowVariable = Stg_ComponentFactory_ConstructByKey( cf, self->name, (Dictionary_Entry_Key)"RowData", FeVariable, True, data );

	/* Scaling stuff */
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


void _Ppc_Table_Build( void* _self, void* data ) {
  Ppc_Table* self = (Ppc_Table*)_self;
  Index tableSize = 0;
  Index i;
  double aux;

  /* Build parent */
  _Ppc_Build( self, data );

  /* Scale */
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


void _Ppc_Table_Initialise( void* _self, void* data ) {
   Ppc_Table* self = (Ppc_Table*)_self;

	/* Initialize parent */
	_Ppc_Initialise( self, data );
}


void _Ppc_Table_Delete( void* _self ) {
   Ppc_Table* self = (Ppc_Table*)_self;
	
	if( self->nCoeff > 0 )
	  Memory_Free( self->table );

	/* Delete parent */
	_Ppc_Delete( self );
}


void _Ppc_Table_Print( void* _self, Stream* stream ) {
   Ppc_Table* self = (Ppc_Table*)_self;

	/* Print parent */
	_Ppc_Print( self, stream );
}


void _Ppc_Table_Execute( void* _self, void* data ) {
   Ppc_Table* self = (Ppc_Table*)_self;

	/* Execute parent */
	_Ppc_Execute( self, data );
}


void _Ppc_Table_Destroy( void* _self, void* data ) {
   Ppc_Table* self = (Ppc_Table*)_self;

	/* Destroy parent */
	_Ppc_Destroy( self, data );
}

valueType Ppc_Table_Search( void* _self, valueType rowValue, valueType colValue ) {
  Ppc_Table* self = (Ppc_Table*)_self;
  Index rowNumber, colNumber, tableSize;
  Index ixr1c1, ixr2c1, ixr1c2, ixr2c2; 
  valueType var1c1, var2c1, var1c2, var2c2, result;
  double dr, dc;

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


/* 
 * Public functions 
 *
 */

int _Ppc_Table_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result ) {
	Ppc_Table* self = (Ppc_Table*) _self;
	double T, P;

	_Ppc_GetValueFromFeVariable( self, self->rowVariable, lElement_I, particle, &T );
	_Ppc_GetValueFromFeVariable( self, self->columnVariable, lElement_I, particle, &P );

	result[0] = Ppc_Table_Search( self, T, P );

	return 0;
}
