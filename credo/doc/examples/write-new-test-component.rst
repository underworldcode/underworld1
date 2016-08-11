.. _credo-examples-write-new-test-component:

Writing new CREDO Testing (and analysis) components
---------------------------------------------------

.. note:: This is currently a very bare-bones description, needs to be expanded upon.
   In future refactors we also hope to simplify this process.

The recommended steps are essentially as follows:

#. Test that your new code works, e.g. just write a basic Python script
   (Use Docstrings, and ideally list arguments of your functions).

#. Write a :mod:`credo.analysis` component to perform your operation

   .. note:: Remember to add the CREDO header/licence text
      (just copy it from one of the other files).

#. Write a new :class:`credo.systest.api.TestComponent`, in the credo/systest
   directory.

   * specify members needed by the class in the "init" function;
   * Fill out the "check" function;
   * Save specification of the test in the _writeXMLCustomSpec() function;
   * Save results of the test in the _writeXMLCustomResult() function.

#. Write a Python unittest that your new test component works

   This should live in the 'tests' sub-directory.

#. (Optional) Write a new SysTest component that will use your new TestComponent.

   E.g. in the case of the Image testing, the new component that simply creates a
   SysTest, attaches an image testing TestComponent, and passes relevant images through.

#. Add the new SysTest and TestComponent to the `credo.systest.__init__.py`
   systest module file's import lists, so they can be easily imported by
   user system testing scripts.

#. add your new modules to the Sphinx doc-generator, to be auto
   generated. E.g. for images:

   To file credo/doc/api/analysis.rst, added section::

     :mod:`credo.analysis.images`
     ============================

     .. automodule:: credo.analysis.images
        :members:
        :undoc-members:
        :show-inheritance:

   TODO: also add the new component to the documentation list to generate main inheritance
   diagrams for SysTests and TestComponents.

