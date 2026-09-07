.. index:: pbescegps

.. _tool-pbescegps:

pbescegps
=================

This tool solves a parameterised Boolean equation system by iteratively under- and over-approximating the values of the
parameters of the predicate variables. Parameters are abstracted by replacing data expressions that depend on them by
``true`` or ``false``, which yields smaller approximations: if an under-approximation solves to ``true``, or an
over-approximation solves to ``false``, then so does the original PBES. Otherwise, one of the abstracted parameters is
made concrete and the process repeats.

The initial set of abstracted parameters can be read from a file via the option ``-a``/``--initial-state``,
for instance containing an abstraction set found by :ref:`tool-pbesfindabs`. 

.. mcrl2_manual:: pbescegps
