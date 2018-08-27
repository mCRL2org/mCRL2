.. _build-prerequisites:

Prerequisites
=============

.. include:: toggle-platform.inc

.. toctree::
   :hidden:

   boost

To build the source code, the first requirement is to have a compiler, and,
optionally, the necessary tools to build the documentation.
To build the documentation, the following is needed:

.. |python| replace:: *Python 2.5.1*
.. _python: http://www.python.org

.. |cmake| replace:: *CMake 2.8.6*
.. _cmake: http://www.cmake.org/cmake/resources/software.html

.. |cmakeosx| replace:: *CMake 2.8.12*
.. _cmakeosx: http://www.cmake.org/cmake/resources/software.html

.. |doxygen| replace:: *Doxygen 1.7.4*
.. _doxygen: http://www.doxygen.org

- |doxygen|_ or higher.
- |python|_ or higher.
- The following python packages, that may be installed using `PIP
  <http://www.pip-installer.org>`_ :

  - sphinx (Linux: can also be installed with the package ``python-sphinx``)

The compiler suites available vary per platform. We recommend the following
setup:

.. admonition:: Windows
   :class: platform-specific win

   Visual Studio 2015 or higher with C++ development

   .. note::

      The tool ltsview fails to build when using Visual Studio 2017

.. admonition:: Mac OS X
   :class: platform-specific mac

   - *XCode* must be installed. It can be found on the OS X installation DVDs.
     Consult the following table for the required version.

     +-----------------+-----------------+
     | OS X version    |  XCode version  |
     +=================+=================+
     | 10.5            | 3.1             |
     +-----------------+-----------------+
     | 10.6            | 3.2.5           |
     +-----------------+-----------------+
     | 10.7            | 4.2.1           |
     +-----------------+-----------------+


   - |cmakeosx|_ or higher.

.. admonition:: Linux
   :class: platform-specific linux

   - *g++* version 4.9.0 or higher.
   - *make* version 3.80 or higher.
   - |cmake|_ or higher.

   We recommend installing these by using the package manager of your
   distribution. On, e.g., Ubuntu, these can be installed by installing
   the ``build-essential`` PPA package.

Boost
-----

.. |boost| replace:: *Boost 1.36*
.. _boost: http://www.boost.org

The mCRL2 sources use libraries from the |boost|_ collection (or a more recent
version). The build only depends on the Boost header files, and does not
link to any boost libraries.

.. admonition:: Windows
   :class: platform-specific win

   To install a binary version of *Boost*, download *Boost* from https://www.boost.org,
   unzip and then :doc:`compile the Boost libraries yourself <boost>`.

.. _osx-boost:

.. admonition:: Mac OS X
   :class: platform-specific mac

   .. warning::

      Only a 32-bit version seems to be available at the time of writing. For
      64-bit versions, :doc:`compile the Boost libraries yourself <boost>`.

   Boost can be installed using MacPorts by doing the following:

   - First go to http://www.macports.org/.
   - In the left menu bar select "Available Downloads" and download the
     appropriate version.
   - Install the downloaded image.
   - After installing open a terminal and execute the following to test and
     update MacPort::

       sudo port selfupdate

     Note that ``port`` is usually installed in ``/opt/local/bin``.
   - To install *Boost*, execute::

       sudo port install boost

.. admonition:: Linux
   :class: platform-specific linux

   The easiest way to install Boost using the package
   manager in your distribution. For instance, the libraries can be installed
   by installing the ``libboost-*-dev`` PPA packages when you are using
   Ubuntu.

   You can also :doc:`compile the Boost libraries yourself <boost>`.

.. _build_prerequisites_qt:

QT
--

.. |qt| replace:: *QT 5.5*
.. _qt: https://www.qt.io

The mCRL2 toolset requires |qt|_ or higher for compilation of the graphical tools
(:ref:`tool-diagraphica`, :ref:`tool-ltsgraph`, :ref:`tool-ltsview`,
:ref:`tool-lpsxsim`, :ref:`tool-mcrl2-gui`, :ref:`tool-mcrl2xi`).

.. admonition:: Windows
   :class: platform-specific win

   Download and install Qt from https://www.qt.io/download-qt-installer.

.. admonition:: Mac OS X
   :class: platform-specific mac

   QT can be installed using MacPorts by doing the following:

   - First go to http://www.macports.org/.
   - In the left menu bar select "Available Downloads" and download the
     appropriate version.
   - Install the downloaded image.
   - After installing open a terminal and execute the following to test and
     update MacPort::

       sudo port selfupdate

     Note that ``port`` is usually installed in ``/opt/local/bin``.
   - To install *QT*, execute::

       sudo port install qt5-mac

.. admonition:: Linux
   :class: platform-specific linux

   Binary development versions are available in the package manager in most
   distributions (for instance the ``libqt5-dev`` PPA package in Ubuntu).

   On Linux it is also required to install OpenGL related development packages.
   The exact package to be installed depends on your distribution. For Ubuntu
   these are e.g. ``libgl1-mesa-dev`` and ``libglu1-mesa-dev``.
