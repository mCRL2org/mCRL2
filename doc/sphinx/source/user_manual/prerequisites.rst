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

.. |cmake| replace:: *CMake 2.8*
.. _cmake: http://www.cmake.org/cmake/resources/software.html

.. |doxygen| replace:: *Doxygen 1.7.4*
.. _doxygen: http://www.doxygen.org

- |doxygen|_ or higher.
- |python|_ or higher.
- The following python packages, that may be installed using `PIP 
  <http://www.pip-installer.org>`_ :

  - sphinx

The compiler suites available vary per platform. We recommend the following 
setup:

.. admonition:: Windows
   :class: platform-specific win-only

   - The following components of *Windows SDK 7.0 SP1*:

     - Compilers
     - Headers
     - Win32 development tools

     Alternatively, one can install *Microsoft Visual C++ Express 2010* or a 
     more recent or complete version of Visual Studio. 
   - |cmake|_ or higher.

   We will assume that there is a command prompt available that performs the 
   necessary configuration to make the ``nmake`` executable available in the 
   path. This command prompt can be found as a batch file in 
   ``C:\Program Files (x86)\Microsoft Visual Studio 9.0\VC\bin`` for Windows
   SDK 7.0 SP1. In the start menu, look for either of the following:
    
    - 32bit: Visual Studio 2008 Command Prompt
    - 64bit: Visual Studio 2008 x64 Win64 Command Prompt

.. admonition:: Mac OS X
   :class: platform-specific mac-only

   - *XCode* must be installed. It can be found on the OS X installation DVDs.
     Consult the following table for the required version.

       ============ =================
       OS X version XCode version
       ============ =================
       10.4         2.5
       ------------ -----------------
       10.5         3.1
       ------------ -----------------
       10.6         3.2.5
       ============ =================

   - |cmake|_ or higher.
     
.. admonition:: Linux
   :class: platform-specific linux-only

   - *g++* version 4.0.0 or higher.
   - *make* version 3.80 or higher.
   - |cmake|_ or higher.

   We recommend installing these by installing the ``build-essential`` PPA
   package (if available for your distribution).

Boost
-----

.. |boost| replace:: *Boost 1.36*
.. _boost: http://www.boost.org

The mCRL2 sources use libraries from the |boost|_ collection (or a more recent
version). The following libraries are required:

- *filesystem*
- *serialization*
- *signals*
- *regex*
- *system*

.. admonition:: Windows
   :class: platform-specific win-only

   .. warning:: 
 
      Only a 32-bit version is freely available at the time of writing. For
      64-bit versions, :doc:`compile the Boost libraries yourself <boost>`.

   To install *Boost*, follow the steps below.

   - Download *BoostPro* from http://www.boostpro.com/download/
   - Install file after downloading.
   - Install all static runtime libraries (Debug/Release, Multi/Single thread)
     for the compiler you are using.
   - Select the required *Boost* packages.   

.. _osx-boost:

.. admonition:: Mac OS X
   :class: platform-specific mac-only

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
   :class: platform-specific linux-only

   The easiest way to install the required libraries is to use the package 
   manager in your distribution. For instance, the libraries can be installed
   by installing the ``libboost-*-dev`` PPA packages.

   You can also :doc:`compile the required Boost libraries yourself <boost>`.


wxWidgets
---------

.. |wx| replace:: *wxWidgets 2.8.11*
.. _wx: http://www.wxwidgets.org

The mCRL2 toolset requires |wx|_ or higher for compilation of the graphical 
tools (:ref:`tool-diagraphica`, :ref:`tool-ltsgraph`, :ref:`tool-ltsview`,
:ref:`tool-lpsxsim`, :ref:`tool-mcrl2-gui`). 

.. warning::

   It is important that you have matching wxWidgets libraries for your build
   setup. Linking debug versions of the wxWidgets libraries to the mCRL2 
   binaries or vice versa may give unexpected results.

