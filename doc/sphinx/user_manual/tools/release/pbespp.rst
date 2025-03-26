.. index:: pbespp

.. _tool-pbespp:

pbespp
======

This tool is intended to pretty print parameterised Boolean equation systems in
a human readable form. It has an option to print a PBES in internal aterm
format, but this is for deep debugging purposes only. To convert the obtained
textual PBES back to the aterm-encoded format that is used by other tools, one
can use the tool :ref:`tool-txt2pbes`. When the argument for the output location
is omitted, the tool prints to stdout. When both the arguments for input and
output locations are omitted, the tool reads from stdin and writes to stdout.

.. mcrl2_manual:: pbespp
