.. _rewriter-implementation-notes:

Rewriter implementation notes
##############################

.. seealso::

   :doc:`Data rewriters <data_rewriters>` — user-facing API for the rewriter
   (``data::rewriter`` class, the Rewriter concept, and how to use rewriters
   from algorithms).

This section provides the mathematical foundations for the :doc:`Rewriter concept
<data_rewriters>` and its operations, based on [weerdenburg2009]_ and [vanderwulp2009]_.

Introduction
============

Higher order rewriting
----------------------

There are several formalisms for higher order rewriting. We choose
higher-order rewriting systems (HRSs) introduced by Nipkow. In [raamsdonk2001]_
HRSs are summarized as follows.

In a HRS we work modulo the :math:`\beta\eta`-relation of simply typed
:math:`\lambda`-calculus. *Types* are built from a non-empty set of base types
and the binary type constructor :math:`\rightarrow` as usual. For every type we
assume a countably infinite set of *variables* of that type, written as
:math:`x, y, z, \ldots`. A *signature* is a non-empty set of typed function
symbols. The set of *preterms* of type :math:`A` over a signature :math:`\Sigma`
consists exactly of the expressions :math:`s` for which we can derive
:math:`s : A` using the following rules:

1. :math:`x : A` for a variable :math:`x` of type :math:`A`,
2. :math:`f : A` for a function symbol :math:`f` of type :math:`A` in :math:`\Sigma`,
3. if :math:`A = A' \rightarrow A''`, and :math:`x : A'` and :math:`s : A''`,
   then :math:`(x.s) : A`,
4. if :math:`s : A' \rightarrow A` and :math:`t : A'`, then :math:`(s\ t) : A`.

The abstraction operator :math:`\_.\_` binds variables, so occurrences of
:math:`x` in :math:`s` in the preterm :math:`x.s` are bound. We work modulo
type-preserving :math:`\alpha`-conversion and assume that bound variables are
renamed whenever necessary in order to avoid unintended capturing of free
variables. Parentheses may be omitted according to the usual conventions. We
make use of the usual notions of *substitution* of a preterm :math:`t` for the
free occurrences of a variable :math:`x` in a preterm :math:`s`, notation
:math:`s[x := t]`, and *replacement in a context*, notation :math:`C[t]`. We
write :math:`s \supseteq s'` if :math:`s'` is a subpreterm of :math:`s`, and
use :math:`\supset` for the strict subpreterm relation.

The :math:`\beta`-*reduction relation*, notation :math:`\rightarrow_\beta`, is
the smallest relation on preterms that is compatible with formation of preterms
and that satisfies the following:

.. math::

   (x.s)\,t \rightarrow_\beta s[x := t]

The *restricted* :math:`\eta`-*expansion relation*, notation
:math:`\rightarrow_{\bar\eta}`, is defined as follows. We have

.. math::

   C[s] \rightarrow_{\bar\eta} C[x.(s\ x)]

if :math:`s : A \rightarrow B`, and :math:`x : A` is a fresh variable, and no
:math:`\beta`-redex is created (hence the terminology restricted
:math:`\eta`-expansion). The latter condition is satisfied if :math:`s` is not
an abstraction (so not of the form :math:`z.s'`), and doesn't occur in
:math:`C[s]` as the left part of an application (so doesn't occur in a
sub-preterm of the form :math:`(s\ s')`).

In the sequel we employ only preterms in :math:`\bar\eta`-normal form, where
every sub-preterm has the right number of arguments. Instead of
:math:`s_0 s_1 \ldots s_m` we often write :math:`s_0(s_1, \ldots, s_m)`. A
preterm is then of the form :math:`x_1 \ldots x_n.s_0(s_1, \ldots, s_m)` with
:math:`s_0(s_1, \ldots, s_m)` of base type and all :math:`s_i` in
:math:`\bar\eta`-normal form.

A *term* is a preterm in :math:`\beta`-normal form. It is also in
:math:`\bar\eta`-normal form because :math:`\bar\eta`-normal forms are closed
under :math:`\beta`-reduction. A term is of the form
:math:`x_1 \ldots x_n.a(s_1, \ldots, s_m)` with :math:`a` a function symbol or
a variable. Because the :math:`\beta\bar\eta`-reduction relation is confluent
and terminating on the set of preterms, every :math:`\beta\bar\eta`-equivalence
class of preterms contains a unique term, which is taken as the representative
of that class.

Because in the discussion we will often use preterms, we use here the notation
:math:`s^\sigma` for the replacement of variables according to the substitution
:math:`\sigma` (*without* reduction to :math:`\beta`-normal form), and write
explicitly :math:`s^\sigma{\downarrow_\beta}` for its :math:`\beta`-normal form.
This is in contrast with the usual notations for HRSs.

A *rewrite rule* is a pair of terms :math:`(l, r)`, written as
:math:`l \rightarrow r`, satisfying the following requirements:

