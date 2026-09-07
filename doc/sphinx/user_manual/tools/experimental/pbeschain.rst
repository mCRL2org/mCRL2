.. index:: pbeschain

.. _tool-pbeschain:

pbeschain
=================

This tool is aimed at reducing parameterised Boolean equation systems by selectively 'unfolding' specific predicate
variable instances if its unfolding contains at most one predicate variable instance.

When used in combination with :ref:`tool-pbessolvesymbolic`, it is strongly recommended to translate the PBES to SRF first using :ref:`tool-pbesrewr` to avoid an exponential increase in guard complexity.

.. mcrl2_manual:: pbeschain
