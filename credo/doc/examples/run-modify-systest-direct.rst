.. _credo-examples-run-systest-direct:

Running CREDO system test suites directly, and how to modify them
-----------------------------------------------------------------

CREDO provides the ability to run suites of system tests directly via the command
line, similar to the interface of the previous SYS scripts system.

To start off with, make sure the necessary environment variables necessary to
run CREDO have been set up, as detailed in :ref:`environment_setup`.

Then check out one of the test suite files in a SysTest directory within the
code. We'll show in this example StgFEM/SysTest/RegressionTests/testAll-new.py:

.. include:: ../../../StgFEM/SysTest/RegressionTests/testAll.py
   :literal:

This Python script uses CREDO to run several system tests, and process their
results. To see it in practice, cd to that directory and then run the script
(since it's set as executable, you don't need to invoke Python explicitly).
You should see output starting with::

  Running System Test Suite for Project 'StgFEM', named 'RegressionTests', containing 33 direct tests and 0 sub-suites:
  Running System test 1/33, with name 'CosineHillRotateBC-analyticTest-np1':
  Writing pre-test info to XML
  Running the 1 modelRuns specified in the suite
  Doing run 1/1 (index 0), of name 'CosineHillRotateBC-analyticTest-np1':
  ModelRun description: "Run the model and generate analytic soln."
  Generating analysis XML:

and finishing with::

  --------------------------------------------------------------------------------
  CREDO System Tests results summary, project 'StgFEM', suite 'RegressionTests':
  Ran 33 system tests, with 33 passes, 0 fails, and 0 errors
  --------------------------------------------------------------------------------

The following sections will explain how the file is set up, and show what the different sections do.


Importing CREDO and creating a testRunner object
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To explain the first few lines of the script, as shown below::

  #!/usr/bin/env python

  from credo.systest import *

  testSuite = SysTestSuite("StgFEM", "RegressionTests")

The first denotes the file as an executable script, using Python.

The next imports everything directly under the credo.systest package for use in
the rest of the Python script - this is a convenience since all the objects
we'll need for the rest of the script, such as various types of System
test classes, are contained here.

The next line creates a SysTestSuite object to use for managing a test suite,
and assigns it to the name 'testSuite'. The 2 arguments when creating the
SysTestSuite are for recording the project the suite belongs to, and a textual
name of the suite.

Note that the SysTestSuite object definition was one of those we imported
with the preceding *import* statement.

.. seealso:: The :class:`credo.systest.api.SysTestSuite` class in the
   API documentation.

Adding system tests to the suite, and configuring them
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Let's look at the next few lines, which declare a set of test models to run,
and add them to the test suite::

  analyticModels = ["CosineHillRotateBC.xml", "CosineHillRotateBC-DualMesh.xml",
      "HomogeneousNaturalBCs.xml", "HomogeneousNaturalBCs-DualMesh.xml",
      "SteadyState1D-x.xml", "SteadyState1D-y.xml",
      "AnalyticSimpleShear.xml"]

  for modelXML in analyticModels:
      for nproc in [1, 2, 4]:
          testSuite.addStdTest(AnalyticTest, modelXML, nproc=nproc)

The analyticModels has been created as a `Python List
<http://docs.python.org/tutorial/introduction.html#lists>`_. This list can then
be used inside the `for` loop below it to add each particular model as an
AnalyticTest using the `addStdTest` method of the testSuite object we created
above.

The addStdTest method uses a special shorthand to add tests to the suite, to
save some of the work from the users in this high-level interface.
Its arguments are:

* Test class: the class of test you're adding. Generally this should be one of
  the standard set imported above, i.e. RestartTest, ReferenceTest,
  AnalyticTest, or AnalyticMultiResTest.
* XML file name(s): list of XML files that make up the model to be used in the
  test. If in the common case this is just one top-level XML file, a single
  string instead of a list is ok.
* Any over-riding options you wish to pass to the test.

In the case of over-riding parameters to the tests, there are 2 main categories:

#. Passing parameters that customise the model itself (paramOverrides)
#. Passing parameters that modify the nature of the test.

We can see an example of the first of these in the next section of the test
script::

  ss0_5Opts = {"defaultDiffusivity":0.5, "A":0.1}
  for nproc in [1, 2, 4]:
      testSuite.addStdTest(AnalyticTest, ["SteadyState1D-x.xml"],
          nproc=nproc, paramOverrides=ss0_5Opts)

... this section means that an AnalyticTest on the `SteadyState1D-x.xml` model
is being added to the suite, with the model being modified by over-riding the
"defaultDiffusivity" and "A" parameters from the values specified in the 
Model XML file, to 0.5 and 0.1 respectively. The `paramOverrides` option is 
a `Python Dictionary <http://docs.python.org/tutorial/datastructures.html#dictionaries>`_.

Here, the paramOverrides option is a dictionary of overrides to perform exactly
the same as described in the section for running a single analysis model through
CREDO below.

