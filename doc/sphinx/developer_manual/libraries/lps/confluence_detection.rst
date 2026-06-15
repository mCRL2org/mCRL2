.. _lps-confluence-detection:

Confluence Detection
====================

| *Author*: Wieger Wesselink
| *Date*: September 2019

Confluence
----------

Consider the following untimed linear process specification :math:`P`, with initial state :math:`d_0`.

.. math::

   \begin{array}{l}
   P(d)=
   \sum\limits_{i\in I}\sum\limits_{e_i}c_i(d, e_i)\rightarrow a_i(f_i(d,e_i)) \cdot P(g_i(d,e_i))
   \end{array}

We distinguish different kinds of confluence. Let summand :math:`j \in I` be the index of a
:math:`\tau`-summand, and :math:`i \in I` be the index of an arbitrary summand.
In the sequel we abbreviate :math:`c_i(d,e_i)`, :math:`f_i(d,e_i)` and :math:`g_i(d,e_i)` with
:math:`c_i`, :math:`f_i` and :math:`g_i`. Note that for the :math:`\tau`-summand with index
:math:`j` we have :math:`a_j = \tau` and :math:`f_j = []`.

Trivial confluence
~~~~~~~~~~~~~~~~~~

Trivial confluence is defined as

.. math::

   \begin{array}{l}
   C_{trivial}(i, j) = \forall d, e_i, e_j : (c_i \land c_j) \Rightarrow
         (a_i = \tau)
        \land (g_i = g_j)
   \end{array}

Note that trivial confluence only applies to :math:`\tau`-summands. In isolation it is not a
very useful property to check.

Triangular confluence
~~~~~~~~~~~~~~~~~~~~~

Triangular confluence is defined as

.. math::

   \begin{array}{l}
   C_{triangular}(i, j) = \forall d, e_i, e_j : (c_i \land c_j) \Rightarrow
         (c_i[d := g_j]
   \land (f_i = f_i[d := g_j])
   \land (g_i[d := g_j] = g_i))
   \end{array}

Commutative confluence
~~~~~~~~~~~~~~~~~~~~~~

Commutative confluence is defined as

.. math::

   \begin{array}{l}
   C_{commutative}(i, j) = C_{trivial}(i, j) \lor \forall d, e_i, e_j : (c_i \land c_j) \Rightarrow
   \quad \exists e'_i, e'_j : \\
   \qquad \begin{array}{ll}
   ( & \\
         & c_i[d := g_j, e_i := e'_i] \\
   \quad \land & c_j[d := g_i, e_j := e'_j] \\
   \quad \land & (f_i = f_i[d := g_j, e_i := e'_i]) \\
   \quad \land & (g_i[d := g_j, e_i := e'_i] = g_j[d := g_i, e_j := e'_j]) \\
   ) &
   \end{array} \\
   \end{array}

The reason for adding the term :math:`C_{trivial}(i, j)` is probably that otherwise a simple
:math:`\tau`-summand like

.. math::

   (n=0) \rightarrow \tau \cdot P(n=1)

is not even confluent with itself.

Square confluence
~~~~~~~~~~~~~~~~~

Square confluence is defined as

.. math::

   \begin{array}{l}
   C_{square}(i, j) = C_{trivial}(i, j) \lor \forall d, e_i, e_j : (c_i \land c_j) \Rightarrow \\
   \qquad c_i[d := g_j]
   \land c_j[d := g_i]
   \land (f_i = f_i[d := g_j])
   \land (g_i[d := g_j] = g_j[d := g_i])
   \end{array}

It is obtained from :math:`C_{commutative}(i,j)` by taking :math:`e'_i = e_i` and
:math:`e'_j = e_j`.

.. math::
   :nowrap:
   :class: math-tikz

   \begin{tikzpicture}[node distance=7em]
       \tikzstyle{every state}=[draw, shape=circle, very thick, minimum size=2em, initial text={}]
       \tikzset{->, >=stealth', shorten >=1pt, semithick, auto}
       \node[state,initial]   (d)                       {$d$};
       \node[state]           (gj)  [above right of=d]  {$g_j$};
       \node[state]           (gi)  [below right of=d]  {$g_i$};
       \node[state]           (d1)  [below right of=gj] {$d_1$};
       \path[->]
         (d)  edge node {$\tau$} (gj)
         (d)  edge node {$a_i$}  (gi)
         (gi) edge node {$\tau$} (d1)
         (gj) edge node {$a_i$} (d1);
   \end{tikzpicture}

*Square commutative confluence, with* :math:`d_1 = g_j[d:=g_i]`

.. math::
   :nowrap:
   :class: math-tikz

   \begin{tikzpicture}[node distance=7em]
       \tikzstyle{every state}=[draw, shape=circle, very thick, minimum size=2em, initial text={}]
       \tikzset{->, >=stealth', shorten >=1pt, semithick, auto}
       \node[state,initial]   (d)                      {$d$};
       \node[state]           (gj)  [above right of=d] {$g_j$};
       \node[state]           (gi)  [below right of=d] {$g_i$};
       \path[->]
         (d)  edge node {$\tau$} (gj)
         (d)  edge node {$a_i$}  (gi)
         (gj) edge node {$a_i$} (gi);
   \end{tikzpicture}

*Triangular confluence*

.. math::
   :nowrap:
   :class: math-tikz

   \begin{tikzpicture}[node distance=7em]
       \tikzstyle{every state}=[draw, shape=circle, very thick, minimum size=2em, initial text={}]
       \tikzset{->, >=stealth', shorten >=1pt, semithick, auto}
       \node[state,initial]   (d)               {$d$};
       \node[state]           (gj) [right of=d] {$g_j$};
       \path[->]
         (d) edge[bend left,  above] node {$\tau$} (gj)
         (d) edge[bend right, below] node {$a_j$}  (gj);
   \end{tikzpicture}

*Trivial confluence, with* :math:`a_j = \tau`
