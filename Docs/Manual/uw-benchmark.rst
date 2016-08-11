.. _uw-benchmark:

Scientific Benchmarks
=====================

This chapter describes benchmark models available in Underworld as well
as instructions on setting up and running the science benchmarks. A few examples
with their results are also included in this chapter.

Models
------

Available models for benchmarking include (but are not limited to):

 * Geothermal
 * Groundwater
 * Lithosphere Instabilities
 * Magma
 * Mantle Convection
 * Numerical Capabilities
 * Melt 
 * Subduction
 * Surface Processes

See the `Underworld Models`_ page for a list of 
of available models in Underworld.

.. _Underworld Models: http://www.underworldproject.org/models.html

Science Benchmarks with CREDO
-----------------------------

The CREDO workflow tool, mentioned in the context of scripts to run Underworld tests
(see :ref:`uw-testing`), can also be used to run and report on Underworld benchmarks.
One of the design goals of CREDO is the ability to treat benchmarks as a special type of system
test, that requires a higher level of scientific description and analysis.

See the `CREDO Configuration`_ page for more detailed information on how to write and run 
benchmarks. The benchmarks currently available are located in the *SysTest/ScienceBenchmarks*
sub-directory of each project. In Underworld’s case, this includes several *Rayleigh-Taylor* and
*Thermal Convection* benchmarks.

Benchmark Examples
------------------

Rayleigh-Taylor Benchmark
^^^^^^^^^^^^^^^^^^^^^^^^^

This benchmark models the evolution of a sinusoidal Rayleigh-Taylor instability.
It has a lower density fluid on the bottom of the box, and a higher density fluid above.

The XML file for the benchmark is *Underworld/InputFiles/RayleighTaylorBenchmark.xml*.

Detail on the benchmark can be found in [VanKeken1997]_.

 * **Components** - The free-slip-sides boundary condition *Underworld/InputFiles/RayleighTaylor.xml*,
   which applies a zero condition perpendicular to the box walls, restricting movement so that
   a material can flow along a box wall but not away from it, but also applies a zero condition
   along the top and bottom walls restricting movement along these walls in the *X* direction.
 * **General Equations** - In both analytic and numerical approaches to modelling such phenomena,
   the mathematical description has been a Rayleigh-Taylor instability - a gravitational instability
   of a layer of heavy fluid overlying a lighter one,

   .. math::
      :nowrap:

      \begin{equation}
      \mathbf{\nabla} \cdot \mathbf{\tau} - \mathbf{\nabla} p = -\mathbf{g}\rho \hat{z}
      \end{equation}
   Where :math:`\tau` is deviatoric stress, :math:`p` is pressure, :math:`\mathbf{g}` is gravity,
   :math:`\rho` is density, and :math:`\hat{z}` is the unit vector in :math:`z` direction.

   For approximately incompressible materials (Boussinesq approximation),

   .. math::
      :nowrap:

      \begin{equation}
      \mathbf{\nabla} \cdot \mathbf{u} = 0
      \end{equation}
   Where :math:`\mathbf{u}` is velocity.

   The advection equation for composition is

   .. math::
      :nowrap:

      \begin{equation}
      \frac{\partial \Gamma}{\partial t} + (\mathbf{ u }\cdot \mathbf{\nabla })\Gamma = 0
      \end{equation}
   Where :math:`\Gamma` is composition, :math:`t` is time, and :math:`\mathbf{u}` is velocity.
 * **Results** - The following material properties were used in this sample:

   * *viscosity* = 1.0 (background and light layer) (dimensionless)
   * *density* = 1.0 (background), 0.0 (light layer) (dimensionless)
   * *resolution* = 64 x 64

   See :ref:`figure-rtb-01`, :ref:`figure-rtb-02` and :ref:`figure-rtb-03` for output
   images produced using Underworld-1.0.0. 

   .. _figure-rtb-01:

   .. figure:: _static/RayleighTaylorBenchmark00001.*
      :align: center

      Rayleigh-Taylor Benchmark at 1st Timestep (2nd image)

   .. _figure-rtb-02:

   .. figure:: _static/RayleighTaylorBenchmark00638.*
      :align: center

      Rayleigh-Taylor Benchmark at 638th Timestep (2001.22 dimensionless time)

   .. _figure-rtb-03:

   .. figure:: _static/RayleighTaylorBenchmarkGraph.*
      :align: center

      Graph of Dimensionless Time vs. Velocity RMS (VRMS): of Rayleigh-Taylor Benchmark

Thermo-Chemical Benchmark
^^^^^^^^^^^^^^^^^^^^^^^^^

