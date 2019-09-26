.. index:: pbesabstract

.. _tool-pbesabstract:

pbesabstract
============

Under- or overapproximate the solution of a PBES, by abstracting from variables.

Example
-------

Consider the following PBES:

.. code-block:: mcrl2

   nu X(n:Nat) = val(n > 1000000) || X(n+1)

Computing ``X(0)`` depends on the computation of equations for all ``X(i)``,
for ``i <= 1000000``. After pbesabstract, for variable ``n``, the equation is
reduced to:

.. code-block:: mcrl2

   nu X(n:Nat) = false || X(n+1)

Now, an application of pbesparelm can detect the redundancy of variable
``n``, allowing one to rewrite the above equation to the equivalent PBES:

.. code-block:: mcrl2

   nu X = false || X

The latter is readily seen to have solution ``true``. As a result, also the
original equation systems have result ``true``.
