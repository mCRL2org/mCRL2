.. _compiling_boost:

Compiling Boost
===============

.. include:: toggle-platform.inc

Compiling *Boost* yourself is not recommended, unless you have ample experience
with software development, or if you simply have no other option than to compile
it yourself.

To compile *Boost* manually, download the sources to a directory that we will
call ``<BOOSTROOT>``.

.. admonition:: Windows
   :class: platform-specific win
   
   - In the command prompt, build *Boost* as follows (to install to C:\Boost) ::

       cd <BOOSTROOT> 
       bootstrap.bat 
       b2 address-model=64 architecture=x86 --with-filesystem --with-serialization --with-signals --with-regex --with-test
       b2 install address-model=64 architecture=x86 --with-filesystem --with-serialization --with-signals --with-regex --with-test

.. admonition:: Mac OS X
   :class: platform-specific mac

   - Ensure that the shell scripts we need are executable::

       chmod +x ./bootstrap.sh ./tools/build/v2/engine/build.sh

   - Build *Boost*::

      cd <BOOSTROOT>
      ./bootstrap.sh
      ./bjam <PARAMS> --build-type=complete --layout=tagged --with-filesystem --with-serialization --with-signals --with-regex 

     For OS-X 10.6, substitute ``<PARAMS>`` with ``toolset=darwin address-model=32 architecture=x86``,
     for OS-X 10.5, use ``toolset=darwin address-model=32 architecture=x86 macosx-version=10.5 macosx-version-min=10.5``.
   
.. admonition:: Linux
   :class: platform-specific linux

   - Ensure that the shell scripts we need are executable::

       chmod +x ./bootstrap.sh ./tools/build/v2/engine/build.sh

   - Build *Boost*::

       cd <BOOSTROOT>
       ./bootstrap.sh
       ./bjam --build-type=complete --layout=tagged --with-filesystem --with-serialization --with-signals --with-regex 
   
After the build, Boost reports information on the build. The compiler path
specified here, must be used to set the CMake variable :envvar:`BOOST_ROOT`. 

