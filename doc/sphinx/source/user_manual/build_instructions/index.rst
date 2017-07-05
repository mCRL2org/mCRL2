.. _build:

Build instructions
==================

The mCRL2 source code can be obtained in two ways. The source code of the latest
release version of the toolset is available as a source tarball, see :ref:`download`.
The development
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
   new_build_instructions

