.. _uw-install:

************
Installation
************

Introduction
============

This section provides steps and links on obtaining and setting up Underworld.
For running Underworld on a cluster, see the `Underworld Cluster`_ page.
For running Underworld on the ARCS Grid, see the `Underworld Grid`_ page.

For installation on local machines, Underworld is available as:

 * Binary packages, under Ubuntu and Mac OS X platfroms, no dependencies required.
 * As a stable release source code tarball, dependencies required.
 * From the source code repository, dependencies required.

See the `Underworld Downloads`_ page for more details.

The following instructions assume a beginner’s level of familiarity with the
Unix/Linux operating environment, including:

 * How to open a terminal window
 * How to navigate between directories using a terminal application
 * How to open, edit, save and close a file using an editor through a terminal application

There are many beginners Unix / Linux guides available as textbooks and on the Internet,
including the `Unix Tutorial for Beginners`_.

Getting Help
============

For help, advice or the latest information on Underworld:

 * Have a look through the `Underworld homepage`_.
 * Read the `Underworld FAQ`_ page. 

.. _system-requirements:

System Requirements
===================

Operating System
----------------

Underworld will work and is supported on:

 * Linux-based OS (desktop and supercomputers)
 * Mac OS X (Power PC and Intel) 

.. Note:: Underworld is currently not available on Windows.

Underworld requires:

 * 300-500 MB for a compiled copy of Underworld. (x3 for source code component if using Mercurial)
 * 340 MB approx. for software dependencies.
 * Enough space for your output data (e.g. 500+ MB recommended for medium to large-scale problems).

.. _software-dependencies:

Software Dependencies
---------------------

Underworld relies on a suite of software applications, which need to be installed prior to installing
Underworld. It is recommended that you install each program in the order listed. However, if you
are going to install any of the Visualisation Software or Interactive Visualisation Software listed
further below, install them before configuring PETSc.

See the `Underworld Dependencies`_ page for instructions on how to download and install each program.

When installing Underworld from one of the available Ubuntu binary packages, all dependencies are
automatically downloaded and installed on the system. When using the Mac OS X binary packages,
the dependencies are bundled together with Underworld, so no manual installation of the dependencies
is required.

Required Software
^^^^^^^^^^^^^^^^^

If building Underworld from source either from the tarball or repository checkout, a number of 
software dependencies needs to be installed first.

The following list outlines the required software that needs to be installed prior to configuring
and installing Underworld:

 * **C Compiler** - Needed to compile several dependencies and Underworld; usually installed by
   default on Linux systems. On Macs this can be installed from the Developer Tools package
   that came with your Mac OS. C compilers are *gcc* and *icc*.
 * **MPI Implementation** - Parallel MPI Message-passing library. Underworld supports MPICH and
   OpenMPI.
 
   * *MPICH* - See the `Underworld Dependencies`_ page and `MPICH homepage`_
     for installation and version details.
   * *OpenMPI* - See the `Underworld Dependencies`_ page and `OpenMPI homepage`_
     for installation and version details.
 * **LAPACK and BLAS** - Used to provide basic *numerical kernels* for solving matrices etc. See
   the `Underworld Dependencies`_ page for details on how to install these libraries for various
   platforms.
 * **Python** - Required to compile PETSc and Underworld components.
   To check if you need to install Python, see the `Underworld Dependencies`_ page or the
   `Python homepage <http://www.python.org/>`_.
 * **PETSc** - A set of libraries including equation solvers and assembly routines.
   To Install PETSc see the `Underworld Dependencies`_ page and the
   `PETSc homepage <http://www.mcs.anl.gov/petsc/petsc-as/>`_.
 * **HDF5** - The default application used for checkpointing meshes and particles.
   See the `Underworld Dependencies`_ page for further details.
 * **SCons** - Package for building and compiling Underworld on your system.
   For versions of Underworld greater than 1.2.0, scons is packaged with the Underworld tarball or checkout.

.. _uw-additional-software:

Additional Software
^^^^^^^^^^^^^^^^^^^

To checkout the Underworld code from the repository, the following software is required:

 * **Mercurial** - The version control system used by Underworld. See the `Underworld Dependencies`_ page for further details.

For visualisation output, these additional software are required:

 * **libppng** and/or **libjpeg** and/or **libtiff** - For image output in png, jpeg and tiff formats.
 * **libavcodec** or **libfame** - For producing animations.

For interactive visualisation, these additional windowing software are required:

 * **X11** - Standard on Linux. Comes as an option with *Developer Tools* or separate package on MACs.
   See the `Underworld Dependencies`_ page for further details.
 * **Carbon** - MACs only, comes already installed.
 * **OSMesa** - Can be installed on most systems. Recommended only for *off-screen* rendering as this doesn't
   work with interactive visualisations. See the `Underworld Dependencies`_ page for further details.
 * **SDL** - Can also be used.

To draw the output image inside the created window/s, one of the following additional software are required:

 * **OpenGL** - Comes standard with Linux and MACs platforms.
 * **Mesa** - Comes with OSMesa. See the `Underworld Dependencies`_ page for further details.

To output visualisation data into a database, the following software is required:

 * **SQLite** - Comes standard with MACs. Available as a package from most Linux distros.

.. Note:: For interactive visualisation, don’t use OSMesa.
.. Note:: For cluster installations, only use OSMesa. 

