Introduction
============
The Core library provides a low level interfaces to the internally used data
structures. Using this library internal data structures can be manipulated
directly, parsed from and printed to human readable textual descriptions. 

Concepts
========
The low level data structures are represented as ATerms (short for Annotated
Terms). ATerms can be used to represent tree-like data structures which have
distinguishing features as: maximal subterm sharing, annotations, automatic
garbage collection and compressed binary exchange formats. A more elaborate
description can be found `here <http://www.cwi.nl/htbin/sen1/twiki/bin/view/Meta-Environment/ATerms>`_. 

Structure
=========
The Core library is a collection of methods and functions in the core name space.
In fact it is a collection of small sub-libraries, each with their own functionality.

The alphabet reduction library transforms the mCRL2 specification to an
equivalent specification where renaming operations (allow, hide, rename, block
and communication) are distributed over the other operations. The reduction is
performed to reduce the number of multi-actions which are generated during
intermediate steps in the mCRL2 linearization procedure.

The internal mCRL2 ATerm is a complex structure, especially when the
specifications grow large. To improve the readability on the internal ATerm
format, the print library provides the opportunity to pretty print ATerms in
a more human readable format.

References
----------

.. [ATerm] M.G.J. van den Brand and P. Klint (2007). "ATerms for manipulation and
   exchange of structured data: It's all about sharing." Information and Software
   Technology 49:55--64.

