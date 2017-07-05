.. index:: lpsconfcheck

.. _tool-lpsconfcheck:

lpsconfcheck
============

Given an LPS:

.. math::

   \begin{array}{lll}
   P(d:D) &=& \ldots + \sum_{e_i:E_i} c_i(d,e_i) \to a_i(f_i(d,e_i)) \cdot P(g_i(d,e_i)) +\\
   && \ldots + \sum_{e_j:E_j} c_j(d,e_j) \to \tau \cdot P(g_j(d,e_j)) + \ldots
   \end{array}

:math:`tau`-summand :math:`j` is confluent with summand :math:`i` if the
following condition holds:

.. math::

   \begin{array}{l}
   \forall d{:}D.\forall e_i{:}E_i. \forall e_j{:}E_j . (inv(d) \land c_i(d,e_i) \land c_j(d,e_j))  \Rightarrow\\
   (c_i(g_j(d,e_j),e_i) \land c_j(g_i(d,e_i),e_j) \land\\
   f_i(d,e_i) = f_i(g_j(d,e_j),e_i) \land g_i(g_j(d,e_j),e_i) = g_j(g_i(d,e_i),e_j))
   \end{array}

where :math:`inv` is the invariant specified using the option
:option:`--invariant`. In case :math:`a_i` is also a :math:`tau`-action, this
formula can be weakened to the following:

.. math::

   \begin{array}{l}
   \forall d{:}D. \forall e_i{:}E_i. \forall e_j{:}E_j . (inv(d) \land c_i(d,e_i) \land c_j(d,e_j)) \Rightarrow\\
   (g_i(d,e_i) = g_j(d,e_j) \lor (c_i(g_j(d,e_j),e_i) \land c_j(g_i(d, e_i),e_j) \land\\
   g_i(g_j(d,e_j),e_i) = g_j(g_i(d,e_i),e_j)))
   \end{array}

If the option :option`--invariant` is not used, the invariant is equal to
``true``.

The tool will generate these confluence conditions for all :math:`tau`-summands and
tries to prove that they are tautologies using a BDD based prover for
propositional formulas. In some cases :ref:`tool-lpsconfcheck` indicates that
a :math:`tau`-summand is not confluent even though it is. The option
:option:`--verbose` gives insight into what the prover is doing and can be used
to see if rewrite rules have to be added to the specification in order to enable
the prover to determine that certain condition are indeed tautologies.

In some cases it may be useful to use an SMT solver to assist the prover. The
SMT solver can further reduce BDDs by removing inconsistent paths. A specific
SMT solver can be chosen using the option :option:`--smt-solver=SOLVER``. Either
the SMT solver `Ario <http://www.eecs.umich.edu/~ario>`_ or `CVC3
<http://www.cs.nyu.edu/acsys/cvc3>`_ can be used. To use one of these solvers,
the directory containing the corresponding executable must be in the path.

The tool can determine whether two summands are confluent in three ways and will
indicate which of the methods was used while proving confluence. The three ways
of determining confluence are as follows: 

* If summand number 1 has been proven confluent with summand number 2, summand
  number 2 is also confluent with summand number 1. This method of checking
  confluence is called checking confluence by symmetry. If two summands are
  confluent by symmetry, :ref:`tool-lpsconfcheck` indicates this by printing
  a dot ('.').
* Another way of checking the confluence of two summands is determining whether
  the two summands are syntactically disjoint. Two summands are syntactically
  disjoint if the following holds: 
  
  * The set of variables used by one summand is disjoint from the set of
    variables changed by the other summand and vice versa.
  * The set of variables changed by one summand is disjoint from the set of
    variables changed by the other summand.
    
  If two summands are confluent because of syntactic disjointness,
  :ref:`tool-lpsconfcheck` indicates this by printing a colon (':').

* The most time consuming way of checking the confluence of two summands is
  generating the confluence condition and then checking if this condition is a
  tautology using the prover. If two summands are proven confluent using the
  prover, :ref:`tool-lpsconfcheck` indicates this by printing a plus sign
  ('+'). If the option :option:`--generate-invariants` is used, the
  tool will try to prove that the reduced confluence
  condition is an invariant of the LPS, in case the confluence condition is not
  a tautology. If the reduced confluence condition is indeed an invariant, the
  two summands are proven confluent :ref:`tool-lpsconfcheck` indicates this
  by printing an 'i'.

If there already is an action named ''ctau'' present in the LPS as found in
''INFILE'', an error will be reported.
