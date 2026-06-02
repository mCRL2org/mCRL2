.. index:: lpsrealelm

.. _tool-lpsrealelm:

lpsrealelm
==========

The tool replaces parameters of sort real from the linear process specification
by parameters of sort ``Comp``, which is an structured sort with three constants
``smaller``, ``equal`` and ``larger``. They represent that certain pairs of real
expressions must be smaller, equal or larger to each other. If the resulting
linear process does not contain ``MAY``-actions, it is strongly bisimilar to the
input lps. Otherwise, the result is may-bisimilar to the original.

Example
-------

Consider an LPS with the following process equation.

.. code-block:: mcrl2

  act  step, urgent, reset;
  proc P(x:Real) = sum y:Real.(x<y)->step.P(y)
                 + sum y:Real.(x<y && x<2)->urgent.P(y)
                 + reset.P(0);

  init P(0);

The tool transforms this to the following strongly bisimilar LPS.

.. code-block:: mcrl2

   act  step,urgent,reset;

   proc P(xi: Bool) =
          reset .
            P(xi = true)
        + (xi) ->
            urgent .
            P(xi = true)
        + (xi) ->
            urgent .
            P(xi = false)
        + (xi) ->
            step .
            P(xi = true)
        + step .
            P(xi = false)
        + delta;

   init P(true);

.. mcrl2_manual:: lpsrealelm