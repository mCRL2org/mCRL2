.. index:: mcrl2compilerewriter

.. _tool-mcrl2compilerewriter:

mcrl2compilerewriter
====================

Manual page for mcrl2compilerewriter
------------------------------------

Usage
^^^^^

::

   mcrl2compilerewriter INFILE


Description
^^^^^^^^^^^

This script is the default script to compile rewriter libraries for mCRL2.
It compiles the source file in INFILE. The output consists of a newline separated
list of files. The last file in the list is treated as the compiler library,
and must be a valid executable. All files listed in the output are deleted by
the mCRL2 tool that calls this script, once the library is no longer needed.

To change compiling behaviour, follow the following steps:

* Create a new script (perhaps based on the original one)
* Let the MCRL2_COMPILE_REWRITER environment variable point to the new script.

Author
^^^^^^

Written by Sjoerd Cranen.
