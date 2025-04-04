.. index:: bessolve

.. _tool-bessolve:

bessolve
=========

The tool ``bessolve`` can be used to solve a BES. The solution is printed as ``true`` or ``false``
on the command line output.

Three different solving algorithms have been implemented:

Gauss elimination
  This is a straightforward implementation of the Gauss elimination technique. In general
  it is very inefficient.

Local fixpoints
  This is a more advanced form of Gauss elimination. It is efficient if the BES
  contains no alternating fixed points. For this algorithm a justification for the solution
  can be printed.

Small progress measures
  This is an implementation of the small progress measures algorithm by Marcin Jurdzinski.

Note that a BES can also be solved by the tool ``pbessolve``. This tool uses Zielonka's
algorithm, which often performs better than the ones implemented in ``bessolve``.

.. mcrl2_manual:: bessolve