1. :math:`l` and :math:`r` are of the same base type,
2. :math:`l` is of the form :math:`f(l_1, \ldots, l_n)`,
3. all free variables in :math:`r` occur also in :math:`l`,
4. a free variable :math:`x` in :math:`l` occurs in the form
   :math:`x(y_1, \ldots, y_n)` with :math:`y_i` :math:`\eta`-equivalent to
   different bound variables.

The last requirement guarantees that the rewrite relation is decidable because
unification of patterns is decidable. The rewrite rules induce a rewrite
relation :math:`\rightarrow` on the set of terms which is defined by the
following rules:

1. if :math:`s \rightarrow t` then
   :math:`x(\ldots, s, \ldots) \rightarrow x(\ldots, t, \ldots)`,
2. if :math:`s \rightarrow t` then
   :math:`f(\ldots, s, \ldots) \rightarrow f(\ldots, t, \ldots)`,
3. if :math:`s \rightarrow t` then :math:`x.s \rightarrow x.t`,
4. if :math:`l \rightarrow r` is a rewrite rule and :math:`\sigma` is a
   substitution then
   :math:`l^\sigma{\downarrow_\beta} \rightarrow r^\sigma{\downarrow_\beta}`.

The last clause shows that HRSs use higher-order pattern matching, unlike AFSs,
where matching is syntactic.

mCRL2 terms
-----------

In mCRL2 we have the following terms:

.. math::

   t := x \mid f \mid t(t, \cdots, t) \mid \lambda_x.t \mid \forall_x.t \mid \exists_x.t \mid t\ \mathbf{whr}\ x = t'

where :math:`t` is a term, :math:`x` is a variable and :math:`f` is a function
symbol.

.. admonition:: Remark

   This needs to be further elaborated. Terms are typed, and function symbols
   (and terms?) have an arity. The term :math:`t(t, \cdots, t)` is rather
   unusual, but it is covered by HRSs (?).

.. admonition:: Remark

   In fact the mCRL2 language uses slightly more general terms:
   :math:`\lambda_{x_1 \ldots x_n}.t`, :math:`\forall_{x_1 \ldots x_n}.t`,
   :math:`\exists_{x_1 \ldots x_n}.t` and
   :math:`t\ \mathbf{whr}\ x_1 = t_1, \ldots, x_n = t_n`.

For a rewrite algorithm :math:`\mathit{rewr}` the following rules are suggested:

.. math::

   \begin{aligned}
   \mathit{rewr}(\lambda_x.t, \sigma) &= \lambda_{x'}.\mathit{rewr}(t, \sigma[x := x']) \\
   \mathit{rewr}(\forall_x.t, \sigma) &= \forall_{x'}.\mathit{rewr}(t, \sigma[x := x']) \\
   \mathit{rewr}(\exists_x.t, \sigma) &= \exists_{x'}.\mathit{rewr}(t, \sigma[x := x']) \\
   \mathit{rewr}(t\ \mathbf{whr}\ x = t', \sigma) &= \mathit{rewr}(t, \sigma[x := t'])
   \end{aligned}

where :math:`x'` is a fresh variable not appearing in :math:`t`.

.. admonition:: Remark

   In a rewrite algorithm, the term types are unused. One can add correctness
   checks for proper typing however.

.. admonition:: Remark

   Types in mCRL2 need to be rewritten to normal form as well. A very simple
   rewrite system can be defined for this.

.. admonition:: Remark

   What about normal forms for terms containing :math:`\lambda`-expressions
   and/or quantifiers? Expressions can be equal modulo alpha-conversion, so
   ATerm equality doesn't work here.

.. admonition:: Remark

   In a rewrite algorithm one has to explicitly describe where
   :math:`\alpha`-conversion and :math:`\beta`-reduction is being done. Doing
   :math:`\bar\eta`-expansion is probably not necessary.

Types
-----

A *base type* is a non-function type; typical examples are the Booleans or
Natural numbers. Let :math:`B` be a non-empty set of base types and
:math:`b \in B`. The set of types is inductively defined as follows:

.. math::

   \mathit{type} ::= b \mid \mathit{type} \times \mathit{type} \mid \mathit{type} \rightarrow \mathit{type},

where :math:`\rightarrow` is the function-type constructor. The type constructor
associates to the left, for example:

.. math::

   b \rightarrow b \rightarrow b \text{ is the same as } (b \rightarrow b) \rightarrow b.

Product types are often not present in treatment of simply-typed lambda calculus.
We need them to type non-lambda terms.

The *arity* of a type :math:`A` is a natural number, denoted
:math:`\mathit{arity}(A)`, which is inductively defined on the structure of
:math:`A` as follows:

.. math::

   \begin{array}{ll}
   \mathit{arity}(A) = 0 & \text{if } A \text{ is a base type,} \\
   \mathit{arity}(A \rightarrow A') = \mathit{arity}(A') + 1 &
   \end{array}

