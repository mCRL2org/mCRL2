Testing
=======

.. note::

   The mCRL2 build environment is set up to work with *CDash*, an open source,
   web-based software testing server. *CDash* aggregates, analyzes and displays
   the results of software testing processes submitted from clients located
   around the world. A generic manual on how to install *CDash* can be found 
   `here <http://public.kitware.com/Wiki/CDash:Installation>`_.

Before executing tests, make sure that the makefile has been generated with the
value of ``MCRL2_ENABLE_TEST_TARGETS`` set to ``YES`` and the toolset has been 
compiled.

To conduct the tests specified in the source tree execute::

  ctest .

To upload the tests to the public dashboard execute one of the following 
commands::

  ctest -D Nightly 
  ctest -D Experimental 
  ctest -D Continuous 

A ``Nightly`` build builds and tests a build of the most recent SVN version at 
00:00:00 CEST that night. The ``Experimental`` build should be used to test new 
features. A ``Continuous`` build uses the most recent SVN version.

Specific tests can be run by using ``ctest``. Below some examples are given. 

* To displays the list of tests, without running them, execute::

    ctest -N .

* To execute the 2nd up until the 5th test targets, execute::

    ctest -I2,5 .

* To execute tests targets that match the description "FOO", execute::

    ctest -R FOO .

For additional help, run::

  ctest --help

.. note::

   If multiple Nightly builds are executed, the labels of the buildnames should
   be disjoint from the other buildname label, in order to avoid the override of
   earlier builds. To avoid this side-effect, execute cmake and set the
   ``BUILDNAME`` variable with an unique label::

     cmake . -DBUILDNAME=label 

Packaging
=========

To build platform specific packages execute the following command::

  cpack .

To create packages for a specific distribution set the ``CPACK_SYSTEM_NAME``
variable to specific distribution by specifying the name, version and
architecture.

When package are created for OS-X and Unix, we strongly advise to use the
default value for ``CMAKE_INSTALL_PREFIX``, or use a directory value that is
available in the target platform.

We advise to use the latest stable version of CMake and CPack to generate
packages for the various platforms. We conclude with some distribution-specific
notes on packaging.

*openSUSE*
  Configure Cmake by::

    cmake . -DCPACK_SYSTEM_NAME="`cat /etc/SuSE-release | head -n1 | sed "s/ /\_/g"`"

*Fedora*
  To configure packaging, create ``script.sh``::

    #!/bin/bash
    arch="_(`uname -i`)"
    fed=`cat /etc/fedora-release | sed 's/ /\_/g'`
    echo "$fed$arch"

  Then configure Cmake with::

    cmake . -DCPACK_SYSTEM_NAME="`script.sh`"
