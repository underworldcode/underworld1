
#ifndef __PICellerator_Common_LookupTable_h__
#define __PICellerator_Common_LookupTable_h__

        typedef double valueType;


	/** Textual name of this class */
	extern const Type LookupTable_Type;

 	/** class contents */
   #define __LookupTable																   \
        /* Parent info */																	\
        __Stg_Component																		\
        FiniteElementContext*				                     context;			\
        char*					                                 fileName;		\
        /* number of rows and cols */													\
        char*					                                 tableName;		\
        Index                                               nRow;				\
        Index                                               nCol;				\
        Index                                               nCoeff;			\
        /* \Delta row and \Delta col*/													\
        double                                              ARow;				\
        double                                              ACol;				\
        /* Min and max values in each direction */									\
        double                                              minValueRow;	\
        double                                              maxValueRow;	\
        double                                              minValueCol;	\
        double                                              maxValueCol;	\
        /* The data storage */															\
        valueType*                                          table;			\
        Scaling*                                            scaling;			\
        UnitDefinition_Type                                 rowUnits;	   \
        UnitDefinition_Type                                 columnUnits;	\
        UnitDefinition_Type                                 tableUnits;    \
         

	struct LookupTable { __LookupTable };
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define LOOKUPTABLE_DEFARGS \
                STG_COMPONENT_DEFARGS

	#define LOOKUPTABLE_PASSARGS \
                STG_COMPONENT_PASSARGS

	LookupTable* _LookupTable_New(  LOOKUPTABLE_DEFARGS  );
	

	void _LookupTable_Delete( void* _self );
	void _LookupTable_Print( void* _self, Stream* stream );

	void* _LookupTable_DefaultNew( Name name );
   void _LookupTable_AssignFromXML( void* _self, Stg_ComponentFactory* cf, void* data );
	void _LookupTable_Build( void* _self, void* data );
	void _LookupTable_Initialise( void* _self, void* data );
	void _LookupTable_Execute( void* _self, void* data );
	void _LookupTable_Destroy( void* _self, void* data );


   /* public functions */
   valueType LookupTable_Search( void* _self, valueType rowValue, valueType colValue );

#endif

