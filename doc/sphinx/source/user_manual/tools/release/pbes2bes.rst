.. index:: pbes2bes

.. _tool-pbes2bes:

pbes2bes
=========

Generation of a BES
-------------------

A BES is generated as follows. First the initial instantiated variable is
considered. The instantiated right hand side for this variable is calculated,
where all expressions without PBES variables are eliminated. Expressions are
reduced to ``true`` or ``false`` and quantifiers are eliminated by
enumerating its elements. As an example consider the PBES (where we use natural
numbers as parameters, but these could be different of course).

.. code-block:: mcrl2

   pbes mu X(n:Nat)=(n<5)||(forall m:Nat.(m<=n+1) => X(m));
   init X(0);

The initial instantiated variable is ``X(0)``. The right hand side belonging to 
``X(0)`` is

.. code-block:: mcrl2

   (0<5)||(forall m:Nat.(m<=0+1) => X(m))

which using rewriting reduces to:

.. code-block:: mcrl2

   (forall m:Nat.(m<=1) => X(m))

The variable ``m`` can either be ``0`` or ``1``. Using that natural numbers are
defined by constructors, a technique called narrowing is used, using which it is
calculated that ``0`` and ``1`` are the only useful values for ``m``.
The expression reduces to:

.. code-block:: mcrl2

   X(0) && X(1)

So, the first boolean equation that results is

.. code-block:: mcrl2

   mu X(0)=X(0) && X(1)

The next step is to calculate the equation for ``X(1)``.

Strategies
----------

There are different strategies to generate the equations.

``0``

  In strategy 0, all equations are generated in a breadth first search. The
  equations generated in the order the instantiated variables are encountered.

``1``
  
  In strategy 1, a small improvement is made. If an equation of the form
  
  .. code-block:: mcrl2
     
     nu X(17)=(X(10) && X(18)) || X(19)
     
  is encountered and the right hand side for ``X(10)`` is ``false``, then by
  substituting ``false`` for ``X(10)`` the equation reduces to
  
  .. code-block:: mcrl2
     
     nu X(17)=X(19)
     
  .. note::
  
     The instantiated variable ``X(18)`` disappears. It can be that ``X(18)``
     does not have to be investigated at all, saving work compared to strategy
     0.

``2``

  The idea of substituting ``true`` and ``false`` and avoiding
  unnecessary work is taken one step further in strategy 2. Here, whenever a
  right hand side of an instantiated bes variable is ``true`` or
  ``false``, this value is substituted for the instantiated variable every
  where. The advantage of strategy 2 is that when the validity of a modal
  formula can be detected by only investigating parts of the state space, this
  is detected. The costs of strategy 2 is a higher memory footprint than for
  strategy 0 and 1. Consider the following partially generated BES from some
  PBES, which typically is the result of a deadlock check on a transition system
  with a deadlock.
  
  .. code-block:: mcrl2
  
     nu X(0)=X(1) && X(2) && X(3)
     nu X(1)=X(4) && X(5)
     nu X(2)=X(6)
     nu X(3)=false
  
  With strategy 2, the value for ``X(3)` will be substituted. The result is that
  ``X(0)`` becomes ``false``. Furthermore, the instantiated variables ``X(1)``
  and ``X(2)`` cannot be reached and have become superfluous. Using strategy 2,
  a garbage collection algorithm prevents such variables from being
  investigated. 
  
``3``

  Strategy 3 is very comparable to strategy 2, except that when a dependency
  loop of instantiated variables is detected, it attempts to set all the
  variables in the loop to ``true`` in case the fixpoint symbol is a nu, or
  ``false`` if it is a mu. For example 
  
  .. code-block:: mcrl2
  
     nu X(38)=X(39)
     nu X(39)=X(38)
     
  it can set both ``X(38)`` and ``X(39)`` to ``true``. Moreover, following
  strategy 2, it can subsequently substitute ``true`` for ``X(38)`` and
  ``X(39)`` in the generated equations.