A *signature* :math:`\Sigma` is a non-empty set of *function symbols* each of
which has a type. We write :math:`f : A` to denote that symbol :math:`f` has
type :math:`A` and extend the notion of arity to symbols such that if
:math:`f : A` then :math:`\mathit{arity}(f) = \mathit{arity}(A)`. Symbols with
arity zero are called *constants*.

Let :math:`\Sigma` be a signature and let :math:`\chi_A` be a countably finite
set of variables of type :math:`A` such that :math:`\Sigma \cap \chi_A = \emptyset`,
for each type :math:`A`. The set of terms over :math:`\Sigma`, denoted
:math:`\mathcal{T}(\Sigma)`, is inductively defined as:

- Let :math:`x \in \chi_A` be a variable of type :math:`A`; then
  :math:`x \in \mathcal{T}(\Sigma)`.
- Let :math:`f \in \Sigma` be a function symbol of type
  :math:`A_1 \times \ldots \times A_n \rightarrow B`, and :math:`t_i : A_i`
  for all :math:`i \in \{1, \ldots, n\}`; then
  :math:`f(t_1, \ldots, t_n) \in \mathcal{T}(\Sigma)` is a term of type :math:`B`.
- Let :math:`t : A_1 \times \ldots \times A_n \rightarrow B`, and
  :math:`t_i : A_i` for all :math:`i \in \{1, \ldots, n\}`; then
  :math:`t(t_1, \ldots, t_n) \in \mathcal{T}(\Sigma)` is a term of type :math:`B`.
- Let :math:`x \in \chi_A` be a variable of type :math:`A` and :math:`t` a term
  of type :math:`B`; then :math:`\lambda_x.(t) \in \mathcal{T}(\Sigma)` is a
  term of type :math:`A \rightarrow B`.

Simple terms
------------

Simple terms are terms with the following syntax:

.. math::

   t := x \mid f \mid f(t, \cdots, t),

where :math:`t` is a term, :math:`x` is a variable and :math:`f` is a function
symbol.

Applicative terms
-----------------

Applicative terms are an extension of simple terms:

.. math::

   t := x \mid f \mid t(t, \cdots, t).

The set of all variables is denoted by :math:`\mathbb{V}`, the set of all
function symbols by :math:`\mathbb{F}` and the set of all terms by
:math:`\mathbb{T}`. In this document we use the convention that
:math:`x, y \in \mathbb{V}`, that :math:`t, u \in \mathbb{T}`, and that
:math:`f, g \in \mathbb{F}`.

We write :math:`\mathit{var}(t)` for the set of variables that occur in
:math:`t`. Formally:

.. math::

   \begin{array}{lll}
   \mathit{var}(x) & = & \{x\} \\
   \mathit{var}(f) & = & \emptyset \\
   \mathit{var}(t(t_1, \cdots, t_n)) & = & \mathit{var}(t) \cup \bigcup_{i=1}^{n} \mathit{var}(t_i)
   \end{array}

Subterms
--------

To facilitate operations on subterms we inductively define positions
(:math:`\mathbb{P}`) as follows. A position is either :math:`\epsilon` (the
empty position) or an index :math:`i` (from :math:`1, 2, \cdots`) combined with
a position :math:`\pi`, notation :math:`i \cdot \pi`. We lift :math:`\cdot` to
an associative operator on positions with :math:`\epsilon` as its unit element
and often write just :math:`i` for the position :math:`i \cdot \epsilon`. We
write the subterm of :math:`t` at position :math:`\pi` as :math:`t|_\pi` and
we write term :math:`t` with the subterm at position :math:`\pi` replaced by
:math:`u` as :math:`t[u]_\pi`. These operations are defined as follows:

.. math::

   \begin{array}{llll}
   t|_\epsilon & = & t & \\
   t(t_1, \cdots, t_n)|_{i \cdot \pi} & = & t_i|_\pi & \text{if } 1 \leq i \leq n \\
   t[u]_\epsilon & = & u & \\
   x[u]_{i \cdot \pi} & = & x & \\
   f(t_1, \cdots, t_n)[u]_{i \cdot \pi} & = & f(t_1, \cdots, t_{i-1}, t_i[u]_\pi, t_{i+1}, \cdots, t_n) & \text{if } i \leq n \\
   f(t_1, \cdots, t_n)[u]_{i \cdot \pi} & = & f(t_1, \cdots, t_n) & \text{if } i > n
   \end{array}

Some examples are:

.. math::

   \begin{array}{l}
   f(x, g(y))|_1 = x \\
   f(x, g(y))|_{2 \cdot 1} = y \\
   f(x, g(y))[h(x)]_2 = f(x, h(x))
   \end{array}

Substitutions
-------------

A substitution is a function :math:`\sigma : \mathbb{V} \rightarrow \mathbb{T}`.
A substitution :math:`\sigma` can also be applied to a term :math:`t`. This is
denoted by :math:`t\sigma` and is defined as:

.. math::

   \begin{array}{lll}
   x\sigma & = & \sigma(x) \\
   f\sigma & = & f \\
   t(t_1, \cdots, t_n)\sigma & = & t\sigma(t_1\sigma, \cdots, t_n\sigma)
   \end{array}

