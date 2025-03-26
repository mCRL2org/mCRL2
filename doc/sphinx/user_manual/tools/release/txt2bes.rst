.. index:: txt2bes

.. _tool-txt2bes:

txt2bes
=======

The tool :ref:`tool-txt2bes` reads a textfile containing a boolean equation system and
writes it into one of the availabel internal formats to represent a BES.
A typical example of textual input is the following::

  pbes nu Y1 = Y1 && Y2;
       mu Y2 = Y1 || Y2;
  init Y1;

It is possible to declare data types in the input, but this is generally of
no use as the data cannot be used in the BES.

.. mcrl2_manual:: txt2pbes
