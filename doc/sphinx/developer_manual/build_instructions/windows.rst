.. _build-windows:

Windows instructions
====================

Below we list the various dependencies required to build the mCRL2 toolset in
its basic configuration. For testing and building the documentation we need
additional dependencies that are listed at the end.

  * Visual Studio 2022 with C++ development to obtain a compiler and IDE to build the toolset.
  * Boost library sources, which can be downloaded from `<https://www.boost.org/>`_ and extracted in any desired location. 
  * Qt version at least 5.12.0. Download and install `Qt <https://www.qt.io/download-qt-installer>`_, which requires a Qt account. Alternatively, install `Conda <https://docs.conda.io>`_ and install the qt5-base package.
  * `CMake 3.14.0 <http://www.cmake.org/cmake/resources/software.html>`_ must be installed. 
  

After installing the required dependencies we can open `cmake-gui` to perform
the configuration steps. First of all, select the ``mCRL2`` directory as the
source directory and ``mCRL2-build`` as the directory to build the binaries.
Then press `Generate` and select the installed Visual Studio version as
generator. When generation fails make sure to set `Boost_INCLUDE_DIR` to
<boost_dir>/include and `Qt5_DIR` to the place where Qt is installed followed by
`/lib/cmake/Qt5`.

To compile the toolset, open the generated ``mCRL2.sln`` located in the
build directory with Visual Studio (or click *Open Project* in cmake-gui)
and choose *Build Solution*.

.. note::

  When running graphical tools Windows may have difficulty finding the Qt DLLs,
  which prevents the GUI tools from running. To repair this we can use the
  `windeployqt` tool provided by Qt to install the required DLLs and platform
  plugins in the local directory by executing `windeployqt <ltsgraph.exe>` in the
  `stage/bin/<Configuration>` directory.