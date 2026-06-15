PBES Instantiation in ``pbesinst``
==================================

*Jeroen Keiren, Maurice Laveaux, Anna Stramaglia, Wieger Wesselink and Tim Willemse*

This document describes instantiation algorithms for PBESs that
are used in the tool ``pbesinst``.

Finite algorithm
----------------

In this section we describe an implementation of the finite instantiation
algorithm :math:`\operatorname{PbesInstFinite}` that eliminates data parameters with
finite sorts. It is implemented in the tool ``pbesinst``. Let
:math:`\mathcal{E} = (\sigma_1 X_1(d_1{:}D_1, e_1{:}E_1) = \varphi_1) \cdots (\sigma_n X_n(d_n{:}D_n, e_n{:}E_n) = \varphi_n)`
be a PBES. We assume that all data sorts :math:`D_i` are finite and all data
sorts :math:`E_i` are infinite. Let :math:`r` be a data rewriter, and let
:math:`\rho` be an injective function that creates a unique predicate variable
from a predicate variable name and a data value according to
:math:`\rho(X(d{:}D, e{:}E), d_0) \rightarrow Y(e{:}E)`,
where :math:`D` is finite and :math:`E` is infinite and :math:`d_0 \in D`.
Note that :math:`D` and :math:`D_i` may be multi-dimensional sorts.

.. math::

   \begin{array}{l}
   \operatorname{PbesInstFinite}(\mathcal{E},\; r,\; \rho) \\
   \mathbf{for}\ i := 1 \cdots n\ \mathbf{do} \\
   \qquad \mathcal{E}_i := \{\sigma_i\,\rho(X_i, d) = R(\varphi_i[d_i := d]) \mid d \in D_i\} \\
   \mathbf{return}\ \mathcal{E}_1 \cdots \mathcal{E}_n
   \end{array}

with :math:`R` a rewriter on PBES expressions that is defined as follows:

