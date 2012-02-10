.. index:: lpssumelm

.. _tool-lpssumelm:

lpssumelm
=========

Remove superfluous summations from a linear process specification (LPS).

The simplest case this tool handles is that, whenever a summation variable does
not occur in the other terms of the summand, this summation variable is plainly
removed. There is however also a more complex situation where the summation
variable occurs in an equality within the condition. There is an axiom that says
that we may then substitute the other term of the equality for the summation
variable. We can then remove both the summation variable and the condition in
which it occurred.

Known issues
------------

The tool is not applicable if types in an equality in a condition do not match
directly, i.e. an implicit type cast is used in order to match the types. An
example of this is the following process:

.. code-block:: mcrl2

   proc P(i : Int) = sum n : Nat . (n == i) -> tau . P(i);
   
   init P(5);
   
In this example an implicit cast ``Nat2Int`` (i.e. ``Nat2Int(n) == i``) is used
in order to have compatible types. Because of this, the tool is unable to remove
the summation over ``n``.

