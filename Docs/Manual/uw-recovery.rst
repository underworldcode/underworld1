.. _uw-recovery:

Recovery Methods
================

Underworld currently supports two recovery methods, *REP* and *SPR*. These methods recover
stress and/or strain rate fields so they become continuous across the computational domain.
Generally with the default element types in Underworld - linear velocity and constant pressure
elements - the numeric fields for the *raw* stress and strain rate are discontinuous across element
boundaries.

Recovery methods smooth these *raw* fields to create continuous *recovered* fields
that exhibit better convergence behaviour than the *raw* fields, especially if the model flow is
continuous. Models with thin, discontinuous flow can suffer from the effective smoothing of recovery
methods.

All recovery methods in Underworld produce FeVariable fields as output that store the recovered values.
These fields can be used in any other functionality that takes an *FeVariable* as input like *checkpointing*
and *visualisation*.

Recovery by Equilibrium in Patches (REP)
----------------------------------------

Used for recovering total strain rate, total and deviatoric stress including pressure.
This algorithm is enabled by including the *REP_Setup_Compressible.xml* in the XML input file.

For example:

.. code-block:: xml

 <include>Underworld/REP_Setup_Compressible.xml</include>
This will create the following recovered fields:
 
 * *recoveredStrainRateField*
 * *recoveredStressField*
 * *recoveredTauField* - recovered deviatoric stress field.
 * *recoveredPressureField*
 * *recoveredStrainRateInvariantField*
 * *recoveredStressInvariantField*
 * *recoveredTauInvariantField* - invariant of recovered deviatoric stress field.

Superconvergent Patch Recovery (SPR)
------------------------------------

Used for recovering total strain rate. This algorithm is enabled by including the *SPR_Setup.xml*
file in the XML input file.

For example:

.. code-block:: xml

 <include>Underworld/SPR_Setup.xml</include>
This will create a recovered strain rate field, called *recoveredStrainRateField*.
