===================================
 Underworld 1.5.2 build on msg
===================================

**Date**  21/04/2011

**URL** msgln4.its.monash.edu

Modules to load
------------------------------

::

    $ module load petsc/3.1-p5
    $ module load Mesa/7.2
    $ module load hdf5/1.8.4-p1
    $ module load libav

   
List of loaded modules
------------------------------
::
                                                                             
    $ module list
    Currently Loaded Modules:
    
      1) openmpi/1.4.3   3) Mesa/7.2        5) hdf5/1.8.4-p1   7) geos/2.2.3      9) cmkl/9.1.023
      2) petsc/3.1-p5    4) szip/2.1        6) libav/0.6.2     8) Qt/4.6.2       10) python/2.5.1
     

Repository state
------------------------------
::

    1.5.2 release


Configuration instructions
------------------------------
(note: run ./configure.py --help for configuration options list)
::

    $ ./configure.py --libavcodec-dir=/opt/sw/libav-0.6.2


Build instructions
------------------------------
::

    $ ./scons.py


Runtime instructions
------------------------------

The above module state should be reproduced via the runtime PBS script to ensure access to the required/correct libraries


