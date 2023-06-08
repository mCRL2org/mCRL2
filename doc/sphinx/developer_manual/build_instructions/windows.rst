.. _build-compilation:

Compilation
===========

.. include:: ../../_includes/toggle-platform.inc

After :doc:`configuring <configuration>` CMake, build files for your build
system can be generated and used to build the binaries.

.. admonition:: Windows
   :class: platform-specific win

   To compile the toolset, open the generated ``mCRL2.sln`` located in the
   build directory with Visual Studio (or click *open project* in cmake-gui)
   and choose *build solution*.

   .. note::

      Windows may have difficulty finding the Qt dll's, even when Qt is added
      to PATH, which prevents the GUI tools from running. To repair this for the
      release build, copy the following files from
      ``<path_to_Qt_dir>\<Qt_version>\msvc2017_64\bin`` to
      ``<path_to_build_dir>\stage\bin\Release``::

        Qt5Core.dll
        Qt5Gui.dll
        Qt5OpenGL.dll
        Qt5Widgets.dll
        Qt5Xml.dll

      In case of the debug build do the same, except with the dll file names
      appended with a `d`.


.. admonition:: Mac OS X & Linux
   :class: platform-specific mac linux

   The toolset can be compiled using the following command line::

     cd <mcrl2>/build
     cmake .
     make

   For compilation using multiple cores, use the ``-j`` flag; *e.g.*, to compile
   using 4 cores, use::

     make -j4

.. note::

   For every tool, an individual make target is defined. To compile only
   ``mcrl22lps``, for instance, use::

     make mcrl22lps

   Substitute ``nmake Makefile`` for ``make`` when using the Microsoft compiler.
   If you are developing a tool, and have made only changes to the tool code,
   and not to any of the libraries, consider using::

     make mcrl22lps/fast

   This disables dependency checking, speeding up compilation dramatically.

.. _build-installation:

Installation
============

The toolset may be installed by executing making the ``install`` target::

  make install

Substitute ``nmake Makefile`` for ``make`` when using the Microsoft compiler.

Note that you may need administrative rights to install into the default
location. You can install into a different location by configuring CMake
appropriately.

Flags
=====

The command ``ccmake .`` executed in the build directory allows to set compilation
and installation flags.

.. admonition:: Mac OS X
   :class: platform-specific mac

   The Apple clang compiler versions 7.2 and 8.0 have a problem compiling the toolset
   in DEBUG mode. This leads to problems with asserts. A solution is to add the
   flag ``-fsanitize=address`` to the compiler flags in CMAKE_CXX_FLAGS_DEBUG,
   CMAKE_C_FLAGS_DEBUG and CMAKE_EXE_LINKER_FLAGS_DEBUG. The problem does
   not occur when no -DNDEBUG is used. Also other compilers or platform do not appear
   to have this issue.
