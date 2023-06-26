.. _build-macos:

MacOS instructions
====================

Below we list the various dependencies required to build the mCRL2 toolset in
its basic configuration. For testing and building the documentation we need
additional dependencies that are listed at the end. We can use `Homebrew
<https://brew.sh/>_` to acquire the necessary dependencies easily or use the
provided websites to acquire them individually.

  * *XCode* must be installed. It can be obtained from Apple's developer website. Afterwards we have to execute `xcode-select --install` to install the required command line tools.
  * Boost library sources, which can be downloaded from `<https://www.boost.org/>`_ and extracted in any desired location.
  * Qt version 5.15.0. Download and install Qt from `<https://www.qt.io/download-qt-installer>`, which requires a Qt account.
  * `CMake 3.14.0` or higher from <http://www.cmake.org/cmake/resources/software.html>`. 

.. note::

    Qt version 5.12.0 does not work properly and results in non functioning graphical
    tools. Qt version 5.12.1 appears to work fine. Qt 5.13.0 appears to support dark mode on MacOs Mojave
    properly, whereas earlier versions of Qt do not properly adapt the color schemes
    leading to unreadable (white on white) text in some tools.


A user-friendly command-line interface is provided by the ``ccmake``
executable. Example usage is as follows (from the directory
``mCRL2-build``)::

  ccmake ../mCRL2

From the interface you are presented with, you can choose the options
explained on the :ref:`instructions <instructions>`. Then prcessing `c`
will configuration the project, and pressing `g` will generate the Makefile.
For compilation using multiple cores, use the ``-j`` flag; *e.g.*, to compile
using 4 cores, use::

  make -j4

.. note::

  For every tool, an individual make target is defined. To compile only
  ``mcrl22lps``, for instance, use::

    make mcrl22lps