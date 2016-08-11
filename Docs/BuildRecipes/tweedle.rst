==============================
 Underworld 1.5.1 build on tweedle
==============================

**Date**  1/2/2011

**URL** tweedle.maths.monash.edu

Modules to load
------------------------------

::

    $ module purge
    $ module load mercurial/1.6.3
    $ module load underworld
    $ module rm underworld/dev
    $ module load hdf5 mesa

   
List of loaded modules
------------------------------
::
                                                                             
    $ module list

    Currently Loaded Modulefiles:
      1) python/2.7                        4) petsc/3.0.0-p10-openmpi-1.4-opt
      2) mercurial/1.6.3                   5) hdf5/1.8.5-patch1
      3) openmpi/openmpi-1.4               6) mesa/7.8.1

Repository state
------------------------------
::

   $ ./identify-all.sh 

   b02b70a51238 (1.5.x) 1.5.1 .
   dc3d3492f28c (1.5.x) 1.5.1 ./config
   18dd346bc066 (1.5.x) 1.5.1 ./credo
   c9c8054cca7b (1.5.x) 1.5.1 ./Experimental
   030ae942c731 (1.5.x) 1.5.1 ./gLucifer
   2167d665248f (1.5.x) 1.5.1 ./PICellerator
   6bf7cd95dfa2 (1.5.x) 1.5.1 ./StgDomain
   38383ef21982 (1.5.x) 1.5.1 ./StGermain
   0d187e5fab59 (1.5.x) 1.5.1 ./StgFEM
   890457d6b221 (1.5.x) 1.5.1 ./Underworld

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

N/A

