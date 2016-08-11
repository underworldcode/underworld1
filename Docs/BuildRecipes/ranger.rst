===================================
 Underworld 1.6.x build on ranger
===================================

**Date**  29/06/2011

**URL** ranger.tacc.teragrid.org

Modules to load
------------------------------

::
    module load python/2.5.2
    module load phdf5/1.8.2
    module load petsc/3.0.0
    module load vis/1.0 mesa

List of loaded modules
------------------------------
::
                                                                             
    module list
    
    Currently Loaded Modules:
      1) TACC-paths           7) TERAGRID-paths    13) tgusage/3.0     19) TERAGRID-DEV  25) python/2.5.2
      2) Linux                8) gx-map/0.5.3.3    14) uberftp/2.6     20) CTSSV4        26) phdf5/1.8.2
      3) cluster-paths        9) srb-client/3.4.1  15) tginfo/1.0.1    21) gzip/1.3.12   27) petsc/3.0.0
      4) pgi/7.2-5            10) tg-policy/0.2    16) TERAGRID-BASIC  22) tar/1.22      28) vis/1.0
      5) mvapich/1.0.1        11) tgproxy/0.9.1    17) globus/5.0.4    23) cluster       29) mesa/6.4.2
      6) binutils-amd/070220  12) tgresid/2.3.4    18) GLOBUS-5.0      24) TACC       
     

Repository state
------------------------------
::

    ./identify-all.sh 

    64d7992bd4e3 (1.6.x) .
    6791b1e40bf2 (1.6.x) ./config
    9bfe01f8d21f (1.6.x) ./credo
    6778dc940681 (1.6.x) ./gLucifer
    acc79c536511 (1.6.x) ./PICellerator
    f39c8d032a24 (1.6.x) ./StgDomain
    c855791b20d4 (1.6.x) ./StGermain
    b6edad80e60d (1.6.x) ./StgFEM
    9da7c3414b6a (1.6.x) ./Underworld

Configuration instructions
------------------------------
(note: run ./configure.py --help for configuration options list)
::

    $ ./configure.py --cc=mpicc --cflags=-noswitcherror --hdf5-dir=$TACC_HDF5_DIR --osmesa-dir=$TACC_MESA_DIR --x11=no


Build instructions
------------------------------
::

    $ ./scons.py


Runtime instructions
------------------------------

The above module state should be reproduced via the runtime PBS script to ensure access to the required/correct libraries