When installing Underworld on a cluster environment, it is recommended to use OSMesa
only, since most clusters will not be running the X11 daemon on compute nodes.
All *input files* can be ran without interactive visualisation, in which case images are still created
and can be opened manually. For further information on *input files*, see :ref:`underworld_modelling`;
gLucifer output, see :ref:`gl-output`.

.. _getting-underworld:

Getting Underworld
==================

See the `Underworld Releases`_ page for instructions on how to download different versions
of Underworld, or go straight to `Underworld Downloads`_ page to download
a tarball of the code straight away.

As a Binary Package (Ubuntu and MACs)
-------------------------------------

Binaries for Underworld are available at the `Underworld Downloads`_ page.
Supported binaries are Ubuntu (.deb) and MAC (.dmg). The Ubuntu binary packages are available in
i686 (32-bit) and amd64 (64-bit) architectures. Make sure that you're downloading the right package
for your computer's architecture. The *input files*, available on the same page,
need to be downloaded separately.

As a Release Tarball
--------------------

Release tarballs are available at the `Underworld Downloads`_ page.

.. Note::
    
    Tarball copy of the code cannot be updated to a later version. There will be
    a separate tarball for each *stable release* of Underworld.

From the Mercurial Repository
-----------------------------

Information on how to check-out Underworld is available at the `Underworld Releases`_ page.
There are two options available: *stable release* and *bleeding-edge*.
Unless you are interested in writing or contributing code, the *stable release* checkout is
the recommended version to use.

.. _getting-underworld-inputfiles:

Getting Underworld Input Files
------------------------------

If you are using the latest *stable release* version of Underworld, the *input files* can be downloaded
from the `Underworld Downloads`_ page. The *input files* are the standard XML files that form the 
models for Underworld. 

Underworld is designed to operate in a multi-user environment, in which users can have their
own files located in their home directory which link to the files downloaded from the repository.
This ensures that conflicts do not arise when running updates to Underworld and reduces the amount of
work required for each user to produce their own working model.

.. _installing-underworld:

Installing Underworld
=====================

The `Underworld Releases`_ page, updated on a regular-basis contains references for installing Underworld.

From a Binary Package (Ubuntu and MACs)
---------------------------------------

Once the binary package has been downloaded (see :ref:`getting-underworld`), 

On Ubuntu:

 1. Double-click on the file to launch the installation manager. 
 2. Click on the *Install Package* option to start the installation.
    This process might take a moment as this step also downloads and installs the required
    dependencies.
 3. Once the installation is finished you can now close the manager.
 4. The *Underworld* executable will be part of the environment so you can now issue *Underworld*
    on the command line and start running your models.

On MAC:

 1. Double-click on the file to mount the package.
 2. Once mounted, Double-click on the package to start the installation process.
 3. Follow the standard prompt for installing the package.
 4. Once the installation is finished you can now close the manager.
 5. The *Underworld* executable will be part of the environment so you can now issue *Underworld*
    on the command line and start running your models.

From Source (Tarball, Repository)
---------------------------------

Once all required and additional dependencies are installed (see :ref:`software-dependencies`),
and the Underworld source code has been acquired either from the tarball or repository (see :ref:`getting-underworld`),
you can now *compile* the code.

 1. First navigate to the source code directory, for example of the directory name is Underworld-1.5::

    cd Underworld-1.5

 2. Configure Underworld, here we're configuring in optimised mode::

    ./configure.py --with-debugging=0

    Typing ``./configure.py --help`` will list all avaiable configuration options.

 3. Compile Underworld::
  
    ./scons.py

See the `Underworld Documentation on SCons <http://www.underworldproject.org/documentation/CompileSCons.html/>`_ for
a complete reference on installing Underworld from source using SCons.

Running & Testing Underworld
----------------------------

After installing Underworld, you can check the build directory for the created executables and libraries.
The build directory will contain the imporant executables, *Underworld* and *FlattenXML*.
To verify your installation:

 * You can check that the executables are present::

    ls Underowrld-1.5/build/bin

 * You can also run the standard tests::

    ./scons.py check > results

   This will run the standard unit and system tests for Underworld.

See :ref:`uw-testing` for more information on Underworld tests.
If you encounter any issues running Underworld or if you get any errors in running the tests and you're
using the *stable release* version of the code, contact users@underworldproject.org, otherwise if you're 
using the *bleeding edge* version of the code, submit your enquiry to development@underworldproject.org.

.. _Underworld homepage: http://www.underworldproject.org
.. _Underworld FAQ: http://www.underworldproject.org/documentation/FAQ.html
.. _Underworld Downloads: http://www.underworldproject.org/downloads.html
.. _Underworld Dependencies: http://www.underworldproject.org/documentation/Dependencies.html
.. _Underworld Releases: http://www.underworldproject.org/documentation/Releases.html
.. _Underworld Cluster: http://www.underworldproject.org/documentation/Cluster.html
.. _Underworld Grid: http://www.underworldproject.org/documentation/UnderworldARCSGrid.html
.. _Unix Tutorial for Beginners: http://www.ee.surrey.ac.uk/Teaching/Unix
.. _MPICH homepage: http://www.mcs.anl.gov/research/projects/mpich2/
.. _OpenMPI homepage: http://www.open-mpi.org/software/ompi/v1.0/

