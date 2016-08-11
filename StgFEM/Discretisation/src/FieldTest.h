/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003-2006, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street,
**   Melbourne, 3053, Australia.
**
** Primary Contributing Organisations:
**   Victorian Partnership for Advanced Computing Ltd, Computational Software Development - http://csd.vpac.org
**   AuScope - http://www.auscope.org
**   Monash University, AuScope SAM VIC - http://www.auscope.monash.edu.au
**   Computational Infrastructure for Geodynamics - http://www.geodynamics.org
**
** Contributors:
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Robert Turnbull, Research Assistant, Monash University. (robert.turnbull@sci.monash.edu.au)
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   David May, PhD Student, Monash University (david.may@sci.monash.edu.au)
**   Louis Moresi, Associate Professor, Monash University. (louis.moresi@sci.monash.edu.au)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
**   Julian Giordani, Research Assistant, Monash University. (julian.giordani@sci.monash.edu.au)
**   Vincent Lemiale, Postdoctoral Fellow, Monash University. (vincent.lemiale@sci.monash.edu.au)
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
**  Role:
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgFEM_Discretisation_FieldTest_h__
#define __StgFEM_Discretisation_FieldTest_h__
   
   typedef struct event {
      double time;
      double place[3];
      double value[9];
   } Event;

   /* Solution types to use */
   typedef enum EvaluateSolution {
      ANALYTIC_SOLUTION,
      REFERENCE_SOLUTION,
      HIGHRES_REFERENCE_SOLUTION
   } EvaluateSolution;

   typedef Bool (FieldTest_ExpectedResultFunc)
      ( void** data, void* context, Event* expected, Event* numeric, Event* tolerance );

   /* Textual name of this class */
   extern const Type FieldTest_Type;

   /* Data structure for storing references to analytic/error fields. */
   #define __FieldMapping \
      FeVariable*         numericField; \
      AnalyticFeVariable* analyticField; \
      OperatorFeVariable* analyticMagField; \
      ErrorFeVariable*    errorField; \
      OperatorFeVariable* errorMagField;
   
  typedef struct FieldMapping_t { __FieldMapping } FieldMapping;

   /* FieldTest information */
   #define __FieldTest \
      /* Parent info */ \
      __Stg_Component \
      /* Virtual info */ \
      /* FieldTest info */ \
      /* FieldMapping list */ \
      FieldMapping**                   fieldMappings; \
      unsigned                         fieldCount; \
      Swarm*                           numericSwarm; \
      Name*                            swarmVarNameList; \
      unsigned                         swarmVarCount; \
      FeMesh*                          elementMesh; \
      Bool                             normalise; \
      /* Constant to add to analytic element integral */ \
      /* normalising by this so you dont get divide by zero issues */ \
      double                           epsilon; \
      /* the timestep to run the test */ \
      int                              testTimestep; \
      double**                         gAnalyticSq; \
      double**                         gErrorSq; \
      double**                         gError; \
      double**                         gErrorNorm; \
      Name*                            referenceSolnFileList; \
      Name                             referenceSolnPath; \
      Swarm*                           integrationSwarm; \
      DomainContext*                   context; \
      LiveComponentRegister*           LCRegister; \
      Bool                             referenceSolnFromFile; \
      Bool                             highResReferenceSoln; \
      Name                             expectedFileName; \
      Name                             expectedFilePath; \
      void**                           expectedData; \
      FieldTest_ExpectedResultFunc*    expectedFunc; \
      unsigned                         expectedDofs; \
      Event*                           expected; \
      Event*                           numeric; \
      Event*                           tolerance; \
      Bool                             expectedPass; \
      Name                             dumpExpectedFileName; \
      Bool                             appendToAnalysisFile; \
      Bool                             testAtRestartStep;

   struct FieldTest { __FieldTest };
   
   /* --- Constructors / Destructor --- */
   
   /* Create a new FieldTest and initialise */

   void* _FieldTest_DefaultNew( Name name );
   
   /* Creation implementation / Virtual constructor */
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define FIELDTEST_DEFARGS \
      STG_COMPONENT_DEFARGS

   #define FIELDTEST_PASSARGS \
      STG_COMPONENT_PASSARGS

   FieldTest* _FieldTest_New( FIELDTEST_DEFARGS );

   void _FieldTest_Init( 
      void*                   fieldTest,
      DomainContext*          context,
      Dictionary_Entry_Value* fieldMappings,
      unsigned                fieldCount,
      Bool                    referenceSolnFromFile,
      Bool                    highResReferenceSoln,
      Swarm*                  integrationSwarm,
      FeMesh*                 elementMesh, 
      unsigned                swarmVarCount,
      Name                    swarmVarName,
      Name                    referenceSolnPath,
      Bool                    normalise,
      double                  epsilon,
      double                  testTimestep,
      Bool                    appendToAnalysisFile,
      Bool                    testAtRestartStep,
      Name                    expectedFileName,
      Name                    expectedFilePath,
      Name                    dumpExpectedFileName,
      Bool                    expectedPass,
      Dictionary_Entry_Value* swarmVarList,
      void*                   data );

   /* Stg_Class_Delete a FieldTest construst */
   void _FieldTest_Delete( void* fieldTest );

   /* --- Virtual Function implementations --- */
   
   /* Print the contents of an FieldTest construct */
   void _FieldTest_Print( void* fieldTest, Stream* stream );
   
   /* Copy */
   #define FieldTest_Copy( self ) \
      (FieldTest*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
   #define FieldTest_DeepCopy( self ) \
      (FieldTest*)Stg_Class_Copy( self, NULL, True, NULL, NULL )
   
   void* _FieldTest_Copy( void* fieldTest, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
   
   /* Build implementation */
   void _FieldTest_Build( void* fieldTest, void* data );
   
   /* Construct implementation */
   void _FieldTest_AssignFromXML( void* fieldTest, Stg_ComponentFactory* cf, void* data );
   
   /* Initialisation implementation */
   void _FieldTest_Initialise( void* fieldTest, void* data );
   
   /* Execution implementation */
   void _FieldTest_Execute( void* fieldTest, void* data );
   
   /* Destruct Implementation */
   void _FieldTest_Destroy( void* fieldTest, void* data );

   /* --- Public Functions --- */
   void FieldTest_LoadReferenceSolutionFromFile( FeVariable* referenceField, Name referenceSolnName, Name referenceSolnPath, DomainContext* context );

   void FieldTest_CalculateAnalyticSolutionForField( void* fieldTest, Index field_I );

   /* Entry Points */
   void FieldTest_GenerateErrFields( void* _context, void* data );

   void FieldTest_EvaluatePhysicsTest( void* _context, void* data );

   void FieldTest_EvaluateErrorFromField( void* fieldTest, Index field_I, Index lElement_I, double* elErrorSq, double* elNormSq, EvaluateSolution type );

   void FieldTest_ElementErrAnalyticFromSwarm( void* fieldTest, Index field_I, Index lElement_I, double* elErrorSq, double* elNormSq );

   void FieldTest_ElementErrReferenceFromSwarm( void* fieldTest, Index field_I, Index lElement_I, double* elErrorSq, double* elNormSq );

#endif /* __StgFEM_Discretisation_FieldTest_h__ */

