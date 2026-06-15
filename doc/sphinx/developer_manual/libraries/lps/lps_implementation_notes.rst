LPS Implementation Notes
========================

The LPS-library
---------------

This document describes data types and algorithms of the LPS-library.

Terms
~~~~~

For an arbitrary term :math:`t` we define :math:`\mathcal{V}ar(t)` as the set of data
variables that occur in :math:`t`. The result of substituting :math:`d'` for :math:`d`
in a term :math:`t` is denoted as :math:`t[d:=d']`. With :math:`d\in Sub(t)` we
denote that data variable :math:`d` is a subterm of data expression :math:`t`.

Timed Linear processes
~~~~~~~~~~~~~~~~~~~~~~

Let :math:`\mathcal{D}` be the set of all data expressions, with :math:`\simeq` an
equivalence relation on :math:`\mathcal{D}`. All data expressions have implicitly
defined an associated sort. Let :math:`\mathcal{V\subset D}` be the set of all
data variables. Furthermore, let :math:`\mathcal{A}` be the set of action labels.
A timed linear process :math:`X` is a process equation of the following form:

.. math::
   :label: lps-impl-timed

   X(d:D) = \sum_{i\in I}\sum_{e:E_{i}} c_{i}(d,e) \rightarrow
   a_{i}(f_{i}(d,e)) \mathbin{@} t_{i}(d,e) \cdot X(g_{i}(d,e))
   + \sum_{i\in J}\sum_{e:E_{i}} c_{i}(d,e) \rightarrow \delta \mathbin{@} t_{i}(d,e)

and :math:`I` and :math:`J` are disjoint and finite index sets, and for :math:`i\in I\cup J`:

- :math:`c_{i}:D\times E_{i}\rightarrow\mathbb{B}` is the condition function,
- :math:`a_{i}(d,e)` is a multi-action
  :math:`a_{i}^{1}(f_{i}^{1}(d,e))|\cdots|a_{i}^{n_{i}}(f_{i}^{n_{i}}(d,e))`,
  where :math:`f_{i}:D\times E_{i}\rightarrow\mathcal{D}` is the action parameter function,
- :math:`t_{i}:D\times E_{i}\rightarrow\mathbb{R}` is the time stamp function,
- :math:`g_{i}:D\times E_{i}\rightarrow D` is the next state function.

The components :math:`d_{i}\in\mathcal{V}` of vector :math:`d=[d_{1},\cdots,d_{m}]`
are called the *process parameters* of :math:`X`.

Untimed Linear processes
~~~~~~~~~~~~~~~~~~~~~~~~

Similarly, an untimed linear process :math:`X` is a process equation of the
following form:

.. math::
   :label: lps-impl-untimed

   X(d:D) = \sum_{i\in I}\sum_{e:E_{i}} c_{i}(d,e) \rightarrow
   a_{i}(f_{i}(d,e)) X(g_{i}(d,e))
   + \sum_{i\in J}\sum_{e:E_{i}} c_{i}(d,e) \rightarrow \delta

with :math:`c_{i}`, :math:`a_{i}` and :math:`g_{i}` defined as above.

State Space
^^^^^^^^^^^

An untimed linear process :eq:`lps-impl-untimed` with initial value
:math:`\overline{d}\in D` defines a labeled transition system
:math:`M=(S,\Sigma,\rightarrow,s_{0})`, where

- :math:`S=D` is the (possibly infinite) set of states,
- :math:`\Sigma=\{a_{i}(f_{i}(d,e))\mid i\in I\wedge e:E_{i}\}` is the (possibly
  infinite) set of labels,
- :math:`{\rightarrow}=\{(d,a_{i}(f_{i}(d,e)),g_{i}(d,e))\mid i\in I\wedge e:E_{i}\wedge c_{i}(d,e)\}`,
- :math:`s_{0}=\overline{d}` is the initial state.

For :math:`R\subset S` we define :math:`next\_states(R)` as
:math:`\{s\in S\mid\exists r\in R:r\rightarrow s\}`.

Global variables
~~~~~~~~~~~~~~~~

In the mCRL2 tool set a linear process is parameterized with a finite set
:math:`DC` of so called *global* variables. This means that the expressions
:math:`c_{i}(d,e)`, :math:`f_{i}(d,e)`, :math:`g_{i}(d,e)`, and :math:`t_{i}(d,e)` may contain
unbound variables from the set :math:`DC`. Global variables have the implicit
property that any two processes obtained by assigning values to them are
strongly bisimulation equivalent. In many algorithms we will simply ignore
global variables. In some cases, algorithms may assign values to some of the
global variables.

