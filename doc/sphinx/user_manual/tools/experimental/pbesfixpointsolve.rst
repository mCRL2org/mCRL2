.. index:: pbesfixpointsolve

.. _tool-pbesfixpointsolve:

pbesfixpointsolve
=================

This tool is aimed at solving Parameterised Boolean Equation Systems with an
infinite underlying Boolean equation system using *iteration* or *symbolic
approximation* [GW05]_.  In the context of finite data types, it is guaranteed
(at least in theory) that we reach a fixpoint for each equation in a finite
number of steps. In practice, we can also reach fixpoints in some cases with
infinite data types.

Fixpoint checking can be done using the internal EQ-BDD library or SMT library.
By default, EQ-BDD are used.

The option ``-s/--smt`` runs the tool using the SMT library. It is often much faster,
however, it can be used in more limited cases than the EQ-BDD option, due to the
fact that the translation to SMT of certain data sorts are hard (if not
impossible), e.g. lists or sets.  When using the SMT option, the Z3 SMT-solver
should be installed and its bin-directory has to be added to the PATH variable.

Iteration
---------

Iteration, or symbolic approximation, is based on fixpoint theory. Let
:math:`(\sigma X(d:D) = \varphi_X)` be an equation where :math:`\sigma \in
\{\mu, \nu\}`.

.. math::
    \begin{split}
      X^0(d) &=
      \begin{cases}
        \bot &  \text{if } \sigma = \mu \\
        \top &  \text{if } \sigma = \nu
      \end{cases}\\
      X^{n+1}(d) &= \varphi_X[X^n/X](d)
    \end{split}

We reach a fixpoint when :math:`\varphi_X[X^{n+1}/X] \equiv \varphi_X[X^n/X]`,
so when substituting again does not semantically change the right-hand side
anymore. We are guaranteed to reach a fixpoint if the equation is monotone, i.e.
when for all :math:`d` we have that :math:`X^{n+1}(d) \implies X^{n}(d)` if
:math:`\sigma = \nu`, and :math:`X^{n}(d) \implies X^{n+1}(d)` if :math:`\sigma
= \mu`. All PBES generated from mcrl2 specifications are monotone. Due to
Knaster-Tarski, we know the fixpoint is not just any fixpoint, but also that it
is the least/greatest fixpoint.

Let's show an example.

.. math::
    \begin{split}
    \mathcal{E}  = (\nu X(a,b : Nat) = &\ \ \neg (a = 3)  \\
    &\land (a = 1) \implies X(2, b) \\
    & \land  (a = 2) \implies X(3, b)) \\
    \end{split}

Iteration goes as follows:

.. math::
    \begin{split}
    X^0(a,b) =&\ \top \\
    \\
    X^1(a,b) = &\ \ \neg (a = 3)  \\
    &\land (a = 1) \implies X^0(2,b) \\
    & \land  (a = 2) \implies X^0(3,b) \\
    = &\ \ \neg (a = 3)  \\
    &\land (a = 1) \implies \top \\
    & \land  (a = 2) \implies \top \\
    = &\ \ \neg (a = 3) \\
    \\
    X^2(a,b) = &\ \ \neg (a = 3)  \\
    &\land (a = 1) \implies X^1(2,b) \\
    & \land  (a = 2) \implies X^1(3,b) \\
    = &\ \ \neg (a = 3)  \\
    &\land (a = 1) \implies \neg(2 = 3) \\
    & \land  (a = 2) \implies \neg(3 = 3) \\
    = &\ \ \neg (a = 3)  \\
    &\land (a = 1) \implies \top \\
    & \land  (a = 2) \implies \bot \\
    = &\ \ \neg (a = 3)  \land \neg (a = 2) \\
    \\
    X^3(a,b) = &\ \ldots = \neg (a = 3)  \land \neg (a = 2) \land \neg (a = 1) \\
    \\
    X^4(a,b) = &\ \ldots = \neg (a = 3)  \land \neg (a = 2) \land \neg (a = 1)
    \end{split}

Thus :math:`X^3` is :math:`\nu X`. The final equation system becomes 

.. math::
    \mathcal{E} \equiv (\nu X(a,b : Nat) = \ \neg (a = 3)  \land \neg (a = 2)
    \land \neg (a = 1))

One might notice that we can now indeed conclude that parameter :math:`b` is
irrelevant, as it does not occur syntactically in the fixpoint solution.

Alternating equations
---------------------
If the equation we are currently iterating contains a predicate variable
instance (PVI) refering to another equation, we need to translate the PVI to
some data expression for the BDD and SMT libraries. We make use of uninterpreted
functions (see also [GP00]_) to do this. I.e. translating the PVI to a function
of type 'parameters' to boolean. Two function expression are then equal if all
of the parameters match and the function symbol matches.

In practice
-----------

(1) Rewrite rules

Due to the nature of EQ-BDD, when your specification contains operators other
than equality, it might not terminate. For instance, when trying to verify the
mutual exclusion property of Lamport's bakery protocol, the BDD solver could not
reduce BDDs where nodes included the 'less-than' operator that were clearly true
or false. For instance, :math:`0 = n + 1` or :math:`n + 1 < n` for :math:`n \in
\mathbb{N}`. To solve this, the following rewrite rules needed to be added to
the mCRL2 specification.::

    var n, m : Nat;
    eqn (n + m < n) = false;
    (n < n + m) = m > 0;
    m > 0 -> (0 == n + m) = false;

(2) PBES optimization

At one point, we studied an equation of some PBES of the following form.::

    mu X0(b1: Bool) = ... || exists n1: Nat. val(b1 && 0 == n1) && ....

Here, we can apply the one-point rule to rewrite all instances of `n1` to `0`.
Adding `pbesrewr -pquantifier-inside` and `pbesrewr -pquantifier-one-point`
before solving the PBES resulted in a solving time of 413ms compared to 8m7s
without the one-point rule.

.. mcrl2_manual:: pbesfixpointsolve
