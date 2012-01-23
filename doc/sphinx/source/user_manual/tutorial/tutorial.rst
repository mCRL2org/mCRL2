mCRL2 tutorial
==============

In this tutorial we give a basic introduction into the basic use of
the mCRL2 toolset. In each of the sections we present a number of new
concepts, guided by an example, and some exercises to gain hands on
experience with the tools.  Note that in this tutorial we mainly focus
at the use of the tools, and not on the theory that is underlying the
tools. For the latter, we refer to [GMRUW09]_ as a brief
introduction of the main concepts, and to [GR11]_ for an in-depth
discussion.

Getting started
---------------

Before starting this tutorial you should first get a copy of mCRL2 for
your platform from the mCRL2 website at
`<http://mcrl2.org/mcrl2/wiki/index.php/Download>`_. Installation
instructions can be found at
`<http://mcrl2.org/mcrl2/wiki/index.php/Installation_instructions>`_.

.. note::

  If you are using mCRL2 on Windows, then the compiling
  rewriter is unavailable, meaning that the flag ``-r jittyc`` to
  any of the tools will fail.

In this tutorial, we assume that you will be using the tools from the command
line. On Windows this is the command prompt, on other platforms this is a
terminal. Commands that should be enterend at the prompt are displayed as::

  $ command

.. toctree::
 
   machine/index
   watercans/index
   hanoi/index
   ropebridge/index
   phonebook/index
   mcrl2-gui
