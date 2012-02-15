CMake F.A.Q.
============

`CMake <http://www.cmake.org/>`_ is a cross-platform, open-source make
system. CMake generates native makefiles and workspaces that are used in the
compiler environment of your choice. CMake supports preprocessor generation,
code generation, and template instantiation.

.. toctree::
   :maxdepth: 2

   self

Running CMake
-------------

I used to run the following commands for developing using bjam, what should I run now?
""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""""

A lot of people seem to be used to configuring as follows::

  ./configure --enable-experimental --enable-deprecated --enable-debug --enable-static --prefix=/path/to/mcrl2

and then running make, make install or make test.

Now you should run something like::

  cmake -DMCRL2_ENABLE_EXPERIMENTAL=TRUE -DMCRL2_ENABLE_DEPRECATED=TRUE -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=FALSE -DCMAKE_INSTALL_PREFIX=/path/to/mcrl2

This however does not allow you to run tests. If you also want to build the
tests, you should instead run::

  cmake -DMCRL2_ENABLE_EXPERIMENTAL=TRUE -DMCRL2_ENABLE_DEPRECATED=TRUE -DCMAKE_BUILD_TYPE=Debug -DBUILD_SHARED_LIBS=FALSE -DCMAKE_INSTALL_PREFIX=/path/to/mcrl2 -DMCRL2_ENABLE_TEST_TARGETS

Alternatively you can configure the options using the interactive tool
``ccmake``.

Now you can run make, make install or make test as you were used to.

How do I create a static/shared debug build?
""""""""""""""""""""""""""""""""""""""""""""

This can be done by setting the CMake variable ``BUILD_SHARED_LIBS`` (see
:doc:`configuration`).

How do I create a release build?
""""""""""""""""""""""""""""""""

This can be done by setting the CMake variable ``CMAKE_BUILD_TYPE`` (see
:doc:`configuration`).

What's the difference between the build types?
""""""""""""""""""""""""""""""""""""""""""""""

The tooslet is build according to a preset configuration. Depending on its
purpose, a configuration should be selected. See :doc:`configuration` for more
information on the available build types.

How do I run tests?
"""""""""""""""""""

When :doc:`configuring <configuration>` CMake, make sure to enable 
``BUILD_TESTING``, and optionally ``MCRL2_ENABLE_TEST_TARGETS``. See
:doc:`test_pack` for further instructions.