With regard to the second option type, there are several options you can use to
over-ride the default behaviour of system tests. 
Please refer to the API section on System Tests for more. The
principle is the same for all of these regarding SysTestRunner suites though:
just specify the options as you would to the constructor of an individual
SystemTest, and they will be passed through by the SysTestRunner.

.. seealso:: The :class:`~credo.systest.analytic.AnalyticTest`,
   :class:`~credo.systest.restart.RestartTest`,
   :class:`~credo.systest.reference.ReferenceTest`, and
   :class:`~credo.systest.analyticMultiRes.AnalyticMultiResTest` classes.

Creating a SysTestRunner to run all tests
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

To actually run tests, in the test script we need to call a SysTestRunner
to run a group of system tests, usually packaged together in one or more
Suites. The basic code to do this is::

  testRunner = SysTestRunner()
  testRunner.runSuite(testSuite)

This will actually trigger the running of the tests, and produce a
report of what happened both to the terminal, and in XML files.

.. seealso:: The  :class:`~credo.systest.systestrunner.SysTestRunner` class
   documentation.

.. _credo-examples-run-systest-direct-importingReqs:

Requirements for importing test suites: Dual-mode, and the suite() function
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You'll notice in the example test script from StgFEM, we don't simply create a
testRunner at the end and run the tests. As a reminder, the actual code is::

  def suite():
      return testSuite

  if __name__ == "__main__":
      testRunner = SysTestRunner()
      testRunner.runSuite(testSuite)

The reason for this approach is so that the test can be run directly from the
command line to run the suite and report the results, *or* imported from another
Python file when running and analysing a whole set of suites. This is known as a
"dual-mode" script in Python, and is needed so that CREDO test scripts can be run
via SCons, as discussed in :ref:`credo-examples-run-systest-scons`.

Generally those working on these scripts can just follow this pattern. 
The 2 key aspects to keep in mind are:

#. The `suite()` function must be defined if you expect to be able to use this
   script in SCons from other directories.
#. The `__name__ == "__main__"` section will only be executed if the script is
   running directly from the terminal - meaning that the testRunner will only be
   created and run in this case. This means that when importing this script,
   you can control how and when to run the suite from the importing program.

.. seealso:: http://docs.python.org/tutorial/modules.html#executing-modules-as-scripts

.. _credo-examples-run-systest-direct-modifyExistingSuite:

Example: modifying an existing test suite to test with Multigrid options
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Suppose you want to run a pre-defined test suite, but with some non-standard
options applied to test a particular feature or algorithm, such as Multigrid.

While it'd be fine to make a copy of the original script and just modify
each system test case as it's added to the suite, to reduce the amount of
typing and also possibly ease long-term challenge of keeping the scripts
in Synch, with CREDO you can modify the tests in a system test-suite 
*after* they are defined but *before* they are run.

For example, suppose for each test in a suite we want to do several things
to test how they perform with Multigrid:
 
* Add an extra XML file to be applied
* Define a PETSc solver options file
* Get the test directory record and log file to append "-mg" to the end.

This can be done by modifying the :class:`~credo.systest.api.SysTest` objects
that are kept in the :attr:`~credo.systest.api.SysTestSuite.sysTests` attribute of test suites, in a `for` loop::

  # Customise to run each test with Multigrid options
  mgSetup = "MultigridEXPERI.xml"
  mgOpts = "options-uzawa-mg.opt"
  for sysTest in testSuite.sysTests:
      sysTest.testName += "-mg"
      sysTest.outputPathBase += "-mg"
      sysTest.inputFiles.append(mgSetup)
      sysTest.solverOpts = mgOpts

Perhaps an even better way to achieve this would be to run our modified script
as a totally different file that *imports* the existing test suite.

For example if the existing test suite is in a file `testAll.py`, and we
want to run the Multigrid-extended version as `testAll-mg.py`, then the 
contents of the latter file would be:

.. include:: ./MiscScripts/testAll_mg.py
   :literal:

The only significant change in this imported script from the lines of Python
earlier is we've used the "deep copy" functionality of Python to create a
new TestSuite that is a copy of the one we imported. In this case it's not
essential as our changes here wouldn't permanently affect the old suite,
but it's good practice to get in to for this sort of activity.

Alternative: Running a single test from the command-line
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Alternatively to the above, scripts have been provided to allow you to run a
single system test from the command line prompt, just as you could with the SYS
scripts.

To do this, the relevant scripts are as follows:

================================ =========================
Script Name                      Sys Test class it invokes
================================ =========================
credo-referenceTest.py            ReferenceTest
credo-restartTest.py              RestartTest
credo-analyticTest.py             AnalyticTest
credo-analyticTestMultiResCvg.py  AnalyticMultiResTest
================================ =========================

So for example, to run a RestartTest on the Multigrid.xml model, type::

  credo-restartTest.py Multigrid.xml

... which will run the test.

.. TODO: include output examples
.. TODO: Also, need to actually document these script programs properly in the
   API reference

