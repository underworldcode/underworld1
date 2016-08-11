.. _credo-architecture:

*****************************************
Core CREDO Architecture
*****************************************

This section is aimed to give you a quick overview of the architecture of the
CREDO toolkit. It's strongly suggested you read at least the
:ref:`credo-intro-workflow` section from the introductory section first, to put
this in perspective. This architecture description may help illustrate the
:ref:`credo-examples`, and help explain the details in the :ref:`credo-apidocs`.

This diagram gives an idea of the essential architecture of the CREDO toolkit's
classes:

.. image:: _static/CREDO-basicArchitecture.*

The diagram shows that the CREDO framework works at two main levels:

* Running and analysis of models (inside the dotted box): ability to construct a
  :term:`Model Run`, run it, and analyse the results.
* Testing and benchmarking: run and analyse models in defined ways, to allow
  system testing. Thus, if CREDO is being used purely for testing, it can operate
  at a high-level of abstraction and manage the details of constructing and
  analysing models [#f1]_.

Inside the dotted lines are shown the core Analysis functionality and classes.
They implement the concept discussed in the :ref:`credo-intro-workflow`, of the
ability to abstract and manage a StGermain-based code simulation through the
concepts of a :term:`Model Run` and :term:`Model Result`. For each ModelRun, one
or more analysis operations (AnalysisOps) can be applied to it, which will also
produce data that can be referenced through an AnalysisResult.

Thus what essentially happens for an analysis or testing run is generally: 

* A script constructs a ModelRun (or suite of runs as a ModelSuite),
  and customises it, possibly including specifying AnalysisOps.
* The model is ran, producing a ModelResult class.
* This class can then be used as a basis for post-processing, with a defined set
  of capabilities that allow the user to explore the results of the model, and
  perform analysis on those results. 

.. Note::

   In future, we may allow the ability of running a set of AnalysisOps as a
   post-processing operation on the results of a ModelRun, based on checkpointed
   data. However, this feature is not currently implemented.

The examples in the :ref:`credo-examples` section help illustrate how these
capabilities work in practice.

Organisation of Python Code and class hierarchy
-----------------------------------------------

As usual for Python projects, the code is separated into a set of directories of
related functionality called *packages*, which are then further divided into
individual *modules*.

.. note::
   CREDO uses Python's object-oriented capabilities to manage a set of Classes to
   implement this architecture. Python is designed to be easy to pick up,
   and from a user's perspective the object-oriented features shouldn't make
   scripting difficult. Thus if you only intend to write CREDO scripts to perform
   analysis, you should be able to move straight to the :ref:`credo-examples`
   without worrying about how Python classes work in detail. 
   However if you intend to add and develop new Analysis capabilities,
   you may wish to run through the Python tutorial's `section on Classes
   <http://docs.python.org/tutorial/classes.html>`_ as a primer
   to get you started.

These key models and packages are described and documented in the
:ref:`credo-apidocs` section. The linked diagrams below show several of
the key classes that implement the architecture discussed above:

The core ModelRun, ModelResult and Analysis hierarchy:

.. inheritance-diagram:: credo.modelrun credo.modelresult
    credo.analysis.api credo.analysis.fields
    :parts: 1

And several key System testing classes:

.. inheritance-diagram:: credo.systest.api credo.systest.analyticTest
    credo.systest.restartTest credo.systest.referenceTest
    credo.systest.analyticMultiResTest credo.systest.sciBenchmarkTest
    credo.systest.fieldWithinTolTC credo.systest.fieldCvgWithScaleTC
    credo.systest.outputWithinRangeTC
    :parts: 1

.. rubric:: Footnotes

.. [#f1]  Although for complex benchmarks, the user will likely need to work
   with and customise the models at a more detailed level, which is supported.
