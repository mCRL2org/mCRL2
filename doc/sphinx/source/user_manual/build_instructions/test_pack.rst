.. _build-testing:

Testing
=======

Before executing tests, make sure that your build has been configured
with the value of ``MCRL2_ENABLE_TEST_TARGETS`` set to ``YES`` and that
the toolset has been compiled.

To conduct the tests specified in the build tree execute::

  ctest

Specific tests can be run by using ``ctest``. Below some examples are given. 

* To displays the list of tests, without running them, execute::

    ctest -N .

* To execute the 2nd up until the 5th test targets, execute::

    ctest -I2,5 .

* To execute tests targets that match the description "FOO", execute::

    ctest -R FOO .

For additional help, run::

  ctest --help

.. _build-packaging:

Packaging
=========

To build platform specific packages execute the following command::

  cpack

To create packages for a specific distribution set the ``CPACK_SYSTEM_NAME``
variable to specific distribution by specifying the name, version and
architecture.

When packages are created for OS-X and Unix, we strongly advise to use the
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

  Then configure CMake with::

    cmake . -DCPACK_SYSTEM_NAME="`script.sh`"