The D” (core-mantle boundary) of the Earth is modelled with the ThermoChemBenchmark model.
A dense layer is placed at the bottom of the box. This models the entrainment of a low lying
dense layer.

The XML file for the benchmark is *Underworld/InputFiles/ThermoChemBenchmark.xml*.

Detail on the benchmark can be found in [VanKeken1997]_.

 * **General Equations** - We solve the equations of thermal convection for a creeping fluid
   with the Bousinnesq approximation and infinite Prandtl number.

   The deviatoric stress tensor, :math:`\tau_{ij}` is given by,

   .. math::
      :nowrap:

      \begin{equation}
      \tau_{ij} = \eta \left( \frac{\partial u_i}{\partial x_j} + \frac{\partial u_j}{\partial x_i} \right)
      \end{equation}
   Where :math:`\eta` is the viscosity. This is a constitutive relationship.

   The surface temperature is :math:`T_0 = 0` and the bottom temperature is :math:`T_l =\Delta T`.
   All boundaries are free-slip.

   Choosing the following scales,

   .. math::
      :nowrap:

      \begin{eqnarray}
      x_i &\rightarrow& lx_i, \\
      T &\rightarrow& \Delta TT, \\
      u_i &\rightarrow& \frac{\kappa}{l}u_i
      \end{eqnarray}
   results in a non-dimensional form of the equations, given as,

   .. math::
      :nowrap:

      \begin{eqnarray}
      - \frac{\partial \tau_{ij}}{\partial x_j} + \frac{\partial p}{\partial x_i} &=& \rho g (1 - \alpha T ) \lambda_i, \\
      \frac{\partial u_i}{\partial x_i} &=& 0, \\
      \frac{\partial T}{\partial t } + u_i\frac{\partial T}{\partial x_i} &=& \frac{\partial^2 T}{\partial x^2_j}
      \end{eqnarray}
   Where :math:`\rho` and :math:`\alpha` are material properties.
 * **Components** - The base application is *ThermoChem.xml*, but in ThermoChemBenchmark,
   *BuoyancyFormTerm* is replaced with *BuoyancyForceTermThermoChem*.

   The temperature boundary condition applies a temperature of 0 (zero) to the top, and 1 (one) to
   the bottom of the box (dimensionless values). A dense layer is placed at the bottom of the box,
   and then entrained by thermal convection. This model solves for an energy equation, as well as the
   standard Stokes Equation.

   The following line is added to the model’s input file:

   .. code-block:: xml

    <include>Underworld/VariableConditions/temperatureBCs.xml</include>
   The free-slip boundary condition applies a zero condition perpendicular to the box wall, which
   restricts movement so that a material can flow along a box wall but not away from it. In the
   example , this condition is applied to every box wall (i.e., in the 3D case, top, bottom, left, right,
   front and back). This produces the effect of a mirror of the box outside the box wall to which this
   boundary condition is applied.

   The following line is added to the model’s input file:

   .. code-block:: xml

    <include>Underworld/VariableConditions/velocityBCs.freeslip.xml</include>
   The thermochemical convection template input files contain an analytic temperature initial condition.
 * **Results** - The following material properties were used in this sample:

   * *viscosity* = 1.0 (ambient and dense layer) (dimensionless)
   * *density* = 0.0 (ambient), 1.0 (dense layer) (dimensionless)
   * *RaT* = 3.0e5
   * *RaC* = 4.5e5
   * *alpha* = 1.0 (ambient and dense layer) (dimensionless)
   * *courantFactor* = 0.25
   * *resolution* = 64 x 32

   See :ref:`figure-tcb-01`, :ref:`figure-tcb-02` and :ref:`figure-tcb-03` for output
   images produced using Underworld-1.0.0.

   .. _figure-tcb-01:

   .. figure:: _static/ThermoChemBench00010.*
      :align: center
  
      Thermo-Chemical Benchmark at 10th Timestep (11th image)

   .. _figure-tcb-02:

   .. figure:: _static/ThermoChemBench02080.*
      :align: center

      Thermo-Chemical Benchmark at 2080th Timestep (0.0200099 dimensionless time)

   .. _figure-tcb-03:

   .. figure:: _static/ThermoChemBenchmarkGraph.*
      :align: center

      Graph of Dimensionless Time vs. Velocity RMS (VRMS) for Thermo-Chemical Benchmark

Faulting Moresi-Mühlhaus-2006 3D Extension Model
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This model simulates the yielding of the earth’s crust in three dimensions based on the
Mohr-Coulomb criterion, according to the FaultingMoresiMuhlhaus2006 model. One
(2D) or two (3D) weak zones are placed in the top of the upper mantle in order to facilitate the
localisation of the deformation near the weak zone(s).

