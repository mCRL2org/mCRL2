Compilation
===========

.. include:: toggle-platform.inc

After :doc:`configuring <configuration>` CMake, build files for your build 
system can be generated and used to build the binaries.

.. admonition:: Windows
   :class: platform-specific win-only

   To compile using ``nmake``, execute the following in the Visual Studio/
   Windows SDK command prompt::

     cd <mcrl2>/build 
     cmake <mcrl2/src> -G "NMake Makefiles" 
     nmake Makefile all 

Installation
============

The toolset may be installed by executing making the ``install`` target::

  make install

In Windows, use::

  nmake Makefile install

Note that you may need administrative rights to install into the default 
location. You can install into a different location by configuring CMake
appropriately.

|enable_toggle|
