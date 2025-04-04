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
`-s/--simplifier`. The ``fm`` simplifier is based on the Fourier-Motzkin
algorithm and only works on linear inequalities.

To track the progress of the tool, one can use the option
`--log-level=status`. The option `--fine-initial` causes the
algorithm to start off with a finer partition. In general, this will lead to
a longer runtimes. However, in certain cases, it drastically speeds up the
algorithm.

The option `--no-early-termination/-t` in some cases also speeds up the
algorithm. When this option is supplied, no early termination checks are done
at the start of every iteration. This can lead to a speed up of at most a factor
two. However, the resulting proof graph may be much larger, negating the
speed-up.

If the tool spends a lot of time on parity game generation, it might help to
set the option `--refine-steps/-n` to 2 or 3. It saves some overhead by
generating a parity game less often, but it may easily lead to a larger proof
graph.

To run this tool, the Z3 SMT-solver should be installed and its bin-directory
has to be added to the PATH variable.

.. mcrl2_manual:: pbessymbolicbisim