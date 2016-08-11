*****************************
Underworld Release Change Log
*****************************

Underworld-1.7
==============

* General stability improvements and bux fixes to codebase and build system.

* New Features:
  * Semi-Lagrangian advection diffusion:  improved scheme for advection-diffusion problems.
  * RateFieldTimeIntegrator:  allows lagrangian integration of provided ratefield.
  * FieldValueShape:  Creates an Stg_Shape using a provided field.  At any location, field values within user provided delimiting values are considering inside the shape.
  * BelowHeightField:  Creates an Stg_Shape using a provided Heightfield surface.
  * BetweenHeightField:  Creates an Stg_Shape using two provided HeightField surfaces.
  * ASCIIParticleLayout:  Creates a particle layout using user provided csv data.
  * SPHSwarm:  creates an SPHSwarm useful for creating fields from arbitrarily located particles.
  * SPHFieldVariable: creates a field variable using SPHSwarms.
  * MaterialIndexField: creates a nodal field using the particle material index values.
  * Various new matrix assembly terms.
  * Many other minor additions.

* Additional information in the flattened XML file.
  * If mercurial libraries are available and Underworld is built from source,
    the flattened XML file will contain the repository details (StGermain,
    StgDomain, etc.): revision, path and branch. This provides a 'snapshot'
    to the state of the checkout repositories when Underworld is ran.
  * Source information from where each parameter are referenced is also now
    included in the flattened XML file.

* gLucifer 2 release:
  * All output from the gLucifer toolbox is to a render database (output/gLucifer.gldb by default), allowing removal of all graphics related dependencies from Underworld.
  * Database output file can then be provided as input to gLucifer (interactive viewer) or gLuciferOS (offscreen renderer) to view or produce images/video.
  * Major improvements to rendering, image and video output quality.
  * Off-screen renderer is run automatically to produce images as simulations run, although this behaviour can be disabled.
  * See manual and http://underworldproject.org/forums/viewtopic.php?f=26&t=41 for details of new features.
  * Changes logged here https://www.underworldproject.org/hg/gLucifer ]

Underworld-1.6
==============

