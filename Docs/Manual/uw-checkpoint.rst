.. _uw-checkpoint:

Checkpointing
=============

Underworld provides the facility to *check point* simulation runs periodically,
which saves key information about the application to disk. This is useful for analysing
results in detail after a simulation has completed, or restarting the simulation. It
allows key parameters to be changed when simulations are restarted.

Scope
-----

When checkpointing is enabled for StgFEM, it will dump the values of certain 
*FeVariables* in the current simulation into separate files in the output directory.
The output directory can be specified by setting the *outputPath* in the XML file
(see :ref:`create-own-xml`).  By default, the system will save only the *FeVariables*
required for a restart of the model. Additional fields can be included for checkpointing by
declaring them in the *FieldVariablesToCheckpoint* list of the XML input file. 

For example:

.. code-block:: xml

 <list name="FieldVariablesToCheckpoint">
    <param>VelocityField</param>
    <param>VelocityGradientsField</param>
 </list>
.. note:: Specifying an own *FieldVariablesToCheckpoint* list may result in insufficient data
 being stored for checkpoint restarts. If storing auxiliary fields for general analysis is required,
 it is best to use the *FieldVariablesToSave* list.
At times it is also desirable to specify the data to store to file. This is distinct from
checkpointed data in that it will not be reloaded when a job is resumed. Currently only field data
and not swarm data may be stored. The fields which the user wishes to store are defined using the
*FieldVariablesToSave* list.

For example:

.. code-block:: xml

 <list name="FieldVariablesToSave">
    <param>VelocityGradientsField</param>
    <param>StrainRateField</param>
 </list>
A list of available fields to save may be generated using the *StgFEM_FeVariableList* plugin.

Data Options
------------

There are two modes for checkpointing in Underworld:

 * **HDF5** - This mode relies upon the HDF5 libraries for data storage. If during the compile
   configuration stage the HDF5 libraries are found, this is used as the default mode.

   The HDF5 mode is the preferred mode for checkpointing. Files stored using the HDF5 mode
   receive the *.h5* extension. In this mode, each field is stored in its own file, and only one file
   is create irrespective of how many processors the jobs utilises. Swarm data is treated differently
   however, with each processor dumping a file for each swarm in the simulation. An eight processor
   simulation would for example result in eight output files for each swarm.
  
   Restarting using arbitrary number of processors is still allowed and will occur automatically.
   Users don't need to specify how many files were used for the original checkpoint. One key benefit of
   HDF5 mode is that checkpoints are portable, and may be restarted on any machine irrespective of
   the architecture.  File sizes are also smaller for swarm data, as only the required data is stored.
   The *H5utils* package may prove useful to the user wishing to query and manipulate HDF5 files.
 * **ASCII** - This mode stores all field data in ASCII format, while
   swarm data is stored in the native binary format of the machine. ASCII mode does not
   require any external package.

   The ASCII checkpoint mode stores each checkpointed field/swarm in its own file with the *.dat*
   extension. For parallel simulations this is still the case, with only one file per field/swarm created
   regardless of how many processors are used. Restarts are possible using any number of processors,
   irrespective of how many processors the simulation was originally run across.

   Generally, checkpoints created using ASCII mode are not portable due to the binary mode in which
   swarms are stored. The exceptions to this are where no swarm checkpointing is required, in which case
   data is only stored in ASCII format which may be re-read on other machines. The other exception is where
   machines have identical architectures in which case the binary files should be re-read without
   issue. Note also that some cluster machines do not have homogeneous architectures, and so binary
   checkpoint files may fail to be read correctly. Generally this will result in terminal failure of the
   simulation.

If required, HDF5 can be disabled when configuring and compiling Underworld (see :ref:`installing-underworld`). 

Mesh and Node Locations
-----------------------

Locations of the nodes are stored in a separate mesh checkpoint file. For HDF5 checkpoints, the
mesh connectivity is also contained within this file. The nodes' locations can be stored
within the field checkpoint files by setting the *saveCoordsWithFields* flag in the XML file.

For example:

.. code-block:: xml

  <param name="saveCoordsWithFields">True</param>

Format
------

The format of the nodal field dump files is::
 
 value1 [value2] [value3]
