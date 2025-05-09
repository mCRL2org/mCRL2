.. index:: ltspcompare

.. _tool-ltspcompare:

ltspcompare
===========

The tool ``ltspcompare`` compares two transition systems that both must have
either the extension .aut, .lts or .fsm. The tool is intended to compare
probabilistic transition systems. As it stands there is only one equivalence
implemented, which is strong probabilistic bisimulation for non-deterministic
processes a la Larsen and Skou. There are two implementations, namely the
classical but rather slow algorithm by Baier, Engelen and Majster-Cederbaum, and
the much faster one by Groote, Rivera-Verduzco and de Vink.

The intention is to add more equivalences and preorders when efficient
algorithms for them become available.

.. mcrl2_manual:: ltspcompare
