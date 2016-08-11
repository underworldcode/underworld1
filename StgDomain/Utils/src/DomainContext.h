/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
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
*/
/** \file
**  Role:
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgDomain_Utils_DomainContext_h__
#define __StgDomain_Utils_DomainContext_h__
   
   /* Textual name of this class */
   extern const Type DomainContext_Type;
   
   #define __DomainContext \
      /* General info */ \
      __AbstractContext \
      \
      /* Virtual info */ \
      \
      /* DomainContext info */ \
      FieldVariable_Register* fieldVariable_Register; \
      Dimension_Index         dim; \
      Scaling*                scaling;    \
      unsigned                verticalAxis; \

   struct DomainContext { __DomainContext };

   DomainContext* _DomainContext_DefaultNew( Name name );
   
   DomainContext* DomainContext_New( 
      Name        name,
      double      start,
      double      stop,
      MPI_Comm    communicator,
      Dictionary* dictionary );
   
   /** Creation implementation / Virtual constructor */
   
   #ifndef ZERO
   #define ZERO 0
   #endif

   #define DOMAINCONTEXT_DEFARGS \
      ABSTRACTCONTEXT_DEFARGS

   #define DOMAINCONTEXT_PASSARGS \
      ABSTRACTCONTEXT_PASSARGS

   DomainContext* _DomainContext_New( DOMAINCONTEXT_DEFARGS );
   
   /** Initialisation implementation */
   void _DomainContext_Init( DomainContext* self );

   /* Virtual Functions ------------------------------------------------------------------------------------------------*/

   void _DomainContext_AssignFromXML( void* context, Stg_ComponentFactory* cf, void* data );
   
   /* Stg_Class_Delete implementation */
   void _DomainContext_Delete( void* context );
   void _DomainContext_Destroy( void* component, void* data );
   
   /* Print implementation */
   void _DomainContext_Print( void* context, Stream* stream );
   
   void _DomainContext_SetDt( void* context, double dt ) ;

#endif /* __DomainContext_h__*/

