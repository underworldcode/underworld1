***********************************
SysTest API
***********************************

.. automodule:: credo.systest

:mod:`credo.systest.api`
==========================

.. inheritance-diagram:: credo.systest.api

.. automodule:: credo.systest.api
   :members:
   :undoc-members:
   :show-inheritance:

:mod:`credo.systest.systestrunner`
==================================

.. inheritance-diagram:: credo.systest.systestrunner

.. automodule:: credo.systest.systestrunner
   :members:
   :undoc-members:
   :show-inheritance:

Core System Test class implementations
======================================

CREDO provides a set of core :class:`~credo.systest.api.SysTest` instantations,
which supercede the functionality of the pre-existing test scripts system,
which are documented below.

The user can always add to this list, by defining new SysTest classes to use.

The most flexible of the set is the
:class:`~credo.systest.scibenchmark.SciBenchmarkTest`, but this requires the
most customisation (i.e. generally can't be created in the short-hand form
of the other tests using the sysTestRunner's
:meth:`~credo.systest.systestrunner.SysTestRunner.addStdTest` method).

.. inheritance-diagram:: credo.systest.analyticTest
    credo.systest.restartTest credo.systest.referenceTest 
    credo.systest.analyticMultiResTest credo.systest.highResReferenceTest
    credo.systest.sciBenchmarkTest

depending on whether you have the Python Imaging Library (PIL) installed,
you can also use the :class:`credo.systest.imageReference.ImageReferenceTest`
system test.

:mod:`credo.systest.analyticTest`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. automodule:: credo.systest.analyticTest
   :members:
   :undoc-members:
   :show-inheritance:

:mod:`credo.systest.referenceTest`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. automodule:: credo.systest.referenceTest
   :members:
   :undoc-members:

:mod:`credo.systest.restartTest`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. automodule:: credo.systest.restartTest
   :members:
   :undoc-members:

:mod:`credo.systest.analyticMultiResTest`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. automodule:: credo.systest.analyticMultiResTest
   :members:
   :undoc-members:
   :show-inheritance:

:mod:`credo.systest.imageReferenceTest`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. automodule:: credo.systest.imageReferenceTest
   :members:
   :undoc-members:
   :show-inheritance:

:mod:`credo.systest.sciBenchmarkTest`
=====================================

.. automodule:: credo.systest.sciBenchmarkTest
   :members:
   :undoc-members:
   :show-inheritance:

Core TestComponent implementations
==================================

.. inheritance-diagram:: credo.systest.fieldWithinTolTC
    credo.systest.fieldCvgWithScaleTC credo.systest.outputWithinRangeTC

:mod:`credo.systest.fieldWithinTolTC`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. automodule:: credo.systest.fieldWithinTolTC
   :members:
   :undoc-members:
   :show-inheritance:

:mod:`credo.systest.fieldCvgWithScaleTC`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. automodule:: credo.systest.fieldCvgWithScaleTC
   :members:
   :undoc-members:
   :show-inheritance:

:mod:`credo.systest.outputWithinRangeTC`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. automodule:: credo.systest.outputWithinRangeTC
   :members:
   :undoc-members:
   :show-inheritance:

:mod:`credo.systest.imageCompTC`
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. automodule:: credo.systest.imageCompTC
   :members:
   :undoc-members:
   :show-inheritance:
