.. _credo-documentation:

How to build CREDO Documentation locally
========================================

CREDO uses the Sphinx Python documentation package to manage and build 
its documentation, see http://sphinx.pocoo.org/.

It uses several Sphinx plugins to auto-generate various documentation
sections such as class diagrams, requiring Graphviz to be installed.
If you want it to generate Latex output as well as HTML, you will
need to install a suitable Latex compiler as well.

On Linux systems such as Ubuntu, you should be able to install the Sphinx
package.

On Mac Os X, we've tested using the `py26-sphinx` and `graphviz` MacPorts
packages and it's working. You will need to set the variable
`SPHINXBUILD=sphinx-build-2.6` as Macports installs the Sphinx binaries
with a non-standard name.
