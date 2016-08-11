.. _credo-examples-ppc-compare:

Running 2 suites using different numerical features and comparing results
-------------------------------------------------------------------------

This examples shows how to use CREDO to run two suites of Underworld runs, with varying numerical solver approaches, and compare the performance of the two.

Thus, it is a good baseline to use as an example for more complex performance
analysis/accuracy testing.

.. Note:: The actual script is available in the `examples` sub-directory
   of the CREDO distribution.

Setup
"""""

The script is shown below:

.. literalinclude:: ../../examples/credo_PPCCompare.py
   :linenos:

Similar to the :ref:`credo-examples-raytay-run-suite`, 
the script first sets up CREDO suites to run, runs them using a JobRunner,
then performs some analysis on the results.

Unlike the RayTaySuite example though, in this case we're not varying a
parameter across the suites, but are attaching the same model run a fixed
number of times to both suites, in order to be able to average results.

.. seealso:: Modules :mod:`credo.modelsuite`, :mod:`credo.modelrun`, and
   :mod:`credo.modelresult`.

Note that in this example, we're using the `basePath` option to one of the 
suites, because the XML Model files must be run in a sub-directory of the
current path the script is located in. This is an example of how CREDO
can work in with any arbitrary directory structure that best suits you.

Other things of note about this script as an example are:

* Use of the `os.path.join()` Python standard library function to construct 
  paths, and re-using the :attr:`credo.modelsuite.ModelSuite.outputPathBase`
  attribute to help with constructing these. This is a good practice to keep
  outputs from an analysis run all in the same directory.
* Use of the Python `csv` library to write custom results of interest to a 
  CSV file as a useful record. A good tutorial on writing CSV files is 
  available on `Steven Lott's Python pages <http://homepage.mac.com/s_lott/books/python/html/p04/p04c07_file2.html#comma-separated-values-the-csv-module>`_.

Expected Results
""""""""""""""""

Running the script should produce a report at the end similar to the following:

.. literalinclude:: ../../examples/exampleOutput/credo_PPCCompare.stdout.txt

And also save a text file and CSV file with contents such as:

.. literalinclude:: ../../examples/exampleOutput/comparePPC.txt

.. literalinclude:: ../../examples/exampleOutput/comparePPC-runs.csv
