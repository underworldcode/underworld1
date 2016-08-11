/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
**
** Copyright (C), 2003, Victorian Partnership for Advanced Computing (VPAC) Ltd,
** 110 Victoria Street, Melbourne, 3053, Australia.
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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StGermain_Utils_Progress_h__
#define __StGermain_Utils_Progress_h__

#define __Progress				\
   __Stg_Class					\
   int rank;					\
   Stream* strm;				\
   char* title;					\
   Bool printTitle;				\
   char* preStr;				\
   int width;					\
   int start;					\
   int end;					\
   int pos;					\
   int perc;
struct Progress { __Progress };

	
	#ifndef ZERO
	#define ZERO 0
	#endif

	#define PROGRESS_DEFARGS \
                STG_CLASS_DEFARGS

	#define PROGRESS_PASSARGS \
                STG_CLASS_PASSARGS

Progress* Progress_New();
Progress* _Progress_New(  PROGRESS_DEFARGS  );
void _Progress_Init( void* self );
void _Progress_Delete( void* self );
void _Progress_Print( void* self, Stream* stream );

void Progress_SetStream( void* self, Stream* strm );
void Progress_SetTitle( void* self, const char* str );
void Progress_SetPrefix( void* self, const char* str );
void Progress_SetRange( void* self, int start, int end );
void Progress_Restart( void* self );
void Progress_Update( void* self );
void Progress_Increment( void* self );

#endif /* __StGermain_Utils_Progress_h__ */

