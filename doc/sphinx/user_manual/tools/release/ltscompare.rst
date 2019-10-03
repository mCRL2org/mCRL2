.. index:: ltscompare

.. _tool-ltscompare:

ltscompare
==========

The tool ``ltscompare`` compares two transition systems that both must have
either the extension .aut, .lts or .fsm. The transition systems should not
be probabilistic.

The transition systems can either be compared using an equivalence relation
(option ``--equivalence``) or a preorder (option ``--preorder``). The list of
available equivalences is provided below.

There are two useful options. One allows to generate counter examples in the form
of a trace (option ``--counter-example``). These counter examples are useful for
trace based equivalences. They can also be generated for some variants of bisimulation,
but in this case they can also point to states where the two transition systems are
nondeterministic.

The second useful option is to hide some actions while doing the comparisons
(option ``--tau=`` followed by a comma separated list of actions). Counter examples
are provided without applying hiding.


.. include:: man/ltscompare.txt
