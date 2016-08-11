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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/** \file
** <b>Role:</b>
**	Prepends the rank of the process which is currently printing.
**
** <b>Assumptions:</b>
**	None.
**
** <b>Comments:</b>
**	None.
**
** <b>Description:</b>
**	Prepends each line printed with printf() with the rank of the process doing the task.
**	If computing as a serial job, RankFormatter will prepend nothing. 
**
**
**/

#ifndef __StGermain_Base_IO_RankFormatter_h__
#define __StGermain_Base_IO_RankFormatter_h__


	/** Textual name for RankFormatter class. */
	extern const Type RankFormatter_Type;

	/** \def __RankFormatter See RankFormatter */
	#define __RankFormatter \
		__LineFormatter
	struct RankFormatter { __RankFormatter };


	/** Creates a new RankFormatter, automatically determining the current rank. */
	StreamFormatter* RankFormatter_New();
	
	/** Init a new RankFormatter, automatically determining the current rank. */
	void RankFormatter_Init( RankFormatter* self );


	/** Constructor interface. */
	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define RANKFORMATTER_DEFARGS \
                LINEFORMATTER_DEFARGS

	#define RANKFORMATTER_PASSARGS \
                LINEFORMATTER_PASSARGS

	RankFormatter* _RankFormatter_New(  RANKFORMATTER_DEFARGS  );

	/** Init interface. */
	void _RankFormatter_Init(
		RankFormatter*			self,
		StreamFormatter_FormatFunction*	_format );
			
	/** Stg_Class_Delete interface. */
	void _RankFormatter_Delete( void* formatter );

	/** Print interface. */
	void _RankFormatter_Print( void* formatter, Stream* stream );


#endif /* __StGermain_Base_IO_RankFormatter_h__ */




