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
**~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#ifndef __StGermain_Base_Foundation_TimeMonitor_h__
#define __StGermain_Base_Foundation_TimeMonitor_h__

#include <mpi.h>

   extern double Stg_TimeMonitor_t0;
   extern double Stg_TimerWatchCriteria;

   extern const Type Stg_TimeMonitor_Type;
   extern const Type Stg_TimeMonitor_InfoStreamName;
   extern const Type Stg_TimeMonitor_TagType;

   typedef struct {
      double totalSinceInit;
      double dt;
      double aveProcDt;	
      double minProcDt;
      double maxProcDt;
      double percentTM_ofTotal;
      Bool   criterionPassed;
   } TimeMonitorData;

   typedef struct {
      double   t1;
      double   t2;
      char*    tag;
      Bool     criteria;
      Bool     print;
      MPI_Comm communicator;
   } Stg_TimeMonitor;

   void Stg_TimeMonitor_Initialise();
   void Stg_TimeMonitor_Finalise();
   void Stg_TimeMonitor_SetTimerWatchCriteria( double ratioOfTotalTime );

   Stg_TimeMonitor* Stg_TimeMonitor_New( char* tag, Bool criteria, Bool print, MPI_Comm communicator );

   void Stg_TimeMonitor_Delete( Stg_TimeMonitor* tm );

   void Stg_TimeMonitor_Begin( Stg_TimeMonitor* tm );
   /* As well as printing, will also store all relevant info in the passed-in tmData reference */
   double Stg_TimeMonitor_End( Stg_TimeMonitor* tm, TimeMonitorData* tmData );

#endif
