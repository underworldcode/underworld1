.. _credo-release-process:

Process to follow for creating new CREDO releases
=================================================

General release configuration description
-----------------------------------------

CREDO follows a fairly standard open source project release naming convention,
where releases are named MAJOR.MINOR.MICRO, e.g. 0.1.3, 
where MAJOR is the major release, MINOR is the micro release, and MICRO
are small releases with incremental updates/bugfixes.

CREDO follows a typical 'named branches' and tags strategy for managing
these versions and releases in the Mercurial repository:

* There are 'named branches' for major release lines, e.g. `credo-0.1`

* Actual micro releases are tags along these branches of when the code was
  released, e.g. `credo-0.1.1`

* That way, patches/fixes can be ported across to the release branch lines
  from the 'default' trunk branch if necessary.

.. note:: The following websites are useful references for how named branches, 
   especially w.r.t. releases, can be managed in Mercurial:

   * http://hgbook.red-bean.com/read/managing-releases-and-branchy-development.html
   * http://mercurial.selenic.com/wiki/NamedBranches
   * http://stackoverflow.com/questions/890723/mercurial-named-branches-vs-multiple-repositories (especially Geoffrey Zheng's answer).

Release Process Steps
---------------------

Follow these steps when creating a CREDO release:

#. Review the README.txt file, and if necessary make any updates (eg updating
   the list of contributors, or web links).

#. Create/switch to the release branch:

   * In the cases where the release branch doesn't exist, you need to create it
     by using a "hg branch BRANCHNAME" command.

   * In cases where it does exist, just switch to the branch using 
     "hg update -C BRANCHNAME".

#. Create a CHANGELOG file for the release:

   * use Mercurial command to select all changes since last release tag made,
     and save to a file in the changelogs subdirectory.
     E.g. for getting changes between the 0.1.2 and the tip (upcoming 0.1.3
     release)::

       hg log -v -r credo-0.1.2:tip > changelogs/CHANGELOG-0_1_3

     .. note:: Save this to file in the "changelogs" subdirectory named
               `CHANGELOG-X-Y-Z`, where X, Y, and Z are the major, minor
               and micro revisions.

   * Edit the file you just created, and add an appropriate header, e.g. ::

       ===================================================================
       Mercurial commits made during CREDO development from 0.1.2 to 0.1.3
       ===================================================================

   * Remember to `hg add` the file, you will commit it later.

   .. note:: We would be interested in any better method for assembling 
            changelogs in Python mercurial code that helps select changes
            of real interest.

#. Update the `doc/credo-whatsnew.rst` file:

   * Add a new section header for the new release following on the pattern
     of previous ones, and add a short summary of key new features in
     bullet point form.

     The CHANGELOG file you just created in the previous step should be
     helpful in doing this, as well as trac tickets and roadmaps.

     Where appropriate use Sphinx links in this summary to help users find
     new features.

#. Update the release number in the documentation conf.py file:

   This will look something like the following, just edit as appropriate::

     # The short X.Y version.
     version = '0.1'
     # The full version, including alpha/beta/rc tags.
     release = '0.1.2'

#. At this stage you should commit all the changes made above to the release
   branch, and also tag the release code::

     hg commit
     hg tag TAGNAME
     hg push

#. Create a source tarball of this release:
 
   Make sure you are in the root directory of your CREDO checkout and type::

     hg archive -t tgz ../credo-X.Y.Z.tar.gz
     
   ... where `X.Y.Z` is the release number, e.g. `0.1.3`. This will create
   the tarball in the parent directory.

#. Update links and upload the just-created tarball in the release table at
   https://www.mcc.monash.edu.au/trac/AuScopeEngineering/wiki/CREDO

#. Now make sure you commit all the changes you just made on the release branch
   back to the default trunk development branch::

     hg update default
     hg merge RELEASE_BRANCH
     hg commit
     hg push
   
   .. note:: This will mark your release branch as (inactive), but this is
      fine: if it's needed you can hg update to it and reactivate it later.

#. TODO: in future we should perhaps have a process to create a copy of
   the documentation of each release (PDF/html) and put online.

