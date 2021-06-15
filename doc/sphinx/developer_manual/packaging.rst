Release and packaging procedure
===============================

Versioning scheme
-----------------

mCRL2 version numbers are standardised to three fields: ``YYYYMM.B.HHHHH``.
Here ``YYYY`` is the year, ``MM`` the month of the last release. ``B`` is the
bugfix number. ``B = 0`` for the planned (initial) release of a version.
``HHHHH`` is the Git commit hash.

For an official release, the tools will show the version ``YYYYMM.B``,
development versions will show all three fields. This enables quick
identification of the version you are dealing with by inspecting the output of
:option:`--version`. When the toolset is built from a Git clone with
modified sources, the commit hash is appended with ``M``, such that the
:option:`--version` output shows that the checkout was not clean.

Version numbers are computed in :file:`scripts/MCRL2Version.cmake`. This is the
*only* place where the main version (``YYYYMM.B``) can be defined. The rest of
the source code *should not* contain hardcoded version numbers.

Creating an official release
----------------------------

This section discusses the steps that need to be taken for creating an official
mCRL2 release. Below, the version number ``YYYYMM.B`` should be replaced
everywhere according to the scheme described above.

Creating release branch
^^^^^^^^^^^^^^^^^^^^^^^

The first step to take is creating a release branch from ``master`` once
master is deemed stable::

  $ git branch release-YYYYMM.B
  $ git checkout release-YYYYMM.B
  $ git push origin release-YYYYMM.B

Testing
^^^^^^^

.. warning::

   Before starting with testing, make sure that there is no other mCRL2
   installation in your ``PATH``!

For all testing, we assume that the release branch is checked out in ``/tmp``::

  $ cd /tmp
  $ git clone https://github.com/mCRL2org/mCRL2.git -b release-YYYYMM.B /tmp/mcrl2-release

Before the actual release is tagged, the release branch must be tested on all
supported platforms::

* Windows 64-bit
* Ubuntu 32-bit
* Ubuntu 64-bit
* Fedora 64-bit
* Mac OS X 32-bit
* Mac OS X 64-bit


Testing is done by running (a platform specific variation of)::

  $ mkdir mcrl2-release-build
  $ cd mcrl2-release-build
  $ cmake ../mcrl2-release -DCMAKE_INSTALL_PREFIX="/tmp/mcrl2-release-build/install" \
                           -DCMAKE_BUILD_TYPE="Release" \
                           -DMCRL2_ENABLE_TEST_TARGETS="ON" \
                           -DMCRL2_ENABLE_EXPERIMENTAL="ON" \
                           -DMCRL2_ENABLE_DEPRECATED="ON" \
                           -DMCRL2_ENABLE_RELEASE_TEST_TARGETS="ON" \
                           -DMCRL2_ENABLE_RANDOM_TEST_TARGETS="ON"
  $ make install
  $ ctest
  $ mkdir mcrl2-debug-build
  $ cd mcrl2-debug-build
  $ cmake ../mcrl2-release -DCMAKE_INSTALL_PREFIX="/tmp/mcrl2-debug-build/install" \
                           -DCMAKE_BUILD_TYPE="Debug" \
                           -DMCRL2_ENABLE_TEST_TARGETS="ON" \
                           -DMCRL2_ENABLE_EXPERIMENTAL="ON" \
                           -DMCRL2_ENABLE_DEPRECATED="ON" \
                           -DMCRL2_ENABLE_RELEASE_TEST_TARGETS="ON" \
                           -DMCRL2_ENABLE_RANDOM_TEST_TARGETS="ON"
  $ make install
  $ ctest



Furthermore, integration with LTSmin must be tested on Linux:

