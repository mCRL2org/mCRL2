Compiling Boost
===============

.. include:: toggle-platform.inc

.. admonition:: Windows
   :class: platform-specific win-only

  - Download Boost to ``<BOOSTROOT>``
  - Open the appropriate Visual Studio command prompt. This prompt can be found
    in ``C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin`` for Windows
    SDK 7.0 SP1.
    
    - 32bit: Visual Studio 2008 Command Prompt
    - 64bit: Visual Studio 2008 x64 Win64 Command Prompt

  - In the command prompt, build *Boost* as follows::

      cd <BOOSTROOT> 
      bootstrap.bat 
      bjam -j8 address-model=64 architecture=x86 --with-filesystem --with-serialization --with-signals stage 

  Boost should now be compiled and the library files should be in the <BOOSTROOT>/stage directory.

.. admonition:: Mac OS X
   :class: platform-specific linux-only

.. admonition:: Linux
   :class: platform-specific linux-only
   

|enable_toggle|

