.. index:: lpsparunfold

.. _tool-lpsparunfold:

lpsparunfold
============

The algorithm applies a transformation on data expressions of an linear process
specification (LPS), by which other tools (such as lpsparelm, lpsconstelm) can
apply their transformations more effectively. In concrete, this tool unfolds a
sort with associated constructor functions to a set of process parameters in the
LPS, in such a way that all behaviour is preserved.

