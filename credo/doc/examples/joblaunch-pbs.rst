.. _credo-examples-joblaunch-pbs:

Running a CREDO script (either System testing or analysis) using PBS
--------------------------------------------------------------------

As well as launching CREDO scripts directly, you can also use CREDO to
run suites of test model runs or analysis model runs on systems that
use the PBS scheduler to manage resources and access.

You will need basic famiarity with how PBS works, e.g. see
`the VPAC page on these <http://www.vpac.org/node/107>`_. 

Essentially, to run a CREDO script using PBS you need to use a PBS script that:

* Loads appropriate modules to run CREDO (Python) and the underlying code
  (eg PETSc, MPI in the case of Underworld)
* Sets the MPI command for CREDO to use correctly (generally `mpiexec` on HPC
  systems).
* Launches the CREDO script command as normal.  

Here is an example script that launches a CREDO test script:

.. literalinclude:: PBSScripts/UW-lowres.pbs