.. math::

   \begin{array}{lll}
   R(b) &=& b \\
   R(\lnot\varphi) &=& \lnot R(\varphi) \\
   R(\varphi \oplus \psi) &=& R(\varphi) \oplus R(\psi) \\
   R(X_i(d, e)) &=& \left\{\begin{array}{ll}
     \rho(X_i, r(d))(r(e)) & \text{if } \FV(d) = \emptyset \\[4pt]
     \bigvee_{d_i \in D_i} r(d = d_i) \wedge \rho(X_i, d_i)(r(e)) & \text{if } \FV(d) \neq \emptyset
   \end{array}\right. \\[4pt]
   R(\forall_{d:D}.\varphi) &=& \forall_{d:D}.R(\varphi) \\
   R(\exists_{d:D}.\varphi) &=& \exists_{d:D}.R(\varphi)
   \end{array}

where :math:`\oplus \in \{\vee, \wedge, \Rightarrow\}`, :math:`b` a data expression,
:math:`\varphi` and :math:`\psi` PBES expressions, and :math:`\FV(d)` the set of free
variables appearing in :math:`d`.

Lazy algorithm
--------------

In this section we describe an implementation of the lazy instantiation
algorithm :math:`\operatorname{PbesInstLazy}` that uses instantiation to compute a BES. It
is implemented in the tool ``pbesinst``. It takes two extra parameters,
an injective function :math:`\rho` that renames predicate variables to
proposition variables, and a rewriter :math:`R` that eliminates quantifiers from
predicate formulae. Let
:math:`\mathcal{E} = (\sigma_1 X_1(d_1{:}D_1) = \varphi_1) \cdots (\sigma_n X_n(d_n{:}D_n) = \varphi_n)`
be a PBES, and :math:`X_{init}(e_{init})` an initial state.

.. math::

   \begin{array}{l}
   \operatorname{PbesInstLazy}(\mathcal{E},\; X_{init}(e_{init}),\; R,\; \rho) \\
   \mathbf{for}\ i := 1 \cdots n\ \mathbf{do}\quad \mathcal{E}_i := \epsilon \\
   \mathit{todo} := \{R(X_{init}(e_{init}))\} \\
   \mathit{done} := \emptyset \\
   \mathbf{while}\ \mathit{todo} \neq \emptyset\ \mathbf{do} \\
   \qquad \mathbf{choose}\ X_k(e) \in \mathit{todo} \\
   \qquad \mathit{todo} := \mathit{todo} \setminus \{X_k(e)\} \\
   \qquad \mathit{done} := \mathit{done} \cup \{X_k(e)\} \\
   \qquad X^e := \rho(X_k(e)) \\
   \qquad \psi^e := R(\varphi_k[d_k := e]) \\
   \qquad \mathcal{E}_k := \mathcal{E}_k(\sigma_k X^e = \rho(\psi^e)) \\
   \qquad \mathit{todo} := \mathit{todo} \cup \{Y(f) \in \occ(\psi^e) \mid Y(f) \notin \mathit{done}\} \\
   \mathbf{return}\ \mathcal{E}_1 \cdots \mathcal{E}_n
   \end{array}

where :math:`\rho` is extended from predicate variables to quantifier-free
predicate formulae using

.. math::

   \begin{array}{lll}
   \rho(b) &=& b \\
   \rho(\varphi \oplus \psi) &=& \rho(\varphi) \oplus \rho(\psi)
   \end{array}

Generic lazy algorithms
-----------------------

In this section two generic variants of lazy PBES instantiation are
described that report all discovered BES equations using a callback
function :math:`\operatorname{ReportEquation}`. These versions are later extended to
compute structure graphs.

The first version :math:`\operatorname{PbesInstLazy1}` maintains a collection
:math:`\mathit{done}`, that contains all BES variables for which an equation has
been computed.

.. math::

   \begin{array}{l}
   \operatorname{PbesInstLazy1}(\mathcal{E},\; X_{init}(e_{init}),\; R) \\
   \mathit{init} := R(X_{init}(e_{init})) \\
   \mathit{todo} := \{\mathit{init}\} \\
   \mathit{done} := \emptyset \\
   \mathbf{while}\ \mathit{todo} \neq \emptyset\ \mathbf{do} \\
   \qquad \mathbf{choose}\ X_k(e) \in \mathit{todo} \\
   \qquad \mathit{todo} := \mathit{todo} \setminus \{X_k(e)\} \\
   \qquad \mathit{done} := \mathit{done} \cup \{X_k(e)\} \\
   \qquad \psi^e := R(\varphi_k[d_k := e]) \\
   \qquad \operatorname{ReportEquation}(X_k(e),\; \psi^e) \\
   \qquad \mathit{todo} := \mathit{todo} \cup (\occ(\psi^e) \setminus \mathit{done})
   \end{array}

The second version :math:`\operatorname{PbesInstLazy2}` maintains a set
:math:`\mathit{discovered}` instead of :math:`\mathit{done}`. This set contains BES
variables that have been discovered, but for which the corresponding
equation may not have been computed yet. The sets are related via
:math:`\mathit{done} = \mathit{discovered} \setminus \mathit{todo}`.

.. math::

   \begin{array}{l}
   \operatorname{PbesInstLazy2}(\mathcal{E},\; X_{init}(e_{init}),\; R) \\
   \mathit{init} := R(X_{init}(e_{init})) \\
   \mathit{todo} := \{\mathit{init}\} \\
   \mathit{discovered} := \{\mathit{init}\} \\
   \mathbf{while}\ \mathit{todo} \neq \emptyset\ \mathbf{do} \\
   \qquad \mathbf{choose}\ X_k(e) \in \mathit{todo} \\
   \qquad \mathit{todo} := \mathit{todo} \setminus \{X_k(e)\} \\
   \qquad \psi^e := R(\varphi_k[d_k := e]) \\
   \qquad \operatorname{ReportEquation}(X_k(e),\; \psi^e) \\
   \qquad \mathit{todo} := \mathit{todo} \cup (\occ(\psi^e) \setminus \mathit{discovered}) \\
   \qquad \mathit{discovered} := \mathit{discovered} \cup \occ(\psi^e)
   \end{array}

It turned out that the second version has slightly better performance for
some larger use cases.

To support breadth first and depth first search, the implementation stores
the set :math:`\mathit{todo}` as a double ended queue. New elements are always
appended to :math:`\mathit{todo}`. In the case of breadth first search always the
first element is chosen, and in the case of depth first search the last
element.
