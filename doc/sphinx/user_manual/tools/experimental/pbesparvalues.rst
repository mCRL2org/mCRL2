.. index:: pbesparvalues

.. _tool-pbesparvalues:

pbesparvalues
=============

The tool ``pbesparvalues`` statically enumerates the values that the parameters of the predicate variables of a
parameterised Boolean equation system can take during exploration. For each parameter of each equation it prints an
over-approximation of the set of reachable values. It also reports an estimate of the number of reachable predicate
variable instances.

Example
-------

Consider the following PBES.

.. code-block:: mcrl2

  pbes nu X(n: Nat) =
          (val(n < 5)) => Y(n + 1, true) && X(n + 1);
       nu Y(m: Nat, b: Bool) = Y(m, b);
  init X(0);

Applying pbesparvalues to this PBES yields::

  This process has at most 30 states.
  Parameter (X, n): Nat := { 0, 1, 2, 3, 4, 5 }
  Parameter (Y, m): Nat := { 1, 2, 3, 4, 5 }
  Parameter (Y, b): Bool := { true }

The domains are per equation and parameter, so parameters with the same name in different equations are distinguished.

.. mcrl2_manual:: pbesparvalues