* OperatorFeVariable parameter rejig and clean-up.
  [ Changeset https://www.underworldproject.org/hg/StgFEM/rev/535f4ad92f80 ]
  [ Changeset https://www.underworldproject.org/hg/StgFEM/rev/1bafcdfbe165 ]

* FieldTest component rejig for explicit generation of analytic/reference fields.
  [ Changeset https://www.underworldproject.org/hg/StgFEM/rev/feeb718c9938 ]

* Added registry for storing analytic functions.
  [ Changeset https://www.underworldproject.org/hg/StGermain/rev/74c2de010696 ]

* gLucifer: Continued improvements to rendering output, database export using sqlite3 to write 3d model data to a file,
  which can be viewed later using the gLucifer Viewer.
  [ See: https://www.underworldproject.org/hg/gLucifer ]

Underworld-1.5.2
================

* Parallel bug fix for HDF5 IC condition.

* Fix for libavcodec on mac static builds.

Underworld-1.5.1
================

* Updated the Underworld_SwarmOutput component to handle the case where no (or an empty) 
  'FeVariable' list is applied. 
  [ Changeset https://www.underworldproject.org/hg/Underworld/rev/077f42ff2d8c ]

* Added error checks for unit conversion. 
  [ Changeset https://csd.vpac.org/hg/StGermain/rev/38383ef21982 ]

* Added 'visualOnly' flag for re-visualizing checkpointed data.
  Flag can be set via command line, (e.g --visualOnly=1) or
  via xml, (e.g <param name="visualOnly"> True </param> ). 
  [ Changeset https://csd.vpac.org/hg/StGermain/rev/0f5f9335ceac ]

* Fixed static build issue on MACs.
  [ Changeset https://csd.vpac.org/hg/StGermain/rev/3e22a8c80234 ]

* Added new 'latent heat' scaling UnitDefinition_Type.
  [ Changeset https://csd.vpac.org/hg/StGermain/rev/ddfbb6475bd6 ]

* Fixed FeVariable_ReadFromFileWithMesh failing issue due to mesh algorithms.
  [ Changeset https://csd.vpac.org/hg/StgFEM/rev/6c905d892ef0 ]

* Added new functionality to XDMF Generator to create the following files:
  XDMF.temporalAll.xmf - load this in paraview (or other) and you can step through time for both swarms and fields.
  XDMF.temporalSwarms.xmf - load this in paraview (or other) and you can step through time for swarms.
  XDMF.temporalFields.xmf - load this in paraview (or other) and you can step through time for fields.
  XDMF.FilesSwarm.xdmf - file list of all timesteps requried for viewing swarms.
  XDMF.FilesField.xdmf - file list of all saved timesteps required for viewing fields.

* Fixed checkpointing failure on multiple procs due to redundant OperatorFeVariable in the
  FieldVariable_Registry.

* Fixed AdvectionCorrection plugin bug.
  [ Changeset https://www.underworldproject.org/hg/Underworld/rev/74493b98b446 ]

* Updated BuoyancyIntegrals plugin.
  [ Changeset https://www.underworldproject.org/hg/Underworld/rev/6ce6abfe8ac8 ]

* Fixed Particle Extension Algorithms extending multiple times if
  multiple version of DruckerPrager were instanciated.
  [ Changeset https://www.underworldproject.org/hg/Underworld/rev/0a700ad65814 ]

Underworld-1.5
==============

* Testing system has been migrated to CREDO toolkit.
  For more information on CREDO (see `CREDO Documentation
  <https://www.underworldproject.org/credo-doc/index.html>`_) 

* './scons.py install' will now install to a PREFIX location.

* Recovery Equilibrium of Patches (REP) method is available for stress and strain rate recovery.
  For more information see 'Recovery Methods' in the Cookbook section of the manual.

* 'Scaling' functionality is available.
  Now input parameters and output values can be in user specified SI units.
  See 'Scaling' in the cookbook section of the manual for details.

* New hdf5 initial conditions method available, see Manual section 'HDF5 Initial Condition'.

* Underworld can now run GALE inputfiles (as of GALE 1.4)
  located in Underworld/InputFiles/GaleModels/.

* Stress Boundary Conditions are now available, StressBCs,
  examples Underworld/InputFiles/ex_stress_bcs.xml.

* Compatible with petsc-3.1 and petsc-3.0.

* Significant gLucifer changes:
  * Default image and movie output objects defined: lucDefaultImageOutput and lucDefaultVideoOutput.
  * Experimental vis database export feature added, requires SQLite3. Disabled by default.
  * Optimisations for isosurface and scalar field sampling speed, Isosurface sampleGlobal=true can be set to disable.
  * Camera: autoZoomTimestep parameter added, is now enabled by default.
  * AutoZoomMargin is no set by margin parameter in Viewport.
  * Default window width/height fits all the viewports at 400x400 pixels each.
  * ColourMaps: allow values assigned to colours by placing a value in parentheses before the colour.
  * CrossSection object added, derived objects can specify position in format x/y/z=min/max/N/N%
  * Capture object created for vis output in plugins.
  * 2d Plot object created for simple line graphs.
  * Tracer 3d rendering improved features, fixed to checkpoint tracer history for vis. 
  * Multisample support added for anti-aliasing.
  * Camera coordX,coordY,coordZ parameters deprecated although coordZ can still be used for backward compatibility.
  * Added translateX,Y,Z parameters to camera to move the model around in 3d space.
  * Added rotateX,Y,Z parameters to camera to rotate the model around in 3d space.
  * gLucifer is no longer a plugin, include in the toolboxes "import" list.
  * More off-screen rendering options for X11 added.

Underworld-1.4
==============

  * New configuration step in installation process, './configure.py -h' for help.

  * Changes to the XML user interface, see the file XMLChanges_1.4 for details.

  * Multigrid solver is now available.

  * Superconvergent Patch Recovery (SPR) Method is now available.

  * Significant checkpointing improvements, hdf5 format is now the standard format,
    see manual for details.

  * Testing have been enabled. Currently 443 unit tests + 105 system tests.
    Automatic testing is run periodically on multiple platforms with Shinkou
    system, (see `Shinkou Continuous Integration System
    <https://www.underworldproject.org/engineering/build>`_).

  * New constructor/destroy/delete phases for all components and plugins.

  * Toolboxes / Context code tangle has been addressed.

  * Bugfixes and memory cleaning changes also.
