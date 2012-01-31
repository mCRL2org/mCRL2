Cronjob information
===================
This page contains information about the cronjobs that are executed
periodically to support the development of the mCRL2 toolset. We
describe how these cronjobs are set up and how they can be configured.
The information on this page is particularly interesting for people
who want to maintain, extend or otherwise change the collection of
cronjobs that are run.

The current host computer for the cronjobs is ``pclin164.win.tue.nl``. It
is expected that they can be migrated to another system without much effort.
Please read this document carefully before doing so.

Overview
--------
In essence, the system consists of a collection of Bash scripts that
gets executed periodically by the host's cron daemon. This collection
is structured into one main script and a number of job scipts.

Main script
^^^^^^^^^^^
The purpose of the main script is to decide which jobs have to be
executed and in what order, and executes these jobs in a proper shell
environment. More precisely, its tasks are:

* To set up any environment variables that are relevant to the job scripts.
* To update the local SVN working copy of the mCRL2 source tree.
* To decide which job scripts have to be run.
* To execute the job scripts in the appropriate order.
* To log any messages that the job scripts may produce, and upload these logs if
  necessary.
* To send e-mail notifications in case certain jobs fail.

The main script gets called by the cron daemon at certain times as
indicated by the installed crontab. We refer to the main script itself for more
detailed information on its workings. See section Directory structure for its
location on the host computer.

Job scripts
^^^^^^^^^^^
Every job script performs a certain, specific task related to the mCRL2
toolset. Examples include building the toolset, generating and uploading
library documentation, and executing performance tests on the toolset.

A job script gets called by the main script. It may access environment
variables or call Bash functions that have been exported by the main
script. Of course, it may also define and use its own, local variables
and functions.

When a job fails, the job script should signal this to the main script
by a terminating with a non-zero exit status. Otherwise its exit status
should be zero. Different non-zero exit statuses may be used to signal
different kinds of failures to the main script. Of course, the main
script should be made aware of these differences, so that it can take
appropriate actions.

A job script should output relevant informative messages to stderr or
stdout, so that the main script can redirect these to the appropriate
log files. A job script is not supposed to write its own log files,
unless this is absolutely necessary.

Directory structure
-------------------
On the current host computer ``pclin164.win.tue.nl``, all files and directories
needed to run the cronjobs are located in ``/scratch/mcrl2-cron2``

The most important subdirectories are the following:

* ``install``     : installation of the mCRL2 toolset;
* ``logs``        : logs from the various job scripts;
* ``performance`` : scripts and collected data of the performance tests;
* ``scripts``     : the cronjob scripts (see section "Overview")
* ``svn``         : working copy of the mCRL2 SVN source tree;
* ``svn_oas``     : working copy of the mcrl2 directory in the OAS repository.

We describe these in more detail below.

``install``
^^^^^^^^^^^
The mCRL2 installation is used by the main script to determine the
currently installed version of the toolset and by the performance job
script for collecting the measurements.

``logs``
^^^^^^^^
The ``logs`` directory is organized in the following way. For every job
script, a subdirectory exists with a corresponding name. Whenever that
job script is run, a log file is placed in that subdirectory by the main
script. This log file will have the following name::

  [revision]_[date]_[time].log
  
where:

* ``[revision]`` is the SVN revision number associated with this run of the main script (see below);
* ``[date]`` is the date on which the main script was called by the cron daemon;
* ``[time]`` is the time on which the main script was called by the cron daemon.

The ``[revision]`` is the SVN revision number of the local working copy in
the ``svn`` directory after the main script has attempted to update it to
the latest SVN revision (a.k.a. the HEAD revision). So, if that update
failed, ``[revision]`` is the revision number of the working copy prior to
the attempt to update it by the main script.

Please note that the log messages of updating the SVN working copy by
the main script are written to the log file of the ``build.sh`` job script.

``performance``
^^^^^^^^^^^^^^^
The most important files and directories in the ``performance`` directory
are the following:

* ``build_data``: collected measurement data from building the toolset
* ``cases``: input files for the performance tests (being symbolic links to
  files in the local mCRL2 installation)
* ``data``: collected measurement data from the performance tests
* ``make_plots.sh``: script for generating tool performance plots from the data
  in ``data``
* ``make_build_plots.sh``: script for generating build performance plots from
  the data in ``build_data``
* ``performance.py``: the main performance test script

We do not describe the performance tests in more detail here.

``scripts``
^^^^^^^^^^^
The ``scripts`` directory contains the following files:

* ``backup.sh``: job script for backing up precious cronjob files
* ``build_plots.sh``: job script for generating build performance plots (calls
  ``make_build_plots.sh`` in ``performance``)
* ``build.sh``: job script for building the toolset using Bjam 
* ``cbuild.sh``: job script for building the toolset using CMake 
* ``libdoc.sh``: job script for updating online library documentation
* ``main.sh``: the main script
* ``performance.sh``: job script for performance tests (calls ``performance.py``
  and ``make_plots.sh`` in ``performance``)
