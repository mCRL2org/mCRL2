.. index:: pbessymbolicbisim

.. _tool-pbessymbolicbisim:

pbessymbolicbisim
=================

This tool is aimed at solving parameterised Boolean equation systems with an
infinite underlying Boolean equation system. Most other tools, such as
:ref:`tool-pbessolve` and :ref:`tool-pbesinst`, rely on enumeration of the data
domain to generate the corresponding BES. To avoid this, pbessymbolicbisim
applies symbolic techniques to represent infinite sets.

For intermediate simplification of the symbolic representation, the tool relies
on one of several simplifying strategies, which can be selected with the option
:option:`-s/--simplifier`. The ``fm`` simplifier is based on the Fourier-Motzkin
algorithm and only works on linear inequalities.

To track the progress of the tool, one can use the option
:option:`--log-level=status`. The option :option:`--fine-initial` causes the
algorithm to start off with a finer partition. In general, this will lead to
a longer runtimes. However, in certain cases, it drastically speeds up the
algorithm.

To run this tool, the Z3 SMT-solver should be installed and its bin-directory
has to be added to the PATH variable.
