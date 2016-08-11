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
**
**
** Assumptions:
**
** Comments:
**
**
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
#ifndef __StGermain_Base_Automation_OneOffComponentFactory_h__
#define __StGermain_Base_Automation_OneOffComponentFactory_h__

	extern const Type OneOffComponentFactory_Type;
	
	#define __OneOffComponentFactory \
		/* General info */ \
		__Stg_ComponentFactory \
		\
		/* Virtual info */ \
		\
		/* Class info */ \
		
	struct OneOffComponentFactory { __OneOffComponentFactory };
	
	/*--------------------------------------------------------------------------------------------------------------------------
	** Constructor
	*/
	/* Creation implementation */
	OneOffComponentFactory* OneOffComponentFactory_New( );
	
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define ONEOFFCOMPONENTFACTORY_DEFARGS \
                STG_COMPONENTFACTORY_DEFARGS

	#define ONEOFFCOMPONENTFACTORY_PASSARGS \
                STG_COMPONENTFACTORY_PASSARGS

	OneOffComponentFactory* _OneOffComponentFactory_New(  ONEOFFCOMPONENTFACTORY_DEFARGS  );
	
	/*-----------------------------------------------------------------------------------------------------------------
	** Virtual functions
	*/

	void _OneOffComponentFactory_Delete( void* OneOffComponentFactory );
	
	void _OneOffComponentFactory_Print( void* OneOffComponentFactory, Stream* stream );
	
	/*-----------------------------------------------------------------------------------------------------------------
	** Functions
	*/
	Dictionary_Entry_Value* _OneOffComponentFactory_GetDictionaryValue( 
			void* cf, Name componentName, Dictionary_Entry_Key key, Dictionary_Entry_Value* defaultVal ) ;

	/** Function to get *numerical* values from component's dictionary
	 * if the key in the sub-dictionary gives a string then it assumes 
	 * that this is a key that points to the root dictionary */
	Dictionary_Entry_Value* _OneOffComponentFactory_GetNumericalValue( void* cf, Name componentName, Dictionary_Entry_Key key, Dictionary_Entry_Value* defaultVal ) ;

	double _OneOffComponentFactory_GetDouble( void* cf, Name componentName, Dictionary_Entry_Key key, const double defaultVal ) ;
	int _OneOffComponentFactory_GetInt( void* cf, Name componentName, Dictionary_Entry_Key key, const int defaultVal ) ;
	unsigned int _OneOffComponentFactory_GetUnsignedInt( void* cf, Name componentName, Dictionary_Entry_Key key, const unsigned int defaultVal);
	Bool _OneOffComponentFactory_GetBool( void* cf, Name componentName, Dictionary_Entry_Key key, const Bool defaultVal ) ;
	char* _OneOffComponentFactory_GetString( void* cf, Name componentName, Dictionary_Entry_Key key, const char* const defaultVal ) ;
	
	double _OneOffComponentFactory_GetRootDictDouble( void* cf, Dictionary_Entry_Key key, const double defaultVal ) ;
	int _OneOffComponentFactory_GetRootDictInt( void* cf, Dictionary_Entry_Key key, const int defaultVal ) ;
	unsigned int _OneOffComponentFactory_GetRootDictUnsignedInt( void* cf, Dictionary_Entry_Key key, const unsigned int defaultVal);
	Bool _OneOffComponentFactory_GetRootDictBool( void* cf, Dictionary_Entry_Key key, const Bool defaultVal ) ;
	char* _OneOffComponentFactory_GetRootDictString( void* cf, Dictionary_Entry_Key key, const char* defaultVal ) ;

	Stg_Component* _OneOffComponentFactory_ConstructByName( 
		void*			componentFactory, 
		Name			componentName, 
		Type			type, 
		Bool			isEssential,
		void*			data ) ;
	Stg_Component* _OneOffComponentFactory_ConstructByKey( 
		void*			componentFactory, 
		Name			parentComponentName, 
		Dictionary_Entry_Key	componentKey, 
		Type			type, 
		Bool			isEssential,
		void*			data ) ;
	Stg_Component* _OneOffComponentFactory_ConstructByNameWithKeyFallback( 
		void*			componentFactory, 
		Name			parentComponentName, 
		Name			componentTrialName, 
		Dictionary_Entry_Key	componentTrialKey, 
		Type			type, 
		Bool			isEssential,
		void*			data ) ;
	Stg_Component** _OneOffComponentFactory_ConstructByList( 
		void*			componentFactory, 
		Name			parentComponentName, 
		Name			listName, 
		unsigned int		maxComponents,
		Type			type,
		Bool			isEssential,
		unsigned int*		componentCount,
		void*			data );

	void OneOffComponentFactory_DocumentType( void* cf, Type type ) ;

#endif /* __StGermain_Base_Automation_OneOffComponentFactory_h__ */

