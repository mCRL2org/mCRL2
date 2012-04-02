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

The tool transforms this to the following strongly bisimilar LPS
(where it must be noted that in this particular case the condition
``x>=0`` has been added to each summand in the input LPS). 

.. code-block:: mcrl2

   sort Comp = struct smaller?is_smaller | equal?is_equal | larger?is_larger;

   act  step,urgent,reset;

   proc P(xi,xi00: Comp) =
          !is_smaller(xi) ->
            reset .
            P(equal, smaller)
        + (is_smaller(xi00) && !is_smaller(xi)) ->
            urgent .
            P(larger, equal)
        + (!is_smaller(xi) && is_smaller(xi00)) ->
            urgent .
            P(larger, smaller)
        + (is_smaller(xi00) && !is_smaller(xi)) ->
            urgent .
            P(larger, larger)
        + (is_smaller(xi00) && !is_smaller(xi)) ->
            step .
            P(larger, equal)
        + (!is_smaller(xi) && is_smaller(xi00)) ->
            step .
            P(larger, smaller)
        + !is_smaller(xi) ->
            step .
            P(larger, larger);

   init P(equal, smaller);


