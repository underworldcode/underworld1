==============================
 Underworld 1.6 build on VAYU
==============================

**Date**  29/06/2011

**URL** vayu.nci.org.au

Modules to load
------------------------------

::

    module purge
    module load pbs
    module load mercurial/1.2.1 
    module load python/2.6 
    module load petsc/3.1-p5 
    module load ffmpeg/0.6.1 
    module load hdf5/1.8.5
    module load openmpi/1.4.3
    module load parmetis/3.1.1_old
   
List of loaded modules
------------------------------
::
                                                                             
    module list

    Currently Loaded Modulefiles:
      1) mercurial/1.2.1        3) intel-mkl/10.2.6.038   5) petsc/3.1-p5           7) hdf5/1.8.5
      2) python/2.6             4) parmetis/3.1.1         6) ffmpeg/0.6.1           8) openmpi/1.4.3       9) pbs


Repository state
------------------------------
::

    ./identify-all.sh 

    64d7992bd4e3 (1.6.x) .
    6791b1e40bf2 (1.6.x) ./config
    9bfe01f8d21f (1.6.x) ./credo
    fcb59665e24a (1.6.x) ./Experimental
    6778dc940681 (1.6.x) ./gLucifer
    acc79c536511 (1.6.x) ./PICellerator
    f39c8d032a24 (1.6.x) ./StgDomain
    92ae55a75276 (1.6.x) ./StGermain
    b6edad80e60d (1.6.x) ./StgFEM
    772712c86af5 (1.6.x) ./Underworld
    

Configuration instructions
------------------------------
(note: run ./configure.py --help for configuration options list)
::

    ./configure.py


Build instructions
------------------------------
::

    ./scons.py


Runtime instructions
------------------------------

The above module state should be reproduced via the runtime PBS script to ensure access to the required/correct libraries


