.. _credo-examples-register-systests-scons:

Registering CREDO Test suites with SCons
----------------------------------------

For CREDO tests to be able to be registered with and work with SCons, it
currently requires several things to be done.

To set up your initial links between the SCons configuration for your
project and CREDO:

* Use of the CREDO SCons-related functions distributed with CREDO in the
  `scons` folder;
* Addition of several targets to your project's main SCons config file,
  e.g. `SConstruct`.

Then to register particular CREDO test suites with SCons just requires:

* Calling the CREDO functions on your environment in your project
  configuration files, e.g. `Underworld/SConscript`.

These sections will be explained in turn - if you are working on an existing
project that already has CREDO system testing integrated into the project's
SCons build system, you can safely jump ahead to 
:ref:`credo-examples-register-systests-scons-register`.

.. note:: for the instructions in this section to work, it requires the
   CREDO system test suite files to follow the conventions in 
   :ref:`credo-examples-run-systest-direct-importingReqs`, as this
   convention allows suites to be easily imported from other files.

.. _credo-examples-register-systests-scons-setup:

Setting up the links between SCons and CREDO for a project
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This setup just needs to be done once for a project, but requires several steps.

If you wish to understand the rationale in this section, a quick read through
of SCons primer material may be helpful. Though written in Python, SCons
extends the language with several "magic" features like variable lists that
can be imported and exported between scripts, and a few of these are used
in integrating CREDO with SCons. Some useful links for a primer may be:

* `An intro slideset to SCons
  <http://www.mrao.cam.ac.uk/~bn204/alma/sweng/sconsintro.html>`_ ;
* The `SCons user guide <http://www.scons.org/doc/HTML/scons-user/>`_,
  especially the sections on:

  * 'Builder' objects, that `use Python functions 
    <http://www.scons.org/doc/HTML/scons-user/x3594.html>`_;
  * `Where to put custom builders and tools 
    <http://www.scons.org/doc/HTML/scons-user/x3697.html>`_.
* The online `SCons manual <http://www.scons.org/doc/HTML/scons-man.html#lbAQ>`_
  - especially the Action objects section.
* The SCons wiki page http://www.scons.org/wiki/UnitTests, especially 
  the section "unit tests integration with an SCons tool".

CREDO integration into SCons is based on the above strategies - essentially
we provide a CREDO SCons "toolkit" that defines several 'Builders' and
'Actions' relevant to running CREDO tests, based on test lists.

The process of using this in a project is as follows:

#. Add the CREDO toolkit in your project's Environment in your SConstruct.

   This would involve a line such as the following as soon as you create your
   main SCons environment object::

     # Load CREDO, the system testing tool
     env.Tool('credosystest', toolpath=['credo/scons'])

#. Add SCons targets to run system tests linked to the relevant CREDO SCons
   builders and variables.

   This part needs to occur at the end of your main SConstruct file, after all
   project-related configuration has been read and processed. The first 
   part is to 'magic import' the special SCons variables CREDO uses to keep
   track of test lists, e.g.::

     Import('LOWRES_SUITES')
     Import('INTEGRATION_SUITES')
     Import('CONVERGENCE_SUITES')
     Import('SCIBENCH_SUITES')
  
   This is immediately followed by the targets needed to execute the suites,
   so that for example running `scons check-lowres` will actually run all the
   low resolution tests you've registered. Each of these targets is set in
   "always run" mode, so SCons knows to re-run the tests whenever you invoke
   the commands.

   This requires SCons code such as the following: repeated for all the test
   suite variable lists defined above::

     lowresSuiteRun = env.RunSuites( 
       Dir(os.path.join(env['TEST_OUTPUT_PATH'], env["CHECK_LOWRES_TARGET"])),
       LOWRES_SUITES)
     env.AlwaysBuild(lowresSuiteRun)
     env.Alias(env["CHECK_LOWRES_TARGET"], lowresSuiteRun)

   ... finally you need to set up any SCons aliases so that one test
   command can run multiple other targets, such as the following::

     # Run the lowres checks as part of default and complete
     env.Alias(env['CHECK_DEFAULT_TARGET'], env['CHECK_LOWRES_TARGET'])
     env.Alias(env['CHECK_COMPLETE_TARGET'], env['CHECK_LOWRES_TARGET'])
     # For the others, just add to the complete target
     env.Alias(env['CHECK_COMPLETE_TARGET'], env['CHECK_INTEGRATION_TARGET'])
     env.Alias(env['CHECK_COMPLETE_TARGET'], env['CHECK_CONVERGENCE_TARGET'])
     env.Alias(env['CHECK_COMPLETE_TARGET'], env['CHECK_SCIBENCH_TARGET'])`

... and that's it (phew)! You then need to actually define which CREDO
test suite files are registered with each target on a per-project basis,
explained in :ref:`credo-examples-register-systests-scons-register`.

.. _credo-examples-register-systests-scons-register:

Registering CREDO system tests suites for your project with SCons
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Registering test suites to belong to SCons-visible testing targets is part
of functionality provided by the CREDO SCons toolkit. So provided you
(or whoever has setup your project) has followed the instructions in
:ref:`credo-examples-register-systests-scons-setup`, you just need to:

#. Right after you set up the cloned environment for a sub-project, define
   a CURR_PROJECT env variable recording the name of the project.
   
   CREDO's SCons toolkit can then use this to record and report on
   which project a test suite is registered to.

   For example, for the Underworld the following lines are 
   near the top of the project's SConscript file::

     Import('env')

     env = env.Clone()
     env['CURR_PROJECT'] = "Underworld"

#. use functions such as the following to add a test suite to an
   SCons target:

   * env.AddLowResTestSuite
   * env.AddIntegrationTestSuite
   * env.AddConvergenceTestSuite
   * env.AddSciBenchTestSuite

   ... where the only input to each function is the relative path to the CREDO
   Suite to register. For example, in the Underworld project this section
   looks like the following towards the bottom of the project's SConscript
   file::

     env.AddLowResTestSuite('SysTest/RegressionTests/testAll_lowres.py')
     env.AddIntegrationTestSuite('SysTest/RegressionTests/testAll.py')
     env.AddConvergenceTestSuite('SysTest/PerformanceTests/testAll.py')
     env.AddSciBenchTestSuite('SysTest/ScienceBenchmarks/testAll.py')

That's it! This will then allow you to run SCons command-line testing targets
and get reporting on a per-project basis as shown in
:ref:`credo-examples-run-systest-scons`.
