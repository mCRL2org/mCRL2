.. index:: formulacheck

.. _tool-formulacheck:

formulacheck
============

The formulacheck tool checks the boolean formula (an mCRL2 data expression of
sort ``Bool``), and indicates whether or not the formula is a tautology or a
contradiction.

In some cases, the tool is unable to determine whether a formula is a tautology
or a contradiction. The option :option:`--verbose` gives insight into what the
prover used by the tool is doing and can be used to see if rewrite rules have to
be added to the specification in order to enable the prover to determine that
certain formulas are indeed tautologies or contradictions.

A BDD based prover is used to the check the formula. In some cases it may be
useful to use an SMT solver to assist the prover. The SMT solver can further
reduce BDDs by removing inconsistent paths. A specific SMT solver can be chosen
using the option :option:`--smt-solver=SOLVER``. Either the SMT solver `Ario
<http://www.eecs.umich.edu/~ario>`_ or `CVC3
<http://www.cs.nyu.edu/acsys/cvc3>`_ can be used. To use one of these solvers,
the directory containing the corresponding executable must be in the path.

Example of use
--------------

The input file typically contains a single expression such as

.. code-block:: mcrl2

  3+3<4

When formulacheck would be applied to this file it would respond with::

 '3 + 3 < 4': Contradiction

If the input formula holds, the following output would be::

 '#[4, 5, 6] > 2': Tautology

If the input contains an expression which cannot be rewritten to true or false
the response is::

 '[1, 4, 7]': Undeterminable

It is not possible to use formulacheck for open terms. For this the interactive
tool :ref:`tool-mcrl2i` or its graphical counterpart :ref:`tool-mcrl2xi` are
more convenient.
