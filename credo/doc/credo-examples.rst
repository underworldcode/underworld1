.. _credo-examples:

****************************
Examples of how to use CREDO
****************************

The sections below should help given an overview of how to use CREDO, through
worked examples.

The :ref:`credo-examples-systesting` section explains how to use CREDO to run and
set-up the basic system tests of a StGermain code that supercede the previous
system testing scripts.

The :ref:`credo-examples-analysis` section shows examples of how to configure
and run Underworld runs using CREDO, and analyse/post-process the results.
This is for custom analysis, rather than for addition to the automated
testing system.

The :ref:`credo-examples-scibenchmarking` section shows the more complex
use-case of the code, where scientific benchmarks are set up:
generally requiring
both analysis-style set-up of models to run, while also using the system
testing features to allow automated regular running of this benchmark.

The :ref:`credo-examples-joblaunch` section gives examples of how to run CREDO
scripts in different ways, including via PBS.

.. Note::
   CREDO is designed in such a manner that it should be possible to readily
   convert analysis scripts into repeatable system tests, and after
   reading examples of all the sections above you should have a handle
   on how to go about this.

.. _credo-examples-systesting:

Using CREDO for System Testing of StGermain-based codes such as Underworld
==========================================================================

.. toctree::

   examples/run-systest-scons.rst
   examples/register-systests-scons.rst
   examples/run-modify-systest-direct.rst
   examples/write-new-test-component.rst

.. _credo-examples-analysis:

Doing Model analysis with CREDO
===============================

.. toctree::
   :maxdepth: 1

   examples/raytay-run-basic.rst
   examples/raytay-run-suite.rst
   examples/ppc-compare.rst

.. _credo-examples-scibenchmarking:

Scientific Benchmarking using CREDO
===================================

.. toctree::
   :maxdepth: 1

   examples/configure-scibenchmark.rst

.. _credo-examples-joblaunch:

Different ways to launch CREDO scripts
======================================

.. toctree::
   :maxdepth: 1
   
   examples/joblaunch-pbs.rst
