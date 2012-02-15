Configuration
=============

To configure CMake to build in ``<mcrl2>/build``, make sure this directory 
exists before continuing with the following steps. There are three ways to
configure CMake:

*Graphically*
  A graphical user interface is available on all platforms, either as
  ``CMakeSetup.exe`` in Windows (should be in the start menu), or as 
  ``cmake-gui`` in Linux (you may have to install it separately, it is currently
  in the ``cmake-qt-gui`` package in PPA).

*CCMake*
  A user-friendly command-line interface is provided by the ``ccmake`` 
  executable.

*Command-line*
  CMake can also be run directly from the command line. This is recommended only
  for experienced users who need to configure the build from a script. To set
  a configuration variable, use the ``-D<FLAG=option>`` command-line option.

All three methods allow you to change the value of CMake configuration 
variables. Below, we describe the meaning of these variables. Note that in the
graphical user interfaces, some advanced settings are initially hidden.

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
  ------------------ ---------------------------------------------------------
  ``Maintainer``     Same as ``Debug``, except that code coverage tests are
                     generated (and executed when running tests).
  ================== =========================================================

``CMAKE_INSTALL_PREFIX``
  *Default*: ``/usr/local/``

  .. index:: CMAKE_INSTALL_PREFIX

  The installation prefix for mCRL2. All mCRL2 files will be installed there.
  
  .. note::
  
     Write permissions are required for the path set in
     ``CMAKE_INSTALL_PREFIX``.

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

``MCRL2_MONO_LIB``
  *Default*: ``OFF``

  ======= ======================================================================
  ``ON``  Combine all libraries into one monolithic library. This excludes third
          party libraries.
  ------- ----------------------------------------------------------------------
  ``OFF`` Compile libraries into separate binaries.
  ======= ======================================================================

``MCRL2_ENABLE_GUI_TOOLS``
  *Default*: ``ON``

  ======= ======================================================================
  ``ON``  Compile graphical tools (:ref:`tool-diagraphica`,
          :ref:`tool-ltsgraph`, :ref:`tool-ltsview`, :ref:`tool-mcrl2-gui`,
          :ref:`tool-lpsxsim`).
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

``DART_TESTING_TIMEOUT``
  *Default*: ``1500``

  This variable controls the timeout for running tests.

``SVNVERSION``
  *Default*: ``/PATH/WITH/FILE/svnversion``

  This variable specifies the location where ``svnversion`` can be found. 
  ``svnversion`` is a program for summarizing the revision mixture of a working
  copy. The program is used to generate the version information that the tools
  report.

``wxWidgets_CONFIG_EXECUTABLE``
  Default: ``/PATH/WITH/FILE/wx-config``

  This variable specifies the full pathname to the wx-config utility in your
  wxWidgets installation (typically this is located in the ``bin/`` subdirectory
  of your wxWidgets root install. ``wx-config`` is a small command-line utility
  which can help you while building on Unix-like systems (including Linux and
  Mac OS X). ``wx-config`` will tell what compile flags to use (``wx-config
  --cppflags``), tell what link flags to use (``wx-config --libs``) and manage
  multiple wxWidgets installs with different configurations (``wx-config 
  --list`` et al).

``wxWidgets_USE_STATIC``
  Default: ``OFF``

  ======= ======================================================================
  ``ON``  Link wxWidgets libraries statically. 
  ------- ----------------------------------------------------------------------
  ``OFF`` Link wxWidgets libraries dynamically. 
  ======= ======================================================================

``wxWidgets_USE_UNICODE``
  Default: ``ON``

  ======= ======================================================================
  ``ON``  Enable if wxWidgets was compiled with Unicode support.
  ------- ----------------------------------------------------------------------
  ``OFF`` Enable if wxWidgets was compiled without Unicode support. 
  ======= ======================================================================

``wxWidgets_wxrc_EXECUTABLE``
  Default: ``/PATH/WITH/FILE/wxrc``

  This variable specifies the full pathname to the ``wxrc`` utility in your
  wxWidgets installation (typically this is located in the ``bin/`` subdirectory
  of your wxWidgets root install. The ``wxrc`` utility compiles binary XML
  resource files.