*LTSmin support*

  Run the following commands::

    $ mkdir /tmp/mcrl2-ltsmin-build
    $ cd /tmp/mcrl2-ltsmin-build
    $ cmake ../mcrl2-release -DCMAKE_INSTALL_PREFIX="/tmp/mcrl2-ltsmin-build/install"
    $ make install
    $ cd /tmp
    $ git clone http://fmt.cs.utwente.nl/tools/scm/ltsmin.git ltsmin
    $ cd ltsmin
    $ git checkout -b main origin/maint
    $ git submodule update --init
    $ ./ltsminreconf
    $ ./configure --disable-dependency-tracking --with-mcrl2=/tmp/mcrl2-ltsmin-build/install --prefix=/tmp/ltsmin/install
    $ make install
    $ /tmp/mcrl2-ltsmin-build/install/bin/mcrl22lps /tmp/mcrl2-release/examples/academic/abp/abp.mcrl2 abp.lps
    $ export PATH=/tmp/mcrl2-ltsmin-build/install/bin:$PATH
    $ export LD_LIBRARY_PATH=/tmp/mcrl2-ltsmin-build/install/lib/mcrl2:$LD_LIBRARY_PATH
    $ /tmp/ltsmin/install/bin/lps2lts-mc abp.lps
    $ /tmp/ltsmin/install/bin/lps2lts-sym abp.lps
    $ /tmp/ltsmin/install/bin/lps2lts-dist abp.lps

Updating release number and copyright information
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* Update the version number in the file
  ``scripts/MCRL2Version.cmake`` by updating the value assigned to the CMake
  variable ``MCRL2_MAJOR_VERSION``.

* Update the copyright period in the file ``COPYING``.

* In ``doc/sphinx/source/user_manual/historic_releases.rst``, add the previous
  release.

* Commit the changes to both ``release-YYYYMM.B`` and ``master``.

Tagging
^^^^^^^

Once the above procedure has been carried out, and all test have succeeded,
it is time to tag the release. This can be done using the interface of Github,
where a new tag can be created while making a release.

Source release
^^^^^^^^^^^^^^

The source package for the mCRL2 release is generated as follows::

  $ cd /tmp
  $ git clone https://github.com/mCRL2org/mCRL2.git -b mcrl2-YYYYMM.B sources
  $ mkdir mcrl2-package
  $ cd mcrl2-package
  $ cmake ../sources -DMCRL2_PACKAGE_RELEASE="ON"
  $ make package_source

Upload the source package::

  $ scp mcrl2-VERSION.tar.gz mcrl2@linux2.webhosting.tue.nl:www/download/release

Debian/Ubuntu packages
^^^^^^^^^^^^^^^^^^^^^^

Debian packages can be built using the script in ``build/make_release``. This
requires an account on Launchpad. Note that a failed upload requires the version
number to be increased, so proper testing of the package is required before
attempting to upload it.

Windows release
^^^^^^^^^^^^^^^

We assume here that cpack.exe (provided by CMake) is in PATH.

First, clone the correct release branch of the mCRL2 source using a Git client
to some folder (called ``source`` from now on). Then open CMake, enter
``source`` as the folder with the source code and enter some folder for the
binaries (called ``build`` from now on). On top of :doc:`configuring the CMake
options normally <../user_manual/build_instructions/configuration>`, also set
MCRL2_PACKAGE_RELEASE to true. Then click on configure and set the generator to
Visual Studio (the version used for development) and the platform to x64.
Afterwards click on generate and then on open project to open the project in
Visual Studio. Put the project in the ``Release`` configuration and build it
(by building ``BUILD_ALL``).

When the project is done building, open ``build`` in a command prompt. Then
execute ``cpack -G NSIS`` to create the installer and ``cpack -G ZIP`` to create
the zip file.

Lastly, upload both the generated installer and the zip file to
``http://www.mcrl2.org/download/release``.

Mac OS-X installer for 10.5+
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

First check out ``tags/mcrl2-VERSION`` using a Git client, assume to
``mcrl2-VERSION``. It is desirable to compile with the newest version of Qt to
avoid bugs.

Configure cmake::

  $ cmake . -DCMAKE_OSX_DEPLOYMENT_TARGET={SDK version (e.g. '10.9')} \
            -DCMAKE_OSX_SYSROOT={/Path/To/SDK} \
            -DCMAKE_INSTALL_PREFIX=/ \
            -DMCRL2_PACKAGE_RELEASE=ON

Build the toolset::

  $ make

Create the DMG-installer::

  $ cpack -G DragNDrop

Upload the disk image that has been generated to
``http://www.mcrl2.org/download/release``.

Checking the installers
^^^^^^^^^^^^^^^^^^^^^^^

Double check the installers that have been built, as well as a build from
the source tarball succeeds!
