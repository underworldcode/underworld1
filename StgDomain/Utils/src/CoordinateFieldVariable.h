/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**   Owen L. Kaluza, Software Developer, Monash University. (owen.kaluza@monash.edu)
**   Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**   Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**   Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**   Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**   Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**   Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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



#ifndef __StgDomain_Utils_CoordinateFieldVariable_h__
#define __StgDomain_Utils_CoordinateFieldVariable_h__

   #define MAX_FIELD_COMPONENTS 9

   /** Textual name of this class */
   extern const Type CoordinateFieldVariable_Type;

   /** CoordinateFieldVariable contents */
   #define __CoordinateFieldVariable \
      /* General info */ \
      __FieldVariable \
      \
      /* Virtual info */ \
      FieldVariable*  fieldVariable;       \
      Name            fieldVariableName;   \
      Axis            axis;                \

   struct CoordinateFieldVariable { __CoordinateFieldVariable };
   
   CoordinateFieldVariable* _CoordinateFieldVariable_DefaultNew( );

   /** Private Constructor */
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define COORDINATEFIELDVARIABLE_DEFARGS \
      FIELDVARIABLE_DEFARGS

   #define COORDINATEFIELDVARIABLE_PASSARGS \
      FIELDVARIABLE_PASSARGS

   CoordinateFieldVariable* _CoordinateFieldVariable_New( COORDINATEFIELDVARIABLE_DEFARGS );

   void _CoordinateFieldVariable_Init(
      void*           fieldVariable,
      Axis            axis);

   void _CoordinateFieldVariable_Delete( void* variable );

   void _CoordinateFieldVariable_Print( void* _fieldVariable, Stream* stream );

   void* _CoordinateFieldVariable_Copy(
      void*   fieldVariable,
      void*   dest,
      Bool    deep,
      Name    nameExt,
      PtrMap* ptrMap );

   void _CoordinateFieldVariable_AssignFromXML(
      void*                 fieldVariable,
      Stg_ComponentFactory* cf,
      void*                 data );

   void _CoordinateFieldVariable_Build( void* fieldVariable, void* data );

   void _CoordinateFieldVariable_Execute( void* variable, void* data );

   void _CoordinateFieldVariable_Destroy( void* variable, void* data );

   void _CoordinateFieldVariable_Initialise( void* variable, void* data );

   InterpolationResult _CoordinateFieldVariable_InterpolateValueAt(
      void*   fieldVariable,
      Coord   coord,
      double* value );

   double _CoordinateFieldVariable_GetMinLocalFieldMagnitude( void* fieldVariable );

   double _CoordinateFieldVariable_GetMaxLocalFieldMagnitude( void* fieldVariable );

   void _CoordinateFieldVariable_CacheMinMaxGlobalFieldMagnitude( void* fieldVariable );

   void  _CoordinateFieldVariable_GetMinAndMaxLocalCoords( void* fieldVariable, Coord min, Coord max );

   void  _CoordinateFieldVariable_GetMinAndMaxGlobalCoords( void* fieldVariable, Coord min, Coord max );

   InterpolationResult _CoordinateFieldVariable_InterpolateValueAt(
      void*   fieldVariable,
      Coord   coord,
      double* value );
   
#endif /* __StgDomain_Utils_CoordinateFieldVariable_h__ */