For terms containing λ-abstractions, quantifiers and where clauses, a more refined
notion of capture-avoiding substitution is needed; see the
:doc:`Capture-avoiding substitutions <data_substitutions>` page.

Rewrite rules
-------------

A rewrite rule is a rule :math:`l \rightarrow r\ \mathbf{if}\ c`, with
:math:`l, r, c \in \mathbb{T}`. We put three restrictions on rewrite rules:

.. math::

   \begin{array}{ll}
   1) & l \text{ is a simple term} \\
   2) & l \notin \mathbb{V} \\
   3) & \mathit{var}(r) \cup \mathit{var}(c) \subseteq \mathit{var}(l)
   \end{array}

For a set :math:`R` of rewrite rules we define the rewrite relation
:math:`\rightarrow_R` as follows: :math:`t \rightarrow_R u` if there is a rule
:math:`l \rightarrow r\ \mathbf{if}\ c` in :math:`R`, a position :math:`\pi`
and a substitution :math:`\sigma` such that

.. math::

   t|_\pi = l\sigma \wedge u = t[r\sigma]_\pi \wedge \eta(c\sigma),

where :math:`\eta` is a boolean function that determines if a condition is true.
We write :math:`\rightarrow` instead of :math:`\rightarrow_R` if no confusion
can occur. We write :math:`\rightarrow_R^*` for the reflexive and transitive
closure of :math:`\rightarrow_R` and :math:`t \not\rightarrow_R` if there is no
:math:`u` such that :math:`t \rightarrow_R u`. A normal form is a term :math:`u`
such that :math:`t \rightarrow_R^* u` and :math:`u \not\rightarrow_R`.

Rewrite algorithm
-----------------

We now formulate an abstract rewrite algorithm :math:`\mathit{rewrite}`, where
we assume that :math:`R` is a given, fixed set of rewrite rules:

.. math::

   \begin{array}{l}
   \textbf{function}\ \mathit{rewrite}(t) \\
   u := t \\
   \textbf{while}\ \{v \mid u \rightarrow_R v\} \neq \emptyset\ \textbf{do} \\
   \quad \textbf{choose}\ v\ \textbf{such that}\ u \rightarrow_R v \\
   \quad u := v \\
   \textbf{return}\ u
   \end{array}

Note that this algorithm does not need to terminate. In practice we are also
interested in an algorithm :math:`\mathit{rewrite}(t, \sigma)`, that applies a
substitution :math:`\sigma` to the variables in :math:`t` during rewriting. The
specification of this algorithm is simply

.. math::

   \mathit{rewrite}(t, \sigma) = \mathit{rewrite}(t\sigma).

The reason we are interested in such an algorithm is that it can be implemented
more efficiently than the straightforward solution to first compute
:math:`u = t\sigma` and then compute :math:`\mathit{rewrite}(u)`.

Match trees
===========

A match tree is a tree structure that represents a number of rewrite rules that
have left-hand sides with the same function symbol as head. It is used to
compute all possible results of applying one of these rules to a term. Match
trees are only defined for simple terms. A match tree consists of nodes of the
following types:

- :math:`F(f, T, U)`: If the current term has the form
  :math:`f(t_1, \cdots, t_n)` replace the top of the stack by
  :math:`t_1 \rhd \cdots \rhd t_n` and continue with :math:`T`, otherwise
  continue with :math:`U`.
- :math:`S(x, T)`: Assign the current term to variable :math:`x` and continue
  with :math:`T`.
- :math:`M(x, T, U)`: If the current term is equal to :math:`x` continue with
  :math:`T`, otherwise continue with :math:`U`.
- :math:`R(Q)`: Return :math:`Q`.
- :math:`X`: Return the empty set.
- :math:`N(n, T)`: Remove :math:`n` elements from the stack and continue with
  :math:`T`. We abbreviate :math:`N(1, T)` as :math:`N(T)`.
- :math:`E(T, U)`: If the stack is not empty continue with :math:`T`, otherwise
  continue with :math:`U`.
- :math:`C(t, T, U)`: If :math:`t` evaluates to :math:`\mathit{true}`, continue
  with :math:`T`, otherwise continue with :math:`U`.

where :math:`f` is a function symbol, :math:`x` is a variable, :math:`t` is a
term, :math:`Q` is a set of terms annotated with a rewrite rule, and :math:`T`
and :math:`U` are match tree nodes.

Evaluating a match tree
-----------------------

Let :math:`l` be a sequence of terms, and let :math:`\sigma` be an arbitrary
substitution function. Then the evaluation of a match tree with arguments
:math:`l` and :math:`\sigma` is a set of terms and is defined as follows:

.. math::

   \begin{array}{lll}
   F(f,T,U)(l,\sigma) & = & \begin{cases}
     \emptyset & \text{if } l = [] \\
     T(m, \sigma) & \text{if } l = f \rhd m \\
     T(t_1 \rhd \cdots \rhd t_n \rhd m, \sigma) & \text{if } l = f(t_1, \cdots, t_n) \rhd m \\
     U(l, \sigma) & \text{if } l = g(t_1, \cdots, t_n) \rhd m \wedge f \neq g \\
     U(l, \sigma) & \text{if } l = x \rhd m
   \end{cases} \\[1.5ex]
   X(l, \sigma) & = & \emptyset \\[1.5ex]
   R(Q)(l, \sigma) & = & \begin{cases}
     \{\sigma(t) \mid t^\alpha \in Q\} & \text{if } l = [] \\
     \emptyset & \text{if } l \neq []
   \end{cases} \\[1.5ex]
   S(x,T)(l,\sigma) & = & \begin{cases}
     \emptyset & \text{if } l = [] \\
     T(l, \sigma[x \rightarrow t]) & \text{if } l = t \rhd m
   \end{cases} \\[1.5ex]
   M(x,T,U)(l,\sigma) & = & \begin{cases}
     \emptyset & \text{if } l = [] \\
     T(l, \sigma) & \text{if } l = t \rhd m \wedge \sigma(x) = t \\
     U(l, \sigma) & \text{if } l = t \rhd m \wedge \sigma(x) \neq t
   \end{cases} \\[1.5ex]
   N(n,T)(l,\sigma) & = & \begin{cases}
     \emptyset & \text{if } |l| < n \\
     T(m, \sigma) & \text{if } l = t_1 \rhd \cdots \rhd t_n \rhd m
   \end{cases} \\[1.5ex]
   E(T,U)(l,\sigma) & = & \begin{cases}
     U(l, \sigma) & \text{if } l = [] \\
     T(l, \sigma) & \text{if } l = t \rhd m
   \end{cases} \\[1.5ex]
   C(t,T,U)(l,\sigma) & = & \begin{cases}
     T(l, \sigma) & \text{if } t\sigma \text{ evaluates to } \mathit{true} \\
     U(l, \sigma) & \text{if } t\sigma \text{ does not evaluate to } \mathit{true}
   \end{cases}
   \end{array}

where :math:`T` and :math:`U` are match trees, :math:`f` and :math:`g` are
function symbols, :math:`l` and :math:`m` are sequences of terms and :math:`t`
and :math:`t_i` are terms. The evaluation of a match tree :math:`T` in a single
term :math:`t` with substitution :math:`\sigma` is defined as
:math:`T([t], \sigma)`.

Building a match tree
---------------------

Let :math:`\alpha` be a rewrite rule given by :math:`l \rightarrow r`. Then we
define
:math:`\mathit{match\_tree}(\alpha) = \gamma([l], \{r^\alpha\}, \emptyset)`,
where :math:`\gamma` is defined as:

.. math::

   \begin{array}{lll}
   \gamma([], Q, V) & = & R(Q) \\
   \gamma(x \rhd s, Q, V) & = & \begin{cases}
     S(x, N(\gamma(s, Q, V \cup \{x\}))) & \text{if } x \notin V \\
     M(x, N(\gamma(s, Q, V \cup \{x\})), X) & \text{if } x \in V
   \end{cases} \\[1.5ex]
   \gamma(f(t_1, \cdots, t_n) \rhd s, Q, V) & = & F(f, \gamma(t_1 \rhd \cdots \rhd t_n \rhd s, Q, V), X)
   \end{array}

Match trees are only defined for rewrite rules with simple terms at the
left-hand side.

Joining match trees
-------------------

Two match trees :math:`\mathit{left}` and :math:`\mathit{right}` can be joined
into one using the operator :math:`\|`, defined as follows.
:math:`\mathit{left}\ \|\ \mathit{right} =`

