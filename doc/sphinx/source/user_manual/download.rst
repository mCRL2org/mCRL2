.. include:: toplevel.inc

.. The links to the release are hard coded in downloads-release.inc.
.. The nightly builds are linked in downloads-nightly.inc.

.. include:: downloads-release.inc
.. include:: downloads-nightly.inc
   
.. _download:

Download mCRL2
==============

The mCRL2 toolkit can be run on a number of different platforms. For some 
platforms, precompiled binaries are available. The table below shows which 
binaries are available. 

.. ifconfig:: release == version


   +------------------------+------------------------+
   |Operating system        |Release (|release|)     |
   +========================+========================+
   |Windows                 | |bin_w32|_             |
   |                        +------------------------+
   |                        | |bin_w64|_             |
   +------------------------+------------------------+
   |Mac OS X (Darwin)       | |bin_osx|_             |
   +------------------------+------------------------+
   |Ubuntu                  | |bin_ppa|_             |
   +------------------------+------------------------+
   |Source                  | |src_tgz|_             |
   +------------------------+------------------------+
  
.. ifconfig:: release != version

   +------------------------+-------------------------+
   |Operating system        |Nightly build (|version|)|
   +========================+=========================+
   |Windows                 | |bin_w32n|_             |
   |                        +-------------------------+
   |                        | |bin_w64n|_             |
   +------------------------+-------------------------+
   |Ubuntu                  | |bin_deb32n|_           |
   |                        +-------------------------+
   |                        | |bin_deb64n|_           |
   +------------------------+-------------------------+
   |Fedora                  | |bin_fedora64n|_        |
   +------------------------+-------------------------+
   |Source                  | |src_tgzn|_             |
   +------------------------+-------------------------+

If you are using a Linux distribution that is not listed, you may wish to try
:ref:`compiling mCRL2 yourself <build>`.

.. ifconfig:: release == version

   .. note::
   
      A nightly build can be found `here <http://www.mcrl2.org/dev>`_.
   
.. ifconfig:: release != version

   .. warning::
   
      If you are looking for a release version of mCRL2, check out the
      `homepage <http://www.mcrl2.org>`_!
   

Previous releases
-----------------

Earlier versions of mCRL2 are listed in the :ref:`historic_releases` section.
