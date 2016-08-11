.. _uw-convergence:

Non-Linear Convergence Plot
===========================

The *Convergence tool* allows the user to see if their code is converging for a certain tolerance
on the Uzawa solver.  The default tolerance is non-linear.  This works by the residual for each
non-linear iteration being recorded in the file *convergence.dat*. The tool can be enabled in the
XML input file.

For example:

.. code-block:: xml

 <struct name="stokesEqn" mergeType="merge">
    <param name="makeConvergenceFile" mergeType="replace">true</param>
 </struct>
This creates the *Convergence.dat* file when the model is ran. The file will have the following
format::
 
 1 2 0.002 0.001
 1 3 0.0015 0.001
Where the first column represents the *timestep*, the second column the *number of non-linear
iterations at that time step*, the third column the *residual of non-linear iterations* and the 
fourth column the *tolerance condition for non-linear solver*.

The maximum amount of non-linear iterations can also be set in the XML input file as a parameter.

For example:

.. code-block:: xml

 <param name="nonLinearMaxIterations">5</param>
The default value is quite low, and could be too low to get a convergence. The output file,
*Convergence.dat* helps the user decide if this number needs to be higher to get a convergence.

