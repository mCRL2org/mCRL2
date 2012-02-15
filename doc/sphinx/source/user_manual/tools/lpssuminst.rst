.. index:: lpssuminst

.. _tool-lpssuminst:

lpssuminst
==========

Instantiate the summation variables in an LPS by applying induction on the
values of the sort of the variable.

Example
-------

Take the the following LPS:

.. code-block:: mcrl2

   sort D = struct d1 | d2;
        E = struct e1(D) | e2;

   act  a: D;
        b: E;

   proc P = sum d: D. a(d) . P
          + sum e: E. b(e) . P
          ;

   init P;

:ref:`tool-lpssuminst` instantiates the variable ``d`` to the constructors
``d1`` and ``d2`` of sort ``D``, and variable ``e`` to the constructors
``e1(d1)``, ``e1(d2)`` and ``e2``:

.. code-block:: mcrl2

   sort D = struct d1 | d2;
        E = struct e1(D) | e2;

   act  a: D;
        b: E;

   proc P = a(d1) . P
          + a(d2) . P
          + b(e1(d1)) . P
          + b(e1(d2)) . P
          + b(e2) . P
          ;

   init P;


