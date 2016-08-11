/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd, 110 Victoria Street, Melbourne, 3053, Australia.
**
** Authors:
**	Stevan M. Quenette, Senior Software Engineer, VPAC. (steve@vpac.org)
**	Patrick D. Sunter, Software Engineer, VPAC. (pds@vpac.org)
**	Luke J. Hodkinson, Computational Engineer, VPAC. (lhodkins@vpac.org)
**	Siew-Ching Tan, Software Engineer, VPAC. (siew@vpac.org)
**	Alan H. Lo, Computational Engineer, VPAC. (alan@vpac.org)
**	Raquibul Hassan, Computational Engineer, VPAC. (raq@vpac.org)
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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __Geothermal_Base_ConstantField_h__
#define __Geothermal_Base_ConstantField_h__

	/** Textual name of this class */
	extern const Type ConstantField_Type;
	
	/** ConstantField contents */
	#define __ConstantField                          \
		/* General info */                            \
		__FeVariable                                  \
		                                              \
		/* Member info */                             \
		double*                 value;                \
      double                  maxFieldValue;        \
		double                  minFieldValue;        \

	struct ConstantField { __ConstantField };	

	/** Creation implementation */
	void* _ConstantField_DefaultNew( Name name );

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define CONSTANTFIELD_DEFARGS \
                FEVARIABLE_DEFARGS
                
	#define CONSTANTFIELD_PASSARGS \
                FEVARIABLE_PASSARGS  

	ConstantField* _ConstantField_New(  CONSTANTFIELD_DEFARGS  );

	/** Member initialisation implementation */
   ConstantField* ConstantField_New( 
      Name           name,
      Index          fieldComponentCount,
      double*        value,
      FeMesh*        feMesh ) ;

	void _ConstantField_Init( 
   ConstantField*          self,
   Index                   fieldComponentCount,
   double*                 value,
   FeMesh*                 feMesh) ;
	
	void _ConstantField_AssignFromXML( void* ConstantField, Stg_ComponentFactory* cf, void* data ) ;

	void _ConstantField_Build( void* ConstantField, void* data ) ;

	void _ConstantField_Execute( void* ConstantField, void* data ) ;

	void _ConstantField_Destroy( void* ConstantField, void* data ) ;

	void _ConstantField_Initialise( void* ConstantField, void* data ) ;
	
	InterpolationResult _ConstantField_InterpolateValueAt( void* ConstantField, double* coord, double* value );

	/** Implementations of the min and max val functions */
	double _ConstantField_GetMinGlobalFieldMagnitude( void* ConstantField );

	double _ConstantField_GetMaxGlobalFieldMagnitude( void* ConstantField );
	
	/** Implementations of the coord-getting functions */
	void _ConstantField_GetMinAndMaxLocalCoords( void* ConstantField, double* min, double* max ) ;

	void _ConstantField_GetMinAndMaxGlobalCoords( void* ConstantField, double* min, double* max ) ;

   void _ConstantField_CacheMinMaxGlobalFieldMagnitude( void* ConstantField, double* min, double* max ) ;

   void _ConstantField_InterpolateWithinElement( void* _ConstantField, Element_DomainIndex dElement_I, Coord coord, double* value ) ;
   
   void _ConstantField_GetValueAtNode( void* _ConstantField, Node_DomainIndex dNode_I, double* value ) ;

   void _ConstantField_SyncShadowValues( void* _ConstantField ) ;

#endif /* __Geothermal_Base_ConstantField_h__ */

