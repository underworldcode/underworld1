.. _uw-timestep:

Timestepping
============

Depending on the equations used in a simulation, Underworld uses different algorithms to calculate
the timestep, :math:`\Delta T`, required to maintain numerical stability.  When modelling time
independent Stokes Flow with particle advection, the stability criterion is,

.. math::
   :nowrap:

   \begin{eqnarray}
   \Delta T < 0.5 \cdot \frac{\Delta x}{\left|\mathbf{u}\right|}
   \end{eqnarray}
Where :math:`\Delta x` is the minimum width of an element and :math:`\left|\mathbf{u}\right|` is the
maximum velocity magnitude measured in the entire domain. This method ensures a stable timestep is
always selected.
 
For models with both Stokes Flow and Energy (advection diffusion) equations an added diffusion
stability criterion is calculated,

.. _stability-critertion:

.. math::
   :nowrap:

   \begin{eqnarray} 
   \Delta T < C \cdot \frac{(\Delta x)^2}{K} \label{eqn:stability_diffusion}
   \end{eqnarray}
Where :math:`C` is the courant factor and :math:`K` is the maximum value of thermal diffusivity.
The minimum :math:`\Delta T` from the above criteria is used as the timestep to update Underworld.
To see the timestep values chosen by Underworld look at the *FrequentOutput.dat* file in the output directory.

Controls
--------

The following list outlines available timestepping controls:

 * **courantFactor** - *Default is 0.5*. The courant factor, :math:`C`.
 * **timestepFactor** - *Default is 1.0*. Scales the minimum timestep returned by the stability criteria.

   For example, *–timestepFactor=0.1* would use a timestep one tenth the original timestep calculated
   from the stability criteria.
 * **limitTimeStepIncreaseRate** - *Default is False*. Determines whether the increase in :math:`\Delta T`
   should be limited.
 * **maxTimeStepIncreasePercentage** - *Default is 10.0*. Sets a limit on the percentage increase in :math:`\Delta T`
   per timestep. This option requires *limitTimeStepIncreaseRate* to be *True*.
 * **DtGranularity** - *Default is 0.0*. Sets the timestep to the nearest granularity of the problem,
   so long as *DtGranularity* is less than the calculated timestep from the criteria.
 * **maxTimeStepSize** - *Default is 0.0*. Sets limit on the maximum time step possible. Timesteps greater than the
   limit are clipped to the limit.
All the options mentioned above can be set from the XML file or passed as command line arguments.

For example:

.. code-block:: xml

 <param name="courantFactor">0.5</param>
See :ref:`timestep-figure-01` for the timestepping and checkpointing states that is configured in Underworld.

.. _timestep-figure-01:

.. figure:: _static/timestepsequence.*
   :align: center

   *AbstractContext* Time Sequence.

