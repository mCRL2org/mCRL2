Regression testing
==================

The quality of the mCRL2 toolset is continuously monitored through
regression tests, which are run automatically in a Jenkins setup.
The results of the regression tests can be found at `<http://mcrl2build1.win.tue.nl:8080>`_.

Currently there are three builds that are of importance:
  - *mCRL2* is a continuous build that is run on all supported platforms,
    in Release and Maintainer mode after every SVN commit to ``trunk``.
  - *mCRL2-nightly* is a build that is run every night and builds ``trunk``
    on all supported platforms in Release mode. The resulting builds are packaged, and
    a snapshot of the website including the packages is uploaded to `<http://dev.mcrl2.org>`_.
  - *mCRL2-release* is a continuous build that is run after every SVN commit
    to ``branches/release``. The build is run in Release and Maintainer
    mode on all supported platforms, and an extended set of tests is
    run (by setting the flag ``MCRL2_ENABLE_RELEASE_TEST_TARGETS=ON``.
     
Setup
-----

The Jenkins build system runs on 4 different machines:
  - ``mcrl2build1`` (Ubuntu Linux 12.4 LTS)
  - ``mcrl2build2`` (Ubuntu Linux 12.4 LTS)
  - ``mcrl2build3`` (Mac OS X, Mac Mini)
  - ``mcrl2build4`` (Ubuntu Linux 12.4 LTS)
  
On ``mcrl2build3``, we only run MacOSX and the corresponding builds.
On each of the other machines, a number of virtual machines is run
using KVM. Details on the configuration follow. A nice overview of how
to configure virtual machines on Ubuntu using KVM can be found 
`here <http://www.howtogeek.com/117635/how-to-install-kvm-and-create-virtual-machines-on-ubuntu/>`_.
Some notes on managing LVM in Ubuntu are described
`here <http://www.howtogeek.com/howto/40702/how-to-manage-and-use-lvm-logical-volume-management-in-ubuntu/>`_.

The main system running the Jenkins server is ``mcrl2build1``; on the
other systems a slave instance of Jenkins is run. We describe the
setup of ``jenkins-slave`` on every platform.

Ubuntu 12.04 LTS
^^^^^^^^^^^^^^^^
First install the following packages:
  - Subversion
  - Git
  - Build tools
  - Clang
  - Boost developer tools
  - Java JDK
  - QT
  - CMake
  - Doxygen
  - Graphviz
  - Python
  - Sphinx
  - TexLive
  - XsltProc

The command to install these packages is the following::

  sudo apt-get install subversion build-essential clang \
    libboost1.48-all-dev java7-jdk libqt4-dev cmake python-software-properties \
    doxygen git graphviz poppler-utils python python-sphinx swig \
    texlive-math-extra texlive-science texlive-latex3 xsltproc

Furthermore, to test with newer GCC versions (4.7 at the time of writing)
add the PPA repository and install the compiler::

  sudo add-apt-repository ppa:ubuntu-toolchain-r/test
  sudo apt-get update
  
Also, you will need to install the python version of DParser manually
using the following sequence of commands::

  cd /usr/local/src
  sudo git clone git://dparser.git.sourceforge.net/gitroot/dparser/dparser
  sudo chown `whoami`:users dparser
  cd dparser
  make PREFIX=/usr/local
  sudo make PREFIX=/usr/local install
  cd python
  sudo make PREFIX=/usr/local
  sudo make PREFIX=/usr/local install
  
Jenkins-slave can now be installed by copying two simple scripts,
:download:`jenkins-slave <jenkins/ubuntu/jenkins-slave>`, which is an
init-script, and :download:`start-jenkins-slave <jenkins/ubuntu/start-jenkins-slave>`,
which does the actual work. The first one needs to be in the directory
``/etc/init.d``, the second in ``/usr/local/bin``.
  
Fedora 14
^^^^^^^^^
First install the following packages:
  - Java JDK
  - CMake
  - GCC
  - QT
  - Boost
  - Doxygen
  
This can, e.g., be achieved using::

  yum install java cmake gcc gcc-c++ qt qt-devel boost-devel doxygen
  
Next create a directory where the builds are stored, and assign it
to the jenkins user::

  sudo mkdir /scratch
  sudo chown jenkins /scratch
  
Install the script
:download:`start-jenkins-slave <jenkins/fedora/start-jenkins-slave>` to
``/usr/local/bin``.

.. warning::

   Currently this script needs to be started manually on boot. A proper
   solution is desirable.

Windows 7
^^^^^^^^^
First install all of the following:
  - Windows SDK 7.1
  - If 64-bit Windows SDK: patch from `KB2280741 <http://support.microsoft.com/kb/2280741>`_
  - Boost, see instructions in the `user manual <../user_manual/build_instructions/boost.html>`_.
  - QT, see instructions in the `user manual <../user_manual/build_instructions/prerequisites.html#qt>`_.
  - SlikSVN (version 1.6.17)
  - CMake
  - Java
  - Python
  - `Wget for Windows <http://gnuwin32.sourceforge.net/packages/wget.htm>`_
  - Notepad++ (optional but convenient)
  - `NSIS <http://nsis.sourceforge.net/Download>`_
  
To get a working Jenkins slave, perform the following steps:
  #. create a ``Jenkins-slave`` directory, e.g. ``E:\jenkins-slave``, and
  #. store the file :download:`start-jenkins-slave.bat <jenkins/ubuntu/start-jenkins-slave.bat>` in this directory.
  #. Add this script to the task scheduler:
  
     a. Start -> type "task scheduler";
     b. select "Task Scheduler Library",
     c. right click -> "Create basic task", with:
     
        - Name "Jenkins slave",
        - Description "Starts a Jenkins slave".
        - Trigger "When the computer starts",
        - Action "Start a program", with script `E:\jenkins-slave\start-jenkins-slave.bat`.
        - Arguments and start-in can be left blank.
  
.. note::
  
   Make sure the paths in the script ``start-jenkins-slave.bat`` are
   correct for the given installation.

For proper packaging, the redistributable DLLs for the MSVC runtime
need to be found. Since the Windows SDK and MSVC 2010 express do not
properly install redistributable DLLs we need to do some work to get
this working. Perform the following steps:
  #. Locate the files ``msvc*100*.dll`` (typically ``C:\windows\system32``)
  #. Create a folder ``C:\Program Files\Common Files\VC\redist\${ARCHITECTURE}\Microsoft.VC100.CRT``
  #. Copy the dlls to this folder.
  
Activating Windows
""""""""""""""""""
To activate the Windows installations, the Windows machine needs to be
added to the TU/e domain. Given that all Windows installations are
currently on a virtual machine, some work is required. We need to make
sure that the virtual machines are added to the TU/e network using
bridging. On the host machine, perform the following steps.

Install the ``bridge-utils`` package::

  sudo apt-get install bridge-utils
 
To set up a bridge interface, edit ``/etc/network/interfaces`` and
replace the existing config with (replace with the values for your network)::

  auto lo
  iface lo inet loopback

  auto eth0
  iface eth0 inet manual

  auto br0
  iface br0 inet dhcp
        bridge_ports eth0
	    bridge_stp off
        bridge_fd 0
        bridge_maxwait 0
        
This will create a virtual interface ``br0``.

Now restart networking::

  sudo /etc/init.d/networking restart

If your VM host "freezes" for a few seconds after starting or stopping
a KVM guest when using bridged networking, it is because a Linux bridge
will take the hardware address of the lowest numbered interface out of
all of the connected interface. To work around this, add the following
to your bridge configuration::

  post-up ip link set br0 address f4:6d:04:08:f1:5f

and replace ``f4:6d:04:08:f1:5f`` with the hardware address of a
physical ethernet adapter which will always be part of the bridge.

Once the bridge has been configured, the configuration of the virtual
machine running Windows can be changed such that it uses bridging via
``br0``. Now follow the standard procedure with BCF to get the hostname
recognised in the TU/e network, and get the Windows machines integrated
in the domain for Windows activation. Once the machine has been joined
to the domain, activation is automatic and immediate.

Mac OS X
^^^^^^^^
First install the following packages through the App Store:
  - XCode
  - Java runtime

Next install the following through XCode -> Preferences -> Downloads:
  - XCode command line tools
  
Also, install `MacPorts <http://www.macports.org>`_.

The following then must be installed through macports:
- Boost
- QT
- CMake
- Wget
This can be done using the following command::

  sudo port install boost qt4-mac cmake wget
  
Finally, create a directory in which Jenkins is run::

  sudo mkdir /scratch
  sudo chown -R jenkins /scratch
  
Also save the script :download:`start-jenkins-slave <jenkins/macosx/start-jenkins-slave>`
to ``/opt/local/bin``.

.. warning::

  Currently, this script needs to be started manually upon boot.

.. note::

  This should probably be made into a `launchd <https://developer.apple.com/library/mac/documentation/Darwin/Reference/ManPages/man8/launchd.8.html>`_
  solution to start the Jenkins slave automatically at boot.
