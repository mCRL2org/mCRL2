.. include:: toplevel.inc

.. The links to the release are hard coded in this file. The nightly builds are
   linked in downloads.inc.
.. include:: downloads.inc
.. |bin_w32| replace:: Windows Installer
.. _bin_w32: http://www.win.tue.nl/mcrl2/download/release/mcrl2-201107-2-win32.exe

.. |bin_osx| replace:: Darwin package
.. _bin_osx: http://www.win.tue.nl/mcrl2/download/release/mcrl2-201107-2-Darwin.dmg

.. |bin_ppa| replace:: PPA release
.. _bin_ppa: https://launchpad.net/~mcrl2/+archive/release-ppa

.. |src_tgz| replace:: GZipped Tarball
.. _src_tgz: http://www.win.tue.nl/mcrl2/download/release/mcrl2-201107.1-src.tar.gz

.. _download:

Download mCRL2
==============

The mCRL2 toolkit can be run on a number of different platforms. For some 
platforms, precompiled binaries are available. The table below shows which 
binaries are available. 

  ======================== ======================== ============================
  Operating system         Release (|release|)      Nightly build (|version|)
  ======================== ======================== ============================
  Windows                  |bin_w32|_ (32-bit)      |bin_w32_n|_ (32-bit)
                                                    |bin_w64_n|_ (64-bit)
  ------------------------ ------------------------ ----------------------------
  Mac OS X (Darwin)        |bin_osx|_               |bin_osx_n|_     
  ------------------------ ------------------------ ----------------------------
  Ubuntu 10.4 Lucid Lynx   |bin_ppa|_             
  ======================== ======================== ============================

If you are using a Linux distribution that is not listed, you may wish to try
:doc:`compiling mCRL2 yourself <build>`.
