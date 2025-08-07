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
`--version`. When the toolset is built from a Git clone with
modified sources, the commit hash is appended with ``M``, such that the
`--version` output shows that the checkout was not clean.

Version numbers are computed in :file:`cmake/MCRL2Version.cmake`. This is the
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

Before the actual release is tagged, the release branch must be tested on all
supported platforms::

* Windows 64-bit
* Ubuntu 32-bit
* Ubuntu 64-bit
* Fedora 64-bit
* Mac OS X 32-bit
* Mac OS X 64-bit

Updating release number and copyright information
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* Update the version number in the release branch in the file in the
  ``cmake/MCRL2Version.cmake`` by updating the value assigned to the CMake
  variable ``MCRL2_MAJOR_VERSION``.

* Update the copyright period in the file ``README.md``.

* In ``doc/sphinx/source/user_manual/historic_releases.rst``, add the previous
  release.

* Commit these last changes to ``master``.

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
attempting to upload it. Add your gpg key to the machine where the package is
being build, using ``gpg --import <key>``. The script will automatically sign
the package.

If uploading succeeds, but building fails, we add another minor version to the
package name that is incremented until the build works.

Fedora packages
^^^^^^^^^^^^^^^^

Fedora packages are created by using a default cmake setup with
`-DMCRL2_PACKAGE_RELEASE=ON` set. The package is then build using `cpack -G
RPM`.

Windows release
^^^^^^^^^^^^^^^

We assume here that cpack.exe (provided by CMake) is in PATH.

Open CMake, enter ``source`` as the folder with the source code and enter some
folder for the binaries (called ``build`` from now on). On top of
:ref:`configuring the CMake options normally <build-instructions>`, also set
MCRL2_PACKAGE_RELEASE to true. Then click on configure and set the generator to
Visual Studio (the version used for development) and the platform to x64.
Afterwards click on generate and then on open project to open the project in
Visual Studio. Put the project in the ``Release`` configuration and build it
(by building ``BUILD_ALL``).

When the project is done building, open ``build`` in a command prompt. Then
execute ``cpack -G NSIS`` to create the installer and ``cpack -G ZIP`` to create
the zip file.

Mac OS-X installer for 10.5+
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Configure cmake::

  $ cmake . -DCMAKE_OSX_DEPLOYMENT_TARGET={SDK version (e.g. '10.9')} \
            -DMCRL2_PACKAGE_RELEASE=ON

Build the toolset::

  $ make

Create the DMG-installer::

  $ cpack -G DragNDrop

Checking the installers
^^^^^^^^^^^^^^^^^^^^^^^

Double check the installers that have been built, as well as a build from the
source tarball succeeds! After everything succeeds and has been uploaded update
the version in the master branch as well. This ensures that the next time the
website is updated the new release can be downloaded.
