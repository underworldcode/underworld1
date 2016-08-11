.. _udw-preface:

*******
Preface
*******

.. An introductory document about Underworld

Introduction
============

Underworld is a 3D-parallel geodynamic modelling framework capable of deriving
viscous/visco-plastic thermal, chemical and thermochemical models
consistent with tectonic processes, such as mantle convection and lithospheric
deformation over long time scales.

Underworld utilises a Lagrangian particle-in-cell finite element scheme
(the prototype of which is the Ellipsis code), and is visualised using gLucifer.
The Underworld source code is written in C in an Object Oriented style,
following the methodology of design for change applied to computational codes
implemented in `StGermain <http://www.stgermainproject.org/>`_.

The Underworld to StGermain software stack is released under a mixture of
BSD and LGPL licences. It uses `PETSc <http://www.mcs.anl.gov/petsc/>`_
(optimised numerical solvers) and MPI (parallelism) libraries.

All the projects that make up Underworld now use the CREDO toolkit for
system-level testing. CREDO is an open-source toolkit for benchmarking
and analysis of scientific modelling software written in Python,
and is developed and maintained along with Underworld.
CREDO uses the LGPLv2.1 licence.

Underworld Users
================

The main audience for Underworld, the 3D parallel geodynamic modelling software,
are research scientists interested in modelling geodynamic processes on
long time scales. Computer programmers and mathematical modellers would
also find the underlying modelling framework of interest.

Citation
========

Underworld is made available under the `GNU General Public Licence <http://www.gnu.org/licenses/lgpl.html/>`_,.
A number of individuals have contributed a significant proportion of their careers
towards the development of Underworld. It is essential that you recognise these
individuals in the normal scientific practice by making appropriate acknowledgements.

The code is based on the method described in [MoresiQuenetteLemiale2007]_.
And the core algorithms and structure are based on the method described in [MoresiDufourMuhlhaus2003]_.

This code is developed by the Victorian Partnership of Advanced Computing (VPAC),
computational software developers in collaboration with Monash University’s
Monash Geodynamics group, led by Louis Moresi, as part of the AuScope nation-wide
Geosciences infrastructure program. The Underworld team requests that in your oral
presentations and in your publications that you indicate your use of this code and
acknowledge the authors of the code, `Victorian Partnership for Advanced Computing <http://www.vpac.org/>`_
and `Monash University <http://www.monash.edu.au/>`_.

Support
=======

`Underworld`_ is under development as part of `AuScope Ltd <http://www.auscope.org/>`_,
which is funded under the National Collaborative Research Infrastructure Strategy
(`NCRIS <http://ncris.innovation.gov.au/>`_), an Australian Commonwealth Government Programme.
(Formerly Underworld was a project as part of the Australian Computational Earth Systems Simulator
(`ACcESS MNRF <http://www.access.edu.au/>`_).

`Underworld`_ is jointly maintained by the Victorian Partnership for Advanced Computing (`VPAC <www.vpac.org/>`_)
and `Monash University <http://www.monash.edu.au/>`_.

.. [MoresiQuenetteLemiale2007] Moresi L., Quenette S., Lemiale V., Meriaux C., Appelbe B., and Mühlhaus H. B. 2007.
 Computational approaches to studying non-linear dynamics of the crust and mantle.
 Physics of the Earth and Planetary Interiors 163, 69-82.

.. [MoresiDufourMuhlhaus2003] L.N. Moresi, F. Dufour and H.B. Mühlhaus, A Lagrangian integration point finite element
 method for large deformation modelling of viscoelastic geomaterials, J. Comp. Phys.,
 Volume 184, pp. 476-497, 2003.

.. _Underworld: http://www.underworldproject.org  
