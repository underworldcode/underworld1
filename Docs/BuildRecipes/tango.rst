================================
 Underworld 1.5.2 build on Tango
================================

**Date**  29/4/2011

**URL** tango.vpac.org

Modules to load
------------------------------

::

    $ module purge 
    $ module load petsc/3.0.0-p10-openmpi-gcc
    $ module load hdf5/1.8.5-openmpi-gcc
    $ module load mesa/7.8.1-gcc
    $ module load libfame/0.9.1
    $ module load python/2.6.2-gcc

   
List of loaded modules
------------------------------
::
                                                                             
    $ module list

    Currently Loaded Modulefiles:
     1) gmp/4.3.1                     4) openmpi-gcc/1.4               7) hdf5/1.8.5-openmpi-gcc
     2) mpfr/2.4.2                    5) petsc/3.0.0-p10-openmpi-gcc   8) mesa/7.8.1-gcc
     3) gcc/4.4.5                     6) python/2.6.2-gcc              9) libfame/0.9.1
     

Repository state
------------------------------
Underworld version 1.5.2. Download link 
::

   http://www.underworldproject.org/downloads/underworld-1.5.2/underworld-1.5.2.tar.gz
    

Configuration instructions
------------------------------
(note: run ./configure.py --help for configuration options list)
::

    $ ./configure.py --with-debugging=0


Build instructions
------------------------------
::

    $ ./scons.py


Runtime instructions
------------------------------

The above module state should be reproduced via the runtime PBS script to ensure access to the required/correct libraries

----

================================
 Underworld 1.5.1 build on Tango
================================

**Date**  14/1/2011

**URL** tango.vpac.org

Modules to load
------------------------------

::

    $ module purge 
    $ module rm pgi intel 
    $ module load petsc/3.0.0-p10-openmpi-gcc
    $ module load hdf5/1.8.5-openmpi-gcc
    $ module load mesa/7.7-gcc
    $ module load libfame/0.9.1
    $ module load python/2.6.2-gcc

   
List of loaded modules
------------------------------
::
                                                                             
    $ module list

    Currently Loaded Modulefiles:
      1) modules                       7) petsc/3.0.0-p10-openmpi-gcc
      2) gmp/4.3.1                     8) mesa/7.7-gcc
      3) mpfr/2.4.2                    9) libfame/0.9.1
      4) vpac/config                  10) hdf5/1.8.5-openmpi-gcc
      5) gcc/4.4.5                    11) python/2.6.2-gcc
      6) openmpi-gcc/1.4
     

Repository state
------------------------------
::

    $ ./identify-all.sh 

    544b416806c9 (1.5.x) 1.5.1 .
    dc3d3492f28c (1.5.x) 1.5.1 ./config
    b3ab3463c4fd (1.5.x) 1.5.1 ./credo
    030ae942c731 (1.5.x) 1.5.1 ./gLucifer
    2167d665248f (1.5.x) 1.5.1 ./PICellerator
    412fb804646d (1.5.x) 1.5.1 ./StgDomain
    38383ef21982 (1.5.x) 1.5.1 ./StGermain
    1130986d94e8 (1.5.x) 1.5.1 ./StgFEM
    020ffd19f242 (1.5.x) 1.5.1 ./Underworld
    

Configuration instructions
------------------------------
(note: run ./configure.py --help for configuration options list)
::

    $ ./configure.py


Build instructions
------------------------------
::

    $ ./scons.py


Runtime instructions
------------------------------

The above module state should be reproduced via the runtime PBS script to ensure access to the required/correct libraries

