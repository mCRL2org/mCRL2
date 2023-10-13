.. _build-linux:

Ubuntu instructions
====================

Below we list the various dependencies required to build the mCRL2 toolset in
its basic configuration. For testing and building the documentation we need
additional dependencies that are listed at the end. For linux its most
convenient to use the system package manager to acquire the necessary
dependencies.

    * GCC or Clang compiler, which can be obtained from the ``build-essential`` package.
    * Qt version 5.12. Can be installed using the ``qtbase5-dev`` package on Ubuntu.
    * Boost library sources, which can be acquired from the ``libboost-dev`` package.
    * `CMake 3.14.0` or higher from <http://www.cmake.org/cmake/resources/software.html>`. 

A user-friendly command-line interface is provided by the ``ccmake``
executable. Example usage is as follows (from the directory
``mCRL2-build``)::

  ccmake ../mCRL2

From the interface you are presented with, you can choose the options
explained on the :ref:`instructions <build-instructions>`. Then prcessing `c`
will configuration the project, and pressing `g` will generate the Makefile.
For compilation using multiple cores, use the ``-j`` flag; *e.g.*, to compile
using 4 cores, use::

  make -j4

.. note::

  For every tool, an individual make target is defined. To compile only
  ``mcrl22lps``, for instance, use::

    make mcrl22lps