.. warning::

   To be able to use all graphical tools, wxWidgets must be configured to
   support OpenGL.

.. admonition:: Windows
   :class: platform-specific win-only

   It is recommended to install wxWidgets using the following steps:

   - Download wxWidgets to ``<WXROOT>``
   - Edit ``<WXROOT/include/wx/msw/setup.h>`` such that ``wxUSE_GLCANVAS`` is 
     defined to be 1.
   - In the Visual Studio/Windows SDK command prompt, build *wxWidgets* as 
     follows::
     
       cd <WXROOT>/build/msw 
       nmake BUILD="release" USE_OPENGL=1 makefile.vc 
       nmake BUILD="debug" USE_OPENGL=1 makefile.vc 

   Alternatively, the last step can be replaced by the following:

   - Open ``/build/msw/wx.sln`` in Visual Studio. If it asks to convert 
     projects, confirm for all files in the project.
   - Compile all libraries for a certain build type (e.g. Release, Debug).
   - To build for a x64 platform, perform the following steps: 

     - Select the solution in the Solution Explorer and open the Properties page
       (Right click, then select :guilabel:`Properties`). 
     - Select :guilabel:`Configuration Properties` and click on 
       :guilabel:`Configuration Manager`. 
     - Change the :guilabel:`Active solution configuration` to "Release". 
     - Click on the :guilabel:`Active solution plaform`, and select "New" in the
       dropdown box. 
     - Select "x64" as the new platform, and copy settings from "Win32".

   When installed correctly, CMake should be able to automatically find your 
   installation. If automatic detection fails, configure CMake appropriately by
   by setting the ``wxWidgets_LIB_DIR`` and ``wxWidgets_ROOT_DIR`` to 
   ``<wxdir>/lib/vc_lib`` and ``<wxdir>``, respectively. This can be done by
   executing::

     cmake -DwxWidgets_LIB_DIR="<wxdir>/lib/vc_lib" -DwxWidgets_ROOT_DIR="<wxdir>"

   Optionally, you can compile mCRL2 with wxWidgets Styled Text Control (wxSTC).
   This provides features within :ref:`tool-mcrl2xi` as syntax highlighting and
   bracket matching. To enable these features, perform the following steps:

   - After having wxWidgets compiled, goto "<wxdir>/contrib/build/stc"
   - Open ``stc.dsw`` with MSVC.
   - For Windows X64 set "Active Solution Platform" to X64 
     (:menuselection:`Build -> Configuration Manager`)
   - Compile desired configurations (Debug & Release are sufficient). These 
     options can be found under :menuselection:`Build -> Batch Build`.
   - Press :guilabel:`build` to compile.
   - Close MSVC and copy ``<wxdir>/contrib/include/wx/stc`` to 
     ``<wxdir>/include/wx/stc``
   - Now mCRL2 can be compiled with wxSTC. To enable this option, the CMake 
     configuration flag ``MCRL2_WITH_WXSTC`` should be set to ``TRUE``.

.. admonition:: Mac OS X
   :class: platform-specific mac-only

   wxWidgets can be installed using MacPorts::

     sudo port install wxWidgets

   See the :ref:`notes <osx-boost>` on installing *Boost* for information on how
   to install MacPorts. Installing wxWidgets may take a while.

.. admonition:: Linux
   :class: platform-specific linux-only

   Binary development versions are available in the package manager in most 
   distributions (for instance the ``libwxgtk2.8-dev`` PPA package).

   To build from source, download the sources and build as usual, using the
   following configuration::

     ./configure  --disable-shared --disable-compat24 --disable-mediactrl --disable-sound --with-opengl 
     
   On linux it is also required to install OpenGL related development packages.
   The exact package to be installed depends on your distribution. For Ubuntu
   this are e.g. ``libgl1-mesa-dev`` and ``libglu1-mesa-dev``.

|enable_toggle|
