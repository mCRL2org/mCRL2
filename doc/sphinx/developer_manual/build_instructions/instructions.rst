.. _build-instructions:

Build instructions
==================

The mCRL2 source code can be obtained in two ways. The source code for releases
of the toolset are available as a source tarball, see :ref:`download`. The source
code for the development version of the toolset is available in our `Git <https://git-scm.com/>`_
repository hosted at `Github <https://github.com/mCRL2org/mCRL2>`_. It can be
obtained using:

  git clone https://github.com/mCRL2org/mCRL2.git

In the remainder of this manual, we assume that you have the source code on your
system in a folder called ``mcrl2/src``. We will set up an out-of-source build
in ``mcrl2/build``. To acquire the various dependencies we will list the
official websites. However, they can also be acquired from the system package manager
or alternatives like `brew <https://brew.sh/>` for MacOS or `Conda
<https://docs.conda.io>` for Windows provided that they have sufficient versions.

To compile the mCRL2 toolset we require any of the following compilers:

    GCC: 7.0
    Clang: 5.0
    AppleClang: 11.0
    MSVC: 2019 v16.0


Qt
--

The mCRL2 toolset requires Qt for compilation of the graphical tools
(:ref:`tool-diagraphica`, :ref:`tool-ltsgraph`, :ref:`tool-ltsview`,
:ref:`tool-lpsxsim`, :ref:`tool-mcrl2-gui`, :ref:`tool-mcrl2xi`).
Qt version 5.12.0 does not work properly and results in non functioning graphical
tools. Qt version 5.12.1 appears to work fine. Qt 5.13.0 appears to support dark mode on MacOs Mojave
properly, whereas earlier versions of Qt do not properly adapt the color schemes
leading to unreadable (white on white) text in some tools.

Windows
--------

For Windows the minimum Qt version supported is 5.12.
Download and install Qt from https://www.qt.io/download-qt-installer.

MacOS
------

For MacOS the minimum Qt version supported is 5.12.
However, due to MacOS updates changing the APIs used by applications,
a newer version of MacOS may require a newer version of Qt for some
features to work properly.
Qt can be installed using MacPorts by doing the following:

- First go to http://www.macports.org/.
- In the left menu bar select "Available Downloads" and download the
  appropriate version.
- Install the downloaded image.
- After installing open a terminal and execute the following to test and
  update MacPort::

    sudo port selfupdate

  Note that ``port`` is usually installed in ``/opt/local/bin``.
- To install Qt, execute::

    sudo port install qt5-mac

Linux
-----

For Linux the minimum Qt version supported is 5.12.
Binary development versions are available in the package manager in most
distributions (for instance the ``qtbase5-dev`` package in Ubuntu).

On Linux it is also required to install OpenGL related development packages.
The exact package to be installed depends on your distribution. For Ubuntu
these are e.g. ``libgl1-mesa-dev`` and ``libglu1-mesa-dev``.


Configuration
=============

First of all, we need to install `CMake 3.14.0`
<http://www.cmake.org/cmake/resources/software.html>`. To configure CMake to
build in ``<mcrl2>/build``, make sure ``<mcrl2>/build`` exists before continuing
with the following steps. There are three ways to configure CMake:

*Graphically*
  A graphical user interface ``cmake-gui`` is available on all platforms 
  (in Linux you may have to install it separately, it is, e.g., currently
  in the ``cmake-qt-gui`` package in the PPA for Ubuntu installations).
  Use of this user interface is mostly self-explanatory.

*CCMake*
  A user-friendly command-line interface is provided by the ``ccmake``
  executable. Example usage is as follows (from the directory
  ``<mcrl2>/build``)::

    ccmake <mcrl2>/src

  From the interface you are presented with, you can choose the options
  explained below.

*Command-line*
  CMake can also be run directly from the command line. This is recommended only
  for experienced users who need to configure the build from a script. To set
  a configuration variable, use the ``-D<FLAG=option>`` command-line option.

  Example usage is as follows (from the directory
  ``<mcrl2>/build``)::

    cmake <mcrl2>/src -DFLAG=option

  To install mCRL2 to <installdir>, with all default settings, you can
  issue the command::

    cmake <mcrl2>/src -DCMAKE_INSTALL_PREFIX=<installdir>
	
All three methods allow you to change the value of CMake configuration
variables. Below, we describe the meaning of these variables. Note that in the
graphical user interfaces, some advanced settings are initially hidden.

.. note::

   For a typical installation of mCRL2, the default settings are
   acceptable for all the options listed below. The only common change
   is in the path to which mCRL2 must be installed, which can be set
   using ``CMAKE_INSTALL_PREFIX``. If you are not interested in the
   meaning of detailed configuration settings, you can safely skip to
   :ref:`build-compilation`.

.. note::

   Values of the flags are cached and are case insensitive. This implies that
   when changing a single value, all other flags remain the same.

``BOOST_ROOT``
  *Default*:: Automatically detected

  .. index:: BOOST_ROOT

  Specifies the path to the *Boost* libraries. This is normally automatically
  detected, but may have to be set manually when *Boost* was compiled manually,
  or when detection fails.

