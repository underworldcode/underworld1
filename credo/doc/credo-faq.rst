.. _credo-faq:

*********
CREDO FAQ
*********

CREDO's capabilities
====================

Can I get CREDO to submit PBS jobs, or run it via PBS?
------------------------------------------------------

The answer to the first of these is: yes, but this is an experimental feature
still in beta. Check out :mod:`credo.jobrunner.pbsjobrunner` if you are
interested in this, and if you have an Underworld checkout, the script
in `Underworld/InputFiles/credo_rayTaySuitePBS.py`.

You can also submit a Python CREDO script in parallel on a HPC system
running PBS by writing the appropriate PBS script yourself, and embedding
the CREDO call within it - see :ref:`credo-examples-joblaunch` for an example.

Problems running tests
======================

When I try to run a CREDO system test script get an "ImportError" message
-------------------------------------------------------------------------

This problem is usually because you haven't add the directory containing
the CREDO Python source to your PYTHONPATH. See :ref:`environment_setup`
for the various ways to do this.

Errors trying to run one of the Underworld Science benchmarks from command line
-------------------------------------------------------------------------------

Currently (11/4/2011), these tests have been written so that by default when
run from the command line, they expect to post-process the benchmark tests
and reporting from an existing set of results.

If you want to modify this behaviour so that you *do* first run and generate
the models required by the benchmark, then set the `postProcessFromExisting`
flag to `False` in the __main__ section at the bottom of a benchmark, e.g. ::

    if __name__ == "__main__":
        postProcFromExisting = False
        jobRunner = credo.jobrunner.defaultRunner()
        testResult, mResults = sciBTest.runTest(jobRunner, postProcFromExisting,
            createReports=True)

Problem with parallel system tests, a CVGReadError occurs trying to read CVGs
-----------------------------------------------------------------------------

Q: I have a problem running parallel CREDO system tests, along the lines
of being unable to parse Field convergence results, where it brings up an
exception message along the lines of:
"credo.io.stgcvg.CVGReadError: Error, couldn't read expected error" ...

A: This problem is often caused by using an "mpirun" or "mpiexec"
not corresponding to the MPI library you compiled the code with. Not doing
this (eg running the code with OpenMPI that you compiled with MPICH2)
can result in annoying parallel bugs  e.g. corruptions when writing
output files, which among other things throw off the CREDO system
testing now included with the code.

What are the ways of dealing with this?

1. On clusters using the "module" system, just make sure you load the
   same modules when running the code (eg in PBS scripts) as when you
   compiled it.
2. Make sure your PATH system environment variable is set up
   correctly so that "mpirun" will launch the correct version you
   compiled with. (You can test this by running 'which mpirun'). It's a
   good idea to set this up in your login scripts, e.g. .bashrc.
3. For CREDO specifically, you can set the environment variable
   MPI_RUN_COMMAND to tell it to use a custom mpirun/mpiexec rather than
   the default one in your PATH. (E.g. setting
   MPI_RUN_COMMAND="/usr/local/packages/mpich2-1.2.1p1-debug/bin/".) 
   
