.. _uw-multigrid:

Multigrid
=========

Mutigrid can be used in the following scenarios:

 * 3D or large 2D problem that can't be solved by a direct solver on a single node.
 * There are over 1,000,000 unknowns.
 * If there are large variations in viscosities.
See :ref:`mg-example-fig-01` for an example performance comparison when using multigrid.

.. _mg-example-fig-01:

.. figure:: _static/multigridtimingexample.*

   Timing example using different solvers for an extension model with an imposed weak
   zone (no yielding).

Amount of Levels
----------------

The amount of levels of multigrid can be adjusted using the *mgLevels* parameter.
This parameter is dependent on the configuration and assumptions in the model.
Geometric multigrid is based on the mesh used to solve a problem. It needs to coarsen the mesh
evenly in each dimension a number of times equal to the number of levels requested minus 1.

For example, if solving on a 64 x 64 mesh and the level is set to 3, the sequence of grids will
be 64 x 64, 32 x 32, 16 x 16.

The finest level is considered a part of the three requested levels. So, if for the same example
the level is set to 8, the sequence of grids will be 64 x 64, 32 x 32, 16 x 16, 8 x 8, 4 x 4,
2 x 2, 1 x 1, *--error*.

The mesh can't suppor the number of levels. The number of levels must be selectes in accordance
with the size of the mesh. This can be expressed by

.. math::

   \frac{M}{2^{N-1}} = k 
Where :math:`M` is the mesh size, :math:`N` is the number of multigrid levels, and :math:`k \in \mathbb{N}`.

