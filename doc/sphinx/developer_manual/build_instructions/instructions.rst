.. _build-instructions:

   
Build instructions
==================
   
.. toctree::
  :hidden:

  windows
  macos
  linux

The mCRL2 source code can be obtained in two ways. The source code for releases
of the toolset are available as a source tarball, see :ref:`download`. The source
code for the development version of the toolset is available in our `Git <https://git-scm.com/>`_
repository hosted at `Github <https://github.com/mCRL2org/mCRL2>`_. It can be
obtained using the following command::

  $ git clone https://github.com/mCRL2org/mCRL2.git

In the remainder of this manual, we assume that you have the source code on your
system in a folder called ``mCRL2``. We will set up an in-source build in
``build``, this is especially useful for vscode plugins that rely on the
``compile_commands.json``.

.. note::

  The mCRL2 toolset requires Qt and OpenGL for compilation of the graphical tools
  (:ref:`tool-diagraphica`, :ref:`tool-ltsgraph`, :ref:`tool-ltsview`,
  :ref:`tool-lpsxsim`, :ref:`tool-mcrl2-gui`, :ref:`tool-mcrl2xi`). If this is
  undesirable then building the graphical tools can be disabled using the
  `MCRL2_ENABLE_GUI_TOOLS` cmake flag.

See the platform specific instructions for :doc:`Windows <windows>`, :doc:`MacOS <macos>` and :doc:`Linux <linux>`. For testing see the documentation :ref:`here <build-testing>`.

Python dependencies
--------------------

If you don't want to install Python libraries system wide, you can set up a
Python virtual environment (needs to be done only once) in the build directory::

    $ python3 -m venv sphinx-venv

Activate it::

    $ source sphinx-venv/bin/activate

Afterwards dependencies can be installed using `pip` as follows::

    $ pip install -r <mCRL2>/requirements.txt

When using a virtual environment ensure that CMake uses the right Python binary::

    $ cmake [other options] \
          -DPython_EXECUTABLE=absolute/path/to/sphinx-venv/bin/python \
          <mCRL2-build>

Documentation
---------------

To build the documentation we need various dependencies depending on what exact
configuration is being used. First of all the cmake flag
``MCRL2_ENABLE_DOCUMENTATION`` must be enabled to build the documentation.
Then the `doc` target can be build for a full documentation build, or `fastdoc`
to avoid cleaning up the intermediate results. The documentation build can be
fine tuned using the following cmake configuration flags.

  - ``MCRL2_ENABLE_DOC_DOXYGEN`` enables building the source code documentation
    using doxygen, this requires `Doxygen <https://www.doxygen.nl/>`_ to be
    installed with at least version 1.9.7.
  - ``MCRL2_ENABLE_DOC_PDFLATEX`` enables building the pdfs that are found in
    the library documentation, this required pdflatex to be installed with
    various packages.
  - ``MCRL2_ENABLE_DOC_MANUAL`` enables building the tool documentation pages,
    which requires the tools to be compiled.

.. _configuration:

CMake configuration flags
--------------------------
  
All three methods allow you to change the value of CMake configuration
variables. Below, we describe the meaning of these variables. Note that in the
graphical user interfaces, some advanced settings are initially hidden.

.. note::

   For a typical installation of mCRL2, the default settings are
   acceptable for all the options listed below. The only common change
   is in the path to which mCRL2 must be installed, which can be set
   using ``CMAKE_INSTALL_PREFIX``.

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