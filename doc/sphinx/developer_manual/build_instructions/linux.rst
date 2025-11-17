.. _build-linux:

Ubuntu instructions
====================

Below we list the various dependencies required to build the mCRL2 toolset in
its basic configuration. For testing and building the documentation we need
additional dependencies that are listed at the end. For linux its most
convenient to use the system package manager to acquire the necessary
dependencies.

  * GCC or Clang compiler, which can be obtained from the ``build-essential`` package.
  * Qt version 6.2.4. Can be installed using the ``qt6-base-dev`` package on Ubuntu.
  * Boost library sources, which can be acquired from the ``libboost-dev`` package.
  * `CMake 3.14.0` or higher from <http://www.cmake.org/cmake/resources/software.html>`. 
  * `Python 3.6 <https://www.python.org/>` is optionally needed for various activities such as testing, building documentation and code generation.

A user-friendly command-line interface is provided by the ``ccmake``
executable. Example usage is as follows (from the directory
``build``)::

  ccmake ../mCRL2

From the interface you are presented with, you can choose the options
explained on the :ref:`instructions <build-instructions>`. Then pressing `c`
will configuration the project, and pressing `g` will generate the Makefile.
For compilation using multiple cores, use the ``-j`` flag; *e.g.*, to compile
using 4 cores, use::

  make -j4

.. note::

  For every tool, an individual make target is defined. To compile only
  ``mcrl22lps``, for instance, use::

    make mcrl22lps

.. note::

  Many vscode extensions rely on the ``compile_commands.json``, which can be
  produced by setting the ``CMAKE_EXPORT_COMPILE_COMMANDS`` to true in ``ccmake``.