* ``source.sh``: job script for updating the downloadable source package on the
  website (calls the script for generating this package in the ``packaging``
  subdirectory of the SVN working copy)

These scripts should mostly speak for themselves.

``svn``
^^^^^^^
The working copy of the entire mCRL2 SVN repository. It is used by
the job scripts for various purposes and updated by the main script
before running any of the job scripts.

``svn_oas``
^^^^^^^^^^^
This working copy of the ``mcrl2`` directory of the OAS repository is
updated and used by the cbuild job script. It contains the configuration
files that are necessary for building the toolset using CMake.

File system permissions
-----------------------
The directory for the cronjob files and all of its contents are shared among the
members of the ``mcrl2`` Unix group. Directories have the group sticky bit set,
which ensures that any file/directory that is created within that directory will
automatically inherit the ``mcrl2`` group ownership.

Any file or directory should have both read **and** write permissions for group,
to ensure that any member of the ``mcrl2`` group has full access to that file or
directory. Please note that the system does **not** give group write
permissions to newly created files and directories by default. This can be dealt
with in the following ways:

#. If you only have to create a small number of files/directories, simply set
   the proper permissions after creation::
   
     chmod 775 [files]
     
#. If you have to create a large number of files/directories (or when some
   external program will be creating those on your behalf), it's probably more
   convenient to set your ``umask`` beforehand::
   
     umask 002
     
   Do not forget to set your ``umask`` back to its original value when you are
   done, to prevent any new personal files that you create from getting group
   write permissions.

If you want your user account to be added to the ``mcrl2`` group, please contact
the BCF Unix helpdesk.

Setting up a cronjob
--------------------
To set up a cronjob on the host machine, login to that machine and run the
following command::

  crontab -e
  
Now, add a line to the list of cronjob entries, calling the main script. For
example::

  0 5,17 * * * /scratch/mcrl2-cron2/scripts/main.sh &> /scratch/mcrl2-cron2/logs/latest.log
  
This will run the main script every day at 5am and 5pm, and redirect any output
messages that are not captured by the main script to the indicated log file.

Note that the script will be run under the user account that executed the
``crontab`` command and will inherit that account's privileges.

Dependencies
------------

System requirements
^^^^^^^^^^^^^^^^^^^

In order for the cronjobs to work correctly, recent versions of the following
software has to be installed on the host computer.

  =================================  =================================================================
  Name	                             Commands
  =================================  =================================================================
  Cron daemon	
  Bash shell and builtins	           cd, echo, export, pwd, ulimit, umask, etc.
  Core Linux tools	                 awk, bc, date, find, grep, ps, sed, tar, time, which, xargs, etc.
  SSH secure shell client	           ssh, scp
  Mailx client	                     mail
  SVN client	                       svn, svnversion
  Tools required for building mCRL2	 make, gcc, etc.
  CMake	                             cmake
  Rsync client	                     rsync
  Gnuplot	                           gnuplot
  Python	                           python
  Doxygen	                           doxygen
  LaTeX	                             pdflatex, bibtex
  =================================  =================================================================

These should ideally be installed in locations that are listed in the ``PATH``
environment variable by default (e.g. ``/bin`` and ``/usr/bin``). Otherwise, the
``PATH`` variable should be extended in the main script to include the proper
locations.

User requirements
^^^^^^^^^^^^^^^^^
The user account under which the cronjobs are executed, must meet the following
requirements:

* The account must be a member of the ``mcrl2`` Unix group. This is because the
  scripts need to have read- and write-access to the directory tree. To become a
  member of that group, please contact the BCF Unix helpdesk.
* The account must have a directory called ``backups`` in its home directory.
  The ``backup.sh`` job script will place an archive containing precious cronjob
  files in that directory. This archive is overwritten every time the
  ``backup.sh`` script is run. To achieve this, simply run::
  
    mkdir ~/backups
  
* The account must be able to login to the faculty's webserver as user ``mcrl2``
  via SSH, ''without the need to supply a password''. This is because the
  scripts need to be able to upload files to the webserver. Secure, passwordless
  login via SSH can be set up as follows, using RSA key authentication (DSA is
  very similar):
  #. If your account has no SSH key pair yet, run the following command::
  
       ssh-keygen -t rsa
        
     Save the key to the default location (``$HOME/.ssh/id_rsa``) and do
     **not** enter a passphrase (simply hit enter when prompted, twice).
  #. Upload the **public** key of the account's SSH key pair to the webserver::
  
       scp ~/.ssh/id_rsa.pub mcrl2@www.win.tue.nl:~/
            
     You will need to enter the password for the ``mcrl2`` web account here.
  #. Login to the webserver, append the uploaded key to the list of authorized
     keys and remove the key file::
     
       ssh mcrl2@www.win.tue.nl
       (enter password)
       cat id_rsa.pub >> .ssh/authorized_keys
       rm id_rsa.pub
       
  #. Log out and log back in using the same command as above. This time, SSH
     should not prompt you for the password and should log you in immediately.

