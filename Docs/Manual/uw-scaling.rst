.. _uw-scaling:

Units Scaling
=============

As of version 1.5, Underworld comes with the capability to scale input and output parameters in
*SI* units automatically. This allows users to input parameters using a variety of *SI* values, as well
as output FEM field values in desired values.

Enabling Scaling
----------------

To activate this functionality, the setup of a *Scaling* component is required, then all relevant
input and output units can be defined.

For example:

.. code-block:: xml

 <struct name="default_scaling">
    <param name="Type">Scaling</param>
    <param name="spaceCoefficient_meters">1.0e6</param>
    <param name="timeCoefficient_seconds">3.30e-5</param>
    <param name="massCoefficient_kilograms">3.30e21</param>
    <param name="temperatureCoefficient_kelvin">2044</param>
 </struct>
Here the fundamental scaling coefficients are defined (electric current is left out of this example).
These are used to convert numbers between dimensional or scaled values based on a user specified SI units string.

If the following is defined in the XML file:

.. code-block:: xml

 <param name="referenceTemperature" units="K">1022</param>
Where *K* means kelvin scaling, Underworld will store the referenceTemperature as :math:`1022 / 2044 = 0.5`

And,

.. code-block:: xml

 <param name="myViscosity" units="Pa*s">1e20</param>
Where *Pa*s* means pascal second scaling, Underworld will store the myViscosity as
:math:`1*10^{20} / ( 3.30*10^{21} / (3.30*10^{-5} * 1.0*10^{6} ) ) = 1`.

Scaling gLucifer Output
-----------------------

A FeVariable component can be given an *outputUnits* string parameter to automatically scale
any gLucifer output from the field.

For example:

.. code-block:: xml

 <struct name="VelocityField" mergeType="merge">
    <param name="outputUnits">cm/yr</param>
 </struct>
All velocity field output will be given a cm per year scaling.

Examples
--------

Example of an XML file using scaling is found in:

``$PATHTOUNDERWORLD/Underworld/InputFiles/Woidt78_model_a.xml``.

Important Notes
---------------

Input scaling is only applied to parameters with explicit *units* attributes. Default values
which the user may be using are not appropriately scaled and will most likely cause incorrect
behaviour due to improper scaling. If the scaling mechanism is used, one must avoid using default
values like *defaultDiffusivity*.

The *units* string doesn’t perform any error checking yet on the type of number it is defining.

For example:

.. code-block:: xml

 <param name="referenceTermperature" units="kg">1022</param>
is legal input, but the scaling will be incorrect and the model will misbehave. The *units* string
needs to be checked with the number it is defining.

Only gLucifer output can be dimensionalised with *outputUnits*. All other data, such as
*FrequentOutput.dat* and checkpoint data, will remain scaled.






