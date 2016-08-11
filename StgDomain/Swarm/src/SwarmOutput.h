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

#ifndef __StgDomain_Swarm_SwarmOutput_h__
#define __StgDomain_Swarm_SwarmOutput_h__

	/** Textual name of this class */
	extern const Type SwarmOutput_Type;
	
	typedef void    (SwarmOutput_PrintHeaderFunction) ( void* swarmOutput, Stream* stream, Particle_Index lParticle_I, void* data );
	typedef void    (SwarmOutput_PrintDataFunction) ( void* swarmOutput, Stream* stream, Particle_Index lParticle_I, void* data );
	
	/** SwarmOutput contents */
	#define __SwarmOutput \
		/* General info */ \
		__Stg_Component \
		\
		AbstractContext*				   context;		  \
		/* Virtual info */ \
		SwarmOutput_PrintHeaderFunction*                   _printHeader;          \
		SwarmOutput_PrintDataFunction*                     _printData;            \
		\
		/* Member info */ \
		Swarm*                                             swarm;                 \
		Name                                               baseFilename;          \
		ExtensionInfo_Index                                handle;                \
		Variable*                                          globalIndexVariable;   \
		Index                                              columnWidth;           \
		char*                                              borderString;          \
		char*                                              doubleFormatString;    \

	struct SwarmOutput { __SwarmOutput };	

	/** Creation implementation */
	SwarmOutput* SwarmOutput_New(		
		Name                                               name,
		void*                                              context,
		Swarm*                                             swarm,
		Name                                               baseFilename,
		unsigned int                                       columnWidth, 
		unsigned int                                       decimalLength,
		char*                                              borderString );		
	
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define SWARMOUTPUT_DEFARGS \
                STG_COMPONENT_DEFARGS, \
                SwarmOutput_PrintHeaderFunction*  _printHeader, \
                SwarmOutput_PrintDataFunction*      _printData

	#define SWARMOUTPUT_PASSARGS \
                STG_COMPONENT_PASSARGS, \
	        _printHeader, \
	        _printData  

	SwarmOutput* _SwarmOutput_New(  SWARMOUTPUT_DEFARGS  );

	/** Member initialisation implementation */
	void _SwarmOutput_Init( 
		SwarmOutput*                                       self,
		void*                                              context,
		Swarm*                                             swarm,
		Name                                               baseFilename,
		unsigned int                                       columnWidth, 
		unsigned int                                       decimalLength,
		char*                                              borderString );
	
	void _SwarmOutput_Delete( void* swarmOutput ) ;
	void _SwarmOutput_Print( void* _swarmOutput, Stream* stream ) ;
	void* _SwarmOutput_Copy( void* swarmOutput, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );
	#define SwarmOutput_Copy( self ) \
		(SwarmOutput*)Stg_Class_Copy( self, NULL, False, NULL, NULL )
	
	void* _SwarmOutput_DefaultNew( Name name );
	void _SwarmOutput_AssignFromXML( void* swarmOutput, Stg_ComponentFactory* cf, void* data ) ;
	void _SwarmOutput_Build( void* swarmOutput, void* data ) ;
	void _SwarmOutput_Initialise( void* swarmOutput, void* data ) ;
	void _SwarmOutput_Execute( void* swarmOutput, void* data ) ;
	void _SwarmOutput_Destroy( void* swarmOutput, void* data ) ;

	void _SwarmOutput_PrintHeader( void* swarmOutput, Stream* stream, Particle_Index lParticle_I, void* data ) ;
	void _SwarmOutput_PrintData( void* swarmOutput, Stream* stream, Particle_Index lParticle_I, void* data ) ;
	
	/** Virtual Function Wrappers */
	void SwarmOutput_Execute( void* swarmOutput, void* context ) ;
	void SwarmOutput_PrintHeader( void* swarmOutput, Stream* stream, Particle_Index lParticle_I, void* context ) ;
	void SwarmOutput_PrintData( void* swarmOutput, Stream* stream, Particle_Index lParticle_I, void* context ) ;

	void _SwarmOutput_SetGlobalIndicies( void* swarmOutput, void* data ) ;
	void SwarmOutput_PrintString( void* swarmOutput, Stream* stream, char* string ) ;
	void SwarmOutput_PrintDouble( void* swarmOutput, Stream* stream, double value ) ;
	void SwarmOutput_PrintTuple( void* swarmOutput, Stream* stream, double* value, Index size );
	#define SwarmOutput_PrintValue( swarmOutput, stream, value )\
		SwarmOutput_PrintDouble( swarmOutput, stream, (double) ( value ) )

#endif /* __StgDomain_Swarm_SwarmOutput_h__ */

