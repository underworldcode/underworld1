.. _uw-viscosity:

Viscosity Field
===============

By default the viscosity of the fluid is not produced as output of an Underworld run. 
The field must be calculated first before outputting it.

Calculate
---------

To define the creation and calculation of the viscosity field as a type *FeVariable*,
the user must includes the *ViscosityField* template XML file.

For example:

.. code-block:: xml

 <include>Underworld/ViscosityField.xml</include>
This defines a component, called *ViscosityField* that gets (or if needed calculates) the assumed
isotropic viscosity on a particle. The value of this point-based quantity is turned into a field by a
smoothing algorithm and, as such, this field is recommended for output only and not as a parameter
to feedback into the calculations.

An important parameter of the *ViscosityField* is the swarm where the viscosity is calculated
and subsequently integrated for the smoothing algorithm. In models with PIC particles this should
be *picIntegrationPoints* swarm. This is the default setting of the *ViscosityField*. But in models
with only FEM meshes and not PIC, the swarm should be *gaussSwarm*. 

For example:

.. code-block:: xml

 <struct name="components" mergeType="merge">
    <struct name="ViscosityField" mergeType="merge">
       <param name="Swarm">gaussSwarm</param>
    </struct>
 </struct>

Output
------

As with any other field in Underworld, output can be to disk via checkpointing (see :ref:`uw-checkpoint`)
as well as visualised by gLucifer (see :ref:`uw-visualisation`). To visualise the *ViscosityField*,
a template XML is provided that setups up a subsequent viewport, called *ViscosityVP*. This template
must be included in the XML input file.

For example:

.. code-block:: xml

 <include>Underworld/Viewports/ViscosityVP.xml</include>

With this viewport defined, the *ViscosityVP* needs only to be passed to a window for rendering.