.. math::

   \begin{array}{lllll}
   \mathit{right} & \text{if} & \mathit{head}(\mathit{left}) = X & & \\
   \mathit{left} & \text{if} & \mathit{head}(\mathit{right}) = X & & \\
   E(\mathit{left}, \mathit{right}) & \text{if} & \mathit{head}(\mathit{right}) = R & & \\
   E(\mathit{right}, \mathit{left}) & \text{if} & \mathit{head}(\mathit{left}) = R & & \\
   R(Q \cup Q') & \text{if} & \mathit{left} = R(Q) & \text{and} & \mathit{right} = R(Q') \\
   S(x, T \| \mathit{right}) & \text{if} & \mathit{left} = S(x,T) & \text{and} & \mathit{head}(\mathit{right}) \in \{F,S,U\} \\
   M(y, \mathit{left} \| U, \mathit{left}) & \text{if} & \mathit{left} = S(x,T) & \text{and} & \mathit{right} = M(y,U,V) \\
   M(x, T \| \mathit{right}, T' \| \mathit{right}) & \text{if} & \mathit{left} = M(x,T,T') & \text{and} & \mathit{head}(\mathit{right}) \in \{F,M,N,S\} \\
   S(x, \mathit{left} \| U) & \text{if} & \mathit{left} = F(f,T,T') & \text{and} & \mathit{right} = S(x,U) \\
   M(x, \mathit{left} \| U, \mathit{left}) & \text{if} & \mathit{left} = F(f,T,T') & \text{and} & \mathit{right} = M(x,U,U') \\
   F(f, T \| U, T') & \text{if} & \mathit{left} = F(f,T,T') & \text{and} & \mathit{right} = F(f,U,U') \\
   F(f, T, T' \| \mathit{right}) & \text{if} & \mathit{left} = F(f,T,T') & \text{and} & \mathit{right} = F(g,U,U'),\ f \neq g \\
   F(f, T \| N(\mathit{ar}(f),U), T' \| \mathit{right}) & \text{if} & \mathit{left} = F(f,T,T') & \text{and} & \mathit{right} = N(U) \\
   S(x, \mathit{left} \| U) & \text{if} & \mathit{left} = N(T) & \text{and} & \mathit{right} = S(x,U) \\
   M(x, \mathit{left} \| U, \mathit{left} \| U') & \text{if} & \mathit{left} = N(T) & \text{and} & \mathit{right} = M(x,U,U') \\
   F(f, N(\mathit{ar}(f),T) \| U, \mathit{left}) & \text{if} & \mathit{left} = N(T) & \text{and} & \mathit{right} = F(f,U,X) \\
   N(T \| U) & \text{if} & \mathit{left} = N(T) & \text{and} & \mathit{right} = N(U) \\
   E(T, \mathit{right} \| T') & \text{if} & \mathit{left} = E(T,T') & \text{and} & \mathit{head}(\mathit{right}) \in \{F,M,N,R,S\}
   \end{array}

where :math:`\mathit{head}` is defined as :math:`\mathit{head}(F(f,T,U)) = F`,
:math:`\mathit{head}(R(Q)) = R`, etc.

Optimizing match trees
----------------------

The result of joining match trees is often not optimal. This section gives two
algorithms :math:`\mathit{reduce}` and :math:`\mathit{clean}` to optimize match
trees.

.. math::

   \begin{array}{llll}
   \mathit{reduce}(X) & = & X & \\
   \mathit{reduce}(F(f,T,U)) & = & \mathit{reduce}_F(F(f,T,U), \emptyset) & \\
   \mathit{reduce}(S(x,T)) & = & \mathit{reduce}_S(S(x,T), \emptyset) & \\
   \mathit{reduce}(M(x,T,U)) & = & \mathit{reduce}_M(M(x,T,U), \emptyset, \emptyset) & \\
   \mathit{reduce}(C(t,T,U)) & = & C(t, \mathit{reduce}(T), \mathit{reduce}(U)) & \\
   \mathit{reduce}(N(n,T)) & = & N(n, \mathit{reduce}(T)) & \\
   \mathit{reduce}(E(T,U)) & = & E(t, \mathit{reduce}(T), \mathit{reduce}(U)) & \\
   \mathit{reduce}(R(Q)) & = & R(Q) & \\[1ex]
   \mathit{reduce}_F(X, F) & = & F & \\
   \mathit{reduce}_F(F(f,T,U), F) & = & \mathit{reduce}_F(U, F) & \text{if } f \in F \\
   \mathit{reduce}_F(F(f,T,U), F) & = & F(f, \mathit{reduce}(T), \mathit{reduce}_F(U, F \cup \{f\})) & \text{if } f \notin F \\
   \mathit{reduce}_F(N(n,T)) & = & \mathit{reduce}_M(M(x,T,U), \emptyset, \emptyset) & \\[1ex]
   \mathit{reduce}_S(X, \emptyset) & = & X & \\
   \mathit{reduce}_S(X, \{x\} \cup V) & = & S(x, \mathit{reduce}(X[x/V], \emptyset)) & \\
   \mathit{reduce}_S(F(f,T,U), \emptyset) & = & \mathit{reduce}_F(F(f,T,U), \emptyset) & \\
   \mathit{reduce}_S(F(f,T,U), \{x\} \cup V) & = & S(x, \mathit{reduce}_F(F(f,T,U)[x/V], \emptyset)) & \\
   \mathit{reduce}_S(S(x,T), V) & = & \mathit{reduce}_S(T, V \cup \{x\}) & \\
   \mathit{reduce}_S(N(n,T), \emptyset) & = & \mathit{reduce}(N(n,T), \emptyset) & \\
   \mathit{reduce}_S(N(n,T), \{x\} \cup V) & = & S(x, \mathit{reduce}(N(n,T)[x/V])) & \\[1ex]
   \mathit{reduce}_M(X, M_t, M_f) & = & \mathit{reduce}(X) & \\
   \mathit{reduce}_M(F(f,T,U), M_t, M_f) & = & \mathit{reduce}_F(F(f,T,U), \emptyset) & \\
   \mathit{reduce}_M(S(x,T), M_t, M_f) & = & \mathit{reduce}_S(S(x,T), \emptyset) & \\
   \mathit{reduce}_M(M(x,T,U), M_t, M_f) & = & \mathit{reduce}_M(T, M_t, M_f) & \text{if } x \in M_t \\
   \mathit{reduce}_M(M(x,T,U), M_t, M_f) & = & \mathit{reduce}_M(U, M_t, M_f) & \text{if } x \in M_f \\
   \mathit{reduce}_M(M(x,T,U), M_t, M_f) & = & M(x, \mathit{reduce}_M(T, M_t \cup \{x\}, M_f), & \text{if } x \notin M_t \wedge x \notin M_f \\
    & & \quad \mathit{reduce}_M(U, M_t \cup \{x\}, M_f \cup \{x\})) & \\
   \mathit{reduce}_M(N(n,T)) & = & \mathit{reduce}(N(n,T)) &
   \end{array}

with

.. math::

   \begin{array}{llll}
   X[x/V] & = & X & \\
   F(f,T,U)[x/V] & = & F(f, T[x/V], U[x/V]) & \\
   S(x,T)[y/V] & = & S(x, T[y/(V \setminus \{x\})]) & \\
   M(x,T,U)[y/V] & = & M(y, T[y/V], U[y/V]) & \text{if } x \in V \\
   M(x,T,U)[y/V] & = & M(x, T[y/V], U[y/V]) & \text{if } x \notin V \\
   C(t,T,U)[x/V] & = & C(t[x/y : y \in V], T[x/V], U[x/V]) & \\
   N(n,T)[x/V] & = & N(n, T[x/V]) & \\
   E(T,U)[x/V] & = & E(t, T[x/V], U[x/V]) & \\
   R(Q)[x/V] & = & R(Q[x/y : y \in V]) &
   \end{array}

The algorithm :math:`\mathit{clean}(T) = T'` if
:math:`\chi(T) = \langle T', V \rangle`, where :math:`\chi` is defined as:

.. math::

   \begin{array}{llll}
   \chi(X) & = & \langle X, \emptyset \rangle & \\
   \chi(F(f,T,U)) & = & \langle F(f,T',U'), V \cup W \rangle & \text{if } \chi(T) = \langle T',V \rangle \wedge \chi(U) = \langle U',W \rangle \\
   \chi(S(x,T)) & = & \langle S(x,T'), V \setminus \{x\} \rangle & \text{if } \chi(T) = \langle T',V \rangle \wedge x \in V \\
   \chi(S(x,T)) & = & \langle T', V \rangle & \text{if } \chi(T) = \langle T',V \rangle \wedge x \notin V \\
   \chi(M(x,T,U)) & = & \langle T', V \rangle & \text{if } \chi(T) = \langle T',V \rangle \wedge \chi(U) = \langle U',W \rangle \wedge T' = U' \\
   \chi(M(x,T,U)) & = & \langle M(x,T',U'), V \cup W \cup \{x\} \rangle & \text{if } \chi(T) = \langle T',V \rangle \wedge \chi(U) = \langle U',W \rangle \wedge T' \neq U' \\
   \chi(C(t,T,U)) & = & \langle T', V \rangle & \text{if } \chi(T) = \langle T',V \rangle \wedge \chi(U) = \langle U',W \rangle \wedge T' = U' \\
   \chi(C(t,T,U)) & = & \langle C(t,T',U'), V \cup W \cup \mathit{var}(t) \rangle & \text{if } \chi(T) = \langle T',V \rangle \wedge \chi(U) = \langle U',W \rangle \wedge T' \neq U' \\
   \chi(N(n,T)) & = & \langle N(n,T'), V \rangle & \text{if } \langle T',V \rangle = \chi(T) \\
   \chi(E(T,U)) & = & \langle T', V \rangle & \text{if } \chi(T) = \langle T',V \rangle \wedge \chi(U) = \langle X,W \rangle \\
   \chi(E(T,U)) & = & \langle U', W \rangle & \text{if } \chi(T) = \langle X,V \rangle \wedge \chi(U) = \langle U',W \rangle \\
   \chi(E(T,U)) & = & \langle E(T',U'), V \cup W \rangle & \text{if } \chi(T) = \langle T',V \rangle \wedge \chi(U) = \langle U',W \rangle \wedge T' \neq X \wedge U' \neq X \\
   \chi(R(Q)) & = & \langle R(Q), \mathit{var}(Q) \rangle &
   \end{array}

Prioritized rewrite rules
-------------------------

By adding priorities to rewrite rules, the selection of rewrite rules considered
for a term can be reduced. We model priorities of rewrite rules using a function
:math:`\varphi`, that returns the rules of highest priority for a set of rules.
So :math:`\varphi(R) \subseteq R` and :math:`\varphi(R) = \emptyset` if and only
if :math:`R = \emptyset`. We define a function :math:`\mathit{prior}` that
applies a priority function :math:`\varphi` to a match tree:

.. math::

   \begin{array}{lll}
   \mathit{prior}(X, \varphi) & = & X \\
   \mathit{prior}(F(f,T,U), \varphi) & = & F(f, \mathit{prior}(T,\varphi), \mathit{prior}(U,\varphi)) \\
   \mathit{prior}(S(x,T), \varphi) & = & S(x, \mathit{prior}(T,\varphi)) \\
   \mathit{prior}(M(x,T,U), \varphi) & = & M(x, \mathit{prior}(T,\varphi), \mathit{prior}(U,\varphi)) \\
   \mathit{prior}(C(t,T,U), \varphi) & = & C(t, \mathit{prior}(T,\varphi), \mathit{prior}(U,\varphi)) \\
   \mathit{prior}(N(n,T), \varphi) & = & N(n, \mathit{prior}(T,\varphi)) \\
   \mathit{prior}(R(Q), \varphi) & = & R(\varphi(Q)) \\
   \mathit{prior}(E(T,U), \varphi) & = & E(\mathit{prior}(T,\varphi), \mathit{prior}(U,\varphi))
   \end{array}

The effect of applying :math:`\mathit{prior}` to a match tree is that the
:math:`R`-nodes will contain fewer elements. This can be useful to remove
unwanted results. Consider for example the rewrite system

.. math::

   \left\{ \begin{array}{ccc}
   x = x & \rightarrow & \mathit{true} \\
   x = y & \rightarrow & \mathit{false}
   \end{array} \right.

This system can have both :math:`\mathit{true}` and :math:`\mathit{false}` as a
result of rewriting the term :math:`\mathit{true} = \mathit{true}`. But if we
give the first equation a higher priority than the second, the undesired
derivation
:math:`\mathit{true} = \mathit{true} \rightarrow \mathit{false}` is eliminated.

Rewriting
=========

In this section we describe rewriting strategies. For the moment we only
consider innermost rewriting.

Rewriting using match trees
---------------------------

Suppose that we have a rewrite system, and that for each function symbol
:math:`f` a match tree :math:`M_f` has been constructed that corresponds to
rewrite rules with head symbol :math:`f`. We define the function
:math:`\mathit{rewr}_M` as:

.. math::

   \begin{array}{lll}
   \mathit{rewr}_M(x, \sigma) & = & \sigma(x) \\
   \mathit{rewr}_M(f, \sigma) & = & \begin{cases}
     f & \text{if } M_f([f], \sigma) = \emptyset \\
     u \in M_f([f], \sigma) & \text{if } M_f([f], \sigma) \neq \emptyset
   \end{cases} \\[1.5ex]
   \mathit{rewr}_M(f(t_1, \cdots, t_n), \sigma) & = & \begin{cases}
     f(t_1, \cdots, t_n) & \text{if } M_f([f(t_1, \cdots, t_n)], \sigma) = \emptyset \\
     u \in M_f([f(t_1, \cdots, t_n)], \sigma) & \text{if } M_f([f(t_1, \cdots, t_n)], \sigma) \neq \emptyset
   \end{cases} \\[1.5ex]
   \mathit{rewr}_M(x(t_1, \cdots, t_n), \sigma) & = & x(t_1, \cdots, t_n) \\
   \mathit{rewr}_M(u(u_1, \cdots, u_m)(t_1, \cdots, t_n), \sigma) & = & u(u_1, \cdots, u_m)(t_1, \cdots, t_n)
   \end{array}

Innermost rewriting
-------------------

We now define an algorithm :math:`\mathit{rewr}_I` for innermost rewriting. It
is defined for applicative terms. We assume that :math:`\sigma(x)` is always in
normal form already.

.. math::

   \begin{array}{lll}
   \mathit{rewr}_I(x, \sigma) & = & \sigma(x) \\
   \mathit{rewr}_I(f, \sigma) & = & \mathit{rewr}_M(f, \sigma) \\
   \mathit{rewr}_I(t(t_1, \cdots, t_n), \sigma) & = & \mathit{rewr}_M(\mathit{rewr}_I(t,\sigma)(\mathit{rewr}_I(t_1,\sigma), \cdots, \mathit{rewr}_I(t_n,\sigma)), \sigma)
   \end{array}

Further work
============

- Extend the definition of terms with lambda expressions and quantifier
  expressions, and extend the algorithms so they can handle them.
- Design an algorithm for rewriting using strategies as defined in [weerdenburg2009]_.
- Extend the rewrite algorithms so they handle evaluation of conditions (as
  required in the evaluation of a :math:`C`-node).
- Extend the rewrite algorithms for rewrite rules with more general left-hand
  sides.
- Collect examples of higher-order rewrite systems for testing the algorithms.

.. rubric:: References

.. [weerdenburg2009] M.J. van Weerdenburg. *Efficient Rewriting Techniques*. PhD thesis, 2009.

.. [vanderwulp2009] J. van der Wulp. *Notes for the design of a reusable higher-order conditional rewriting library*. 2009.

.. [raamsdonk2001] F. van Raamsdonk. On Termination of Higher-Order Rewriting. *RTA 2001*, Lecture Notes in Computer Science, vol. 2051, pp. 261–275. Springer, 2001.
