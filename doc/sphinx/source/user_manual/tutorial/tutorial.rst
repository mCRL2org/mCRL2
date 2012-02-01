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

Before starting this tutorial you should first :ref:`download <download>` a copy
of mCRL2. See also the :ref:`build instructions <build>`.

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
   gossip/index
   mcrl2-gui/index
