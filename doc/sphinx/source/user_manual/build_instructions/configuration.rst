.. _build-configuration:

Configuration
=============

To configure CMake to build in ``<mcrl2>/build``, and assuming that
the sources are in ``<mcrl2>/src``, make sure ``<mcrl2>/build``
exists before continuing with the following steps. There are three ways to
configure CMake:

*Graphically*
  A graphical user interface is available on all platforms, either as
  ``CMakeSetup.exe`` in Windows (should be in the start menu), or as 
  ``cmake-gui`` in Linux (you may have to install it separately, it is, e.g., currently
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
  ------------------ ---------------------------------------------------------
  ``Maintainer``     Same as ``Debug``, except that code coverage tests are
                     generated (and executed when running tests).
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

``SVNVERSION``
  *Default*: ``/PATH/WITH/FILE/svnversion``

  This variable specifies the location where ``svnversion`` can be found. 
  ``svnversion`` is a program for summarizing the revision mixture of a working
  copy. The program is used to generate the version information that the tools
  report.
