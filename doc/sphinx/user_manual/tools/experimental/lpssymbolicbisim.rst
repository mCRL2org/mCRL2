.. index:: lpssymbolicbisim

.. _tool-lpssymbolicbisim:

lpssymbolicbisim
================

This tool is aimed at generating the reachable part of the bisimulation quotient
of systems with an infinite state space. Classical state-space enumeration, such
as implemented in :ref:`tool-lps2lts`, relies on enumeration and stores each
state explicitly. To avoid this, lpssymbolicbisim applies symbolic techniques to
represent infinite sets.

To run this tool, the Z3 SMT-solver should be installed and its bin-directory
has to be added to the PATH variable.

Limitations
-----------
The tool can only deal with specifications of limited complexity. Furthermore,
there are several other restrictions:

- The tool cannot deal very well with actions that have data parameters. Actions
  with Boolean parameters will not lead to a large slowdown, but for larger
  domains, scalability will be an issue.

In any case, :ref:`tool-pbessymbolicbisim` is probably more powerful, since its
abstractions can also rely on the information of the property being checked and
it does not have to deal with actions and their parameters.

.. mcrl2_manual:: lpssymbolicbisim