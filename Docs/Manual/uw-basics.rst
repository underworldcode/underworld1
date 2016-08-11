.. _uw-basics:

******
Basics
******

Introduction
============

 * Underworld runs on Unix-style systems, e.g. Linux, Mac OS X, etc., (see :ref:`system-requirements`).
 * There are a number of options available to install Underworld, and a number of package dependencies, 
   (see :ref:`installing-underworld` and :ref:`system-requirements`).
 * It uses XML code contained in input files to define models.
 * It is extensible, and is capable of dynamically loading objects declared in the XML file.
 * It is generally ran from a terminal::

    ./Underworld ~/UnderworldFiles/MyInputFiles/Sample.xml
   Although other options exist for running jobs including using Python scripts with the CREDO
   system, submitting jobs over the Grid using the Grisu tool, and a work-in-progress web portal.
 * It outputs results in three ways:
  
   * Outputs to screen
   * Outputs to user-defined files
   * Outputs images via gLucifer (visualisation package included with Underworld)
 * It can run a large number of geophysical problems.
 * It is written in a hierarchical style with higher-level programs built on the structure of
   lower-level ones. Underworld is the top-level program.

Scope
=====

Underworld solves the Stokes flow problem and the energy (advection/diffusion) equation.
Underworld allows for a modeller to customise these equations by modifying the assembly methods for
these general equation i.e, add force terms, add constitutive behaviours, thus modelling complex
geophysical behaviours.

General Equations
-----------------

Incompressible Stokes Flow:

.. math::
   :nowrap:

   \begin{eqnarray}
   \frac{\partial\tau_{ij}}{\partial x_j} - \frac{\partial p}{\partial x_i} &=& -\alpha\rho gT\lambda_i \\
   \frac{\partial u_i}{\partial x_i} &=& 0
   \end{eqnarray}

Advection Diffusion:

.. math::
   :nowrap:

   \begin{equation}
   \frac{\partial T}{\partial t} + u_i \frac{\partial T}{\partial x_i} = \kappa \frac{\partial^2 T}{\partial x^2_j}
   \end{equation}

Where :math:`x_i` are the spatial coordinates, :math:`u_i` is the velocity, :math:`T` is the temperature,
:math:`\alpha` is the thermal expansivity, :math:`\rho` is the fluid density, :math:`g` is the gravitational acceleration,
:math:`\lambda_i` is the unit vector in the direction of gravity and :math:`\kappa` is the thermal diffusion.

Hierarchical Framework
======================

Underworld is a program that sits at the top of a hierarchy of other programs. Each of these
programs builds on the previous programs expanding the scientific or computational capacity of
the underlying programs. The goal of this approach is to modularise the code at a design level,
which enables users to work at a particular level of the code without having to modify higher
or lower levels significantly.

.. figure:: _static/UnderworldStructure.*
   :align: center
   
   Underworld Hierarchical Structure

The following describes the components in each layer.

 * **StGermain** - Core parallel programming architecture, memory management and OO frameworks.
 * **StgDomain** - Basic mathematical formulations, geometry, mesh, shape and basic swarm framework.
 * **StgFEM** - Finite Element Method (FEM) framework (Eulerian).
 * **PICellerator** - Particle in Cell methods (Lagrangian / Eulerian framework).
 * **gLucifer** - Visualisation package.
 * **Underworld** - Geophysical formulations, e.g. rheologies.

StGermain
---------

StGermain is designed to handle the core parallel programming architecture, memory management
and Object Oriented (OO) frameworks. This is where the code that runs journalling, memory
management, and the various definitions of *containers* such as variables, resides.
This is the layer that allows higher end users to use the same model files across one or multiple
computers. StGermain forms part of the base level of the hierarchy.
Most users will never need to look into this code base.

StgDomain
---------

StgDomain handles the geometry and meshing definitions, (located in the *Geometry* and *Mesh*
sub-directories respectively) as well as basic mathematical formulas (located in *Geometry* sub-directory).
StgDomain also contains all the code on how to move particle swarms at the most basic level.
Users writing their own plugins or components may need to use this layer for it’s available
pre-written mathematical functions.

StgFEM
------

StgFEM follows StGermain’s approach of breaking the Finite Element Method (FEM) into a
component-based, declarative approach, making considerable use of the *Strategy Design Pattern*.

