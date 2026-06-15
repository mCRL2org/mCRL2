Stategraph
==========

This section describes the implementation of the tool ``pbesstategraph``.

Definitions
-----------

We denote the number of predicate variable instances occurring in a predicate
formula :math:`\varphi` by :math:`\mathrm{npred}(\varphi)`. We assume that
predicate variable instances in :math:`\varphi` are assigned a unique natural
number between :math:`1` and :math:`\mathrm{npred}(\varphi)`.

.. definition::

   Let :math:`\varphi` be a predicate formula and let :math:`i` be between
   :math:`1` and :math:`\mathrm{npred}(\varphi)`. The functions
   :math:`\mathrm{pred}(\varphi,i)`, :math:`\mathrm{data}(\varphi,i)` and
   :math:`\mathrm{PVI}(\varphi,i)` are such that the predicate variable
   instance :math:`\mathrm{PVI}(\varphi,i)` is the :math:`i`-th predicate
   variable instance in :math:`\varphi`, syntactically present as
   :math:`\mathrm{pred}(\varphi,i)(\mathrm{data}(\varphi,i))`. Let
   :math:`\psi` be a predicate formula. We write :math:`\varphi[i \rightarrow \psi]`
   to indicate that the predicate variable instance at position :math:`i` is
   replaced syntactically by :math:`\psi` in :math:`\varphi`.

.. definition::

   Let :math:`\varphi` be a predicate formula. We define the guard of
   predicate variable instantiation :math:`\mathrm{PVI}(\varphi,i)` for
   :math:`i \leq \mathrm{npred}(\varphi)` inductively as follows:

.. math::

   \begin{array}{lll}
   guard^i(c) &=& \mathit{false} \\
   guard^i(Y) &=& \mathit{true} \\
   guard^i(\forall d{:}D.\,\varphi) &=& guard^i(\varphi) \\
   guard^i(\exists d{:}D.\,\varphi) &=& guard^i(\varphi) \\[4pt]
   guard^i(\varphi \wedge \psi) &=& \left\{\begin{array}{lll}
     s(\varphi) \wedge guard^{i - \mathrm{npred}(\varphi)}(\psi) &&
       \text{if } i > \mathrm{npred}(\varphi) \\
     s(\psi) \wedge guard^i(\varphi) &&
       \text{if } i \leq \mathrm{npred}(\varphi)
   \end{array}\right. \\[4pt]
   guard^i(\varphi \vee \psi) &=& \left\{\begin{array}{lll}
     n(\varphi) \wedge guard^i(\psi) &&
       \text{if } i > \mathrm{npred}(\varphi) \\
     ns(\psi) \wedge guard^i(\varphi) &&
       \text{if } i \leq \mathrm{npred}(\varphi)
   \end{array}\right.
   \end{array}

where

.. math::

   \begin{array}{lll}
   s(\varphi) &=& \left\{\begin{array}{lll}
     \varphi && \text{if } \mathrm{npred}(\varphi) = 0 \\
     \mathit{true} && \text{otherwise}
   \end{array}\right. \\[8pt]
   ns(\varphi) &=& \left\{\begin{array}{lll}
     \lnot\varphi && \text{if } \mathrm{npred}(\varphi) = 0 \\
     \mathit{true} && \text{otherwise}
   \end{array}\right.
   \end{array}

We define the function :math:`sig` for computing significant variables
recursively as follows:

.. math::

   \begin{array}{lll}
   sig(b) &=& FV(b) \\
   sig(\varphi \wedge \psi) &=& sig(\varphi) \cup sig(\psi) \\
   sig(\varphi \vee \psi) &=& sig(\varphi) \cup sig(\psi) \\
   sig(X(e)) &=& \emptyset \\
   sig(\exists d{:}D.\,\varphi) &=& sig(\varphi) \setminus \{d\} \\
   sig(\forall d{:}D.\,\varphi) &=& sig(\varphi) \setminus \{d\} \\
   sig(\varphi \Rightarrow \psi) &=& sig(\varphi) \cup sig(\psi) \\
   sig(\lnot\varphi) &=& sig(\varphi)
   \end{array}

The functions source, target and copy
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`X(d{:}D) = \varphi` be a PBES equation. Let
:math:`\mathrm{source}` be a function with the property that

.. math::

   \mathrm{source}(X,i,j) = \left\{\begin{array}{lll}
   e && \text{if } guard^i(\mathrm{PVI}(\varphi_X,i)) \Rightarrow d[j] \approx e \\
   \bot && \text{otherwise}
   \end{array}\right.

A possible heuristic for obtaining a source function is to look for positive
occurrences of constraints of the form :math:`d[j] \approx e` in the guards;
these can be used to define the source function. Let
:math:`\mathrm{sigma}(X,i)` be the substitution defined as

.. math::

   \mathrm{sigma}(X,i)(v) = \left\{\begin{array}{lll}
   e && \text{if } \mathrm{source}(X,i,j) = e \text{ for some } j \\
   v && \text{otherwise}
   \end{array}\right.

We define the function :math:`\mathrm{target}` as follows:

.. math::

   \mathrm{target}(X,i,j) = \left\{\begin{array}{lll}
   c && \text{if } rewrite(\mathrm{sigma}(X,i)(\mathrm{PVI}(\varphi,i))[j]) = c \\
   \bot && \text{otherwise}
   \end{array}\right.

with :math:`c` a constant. We define the function :math:`\mathrm{copy}` as
follows:

.. math::

   \mathrm{copy}(X,i,j) = \left\{\begin{array}{lll}
   k && \text{if } \mathrm{PVI}(\varphi,i)[k] = d[j] \\
   \bot && \text{otherwise}
   \end{array}\right.

We define the function :math:`\mathrm{used}` as follows:

.. math::

   \mathrm{used}(X,i,j) = d_X[j] \in FV(guard^i(\mathrm{PVI}(\varphi_X,i)))

We define the function :math:`\mathrm{changed}` as follows:

.. math::

   \mathrm{changed}(X,i,j) =
   \mathrm{pred}(\varphi_X,i) = X \wedge d_X[j] \neq \mathrm{data}(\varphi_X,i)[j]

Let :math:`par(X)` be the set of parameters of the equation corresponding to
:math:`X`. Let :math:`pos(X,i)` denote the :math:`i`-th parameter of the
equation corresponding to :math:`X`.

Control flow parameters
-----------------------

Control flow parameters are computed in phases. First the function
:math:`LCFP` is computed, then the function :math:`GCFP`, and finally they
are related using :math:`\sim`.

LCFP computation
~~~~~~~~~~~~~~~~

There are two versions of the computation of :math:`LCFP`.

.. math::

   \begin{array}{l}
   \operatorname{ComputeLocalControlflowParametersDefault}(\mathcal{E}) \\
   \mathbf{for}\ X \in bnd(\mathcal{E})\ \mathbf{do} \\
   \quad \mathbf{for}\ n = 1, \ldots, |par(X)|\ \mathbf{do} \\
   \quad\quad LCFP(X,n) := \mathbf{true} \\
   \mathbf{for}\ X \in bnd(\mathcal{E})\ \mathbf{do} \\
   \quad \mathbf{for}\ i = 1, \ldots, npred(\varphi_X)\ \mathbf{do} \\
   \quad\quad \mathbf{if}\ pred(\varphi_X,i) = X\ \mathbf{then} \\
   \quad\quad\quad \mathbf{for}\ n = 1, \ldots, |par(X)|\ \mathbf{do} \\
   \quad\quad\quad\quad \mathbf{if}\ source(X,i,n) = \bot\ \wedge
     \mathrm{target}(X,i,n) = \bot \wedge \mathrm{copy}(X,i,n) \neq n\ \mathbf{then} \\
   \quad\quad\quad\quad\quad LCFP(X,n) := \mathbf{false} \\
   \mathbf{return}\ LCFP
   \end{array}

.. math::

   \begin{array}{l}
   \operatorname{ComputeLocalControlflowParametersAlternative}(\mathcal{E}) \\
   \mathbf{for}\ X \in bnd(\mathcal{E})\ \mathbf{do} \\
   \quad \mathbf{for}\ n = 1, \ldots, |par(X)|\ \mathbf{do} \\
   \quad\quad LCFP(X,n) := \mathbf{true} \\
   \mathbf{for}\ X \in bnd(\mathcal{E})\ \mathbf{do} \\
   \quad \mathbf{for}\ i = 1, \ldots, npred(\varphi_X)\ \mathbf{do} \\
   \quad\quad \mathbf{if}\ pred(\varphi_X,i) = X\ \mathbf{then} \\
   \quad\quad\quad \mathbf{for}\ n = 1, \ldots, |par(X)|\ \mathbf{do} \\
   \quad\quad\quad\quad \mathbf{if}\ (\mathrm{source}(X,i,n) = \bot\ \wedge
     \mathrm{target}(X,i,n) = \bot \wedge \mathrm{copy}(X,i,n) \neq \bot) \\
   \quad\quad\quad\quad \mathbf{or}\ (\mathrm{source}(X,i,n) \neq \bot\ \wedge
     \mathrm{target}(X,i,n) \neq \bot \wedge \mathrm{copy}(X,i,n) = \bot) \\
   \quad\quad\quad\quad \mathbf{then} \\
   \quad\quad\quad\quad\quad LCFP(X,n) := \mathbf{false} \\
   \mathbf{return}\ LCFP
   \end{array}

GCFP computation
~~~~~~~~~~~~~~~~

.. math::

   \begin{array}{l}
   \operatorname{ComputeGlobalControlflowParameters}(\mathcal{E},\, LCFP) \\
   \mathbf{for}\ X \in bnd(\mathcal{E})\ \mathbf{do} \\
   \quad \mathbf{for}\ n = 1, \ldots, |par(X)|\ \mathbf{do} \\
   \quad\quad GCFP(X,n) := LCFP(X,n) \\
   \mathbf{for}\ X \in bnd(\mathcal{E})\ \mathbf{do} \\
   \quad \mathbf{for}\ i = 1, \ldots, npred(\varphi_X)\ \mathbf{do} \\
   \quad\quad \mathbf{let}\ Y = pred(\varphi_X,i) \\
   \quad\quad \mathbf{if}\ Y \neq X\ \mathbf{then} \\
   \quad\quad\quad \mathbf{for}\ n = 1, \ldots, |par(X)|\ \mathbf{do} \\
   \quad\quad\quad\quad \mathbf{if}\ \mathrm{target}(X,i,n) = \bot \wedge
     \forall m{:}\, \mathrm{copy}(X,i,m) \neq n\ \mathbf{then} \\
   \quad\quad\quad\quad\quad GCFP(X,n) := \mathbf{false} \\
   \mathbf{return}\ GCFP
   \end{array}

Related control flow parameters
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

GCFP parameters can be related using the relation :math:`\sim`. There are two
versions of the computation of :math:`\sim`.

.. math::

   \begin{array}{l}
   \operatorname{ComputeRelatedGlobalControlflowParametersDefault}(\mathcal{E},\, GCFP) \\
   \mathbf{for}\ X \in bnd(\mathcal{E})\ \mathbf{do} \\
   \quad \mathbf{for}\ i = 1, \ldots, npred(\varphi_X)\ \mathbf{do} \\
   \quad\quad \mathbf{let}\ Y = pred(\varphi_X,i) \\
   \quad\quad \mathbf{for}\ n = 1, \ldots, |par(X)|\ \mathbf{do} \\
   \quad\quad\quad \mathbf{if}\ \mathrm{copy}(X,i,n) = m \neq \bot\ \mathbf{then} \\
   \quad\quad\quad\quad \mathbf{if}\ GCFP(X,n) \wedge GCFP(Y,m)\ \mathbf{then}\
     (X,n) \sim (Y,m)
   \end{array}

.. math::

   \begin{array}{l}
   \operatorname{ComputeRelatedGlobalControlflowParametersAlternative}(\mathcal{E},\, GCFP) \\
   \mathbf{for}\ X \in bnd(\mathcal{E})\ \mathbf{do} \\
   \quad \mathbf{for}\ i = 1, \ldots, npred(\varphi_X)\ \mathbf{do} \\
   \quad\quad \mathbf{let}\ Y = pred(\varphi_X,i) \\
   \quad\quad \mathbf{for}\ n = 1, \ldots, |par(X)|\ \mathbf{do} \\
   \quad\quad\quad \mathbf{if}\ \mathrm{copy}(X,i,n) = m \neq \bot\ \mathbf{then} \\
   \quad\quad\quad\quad \mathbf{if}\ GCFP(X,n) \wedge GCFP(Y,m) \wedge
     \mathrm{target}(X,i,m) = \bot\ \mathbf{then}\ (X,n) \sim (Y,m)
   \end{array}

Control flow graphs
~~~~~~~~~~~~~~~~~~~

The symmetric closure :math:`{\sim}_S` of the relation :math:`\sim` defines
an undirected graph :math:`(V, {\sim}_S)` on the set of vertices
:math:`V = \{(X,i) \mid X \in bnd(\mathcal{E}) \wedge 1 \leq i \leq |d_X|\}`.
This graph is called the global control flow graph. The connected components in
this graph are the local control flow graphs. A local control flow graph is
called *invalid* if it contains two vertices :math:`(X,i)` and :math:`(X,j)`
with :math:`i \neq j`.

Global algorithm
----------------

Global control flow graph
~~~~~~~~~~~~~~~~~~~~~~~~~

The following algorithm computes the global control flow graph.

.. math::

   \begin{array}{l}
   \operatorname{ComputeGlobalControlflowGraph}(\mathcal{E},\; X_{init}(e_{init})) \\
   V := \emptyset \\
   E := \emptyset \\
   \mathit{todo} := \{(X_{init}, e) \mid \forall k \leq |c_X|{:}\; e[k] = e_{init}[{\downarrow_X k}]\} \\
   \mathbf{while}\ \mathit{todo} \neq \emptyset\ \mathbf{do} \\
   \quad \mathbf{choose}\ u \in \mathit{todo} \\
   \quad \mathit{todo} := \mathit{todo} \setminus \{u\} \\
   \quad V := V \cup \{u\} \\
   \quad \mathbf{for}\ i = 1 \cdots \mathrm{npred}(\varphi)\ \mathbf{do} \\
   \quad\quad \mathbf{if}\ \operatorname{EnabledEdge}(u,i) \\
   \quad\quad\quad v := \operatorname{ComputeVertex}(u, i, \mathrm{PVI}(\varphi_X, i)) \\
   \quad\quad\quad \mathbf{if}\ v \notin V \\
   \quad\quad\quad\quad V := V \cup \{v\} \\
   \quad\quad\quad\quad \mathit{todo} := \mathit{todo} \cup \{v\} \\
   \quad\quad\quad E := E \cup \{(u, i, v)\} \\
   \mathbf{return}\ (V, E)
   \end{array}

.. math::

   \begin{array}{l}
   \operatorname{EnabledEdge}(u, i) \\
   \mathbf{let}\ u = (X, e) \\
   \mathbf{for}\ k = 1 \cdots |c_X|\ \mathbf{do} \\
   \quad \mathbf{if}\ \mathrm{source}(X, i, {\downarrow_X k}) \neq \bot \wedge
     \mathrm{source}(X, i, {\downarrow_X k}) \neq e_k\ \mathbf{then} \\
   \quad\quad \mathbf{return}\ \mathbf{false} \\
   \mathbf{return}\ \mathbf{true}
   \end{array}

.. math::

   \begin{array}{l}
   \operatorname{ComputeVertex}(u, i, Y(f)) \\
   \mathbf{let}\ u = (X, e) \\
   \mathbf{for}\ l = 1 \cdots |c_Y|\ \mathbf{do} \\
   \quad q := \mathrm{target}(X, i, {\downarrow_Y l}) \\
   \quad \mathbf{if}\ q = \bot\ \mathbf{then} \\
   \quad\quad \mathbf{choose}\ k\ \mathbf{such\ that}\ \mathrm{copy}(X, i, {\downarrow_X k}) = {\downarrow_Y l} \\
   \quad\quad v_l := e_k \\
   \quad \mathbf{else} \\
   \quad\quad v_l := q \\
   \mathbf{return}\ (Y, v)
   \end{array}

Note that :math:`\mathrm{copy}(X,i,{\downarrow_X k}) = {\downarrow_Y l}` implies
that parameters :math:`(X, {\downarrow_X k})` and :math:`(Y, {\downarrow_Y l})`
are related.

.. remark::

   In the code :math:`{\downarrow_X k}` is represented by ``cfp_X[k]``.

.. remark::

   In the code :math:`\mathrm{copy}(X,i,{\downarrow_X k})` is represented by
   ``Yf.copy(k)``, where :math:`Yf = \mathrm{PVI}(\varphi_X, i)`.

Global control flow marking
~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following algorithm computes the function :math:`marking` that denotes
which parameters are marked in a vertex of the control flow graph
:math:`(V, E)`.

.. math::

   \begin{array}{l}
   \operatorname{ComputeMarkingGlobal}(\mathcal{E},\; X_{init}(e_{init}),\; V,\; E) \\
   \mathbf{for}\ u = X(e) \in V\ \mathbf{do}\quad marking(u) := sig(u) \cap par(X) \\
   \mathit{todo} := V \\
   \mathbf{while}\ \mathit{todo} \neq \emptyset\ \mathbf{do} \\
   \quad \mathbf{choose}\ v = X(e) \in \mathit{todo} \\
   \quad \mathit{todo} := \mathit{todo} \setminus \{v\} \\
   \quad \mathbf{for}\ (u,v) \in E\ \mathbf{do} \\
   \quad\quad \mathbf{let}\ X(f) = label(u,v) \\
   \quad\quad \mathbf{for}\ d_X[j] \in marking(v) \\
   \quad\quad\quad M := (FV(f[j]) \setminus marking(u)) \cap par(X) \\
   \quad\quad\quad \mathbf{if}\ M \neq \emptyset \\
   \quad\quad\quad\quad marking(u) := marking(u) \cup M \\
   \quad\quad\quad\quad \mathit{todo} := \mathit{todo} \cup \{u\}
   \end{array}

Global reset variables
~~~~~~~~~~~~~~~~~~~~~~

Let :math:`\mathrm{PVI}(\varphi_X, i) = Y(e)` and let :math:`V` be the
global control flow graph. Then we define

.. math::

   \begin{array}{l}
   \operatorname{ResetVariableGlobal}(Y(e),\; i,\; V) \\
   \varphi := \mathbf{true} \\
   \mathbf{for}\ u = Y(f) \in V\ \mathbf{do} \\
   \quad c := \mathbf{true} \\
   \quad k := 1 \\
   \quad \mathbf{for}\ j = 1 \cdots |par(Y)|\ \mathbf{do} \\
   \quad\quad r := [] \\
   \quad\quad \mathbf{if}\ CFP(Y,j)\ \mathbf{then} \\
   \quad\quad\quad \mathbf{if}\ \mathrm{target}(X,i,j) = \bot \\
   \quad\quad\quad\quad c := c \wedge (e[j] = f[k]) \\
   \quad\quad\quad r := r \mathbin{\vartriangleleft} f[k] \\
   \quad\quad \mathbf{else\ if}\ e[j] \in marking(u)\ \mathbf{then} \\
   \quad\quad\quad r := r \mathbin{\vartriangleleft} e[j] \\
   \quad\quad \mathbf{else} \\
   \quad\quad\quad r := r \mathbin{\vartriangleleft} default\_value(e[j]) \\
   \quad\quad k := k + 1 \\
   \quad \varphi := \varphi \wedge (c \Rightarrow Y(r)) \\
   \mathbf{return}\ \varphi
   \end{array}

Compute values
--------------

Let :math:`C` be a component containing related CFPs.

.. math::

   \begin{array}{l}
   \operatorname{ComputeValues}(\mathcal{E},\; X_{init}(e_{init}),\; C) \\
   \mathit{result} := \emptyset \\
   \mathbf{for}\ (X,j) \in C\ \mathbf{do} \\
   \quad \mathbf{if}\ X = X_{init}\ \mathbf{then}\quad \mathit{result} := \mathit{result} \cup \{e_{init}[j]\} \\
   \mathbf{for}\ (X,k) \in C\ \mathbf{do} \\
   \quad \mathbf{for}\ i = 1 \cdots \mathrm{npred}(\varphi_X)\ \mathbf{do} \\
   \quad\quad \mathbf{if}\ \mathrm{source}(X,i,k) = v\ \mathbf{then}\quad
     \mathit{result} := \mathit{result} \cup \{v\} \\
   \mathbf{for}\ (Y,k) \in C\ \mathbf{do} \\
   \quad \mathbf{for}\ i = 1 \cdots \mathrm{npred}(\varphi_Y)\ \mathbf{do} \\
   \quad\quad \mathbf{if}\ \mathrm{pred}(\varphi_Y,i) = Y \wedge \mathrm{target}(Y,i,k) = v\ \mathbf{then}\quad
     \mathit{result} := \mathit{result} \cup \{v\} \\
   \mathbf{return}\ \mathit{result}
   \end{array}

Local algorithm
---------------

Local control flow graph
~~~~~~~~~~~~~~~~~~~~~~~~

Given a GCFG :math:`(W, \sim)` and a component :math:`C \subseteq W`, then we
define :math:`U` as
:math:`\{(X,i,d=e) \mid (X,i,d) \in C \wedge e \in \operatorname{ComputeValues}(C)\} \cup \{(X_{init},?,?=?) \mid \forall i{:}\,(X_{init},i) \notin C\}`.
Note that this algorithm potentially extends the graph with 'undefined' nodes
in a lazy fashion.

.. math::

   \begin{array}{l}
   \operatorname{ComputeLocalControlFlowGraph}(U, C) \\
   V, E := U, \emptyset \\
   \mathit{todo} := U \\
   \mathbf{while}\ \mathit{todo} \neq \emptyset\ \mathbf{do} \\
   \quad \mathbf{choose}\ u = (X,k,d=e) \in \mathit{todo} \\
   \quad \mathit{todo} := \mathit{todo} \setminus \{u\} \\
   \quad \mathbf{for}\ i = 1 \cdots \mathrm{npred}(\varphi_X)\ \mathbf{do} \\
   \quad\quad \mathbf{let}\ Y = \mathrm{pred}(\varphi_X, i) \\
   \quad\quad \mathbf{if}\ d = ?\ \mathbf{then} \\
   \quad\quad\quad \mathbf{if}\ (Y,k') \in C\ \text{for some}\ k'\ \mathbf{then} \\
   \quad\quad\quad\quad \mathbf{if}\ \mathrm{target}(X,i,k') = e'\ \mathbf{then} \\
   \quad\quad\quad\quad\quad v := (Y,k',d_Y[k']=e') \\
   \quad\quad\quad\quad\quad insert(V,E,\mathit{todo},u,i,v) \\
   \quad\quad\quad \mathbf{else} \\
   \quad\quad\quad\quad \mathbf{if}\ X \neq Y\ \mathbf{then} \\
   \quad\quad\quad\quad\quad v := (Y,?,?=?) \\
   \quad\quad\quad\quad\quad insert(V,E,\mathit{todo},u,i,v) \\
   \quad\quad \mathbf{else} \\
   \quad\quad\quad \mathbf{if}\ (Y,k') \in C\ \text{for some}\ k'\ \mathbf{then} \\
   \quad\quad\quad\quad \mathbf{if}\ (\mathrm{source}(X,i,k)=e \wedge \mathrm{target}(X,i,k')=e')\ \mathbf{then} \\
   \quad\quad\quad\quad\quad insert(V,E,\mathit{todo},u,i,(Y,k',d_Y[k']=e')) \\
   \quad\quad\quad\quad \mathbf{else\ if}\ (Y \neq X \wedge \mathrm{source}(X,i,k)=\bot \wedge \mathrm{target}(X,i,k')=e')\ \mathbf{then} \\
   \quad\quad\quad\quad\quad insert(V,E,\mathit{todo},u,i,(Y,k',d_Y[k']=e')) \\
   \quad\quad\quad\quad \mathbf{else\ if}\ (Y \neq X \wedge \mathrm{source}(X,i,k)=\bot \wedge \mathrm{copy}(X,i,k)=k')\ \mathbf{then} \\
   \quad\quad\quad\quad\quad insert(V,E,\mathit{todo},u,i,(Y,k',d_Y[k']=e)) \\
   \quad\quad\quad \mathbf{else} \\
   \quad\quad\quad\quad v := (Y,?,?=?) \\
   \quad\quad\quad\quad insert(V,E,\mathit{todo},u,i,v) \\
   \mathbf{return}\ (V,E)
   \end{array}

where :math:`insert(V,E,\mathit{todo},u,i,v)` is shorthand for the statements

.. math::

   \begin{array}{l}
   \mathbf{if}\ v \notin \mathit{todo} \wedge v \notin V\ \mathbf{then}\quad
     \mathit{todo} := \mathit{todo} \cup \{v\} \\
   V := V \cup \{v\} \\
   E := E \cup \{(u,i,v)\}
   \end{array}

Local belongs relation
~~~~~~~~~~~~~~~~~~~~~~

Let :math:`(V, \longrightarrow)` be a local control flow graph, and
:math:`\mathrm{rules}` be a predicate defined as

.. math::

   \mathrm{rules}(V,X,i) = \exists_{(X,j,e) \in V}{:}\; (X,j,e) \overset{i}{\longrightarrow}.

.. math::

   \begin{array}{l}
   \operatorname{ComputeBelongs}(V,\; \longrightarrow,\; \mathrm{rules}) \\
   B_k := \emptyset \\
   \mathbf{for}\ (X,j,v) \in V\ \mathbf{do} \\
   \quad \mathit{belongs} := \{m \mid d_X[m]\ \text{is a data parameter of}\ X\} \\
   \quad \mathbf{for}\ i = 1 \cdots \mathrm{npred}(\varphi)\ \mathbf{do} \\
   \quad\quad \mathbf{for}\ m \in \mathit{belongs}\ \mathbf{do} \\
   \quad\quad\quad \mathbf{if}\ (\mathrm{used}(X,i,m) \vee \mathrm{changed}(X,i,m)) \wedge
     \lnot\,\mathrm{rules}(V,X,i)\ \mathbf{then} \\
   \quad\quad\quad\quad \mathit{belongs} := \mathit{belongs} \setminus \{m\} \\
   \quad B_k := B_k \cup \{(X,d_X[m]) \mid m \in \mathit{belongs}\} \\
   \mathbf{return}\ B_k
   \end{array}

Local control flow marking
~~~~~~~~~~~~~~~~~~~~~~~~~~

Let :math:`(V, E)` be a local control flow graph, and :math:`B` the
corresponding belongs relation.

.. math::

   \begin{array}{l}
   \operatorname{UpdateMarkingRule1}(B,\; u,\; i,\; v) \\
   \mathbf{let}\ u = (X,n,d_X[n]=z) \\
   \mathbf{let}\ v = (Y,m,d_Y[m]=w) \\
   \mathbf{let}\ Y(e) = \mathrm{PVI}(\varphi_X, i) \\
   M := \emptyset \\
   \mathbf{for}\ d_Y[l] \in marking(v)\ \mathbf{do} \\
   \quad M := M \cup \bigl(\mathrm{FV}(\mathrm{rewr}(e[l],[d_X[n]{:=}z])) \cap \{d \mid (X,d) \in B\}\bigr) \\
   \mathbf{return}\ marking(u) \cup M
   \end{array}

.. math::

   \begin{array}{l}
   \operatorname{UpdateMarkingRule2}(B,\; u,\; i,\; v) \\
   \mathbf{let}\ u = (X,n,d_X[n]=z) \\
   \mathbf{let}\ v = (Y,m,d_Y[m]=w) \\
   \mathbf{let}\ Y(e) = \mathrm{PVI}(\varphi_X, i) \\
   M := \emptyset \\
   \mathbf{for}\ d_Y[l] \in marking(v)\ \mathbf{do} \\
   \quad \mathbf{if}\ (Y,d_Y[l]) \notin B\ \mathbf{then} \\
   \quad\quad M := M \cup \bigl(\mathrm{FV}(\mathrm{rewr}(e[l],[d_X[n]{:=}z])) \cap \{d \mid (X,d) \in B\}\bigr) \\
   \mathbf{return}\ marking(u) \cup M
   \end{array}

.. math::

   \begin{array}{l}
   \operatorname{ComputeMarkingLocal}(\mathcal{E},\; (V_1,E_1,B_1), \ldots, (V_J,E_J,B_J)) \\
   \mathbf{for}\ j = 1 \cdots J\ \mathbf{do} \\
   \quad \mathbf{for}\ u=(X,n,d_X[n]=z) \in V_j\ \mathbf{do} \\
   \quad\quad marking(u) := significant(u) \cap \{d \mid (X,d) \in B_j\} \\
   \mathit{stable} := \mathbf{false} \\
   \mathbf{while}\ \lnot\,\mathit{stable}\ \mathbf{do} \\
   \quad \mathit{stableint} := \mathbf{false} \\
   \quad \mathbf{while}\ \lnot\,\mathit{stableint}\ \mathbf{do} \\
   \quad\quad \mathit{stableint} := \mathbf{true} \\
   \quad\quad \mathbf{for}\ j = 1 \cdots J\ \mathbf{do} \\
   \quad\quad\quad \mathit{todo} := V_j \\
   \quad\quad\quad \mathbf{while}\ \mathit{todo} \neq \emptyset\ \mathbf{do} \\
   \quad\quad\quad\quad \mathbf{choose}\ u=(X,n,d_X[n]=z) \in \mathit{todo} \\
   \quad\quad\quad\quad \mathit{todo} := \mathit{todo} \setminus \{u\} \\
   \quad\quad\quad\quad \mathbf{if}\ marking(u) = \{d \mid (X,d) \in B_j\}\ \mathbf{then\ continue} \\
   \quad\quad\quad\quad \mathbf{for}\ (u,i,v) \in E_j\ \mathbf{do} \\
   \quad\quad\quad\quad\quad m := marking(u) \\
   \quad\quad\quad\quad\quad marking(u) := \operatorname{UpdateMarkingRule1}(B_j,\, u,\, i,\, v) \\
   \quad\quad\quad\quad\quad \mathbf{if}\ m \neq marking(u)\ \mathbf{then} \\
   \quad\quad\quad\quad\quad\quad \mathit{todo} := \mathit{todo} \cup \{v' \mid \exists i'{:}\,(v',i',u) \in E_j\} \\
   \quad\quad\quad\quad\quad\quad \mathit{stableint} := \mathbf{false} \\
   \quad \mathit{stableext} := \mathbf{false} \\
   \quad \mathbf{while}\ \lnot\,\mathit{stableext}\ \mathbf{do} \\
   \quad\quad \mathit{stableext} := \mathbf{true} \\
   \quad\quad \mathbf{for}\ j = 1 \cdots J\ \mathbf{do} \\
   \quad\quad\quad \mathbf{for}\ u=(X,n,d_X[n]=z) \in V_j\ \mathbf{do} \\
   \quad\quad\quad\quad \mathbf{if}\ marking(u) = \{d \mid (X,d) \in B_j\}\ \mathbf{then\ continue} \\
   \quad\quad\quad\quad \mathbf{for}\ (u,i,w) \in E_j\ \mathbf{do} \\
   \quad\quad\quad\quad\quad \mathbf{let}\ Y(e) = \mathrm{PVI}(\varphi_X, i) \\
   \quad\quad\quad\quad\quad \mathbf{for}\ k = 1 \cdots J\ \mathbf{do} \\
   \quad\quad\quad\quad\quad\quad \mathbf{if}\ k \neq j\ \mathbf{then} \\
   \quad\quad\quad\quad\quad\quad\quad \mathbf{for}\ v=(Y,m,d_Y[m]=w) \in V_k\ \mathbf{do} \\
   \quad\quad\quad\quad\quad\quad\quad\quad \mathbf{if}\ \exists v'=(X,m',D_X[m']) \in V_k\
     \text{s.t.}\ (v',i,v) \in E_k\ \mathbf{then} \\
   \quad\quad\quad\quad\quad\quad\quad\quad\quad m := marking(u) \\
   \quad\quad\quad\quad\quad\quad\quad\quad\quad marking(u) :=
     \operatorname{UpdateMarkingRule2}(B_j,\, u,\, i,\, v) \\
   \quad\quad\quad\quad\quad\quad\quad\quad\quad \mathbf{if}\ m \neq marking(u)\ \mathbf{then} \\
   \quad\quad\quad\quad\quad\quad\quad\quad\quad\quad \mathit{stableint} := \mathbf{false} \\
   \quad\quad\quad\quad\quad\quad\quad\quad\quad\quad \mathit{stableext} := \mathbf{false} \\
   \quad \mathit{stable} := \mathit{stableint} \wedge \mathit{stableext}
   \end{array}

Local reset variables
~~~~~~~~~~~~~~~~~~~~~

.. math::

   \begin{array}{l}
   \operatorname{ResetVariableLocal}(i,\; \sigma X(d_X)=\varphi_X,\;
     V_1,\ldots,V_J,\; B_1,\ldots,B_J,\; \mathrm{rules}) \\
   \mathbf{let}\ Y(e) = \mathrm{PVI}(\varphi_X, i) \\
   e' := e \\
   \mathbf{for}\ k = 1 \cdots |e|\ \mathbf{do} \\
   \quad \mathbf{if}\ \mathrm{CFP}(Y,k)\ \mathbf{then\ continue} \\
   \quad \mathit{relevant} := \mathbf{true} \\
   \quad \mathit{condition} := \{\} \\
   \quad \mathbf{for}\ j = 1 \cdots J\ \mathbf{do} \\
   \quad\quad \mathbf{if}\ \mathrm{rules}(V_j,X,i)\ \mathbf{then} \\
   \quad\quad\quad \mathbf{let}\ p,q\ \text{be such that}\ (Y,p,q) \in V_j \\
   \quad\quad\quad \mathbf{if}\ \mathrm{target}(X,i,p) \neq \bot\ \mathbf{then} \\
   \quad\quad\quad\quad \mathbf{let}\ q' = \mathrm{target}(X,i,p) \\
   \quad\quad\quad\quad \mathit{relevant} := \mathit{relevant} \wedge
     \bigl((Y,d_Y[k]) \in B_j \Rightarrow d_Y[k] \in marking(Y,p,q')\bigr) \\
   \quad\quad\quad \mathbf{else} \\
   \quad\quad\quad\quad \mathit{relevant} := \mathit{relevant} \wedge
     \bigl((Y,d_Y[k]) \in B_j \Rightarrow \exists r{:}\,d_Y[k] \in marking(Y,p,r)\bigr) \\
   \quad\quad\quad\quad \mathit{condition} \mathrel{{+}{=}}
     \{e[p]=r \mid (Y,d_Y[k]) \in B_j \wedge (Y,p,r) \in V_j \wedge d_Y[k] \notin marking(Y,p,r)\} \\
   \quad \mathbf{if}\ \lnot\,\mathit{relevant}\ \mathbf{then} \\
   \quad\quad e'[k] := default\_value(d_Y[k]) \\
   \quad \mathbf{else\ if}\ \mathit{condition} \neq \{\}\ \mathbf{then} \\
   \quad\quad e'[k] := \mathit{if}(join\_or(\mathit{condition}),\, default\_value(d_Y[k]),\, e[k]) \\
   \mathbf{return}\ Y(e')
   \end{array}
