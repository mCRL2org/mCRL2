.. _language-mu-calculus:

µ-Calculus
==========

The µ-calculus used in mCRL2 is a first-order modal
µ-calculus extended with data-depended processes and regular formulas.
This page describes the concrete syntax of these µ-calculus formulas,
and how it is used in the toolset.

The syntax of the µ-calculus consists of multiactions, action
formulas, regular formulas and state formulas. Next to these, the use of
µ-calculus formulas in files is described.

Multi-actions
-------------

Multi-actions are defined by the following syntax.

.. dparser:: ActionList MultAct

The constant ``tau`` represents for the empty multiaction,
``a0 | ... | an`` represents the multiaction consisting of parameterised actions
``ai``, for ``0 ≤ i ≤ n``.

Action formulas
---------------

Action formulas α are defined by:

.. dparser:: DataValExpr ActFrm

The ``val`` operator stands for the value of a boolean data expression, ``true``
stands for true, ``false`` for false, ``!`` for not, ``&&`` for and, ``||`` for or, and
``=>`` for implication. The rules starting with ``forall`` and ``exists`` stand for
universal and existential quantification. The ``@`` operator stands for an action
formula at the specified time.

The ``!`` operator has the higher priority, followed by ``@``, followed by ``&&`` and
``||``, followed by ``=>``, followed by ``forall`` and ``exists``. These priorities can
be overruled with the use of parentheses ``(`` and ``)``. The infix operator ``@``
associates to the left, while the infix operators ``&&``, ``||`` and ``=>`` associate
to the right.

Regular formulas
----------------

Regular formulas are defined by:

.. dparser:: RegFrm

Here, ``nil`` represents empty, ``.`` concatenation, infix ``+`` choice, ``*``
transitive reflexive closure, and postfix ``+`` transitive closure.

The unary operators have the highest priority, followed by ``.``, followed by
infix ``+``. The infix operators associate to the right. These priorities can be
overruled with the use of parentheses ``(`` and ``)``.

State formulas
--------------

State formulas are defined by:

.. dparser:: StateVarAssignment StateVarAssignmentList StateVarDecl StateFrm

Here :token:`StateVarDecl` represents a propositional variable declaration and
initialisation with a list of assignments (:token:`StateVarAssignment`).

The ``val`` operator used in :token:`DataExprVal` stands for the value of a
boolean data expression, ``true`` stands for true, ``false`` for false, ``!`` for not,
``&&`` for and, ``||`` for or, and ``=>`` for implication. The rules starting with
``forall`` and ``exists`` stand for universal and existential quantification, the
rules ``[`` :token:`RegFrm` ``]`` :token:`StateFrm` and
``<`` :token:`RegFrm` ``>`` :token:`StateFrm` for the must and may operators, and
the rules starting with ``nu`` and ``mu`` for the greatest and smallest fixed point
operators. The timed ``delay`` and ``yaled`` operators stand for the possibility to
delay until a certain time, or not. The untimed ``delay`` and ``yaled`` operators
stand for the possibility to delay forever, or not.

The prefix operators and the must and may operators have the highest priority,
followed by ``&&`` and ``||``, followed by ``=>``, followed by ``forall``, ``exists``,
``nu`` and ``mu``. The infix operators ``&&``, ``||`` and ``=>`` associate to the right.
These priorities can be overruled with the use of parentheses ``(`` and ``)``.

The must and may operators have the following meaning. In a state of the state
space a formula ``[φr]φs`` is valid if all paths that start in this state and
satisfy ``φr``, lead to a state where ``φs`` is valid. In a state of the state
space a formula ``<φr>φs`` is valid if there exists a path that starts in this
state, satisfies ``φr`` and leads to a state where ``φs`` is valid.

The following restrictions apply to propositional variables:

* monotonicity: every occurrence of a propositional variable should be in a
  scope such that the number of ``!`` operators plus the number of left-hand sides
  of the ``=>`` operator is even;
* no overloading: it is not allowed to declare two propositional variables with
  the same name but with a different type.

.. note::

   The tool lps2pbes uses mu-calculus formulas files, which contain precisely one
   state formula.

.. note::

   The suggested extension of formula files is ".mcf".

.. note::

   Data variables declared using ``forall``, ``exists``, ``nu`` and ``mu`` quantifiers,
   we have the following variable conventions:

   * Each occurrence of a variable is bound by the nearest quantifier in scope
     of which the bound variable has the same name and the same number of
     arguments.
   * Variables introduced by a ``nu`` or ``mu`` quantifier may not conflict, i.e.
     all names of data variables have to be distinct.

Relations between symbols
-------------------------

Besides the well-known relations between symbols of first-order logic, the
following relations hold for regular formulas:

.. code-block:: mcrl2

   [nil]φs   = [false*]φs
   [φr.ψr]φs = [φr][ψr]φs
   [φr|ψr]φs = [φr]φs && [ψr]φs
   [φr*]φs   = nu X.(φs && [φr]X), if X is fresh for φs
   [φr+]φs   = [φr.φr*]φs

   <nil>φs   = <false*>φs
   <φr.ψr>φs = <φr><ψr>φs
   <φr|ψr>φs = <φr>φs || φr>φs
   <φr*>φs   = mu X.(φs || <φr>X), if X is fresh for φs
   <φr+>φs   = <φr.φr*>φs

The following relations hold for the modal operators, where ``φs(!X)`` represents
substitution of ``!X`` for every free occurrence of ``X`` in ``φs``:

.. code-block:: mcrl2

   [φr]φs    = !<φr>!φs
   nu X.φs   = !mu X.!φs(!X)

We have the following identities for the ``delay`` and ``yaled`` operators:

.. code-block:: mcrl2

   delay     = forall t: Real. delay@t
   yaled@t   = !(delay@t)
   yaled     = !delay

Examples
--------

Freedom of deadlock:

.. code-block:: mcrl2

   [true*]<true>true

Action ``b`` may not happen after an action ``c``, unless an action ``a`` occurs
after this ``c`` and before this ``b``:

.. code-block:: mcrl2

   [true*.c.!a*.b]false

The action ``b`` may not occur unless an action ``a`` happens first:

.. code-block:: mcrl2

   [!a*.b]false

There exists an infinite sequence of ``a.b.c``'s:

.. code-block:: mcrl2

   <true*>nu X.<a.b.c>X

These formulas are equivalent to the following formulas in which no regular
operations, i.e. empty path ``nil``, concatenation ``.``, choice ``+``, transitive
reflexive closure ``*`` and transitive closure ``+``, occur:

.. code-block:: mcrl2

   nu X.(<true>true && [true]X)
   nu X.([c] nu Y.([b]false && [!a]Y) && [true]X)
   nu X.([b]false && [!a]X)
   mu X.(nu Y.(<a><b><c>Y) || <true>X)