The XML files for the benchmark are:

 * *Underworld/InputFiles/ExtensionFMM.xml*
 * *Underworld/InputFiles/ExtensionFMM3D.xml*

Although the yielding component makes use of a Mohr-Coulomb type failure criterion, it also
implies other physical assumptions.

Detail on the formulation can be found in [MoresiMuhlhaus2006]_.

 * **Components** - The base application of the model is *Underworld/InputFiles/ExtensionBaseApp.xml*.
   The extension boundary condition is free-slip, with velocity conditions applied to
   the left (-0.5) and right (0.5) walls of the box to allow for extension (dimensionless values).

   The conditions are contained in the *Underworld/VariableConditions/velocityBCs.extension.xml* XML file.
 * **Results** - The following additional rheological parameters were used in this sample:

   * *viscosity* - background and incompressible layer 0.1, crust 10.0, mantle (including weak zone)
     1.0 (dimensionless)
   * :math:`\frac{1}{lambda} = 10.0`
   * *Crust*:
 
     * softening strain = 0.1
     * initial damage fraction = 0.0
     * initial damage wavenumber = 0.5
     * initial damage factor = 0.5
     * healing rate = 0.0
     * cohesion = 10.0
     * cohesion after softening = 0.0001
     * friction coefficient = 0.0
     * friction coefficient after softening = 0.0
     * minimum yield stress = 0.00001

   See :ref:`figure-fmmb-01`, :ref:`figure-fmmb-02` for output images produced using Underworld-1.0.0.
 * **Yielding** - Basically, a Mohr-Coulomb criterion is used on each Lagrangian points (particles) to
   determine if a failure plane is (re)activated at this particular point. If yes, the preferred plane of
   sliding (assumed to be the one that is more closely aligned with the local sense of shear strain rate)
   defines a direction of anisotropy. The material properties can then be softened to model the localization
   of deformation. Once a plane of failure is activated, it is assumed to be a preferred orientation for
   subsequent sliding, but in case it is no longer active, new orientations are tested. As one can see,
   this formulation does not simply model a Mohr-Coulomb failure criterion in the strict sense. It
   is actually a way of incorporating brittle (or semi-brittle) material behavior in a viscous code. If
   anyone wants to use it, it is important to bear in mind the underlying assumptions of this model.

   In a material which has the lots of randomly directed faults, and therefore the potential to fail in
   any direction, the faults on which slip occur for the minimum stress orientated at an angle ±θ to
   the most compressive principle stress direction. 

   This angle is found using the equation:

   .. math::
      :nowrap:
   
      \begin{equation}
      \tan ( 2 \theta ) = \frac{1}{\mu}
      \end{equation}
   Where :math:`\mu` is the frictional coefficient. 

   The principle stress directions are found using the functions:

   .. code-block:: c
 
    void UnderworldEigenvalues( double* vector, double* eigenvalues );
    void UnderworldEigenvectors( double* vector,
                                 double* eigenvalue,
                                 double** eigenvector );
   Where the most compressive eigenvector is given by the greatest eigenvector, which is given as the first
   vector in the array.

   Both directions :math:`\pm \theta` to this most compressive principle stress direction are equivalent in
   terms of weakness, but generally, only one of these directions is stable. One direction is aligned
   parallel to the sense of shear, this direction unstable because it will rotate in a hardening direction.
   The other orientation is stable because the director will no longer be rotating if it is perpendicular
   to the shear. This then becomes part of the failure criteria, that the material may only fail in the
   softening direction.

   .. _figure-fmmb-01:

   .. figure:: _static/FMMExtensionMC3D00001.*
      :align: center

      Faulting Moresi-Mühlhaus 2006 Benchmark at 1st Timestep (2nd image)

   .. _figure-fmmb-02:

   .. figure:: _static/FMMExtensionMC3D00013.*
      :align: center

      Faulting Moresi-Mühlhaus 2006 Benchmark at 13th Timestep

.. [VanKeken1997] P.E. van Keken et al. A comparison of methods for the modelling of thermochemical convection.
 Journal of Geophysical Research 102:B10, 22477-22495, 1997

.. [MoresiMuhlhaus2006] Moresi, L. and Mühlhaus, H.-B. Anisotropic viscous models of large-deformation
 Mohr-Coulomb failure. Philosophical Magazine, Volume 86, Numbers 21-22, -22/21 July 1 August 2006,
 pp. 3287-3305

.. _CREDO Configuration: https://www.underworldproject.org/credo-doc/examples/configure-scibenchmark.html
