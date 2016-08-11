#ifndef __PICellerator_Common_Ppc_Table_h__
#define __PICellerator_Common_Ppc_Table_h__

/** Textual name of this class */
extern const Type Ppc_Table_Type;

/** Ppc_Table class contents */
      #define __Ppc_Table                                               \
      /* Parent info */																	\
      __Ppc																					\
      /* Variables */																	\
      FeVariable*                                         columnVariable;	\
		FeVariable*                                         rowVariable;		\
		/* General */																		\
		char*					                                  fileName;		\
		/* number of rows and cols */													\
		char*					                                  tableName;		\
		Index                                               nRow;			\
		Index                                               nCol;			\
		Index                                               nCoeff;			\
		/* \Delta row and \Delta col*/												\
		double                                              ARow;			\
		double                                              ACol;			\
		/* Min and max values in each direction */								\
		double                                              minValueRow;	\
		double                                              maxValueRow;	\
		double                                              minValueCol;	\
		double                                              maxValueCol;	\
		/* Data storage */																\
		valueType*                                          table;			\
		/* Scaling info */																\
		Scaling*                                            scaling;		\
		UnitDefinition_Type                                 rowUnits;	   \
		UnitDefinition_Type                                 columnUnits;	\
		UnitDefinition_Type                                 tableUnits;	\


	struct Ppc_Table { __Ppc_Table };

	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PPC_TABLE_DEFARGS \
                PPC_DEFARGS

	#define PPC_TABLE_PASSARGS \
                PPC_PASSARGS

	Ppc_Table* _Ppc_Table_New(  PPC_TABLE_DEFARGS  );
	
	void _Ppc_Table_Delete( void* _self );
	void _Ppc_Table_Print( void* _self, Stream* stream );
	void* _Ppc_Table_DefaultNew( Name name );
   void _Ppc_Table_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _Ppc_Table_Build( void* _self, void* data );
	void _Ppc_Table_Initialise( void* _self, void* data );
	void _Ppc_Table_Execute( void* _self, void* data );
	void _Ppc_Table_Destroy( void* _self, void* data );

   /* Public functions */
   int _Ppc_Table_Get( void* _self, Element_LocalIndex lElement_I, IntegrationPoint* particle, double* result );

#endif

