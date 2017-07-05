.. index:: lpsinvelm

.. math::
   :nowrap: 
 
    \renewcommand{\implies}{\mathop{\Rightarrow}}

lpsinvelm
=========

Given an LPS:

.. math::

   P(d:D) = \ldots + \sum_{e_i:E_i} c_i(d,e_i) \to a_i(f_i(d,e_i)) \cdot P(g_i(d,e_i)) + \ldots
    
a formula of the form

.. math::

   inv(d) \land c_i(d,e_i) \implies inv(g_i(d,e_i))

is generated for each of the summands, where :math:`inv()` is the expression
passed using the option :option:`--invariant`. This expression is an invariant
of the LPS if it holds in the initial state and all the generated formulas are
tautologies.

The invariant is used to eliminate summands as follows. A formula of the form 

.. math::

   inv(d) \land c_i(d,e_i)

is generated for each of the summands or for the summand indicated using the
option :option:`--summand` only. The tool uses a BDD based prover for expressions
of sort ``Bool`` to see if the generated formula is a contradiction. If the
formula is a contradiction for some summand, this summand will be eliminated
from the LPS. If the formula is not a contradiction, the summand remains
unchanged unless the option :option:`--simplify-all` is used.

The option :option:`--simplify-all` will replace the conditions of all summands
by the equivalent BDD of the condition in conjunction with the invariant passed
using the option :option:`--invariant`. This may enable other tools, like
:ref:`tool-lpsconstelm` and :ref:`tool-lpsparelm`, to simplify the LPS even
further.

In some cases it may be useful to use an SMT solver to assist the prover. The
SMT solver can further reduce BDDs by removing inconsistent paths. A specific
SMT solver can be chosen using the option :option:`--smt-solver=SOLVER``. Either
the SMT solver `Ario <http://www.eecs.umich.edu/~ario>`_ or `CVC3
<http://www.cs.nyu.edu/acsys/cvc3>`_ can be used. To use one of these solvers,
the directory containing the corresponding executable must be in the path.

Without using the option :option:`--no-check`, :ref:`tool-lpsinvelm` will
first check if the given expression is an invariant of the LPS. If this is not
the case, no elimination or simplification will be done. In some cases the
invariant may hold even though the prover is unable to determine this fact. In
cases where an expression is an invariant of the LPS, but the prover is unable
to determine this, the option :option:`--no-check` can be used to eliminate or
simplify summands anyway. Note that this also makes it possible to eliminate or
simplify summands using an expression that is not an invariant of the LPS.

The option :option:`--verbose` gives insight into what the prover is doing and
can be used to see if rewrite rules have to be added to the specification, in
order to enable the prover to determine the invariance of an expression.

Example of use
--------------

Consider a linear process specification

.. code-block:: mcrl2

   act a:Nat; b,c;
   act a, b, c;
   proc X(b1,b2:Bool) = b1 -> a.X(!b1,b2)
                      + b2 ->b.X(true,b2 && b1)
                      + (b1 && b2)->c.X(false,false);
   init X(false,true);

If the lineariser is applied to this process using::

  $ mcrl22lps -D infile.mcrl2 outfile.lps

the resulting LPS looks like

.. code-block:: mcrl2

   act  c,b,a;

   proc P(b1_X,b2_X: Bool) =
          b1_X ->
            a .
            P(b1_X = !b1_X)
        + b2_X ->
            b .
            P(b1_X = true, b2_X = b2_X && b1_X)
        + (b1_X && b2_X) ->
            c .
            P(b1_X = false, b2_X = false)
        + delta;

   init P(false, true);

Inspection of this linear process shows that ``b1_X`` and ``b2_X``
cannot both be true at the same time. So, we can define this in a file
:file:`invariant.inv`. This linear process specification has as an invariant
that 

.. code-block:: mcrl2

   !(b1_X && b2_X)

See below for a detailed definition of an invariant. 

Using::

  $ lpsinvelm -v -iinvariant.inv outfile.lps outfile1.lps
  
it is possible to check the invariant. Moreover, by default the summand with
conditions that in conjunction with the invariant are ``false`` are
removed. In the example above, the summand with action ``c`` is removed.
Using the :option:`-l` flag, the invariant is put into conjunction with the condition of
each summand, and the resulting condition is simplified using the eq-BDD prover.
So, applying::

  $ lpsinvelm -v -l -iinvariant.inv outfile.lps outfile1.lps
  
yields the following:

.. code-block:: mcrl2

   act c,b,a;

   proc P(b1_X,b2_X: Bool) =
          if(b1_X, if(b2_X, false, true), false) ->
            a .
            P(b1_X = !b1_X)
        + if(b1_X, false, if(b2_X, true, false)) ->
            b .
            P(b1_X = true, b2_X = b2_X && b1_X)
        + if(b1_X, if(b2_X, false, true), true) ->
            delta;

   init P(false, true);

Note that the conditions now have an if-then-else structure, due to the eq-BDD
prover. Also note that the summand with action ``c`` has been removed. 

Sometimes, this result is unreadable or the simplifications of the conditions in
conjunction with the invariant is extremely time consuming. This is for instance
the case if many non-boolean data types are used. In such a case the application
of the tool :ref:`tool-lpsbinary` can be helpful, by replacing finite data domains
by boolean data domains. Using the :option:`-e` flag it is possible to add the
invariants to the summands, without simplifying the summands. So, by applying::

  $ lpsinvelm -v -e -iinvariant.inv outfile.lps outfile1.lps
  
the result becomes

.. code-block:: mcrl2

   act  c,b,a;

   proc P(b1_X,b2_X: Bool) =
          (!(b1_X && b2_X) && b1_X) ->
            a .
            P(b1_X = !b1_X)
        + (!(b1_X && b2_X) && b2_X) ->
            b .
            P(b1_X = true, b2_X = b2_X && b1_X)
        + (!(b1_X && b2_X) && b1_X && b2_X) ->
            c .
            P(b1_X = false, b2_X = false)
        + !(b1_X && b2_X) ->
            delta;

   init P(false, true);

Note that the ``c`` summand is now still present.

The usage of :ref:`tool-lpsinvelm` can be useful as a preprocessing step for
symbolic reduction tools such as :ref:`tool-lpsconfcheck` and
:ref:`tool-lpsrealelm`.

When an invariant is being checked, but turns out to be false, then
counterexamples are very helpful (use the :option:`-c` flag. Counterexamples can
also be presented in dot format.

When the data types that are used in a process are complex, the prover is not
able to prove that the invariant is actually an invariant. This for instance
happens when inequalities are used. In such a case, the flag :option:`-n` can be
used to skip the check that the invariant indeed satisfies the invariant
properties.
