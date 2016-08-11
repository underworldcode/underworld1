.. _credo-glossary:

*****************
Glossary of Terms
*****************

.. glossary::
   :sorted:

   API
     API - Application Programming Interface. This means a defined, documented
     interface for using any software tool or framework, that others can use in
     their programs to complete a task.

   Benchmark
     In the context of CREDO, this means a test of a code's results/performance
     according to some metric, which has generally been published in the
     literature.

   Mesh
     A mesh of nodes or data in a discretised domain. For the reference to the
     Mesh class provided by StGermain and Underworld, see 
     http://www.auscope.monash.edu.au/codex/StGermain.html#Mesh.

   Field
     A 3D domain - in StGermain codes, the FieldVariable class provides an
     interface for accessing data from Fields, which are usually discretised as
     either a :term:`Mesh` or :term:`Swarm`. For reference to the FieldVariable
     class, see
     http://www.auscope.monash.edu.au/codex/StgDomain.html#FieldVariable

   Swarm
     In the context of StGermain-based codes, a "Swarm" refers to a set of
     particles. In Underworld a swarm with a large number of particles
     is used to discretise a :term:`Field` via individual material points. See
     http://www.auscope.monash.edu.au/codex/StgDomain.html#Swarm.

   Input File
     In the StGermain context, see :term:`Model`.

   Model
     In the context of StGermain-based codes, a "Model" refers to a complete,
     consistent scientific application: encapsulated by a set of StGermain
     components. These are normally represented by a set of XML files specifying
     these components. Sometimes referred to as simply an "input file" to
     Underworld, and stored in an InputFiles sub-directory.

   Model Run
     In the context of CREDO, this means the specification of a :term:`Model` to
     run, PLUS the metadata required for the run (e.g. over-rides of simulation
     parameters, processors to use, whether to run locally or over the grid,
     etc). See the :mod:`credo.modelrun` module.

   Model Result
     In the context of CREDO, this refers to the "result" of a :term:`Model Run`.
     This includes all the data the model run produced, usually stored in an
     output directory (such as checkpoint files, and the FrequentOutput.dat
     summary), plus any 'meta-data' about how long the model took to run, how
     much data was used, etc. See the :mod:`credo.modelresult` module.

   System Test
     In the context of software, a test that the entire software system works as
     expected, for some sort of non-trivial problem.

   Underworld
     A geophysics modelling framework, implemented using :term:`StGermain` - see
     http://www.underworldproject.org

   StGermain
     An object-oriented framework, written in C, to enable the development of
     Scientific applications to run on parallel computers. See
     http://www.stgermainproject.org

   Virtual method
     A method of a class that is not actually implemented, and thus requires
     sub-classes to implement.

   VRMS
     Velocity Root Mean Squared. An observable commonly calculated for
     convection runs of the Underworld code.