If *saveCoordsWithFields* is set to *True*, the format is::
 
 xCoord yCoord zCoord value1 [value2] [value3]

For example::
 
 0.5 0.5 0.6 32.0 12.1 0.788
This is a 3D velocity dump with *saveCoordsWithFields* set to *True* where the data is at
coord (0.5, 0.5, 0.6) and the value is (32, 12.1, 0.788). The line number within the
dataset of the checkpoint file will correspond to the global node index.

Enabling Checkpointing
----------------------

Checkpointing is disabled by default. To activate it, the parameter *checkpointEvery* in the
XML input file or command line must be set.

For example:

.. code-block:: xml

 <param name="checkpointEvery">5</param>
This will checkpoint every 5 timesteps. Checkpointing can be disabled by setting this parameter
to ``0`` (zero).

Restarting a Simulation
-----------------------

To restart a simulation from a saved set of checkpoint information, the simulation must be 
re-launched with the compatible input files with an extra input parameter *restartTimestep*
which must be specified to refer to the timestem to load. The simulation will start running from
the next timestep.

For example::
 
 ./Underworld myJob.xml --restartTimestep=8
Model *myJob.xml* will be restarted from timestep 8. The *restartTimestep* can also be set
in the XML input file.

For example:

.. code-block:: xml

 <param name="restartTimestep">8</param>
The model can then be ran without the *restartTimestep* command line argument.

For example::

 ./Underworld myJob.xml

Restarting at a Different Resolution
------------------------------------

Often it is useful to restart jobs at a resolution different to that originally used.
This is possible by using the *interpolateRestart flag*. An example run of a standard
Rayleigh-Taylor simulation at resolution of 64 elements in each direction and checkpointed 
at timestep 10 can be restarted to a higher resolution of 128 elements in each direction.

For example::
 
 Underworld RayleighTaylorBenchmark.xml --restartTimestep=10 --interpolateRestart=1
Field data is interpolated to the new resolution using the element interpolation functions, and
so interpolation order will correspond to the element order of the field being interpolated (a field
constructed of linear basis functions will for example result in linear interpolation). The original
particle data is used, with new particles being copied from those existing where required. For best
results, it is recommended that steps up or down in resolution are kept modest. Only regular meshes
are currently supported.

Changeable Attributes in Restart
--------------------------------

The following attributes can be changed when restarting from checkpoint files:
 
 * Visualisation paramters used in gLucifer visualisation can be completely changed since these
   are all generated on the fly and don't affect the core components of the model. This could be 
   potentially used to change the camera angle to highlight different features of the model as they evolve.
 * Global parameters such as diffusivity, solver tolerances, etc.
 * Values applied at boundary conditions.
 * Properties of the previously defined materials like the density of a material or rheologies
   applied to it. Material particles assignment can't be changed after the restart.
 * Model resolution can be interpolated to a new resolution on restart.

Commonly Checkpointed Fields
----------------------------

The following list outlines the commonly checkpointed fields:
 
 * *PressureField*
 * *StrainRateField*
 * *StrainRateInvariantField*
 * *TemperatureField*
 * *VelocityField*
 * *VelocityGradientsField*
 * *VelocityMagnitudeField*
 * *VorticityField*

Checkpointing Flags
-------------------

The following list outlines the available checkpointing flags

 * **checkpointAppendStep** - *Boolean*. Set to *True* to store all checkpoint files in a per timestep directory.
 * **checkpointAtTimeInc** - *Float*. Sets a time increment at which checkpointing should occur.
 * **checkpointEvery** - *Unsigned*. Timestep interval at which checkpointing should occur.
 * **checkPointPrefixString** - *String*. A string prefix which is appended to checkpoint files.
 * **checkpointReadPath** - *String*. Path where checkpointing files may be found.
 * **checkpointWritePath** - *String*. Path where checkpointing files may be stored.
 * **interpolateRestart** - *Boolean*. Set to *True* to interpolate checkpointed data to the new resolution.
 * **restartTimestep** - *Unsigned*. Timestep to resume from checkpointed data from.
 * **saveCoordsWithFields** - *Boolean*. Set to *True* to include node locations with field checkpoint files.
 * **saveDataEvery** - *Unsigned*. Timestep interval at which data saving should occur (not necessarily for checkpointing). 

