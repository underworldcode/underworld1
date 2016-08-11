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
**	Represents a Variable that is a discretisation of a swarm-like physical property over a spatial domain.
**
** Assumptions:
**	The function interfaces assume the spatially disc. variable is stored as a double
**	(it can't be an int because its an approximation to a continuous variable right?)
**
** Comments:
**	Abstract class that defines an interface to use when accessing spatially discretised
**	swarm variables.
**
**	This means that e.g. visualisation code can be written to use this class,
**	and doesn't have to worry exactly how the variable is discretised - that will be
**	done by the back-end implementation of this class.
**
**	The name comes from the definition of "swarm" in the physics domain: A region of space
**	characterized by a physical property, such as gravitational or electromagnetic force or
**	fluid pressure, having a determinable value at every point in the region.
**
**	TODO: should it have a ptr to the Variable its based on at this level?
**	doesn't make sense at the moment as the FeVariable used a \
**	doflayout rather than a variable -> but may in future... 
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StgDomain_Utils_OperatorSwarmVariable_h__
#define __StgDomain_Utils_OperatorSwarmVariable_h__

	#define MAX_DOF 9

	/** Textual name of this class */
	extern const Type OperatorSwarmVariable_Type;

	typedef void (OperatorSwarmVariable_UnaryOperatorFunction) ( void* swarmVariable, double* value0, double* result );
	typedef void (OperatorSwarmVariable_BinaryOperatorFunction) ( void* swarmVariable, double* value0, double* value1, double* result );
	
	/** OperatorSwarmVariable contents */
	#define __OperatorSwarmVariable \
		/* Parent info */ \
		__SwarmVariable \
		\
		/* Other info */ \
		Operator*			_operator; \
		Index					swarmVariableCount; \
		SwarmVariable**	swarmVariableList;

	struct OperatorSwarmVariable { __OperatorSwarmVariable };



	/** Shortcut constructors */
	OperatorSwarmVariable* OperatorSwarmVariable_NewUnary(
		Name					name,
		AbstractContext*	context,
		void*					_swarmVariable,
		Name					operatorName );

	OperatorSwarmVariable* OperatorSwarmVariable_NewBinary(
		Name					name,
		AbstractContext*	context,
		void*					_swarmVariable1,
		void*					_swarmVariable2,
		Name					operatorName );
	
	/* Public Constructor */
	void* _OperatorSwarmVariable_DefaultNew( Name name );

	OperatorSwarmVariable* OperatorSwarmVariable_New( 
		Name										name,
		AbstractContext*						context,
		SwarmVariable_ValueAtFunction*	valueAt,
		Name										operatorName,
		Index										swarmVariableCount,
		SwarmVariable**						swarmVariableList );

	/** Private Constructor */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define OPERATORSWARMVARIABLE_DEFARGS \
                SWARMVARIABLE_DEFARGS

	#define OPERATORSWARMVARIABLE_PASSARGS \
                SWARMVARIABLE_PASSARGS

	OperatorSwarmVariable* _OperatorSwarmVariable_New(  OPERATORSWARMVARIABLE_DEFARGS  ); 

	void _OperatorSwarmVariable_Init( void* _swarmVariable, Name operatorName, Index swarmVariableCount, SwarmVariable** swarmVariableList );

	void _OperatorSwarmVariable_Delete( void* variable );

	void _OperatorSwarmVariable_Print( void* _swarmVariable, Stream* stream );

	void* _OperatorSwarmVariable_Copy( void* swarmVariable, void* dest, Bool deep, Name nameExt, PtrMap* ptrMap );

	void _OperatorSwarmVariable_AssignFromXML( void* swarmVariable, Stg_ComponentFactory* cf, void* data );

	void _OperatorSwarmVariable_Build( void* swarmVariable, void* data );

	void _OperatorSwarmVariable_Execute( void* variable, void* data );

	void _OperatorSwarmVariable_Destroy( void* variable, void* data );

	void _OperatorSwarmVariable_Initialise( void* variable, void* data );

	void _OperatorSwarmVariable_ValueAt( void* swarmVariable, Particle_Index lParticle_I, double* value );

	double _OperatorSwarmVariable_GetMinGlobalMagnitude( void* swarmVariable );

	double _OperatorSwarmVariable_GetMaxGlobalMagnitude( void* swarmVariable );

	void _OperatorSwarmVariable_UnaryValueAt( void* swarmVariable, Particle_Index lParticle_I, double* value );

	void _OperatorSwarmVariable_BinaryValueAt( void* swarmVariable, Particle_Index lParticle_I, double* value );


#endif /* __StgDomain_Utils_OperatorSwarmVariable_h__ */

