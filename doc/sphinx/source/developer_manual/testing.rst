================================
Regression testing with TeamCity
================================

The quality of the mCRL2 toolset is continuously monitored through
regression tests, which are run automatically in a TeamCity setup.
The results of the regression tests can be found at `<http://mcrl2build1.win.tue.nl:8111>`_.
(You will need an account to access it)

In TeamCity there are two top-level projects that are of importance:

- *Maintainer* is a continuous build that is run on all supported platforms
  in Maintainer mode after every SVN commit to ``trunk``.
- *Nightly* is a build that is run every night and builds ``trunk``
  on all supported platforms in Release mode. The resulting builds are packaged.
  Nightly also builds the website including the documentation from the Sphinx sources.
  When all builds pass, the website, documentation and nightly packages are uploaded to `<http://mcrl2.org>`_.

Setup
=====

The TeamCity build system runs on 4 different machines:

- ``mcrl2build1`` (Ubuntu Linux)
- ``mcrl2build2`` (Ubuntu Linux)
- ``mcrl2build3`` (Mac OS X, Mac Mini)
- ``mcrl2build4`` (Ubuntu Linux)
  
On ``mcrl2build3``, we only run MacOSX and the corresponding builds.
On each of the other machines, a number of virtual machines is run
using KVM. A nice overview of how to configure virtual machines on
Ubuntu using KVM can be found
on http://www.howtogeek.com/117635/how-to-install-kvm-and-create-virtual-machines-on-ubuntu/.
Some notes on managing LVM in Ubuntu are described
here (http://www.howtogeek.com/howto/40702/how-to-manage-and-use-lvm-logical-volume-management-in-ubuntu/).

The main system running the TeamCity server is ``mcrl2build1``.
On all systems and virtual machines a TeamCity agent is run.

==============================
TeamCity project configuration
==============================

In TeamCity, build configurations are contained in projects. Projects can contain other projects in a tree-like structure.
Projects and build configurations inherit configuration from their parent projects.

A single project can contain multiple build configurations. Build configurations can be based on templates.

For the mCRL2 toolset, the TeamCity projects are structured as follows:

- **Root project** -
  TeamCity's root project. Has no configuration related to mCRL2.

- **mCRL2 project** -
  This project contains the default configuration for all other projects.
  This project also contains all build configuration templates used in the child projects.

- **Build type level** -
  These projects contain the configuration specific to the build types *Maintainer* or *Nightly*.
  On this level, environment variables are configured that configure the correct build types in the CMake files.
  There is also a *Release* project on this level but this is not used right now.

- **Build tool level** -
  This level contains the configuration for certain build tools, eg. gcc, clang and Visual Studio.

- **Build platform level** -
  This level contains the configuration for the different platforms, eg. Ubuntu, Fedora and Windows.

The lowest project levels contain one or more build configurations each based on a template.
The following templates are currently in use for building mCRL2 in the projects:

- **Build & Test Unix Fast** -
  Triggers a build on changes in the mCRL2 repository. Each build consists of 4 steps:

  - Configure -- runs a CMake configure step
  - Build -- runs a CMake build step
  - Test -- executes the tests
  - CPack -- runs a CPack step

  The steps will be explained later.

- **Build & Test Unix Delayed** -
  Triggers a build on changes in the mCRL2 repository. The build is delayed for 90 minutes to collect
  multiple changes to the repository. The steps are the same as the Build & Test Unix Fast template.

- **Build & Test Windows Fast** -
  Triggers a build on changes in the mCRL2 repository. Has the same 4 steps as the Unix builds and one additional
  step (before the configure step) to setup the Visual Studio build environment.

Build steps
===========

Configure
---------

The *configure* build step is a Meta-runner_ called ``mcrl2_Conf`` with the following script:

.. code:: xml

  <?xml version="1.0" encoding="UTF-8"?>
  <meta-runner name="Configure">
    <description>Configure mCRL2</description>
    <settings>
      <build-runners>
        <runner name="CMake Configure" type="jetbrains-cmake-conf">
          <parameters>
            <param name="teamcity.build.workingDir" value="build" />
            <param name="teamcity.step.mode" value="default" />
            <param name="ui-jetbrains-cmake-conf-additional-cmd-params"><![CDATA[-DCMAKE_BUILD_TYPE=%cfg_buildtype%
  %cfg_mcrl2_options%
  %cfg_compiler%]]></param>
            <param name="ui-jetbrains-cmake-conf-developer-warnings" value="%cfg_developer_warnings%" />
            <param name="ui-jetbrains-cmake-conf-makefile-generator" value="%cfg_generator%" />
            <param name="ui-jetbrains-cmake-conf-redirect-stderr" value="true" />
            <param name="ui-jetbrains-cmake-conf-source-path" value="../src" />
            <param name="ui-jetbrains-cmake-conf-warn-unused-vars" value="false" />
          </parameters>
        </runner>
      </build-runners>
      <requirements />
    </settings>
  </meta-runner>

This meta-runner executes the *CMake Configure* build runner from the `TeamCity CMake plugin`_ with additional parameters:

- ``cfg_buildtype`` - CMake build type. Either ``Release`` or ``Maintainer``. Configured on the **Build type level** projects.
- ``cfg_mcrl2_options`` - Additional mCRL2 build options. For example ``-DMCRL2_ENABLE_EXPERIMENTAL=ON -DMCRL2_ENABLE_DEPRECATED=ON -DMCRL2_SKIP_LONG_TESTS=ON``. Configured on various levels.
- ``cfg_compiler`` - The compiler to use. For example ``-DCMAKE_C_COMPILER=/usr/bin/gcc-6 -DCMAKE_CXX_COMPILER=/usr/bin/g++-6``. Configured on the **Build tool level** projects.
- ``cfg_developer_warnings`` - Enable or disable developer warnings. Either ``true`` or ``false``. Configured on the **Build type level** projects.
- ``cfg_generator`` - Always empty


Build
-----

The *build* build step is a Meta-runner_ called ``mcrl2_BuildTools`` with the following script:

.. code:: xml

  <?xml version="1.0" encoding="UTF-8"?>
  <meta-runner name="BuildTools">
    <description>Build mCRL2 tools</description>
    <settings>
      <build-runners>
        <runner name="CMake build" type="jetbrains-cmake-build">
          <parameters>
            <param name="teamcity.build.workingDir" value="build" />
            <param name="teamcity.step.mode" value="default" />
            <param name="ui-jetbrains-cmake-build-native-tool-params" value="-j%build_jobs%" />
            <param name="ui-jetbrains-cmake-build-redirect-stderr" value="true" />
          </parameters>
        </runner>
      </build-runners>
      <requirements />
    </settings>
  </meta-runner>

This meta-runner executes the *CMake Builder* build runner from the `TeamCity CMake plugin`_ with one additional parameter:

- ``build_jobs`` - Number of make jobs (``-j`` argument of make). This parameter is configured in the .ini file of
  build agents. It is usually configured as one more than the number of CPUs available to the agent.

Test
----

The *test* build step is a Meta-runner_ called ``mcrl2_Test`` with the following script:

.. code:: xml

  <?xml version="1.0" encoding="UTF-8"?>
  <meta-runner name="Test">
    <description>Test mCRL2</description>
    <settings>
      <build-runners>
        <runner name="Test" type="simpleRunner">
          <parameters>
            <param name="command.executable" value="ctest" />
            <param name="command.parameters" value="-T Test --output-on-failure --no-compress-output -j%build_jobs% %test_mcrl2_headertest%" />
            <param name="teamcity.build.workingDir" value="build" />
            <param name="teamcity.step.mode" value="default" />
          </parameters>
        </runner>
      </build-runners>
      <requirements />
    </settings>
  </meta-runner>

This meta-runner executes a *simpleRunner* build running built-in into TeamCity executing the ``ctest`` command.
It has two additional parameters:

- ``build_jobs`` - Number of make jobs (``-j`` argument of make). This parameter is configured in the .ini file of
  build agents. It is usually configured as one more than the number of CPUs available to the agent.
- ``test_mcrl2_headertest`` - An additional option used by some projects. This is either empty or contains the
  string ``-LE headertest``.  Configured on the **Build type level** projects.

CPack
-----

The *cpack* build step is a command line runner that executes a command line with one configurable parameter.

The executed command is ``cpack``. The command is executed with the arguments ``-G %pack_type%``. This has one
configurable parameter:

- ``pack_type`` - A string that defines the generator to use. See the CPack documentation for the possible
  values.  This is configured on the **Build platform level**.

Setup VS Env
------------

This special build step is only required for Windows builds. It is used to set up the required environment
variables for Visual Studio. This step executes a windows shell script using the `Command line`_ runner

.. code:: PowerShell

  REM - execute script to update environment
  IF EXIST "C:\Program Files (x86)\%visual_studio_version%\VC\vcvarsall.bat" (
    CALL "C:\Program Files (x86)\%visual_studio_version%\VC\vcvarsall.bat" x86_amd64
  ) ELSE (
    CALL "C:\Program Files\%visual_studio_version%\VC\vcvarsall.bat" x86
  )

  REM - make TeamCity update build environment
  %env.TEAMCITY_CAPTURE_ENV%

The special instruction ``%env.TEAMCITY_CAPTURE_ENV%`` instructs TeamCity to capture the current environment
and use this for the following build steps.

This script calls the ``vcvarsall.bat`` script supplied by Visual Studio. It contains one configurable parameter:

- ``visual_studio_version`` - A string that contains the installation directory, including the version number,
  of the Visual Studio installation to use. For example, ``Microsoft Visual Studio 12.0``.
  Configured on the **Build tool level** projects for Windows platforms.

.. _Meta-runner: https://confluence.jetbrains.com/display/TCD10/Working+with+Meta-Runner
.. _Command line: https://confluence.jetbrains.com/display/TCD10/Command+Line
.. _TeamCity CMake plugin: https://confluence.jetbrains.com/display/TW/CMake+Plugin