Notations
~~~~~~~~~

Let :math:`p` be an untimed linear process as defined in :eq:`lps-impl-untimed`.

- A *constant parameter* of :math:`p` is a parameter :math:`d_{i}` that has a
  constant value for all reachable states of the corresponding state space,
  given an initial state :math:`\overline{d}:D`.

- An *insignificant parameter* of :math:`p` is a process parameter :math:`d_{i}`
  such that for any two initial states that differ only at the value of :math:`d_{i}`,
  the corresponding state spaces are isomorphic.

- Let :math:`p` be a linear process, :math:`x` a data variable and :math:`y` a data
  expression. Then :math:`p[x:=y]` is the linear process obtained from :math:`p` by
  applying the substitution :math:`x:=y` to all terms :math:`c_{i}(d,e)`,
  :math:`f_{i}(d,e)`, :math:`g_{i}(d,e)`, and :math:`t_{i}(d,e)` that appear in
  the definition of :math:`p`.

Linear processes
~~~~~~~~~~~~~~~~

Linear process expressions in mCRL2 are expressions built according to the
following syntax:

.. list-table::
   :header-rows: 1
   :widths: 35 40 25

   * - Expression
     - C++ equivalent
     - ATerm grammar
   * - :math:`b(e)`
     - ``action(b, e)``
     - Action
   * - :math:`\sum_{v} c \rightarrow a \mathbin{@} t\ P(g)`
     - ``summand(v, c, a, g)``
     - LinearProcessSummand
   * - :math:`\sum_{v} c \rightarrow \delta \mathbin{@} t`
     - ``summand(v, c, delta)``
     - LinearProcessSummand
   * - :math:`P(d:=e)`
     - | ``process_initializer(f, d:=e)``
       | *(where* :math:`f` *is a superset of the free variables appearing in* :math:`e` *)*
     - LinearProcessInit
   * - :math:`\sum_{i\in I} s_{i}`
     - | ``linear_process(f, v, s)``
       | *(where* :math:`s=[s_1,\cdots,s_n]` *is a sequence of summands,* :math:`v` *is the sequence of process parameters of the corresponding process, and* :math:`f` *is a sequence of free variables appearing in* :math:`s` *)*
     - LinearProcess

where the types of the symbols are as follows:

.. list-table::
   :widths: 10 90

   * - :math:`a`
     - a (timed) multi-action
   * - :math:`b`
     - a string (action name)
   * - :math:`\delta`
     - a (timed) deadlock
   * - :math:`P`
     - a process identifier
   * - :math:`e, f, g`
     - a sequence of data expressions
   * - :math:`d, v`
     - a sequence of data variables
   * - :math:`t`
     - a data expression of type real
   * - :math:`s`
     - a sequence of summands
   * - :math:`c`
     - a data expression of type bool

A grammar for linear processes can be found in the Process implementation
notes document.

Well typedness constraints
~~~~~~~~~~~~~~~~~~~~~~~~~~

Not all linear processes that adhere to the grammar for linear processes are
considered valid. A number of restrictions apply to make them valid input
for the mCRL2 toolset. These restrictions are called *well typedness
constraints*.

Well typedness constraints for data specifications
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- the domain and range sorts of constructors are declared in the data
  specification
- the domain and range sorts of mappings are declared in the data
  specification

Well typedness constraints for a linear process
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- the process parameters have unique names
- process parameters and summation variables have different names
- the left hand sides of the assignments of summands are contained in
  the process parameters
- the summands are well typed

Well typedness constraints for linear process specifications
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- the sorts occurring in the summation variables are declared in the
  data specification
- the sorts occurring in the process parameters are declared in the
  data specification
- the sorts occurring in the global variables are declared in the data
  specification
- the sorts occurring in the action labels are declared in the data
  specification
- the action labels occurring in the linear process are declared in the
  action specification
- the data specification is well typed
- the linear process is well typed
- the process initializer is well typed
- the global variables occurring in the linear process are declared in
  the global variable specification
- the global variables occurring in the initial process are declared in
  the global variable specification
- the global variables have unique names

Well typedness constraints for other types
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

- the sorts of the left and right hand side of an assignment are the same
- the time of a summand has type Real
- the condition of a summand has type Bool
- the set of left hand sides of assignments in an action summand or
  process initializer does not contain duplicates

Algorithms
----------

