==============================
 Underworld 1.5 build on VAYU
==============================

**Date**  18/03/2011

**URL** vayu.nci.org.au

Modules to load
------------------------------

::

    $ module load mercurial/1.2.1 
    $ module load python/2.6 
    $ module load petsc/3.1-p5 
    $ module load ffmpeg/0.6.1 
    $ module load hdf5/1.8.5

   
List of loaded modules
------------------------------
::
                                                                             
    $ module list

    Currently Loaded Modulefiles:
      1) rash                   6) openmpi/1.4.3         11) petsc/3.1-p5
      2) pbs                    7) mercurial/1.2.1       12) ffmpeg/0.6.1
      3) dot                    8) python/2.6            13) hdf5/1.8.5
      4) intel-fc/11.1.046      9) intel-mkl/10.2.6.038
      5) intel-cc/11.1.046     10) parmetis/3.1.1

Repository state
------------------------------
::

    $ ./identify-all.sh 

    837f121f1d82 (1.5.x) 1.5.2 .
    bc226dc0a7a4 (1.5.x) tip ./config
    c90ea33c31ba (1.5.x) ./credo
    e1386f68229e (1.5.x) ./gLucifer
    2035d7834734 (1.5.x) ./PICellerator
    585891ff0dec (1.5.x) ./StgDomain
    f9ccfa6c7da5 (1.5.x) ./StGermain
    eea44930d6f1 (1.5.x) ./StgFEM
    739cee621d3f (1.5.x) ./Underworld
    

Configuration instructions
------------------------------
(note: run ./configure.py --help for configuration options list)
::

    $ ./configure.py --with-warning=0


Build instructions
------------------------------
::

    $ ./scons.py


Runtime instructions
------------------------------

The above module state should be reproduced via the runtime PBS script to ensure access to the required/correct libraries


