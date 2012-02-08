Release and packaging procedure
===============================

Versioning scheme
-----------------

mCRL2 version numbers are standardised to three fields: ``YYYYMM.B.RRRRR``.
Here ``YYYY`` is the year, ``MM`` the month of the last release. ``B`` is the
bugfix number. ``B = 0`` for the planned (initial) release of a version.
``RRRRR`` is the SVN revision number.

For an official release, the tools will show the version ``YYYYMM.B``,
development versions will show all three fields. This enables quick
identification of the version you are dealing with by inspecting the output of
:option:`--version`. When the toolset is built from an SVN checkout with
modified sources, the revision number is appended with ``M``, such that the
:option:`--version` output shows that the checkout was not clean.

Version numbers are computed in :file:`scripts/MCRL2Version.cmake`. This is the
*only* place where the main version (``YYYYMM.B``) can be defined. The rest of
the source code *should not* contain hardcoded version numbers.

Creating an official release
----------------------------

Creating release branch
^^^^^^^^^^^^^^^^^^^^^^^

The tag for an mCRL2 release is always created from the release branch
`https://svn.win.tue.nl/repos/MCRL2/branches/release`_. This branch can be
updated from `trunk` by::

  $ svn merge https://svn.win.tue.nl/repos/MCRL2/trunk https://svn.win.tue.nl/repos/MCRL2/branches/release
  
Tagging
^^^^^^^



Source release
^^^^^^^^^^^^^^

The source package for the mCRL2 release is generated as follows:

#. 


Debian/Ubuntu packages
^^^^^^^^^^^^^^^^^^^^^^


Windows installer
^^^^^^^^^^^^^^^^^


Creating a development snapshot
-------------------------------

Source tarball
^^^^^^^^^^^^^^

A development snapshot can be created by::

  $ svn checkout https://svn.win.tue.nl/repos/MCRL2/trunk trunk
  $ mkdir build
  $ cd build
  $ cmake ../trunk
  $ make package_source
  
This command sequence will generate a source tarball in
:file:`build/mcrl2-VERSION.REVISION.tar.gz`, where ``VERSION`` is the major
version of mCRL2, and REVISION is the revision from which the snapshot was
created.
  
.. warning::

   Do not generate a source tarball using ``svn export``. This will not record
   the information of the SVN revision from which the export was made.

Debian/Ubuntu package
^^^^^^^^^^^^^^^^^^^^^

The building of Debian and Ubuntu packages is based on the source tarball.
Create a directory for the purpose of packaging, say ``ubuntu``, and copy the
tarball to it. Then unpack the tarbal and get the debian configuration. For this
example, we use the source package :file:`mcrl2-201107.1.10245.tar.gz`::

  $ cp mcrl2-201107.1.10245.tar.gz ubuntu
  $ cd ubuntu
  $ cp mcrl2-201107.1.10245.tar.gz mcrl2_201107.1.10245.orig.tar.gz
  $ tar -zxvf mcrl2-201107.1.10245.tar.gz
  $ svn export https://svn.win.tue.nl/repos/MCRL2/packaging/mcrl2/trunk/debian mcrl2-201107.1.10245/debian
  $ cd mcrl2-201107.1.10245

.. warning::

   Note the ``_`` in the name of the file with the ``.orig.tar.gz`` extension.
   This is important!

Now, we update the changelog entry, here we have some different possibilities.

If you are creating a package for debian::
 
  $ dch -v 201107.1.10245-0 -D unstable
  
If you are creating a package for ubuntu (say Ubuntu Oneiric)::

  $ dch -v 201107.1.10245-0ubuntu0 -D oneiric
  
If you are creating a package that should be uploaded to the Ubuntu mCRL2 
development ppa (`https://launchpad.net/~mcrl2/+archive/devel-ppa`_), and you
are building for Ubuntu Oneiric. This allows you to generate the pacage for
multiple Ubuntu versions in the same PPA::

  $ dch -v 201107.1.10245-0ubuntu0+1~oneiric -D oneiric
  
Update the changelog entry to contain the string "Snapshot release" as first
line, and save the changelog.

A source package can now be generated using::

  $ debuild -S -sa
  
This will also sign the source package using your PGP key.

You can now build the package locally (assuming build for Ubunto Oneiric)::

  $ cd ..
  $ pbuilder-dist oneiric build mcrl2-201107.1.10245-0ubuntu0+1~oneiric.dsc
  
This will build the package in a clean environment. As an alternative, you can
upload the package to the mCRL2 development PPA on launchpad using::

  $ dput ppa:mcrl2/devel-ppa mcrl2-201107.1.10245-0ubuntu0+1~oneiric_source.changes