``CMAKE_BUILD_TYPE``
  *Default*: ``Release``

  .. index:: CMAKE_BUILD_TYPE

  This variable controls the type of build when using a single-configuration
  generator like the Makefile generator. This variable has no effect for Visual
  Studio projects as they contain multiple-configurations.

  ================== =========================================================
  ``Release``        Compile with optimisations enabled. Debug information is
                     not generated.
  ------------------ ---------------------------------------------------------
  ``Debug``          Compile with extra checks, less optimisation and with
                     generation of debug symbols.
  ------------------ ---------------------------------------------------------
  ``RelwithDebInfo`` Same as ``Release``, except that debug information is
                     also generated.

                     .. warning::

                        Historically, some extra checks were enabled in this
                        mode. There may therefore still be differences in
                        behaviour, compared to the ``Release`` setting.
  ------------------ ---------------------------------------------------------
  ``MinSizeRel``     Same as ``Release``, but instructs the compiler to
                     optimise for size.
  ================== =========================================================

``CMAKE_INSTALL_PREFIX``
  *Default*: ``/usr/local/``

  .. index:: CMAKE_INSTALL_PREFIX

  The installation prefix for mCRL2. All mCRL2 files will be installed there.

``BUILD_SHARED_LIBS``
  *Default*: ``ON``

  .. index:: BUILD_SHARED_LIBS

  ======= ======================================================================
  ``ON``  Generate shared libraries, to which the executables link.
  ------- ----------------------------------------------------------------------
  ``OFF`` Generate static libraries, which are linked into the executables. This
          increases the size of executables dramatically.
  ======= ======================================================================

``MCRL2_ENABLE_EXPERIMENTAL``
  *Default*: ``OFF``

  .. index:: MCRL2_ENABLE_EXPERIMENTAL

  ======= ======================================================================
  ``ON``  Compile experimental tools.
  ------- ----------------------------------------------------------------------
  ``OFF`` Do not compile experimental tools.
  ======= ======================================================================

``MCRL2_ENABLE_DEPRECATED``
  *Default*: ``OFF``

  ======= ======================================================================
  ``ON``  Compile deprecated tools.
  ------- ----------------------------------------------------------------------
  ``OFF`` Do not compile deprecated tools.
  ======= ======================================================================

``MCRL2_ENABLE_PROFILING``
  *Default*: ``OFF``

  ======= ======================================================================
  ``ON``  Enable profiling when executing tools.
  ------- ----------------------------------------------------------------------
  ``OFF`` Disable profiling when executing tools.
  ======= ======================================================================

``MCRL2_ENABLE_TEST_TARGETS``
  *Default*: ``OFF``

  ======= ======================================================================
  ``ON``  Generate test targets. This option needs to be enabled in combination
          with ``BUILD_TESTING`` to execute library tests. If not enabled, only
          tool tests are executed when ``BUILD_TESTING`` is enabled.
  ------- ----------------------------------------------------------------------
  ``OFF`` Disable profiling when executing tools.
  ======= ======================================================================

``MCRL2_MAN_PAGES``
  *Default*: ``ON``

  ======= ======================================================================
  ``ON``  Enable generation of manual pages.
  ------- ----------------------------------------------------------------------
  ``OFF`` Disable generation of manual pages.
  ======= ======================================================================

``MCRL2_ENABLE_GUI_TOOLS``
  *Default*: ``ON``

  ======= ======================================================================
  ``ON``  Compile graphical tools (:ref:`tool-diagraphica`,
          :ref:`tool-ltsgraph`, :ref:`tool-ltsview`, :ref:`tool-mcrl2-gui`,
          :ref:`tool-lpsxsim`, :ref:`tool-mcrl2ide`).
  ------- ----------------------------------------------------------------------
  ``OFF`` Do not compile graphical tools.
  ======= ======================================================================

``BUILD_TESTING``
  *Default*: ``ON``

  ======= ======================================================================
  ``ON``  Execute tests after successful build. The tests that are exeuted are
          determined by the ``MCRL2_ENABLE_TEST_TARGETS`` configuration
          variable.
  ------- ----------------------------------------------------------------------
  ``OFF`` Do not execute tests.
  ======= ======================================================================

``CTAGS``
  *Default*: ``/PATH/WITH/FILE/ctags``

  This variable specifies the location where Ctags can be found. Ctags is a
  program that generates an index (or tag) file of names found in source and
  header files of various programming languages.

``Qt5_DIR``
  This variable specifies the location where Qt can be found.

  .. admonition:: Windows
     :class: platform-specific win

     It should be set to ``<path_to_Qt_dir>\<Qt_version>\msvc2017_64\lib\cmake\Qt5``.

  .. admonition:: Mac OS X
     :class: platform-specific mac

     On Mac, this variable should have the value
     ``<Qt_dir>/qtbase/lib/cmake/Qt5``, ``<Qt_dir>`` is the path to the directory
     where Qt was installed.

  .. admonition:: Linux
     :class: platform-specific linux

     On Linux, the right path is generally automatically detected.
