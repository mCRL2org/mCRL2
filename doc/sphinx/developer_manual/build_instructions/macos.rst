.. _build-macos:

MacOS instructions
====================

Below we list the various dependencies required to build the mCRL2 toolset in
its basic configuration. For testing and building the documentation we need
additional dependencies that are listed at the end. We can use `Homebrew
<https://brew.sh/>`_ to acquire the necessary dependencies easily or use the
provided websites to acquire them individually.

  * *XCode* must be installed. It can be obtained from Apple's developer website. Afterwards we have to execute `xcode-select --install` to install the required command line tools.
  * Boost library sources, which can be downloaded from `<https://www.boost.org/>`_ and extracted in any desired location.
  * Qt version 6.2.4. Download and install `Qt <https://www.qt.io/download-qt-installer>`_, which requires a Qt account.
  * `CMake 3.14.0 <http://www.cmake.org/cmake/resources/software.html>`_ must be installed. 
  * `Python 3.6 <https://www.python.org/>` is optionally needed for various activities such as testing, building documentation and code generation.

.. note::

    If Qt cannot be found automatically, for example when it is installed using the official installer, then the cmake variable ``Qt6_DIR`` must be set to the install directory followed by ``/lib/cmake/Qt6``.

A user-friendly command-line interface is provided by the ``ccmake``
executable. Example usage is as follows (from the directory
``build``)::

  ccmake ../mCRL2

From the interface you are presented with, you can choose the options
explained on the :ref:`instructions <build-instructions>`. Then pressing `c`
will configuration the project, and pressing `g` will generate the `Makefile`.
For compilation using multiple cores, use the ``-j`` flag; *e.g.*, to compile
using 4 cores, use::

  make -j4

.. note::

  For every tool, an individual make target is defined. To compile only
  ``mcrl22lps``, for instance, use::

    make mcrl22lps

.. note::

  Many vscode extensions rely on the `compile_commands.json`, which can be
  produced by setting the `CMAKE_EXPORT_COMPILE_COMMANDS` to true in `ccmake`.

.. warning::

  On newer macOS versions it might be that the compiled GUI tools fail to
  launch. This is due to them being killed with an error related to running
  unsigned code. This issue can be resolved by manually signing the resulting
  `build/stage/mCRL2.app` using the following command::

    sudo codesign --force --deep -s - mCRL2.app