The motivation behind this approach are:

 * Setting it up as a framework that can solve different sets of linear equations.
 * In particular, allowing declarative specification of commonly used elements in a FEM
   system-important for complex systems such as Stokes, and coupled systems.
 * High degree of modularity for each phase of the FEM process - with the view of other codes
   being able to modify all of them.

It is organised around the concept of *System of Linear Equations* and:

 * Assumes the numerical modeller has already translated from weak form to a matrix, vector system.
 * It also puts responsibility on the numerical modeller to be able to translate algebraic terms as
   they appear in the weak form, to assembly term components.

StgFEM has a number of input files/basic models in the *Apps* directory that are well worth
looking at and running. These include the base models for many more complicated XML models.

Some of the XMLs include:

 * The Energy Solver
 * Stokes Momentum Uzawa
 * Thermal Convection
 * Temperature Diffusion
 * and a number of smaller XML files in *StgFEM_Components* directory.

StgFEM has a number of available *plugins* in the plugins sub-directory that can be of use to
the user.

PICellerator
------------

PICellerator is a fairly *lightweight* layer in the framework, in terms of number of new components
provided. It is closely related to the StgFEM layer immediately below it.
PICellerator uses a Particle-In-Cell approach, with the collection of particles and associated
properties known as a *swarm*.

PICellerator’s approach has the following key concepts:

 * Particle-In-Cell material points
 * Integration of FEM at Gauss points is replaced by scheme related to material points
   (by default, integration points and material points are co-located, but this can be changed).
 * Material points are then advected based on the solution of the FEM.

.. Note:: A material point does not map to physical particles, but is designed to
    show the material properties in a local region of the model.

Material and Integration Swarms
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

There are two types of swarms in PICellerator:

 * **Material swarms** - for physical material properties that move.
 * **Integration swarms** - defines discrete integration points.

These two swarms are related, since material properties are often required when integrating, but
the code does not necessarily require that the two be co-located.
PICellerator deals with the difficult problem of locating to which cell a given particle belongs,
particularly across multiple processors. The cells correspond to an *Eulerian Mesh*, with the *Material*
and *Integration swarms* handling the *Lagrangian particle advection*.
PICellerator also deals with population control as particles are advected in and out of the
modelling space; and weighting schemes for integration using both simple and more complicated
methods (i.e. *Voronoi schemes*).

There are a number of XML examples available in the *Apps* sub-directory, including:

 * The Rayleigh Taylor Benchmark
 * Lid Driven PIC convection
 * Thermochemical convection
 * and the Buoyancy Benchmark

PICellerator has a number of available plugins in the *plugins* sub-directory that can be of use
to the user.

Underworld
----------

Underworld forms the top layer in the hierarchy of programs. Underworld at this level, is designed
to deal with the models governing various geophysical rheologies. Most of the component models in
Underworld are located in the *Rheology* sub-directory.

Examples of problems that can be solved, for which a number of *template* XML models are provided
are found in the *InputFiles* sub-directory:

 * **Thermal convection**

   * Temperature and stress-dependent convection;
     (e.g. *NonNewtonian.xml*, *MultiComponent.xml*)
   * Temperature-dependent convection; (e.g. *Arrhenius.xml*, *FrankKanenetskii.xml*)
   * Anisotropic rheology; (e.g. *Anisotropic.xml*)
   * Plastic rheology; (e.g. *MobileLid.xml*)
   * Radiogenic heating; (e.g. *InternalHeating.xml*)
   * Depth-dependent viscosity; (e.g. *DepthDependentViscosity.xml*)
   * and Multi-thermal diffusivity. (e.g. *MultiThermalDiffusivity.xml*)
 * **Chemical convection**

   * The Rayleigh-Taylor benchmark; (e.g. *RayleighTaylorBenchmark.xml*)
   * and subducting / falling viscoplastic slab. (e.g. *SlabSubduction.xml*)
 * **Thermochemical convection**

   * D” (core-mantle boundary) of the Earth. (e.g. *ThermoChemBenchmark.xml*)
 * **Viscoplastic extension**

   * Von-Mises Extension; (e.g. *Extension.xml*)
   * and Faulting Moresi-Muhlhaus, 2006 Extension. (e.g. *ExtensionFMM.xml*, *ExtensionFMM3D.xml*)
