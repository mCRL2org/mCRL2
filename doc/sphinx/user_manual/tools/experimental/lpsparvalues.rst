.. index:: lpsparvalues

.. _tool-lpsparvalues:

lpsparvalues
============

The tool ``lpsparvalues`` statically enumerates the values that the parameters of a linear process specification can
take during state space exploration. For each parameter it prints an over-approximation of the set of values that can be
reached starting from the initial state of the LPS. It also reports an estimate of the size of the state space.

Example
-------

Consider the following mCRL2 specification.

.. code-block:: mcrl2

  act  coin, push;
  proc P(n: Nat) = (n < 5) -> coin.P(n + 1)
                 + (n >= 5) -> push.P(0);
  init P(0);

After linearisation with mcrl22lps, applying lpsparvalues to the resulting linear process yields::

  This process has at most 6 states.
  Parameter n_P: Nat := { 0, 1, 2, 3, 4, 5 }

That is, during exploration the parameter ``n_P`` is always one of the values between 0 and 5, so the state space
consists of at most 6 states.

.. mcrl2_manual:: lpsparvalues
