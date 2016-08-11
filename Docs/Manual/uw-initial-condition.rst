.. _uw-initial-condition:

HDF5 Initial Condition
======================

It is possible to define the initial condition for an FeVariable using checkpointed data.
Only HDF5 checkpoint data is supported. This functionality is enabled by using the 
*HDF5ConditionFunction* plugin.

For example:

.. code-block:: xml

 <list name="plugins" mergeType="merge">
    <struct>
       <param name="Type">Underworld_HDF5ConditionFunction</param>
       <param name="FeVariableHDF5Filename">VelocityField.input.h5</param>
       <param name="MeshHDF5Filename">Mesh.linearMesh.input.h5</param>
       <param name="TargetFeVariable">VelocityField</param>
    </struct>
 </list>
 <struct name="velocityICs">
    <param name="type">CompositeVC</param>
    <list name="vcList">
       <struct>
          <param name="type"> AllNodesVC </param>
          <list name="variables">
             <struct>
                <param name="name">velocity</param>
                <param name="type">func</param>
                <param name="value">HDF5ConditionFunction</param>
             </struct>
          </list>
       </struct>
    </list>
 </struct>
This first section creates an instance of the *HDF5ConditionFunction* plugin. The parameters
*FeVariableHDF5Filename* and *MeshHDF5Filename* respectively define the
filename (and path) for the checkpointed FeVariable and FeMesh, both of which are required.
It is not necessary for the checkpointed resolution to be identical to the current simulation
resolution. The file paths are specified with respect to the simulation execution directory. It
will be generally safest to specify absolute paths to the data files.

The final parameter *TargetFeVariable* is required to determine auxiliary information for
the checkpoint data reload process. It tells the simulation which *FeVariable* should have it’s
initial condition set, as this is handled in (for example) the *velocityICs* XML struct.

The final section configures the initial condition for the *FeVariable*. It sets the
standard velocity *FeVariable* initial condition. For Stokes flow, this will have no
effect as the solution is entirely determined by the boundary conditions and body forces.
This definition follows the standard procedure for variable conditions.

*AllNodesVC* is used to ensure all nodes have their value set. The struct within
the *variables* list instructs that the initial condition well be calculated using the
*HDF5ConditionFunction* plugin. The *variables * struct contains the following parameters:

 * *name* - Determines which variable to set.
 * *type* - Configures the variable to be determined via *func*.
 * *value* - Sets the function to *HDF5ConditionFunction*.

A complete example can be found in *Underworld/plugins/VariableConditions/HDF5ConditionFunction*
inside the *exampleHDF5Condition.xml* file.


