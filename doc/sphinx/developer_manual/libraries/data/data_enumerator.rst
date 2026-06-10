Data enumerator
################

Formal definition
-----------------

Let :math:`\mathit{range}(e)` denote the set of all values that data expression
:math:`e` can attain.

.. admonition:: Definition (Enumerable sort)

   A sort :math:`S` is *enumerable* iff there exists a function
   :math:`\mathit{enum}_S` that maps an arbitrary data expression :math:`e` of
   sort :math:`S` to a finite set of closed data expressions
   :math:`\{e_1, \ldots, e_k\}` such that

   .. math:: \mathit{range}(e) = \bigcup_{i=1}^{k} \mathit{range}(e_i).

By applying :math:`\mathit{enum}_S` repeatedly to the non-closed sub-results,
a tree expansion is obtained whose leaves form a finite representation of
:math:`e`. For sorts that are finite (see `Finiteness of sorts
<data_library.html#finiteness-of-sorts>`_), this tree is always finite.

Algorithm
---------

Given an expression :math:`\varphi` of type :math:`T` and a list of data
variables :math:`v`, the enumerator algorithm iteratively reports expressions
:math:`[\varphi_0, \varphi_1, \ldots]` that can be obtained from :math:`\varphi`
by assigning constant values to the variables in :math:`v`.

Let :math:`R` be a rewriter on expressions of type :math:`T`, :math:`r` a
rewriter on data expressions, and :math:`\sigma` a substitution on data
variables applied during rewriting with :math:`R`. Let :math:`P` be a queue of
pairs :math:`\langle v, \varphi \rangle`, with :math:`v` a non-empty list of
variables and :math:`\varphi` an expression. The function
:math:`\mathit{report\_solution}` is a user-supplied callback; whenever it
returns true the while loop is interrupted. The predicate
:math:`\mathit{reject}` discards an expression so that it does not enter the
queue :math:`P`. The predicate :math:`\mathit{accept}` accepts an expression
as a solution even when it still has a non-empty variable list. By default
both :math:`\mathit{reject}` and :math:`\mathit{accept}` always return false.
The :math:`\mathit{reject}` predicate is not merely cosmetic: termination of
the enumeration may depend on it. Enumeration is often used to find solutions
of boolean predicates; in that case :math:`\mathit{reject}` typically rejects
:math:`\mathit{false}` and :math:`\mathit{accept}` accepts :math:`\mathit{true}`,
or vice versa.

The :math:`\mathrm{is\_finite}` case in the algorithm applies to finite function
sorts and finite sets; all elements of such sorts can be obtained via the
function :math:`\mathrm{values}`. For each sort :math:`s` a non-empty set of
constructor functions :math:`\mathrm{constructors}(s)` is assumed to be defined.

.. math::

   \begin{array}{l}
   \textsc{Enumerate}(P, R, r, \sigma, \mathit{report\_solution}, \mathit{reject}, \mathit{accept}) \\
   \textbf{while}\ P\neq \emptyset\ \textbf{do} \\
   \quad \textbf{let}\ \langle v,\varphi \rangle = \mathrm{head}(P)\ \textbf{with}\ v = [v_1, \ldots, v_n] \\
   \quad \textbf{if}\ v = []\ \textbf{then} \\
   \quad\quad \varphi' := R(\varphi, \sigma) \\
   \quad\quad \textbf{if}\ \mathit{reject}(\varphi')\ \textbf{then}\ \textbf{skip} \\
   \quad\quad \textbf{else if}\ \mathit{report\_solution}(\varphi')\ \textbf{then}\ \textbf{break} \\
   \quad \textbf{else if}\ \mathit{reject}(\varphi)\ \textbf{then} \\
   \quad\quad \textbf{skip} \\
   \quad \textbf{else if}\ \mathrm{is\_finite}(\mathrm{sort}(v_1))\ \textbf{then} \\
   \quad\quad \textbf{for}\ e \in \mathrm{values}(\mathrm{sort}(v_1))\ \textbf{do} \\
   \quad\quad\quad \varphi' := R(\varphi, \sigma[v_1 := e]) \\
   \quad\quad\quad \textbf{if}\ \mathit{reject}(\varphi')\ \textbf{then} \\
   \quad\quad\quad\quad \textbf{skip} \\
   \quad\quad\quad \textbf{else if}\ \mathrm{tail}(v) = []\ \vee\ \mathit{accept}(\varphi')\ \textbf{then} \\
   \quad\quad\quad\quad \textbf{if}\ \mathit{report\_solution}(\varphi)\ \textbf{then}\ \textbf{break} \\
   \quad\quad\quad \textbf{else} \\
   \quad\quad\quad\quad P := P \concat [\langle \mathrm{tail}(v), \varphi' \rangle] \\
   \quad \textbf{else} \\
   \quad\quad \textbf{for}\ c \in \mathrm{constructors}(\mathrm{sort}(v_1))\ \textbf{do} \\
   \quad\quad\quad \textbf{let}\ c : D_1 \times \ldots \times D_m \rightarrow \mathrm{sort}(v_1) \\
   \quad\quad\quad \textbf{choose}\ y_1, \ldots, y_m\ \textbf{such that}\ y_i \notin \{v_1, \ldots, v_n\} \cup FV(\varphi),\ i=1, \ldots, m \\
   \quad\quad\quad \varphi' := R(\varphi, \sigma[v_1 := r(c(y_1, \ldots, y_m))]) \\
   \quad\quad\quad \textbf{if}\ \mathit{reject}(\varphi')\ \textbf{then} \\
   \quad\quad\quad\quad \textbf{skip} \\
   \quad\quad\quad \textbf{else if}\ \mathit{accept}(\varphi')\ \vee\ (\mathrm{tail}(v) = []\ \wedge\ (\varphi = \varphi'\ \vee\ [y_1, \ldots, y_m] = []))\ \textbf{then} \\
   \quad\quad\quad\quad \textbf{if}\ \mathit{report\_solution}(\varphi)\ \textbf{then}\ \textbf{break} \\
   \quad\quad\quad \textbf{else} \\
   \quad\quad\quad\quad \textbf{if}\ \varphi = \varphi'\ \textbf{then}\ P := P \concat [\langle \mathrm{tail}(v), \varphi' \rangle] \\
   \quad\quad\quad\quad \textbf{else}\ P := P \concat [\langle \mathrm{tail}(v) \concat [y_1, \ldots, y_m], \varphi' \rangle] \\
   \quad P := \mathrm{tail}(P)
   \end{array}

.. admonition:: Remarks

   * The algorithm works both for data expressions and PBES expressions.
   * In the case of data expressions, :math:`R` and :math:`r` may coincide.
   * The algorithm can be extended to also return the assignments corresponding to
     a solution.
