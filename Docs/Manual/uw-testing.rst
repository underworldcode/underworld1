.. _uw-testing:

Tests
=====

Tests are an important part of any software project, both for ensuring an installation is running as
expected, and for providing examples of expected behaviour. Underworld provides an extensive set
of tests, grouped into several categories.

Running
-------

The tests in Underworld are currently categorised into `unit`, `integration` and `convergence` tests.
The tests are spread across StGermain, StgDomain, StgFEM, PICellerator and Underworld. The tests
can be executed from the root directory of the Underworld installation where `./scons.py` and 
`./configure.py` are normally executed. For example, in *$PATHTOUNDERWORLD*.

The following list outlines the type of test suites and how to run them:

 * *./scons.py check* - This will run the unit and low-resolution integration tests
   only.
 * *./scons.py check-complete* - All the configured tests (unit, low-resolution and
   normal-resolution integration, convergence).
 * *./scons.py check-unit* - Unit tests only.
 * *./scons.py check-convergence* - Convergence tests only.
 * *./scons.py check-integration*- Normal-resolution integration tests only.  
 * *./scons.py check-lowres* - Low-resolution version of the integration tests only.

Unit Tests
----------

The unit tests are encapsulated into a binary executable for each of the components: StGermain,
StgDomain, StgFEM, PICellerator and Underworld and can be located under the
*$PATHTOUNDERWORLD/build/tests* directory.

Executing the following commands will go to the unit test directory and run the test suites for
each of the components::
 
 cd $PATHTOUNDERWORLD/build/tests
 ./testStGermain
 ./testStgDomain
 ./testStgFEM
 ./testPICellerator
 ./testUnderworld

Underworld uses a custom unit testing system called PCU *(Parallel C-Unit)*, to provide and
manage these tests. More information about PCU, particularly relevant for those developing new
tests, can be found at the `PCU Documentation <https://www.underworldproject.org/pcu-doc/>`_.

CREDO
-----

CREDO is benchmarking and analysis tool for Underworld. It provides methods and workflows on
running Underworld. See the `Underworld Documentation`_ page for links on the CREDO documentation. 

CREDO provies functionality to users to:

 * Run one or more Underworld models in a defined, repeatable way.
 * Define overrides, or modifications to apply to the models.
 * Require certain analysis operations, or tests, to be applied to the models.
 * Post-process the results of the Underworld outputs for scientific analysis.

Example CREDO script and workflow can be found at::

 $PATHTOUNDERWORLD/Underworld/InputFiles/credo-rayTayBasic.py
