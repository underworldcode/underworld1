/*
**  Copyright 2008 Luke Hodkinson
**
**  This file is part of pcu.
**
**  pcu is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  Foobar is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef pcu_runner_h
#define pcu_runner_h

typedef enum { PCU_RUNNER_FAILS, PCU_RUNNER_ALLPASS } PCU_Runner_Status;

extern int PCU_PRINT_DOCS;

void pcu_runner_init( int argc, char* argv[] );
void pcu_runner_finalise();
PCU_Runner_Status pcu_runner_run();
void _pcu_runner_addSuite( const char* name,
			   void (initfunc)( pcu_suite_t* ),
            const char* moduleDir );
void pcu_runner_associateListener( pcu_listener_t* lsnr );

#define pcu_runner_addSuite( suite, initfunc, moduleDir )  \
   _pcu_runner_addSuite( #suite, initfunc, #moduleDir )

#endif
