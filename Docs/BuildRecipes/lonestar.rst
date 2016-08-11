===================================
 Underworld 1.5.x build on lonestar
===================================

**Date**  21/04/2011

**URL** tg-login.lonestar.tacc.teragrid.org

Modules to load
------------------------------

::

    $ module load hdf5/1.8.5
    $ module load petsc/3.1
    $ module load python

   
List of loaded modules
------------------------------
::
                                                                             
    $ module list
    Currently Loaded Modules:
    
      1) TACC-paths     4) intel/11.1    7) tar/1.22  10) hdf5/1.8.5
      2) Linux          5) mvapich2/1.6  8) cluster   11) petsc/3.1
      3) cluster-paths  6) gzip/1.3.12   9) TACC      12) python/2.7.1
     

Repository state
------------------------------
::

    $ ./identify-all.sh 

    7968bc4cd2bd tip .
    c1717962fbaf ./config
    340a6cf0f3a6 tip ./credo
    ee826549b612 tip ./Experimental
    1cfdf3316e6d tip ./gLucifer
    39378eb98a7b ./PICellerator
    a8cd0056c8de ./StgDomain
    347c1f15215e ./StGermain
    fd004dd16efb ./StgFEM
    0b60ff883756+ tip ./Underworld
    

Configuration instructions
------------------------------
(note: run ./configure.py --help for configuration options list)
::

    $ ./configure.py --hdf5-dir=/opt/apps/intel11_1/hdf5/1.8.5 --osmesa-dir=/home1/01646/jgiordan/utils/opt/Mesa-7.8.1/ --libavcodec-dir=/home1/01646/jgiordan/utils/opt/libavcodec/ --petscext-dir=/work/01646/jgiordan/utils/opt/petscext-3.1/


Build instructions
------------------------------
::

    $ ./scons.py


Runtime instructions
------------------------------

The above module state should be reproduced via the runtime PBS script to ensure access to the required/correct libraries