I want to develop using Eclipse, how can I do this?
"""""""""""""""""""""""""""""""""""""""""""""""""""

For some background information from the CMake developers (also giving hints for
our of source support in Eclipse, see
http://www.cmake.org/Wiki/Eclipse_CDT4_Generator)

Developing mCRL2 in Eclipse. (Based on Eclipse Galileo)

Make sure you have Ecplise installed before you start with these instructions.

# Launch Eclipse. It will ask you to create a workspace. This work space has
  nothing to do with a specific project you're working on. Enter a path, e.g. 
  ``$HOME/.eclipse-workspace`` and check "Use this as the default and do not ask 
  again".

  .. note::
  
     this workspace only stores meta data about projects and Eclipse.

# Install the C++ Development Tools (CDT) for Eclipse [1]. This can be done by 
  opening up :menuselection:`Help --> Install new software`. 

# Click :guilabel:`Add`. On the downloads page found at [1], select the update 
  site appropriate for your Eclipse installation. Copy the URL of the update
  site and paste the URL at Location. At name type "CDT". Click :guilabel:`OK`. 
  This has made CDT available.

  At :guilabel:`Work with` select CDT. 
  - CDT Main Features
  - CDT Optional Features
    - CDT GNU Toolchain Build Support
    - CDT GNU Toolchain Debug Support
    - CDT Utilities
    - Eclipse C/C++ Development Platform
    - Eclipse C/C++ DSF gdb Debugger Integration
    - Eclipse C/C++ Memory View Enhancements

  Now click :guilabel:`Next`, :guilabel:`Next`, :guilabel:`accept EULA`,
  :guilabel:`Finish`, and CDT will be installed. If asked to restart Eclipse, do
  so.

To work conveniently with Eclipse, it is advised to install SVN integration as
well.
 
Go the the :menuselection:`Install software` menu. Galileo at "Work with".
Expand "Collaboration" and check:
* Subversive SVN Team Provider; Click Next; Finish and restart Eclipse.
Now to really work with SVN, again go to Install Software menu, and add the update site found in the description at [3].
* Pick site just added, and expand Core SVNKit Library
* Click Next; Next; accept license; Finish
If you get a security warning that the software contains unsigned content, go ahead and click OK. Restart Eclipse

You now have a suitable Eclipse installation to develop mCRL2.

Create a fresh checkout of the mCRL2 toolset at a desired location (as it is required to build mCRL2 in-source).
 svn checkout https://svn.win.tue.nl/repos/MCRL2/trunk /path/to/working-copy
 cd /path/to/working-copy
 ccmake .
Set: MCRL2_BUILD_IN_SOURCE=ON, CMAKE_BUILD_TYPE=Debug (or Maintainer), CMAKE_INSTALL_PREFIX=/path/to/working-copy/Debug, MAN_PAGES=OFF, MCRL2_COMPILE_EXAMPLES=ON, MCRL2_ENABLE_TEST_TARGETS=ON
 Configure and generate
 cmake -G"Eclipse CDT4 - Unix Makefiles"

To import the project perfrom the following steps in Eclipse:
 File -> Import -> General -> Existing Projects into Workspace; next; Browse -> Select /path/to/working-copy; OK; Tick Add project to working sets; Select; New; name: mCRL2; Finish; tick mCRL2; OK; Finish;

You now have an mCRL2 tree in Eclipse, Eclipse will automatically start building...

To enable SVN functionality:
* Right click the mCRL2 project in the project explorer -> Team -> Share project
* If you get asked to select a connector, pick SVN Kit 1.3.0

[1] http://www.eclipse.org/cdt/
[2] http://www.eclipse.org/subversive/
[3] http://svnkit.com/download.php

==How do I set a cache variable?==

To control build settings on the CMake command line, cache variables need to be set with the -D option. The settings are saved in CMakeCache.txt. This file stores variables and corresponding values, such that CMake does not have to repeat a configure each time it runs on the build tree. For example, to set the cache variable FOO with the boolean value TRUE: 

  cmake -DFOO=TRUE


==How do I produce more 'verbose' compiling?==

First method is by setting CMAKE_VERBOSE_MAKEFILE to ON.

Another possibility is to set verbose variable in the generated files.
For Makefile generators, set the Makefile variable VERBOSE to 1. For example on UNIX:

  make VERBOSE=1

On Windows (nmake) override CMAKE_VERBOSE_MAKEFILE by using

  nmake /S


==How to skip checking dependent libraries when compiling==

This example applies for make files generated by the Makefile Generator.
For these files, append "/fast" to your target name. For example:

  make target_name/fast

In Windows use a backslash instead:

  make target_name\fast


==  How do I use a different compiler? ==
This can be done by setting the cache variables using cmake -D.
Set the appropriate CMAKE_FOO_COMPILER variable(s) to either a valid compiler name or to full path on the command-line using cmake -D. As an example, we set the C and CXX compiler:

  cmake -DCMAKE_C_COMPILER=gcc-4.3 -DCMAKE_CXX_COMPILER=g++-4.3

==  How do I generate a make file for an environment? ==

Use:

  cmake -G "Generator_name"

The list of generators available are found at the bottom lines when executing:

  cmake --help

The generators differ for each platform.

== The CMAKE_BUILD_TYPE setting does not seem to work on Windows, what to do? ==

There are three possible values for this variable: Debug, Release and Maintainer.

For Visual C++ and MinGW the Maintainer value does not work.

For Visual C++ the Debug value does not work either. A linker problem with ITERATOR_DEBUGGING will show up. 



== How do I install Boost on Windows ? ==

To install boost, one can use something like

  bjam --build-dir="d:\cpp\boost" --build-type=complete --with-filesystem --with-thread --with-signals --with-date_time --with-regex --with-serialization --with-test msvc

The string 'msvc' corresponds to the Visual C++ compiler. Use 'mgw' for MinGW.
Which of the boost libraries are required for building the mCRL2 toolset may vary over time.
An alternative is to use an installer from http://www.boostpro.com/download/

== How do I setup CMake for Windows/Visual Studio? ==

On the command line one can install CMake support for Visual Studio as follows:

  cmake <MCRL2_ROOT> -G "Visual Studio 10" -DBOOST_ROOT:PATH="d:\cpp\boost" -DBoost_COMPILER="-vc100"

This will generate the necessary Visual Studio projects in the current directory.
Optionally some additional flags can be set, like

  -DMCRL2_ENABLE_GUI_TOOLS=false
  -DMCRL2_ENABLE_TEST_TARGETS=true

By default the flag BUILD_SHARED_LIBS is set to false, this is the recommended setting on Windows.

== How do I setup CMake for Windows/MinGW? ==

A MinGW installation that appears to work fine is TCM-GCC, see http://tdm-gcc.tdragon.net/. This installs a very recent g++ compiler.

On the command line one can install CMake support for MinGW as follows:

  cmake <MCRL2_ROOT> -G "MinGW Makefiles" -DBOOST_ROOT:PATH="d:\cpp\boost" -DBoost_COMPILER="-mgw"

This will generate the necessary MinGW build files in the current directory.
Optionally some additional flags can be set, like

  -DBUILD_SHARED_LIBS=false
  -DMCRL2_ENABLE_GUI_TOOLS=false
  -DMCRL2_ENABLE_TEST_TARGETS=true

By default the flag BUILD_SHARED_LIBS is set to true, but it is recommended to set it to false for MinGW.

= Problems with wxWidgets =

== I got a fatal Error during compiling/ I got a mismatch between the program and library build versions detected. ==

I got the following error, which looks like:

  Fatal Error: Mismatch between the program and library build versions detected.  
  The library used 2.8 (debug,Unicode,compiler with C++ ABI 1002,wx containers,compatible with 2.6),
  and your program used 2.8 (no debug,Unicode,compiler with C++ ABI 1002,wx containers,compatible with 2.6).

This error is related to a mismatch between libraries. In this case it applies for wxWidgets.
CMake detects if a version of wxWidgets is installed. However, it can not detect if all suitable configurations (Debug, Non-Debug, Unicode, etc...) are available. This is the users responsibility. For mCRL2 toolset, wxWidgets requires to be in the same configuration. This means, that if a user want to build the toolset with Debug symbols, wxWidgets should match the configuration. To overcome this error, one should configure, compile and install wxWidgets. Let '''wxdir''' be the library where wxWidgets is installed, then configure Cmake to use that version by executing the following command:

  cmake -DwxWidgets_CONFIG_EXECUTABLE=/wxdir/bin/wx-config -DwxWidgets_wxrc_EXECUTABLE=/wxdir/bin/wxrc

==CMake on Windows complains that it can't find wxWidgets, but I have wxWidgets installed, what should I do?==

For this FAQ, we assume that wxWidgets is compiled in '''<wxdir>''', Visual studio is used for compilation.
As we do not export symbols, it is not possible to create a build with shared libraries. This only allows us to build static versions. 

mCRL2 requires a version of wxWidgets that is installed and compiled accordingly. 
Make sure that your are using the static configuration for compiling wxWidgets. 
To compile graphical tools that require OpenGL, edit the file '''<wxdir>\lib\vc_lib\mswd\wx\setup.h''' , by setting the following define to 1 instead of 0:

  #define wxUSE_GLCANVAS       1


When configuring CMake set the following variables '''wxWidgets_LIB_DIR''' and '''wxWidgets_ROOT_DIR'''
to "<wxdir>/lib/vc_lib" and "<wxdir>", respectively. This can be done by:

 cmake -DwxWidgets_LIB_DIR="<wxdir>/lib/vc_lib" -DwxWidgets_ROOT_DIR="<wxdir>"
