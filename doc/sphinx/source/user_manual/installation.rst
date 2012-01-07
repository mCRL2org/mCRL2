Installing mCRL2
======================

The mCRL2 toolkit can be run on a number of different platforms. For some 
platforms, precompiled binaries are available, for others, only a source 
installation is offered. The table below shows which versions are available. Of
course it is possible to install from source on platforms for which a binary
distribution is available.

  +------------------------+--------+-----------+
  | Operating system       | 64-bit | 32-bit    |
  +========================+========+===========+
  | Windows                |        | |bin_w32| |
  +------------------------+--------+-----------+
  | Mac OS X (Darwin)      |      |bin_osx|     |
  +------------------------+--------+-----------+
  | Ubuntu 10.4 Lucid Lynx |      |bin_ppa|     |
  +------------------------+--------------------+
  | Fedora 12              |                    |
  +------------------------+      |src_tgz|     |
  | OpenSUSE 11.3          |                    |
  +------------------------+--------------------+

Installing from source
------------------------------

The mCRL2 source code can be obtained in two ways. The source code of the latest
release version of the toolset is available as a |src_tgz|. The development 
version can be checked out from our `Subversion <http://subversion.tigris.org>`_ 
repository::

  svn co https://svn.win.tue.nl/repos/MCRL2/trunk mCRL2

In the remainder of this manual, we assume that you have the source code on
your system in a folder called ``mcrl2/src``. We will set up an out-of-source
build in ``mcrl2/build``, with a staging directory in ``mcrl2/stage``.

.. toctree::

   prerequisites
   configuration
   compilation
   test_pack

