.. _credo-developer-notes:

Notes for CREDO developers / maintainers
========================================

Coding standards / style guides
-------------------------------

On recommendation of the majority of Python tutorials out there, we are
following the standard Python style guide, at
http://www.python.org/dev/peps/pep-0008/.

Some of the notable things here are:

* Use *4 spaces* for indentation, instead of tabs (obviously crucial to be
  consistent about this in Python, since indenting whitespace is used to
  control blocks.
* Spaces around assignment symbols, =, but not between brackets and arguments.

Where we 'break' any of these rules in the package, we'll document this here.

Testing framework of CREDO itself
---------------------------------

Yes, a system testing and benchmarking framework needs itself to be tested to
minimise the chance of bugs occuring when CREDO is put to use!

Currently, we use the Python
`Unittest <http://docs.python.org/library/unittest.html>`_ framework.
Tests are mainly organised per file (module), rather than per class.

It's important to write unit tests for all new classes and to maintain and
improve the suite for existing ones. Unittest is pretty easy to use and is well
documented.

The current test structure is:

* Each CREDO package has a `tests` subdirectory (e.g. credo/systests has a 
  subdirectory credo/systests/tests) where test suites are stored;

  * In this subdirectory each CREDO module and/or Class from the above package
    should have a test suite associated with it, named `xxsuite.py`, where
    `xx` is the name of the class/module being tested
    (e.g. imageCompTestsuite.py). The consistent naming pattern aids
    auto-discovery of tests.
  
    .. note:: we leave it up to developer judgement whether to write tests
       focused on modules or classes within them. It varies depending on the
       nature of the package.

  * The test suites themselves are structured as normal for unittest suites,
    including a dual-mode __main__ section at the end that runs the current
    suite if the test is run directly from the command line.

  * Each testing package also should have a "testAll.py", that automatically
    runs all of the suites in that directory. These can be pretty standard 
    code to auto-discover the other suites - look at any of the examples of
    these that already exist.

.. note:: Currently, there is no automated way to test the entire suite of
   CREDO Tests in one batch. This will be coming soon in the next release.
