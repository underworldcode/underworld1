.. _credo-whatsnew:

*******************
What's new in CREDO
*******************

This page summarises what's new in each CREDO version.

(See the files in the "changelogs" sub-directory of the CREDO distribution for
full ChangeLogs based on Mercurial commits.)

new in credo-0.1.3
==================

An incremental release, the main changes of interest were:

* Improvements to management of timeouts in ModelRuns and Tests
* Added the :mod:`credo.jobrunner` package to handle the mechanics of
  running Models, and refactored the appropiate parts out of ModelRun class.

  * This includes the capability to run models in directories other than
    where the CREDO script is invoked using the "basePath" parameter.
  * And a small hierarchy of :class:`credo.jobrunner.api.ModelRunError`
    exceptions.  
  * Default MPI command used by the MPIJobRunner is `mpiexec`.  

* Improving capabilities of ModelSuite (:mod:`credo.modelsuite`):

  * to generate new sorts of suites based on iterators and
    :class:`credo.modelsuite.StgXMLVariant` classes.
  * to post-process existing ModelSuite results, using
    new funcs :attr:`~credo.modelsuite.ModelSuite.readResultsFromPath`
* Added a :mod:`credo.io.stgcmdline` module to handle simple command-line
  issues.
* Improved the reporting of model run errors from the command line.
* General improvements to ModelRun class:

  * Added a hook to do post-run cleanup
  * Added functions to do pre-run validation in various ways
* :mod:`credo.systest` package improvements:

  * added function to import and run a given set of tests based on test names-
    useful for integration into scons test suites.
  * Added a new System test, HighResReference, based on code Wendy Sharples
    sent through.  
  * New system test and test components to compare images based on Owen
    Kaluza's work.

* Bugfixes:

  * Fixed an issue in _createDefaultModelRun so paramOverride lists are
    not accidentally modified.
  * In the CREDO SCons module, changing to avoid problems compiling
    code on systems with old Python.

* Documentation:

  * More examples/howtos, such as :ref:`credo-examples-joblaunch-pbs` and
    :ref:`credo-examples-run-systest-direct-modifyExistingSuite`
  * Several notes in the FAQ about errors that come up running tests
    occasionally.

new in credo-0.1.2
==================

The main changes in this version were:

* Applied an LGPLv2.1 license to the codebase, and added appropriate 
  Copyright statements.
* Can now over-ride the MPI command used for running models by setting the
  MPI_RUN_COMMAND env variable.
* Updated the :class:`~credo.systest.systestrunner.SysTestRunner` class's 
  XML output to be more like that of the Python Unittest XML suite addon,
  unittest-xml-reporting (which helps Bitten integration). The XML suite
  results are now written to separate sub-files, in a "testLogs" sub
  directory by default.
* Refactored the :class:`~credo.systest.api.SysTest` class 
  hierarchy to simplify it, it's now easier to write sub-classes as they
  all use a default `check` function that checks all TestComponents.
* Added capability to specify a Timeout for system tests, after which time the
  test is deemed to have failed if still running.
* New exception classes:

  * Added a custom exception,
    :class:`~credo.modelrun.ModelRunError`,
    to record if a model failed to run.
  * Added a :class:`~credo.modelrun.ModelRunTimeoutError`,
    (see comments on Timeout above.)

* Added new reduction operators for :mod:`~credo.io.stgfreq` module, `first`
  and `last`, that help with setting up system tests based on this.

* Bug/version fixes:
 
  * Updated the code used to generate model suites so that the itertools.product
    function, which is Python 2.6 onwards, is replaced by similar functionality
    if using Python 2.5
  * Fixed SCons integration stuff to make sure paths are set correctly on all
    machines.
  * Fixed the ability to save plots in non-standard directories from frequent
    output data.

new in credo-0.1.1
==================

The main changes in this version were:

* Change from all tests being directly attached to a 
  :class:`~credo.systest.systestrunner.SysTestRunner` class, to ability
  to define :class:`~credo.systest.api.SysTestSuite` classes
  that could then be run by
  the SysTestRunner. This makes it possible to better control running of
  multiple suites, and also makes the interface closer to Python's
  Unittest module.

* Setting up of conventions so CREDO Suites can be both imported and run as
  part of a collection, or run directly. See
  :ref:`credo-examples-run-systest-direct-importingReqs`.

* Much better integration with SCons, so the user doesn't have to set up special
  environment variables if you just wish to run tests via SCons, and also are
  able to generate proper reports on the different suites that were run. See
  :ref:`credo-examples-run-systest-scons`.

* Several small but useful interface improvements when defining suites and 
  system tests, including:

  * the ability to define a suffix to append to the output directory
    of model runs and sys test suites.
  * The ability to define a solverOpts file to use for each run, that contains
    options to customise the PETSc solver.
  * Making sure the stdout and stderr logs of system tests and models are saved
    properly.
  * Checking that the input files specified for a model are defined correctly.  

* Improved the :mod:`credo.systest.fieldCvgWithScaleTest` module considerably, 
  to be more modular, and also better handle fields that don't have a
  convergence criterion specified. Default criterion were also added for
  common recovered fields (eg recovered Pressure).

* Created a new :mod:`credo.io.stgpath` module, which contains several useful
  path-manipulation utilities.

* Testing of CREDO: improved the unit tests of CREDO itself, so the majority of
  testing dirs now have a "testAll.py" script that runs all the other tests.