We now define two operations on linear processes: removing (insignificant)
parameters and removing constant parameters. Let :math:`X(d:D)` be a linear
process as defined in :eq:`lps-impl-untimed` or :eq:`lps-impl-timed`, and let
:math:`\{d_{j_{1}},\cdots,d_{j_{m}}\}` be a set of insignificant parameters of
:math:`X`. Then we define
:math:`\textsc{RemoveParameters}(p,\{d_{j_{1}},\cdots,d_{j_{m}}\})` as a linear
process obtained from :math:`X(d:D)` by removing
:math:`\{d_{j_{1}},\cdots,d_{j_{m}}\}` from the process parameters of :math:`X`,
and by replacing each term :math:`c_{i}(d,e)`, :math:`f_{i}(d,e)`, :math:`g_{i}(d,e)`,
or :math:`t_{i}(d,e)` that appears in the definition of :math:`X`, and that has one
of the variables :math:`d_{j_{1}},\cdots,d_{j_{m}}` as a subterm by a term
:math:`c'_{i}(d,e)`, :math:`f'_{i}(d,e)`, :math:`g'_{i}(d,e)`, or :math:`t'_{i}(d,e)` that
does not have one of the variables :math:`d_{j_{1}},\cdots,d_{j_{m}}` as a subterm,
and such that the remaining process is strongly bisimulation equivalent to
:math:`X(d:D)` [#fn-remparam]_.

.. [#fn-remparam] A more formal definition of this is welcome.

Let :math:`X(d:D)` be a linear process as defined in :eq:`lps-impl-untimed` or
:eq:`lps-impl-timed`, and let :math:`\{d_{j_{1}},\cdots,d_{j_{m}}\}` be a set of
constant parameters of :math:`X`, given the state :math:`\overline{d}:D`. Then we define
:math:`\textsc{RemoveConstantParameters}(p,\{d_{j_{1}},\cdots,d_{j_{m}}\},\overline{d})`
as a linear process obtained from :math:`X(d:D)` by removing
:math:`\{d_{j_{1}},\cdots,d_{j_{m}}\}` from the process parameters of :math:`X`, and
by replacing each term :math:`c_{i}(d,e)`, :math:`f_{i}(d,e)`, :math:`g_{i}(d,e)`, or
:math:`t_{i}(d,e)` that appears in the definition of :math:`X` by a term
:math:`c'_{i}(d,e)`, :math:`f'_{i}(d,e)`, :math:`g'_{i}(d,e)`, or :math:`t'_{i}(d,e)` that
does not have one of the variables :math:`d_{j_{1}},\cdots,d_{j_{m}}` as a subterm,
and such that the remaining process is strongly bisimulation equivalent to
:math:`X(d:D)` [#fn-remconst]_.

.. [#fn-remconst] Or should this be restricted to the result of substituting all
   the constant values, and possibly applying the rewriter to it?

Parelm
~~~~~~

Let :math:`P` be the stochastic linear process

.. math::

   P(d:D) = \sum_{i\in I}\sum_{e:E_{i}} c_{i}(d,e_{i}) \rightarrow
   a_{i}(f_{i}(d,e_{i})) \mathbin{@} t_{i}(d,e_{i}) \cdot
   \frac{p_{i}(d,e_{i},h_{i})}{h_{i}:F_{i}} P(g_{i}(d,e_{i},h_{i}))

with initial state

.. math::

   P_{init} = \frac{p(h)}{h:F} P(g(h))

with :math:`d=[d_1,\ldots,d_n]` the process parameters of :math:`P`. The algorithm
:math:`\textsc{Parelm}` is used to find insignificant process parameters that can be
eliminated from :math:`P` without altering the corresponding state space. It is
not guaranteed that all insignificant process parameters are detected.

Parelm implementations
^^^^^^^^^^^^^^^^^^^^^^^

We define the following implementations of parelm:

.. math::

   \begin{array}{|l|}
   \hline
   \textsc{Parelm1}(P, P_{init}) \\
   G := \{k\in [1,\ldots,n] \mid \exists_{i\in I} : d_k \in
     \mathcal{FV}(c_i(d,e_i)) \cup \mathcal{FV}(f_i(d,e_i)) \cup
     \mathcal{FV}(t_i(d,e_i)) \cup \mathcal{FV}(p_i(d,e_i,h_i))\} \\
   \text{\textbf{do}} \\
   \qquad \Delta G := \{k \in [1,\ldots,n] \mid \exists_{l\in G}\ \exists_{i\in I} :
     d_k \in \mathcal{FV}(g_i(d,e_i,h_i)_l)\} \\
   \qquad G := G \cup \Delta G \\
   \text{\textbf{while}}\ (\Delta G \neq \emptyset) \\
   \text{\textbf{return}}\ \{d_{j_1},\cdots,d_{j_m}\} \\
   \hline
   \end{array}

where :math:`\{j_1,\cdots,j_m\}=G`. Note that :math:`g_i(d,e)_l` is the :math:`l`-th
component of the vector of terms :math:`g_i(d,e)`.

.. math::

   \begin{array}{|l|}
   \hline
   \textsc{Parelm2}(P, P_{init}) \\
   W := \{d_k \mid \exists_{i\in I} : d_k \in
     \mathcal{FV}(c_i(d,e_i)) \cup \mathcal{FV}(f_i(d,e_i)) \cup
     \mathcal{FV}(t_i(d,e_i)) \cup \mathcal{FV}(p_i(d,e_i,h_i))\} \\
   V := \{d_1,\ldots,d_n\} \\
   E := \{(d_j,d_k) \mid \exists_{i\in I} : d_j \in \mathcal{FV}(g_i(d,e_i,h_i)_k)\} \\
   R := \{d_k \mid \text{graph}\ G=(V,E)\ \text{contains a directed path}\
     w,\cdots,d_k\ \text{for some}\ w\in W\} \\
   \text{\textbf{return}}\ R \\
   \hline
   \end{array}

.. note::

   In the implementation, the initial seed set :math:`W` (resp. :math:`G` in
   Parelm1) also includes variables occurring in **deadlock summands**
   :math:`\sum_{i\in J}\sum_{e:E_i} c_i(d,e_i) \rightarrow \delta \mathbin{@} t_i(d,e_i)`,
   specifically variables free in the condition :math:`c_i(d,e_i)` and in the
   time expression :math:`t_i(d,e_i)`. The dependency graph :math:`E` is built
   from action summands only, since deadlock summands have no next-state
   assignment.

Constelm
~~~~~~~~

Let :math:`P` be the stochastic linear process

.. math::

   P(d:D) = \sum_{i\in I}\sum_{e:E_{i}} c_{i}(d,e_{i}) \rightarrow
   a_{i}(f_{i}(d,e_{i})) \mathbin{@} t_{i}(d,e_{i}) \cdot
   \frac{p_{i}(d,e_{i},h_{i})}{h_{i}:F_{i}} P(g_{i}(d,e_{i},h_{i}))

with initial state

.. math::

   P_{init} = \frac{p(h)}{h:F} P(g(h))

with :math:`d=[d_1,\ldots,d_n]` the process parameters of :math:`P`. The algorithm
:math:`\textsc{Constelm}` is used to find process parameters that have a constant
value. If needed the global variables of :math:`P` will be assigned constant
values. The result is a substitution :math:`\sigma` that assigns these constant
values to the corresponding process parameters and global variables. It is
not guaranteed that all constant parameters are detected.

Constelm implementations
^^^^^^^^^^^^^^^^^^^^^^^^^

Then we define the following implementation of :math:`\textsc{Constelm}`:

.. math::

   \begin{array}{|l|}
   \hline
   \textsc{Constelm}(P, P_{init}, V, R) \\
   r := R(g(h)) \\
   G := \{d_1,\ldots,d_n\} \\
   \sigma := \epsilon \\
   \text{\textbf{for}}\ j:=1\ \text{\textbf{to}}\ n\ \text{\textbf{do}} \\
   \qquad \sigma[d_j] := r_j \\
   \qquad undo[j] := \emptyset \\
   \text{\textbf{do}} \\
   \qquad \Delta G := \emptyset \\
   \qquad \text{\textbf{for}}\ i\in I\ \text{\textbf{do}} \\
   \qquad\qquad \text{\textbf{if}}\ R(\sigma(c_i(d,e_i))) \neq false \\
   \qquad\qquad\qquad \text{\textbf{for}}\ d_j \in G \setminus \Delta G\ \text{\textbf{do}} \\
   \qquad\qquad\qquad\qquad \text{\textbf{let}}\ z = R(g_i(d,e_i,h_i)_j, \sigma) \\
   \qquad\qquad\qquad\qquad \text{\textbf{if}}\ z \neq \sigma(d_j) \\
   \qquad\qquad\qquad\qquad\qquad \text{\textbf{if}}\ z \in V \\
   \qquad\qquad\qquad\qquad\qquad\qquad \sigma[z] := r_j \\
   \qquad\qquad\qquad\qquad\qquad\qquad undo[j] := undo[j] \cup \{z\} \\
   \qquad\qquad\qquad\qquad\qquad \text{\textbf{else}} \\
   \qquad\qquad\qquad\qquad\qquad\qquad \Delta G := \Delta G \cup \{d_j\} \\
   \qquad\qquad\qquad\qquad\qquad\qquad \sigma[d_j] := d_j \\
   \qquad\qquad\qquad\qquad\qquad\qquad \text{\textbf{for}}\ w\in undo[j]\ \text{\textbf{do}} \\
   \qquad\qquad\qquad\qquad\qquad\qquad\qquad \sigma[w] := w \\
   \qquad\qquad\qquad\qquad\qquad\qquad undo[j] := \emptyset \\
   \qquad G := G \setminus \Delta G \\
   \text{\textbf{while}}\ (\Delta G \neq \emptyset) \\
   \text{\textbf{return}}\ \sigma \\
   \hline
   \end{array}

where :math:`V` is the set of global variables of the linear process :math:`P`,
:math:`R` is a rewriter, and :math:`\epsilon` is the empty substitution (i.e. the
identity function). The notation :math:`\sigma[v]:=w` means :math:`\sigma=\sigma'`
with

.. math::

   \sigma'(x) = \begin{cases} \sigma(x) & \text{if}\ x\neq v \\ w & \text{if}\ x=v \end{cases}

An alternative implementation:

.. math::

   \begin{array}{|l|}
   \hline
   \textsc{Constelm2}(p, \overline{d}) \\
   R := \overline{\{d\}} \\
   G := \{1,\ldots,n\} \\
   \text{\textbf{do}} \\
   \qquad \Delta R := next\_states(p,R) \setminus R \\
   \qquad \text{\textbf{if}}\ \exists_{i\in I}.\,\exists_{j\in G}.\,\exists_{r\in \Delta R}.\,
     r_j \neq \overline{d}_j \\
   \qquad\qquad G := G \setminus \{j\} \\
   \qquad R := R \cup \Delta R \\
   \text{\textbf{while}}\ (\Delta R \neq \emptyset) \\
   \text{\textbf{return}}\ \textsc{RemoveConstantParameters}(p,\{d_{j_1},\cdots,d_{j_m}\},\overline{d}) \\
   \hline
   \end{array}

where :math:`\{j_1,\cdots,j_m\}=G`.

Conversion to linear process
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The function :math:`lin` converts a process expression to linear process format,
if it is linear.

.. math::

   \begin{aligned}
   lin(\text{if\_then}(c,\delta)) &= \text{summand}([],c,\delta) \\
   lin(\text{if\_then}(c,\text{seq}(x,P(g)))) &= \text{summand}([],c,lin(x),g) \\
   \text{add\_summand\_variables}(\text{summand}(v,c,\overrightarrow{a},g),w)
     &= \text{summand}(v+w,c,\overrightarrow{a},g) \\
   \text{add\_summand\_variables}(\text{summand}(v,c,\delta),w)
     &= \text{summand}(v+w,c,\delta) \\
   lin(\text{sum}(v,x)) &= \text{add\_summand\_variables}(lin(x),w) \\
   lin(\text{seq}(x,P)) &= lin(x) \\
   \text{set\_multi\_action\_time}(\text{multi\_action}([a_1,\cdots,a_n]),t)
     &= \text{multi\_action}([a_1,\cdots,a_n],t) \\
   lin(\text{at\_time}(x,t)) &= \text{set\_multi\_action\_time}(lin(x),t) \\
   \text{multi\_action}([a_1,\cdots,a_m]) + \text{multi\_action}([a_{m+1},\cdots,a_n])
     &= \text{multi\_action}([a_1,\cdots,a_n]) \\
   lin(\text{sync}(x,y)) &= lin(x)+lin(y) \\
   lin(a) &= \text{multi\_action}([a]) \\
   lin(\tau) &= \text{multi\_action}([]) \\
   lin(\text{choice}(\text{choice}(x,y),z))
     &= lin(\text{choice}(x,y)) + [lin(z)] \\
   lin(\text{choice}(x,\text{choice}(y,z)))
     &= [lin(x)] + lin(\text{choice}(y,z)) \\
   lin(\text{choice}(x,y)) &= [lin(x), lin(y)]
   \end{aligned}
