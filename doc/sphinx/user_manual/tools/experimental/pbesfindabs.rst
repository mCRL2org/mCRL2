.. index:: pbesfindabs

.. _tool-pbesfindabs:

pbesfindabs
=================

This tool enumerates all valid abstraction sets of a PBES. An abstraction set is a set of parameters of the predicate
variables of a PBES, and it is valid if replacing data expressions that depend on these parameters by ``true`` or
``false`` yields an approximation that proves the answer of the original PBES. Every valid set is written to the file
given by ``-a``/``--abstraction-file`` as soon as it is found.

.. mcrl2_manual:: pbesfindabs
