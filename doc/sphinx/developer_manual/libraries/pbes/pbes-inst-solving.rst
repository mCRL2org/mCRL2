:Date: 16-01-2026
:Authors: Jeroen Keiren, Maurice Laveaux, Anna Stramaglia, Wieger Wesselink and Tim Willemse.
:Institute: Eindhoven University of Technology

.. default-role:: math
.. highlight:: mcrl2
   
Parameterised Boolean Equation Systems Instantiation and Solving
================================================================

This section describes instantiation and solving algorithms for PBESs that are used in the tool ``pbessolve``.

Preliminaries to PBES instantiation to structure graph
------------------------------------------------------

The PBES instantiation algorithm, defined in Algorithm :ref:`PbesInstStructureGraph <instantiation_algorithm>` builds a structure graph given a PBES. The graph is constructed by instantiation of the PBES equations. 

Structure graph
"""""""""""""""

A structure graph, see [KRW12]_, is a graph structure that captures BESs in their full generality. Structure graphs capture information regarding (the order of) fixed points, as well as the structure of the right hand sides of the equations.

.. _strcture-graph:

.. admonition:: Definition (Structure graph)

    A structure graph is a tuple `(V,E,d,r)` with `V` a set of nodes containing proposition variables, `E \subseteq V \times V` a set of edges, `r:V\rightarrow \mathbb{N}` a partial function that assigns a rank to each node, and `d:V\rightarrow \{\blacktriangle ,\blacktriangledown ,\top ,\bot \}` a partial function that assigns a decoration to each node. 
    A structure graph is formally defined using the following SOS rules, where `\mathtt{B}` is the BES and `\bnd(\mathtt{B})` its bound proposition variables, `X` is proposition variable, `f,f',g,g'` are proposition formulae and `\sigma \in \{\mu, \nu\}` is the fixpoint:

    .. math::
        :nowrap:

        \begin{equation*}
        \frac{X\in \bnd(\mathtt{B})}{r(X) = \rank_{\mathtt{B}}(X)}
        \end{equation*}%
        \begin{equation*}
        \frac{{}}{d(\true) = \top }\qquad \frac{{}}{d(\false) = \bot } \qquad
        \frac{{}}{d(f \wedge f^{\prime}) = \blacktriangle } \qquad
        \frac{{}}{d(f \vee f^{\prime}) = \blacktriangledown }
        \end{equation*}%
        \begin{equation*}
        \frac{f\blacktriangle \quad f\rightarrow g}{(f'\wedge f)\rightarrow g} \qquad
        \frac{f'\blacktriangle \quad f'\rightarrow g'}{(f^{\prime }\wedge f)\rightarrow g'} \qquad
        \frac{f\blacktriangledown \quad f\rightarrow g}{(f
        '\vee f)\rightarrow g} \qquad
        \frac{f'\blacktriangledown \quad f'\rightarrow g'}{(f^{\prime }\vee f)\rightarrow g'}
        \end{equation*}%
        \begin{equation*}
        \frac{\lnot f\blacktriangle }{f\wedge f^{\prime }\rightarrow f}\qquad \frac{%
        \lnot f^{\prime }\blacktriangle }{f\wedge f^{\prime }\rightarrow f^{\prime }} \qquad
        \frac{\lnot f\blacktriangledown }{f\vee f^{\prime }\rightarrow f}\qquad
        \frac{\lnot f^{\prime }\blacktriangledown }{f\vee f^{\prime }\rightarrow
        f^{\prime }}
        \end{equation*}%
        \begin{equation*}
        \frac{{}}{X\wedge f\rightarrow X}\qquad \frac{{}}{f\wedge X\rightarrow X} \qquad
        \frac{{}}{X\vee f\rightarrow X}\qquad \frac{{}}{f\vee X\rightarrow X}
        \end{equation*}%
        \begin{equation*}
        \frac{\sigma X=f\wedge f^{\prime }\in \mathtt{B}}{d(X) = \blacktriangle }\qquad
        \frac{\sigma X=f\vee f^{\prime }\in \mathtt{B}}{d(X) = \blacktriangledown }
        \end{equation*}%
        \begin{equation*}
        \frac{\sigma X=Y\in \mathtt{B}}{X\rightarrow Y}\qquad \frac{\sigma X=\top
        \in \mathtt{B}}{X\rightarrow \top }\qquad \frac{\sigma X=\bot \in {\mathtt{B}%
        }}{X\rightarrow \bot }
        \end{equation*}%
        \begin{equation*}
        \frac{\sigma X=f\wedge f^{\prime }\in \mathtt{B}\quad f\wedge f^{\prime
        }\rightarrow g}{X\rightarrow g} \qquad
        \frac{\sigma X=f\vee f^{\prime }\in \mathtt{B}\quad f\vee f^{\prime
        }\rightarrow g}{X\rightarrow g}
        \end{equation*}%

    where the rank of a proposition variable `X \in \bnd(\mathtt{B})`, denoted as `\rank_\mathtt{B}(X)` is defined as follows:

    .. math::
        :nowrap:

            \[\rank_{(\sigma Y = \varphi)\mathtt{B}}(X) = 
            \begin{cases}
                \rank_\mathtt{B}(X) & \text{if $X \neq Y$,} \\
                \mathtt{B} \rceil \sigma & \text{otherwise.}
            \end{cases}\]

    where `\mathtt{B} \rceil \sigma` is defined as:

    .. math::
        :nowrap:

            \begin{equation*}
            \mathcal{\epsilon \rceil \sigma} = 
            \begin{cases}
                0 & \text{if $\sigma = \nu$,} \\
                1 & \text{otherwise.}
            \end{cases} \qquad
            (\sigma' Y = \varphi) \mathtt{B} \rceil \sigma = 
            \begin{cases}
                \mathtt{B} \rceil \sigma & \text{if $\sigma = \sigma'$,} \\
                1 + \mathtt{B} \rceil \sigma & \text{if $\sigma \neq \sigma'$}.
            \end{cases}
            \end{equation*}

In the implementation, each proposition variable (BES variables) is replaced by a propositional variable instantiation (PVI). A PVI is a predicate variable instantiation, i.e., given a predicate formula `\varphi`, an occurrence `Y(e)` in `\varphi` is a PVI. In the implementation, the nodes of the structure graphs are PVIs.


Attractor sets
""""""""""""""

Let `A \subseteq V` be a subset of vertices of a structure graph `G = (V, E, d , r)`. We define Algorithm :ref:`AttrDefault <attr>` to compute the attractor set of A. The value `\alpha = 0` corresponds with disjunction (`\blacktriangledown`) and `\alpha = 1` corresponds with conjunction (`\blacktriangle`), i.e. the decoration of `u` is `\blacktriangledown` if `d_u = 0`. 

.. _attr:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \Function {AttrDefault}{$A, \alpha$}
    \State {$ \td := \bigcup_{u \in A}(\mathit{pred(u)} \setminus A )$} 
    \State $\mathbf{while}\ \td \neq \emptyset\ \mathbf{do}$
        \State $\qquad \textbf{choose}\ u \in \td$
        \State $\qquad \td := \td \setminus \{u\}$
        \State {$\qquad \mathbf{if}\ d_u = \alpha \vee \mathit{succ(u)} \subseteq A\ \mathbf{then}$} 
            \State \qquad \qquad {\colorbox{lightgray}{$\mathbf{if}\ d_u = \alpha\ \mathbf{then}\ \tau[u \to v] \mid v \in A \cap \mathit{succ(u)}$}}
            \State $\qquad \qquad A := A \cup \{u\}$
            \State $\qquad \qquad \td := \td \cup (\mathit{pred(u)} \setminus A )$
    \State \Return {$A$}
    \EndFunction
    \end{algorithmic}

where, `\pred(v) = \{u \in V | (u,v) \in E\}` and `\scc(u) = \{v \in V | (u,v) \in E\}`. The algorithm returns, for player `\alpha`, the attractor set `A`. As a side effect a mapping `\tau` is produced that corresponds to a winning strategy.

The algorithm is extended in Algorithm :ref:`AttrDefaultWithTau <attr-tau>` such that in addition it sets a local strategy in the mapping `\tau_\alpha`.

.. _attr-tau:

.. math::
    :nowrap:
    :class: math-left
 
    \begin{algorithmic}[1]
    \Function{AttrDefaultWithTau}{$A, \alpha, \tau_\alpha$}
    \State {$ \td := \bigcup_{u \in A}(\mathit{pred(u)} \setminus A )$} 
    \State $\mathbf{while}\ \td \neq \emptyset\ \mathbf{do}$
        \State $\qquad \textbf{choose}\ u \in \td$
        \State $\qquad \td := \td \setminus \{u\}$
        \State $\qquad \mathbf{if}\ d_u = \alpha \vee \mathit{succ(u)} \subseteq A\ \mathbf{then}$ 
            \State \qquad \qquad {\colorbox{lightgray}{$\mathbf{if}\ d_u = \alpha\ \mathbf{then}\ \tau_\alpha[u \to v] \mid v \in A \cap \mathit{succ(u)}$}}
            \State $\qquad \qquad A := A \cup \{u\}$
            \State $\qquad \qquad \td := \td \cup (\mathit{pred(u)} \setminus A)$
    \State \Return {$A, \tau_\alpha$}
    \EndFunction
    \end{algorithmic}

Finally Algorithm :ref:`AttrDefaultNoStrategy <attr-no-tau>` does not set any strategy.

.. _attr-no-tau:

.. math::
    :nowrap:
    :class: math-left
 
    \begin{algorithmic}[1]
    \Function{AttrDefaultNoStrategy}{$A, \alpha$}
    \State {$ \td := \bigcup_{u \in A}(\mathit{pred(u)} \setminus A )$} 
    \State $\mathbf{while}\ \td \neq \emptyset\ \mathbf{do}$
        \State $\qquad \textbf{choose}\ u \in \td$
        \State $\qquad \td := \td \setminus \{u\}$
        \State $\qquad \mathbf{if}\ d_u = \alpha \vee \mathit{succ(u)} \subseteq A\ \mathbf{then}$ 
            \State $\qquad \qquad A := A \cup \{u\}$
            \State $\qquad \qquad \td := \td \cup (\mathit{pred(u)} \setminus A)$
    \State \Return {$A$}
    \EndFunction
    \end{algorithmic}

Preprocessing rewriters
"""""""""""""""""""""""

Two rewriters are used to preprocess PBES equations before instantiation: the *simplify rewriter* and the *one-point-rule quantifier elimination rewriter*. 

In Algorithm :ref:`PbesInstStructureGraph <instantiation_algorithm>` the application of first *simplify rewriter* and then *one-point-rule quantifier elimination rewriter* is combined in rewriter `R`.

Simplify rewriter
~~~~~~~~~~~~~~~~~

The *simplify* rewriter for PBESs takes a PBES expression and simplifies it. 
We define the *simplify* rewriter `\sr` inductively as follows. For the cases in which quantifiers are used, we consider them ranging over a set `V = \{x_1:D_1, \ldots, x_n: D_n\}` of typed variables, and indicate with `\free(\varphi)` the free variables appearing in PBES expression `\varphi`.

.. _rewriter-simplify:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \sr(b) &=& b \\
        \sr(X(e)) &=& X(e) \\
        \sr(\lnot \varphi)  &=& \left\{
        \begin{array}{ll}
            \true & \text{if } \sr(\varphi) = \false\\
            \false & \text{if } \sr(\varphi) = \true \\
            \psi & \text{if } \sr(\varphi) = \lnot \psi \\
            \lnot \sr(\varphi) & \text{otherwise}
        \end{array}%
        \right.  \\
        \sr(\varphi \land \psi) &=& \left\{
        \begin{array}{ll}
            \false & \text{if } \sr(\varphi) = \false \\
            \sr(\psi) & \text{if } \sr(\varphi) = \true \\
            \false & \text{if } \sr(\psi) = \false  \\
            \sr(\varphi) & \text{if } \sr(\psi) = \true \\
            \sr(\varphi) & \text{if } \sr(\varphi) = \sr(\psi) \\
            \sr(\varphi) \land \sr(\psi) & \text{otherwise}
        \end{array}%
        \right.  \\
        \sr(\varphi \lor \psi) &=& \left\{
        \begin{array}{ll}
            \true & \text{if } \sr(\varphi) = \true \\
            \sr(\psi) & \text{if } \sr(\varphi) = \false \\
            \true & \text{if } \sr(\psi) = \true \\
            \sr(\varphi) & \text{if } \sr(\psi) = \false \\
            \sr(\varphi) & \text{if } \sr(\varphi) = \sr(\psi) \\
            \sr(\varphi) \lor \sr(\psi) & \text{otherwise}
        \end{array}%
        \right.  \\
        \sr(\varphi \implies \psi) &=& \left\{
        \begin{array}{ll}
            \true & \text{if } \sr(\varphi) = \false \\
            \true & \text{if } \sr(\psi) = \true \\
            \sr(\psi) & \text{if } \sr(\varphi) = \true \\
            \sr(\varphi) & \text{if } \sr(\psi) = \false \\
            \true & \text{if } \sr(\varphi) = \sr(\psi) \\
            \lnot \sr(\varphi) \lor \sr(\psi) & \text{otherwise}
        \end{array}%
        \right.  \\
        \sr(\forall_V . \varphi) &=& \left\{
        \begin{array}{ll}
            \true & \text{if } n = 0\\
            \lnot \exists_{V \cap \free(\psi)}. \psi & \text{if } \sr(\varphi) = \lnot \psi\\
            (\forall_{V \cap \free(\psi_1)} .\psi_1) \wedge (\forall_{V \cap \free(\psi_1)}. \psi_2) & \text{if } \sr(\varphi) = \psi_1 \wedge \psi_2\\
            (\forall_{V \cap \free(\psi_1)}. \psi_1) \vee \psi_2 & \text{if } \sr(\varphi) = \psi_1 \vee \psi_2\\
            \psi_1 \vee (\forall_{V \cap \free(\psi_2)}. \psi_2) & \text{if } \sr(\varphi) = \psi_1 \vee \psi_2\\
            \forall_{V \cap \free(\varphi)}. \sr(\varphi) & \text{otherwise}
        \end{array}%
        \right.  \\
        & & \text{where } \forall_{\emptyset} .\varphi = \varphi.\\
        \sr(\exists_V . \varphi) &=& \left\{
        \begin{array}{ll}
            \true & \text{if } n = 0\\
            \lnot \forall_{V \cap \free(\psi)}. \psi & \text{if } \sr(\varphi) = \lnot \psi\\
            (\exists_{V \cap \free(\psi_1)} .\psi_1) \vee (\exists_{V \cap \free(\psi_1)}. \psi_2) & \text{if } \sr(\varphi) = \psi_1 \vee \psi_2\\
            (\exists_{V \cap \free(\psi_1)}. \psi_1) \wedge \psi_2 & \text{if } \sr(\varphi) = \psi_1 \wedge \psi_2\\
            \psi_1 \wedge (\exists_{V \cap \free(\psi_2)}. \psi_2) & \text{if } \sr(\varphi) = \psi_1 \wedge \psi_2\\
            \exists_{V \cap \free(\varphi)}. \sr(\varphi) & \text{otherwise}
        \end{array}%
        \right.  \\
        & & \text{where } \exists_{\emptyset} .\varphi = \varphi.
    \end{eqnarray*}

where `b` is a Boolean data variable, `e` is a data expression and `\varphi, \psi, \psi_1` and `\psi_2` are PBES expressions.

One-point-rule quantifier elimination rewriter
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Before presenting the rewriter itself, we need to define the function `\Equ`.
Function `\Equ` computes a set of equalities and inequalities for a data expression `f`, such that the following holds:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Equ(f) &=& (\{d_1 = e_1, \ldots, d_n = e_n\}, W) \implies f \equiv f' \wedge \bigwedge_{i=1}^n (d_i = e_i)\\
        \Equ(f) &=& (V, \{d_1 \neq e_1, \ldots, d_n \neq e_n\}) \implies f \equiv f' \vee \bigvee_{i=1}^n (d_i \neq e_i)\\
    \end{eqnarray*}

for some data expression `f'`, data variables `d_i` and data expressions `e_i`.

The function `\Equ` is inductively defined as follows:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Equ(\true) &=& (\emptyset, \top)\\
        \Equ(\false) &=& (\top, \emptyset)\\
        \Equ(b) &=& (\{b = \true\}, \{b = \false\})\\
        \Equ(d=e) &=& \left\{
        \begin{array}{cc}
            (\{ d = e \}, \emptyset) & \text{if } d \notin \FV(e)\\
            (\emptyset, \emptyset) & \text{otherwise}
        \end{array}%
        \right.  \\
        \Equ(e=d) &=& \Equ(d=e)\\
        \Equ(d \neq e) &=& \left\{
        \begin{array}{cc}
            (\emptyset, \{ d \neq e \}) & \text{if } d \notin \FV(e)\\
            (\emptyset, \emptyset) & \text{otherwise}
        \end{array}%
        \right.  \\
        \Equ(e \neq d) &=& \Equ(d \neq e)\\
        \Equ(\neg e) &=& \mathit{swap}(\Equ(e))\\
        \Equ(e \wedge e') &=& \mathit{join\_and}(\Equ(e), \Equ(e'))\\
        \Equ(e \vee e') &=& \mathit{join\_or}(\Equ(e), \Equ(e'))\\
        \Equ(e \implies e') &=& \mathit{join\_or}(\mathit{swap}(\Equ(e)), \Equ(e'))\\
        \Equ(\mathit{if}(e,e',e'')) &=& \left\{
        \begin{array}{c}
            ((V_e \cup V_{e'}) \cap (W_e \cup V_{e''}), (V_e \cup W_{e'}) \cap (W_e \cup W_{e''})) \\
            \text{where }(V_e, W_{e}) = \Equ(e), (V_{e'}, W_{e'}) = \Equ(e') \text{ and } (V_{e''}, W_{e''}) = \Equ(e'')
        \end{array}%
        \right.  \\
        \Equ(\forall x . e) &=& \mathit{delete}(x, \Equ(e))\\
        \Equ(\exists x . e) &=& \mathit{delete}(x, \Equ(e))\\
        \Equ(e) &=& (\emptyset, \emptyset) \text{ otherwise}\\
    \end{eqnarray*}


where `b` is a Boolean data variable, `d` is a data variable, `e,e',` and `e''` are data expressions, `\top` is the set of all equalities and inequalities, and:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \mathit{swap}(V, W) &=& (W,V)\\
        \mathit{join\_and}((V_1, W_1), (V_2, W_2)) &=& (V_1 \cup V_2, W_1 \cup W_2)\\
        \mathit{join\_or}((V_1, W_1), (V_2, W_2)) &=& (V_1 \cap V_2, W_1 \cap W_2)\\
        \mathit{delete}(x, (V,W)) &=& (V_1, W_1) \text{ where} \left\{
        \begin{array}{c}
            V_1 = \{ d = e' \in V \mid d \neq x \wedge x \notin \FV(e)\}\\
            W_1 = \{ d \neq e' \in W \mid d \neq x \wedge x \notin \FV(e)\}
        \end{array}%
        \right.  \\
    \end{eqnarray*}


Function `\Equ` is extended to PBES expressions, consider `\varphi`, as follows:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \Equ(\varphi) &=& (\{d_1 = e_1, \ldots, d_n = e_n\}, W) \implies \varphi \equiv \psi \wedge \bigwedge_{i=1}^n (d_i = e_i)\\
        \Equ(\varphi) &=& (V, \{d_1 \neq e_1, \ldots, d_n \neq e_n\}) \implies \varphi \equiv \psi \vee \bigvee_{i=1}^n (d_i \neq e_i)\\
    \end{eqnarray*}

for some data expression `\psi`. The inductive defition of `\Equ` is extended with the following:


.. math::
    :nowrap:

    \begin{eqnarray*}
        \Equ(\neg \varphi) &=& \mathit{swap}(\Equ(\varphi))\\
        \Equ(\varphi \wedge \psi) &=& \mathit{join\_and}(\Equ(\varphi), \Equ(\psi))\\
        \Equ(\varphi \vee \psi) &=& \mathit{join\_or}(\Equ(\varphi), \Equ(\psi))\\
        \Equ(\varphi \implies \psi) &=& \mathit{join\_or}(\mathit{swap}(\Equ(\varphi)), \Equ(\psi))\\
        \Equ(\forall x . \varphi) &=& \mathit{delete}(x, \Equ(\varphi))\\
        \Equ(\exists x . \varphi) &=& \mathit{delete}(x, \Equ(\varphi))\\
        \Equ(\varphi) &=& (\emptyset, \emptyset) \text{ otherwise}\\
    \end{eqnarray*}

We define the *one-point-rule* rewriter `\qr` inductively as follows:

.. _rewriter-onepoint:

.. math::
    :nowrap:

    \begin{eqnarray*}
        \qr(\lnot \varphi ) &=&\lnot \qr(\varphi ) \\
        \qr(\varphi \wedge \psi ) &=& \qr(\varphi )\wedge \qr(\psi ) \\
        \qr(\varphi \vee \psi ) &=& \qr(\varphi )\vee \qr(\psi )\\
        \qr(\varphi \Rightarrow \psi ) &=& \qr(\varphi ) \Rightarrow \qr(\psi ) \\
        \qr(\forall x.\varphi ) &=&\left\{
        \begin{array}{cc}
            \qr(\varphi)[x := e] & \text{if } x \neq e \in W \text{ , where } (V,W) = \Equ(\varphi)\\
            \forall x . \qr(\varphi) & \text{ otherwise}
        \end{array}%
        \right.  \\
        \qr(\exists x.\varphi ) &=&\left\{
        \begin{array}{cc}
            \qr(\varphi)[x := e] & \text{if } x = e \in V \text{ , where } (V,W) = \Equ(\varphi)\\
            \exists x . \qr(\varphi) & \text{ otherwise}
        \end{array}%
        \right.  \\
        \qr(\varphi) &=& \text{ otherwise}\\
    \end{eqnarray*}


Rewriter `Rw^*`
"""""""""""""""

.. admonition:: Disclaimer on rewriter `Rw^*`
    :class: warning

    This rewriter is effective only if we are solving a PBES with evidence information after having effectively solved the corresponding PBES without evidence information; otherwise the rewriter does nothing.

Rewriter `Rw^*` aims to remove irrelevant PVIs while instantiating a PBES with information about evidence, by exploiting information obtained after having solved the same PBES but without evidence information. 

The rewriter relies on two sets of PVIs: `\rnknodes` is a set of PVIs without evidence information and `\lsetpre= \{R(Z^{\pm}_a(d_1, \ldots, d_n)) \mid \forall  i \in 1, \ldots, n . d_i \in D, a \in \act\}` is a set of PVIs that encode information about evidence, where `\act` is the set of actions present in the summands of the original LPS.
If the solution of the PBES without evidence information is *true* then `Z^{\pm} = Z^{+}`, otherwise `Z^{\pm}= Z^{-}`. 

The PVIs belonging to `\rnknodes` or `\lsetpre` are relevant and must be kept untouched by the rewriter.

If the solution of the PBES without evidence information is *true* then the rewriter is as follows:

.. _rewrite-star:

.. math::
    :nowrap:

    \begin{eqnarray*}
    Rw^*(X(e), \rnknodes, \lsetpre) &=&\left\{
        \begin{array}{cc}
            \false & \text{if $X(e) \notin \rnknodes$ and $X(e) \notin \lsetpre$},\\
            X(e) & \text{otherwise}
        \end{array}%
        \right.  \\
    Rw^*(p, \rnknodes, \lsetpre) &=&p \\
    Rw^*(\lnot \varphi, \rnknodes, \lsetpre ) &=&\lnot Rw^*(\varphi, \rnknodes, \lsetpre ) \\
    Rw^*(\varphi \wedge \true, \rnknodes, \lsetpre) &=&Rw^*(\varphi, \rnknodes, \lsetpre)\\
    Rw^*(\varphi \vee \false, \rnknodes, \lsetpre) &=&Rw^*(\varphi, \rnknodes, \lsetpre)\\
    Rw^*(\varphi \vee \true, \rnknodes, \lsetpre ) &=& \true\\
    Rw^*(\varphi \wedge \false, \rnknodes, \lsetpre) &=& \false\\
    Rw^*(\varphi \oplus \psi, \rnknodes, \lsetpre ) &=&Rw^*(\varphi, \rnknodes, \lsetpre )\oplus Rw^*(\psi,\rnknodes, \lsetpre ) \\
    %Rw^*(\varphi \oplus \psi, \rnknodes, \lsetpre ) &=&Rw^*(\psi \oplus \varphi, \rnknodes, \lsetpre) \\
    Rw^*(\varphi \Rightarrow \psi, \rnknodes, \lsetpre ) &=&Rw^*(\lnot \varphi, \rnknodes, \lsetpre )\ \vee Rw^*(\psi, \rnknodes, \lsetpre ) \\
    Rw^*( \bigvee \{\varphi_1, \ldots,\varphi_n\}, \rnknodes, \lsetpre) &=&Rw^*(\varphi_1, \rnknodes, \lsetpre)\vee \ldots \vee Rw^*(\varphi_n, \rnknodes, \lsetpre ) \\
    Rw^*( \bigwedge \{\varphi_1, \ldots,\varphi_n\}, \rnknodes, \lsetpre) &=&Rw^*(\varphi_1, \rnknodes, \lsetpre)\wedge \ldots \wedge Rw^*(\varphi_n, \rnknodes, \lsetpre ) \\
    \end{eqnarray*}

where `\oplus \in \{\vee ,\wedge\}`, `p` a data expression and, `\varphi`, `\varphi_i` and `\psi` PBES expressions.

While, if the solution of the PBES without evidence information is *false* the rewriter changes in the following case:

.. _rewrite-star-if:

.. math::
  :nowrap:

  \begin{eqnarray*}
  Rw^*(X(e), \rnknodes, \lsetpre) &=&\left\{
      \begin{array}{cc}
          \true & \text{if $X(e) \notin \rnknodes$ and $X(e) \notin \lsetpre$},\\
          X(e) & \text{otherwise}
      \end{array}%
      \right. 
  \end{eqnarray*}

PBES instantiation algorithm
----------------------------

Algorithm :ref:`PbesInstStructureGraph <instantiation_algorithm>` gets as input a PBES `\E`, its initial state `X_\init(e_\init)`, rewriter `R` which first applies :ref:`simplify rewriter <rewriter-simplify>` and then :ref:`one-point-rule rewriter <rewriter-onepoint>`, a set of edges `E^0` (can be empty), a set of vertices `\lsetpre` (can be empty), and generates a structure graph `(V,E,d,r)`.

Let PBES `\E` be `(\sigma_1 X_1 (d_1 \colon D_1) = \varphi_1) \ldots (\sigma_n X_n (d_n \colon D_n) = \varphi_n)`, then with `\psi_{X_k}^e`, for `k \in 1,\ldots, n`, we indicate the right-hand side of the equation for `X_k` where `e` is substituted to `d_k` and then the rewriter `R` is applied, namely `R(\varphi_{X_k}[d_k:= e])`.
Sets `E^0` and `\lsetpre` are the input for :ref:`rewriter <rewrite-star>` `Rw^*` --the set `\rnknodes` used in :ref:`rewriter <rewrite-star>` `Rw^*` is here defined as the set of PVIs that are successors of node `X_k(e)` in the set of edges `E^0`.
In case the structure graph of PBES `\E` without evidence information has been already solved, then `E^0` is the set of relevant dependencies (edges) between nodes that do not encode information about evidence, and `\lsetpre` is the set of nodes that encode information about evidence. Otherwise, the sets are empty.

Algorithm :ref:`PbesInstStructureGraph <instantiation_algorithm>` uses :ref:`function <sg0>` `SG^0` to compute nodes and edges of the structure graph. 
Function `SG^0` uses functions :ref:`decoration <decoration>` `\dec` and :ref:`rank <rank>` `\rnk` to assign decorations and ranks to the nodes in the structure graph.

.. _decoration:

.. admonition:: Definition (Decoration)

    Let `\varphi, \psi` and `\psi_i` for `0<i < n+1`, `\dec` is a partial function that assigns a decoration to a node in `V`, where `-` stands for undefined:

    .. math::
        :nowrap:

        \[\dec(\psi) = 
        \begin{cases}
            \top & \text{if $\psi = \true$,} \\
            \bot & \text{if $\psi = \false$,} \\
            \blacktriangle & \text{if $\psi = \psi_1 \wedge \ldots \wedge \psi_n$,} \\
            \blacktriangledown & \text{if $\psi = \psi_1 \vee \ldots \vee \psi_n$,} \\
            %\blacktriangle & \text{if $\psi = \psi_1 \wedge \psi_2$,} \\
            %\blacktriangledown & \text{if $\psi = \psi_1 \vee\psi_2$,} \\
            - & \text{otherwise}
        \end{cases}\]

    where we assume that in `\psi _{1}\wedge \cdots \wedge \psi _{n}` none of the `\psi _{i}` is a conjunction, and in `\psi _{1}\vee \cdots \vee \psi _{n}` none of the `\psi _{i}` is a disjunction.


.. _rank:

.. admonition:: Definition (Rank)

  Every predicate variable bound in PBES `\E` is assigned a rank, where `\rnk_{\E}(X) \leq \rnk_{\E}(Y)` if `X` occurs before `Y` in `\E`, and `\rnk_{\E}(X)` is even is and only if `X` is labelled with a greatest fixpoint. 
  
In the following, rank is undefined, denoted with `-`, if not applied to a predicate variable.

Function `SG^0` is defined as follows:

.. _sg0:

.. admonition:: Definition (:math:`SG^0`)

    .. math::

        SG^0(\varphi,\psi, \dec(\psi), \rnk(\varphi)) = 
        \begin{align*}
        \begin{cases}
            (\{\varphi\}, \emptyset, \dec(\psi), \rnk(\varphi)) &\text{if $\psi = \true$ or $\psi = \false$,} \\
            (\{\varphi, \psi\}, \{(\varphi, \psi)\}, \dec(\psi), \rnk(\varphi)) &\text{if $\psi = Y(e)$,} \\
            (\{\varphi\}, \{(\varphi, \psi_1), \ldots, (\varphi, \psi_n)\}, \dec(\psi), \rnk(\varphi)) \cup SG^1(\psi_i, \dec(\psi_i)) &\text{otherwise} 
            %(\{\varphi\}, \{(\varphi, \psi_1),(\varphi, \psi_2)\}, \dec(\psi), \rnk(\varphi)) \cup SG^1(\psi_i, \dec(\psi_i)) &\text{otherwise} 
        \end{cases}
        \end{align*}

    where the *otherwise* case considers `\psi = \psi_1 \wedge \cdots \wedge \psi_n` or `\psi = \psi_1 \vee \cdots \vee \psi_n` and for which we assume that in `\psi _{1}\wedge \cdots \wedge \psi _{n}` none of the `\psi _{i}` is a conjunction, and in `\psi _{1}\vee \cdots \vee \psi _{n}` none of the `\psi _{i}` is a disjunction. And where `SG^1(\psi_i, \dec(\psi_i))` returns a tuple where nodes, edges, decoration and rank are computed for subformulas `\psi_i` of `\psi`.


We define `SG^1` as follows:

.. _sg1:

.. admonition:: Definition (:math:`SG^1`)

    .. math::

        SG^1(\psi, \dec(\psi)) = 
        \begin{align*}
        \begin{cases}
            \mathit{undefined} &\text{if $\psi = Y(e)$}\\
            (\{\psi\}, \emptyset, \dec(\psi), -) &\text{if $\psi = \true$ or $\psi = \false$,} \\
            (\{\psi\}, \{(\psi, \psi_1), \ldots, (\psi, \psi_n)\}, \dec(\psi), -) \cup SG^1(\psi_i, \dec(\psi_i)) &\text{otherwise}
        \end{cases}
        \end{align*}

    where the *otherwise* case considers `\psi = \psi_1 \wedge \cdots \wedge \psi_n` or `\psi = \psi_1 \vee \cdots \vee \psi_n` and for which we assume that in `\psi _{1}\wedge \cdots \wedge \psi _{n}` none of the `\psi _{i}` is a conjunction, and in `\psi _{1}\vee \cdots \vee \psi _{n}` none of the `\psi _{i}` is a disjunction.

Note that, for `SG^1`, if `\psi = Y(e)` then the node `Y(e)` is not created, but the edge to it is created. This is because either the node has already been created or it will be created (since it is a predicate variable). 


The instantiation algorithm :ref:`PbesInstStructureGraph <instantiation_algorithm>` is as follows:

.. _instantiation_algorithm:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \Function {PbesInstStructureGraph}{$\E, X_{\init}(e_{\init}), R, E^0, \lsetpre$}
        \State $\init := X_{\init}(e_{\init})$
        \State $\td := \{\init\}$
        \State $\discovered := \{\init\}$
        \State $(V,E,d,r) := (\emptyset, \emptyset, -, -)$
        \State $\textbf{while}\ \td \neq \emptyset \ \textbf{do}$
            \State $\qquad \textbf{choose}\ X_k(e) \in \td$ 
            \State $\qquad \td := \td \setminus \{X_k(e)\}$
            \State $\qquad \psi_{X_k}^e := R(\varphi_{X_k}[d_k := e])$
            \State $\qquad \psi_{X_k}^e := Rw^*(\psi_{X_k}^e, \{X \mid (X_k(e), X) \in E^0\}, \lsetpre)$
            \State $\qquad (V', E', d', r') := SG^0(X_k(e), \psi_{X_k}^e, \dec(\psi_{X_k}^e), \rnk(X_k(e)))$
            \State $\qquad (V,E, d, r) := (V \cup V',E \cup E',d'\lceil d,r' \lceil r)$
            \State $\qquad \td := \td \cup (\occ(\psi_{X_k}^e) \setminus \discovered)$
            \State $\qquad \discovered := \discovered \cup \occ(\psi_{X_k}^e)$
        \State \Return $(V,E, d, r)$
    \EndFunction
    \end{algorithmic}

The algorithm initializes sets `\td` and `\discovered` to the initial state `X_{\init}(e_{\init})`. Set `\td` contains those PVIs that still need to be explored and set `\discovered` that contains PVIs that have been discovered but for which the corresponding right-hand side equation may not have been computed yet.
While the set `\td` is not empty then an element (`X_k(e)`) of `\td` is choosen to be the current candidate. The element is removed from the `\td` set.
In line 9, the rewriter `R` is applied to the right-hand side of the equation for `X_k` where `e` is substituted to `d_k`, and the result is stored in `\psi_{X_k}^e`. In line 10, rewriter `Rw^*` is applied to `\psi_{X_k}^e` and sets `\{X \mid (X_k(e), X) \in E^0\}` and `\lsetpre` are those containing relevant PVIs that should be not rewritten (note that this only applies if a PBES without evidence information as been solved previously, othewise the sets are empty). In line 12, `SG^0` computes nodes and edges to be added to the structure graph based on the current PVI that is considered and its right-hand side `\psi_{X_k}^e`.
In line 13, the nodes and edges discovered by `SG^0` are added to the actual structure graph that will be returned.
Afterwards, sets `\td` and `\discovered` are updated such that the former will be extended with the occurrences found in `\psi_{X_k}^e` that have not been discovered yet, and the latter is extended with the occurrences found in `\psi_{X_k}^e`.

Solving strategies for PBES instantiation
-----------------------------------------

The tool ``pbessolve`` has a flag that is used to set the solving strategy. The flas is ``--solve-strategy=NAME`` (for short ``-sNAME``) where ``NAME`` goes from ``0`` to ``4``. The :ref:`list of solving strategies <list-opt>` is as follows:

.. _list-opt:

.. admonition:: List of solving strategies

    .. math::
        :nowrap:
        :class: math-left

        \begin{itemize}
            \item 0- No on-the-fly solving is applied
            \item 1- Propagate solved equations using an attractor
            \item 2- Detect winning loops
            \item 3- Solve subgames using a fatal attractor
            \item 4- Solve subgames using the solver
        \end{itemize}


Solving strategy *0- No on-the-fly solving is applied* is the combination of two techniques: *self-loop removal* and *propagation of solved equations*.
Thus, we first present these two techniques.

Self-loop removal
"""""""""""""""""

The lemma below indicated that one can simplify the PBES equation that is being created without affecting the solution to the PBES.

.. _self-loop-removal:
 
.. admonition:: Lemma 1

  The solution to all variables in a PBES `\mathcal{E} (\sigma X(d:D) = \varphi) \mathcal{E}'` is equivalent to the solution to those variables in the PBES `\mathcal{E} (\sigma X(d:D)=` `\varphi[X := \lambda d':D . (d \approx d' \implies b_\sigma) \wedge (d \not \approx d' \implies X(d'))]) \mathcal{E}'`, where `b_\sigma = \true` if `\sigma = \nu` and `b_\sigma = \false` if `\sigma = \mu`.

Using this lemma in the instantiation algorithm, rather than creating a structure graph underlying the equation `\sigma X_k(d_k:D) = \psi_{X_k}^e`, we can create a structure graph for `\sigma X_k(d_k:D) = \psi_{X_k}^e[X_k(e) := b_\sigma]`. This can be done by adding, in Algorithm :ref:`PbesInstStructureGraph0 <instantiation-no-on-the-fly>`, the assignment `\psi_{X_k}^e := R(\psi_{X_k}^e[X(e) := b_\sigma])` below assignment `\psi_{X_k}^e := R(\varphi_{X_k}[d_k := e])`.

Propagation of solved equations
"""""""""""""""""""""""""""""""

This technique exploits the fact that some of the PBES equations that are generated while exploring the PBES are already solved (possibly after using the self-loop removal technique).

We first introduce some additional notation. By `S_0` we denote the set of vertices that represent equations with solution `\true`, whereas `S_1` denotes the set of vertices representing equations with solution `\false`.  Let `\pi` be a partial function that maps vertices to the PVIs they represent. For a set of vertices `S \subseteq V`, we define the substitution `\rho_i` as follows for all `s \in S \cap \textsf{dom}(\pi): \rho_i(\pi(s)) = \true` if `i = 0` and `\rho_i(\pi(s)) = \false` if `i = 1`. The union of two substitutions is again a substitution, provided that the domain of variables these substitutions range over are disjoint.

The lemma below indicates how one can utilise such information to simplify the PBES equation that is being created, again without affecting the solution to the PBES.

.. _propagate-solve-eq:

.. admonition:: Lemma 2

  The solution to all variables in a PBES `\mathcal{F} \equiv \mathcal{E} (\sigma X(d:D) = \varphi) \mathcal{E}'` is equivalent to the solution to those variables in the PBES `\mathcal{F}' \equiv \mathcal{E} (\sigma X(d:D) = \varphi(\rho_0(S_0) \cup \rho_1(S_1))) \mathcal{E}'`, where for all `S_0 \cup S_1 \subseteq \{v \in V \mid \forall \theta,\theta': \lbrack \mathcal{F} \rbrack \theta(\pi(v)) =   \lbrack \mathcal{F'} \rbrack \theta'(\pi(v))\}`, where `\lbrack \mathcal{F} \rbrack \theta` denotes the solution to `\mathcal{F}` under environment `\theta`.

Using this lemma, rather than creating a structure graph underlying the equation `\sigma X_k(d_k:D) = \psi_{X_k}^e`, we can create a structure graph for `\sigma X_k(d_k:D) = \psi_{X_k}^e(\rho_0(S_0) \cup \rho_1(S_1))`, provided that `S_0` and `S_1` contain vertices that represent solved equations.

We implement this using a rewriter `R^+` which takes a formula (and implicitly takes sets `S_0` and `S_1` into account). This leads to the following bottom-up procedure in which `R^+(\varphi)` yields a tuple `(b, \varphi',g_0,g_1)`, where `b` is either a Boolean value, or the value `\bot`, and `\varphi'` is a predicate formula that is equivalent to `\varphi` under the assumption that `S_0` and `S_1` are solved, but the formula `\varphi` is not fully solved using that information. Predicate formula `g_0` (resp. `g_1`) is a conjunctive (resp. disjunctive) formula, representing `\varphi` in case `b` is true (resp. false); in case `b` is true, the conjunctive formula `g_0` can be used to construct a witness, as it contains exactly all dependencies on `S_0` that are needed to make formula `\varphi` hold. Dually for formula `g_1`.

.. _rewrite-plus:

.. math::
    :nowrap:

    \begin{eqnarray*}
    R^+(\true) &=& (\true,\true, \true,\false) \\
    R^+(\false) &=& (\false,\false,\true,\false) \\
    R^+(X(e)) &=& \left\{
        \begin{array}{ll}
            (\true,X(e),X(e),\false) & \text{if $X(e) \in S_0$}\\
            (\false, X(e),\true,X(e)) & \text{if $X(e) \in S_1$} \\
            (\bot, X(e),\true,\false) & \text{otherwise}
        \end{array}%
        \right.  \\
    R^+(\varphi_1 \wedge \varphi_2) &=& \left\{
        \begin{array}{ll}
            (\true, \varphi_1' \wedge \varphi_2',g_0^1 \wedge g_0^2,\false) & \text{if $b_1 = \true$ and $b_2 = \true$}\\
            (\false, \varphi_1',\true,g_1^1) & \text{if $b_1 = \false$ and ($b_2 \neq \false$ or ($b_2 = \false$ and $|g_1^1| < |g_1^2|$))} \\
            (\false,\varphi_2',\true,g_1^2) & \text{if $b_2 = \false$ and ($b_1 \neq \false$ or ($b1 = \false$ and $|g_1^2| < |g_1^1|$))}\\
            (\bot, \varphi_1' \wedge \varphi_2',\true,\false) & \text{if $b_1 = \bot$ and $b_2 = \bot$}
        \end{array}%
        \right.  \\
    R^+(\varphi_1 \vee \varphi_2) &=& \left\{
        \begin{array}{ll}
            (\false, \varphi_1' \vee \varphi_2',\true,g_1^1\vee g_1^2) & \text{if $b_1 = \false$ and $b_2 = \false$}\\
            (\true, \varphi_1',g_0^1,\false) & \text{if $b_1 = \true$ and ($b_2 \neq \true$ or ($b_2 = \true$ and $|g_0^1| < |g_0^2|$))} \\
            (\true,\varphi_2',g_0^2,false) & \text{if $b_2 = \true$ and ($b_1 \neq \true$ or ($b1 = \true$ and $|g_0^2| < |g_0^1|$))}\\
            (\bot, \varphi_1' \vee \varphi_2',\true,\false) & \text{if $b_1 = \bot$ and $b_2 = \bot$}
        \end{array}%
        \right.
    \end{eqnarray*}

0- No on-the-fly solving is applied
"""""""""""""""""""""""""""""""""""

In Algorithm :ref:`PbesInstStructureGraph0 <instantiation-no-on-the-fly>` we present solving strategy *0- No on-the-fly solving is applied*. This is a combination of two techniques: `\colorbox{lightgray}{self-loop removal}` and `\colorbox{orange}{propagation of solved equations}`:

.. _instantiation-no-on-the-fly:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \Function {PbesInstStructureGraph0}{$\E, X_{\init}(e_{\init}), R, E^0, \lsetpre$}
        \State $\init := X_{\init}(e_{\init})$
        \State $\td := \{\init\}$
        \State $\discovered := \{\init\}$
        \State $(V,E,d,r) := (\emptyset, \emptyset, -, -)$
        \State {\colorbox{orange}{$S_0 := \emptyset$}}
        \State {\colorbox{orange}{$S_1 := \emptyset$}}
        \State $\textbf{while}\ \td \neq \emptyset \ \textbf{do}$
            \State $\qquad \textbf{choose}\ X_k(e) \in \td$ 
            \State $\qquad \td := \td \setminus \{X_k(e)\}$
            \State $\qquad \psi_{X_k}^e := R(\varphi_{X_k}[d_k := e])$
            \State \qquad{\colorbox{lightgray}{$\psi_{X_k}^e := \mathbf{if}\ \sigma_k = \mu\ \mathbf{then}\ R(\psi_{X_k}^e[X_k(e) := \false])\ \mathbf{else}\ R(\psi_{X_k}^e[X_k(e) := \true]) $}}
            \State \qquad{\colorbox{orange}{$(b,\psi_{X_k}^e,g_0,g_1) := R^+(\psi_{X_k}^e)$}}
            \State \qquad {\colorbox{orange}{$\mathbf{if}\ b = \true\ \mathbf{then}$}}
                \State \qquad \qquad {\colorbox{orange}{$S_0 := S_0 \cup \{X_k(e)\}$}}
                \State \qquad \qquad {\colorbox{orange}{$\psi_{X_k}^e := g_0$}}
            \State \qquad {\colorbox{orange}{$\mathbf{if}\ b = \false\ \mathbf{then}$}}
                \State \qquad \qquad {\colorbox{orange}{$S_1 := S_1 \cup \{X_k(e)\}$}}
                \State \qquad \qquad {\colorbox{orange}{$\psi_{X_k}^e := g_1$}}
            \State $\qquad \psi_{X_k}^e := Rw^*(\psi_{X_k}^e, \{X \mid (X_k(e), X) \in E^0\}, \lsetpre)$
            \State $\qquad (V', E', d', r') := SG^0(X_k(e), \psi_{X_k}^e, \dec(\psi_{X_k}^e), \rnk(X_k(e)))$
            \State $\qquad (V,E, d, r) := (V \cup V',E \cup E',d'\lceil d,r' \lceil r)$
            \State $\qquad \td := \td \cup (\occ(\psi_{X_k}^e) \setminus \discovered)$
            \State $\qquad \discovered := \discovered \cup \occ(\psi_{X_k}^e)$
        \State \Return $(V,E, d, r)$
    \EndFunction
    \end{algorithmic}

Self-loop removal example
~~~~~~~~~~~~~~~~~~~~~~~~~

.. admonition:: Example 1
    :class: admonition-example

    Let us consider the following mCRL2 specification

    ::

        act b;

        proc L(n: Nat) = (n==1) -> b . L(2)
                    + (n==2) -> b.  L(2);
        init L(1);

    The labelled transition system underlying this specification is

    .. math::
        :nowrap:
        :class: math-tikz

        \begin{tikzpicture}
                \tikzstyle{every state}=[
                    draw,
                    shape=circle,
                    inner sep=1pt,
                    minimum size=8pt,
                    node distance=50pt,
                    initial text={},
                    auto]
            \node [state, initial] (1) {};
            \node [state, right= of 1] (2) {};
            
            \draw[->] 
            (2) edge[loop above] node {$b$} (2)
            (1) edge[above] node {$b$} (2)
            ;
        \end{tikzpicture}

    The property we verify is 

    ::

        mu V.(<b>V || nu W. <true>W ).


    Let us consider the PBES without evidence information

    ::

        pbes mu X(n_L: Nat) =
            (exists e_L: Bool. val(if(e_L, n_L == 2, n_L == 1)) && X(2)) || Y(n_L);
            nu Y(n_L: Nat) =
            exists e_L: Bool. val(if(e_L, n_L == 2, n_L == 1)) && Y(2);

        init X(1);

    If no optimization is applied then PBES is instantiated to the following structure graph

    .. math::
        :nowrap:
        :class: math-tikz

        \begin{tikzpicture}[scale=0.5,
            every node/.style={scale=0.8},
            node distance = 2cm]

            \node (X1) {${X(1)}_{\blacktriangledown\textcircled{1}}$};
            \node[below of=X1] (X2) {${X(2)}_{\blacktriangledown\textcircled{1}}$};
            \node[right of=X2,xshift=3.3cm] (Y1) {${Y(1)}_{-\textcircled{2}}$};
            \node[below of=X2] (Y2) {${Y(2)}_{-\textcircled{2}}$};
            
            \path[->]
            (X1) edge node {} (X2)
            (X2) edge[loop left] node {} (X2)
            (X1) edge node {} (Y1)
            (X2) edge node {} (Y2)
            (Y2) edge[loop left] node {} (Y2)
            (Y1) edge node {} (Y2)
            ;
        \end{tikzpicture}

    By the application of :ref:`self-loop removal <self-loop-removal>` the PBES is instantiated to the following structure graph

    .. math::
        :nowrap:
        :class: math-tikz

        \begin{tikzpicture}[scale=0.5,
            every node/.style={scale=0.8},
            node distance = 2cm]

            \node (X1) {${X(1)}_{\blacktriangledown\textcircled{1}}$};
            \node[below of=X1] (X2) {${X(2)}_{-\textcircled{1}}$};
            \node[right of=X2,xshift=3.3cm] (Y1) {${Y(1)}_{-\textcircled{2}}$};
            \node[below of=X2] (Y2) {${Y(2)}_{\top\textcircled{2}}$};
            
            \path[->]
            (X1) edge node {} (X2)
            (X1) edge node {} (Y1)
            (X2) edge node {} (Y2)
            (Y1) edge node {} (Y2)
            ;
        \end{tikzpicture}

    In this latter the self-loops in `X(2)` and `Y(2)` are removed.


0- No on-the-fly solving is applied - example
"""""""""""""""""""""""""""""""""""""""""""""

.. _example-solved:

.. admonition:: Example 2
    :class: admonition-example

    Let us consider the following mCRL2 specification

    ::

        act a;
        proc P(n:Nat) = (n < 50) -> a. P(n+1)
                        + (n == 10) -> a. P(n)
                        + (n == 9)  -> a. P(n+2)
                        + (n == 20) -> a. P(10);

        init P(0);

    The labelled transition system underlying this specification is (for the sake of clarity we also add numbers to identify the states)
    
    .. math::
        :nowrap:
        :class: math-tikz

        \begin{tikzpicture}
                \tikzstyle{every state}=[
                    draw,
                    shape=circle,
                    inner sep=1pt,
                    minimum size=8pt,
                    node distance=50pt,
                    initial text={},
                    auto]
            \node [state, initial] (0) {0};
            \node [state, right= of 0] (1) {9};
            \node [state, right= of 1] (2) {11};
            \node [state, above= of 2] (3) {10};
            \node [state, right= of 2] (4) {20};
            \node [state, right= of 4] (5) {50};
            
            \draw[dotted, ->] 
            (0) edge[above] node {$a$} (1)
            (2) edge[above] node {$a$} (4)
            (4) edge[above] node {$a$} (5)
            ;
            \draw[->] 
            (1) edge[above] node {$a$} (2)
            (1) edge[above] node {$a$} (3)
            (3) edge[left] node {$a$} (2)
            (3) edge[loop above] node {$a$} (3)
            (4) edge[above] node {$a$} (3)
            ;
        \end{tikzpicture}

    The property we verify is 

    ::
        
        nu X. <a>X.
    
    We again consider to instantiate the PBES without evidence information.

    If no optimization is applied then 51 equations are generated and are the following

    ::

        Y(0) = Y(1) with rank 0
        Y(1) = Y(2) with rank 0
        Y(2) = Y(3) with rank 0
        Y(3) = Y(4) with rank 0
        Y(4) = Y(5) with rank 0
        Y(5) = Y(6) with rank 0
        Y(6) = Y(7) with rank 0
        Y(7) = Y(8) with rank 0
        Y(8) = Y(9) with rank 0
        Y(9) = Y(11) || Y(10) with rank 0
        Y(11) = Y(12) with rank 0
        Y(10) = Y(10) || Y(11) with rank 0
        Y(12) = Y(13) with rank 0
        Y(13) = Y(14) with rank 0
        Y(14) = Y(15) with rank 0
        Y(15) = Y(16) with rank 0
        Y(16) = Y(17) with rank 0
        Y(17) = Y(18) with rank 0
        Y(18) = Y(19) with rank 0
        Y(19) = Y(20) with rank 0
        Y(20) = Y(10) || Y(21) with rank 0
        Y(21) = Y(22) with rank 0
        Y(22) = Y(23) with rank 0
        Y(23) = Y(24) with rank 0
        Y(24) = Y(25) with rank 0
        Y(25) = Y(26) with rank 0
        Y(26) = Y(27) with rank 0
        Y(27) = Y(28) with rank 0
        Y(28) = Y(29) with rank 0
        Y(29) = Y(30) with rank 0
        Y(30) = Y(31) with rank 0
        Y(31) = Y(32) with rank 0
        Y(32) = Y(33) with rank 0
        Y(33) = Y(34) with rank 0
        Y(34) = Y(35) with rank 0
        Y(35) = Y(36) with rank 0
        Y(36) = Y(37) with rank 0
        Y(37) = Y(38) with rank 0
        Y(38) = Y(39) with rank 0
        Y(39) = Y(40) with rank 0
        Y(40) = Y(41) with rank 0
        Y(41) = Y(42) with rank 0
        Y(42) = Y(43) with rank 0
        Y(43) = Y(44) with rank 0
        Y(44) = Y(45) with rank 0
        Y(45) = Y(46) with rank 0
        Y(46) = Y(47) with rank 0
        Y(47) = Y(48) with rank 0
        Y(48) = Y(49) with rank 0
        Y(49) = Y(50) with rank 0
        Y(50) = false with rank 0

    and the PBES is instantiated to the following structure graph

    .. math::
        :nowrap:
        :class: math-tikz

        \begin{tikzpicture}[scale=0.5,
            every node/.style={scale=0.8},
            node distance = 2.5cm]

            \node (0) {${Y(0)}_{-\textcircled{0}}$};
            \node [right of= 0,xshift=1.5cm] (1) {${Y(9)}_{\blacktriangledown\textcircled{0}}$};
            \node [right of= 1,xshift=1.5cm] (2) {${Y(11)}_{-\textcircled{0}}$};
            \node [above of= 2] (3) {${Y(10)}_{\blacktriangledown\textcircled{0}}$};
            \node [right of= 2,xshift=1.5cm] (4) {${Y(20)}_{-\textcircled{0}}$};
            \node [right of= 4,xshift=1.5cm] (5) {${Y(49)}_{-\textcircled{0}}$};
            \node [right of= 5,xshift=1.5cm] (6) {${Y(50)}_{\bot\textcircled{0}}$};
            
            \path[dotted, ->] 
            (0) edge node {} (1)
            (2) edge node {} (4)
            (4) edge node {} (5)
            ;
            \path[->] 
            (1) edge node {} (2)
            (1) edge node {} (3)
            (3) edge node {} (2)
            (3) edge[loop above] node {} (3)
            (4) edge node {} (3)
            (5) edge node {} (6)
            ;

        \end{tikzpicture}

    If :ref:`0- No on-the-fly solving is applied <instantiation-no-on-the-fly>` is applied then 21 equations are generated and are the following

    ::

        Y(0) = Y(1) with rank 0
        Y(1) = Y(2) with rank 0
        Y(2) = Y(3) with rank 0
        Y(3) = Y(4) with rank 0
        Y(4) = Y(5) with rank 0
        Y(5) = Y(6) with rank 0
        Y(6) = Y(7) with rank 0
        Y(7) = Y(8) with rank 0
        Y(8) = Y(9) with rank 0
        Y(9) = Y(11) || Y(10) with rank 0
        Y(11) = Y(12) with rank 0
        Y(10) = true with rank 0
        Y(12) = Y(13) with rank 0
        Y(13) = Y(14) with rank 0
        Y(14) = Y(15) with rank 0
        Y(15) = Y(16) with rank 0
        Y(16) = Y(17) with rank 0
        Y(17) = Y(18) with rank 0
        Y(18) = Y(19) with rank 0
        Y(19) = Y(20) with rank 0
        Y(20) = Y(10) with rank 0

    The instantiation procedure stops when it finds that `Y(10)`, which is solved, is successor of `Y(20)`, and the structure graph is simplified to the following

    .. math::
        :nowrap:
        :class: math-tikz

        \begin{tikzpicture}[scale=0.5,
            every node/.style={scale=0.8},
            node distance = 2.5cm]

            \node (0) {${Y(0)}_{-\textcircled{0}}$};
            \node [right of= 0,xshift=1.5cm] (1) {${Y(9)}_{\blacktriangledown\textcircled{0}}$};
            \node [right of= 1,xshift=1.5cm] (2) {${Y(11)}_{-\textcircled{0}}$};
            \node [above of= 2] (3) {${Y(10)}_{\top\textcircled{0}}$};
            \node [right of= 2,xshift=1.5cm] (4) {${Y(20)}_{-\textcircled{0}}$};
            
            \path[dotted, ->] 
            (0) edge node {} (1)
            (2) edge node {} (4)
            ;
            \path[->] 
            (1) edge node {} (2)
            (1) edge node {} (3)
            (4) edge node {} (3)
            ;
        \end{tikzpicture}

Optimizations 1-4
"""""""""""""""""

Solving strategies 1 to 4 each use a different function for `applyAttractor`, in Algorithm :ref:`PbesInstHolder <placeholder>`. These solving strategies are considered to be *optimizations* of solving strategy 0.

.. _placeholder:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \Function {PbesInstHolder}{$\E, X_{\init}(e_{\init}), R, E^0, \lsetpre$}
        \State $\init := X_{\init}(e_{\init})$
        \State $\td := \{\init\}$
        \State $\discovered := \{\init\}$
        \State $(V,E,d,r) := (\emptyset, \emptyset, -, -)$
        \State $S_0 := \emptyset$
        \State $S_1 := \emptyset$
        \State $\textbf{while}\ \td \neq \emptyset {\colorbox{lightgray}{$\wedge X_{\init}(e_{\init}) \notin S_0 \cup S_1$}}\ \textbf{do}$
            \State $\qquad \textbf{choose}\ X_k(e) \in \td$ 
            \State $\qquad \td := \td \setminus \{X_k(e)\}$
            \State $\qquad \psi_{X_k}^e := R(\varphi_{X_k}[d_k := e])$
            \State {\colorbox{white}{$\qquad \psi_{X_k}^e := \mathbf{if}\ \sigma_k = \mu\ \mathbf{then}\ R(\psi_{X_k}^e[X_k(e) := \false])\ \mathbf{else}\ R(\psi_{X_k}^e[X_k(e) := \true])$}}
            \State $\qquad (b,\psi_{X_k}^e,g_0,g_1) := R^+(\psi_{X_k}^e)$
            \State $\qquad \mathbf{if}\ b = \true\ \mathbf{then}$
                \State $\qquad \qquad S_0 := S_0 \cup \{X_k(e)\}$
                \State $\qquad \qquad \psi_{X_k}^e := g_0$
            \State $\qquad \mathbf{if}\ b = \false\ \mathbf{then}$
                \State $\qquad \qquad S_1 := S_1 \cup \{X_k(e)\}$
                \State $\qquad \qquad \psi_{X_k}^e := g_1$
            \State $\qquad \psi_{X_k}^e := Rw^*(\psi_{X_k}^e, \{X \mid (X_k(e), X) \in E^0\}, \lsetpre)$
            \State $\qquad (V', E', d', r') := SG^0(X_k(e), \psi_{X_k}^e, \dec(\psi_{X_k}^e), \rnk(X_k(e)))$
            \State $\qquad (V,E, d, r) := (V \cup V',E \cup E',d'\lceil d,r' \lceil r)$
            \State $\qquad \td := \td \cup (\occ(\psi_{X_k}^e) \setminus \discovered)$
            \State $\qquad \discovered := \discovered \cup \occ(\psi_{X_k}^e)$
            \State \qquad {\colorbox{lightgray}{$S_0, S_1, \tau_0 , \tau_1:= applyAttractor(S_0,  S_1, \tau_0, \tau_1)$ \text{(executed periodically)}}}
        \State {\colorbox{lightgray}{$V := \textsc{ExtractMinimalStructureGraph}(V, \init, S_0, S_1, \tau_0, \tau_1)$}}
        \State \Return $(V,E, d, r)$
    \EndFunction
    \end{algorithmic}

Routine :ref:`ExtractMinimalStructureGraph <extract-minimal>` is used to extract a minimal structure graph based on the strategies set in `applyAttractor`. The procedure returns a structure graph in which information about local winning strategies has been exploited to minimise the structure graph, preserving relevant information to construct evidence. The strategies `\tau_0` and `\tau_1` are assumed to be consistent with the sets `S_0` and `S_1`, i.e, be a closed and winning strategy for the vertices in those sets.

.. _extract-minimal:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \State $\mathbf{function}\ \textsc{ExtractMinimalStructureGraph}(V, \init, S_0, S_0, \tau_0, \tau_1)$
        \State $\qquad \td := {\init}$
        \State $\qquad \done := \emptyset$
        \State $\qquad \textbf{while}\ \td \neq \emptyset$
            \State $\qquad \qquad \textbf{choose}\ u \in \td$ 
            \State $\qquad \qquad \td := \td \setminus \{u\}$
            \State $\qquad \qquad \done := \done \cup \{u\}$
            \State $\qquad \qquad \mathbf{if}\ (u \in S_0 \wedge \dec(u) = \blacktriangledown)\ \mathbf{then}$
                \State $\qquad \qquad \qquad v := \tau_0(u)$
                \State $\qquad \qquad \qquad \mathbf{if}\ v \notin \done\ \mathbf{then}$
                        \State $\qquad \qquad \qquad \qquad  \td := \td \cup \{v\}$
            \State $\qquad \qquad \mathbf{else}\ \mathbf{if}\ (u \in S_1 \wedge \dec(u) = \blacktriangle)\ \mathbf{then}$
                \State $\qquad \qquad \qquad v := \tau_1(u)$
                \State $\qquad \qquad \qquad \mathbf{if}\ v \notin \done\ \mathbf{then}$
                    \State $\qquad \qquad \qquad \qquad  \td := \td \cup \{v\}$
            \State $\qquad \qquad \mathbf{else}$
                \State $\qquad \qquad \qquad \mathbf{for}\ v \in \scc(u) \mathbf{do}$
                    \State $\qquad \qquad \qquad \qquad \mathbf{if}\ v \notin \done\ \mathbf{then}$
                        \State $\qquad \qquad \qquad \qquad \qquad \td := \td \cup \{v\}$
        \State \qquad \Return $\done$
    \State $\mathbf{end\ function}$
    \end{algorithmic}


Since optimizations 1 to 4 each use a different function for `applyAttractor` that is *executed periodically*, to reproduce the results of :ref:`Example 3 <example-solved-attractor>`, :ref:`Example 4 <example-detect-loops>` and :ref:`Example 5 <example-solved-fatal>` flag ``--aggressive``, of tool ``pbessolve``, must be used.

1- Propagate solved equations using an attractor
""""""""""""""""""""""""""""""""""""""""""""""""

In Algorithm :ref:`PbesInstStructureGraph1 <propagate-solved-eq-attr>` we present the algorithm for optimization *1- Propagate solved equations using an attractor*. Algorithm :ref:`PbesInstStructureGraph1 <propagate-solved-eq-attr>` uses :ref:`AttrDefaultWithTau <attr-tau>`.

.. _propagate-solved-eq-attr:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \Function {PbesInstStructureGraph1}{$\E, X_{\init}(e_{\init}), R, E^0, \lsetpre$}
        \State $\init := X_{\init}(e_{\init})$
        \State $\td := \{\init\}$
        \State $\discovered := \{\init\}$
        \State $(V,E,d,r) := (\emptyset, \emptyset, -, -)$
        \State $S_0 := \emptyset$
        \State $S_1 := \emptyset$
        \State $\textbf{choose}\ X_k(e) \in \td$ 
        \State $\textbf{while}\ \td \neq \emptyset \wedge X_{\init}(e_{\init}) \notin S_0 \cup S_1\ \textbf{do}$
            \State $\qquad \textbf{choose}\ X_k(e) \in \td$ 
            \State $\qquad \td := \td \setminus \{X_k(e)\}$
            \State $\qquad \psi_{X_k}^e := R(\varphi_{X_k}[d_k := e])$
            \State \qquad {\colorbox{white}{$\psi_{X_k}^e := \mathbf{if}\ \sigma_k = \mu\ \mathbf{then}\ R(\psi_{X_k}^e[X_k(e) := \false])\ \mathbf{else}\ R(\psi_{X_k}^e[X_k(e) := \true])$}}
            \State $\qquad (b,\psi_{X_k}^e,g_0,g_1) := R^+(\psi_{X_k}^e)$
            \State $\qquad \mathbf{if}\ b = \true\ \mathbf{then}$
                \State $\qquad \qquad S_0 := S_0 \cup \{X_k(e)\}$
                \State $\qquad \qquad \psi_{X_k}^e := g_0$
            \State $\qquad \mathbf{if}\ b = \false\ \mathbf{then}$
                \State $\qquad \qquad S_1 := S_1 \cup \{X_k(e)\}$
                \State $\qquad \qquad \psi_{X_k}^e := g_1$
            \State $\qquad \psi_{X_k}^e := Rw^*(\psi_{X_k}^e, \{X \mid (X_k(e), X) \in E^0\}, \lsetpre)$
            \State $\qquad (V', E', d', r') := SG^0(X_k(e), \psi_{X_k}^e, \dec(\psi_{X_k}^e), \rnk(X_k(e)))$
            \State $\qquad (V,E, d, r) := (V \cup V',E \cup E',d'\lceil d,r' \lceil r)$
            \State $\qquad \td := \td \cup (\occ(\psi_{X_k}^e) \setminus \discovered)$
            \State $\qquad \discovered := \discovered \cup \occ(\psi_{X_k}^e)$
            \State \qquad {\colorbox{lightgray}{$S_0, \tau_0 := \textsc{AttrDefaultWithTau}(S_0, 0, \tau_0)$}}
            \State \qquad {\colorbox{lightgray}{$S_1, \tau_1 := \textsc{AttrDefaultWithTau}(S_1, 1, \tau_1)$}}
        \State $V := \textsc{ExtractMinimalStructureGraph}(V, \init, S_0, S_1, \tau_0, \tau_1)$
        \State \Return $(V,E, d, r)$
    \EndFunction
    \end{algorithmic}

.. _example-solved-attractor:

.. admonition:: Example 3
    :class: admonition-example

    Let us consider again :ref:`Example 2 <example-solved>`. 

    If :ref:`1- Propagate solved equations using an attractor <propagate-solved-eq-attr>` is applied then 12 equations are generated and are the following

    :: 

        Y(0) = Y(1) with rank 0
        Y(1) = Y(2) with rank 0
        Y(2) = Y(3) with rank 0
        Y(3) = Y(4) with rank 0
        Y(4) = Y(5) with rank 0
        Y(5) = Y(6) with rank 0
        Y(6) = Y(7) with rank 0
        Y(7) = Y(8) with rank 0
        Y(8) = Y(9) with rank 0
        Y(9) = Y(11) || Y(10) with rank 0
        Y(11) = Y(12) with rank 0
        Y(10) = true with rank 0


    and the PBES is instantiated to the following structure graph

    .. math::
        :nowrap:
        :class: math-tikz

        \begin{tikzpicture}[scale=0.5,
            every node/.style={scale=0.8},
            node distance = 2.5cm]

            \node (0) {${Y(0)}_{-\textcircled{0}}$};
            \node [right of= 0,xshift=1.5cm] (1) {${Y(9)}_{\blacktriangledown\textcircled{0}}$};
            \node [right of= 1,xshift=1.5cm] (2) {${Y(11)}_{-\textcircled{0}}$};
            \node [above of= 2] (3) {${Y(10)}_{\top\textcircled{0}}$};
            \node [right of= 2,xshift=1.5cm] (4) {${Y(12)}_{-\textcircled{0}}$};
            
            \path[dotted, ->] 
            (0) edge node {} (1)
            ;
            \path[->] 
            (1) edge node {} (3)
            (1) edge node {} (2)
            (2) edge node {} (4)
            ;
        \end{tikzpicture}
    
    The instantiation procedure does not explore further vertex `Y(12)` and, in turn, it finds that vertex `Y(10)` is solved. The information that vertex `Y(10)` is solved is propagated backwords. Consequently, strategy `\tau_{0}[Y(9)] = Y(10)` is set and the structure graph is simplified to the following minimal structure graph

    .. math::
        :nowrap:
        :class: math-tikz

        \begin{tikzpicture}[scale=0.5,
            every node/.style={scale=0.8},
            node distance = 2.5cm]

            \node (0) {${Y(0)}_{-\textcircled{0}}$};
            \node [right of= 0,xshift=1.5cm] (1) {${Y(9)}_{\blacktriangledown\textcircled{0}}$};
            \node [right of= 1,xshift=1.5cm] (2) {};
            \node [above of= 2] (3) {${Y(10)}_{\top\textcircled{0}}$};
            
            \path[dotted, ->] 
            (0) edge node {} (1)
            ;
            \path[->] 
            (1) edge node {} (3)
            ;
        \end{tikzpicture}

2- Detect winning loops
"""""""""""""""""""""""

In Algorithm :ref:`PbesInstStructureGraph2 <detect-loops>` we present the algorithm for optimization *2- Detect winning loops*. 
Algorithm :ref:`PbesInstStructureGraph2 <detect-loops>` uses :ref:`FindLoops2 <find-loops-2>`.
In :ref:`FindLoops2 <find-loops-2>` routine `dom(rnk)` yields the ranked vertices.

.. _find-loops-2:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \Function {FindLoops2}{$V, S_0, S_1, \tau_0, \tau_1$}
    \State $J := \{ j \mid \exists u \in V \cap \mathrm{dom}(\rnk) : \rnk(u) = j \}$
    \State $S_0, \tau_0 := \textsc{AttrDefaultWithTau}(S_0, 0, \tau_0)$
    \State $S_1, \tau_1 := \textsc{AttrDefaultWithTau}(S_1, 1, \tau_1)$
    \State $\mathbf{for}\ j \in J$
            \State $\qquad \alpha := j \bmod 2$
            \State \qquad {\colorbox{white}{$U_j := \{u \in V \mid \rnk(u) = j \wedge (\alpha = 0 \rightarrow \dec(u) \neq \false) \wedge (\alpha = 1 \rightarrow \dec(u) \neq \true) \} \setminus S_{1 - \alpha}$}}
            \State $\qquad U := U_j \cup S_\alpha$
            \State $\qquad X := \textsc{AttrMinRankGeneric}(U, \alpha, V, j)$
            \State $\qquad Y := V \setminus \textsc{AttrDefault}(V \setminus X, 1-\alpha)$
            \State {$\qquad \textbf{while}\ X \neq Y\ \textbf{do}$}
                \State $\qquad \qquad X := \textsc{AttrMinRankGeneric}(U \cap Y, \alpha, V, j)$
                \State $\qquad \qquad Y := Y \setminus \textsc{AttrDefault}(Y \setminus X, 1-\alpha)$
            \State $\qquad\ \mathbf{for}\ v \in X \setminus S_{\alpha}\ \mathbf{do}$
                \State \qquad \qquad {\colorbox{white}{$\mathbf{if}\ (\alpha = 0 \land \dec(u) = \blacktriangledown) \lor (\alpha = 1 \land \dec(u) = \blacktriangle)\ \mathbf{then}$}}
                    \State \qquad \qquad \qquad {\colorbox{white}{$\mathbf{if}\ v \in U_j\ \mathbf{then}\ \tau_{\alpha}(v) := w\ \mathbf{with}\ w \in \scc(v) \cap Y$}}
                    \State $\qquad \qquad \qquad \mathbf{else}\ \tau_{\alpha}(v) := \tau(v)$
            \State $\qquad S_\alpha := S_\alpha \cup X$
            \State $\qquad S_\alpha, \tau_\alpha := \textsc{AttrDefaultWithTau}(S_\alpha, \alpha, \tau_\alpha)$
        \State \Return $S_0, S_1, \tau_0,\tau_1$
    \EndFunction
    \end{algorithmic}

where :ref:`AttrMinRankGeneric <attr-min-rank-generic>` is a slightly modified version of the original attractor set computation :ref:`AttrDefault <attr>`, which computes the `\alpha`-min attractor into set `U` restricted to vertices of rank `j`:

.. _attr-min-rank-generic:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \Function {AttrMinRankGeneric}{$A, \alpha, U, j$}
        \State $\td := \bigcup_{u \in A} (pred^{= j}(u, U) \setminus A)$
        \State {$\textbf{while}\ \td \neq \emptyset\ \textbf{do}$}
        \State $\qquad \textbf{choose}\ u \in \td$
        \State $\qquad \td := \td \setminus \{u\}$
        \State $\qquad \mathbf{if}\ d_u = \alpha \vee succ(u) \subseteq A\ \mathbf{then}$
            \State $\qquad \qquad \mathbf{if}\ d_u = \alpha\ \mathbf{then}\ \tau \lbrack u \rbrack:=v\ \mathbf{with}\ v\in A\cap succ(u) $ 
            \State $\qquad \qquad A := A \cup \{u \}$
            \State $\qquad \qquad \td := \td \cup (pred^{= j}(u, U) \setminus A)$
        \State \Return $A$ 
    \EndFunction
    \end{algorithmic}

where

.. math::
    :nowrap:

    \begin{eqnarray*}
    pred^{= j}(u, U) = \{v \in U \mid (v,u) \in E \wedge (\rnk(v) = j \vee (v \notin \mathrm{dom}(\rnk) \wedge \dec(v) \in \{\blacktriangle,\blacktriangledown\}) )\}
    \end{eqnarray*}

The above optimisation can be integrated in the instantiation algorithm as follows:

.. _detect-loops:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \Function {PbesInstStructureGraph2}{$\E, X_{\init}(e_{\init}), R, E^0, \lsetpre$}
        \State $\init := X_{\init}(e_{\init})$
        \State $\td := \{\init\}$
        \State $\discovered := \{\init\}$
        \State $(V,E,d,r) := (\emptyset, \emptyset, -, -)$
        \State $S_0 := \emptyset$
        \State $S_1 := \emptyset$
        \State {$\mathbf{while}\ \td \neq \emptyset \wedge X_{\init}(e_{\init}) \notin S_0 \cup S_1 \ \mathbf{do}$}
            \State $\qquad \mathbf{choose}\ X_k(e) \in \td$ 
            \State $\qquad \td := \td \setminus \{X_k(e)\}$
            \State $\qquad \psi_{X_k}^e := R(\varphi_{X_k}[d_k := e])$
            \State \qquad {\colorbox{white}{$\psi_{X_k}^e := \mathbf{if}\ \sigma_k = \mu\ \mathbf{then}\ R(\psi_{X_k}^e[X_k(e) := \false])\ \mathbf{else}\ R(\psi_{X_k}^e[X_k(e) := \true])$}}
            \State $\qquad (b,\psi_{X_k}^e,g_0,g_1) := R^+(\psi_{X_k}^e)$
            \State $\qquad \mathbf{if}\ b = \true\ \mathbf{then}$
                \State $\qquad \qquad S_0 := S_0 \cup \{X_k(e)\}$
                \State $\qquad \qquad \psi_{X_k}^e := g_0$
            \State $\qquad \mathbf{if}\ b = \false\ \mathbf{then}$
                \State $\qquad \qquad S_1 := S_1 \cup \{X_k(e)\}$
                \State $\qquad \qquad \psi_{X_k}^e := g_1$
            \State $\qquad \psi_{X_k}^e := Rw^*(\psi_{X_k}^e, \{X \mid (X_k(e), X) \in E^0\}, \lsetpre)$
            \State $\qquad (V', E', d', r') := SG^0(X_k(e), \psi_{X_k}^e, \dec(\psi_{X_k}^e), \rnk(X_k(e)))$
            \State $\qquad (V,E, d, r) := (V \cup V',E \cup E',d'\lceil d,r' \lceil r)$
            \State $\qquad \td := \td \cup (\occ(\psi_{X_k}^e) \setminus \discovered)$
            \State $\qquad \discovered := \discovered \cup \occ(\psi_{X_k}^e)$
            \State \qquad {\colorbox{lightgray}{$S_0, S_1, \tau_0, \tau_1 := \text{\textsc{FindLoops2}}(V, S_0, S_1, \tau_0, \tau_1) $}} 
        \State $V := \textsc{ExtractMinimalStructureGraph}(V, \init, S_0, S_1, \tau_0, \tau_1)$
        \State \Return $(V,E, d, r)$
    \EndFunction
    \end{algorithmic}

.. _example-detect-loops:

.. admonition:: Example 4
    :class: admonition-example

    Let us consider the following mCRL2 specification

    ::

        act a;
        proc P(n:Nat) = (n < 10) -> a. P(n+1)
                    + (n == 5) -> a. P(Int2Nat(n-1))
                    + (n == 4)  -> a. P(n+2)
                    + (n == 8) -> a. P(5);

        init P(0);

    The labelled transition system underlying this specification is (for the sake of clarity we also add numbers to identify the states)
    
    .. math::
        :nowrap:
        :class: math-tikz

        \begin{tikzpicture}
                \tikzstyle{every state}=[
                    draw,
                    shape=circle,
                    inner sep=1pt,
                    minimum size=8pt,
                    node distance=50pt,
                    initial text={},
                    auto]
            \node [state, initial] (0) {0};
            \node [state, right= of 0] (1) {4};
            \node [state, right= of 1] (2) {6};
            \node [state, above= of 2] (3) {5};
            \node [state, right= of 2] (4) {7};
            \node [state, right= of 4] (5) {8};
            \node [state, right= of 5] (6) {9};
            \node [state, right= of 6] (7) {10};
            
            \draw[dotted, ->] 
            (0) edge[above] node {$a$} (1)
            ;
            \draw[->] 
            (1) edge[above] node {$a$} (2)
            (1) edge[above] node {$a$} (3)
            (3) edge[left] node {$a$} (2)
            (2) edge[above] node {$a$} (4)
            (3) edge[bend left=-45, above] node {$a$} (1)
            (4) edge[above] node {$a$} (5)
            (5) edge[above] node {$a$} (6)
            (5) edge[above] node {$a$} (3)
            (6) edge[above] node {$a$} (7)
            ;
        \end{tikzpicture}

    The property we verify is 

    ::
        
        nu X. <a>X.
    
    We again consider to instantiate the PBES without evidence information.

    If :ref:`0- No on-the-fly solving is applied <instantiation-no-on-the-fly>` or :ref:`1- Propagate solved equations using an attractor <propagate-solved-eq-attr>` then 11 equations are generated and are the following

    ::

        Y(0) = Y(1) with rank 0
        Y(1) = Y(2) with rank 0
        Y(2) = Y(3) with rank 0
        Y(3) = Y(4) with rank 0
        Y(4) = Y(5) || Y(6) with rank 0
        Y(6) = Y(7) with rank 0
        Y(5) = Y(4) || Y(6) with rank 0
        Y(7) = Y(8) with rank 0
        Y(8) = Y(5) ||Y(9) with rank 0
        Y(9) = Y(10) with rank 0
        Y(10) = false with rank 0
        

    and the PBES is instantiated to the following structure graph

    .. math::
        :nowrap:
        :class: math-tikz

        \begin{tikzpicture}[scale=0.5,
            every node/.style={scale=0.8},
            node distance = 2.5cm]

            \node (0) {${Y(0)}_{-\textcircled{0}}$};
            \node [right of= 0,xshift=1.5cm] (1) {${Y(4)}_{\blacktriangledown\textcircled{0}}$};
            \node [right of= 1,xshift=1.5cm] (2) {${Y(6)}_{-\textcircled{0}}$};
            \node [above of= 2] (3) {${Y(5)}_{\blacktriangledown\textcircled{0}}$};
            \node [right of= 2,xshift=1.5cm] (4) {${Y(7)}_{-\textcircled{0}}$};
            \node [right of= 4,xshift=1.5cm] (5) {${Y(8)}_{\blacktriangledown\textcircled{0}}$};
            \node [right of= 5,xshift=1.5cm] (6) {${Y(9)}_{-\textcircled{0}}$};
            \node [right of= 6,xshift=1.5cm] (7) {${Y(10)}_{\bot\textcircled{0}}$};
            
            
            \path[dotted, ->] 
            (0) edge node {} (1)
            ;
            \path[->] 
            (1) edge node {} (2)
            (1) edge node {} (3)
            (2) edge node {} (4)
            (3) edge node {} (2)
            (3) edge[bend left=-45] node {} (1)
            (4) edge node {} (5)
            (5) edge node {} (6)
            (5) edge node {} (3)
            (6) edge node {} (7)
            ;

        \end{tikzpicture}

    If :ref:`2- Detect winning loops <detect-loops>` is applied then 7 equations are generated and are the following

    ::

        Y(0) = Y(1) with rank 0
        Y(1) = Y(2) with rank 0
        Y(2) = Y(3) with rank 0
        Y(3) = Y(4) with rank 0
        Y(4) = Y(5) || Y(6) with rank 0
        Y(6) = Y(7) with rank 0
        Y(5) = Y(4) || Y(6) with rank 0
        

    and the PBES is instantiated to the following structure graph

    .. math::
        :nowrap:
        :class: math-tikz

        \begin{tikzpicture}[scale=0.5,
            every node/.style={scale=0.8},
            node distance = 2.5cm]

            \node (0) {${Y(0)}_{-\textcircled{0}}$};
            \node [right of= 0,xshift=1.5cm] (1) {${Y(4)}_{\blacktriangledown\textcircled{0}}$};
            \node [right of= 1,xshift=1.5cm] (2) {${Y(6)}_{-\textcircled{0}}$};
            \node [above of= 2] (3) {${Y(5)}_{\blacktriangledown\textcircled{0}}$};
            \node [right of= 2,xshift=1.5cm] (4) {${Y(7)}_{-\textcircled{-}}$};
            
            \path[dotted, ->] 
            (0) edge node {} (1)
            ;
            \path[->] 
            (1) edge node {} (3)
            (3) edge[bend left=-45] node {} (1)
            (1) edge node {} (2)
            (2) edge node {} (4)
            ;

        \end{tikzpicture}

    The instantiation procedure does not explore further vertex `Y(7)` and, in turn, sets strategies `\tau_{0}[Y(4)] = Y(5)` and `\tau_{0}[Y(5)] = Y(4)`. Consequently, the structure graph is simplified to the following minimal structure graph

    .. math::
        :nowrap:
        :class: math-tikz

        \begin{tikzpicture}[scale=0.5,
            every node/.style={scale=0.8},
            node distance = 2.5cm]

            \node (0) {${Y(0)}_{-\textcircled{0}}$};
            \node [right of= 0,xshift=1.5cm] (1) {${Y(4)}_{\blacktriangledown\textcircled{0}}$};
            \node [right of= 1,xshift=1.5cm] (2) {};
            \node [above of= 2] (3) {${Y(5)}_{\blacktriangledown\textcircled{0}}$};
            
            \path[dotted, ->] 
            (0) edge node {} (1)
            ;
            \path[->] 
            (1) edge node {} (3)
            (3) edge[bend left=-45] node {} (1)
            ;

        \end{tikzpicture}

3- Solve subgames using a fatal attractor
"""""""""""""""""""""""""""""""""""""""""

In Algorithm :ref:`PbesInstStructureGraph3 <solve-fatal-attractor>` we present the algorithm for optimization *3- Solve subgames using a fatal attractor*.
Algorithm :ref:`PbesInstStructureGraph3 <solve-fatal-attractor>` uses :ref:`FatalAttractorOriginal <fatal-attractor-original>`, which is very close to the original fatal attractors computation [HKP13]_. 

.. _fatal-attractor-original:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \Function {FatalAttractorOriginal}{$V, S_0, S_1, \tau_0, \tau_1$}
    \State $J := \{ j \mid \exists u \in V : \rnk(u) = j \}$
    \State $S_0, \tau_0 := \textsc{AttrDefaultWithTau}(S_0, 0, \tau_0)$
    \State $S_1, \tau_1 := \textsc{AttrDefaultWithTau}(S_1, 1, \tau_1)$
    \State $\mathbf{for}\ j \in J$
            \State $\qquad \alpha := j \bmod 2$
            \State \qquad {\colorbox{white}{$U_j := \{u \in V \mid \rnk(u) = j \wedge (\alpha = 0 \rightarrow \dec(u) \neq \false) \wedge (\alpha = 1 \rightarrow \dec(u) \neq \true) \} \setminus S_{1 - \alpha}$}}
            \State $\qquad X := \emptyset$
            \State {$\qquad \textbf{while}\ U_j \neq \emptyset \wedge U_j \neq X \ \textbf{do}$}
                \State $\qquad \qquad X := U_j$
                \State $\qquad \qquad Y := \textsc{AttrMinRankOriginal}(X \cup S_{\alpha}, \alpha, V, j)$
                \State $\qquad \qquad \mathbf{if}\ U_j \subseteq Y\ \mathbf{then}$
                    \State $\qquad \qquad \qquad \ \mathbf{for}\ v \in Y \setminus S_{\alpha}\ \mathbf{do}$
                        \State \qquad \qquad \qquad \qquad {\colorbox{white}{$\mathbf{if}\ (\alpha = 0 \land \dec(u) = \blacktriangledown) \lor (\alpha = 1 \land \dec(u) = \blacktriangle)\ \mathbf{then}$}}
                        \State \qquad \qquad \qquad \qquad \qquad {\colorbox{white}{$\mathbf{if}\ v \in U_j\ \mathbf{then}\ \tau_{\alpha}(v) := w\ \mathbf{with}\ w \in \scc(v) \cap Y$}}
                        \State $\qquad \qquad \qquad \qquad \qquad \mathbf{else}\ \tau_{\alpha}(v) := \tau(v)$
                    \State $\qquad \qquad \qquad  S_\alpha := S_\alpha \cup Y$
                    \State $\qquad \qquad \qquad  S_\alpha, \tau_\alpha := \textsc{AttrDefaultWithTau}(S_\alpha, \alpha, \tau_\alpha)$
                    \State $\qquad \qquad \qquad \mathbf{break}$
                \State $\qquad \qquad \mathbf{else}$
                    \State $\qquad \qquad \qquad U_j := U_j \cap Y$
        \State \Return $S_0, S_1, \tau_0,\tau_1$
    \EndFunction
    \end{algorithmic}

where :ref:`AttrMinRankOriginal <attr-min-rank-original>` is a slightly modified version of the original attractor set computation :ref:`AttrMinRankGeneric <attr-min-rank-generic>`, which computes the  `\alpha`-min attractor into set `U` restricted to vertices of rank at least `j`:

.. _attr-min-rank-original:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \Function {AttrMinRankOriginal}{$A, \alpha, U, j$}
        \State $\td := \bigcup_{u \in A} (pred^{\geq j}(u, U))$
        \State $X := \{ u \in \td \cap A \mid d_u = \alpha \vee \scc(u) \subseteq A\}$
        \State {$\textbf{while}\ \td \neq \emptyset\ \textbf{do}$}
        \State $\qquad \textbf{choose}\ u \in \td$
        \State $\qquad \td := \td \setminus \{u\}$
        \State $\qquad \mathbf{if}\ d_u = \alpha \vee succ(u) \subseteq A\ \mathbf{then}$
            \State \qquad \qquad {\colorbox{white}{$\mathbf{if}\ d_u = \alpha\ \mathbf{then}\ \tau \lbrack u \rbrack:=v\ \mathbf{with}\ v\in (A \cup X) \cap succ(u)$}} 
            \State $\qquad \qquad X := X \cup \{u \}$
            \State $\qquad \qquad \td := \td \cup (pred^{\geq j}(u, U) \setminus X)$
        \State \Return $X$ 
    \EndFunction
    \end{algorithmic}

where

.. math::
    :nowrap:

    \begin{eqnarray*}
    pred^{\geq j}(u, U) = \{v \in U \mid (v,u) \in E \wedge (\rnk(v) \geq j \vee (v \notin \mathrm{dom}(\rnk) \wedge \dec(v) \in \{\blacktriangle,\blacktriangledown\}) )\}
    \end{eqnarray*}

The above optimisation can be integrated in the instantiation algorithm as follows:

.. _solve-fatal-attractor:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \Function {PbesInstStructureGraph3}{$\E, X_{\init}(e_{\init}), R, E^0, \lsetpre$}
        \State $\init := X_{\init}(e_{\init})$
        \State $\td := \{\init\}$
        \State $\discovered := \{\init\}$
        \State $(V,E,d,r) := (\emptyset, \emptyset, -, -)$
        \State $S_0 := \emptyset$
        \State $S_1 := \emptyset$
        \State {$\mathbf{while}\ \td \neq \emptyset \wedge X_{\init}(e_{\init}) \notin S_0 \cup S_1 \ \mathbf{do}$}
            \State $\qquad \mathbf{choose}\ X_k(e) \in \td$ 
            \State $\qquad \td := \td \setminus \{X_k(e)\}$
            \State $\qquad \psi_{X_k}^e := R(\varphi_{X_k}[d_k := e])$
            \State \qquad {\colorbox{white}{$\psi_{X_k}^e := \mathbf{if}\ \sigma_k = \mu\ \mathbf{then}\ R(\psi_{X_k}^e[X_k(e) := \false])\ \mathbf{else}\ R(\psi_{X_k}^e[X_k(e) := \true])$}}
            \State $\qquad (b,\psi_{X_k}^e,g_0,g_1) := R^+(\psi_{X_k}^e)$
            \State $\qquad \mathbf{if}\ b = \true\ \mathbf{then}$
                \State $\qquad \qquad S_0 := S_0 \cup \{X_k(e)\}$
                \State $\qquad \qquad \psi_{X_k}^e := g_0$
            \State $\qquad \mathbf{if}\ b = \false\ \mathbf{then}$
                \State $\qquad \qquad S_1 := S_1 \cup \{X_k(e)\}$
                \State $\qquad \qquad \psi_{X_k}^e := g_1$
            \State $\qquad \psi_{X_k}^e := Rw^*(\psi_{X_k}^e, \{X \mid (X_k(e), X) \in E^0\}, \lsetpre)$
            \State $\qquad (V', E', d', r') := SG^0(X_k(e), \psi_{X_k}^e, \dec(\psi_{X_k}^e), \rnk(X_k(e)))$
            \State $\qquad (V,E, d, r) := (V \cup V',E \cup E',d'\lceil d,r' \lceil r)$
            \State $\qquad \td := \td \cup (\occ(\psi_{X_k}^e) \setminus \discovered)$
            \State $\qquad \discovered := \discovered \cup \occ(\psi_{X_k}^e)$
            \State \qquad {\colorbox{lightgray}{$S_0, S_1, \tau_0, \tau_1 := \text{\textsc{FatalAttractorOriginal}}(V, S_0, S_1, \tau_0, \tau_1) $}} 
        \State $V := \textsc{ExtractMinimalStructureGraph}(V, \init, S_0, S_1, \tau_0, \tau_1)$
        \State \Return $(V,E, d, r)$
    \EndFunction
    \end{algorithmic}

.. _example-solved-fatal:

.. admonition:: Example 5
    :class: admonition-example

    Let us consider the following mCRL2 specification

    ::

        act a,b;
        proc P(n:Nat) = (n <= 4) -> a. P(n+1)
                    + (n < 8 && n > 4) -> b . P(n+1)
                    + (n == 5) -> b. P(Int2Nat(n-1) )
                    + (n == 6) -> b. P(5);

        init P(0);

    The labelled transition system underlying this specification is (for the sake of clarity we also add numbers to identify the states)
    
    .. math::
        :nowrap:
        :class: math-tikz

        \begin{tikzpicture}
                \tikzstyle{every state}=[
                    draw,
                    shape=circle,
                    inner sep=1pt,
                    minimum size=8pt,
                    node distance=50pt,
                    initial text={},
                    auto]
            \node [state, initial] (0) {0};
            \node [state, right= of 0] (1) {4};
            \node [state, right= of 1] (2) {5};
            \node [state, right= of 2] (3) {6};
            \node [state, right= of 3] (4) {7};
            \node [state, right= of 4] (5) {8};
            
            \draw[dotted, ->] 
            (0) edge[above] node {$a$} (1)
            ;
            \draw[->] 
            (1) edge[above] node {$a$} (2)
            (2) edge[above] node {$b$} (3)
            (3) edge[bend left=-45, above] node {$b$} (2)
            (3) edge[above] node {$b$} (4)
            (2) edge[bend left=-45, above] node {$b$} (1)
            (4) edge[above] node {$b$} (5)
            ;
        \end{tikzpicture}

    The property we verify is 

    ::
        
        mu Y.(nu X .(mu W.(<a>Y || <b>X || <a>W)))
    
    We again consider to instantiate the PBES without evidence information. The PBES is as follows:

    :: 

        sort Enum3 = struct e2_3 | e1_3 | e0_3;

        map C3_: Enum3 # Nat # Nat # Nat -> Nat;
            C3_1: Enum3 # Bool # Bool # Bool -> Bool;

        var x2,y3,y2,y1: Nat;
            e1,e2: Enum3;
            x3,y6,y5,y4: Bool;
        eqn C3_(e1, x2, x2, x2)  =  x2;
            C3_(e2_3, y3, y2, y1)  =  y3;
            C3_(e1_3, y3, y2, y1)  =  y2;
            C3_(e0_3, y3, y2, y1)  =  y1;
            C3_1(e2, x3, x3, x3)  =  x3;
            C3_1(e2_3, y6, y5, y4)  =  y6;
            C3_1(e1_3, y6, y5, y4)  =  y5;
            C3_1(e0_3, y6, y5, y4)  =  y4;

        pbes mu Z(n_P: Nat) =
            X0(n_P);
            nu X0(n_P: Nat) =
            Y0(n_P);
            mu Y0(n_P: Nat) =
            val(n_P <= 4) && Z(n_P + 1) || (exists e_P: Enum3. val(false)) || (exists e_P: Enum3. val(C3_1(e_P, n_P == 6, n_P == 5, n_P < 8 && 4 < n_P)) && X0(C3_(e_P, 5, Int2Nat(n_P - 1), n_P + 1))) || val(n_P <= 4) && Y0(n_P + 1) || (exists e_P: Enum3. val(false));

        init Z(0);


    If :ref:`0- No on-the-fly solving is applied <instantiation-no-on-the-fly>` or :ref:`1- Propagate solved equations using an attractor <propagate-solved-eq-attr>` or :ref:`2- Detect winning loops <detect-loops>` then 24 equations are generated and are the following

    ::

        Z(0) = X0(0) with rank 1
        X0(0) = Y0(0) with rank 2
        Y0(0) = Z(1) || Y0(1) with rank 3
        Z(1) = X0(1) with rank 1
        Y0(1) = Z(2) || Y0(2) with rank 3
        X0(1) = Y0(1) with rank 2
        Z(2) = X0(2) with rank 1
        Y0(2) = Z(3) || Y0(3) with rank 3
        X0(2) = Y0(2) with rank 2
        Z(3) = X0(3) with rank 1
        Y0(3) = Z(4) || Y0(4) with rank 3
        X0(3) = Y0(3) with rank 2
        Z(4) = X0(4) with rank 1
        Y0(4) = Z(5) || Y0(5) with rank 3
        X0(4) = Y0(4) with rank 2
        Z(5) = X0(5) with rank 1
        Y0(5) = X0(4) || X0(6) with rank 3
        X0(5) = Y0(5) with rank 2
        X0(6) = Y0(6) with rank 2
        Y0(6) = X0(5) || X0(7) with rank 3
        X0(7) = Y0(7) with rank 2
        Y0(7) = X0(8) with rank 3
        X0(8) = Y0(8) with rank 2
        Y0(8) = false with rank 3
        

    and the PBES is instantiated to the following structure graph

    .. math::
        :nowrap:
        :class: math-tikz

        \begin{tikzpicture}[scale=0.5,
            every node/.style={scale=0.8},
            node distance = 2.5cm]

            \node (0) {${Z(0)}_{-\textcircled{1}}$};
            \node [below of= 0] (1) {${X0(0)}_{-\textcircled{2}}$};
            \node [below of= 1] (2) {${Y0(0)}_{\blacktriangledown\textcircled{3}}$};
            \node [below of= 2] (3) {${Z(1)}_{-\textcircled{1}}$};
            \node [below of= 3] (4) {${X0(1)}_{-\textcircled{2}}$};
            \node [right of= 2,xshift=1.5cm] (5) {${Y0(1)}_{\blacktriangledown\textcircled{3}}$};
            \node [below of= 5] (6) {${Z(2)}_{-\textcircled{1}}$};
            \node [below of= 6] (7) {${X0(2)}_{-\textcircled{2}}$};
            \node [right of= 5,xshift=1.5cm] (8) {${Y0(2)}_{\blacktriangledown\textcircled{3}}$};
            \node [below of= 8] (9) {${Z(3)}_{-\textcircled{1}}$};
            \node [below of= 9] (10) {${X0(3)}_{-\textcircled{2}}$};
            \node [right of= 8,xshift=1.5cm] (11) {${Y0(3)}_{\blacktriangledown\textcircled{3}}$};
            \node [below of= 11] (12) {${Z(4)}_{-\textcircled{1}}$};
            \node [below of= 12] (13) {${X0(4)}_{-\textcircled{2}}$};
            \node [right of= 11,xshift=1.5cm] (14) {${Y0(4)}_{\blacktriangledown\textcircled{3}}$};
            \node [below of= 14] (15) {${Z(5)}_{-\textcircled{1}}$};
            \node [below of= 15] (16) {${X0(5)}_{-\textcircled{2}}$};
            \node [below of= 16] (17) {${Y0(5)}_{\blacktriangledown\textcircled{3}}$};
            \node [below of= 17] (18) {${X0(6)}_{-\textcircled{2}}$};
            \node [right of= 17,xshift=1.5cm] (19) {${Y0(6)}_{-\textcircled{3}}$};
            \node [below of= 19] (20) {${X0(7)}_{-\textcircled{2}}$};
            \node [right of= 19,xshift=1.5cm] (21) {${Y0(7)}_{-\textcircled{3}}$};
            \node [below of= 21] (22) {${X0(8)}_{-\textcircled{2}}$};
            \node [right of= 21,xshift=1.5cm] (23) {${Y0(8)}_{-\textcircled{3}}$};
            
            \path[->] 
            (0) edge node {} (1)
            (1) edge node {} (2)
            (2) edge node {} (3)
            (3) edge node {} (4)
            (2) edge node {} (5)
            (4) edge node {} (5)
            (5) edge node {} (6)
            (6) edge node {} (7)
            (5) edge node {} (8)
            (7) edge node {} (8)
            (8) edge node {} (9)
            (9) edge node {} (10)
            (8) edge node {} (11)
            (10) edge node {} (11)
            (11) edge node {} (12)
            (12) edge node {} (13)
            (11) edge node {} (14)
            (13) edge node {} (14)
            (14) edge node {} (15)
            (15) edge node {} (16)
            (14) edge[bend right=-45] node {} (17)
            (16) edge node {} (17)
            (17) edge node {} (13)
            (17) edge node {} (18)
            (18) edge node {} (19)
            (19) edge node {} (20)
            (20) edge node {} (21)
            (21) edge node {} (22)
            (22) edge node {} (23)
            ;

        \end{tikzpicture}


    If :ref:`3- Solve subgames using a fatal attractor <solve-fatal-attractor>` is applied then 17 equations are generated and are the following

    ::

        Z(0) = X0(0) with rank 1
        X0(0) = Y0(0) with rank 2
        Y0(0) = Z(1) || Y0(1) with rank 3
        Z(1) = X0(1) with rank 1
        Y0(1) = Z(2) || Y0(2) with rank 3
        X0(1) = Y0(1) with rank 2
        Z(2) = X0(2) with rank 1
        Y0(2) = Z(3) || Y0(3) with rank 3
        X0(2) = Y0(2) with rank 2
        Z(3) = X0(3) with rank 1
        Y0(3) = Z(4) || Y0(4) with rank 3
        X0(3) = Y0(3) with rank 2
        Z(4) = X0(4) with rank 1
        Y0(4) = Z(5) || Y0(5) with rank 3
        X0(4) = Y0(4) with rank 2
        Z(5) = X0(5) with rank 1
        Y0(5) = X0(4) || X0(6) with rank 3
        

    and the PBES is instantiated to the following structure graph

    .. math::
        :nowrap:
        :class: math-tikz

        \begin{tikzpicture}[scale=0.5,
            every node/.style={scale=0.8},
            node distance = 2.5cm]

            \node (0) {${Z(0)}_{-\textcircled{1}}$};
            \node [below of= 0] (1) {${X0(0)}_{-\textcircled{2}}$};
            \node [below of= 1] (2) {${Y0(0)}_{\blacktriangledown\textcircled{3}}$};
            \node [below of= 2] (3) {${Z(1)}_{-\textcircled{1}}$};
            \node [below of= 3] (4) {${X0(1)}_{-\textcircled{2}}$};
            \node [right of= 2,xshift=1.5cm] (5) {${Y0(1)}_{\blacktriangledown\textcircled{3}}$};
            \node [below of= 5] (6) {${Z(2)}_{-\textcircled{1}}$};
            \node [below of= 6] (7) {${X0(2)}_{-\textcircled{2}}$};
            \node [right of= 5,xshift=1.5cm] (8) {${Y0(2)}_{\blacktriangledown\textcircled{3}}$};
            \node [below of= 8] (9) {${Z(3)}_{-\textcircled{1}}$};
            \node [below of= 9] (10) {${X0(3)}_{-\textcircled{2}}$};
            \node [right of= 8,xshift=1.5cm] (11) {${Y0(3)}_{\blacktriangledown\textcircled{3}}$};
            \node [below of= 11] (12) {${Z(4)}_{-\textcircled{1}}$};
            \node [below of= 12] (13) {${X0(4)}_{-\textcircled{2}}$};
            \node [right of= 11,xshift=1.5cm] (14) {${Y0(4)}_{\blacktriangledown\textcircled{3}}$};
            \node [below of= 14] (15) {${Z(5)}_{-\textcircled{1}}$};
            \node [below of= 15] (16) {${X0(5)}_{-\textcircled{-}}$};
            \node [below of= 16] (17) {${Y0(5)}_{\blacktriangledown\textcircled{3}}$};
            \node [below of= 17] (18) {${X0(6)}_{-\textcircled{-}}$};
            
            \path[->] 
            (0) edge node {} (1)
            (1) edge node {} (2)
            (2) edge node {} (3)
            (3) edge node {} (4)
            (2) edge node {} (5)
            (4) edge node {} (5)
            (5) edge node {} (6)
            (6) edge node {} (7)
            (5) edge node {} (8)
            (7) edge node {} (8)
            (8) edge node {} (9)
            (9) edge node {} (10)
            (8) edge node {} (11)
            (10) edge node {} (11)
            (11) edge node {} (12)
            (12) edge node {} (13)
            (11) edge node {} (14)
            (13) edge node {} (14)
            (14) edge node {} (15)
            (15) edge node {} (16)
            (14) edge[bend right=-45] node {} (17)
            (17) edge node {} (13)
            (17) edge node {} (18)
            ;

        \end{tikzpicture}
    
    The instantiation procedure does not explore further vertices `X(5)` and `X(6)` and, in turn, sets strategies `\tau[Y0(0)] = Y0(1)`, `\tau[Y0(1)] = Y0(2)`, `\tau[Y0(2)] = Y0(3)`, `\tau[Y0(3)] = Y0(4)`, `\tau[Y0(4)] = Y0(5)`, `\tau[Y0(15)] = X0(4)` and `\tau[X0(4)] = Y0(4)`. Consequently, the structure graph is simplified to the following minimal structure graph

    .. math::
        :nowrap:
        :class: math-tikz

        \begin{tikzpicture}[scale=0.5,
            every node/.style={scale=0.8},
            node distance = 2.5cm]

            \node (0) {${Z(0)}_{-\textcircled{1}}$};
            \node [below of= 0] (1) {${X0(0)}_{-\textcircled{2}}$};
            \node [below of= 1] (2) {${Y0(0)}_{\blacktriangledown\textcircled{3}}$};
            \node [right of= 2,xshift=1.5cm] (5) {${Y0(1)}_{\blacktriangledown\textcircled{3}}$};
            \node [right of= 5,xshift=1.5cm] (8) {${Y0(2)}_{\blacktriangledown\textcircled{3}}$};
            \node [right of= 8,xshift=1.5cm] (11) {${Y0(3)}_{\blacktriangledown\textcircled{3}}$};
            \node [right of= 11,xshift=1.5cm] (14) {${Y0(4)}_{\blacktriangledown\textcircled{3}}$};
            \node [right of= 14,xshift=1.5cm] (17) {${Y0(5)}_{\blacktriangledown\textcircled{3}}$};
            \node [above of= 17] (13) {${X0(4)}_{-\textcircled{2}}$};
            
            \path[->] 
            (0) edge node {} (1)
            (1) edge node {} (2)
            (2) edge node {} (5)
            (5) edge node {} (8)
            (8) edge node {} (11)
            (11) edge node {} (14)
            (14) edge node {} (17)
            (17) edge node {} (13)
            (13) edge node {} (14)
            ;

        \end{tikzpicture}

4- Solve subgames using the solver
""""""""""""""""""""""""""""""""""

In Algorithm :ref:`PbesInstStructureGraph4 <solve-using-solver>` we present the algorithm for optimization *4- Solve subgames using the solver*.
Algorithm :ref:`PbesInstStructureGraph4 <solve-using-solver>` uses :ref:`PartialSolve <partial-solve>`, where `S_0` and `S_1` are extended by solving a partial game. 

.. _partial-solve:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \Function {PartialSolve}{$V, \td, S_0, S_1, \tau_0, \tau_1$}
        \State $S_0, \tau_0 := \textsc{AttrDefaultWithTau}(S_0, 0, \tau_0)$
        \State $S_1, \tau_1 := \textsc{AttrDefaultWithTau}(S_1, 1, \tau_1)$
        \State{\colorbox{white}{$W_0, W_1 := \textsc{SolveRecursive}(V \setminus (S_1 \cup \textsc{AttrDefaultNoStrategy}(S_0 \cup \td, 0)))$}}
        \State $\mathbf{for}\ v \in W_1 \setminus S_1\ \mathbf{do}$
            \State \qquad $\mathbf{if}\ \dec(u) = \blacktriangle\ \mathbf{then}\ \tau_1(v) := \tau(v)$
        \State $S_1 := S_1 \cup W_1$   
        \State{\colorbox{white}{$W_0, W_1 := \textsc{SolveRecursive}(V \setminus (S_0 \cup \textsc{AttrDefaultNoStrategy}(S_1 \cup \td, 1)))$}}
        \State $\mathbf{for}\ v \in W_0 \setminus S_0\ \mathbf{do}$
            \State \qquad $\mathbf{if}\ \dec(u) = \blacktriangle\ \mathbf{then}\ \tau_0(v) := \tau(v)$
        \State $S_0 := S_0 \cup W_0$              
        \State \Return $S_0, S_1, \tau_0,\tau_1$
    \EndFunction
    \end{algorithmic}

The above optimisation can be integrated in the instantiation algorithm as follows:

.. _solve-using-solver:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \Function {PbesInstStructureGraph4}{$\E, X_{\init}(e_{\init}), R, E^0, \lsetpre$}
        \State $\init := X_{\init}(e_{\init})$
        \State $\td := \{\init\}$
        \State $\discovered := \{\init\}$
        \State $(V,E,d,r) := (\emptyset, \emptyset, -, -)$
        \State $S_0 := \emptyset$
        \State $S_1 := \emptyset$
        \State {$\mathbf{while}\ \td \neq \emptyset \wedge X_{\init}(e_{\init}) \notin S_0 \cup S_1 \ \mathbf{do}$}
            \State $\qquad \mathbf{choose}\ X_k(e) \in \td$ 
            \State $\qquad \td := \td \setminus \{X_k(e)\}$
            \State $\qquad \psi_{X_k}^e := R(\varphi_{X_k}[d_k := e])$
            \State \qquad {\colorbox{white}{$\psi_{X_k}^e := \mathbf{if}\ \sigma_k = \mu\ \mathbf{then}\ R(\psi_{X_k}^e[X_k(e) := \false])\ \mathbf{else}\ R(\psi_{X_k}^e[X_k(e) := \true])$}}
            \State $\qquad (b,\psi_{X_k}^e,g_0,g_1) := R^+(\psi_{X_k}^e)$
            \State $\qquad \mathbf{if}\ b = \true\ \mathbf{then}$
                \State $\qquad \qquad S_0 := S_0 \cup \{X_k(e)\}$
                \State $\qquad \qquad \psi_{X_k}^e := g_0$
            \State $\qquad \mathbf{if}\ b = \false\ \mathbf{then}$
                \State $\qquad \qquad S_1 := S_1 \cup \{X_k(e)\}$
                \State $\qquad \qquad \psi_{X_k}^e := g_1$
            \State $\qquad \psi_{X_k}^e := Rw^*(\psi_{X_k}^e, \{X \mid (X_k(e), X) \in E^0\}, \lsetpre)$
            \State $\qquad (V', E', d', r') := SG^0(X_k(e), \psi_{X_k}^e, \dec(\psi_{X_k}^e), \rnk(X_k(e)))$
            \State $\qquad (V,E, d, r) := (V \cup V',E \cup E',d'\lceil d,r' \lceil r)$
            \State $\qquad \td := \td \cup (\occ(\psi_{X_k}^e) \setminus \discovered)$
            \State $\qquad \discovered := \discovered \cup \occ(\psi_{X_k}^e)$
            \State \qquad {\colorbox{lightgray}{$S_0, S_1, \tau_0, \tau_1 := \text{\textsc{PartialSolve}}(V, \td, S_0, S_1, \tau_0, \tau_1) $}} 
        \State $V := \textsc{ExtractMinimalStructureGraph}(V, \init, S_0, S_1, \tau_0, \tau_1)$
        \State \Return $(V,E, d, r)$
    \EndFunction
    \end{algorithmic}


.. admonition:: 4- Solve subgames using the solver
    :class: warning

    Optimization :ref:`4- Solve subgames using the solver <solve-using-solver>` does not perform as expected. 
    Currently, for :ref:`Example 2 <example-solved>` it is comparable to optimization :ref:`1- Propagate solved equations using an attractor <propagate-solved-eq-attr>`, for :ref:`Example 4 <example-detect-loops>` it is comparable to optimizations :ref:`0- No on-the-fly solving is applied <instantiation-no-on-the-fly>` and :ref:`1- Propagate solved equations using an attractor <propagate-solved-eq-attr>`, and for :ref:`Example 5 <example-solved-fatal>` it is comparable to optimizations :ref:`0- No on-the-fly solving is applied <instantiation-no-on-the-fly>`, :ref:`1- Propagate solved equations using an attractor <propagate-solved-eq-attr>` and :ref:`2- Detect winning loops <detect-loops>`.

Additional optimizations to PBES instantiation
----------------------------------------------

During the execution of the instantiation algorithm (with any solving strategy), the set *todo*  may contain nodes that can be proven to be irrelevant, i.e., the solution of the PBES can already be computed without exploring these irrelevant nodes. 
To this end we present a routine, :ref:`PruneTodo <prune-todo>`, that partitions the set *todo* into a new set *todo* and a set *irrelevant*. Note that elements from *irrelevant* may be moved to the new set *todo* when new elements are added to the *todo* set.

In the tool ``pbessolve``, flag ``--prune-todo-list`` can be used to enable this routine.
The routine can be used in combination with any solving strategy.

In Algorithm :ref:`PbesInstStructureGraphPrune <solve-prune>` we present the instantiation algorithm in which pruning is applied.
In fact, algorithm :ref:`PbesInstStructureGraphPrune <solve-prune>` uses :ref:`PruneTodo <prune-todo>`. 

.. _prune-todo:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \Function {PruneTodo}{$\init, \td, \irr$}
        \State $\td' := \{\init\}$
        \State $\done' := \emptyset$
        \State $\newtd := \emptyset$
        \State $\mathbf{while}\ \td' \neq \emptyset\ \mathbf{do}$
            \State \qquad $\mathbf{choose}\ u \in \td'$
            \State \qquad $\td' := \td' \setminus \{u\}$
            \State \qquad $\done' := \done' \cup \{u\}$
            \State \qquad $\mathbf{if}\ \dec(u) = - \wedge \scc(u) = \emptyset \ \mathbf{then}$
                \State \qquad \qquad $\newtd := \newtd \cup \{ u\}$
             \State \qquad $\mathbf{else}\ \mathbf{if}\ u \notin S_0 \cup S_1 \ \mathbf{then}$
                \State \qquad \qquad $\td' := \td' \cup (\scc(u) \setminus \done')$
        \State $\newtd := \newtd \cap (\td \cup \irr)$  
        \State $\newirr := (\td \cup \newirr) \setminus \newtd$             
        \State \Return $\newtd, \newirr$
    \EndFunction
    \end{algorithmic}

The above routine can be integrated in the instantiation algorithm as follows. Note, the following is the instantiation algorithm for :ref:`0- No on-the-fly solving is applied <instantiation-no-on-the-fly>` and where pruning is applied.

.. _solve-prune:


.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \State $\mathbf{function}${\colorbox{white}{$\text{\textsc{PbesInstStructureGraphPrune}}(\E, X_{\init}(e_{\init}), R, E^0, \lsetpre)$}}
        \State $\qquad \init := X_{\init}(e_{\init})$
        \State $\qquad \td := \{\init\}$
        \State \qquad {\colorbox{lightgray}{$\irr := \emptyset$}} 
        \State $\qquad \discovered := \{\init\}$
        \State $\qquad (V,E,d,r) := (\emptyset, \emptyset, -, -)$
        \State $\qquad S_0 := \emptyset$
        \State $\qquad S_1 := \emptyset$
        \State $\qquad \mathbf{while}\ ${\colorbox{lightgray}{$(\td \setminus \irr) $}} $\neq \emptyset \wedge X_{\init}(e_{\init}) \notin S_0 \cup S_1\ \mathbf{do}$
            \State $\qquad \qquad \mathbf{choose}\ X_k(e) \in \td$ 
            \State $\qquad \qquad \td := \td \setminus \{X_k(e)\}$
            \State $\qquad \qquad \psi_{X_k}^e := R(\varphi_{X_k}[d_k := e])$
            \State \qquad \qquad {\colorbox{white}{$\psi_{X_k}^e := \mathbf{if}\ \sigma_k = \mu\ \mathbf{then}\ R(\psi_{X_k}^e[X_k(e) := \false])\ \mathbf{else}\ R(\psi_{X_k}^e[X_k(e) := \true])$}}
            \State $\qquad \qquad (b,\psi_{X_k}^e,g_0,g_1) := R^+(\psi_{X_k}^e)$
            \State $\qquad \qquad \mathbf{if}\ b = \true\ \mathbf{then}$
                \State $\qquad \qquad \qquad S_0 := S_0 \cup \{X_k(e)\}$
                \State $\qquad \qquad \qquad \psi_{X_k}^e := g_0$
            \State $\qquad \qquad \mathbf{if}\ b = \false\ \mathbf{then}$
                \State $\qquad \qquad \qquad S_1 := S_1 \cup \{X_k(e)\}$
                \State $\qquad \qquad \qquad \psi_{X_k}^e := g_1$
            \State $\qquad \qquad \psi_{X_k}^e := Rw^*(\psi_{X_k}^e, \{X \mid (X_k(e), X) \in E^0\}, \lsetpre)$
            \State $\qquad \qquad (V', E', d', r') := SG^0(X_k(e), \psi_{X_k}^e, \dec(\psi_{X_k}^e), \rnk(X_k(e)))$
            \State $\qquad \qquad (V,E, d, r) := (V \cup V',E \cup E',d'\lceil d,r' \lceil r)$
            \State $\qquad \qquad \td := \td \cup (\occ(\psi_{X_k}^e) \setminus$ {\colorbox{lightgray}{$(\discovered \setminus \irr)$}}$)$
            \State $\qquad \qquad \discovered := \discovered \cup \occ(\psi_{X_k}^e)$
            \State \qquad \qquad {\colorbox{lightgray}{$\irr := \irr 
            \setminus \occ(\psi_{X_k}^e)$}} 
            \State \qquad \qquad {\colorbox{lightgray}{$\td, \irr := \text{\textsc{PruneTodo}}(\init, \td, \irr) \text{ (executed periodically)}$}} 
        \State \qquad $V := \textsc{ExtractMinimalStructureGraph}(V, \init, S_0, S_1, \tau_0, \tau_1)$
        \State \qquad \Return $(V,E, d, r)$
    \State $\mathbf{end\ function}$
    \end{algorithmic}

Pruning the todo set can help reduce the number of iterations to execute in the instantiation procedure.

Recursive procedure for solving structure graphs
------------------------------------------------


The algorithm to recursively solve a structure graph, Algorithm :ref:`SolveRecursive <ziel-expl>`, is based on Zielonka's recursive algorithm [Z98]_. It computes a partitioning of the set of vertices `V` of a structure graph `G = (V, E, d , r)` into `(W_0, W_1)` of vertices `W_0` that represent equations evaluating to true, and vertices `W_1` that represent equations evaluating to false. A precondition of this algorithm is that it contains no nodes with decoration `\top` or `\bot`. 

To satisfy the precondition, a pre-processing step is needed, see Algorithm :ref:`PreProcessing <preproc>`.

.. _preproc:

.. math::
    :nowrap:
    :class: math-left

        \begin{algorithmic}[1]
        \Function {PreProcessing}{$V$}
        \State \textbf{if} {$ V = \emptyset$} \textbf{then} \Return {$\emptyset$}
        \State $T_0 := \{ v \in V \mid d(v) = \top\}$
        \State $TT_0:=$ \textsc{AttrDefault}{$(T_0, 0)$}
        \State $T_1 := \{ v \in V \mid d(v) = \bot\}$
        \State $TT_1 :=$ \textsc{AttrDefault}{$(T_1, 1)$}
        \State $V := V \setminus (TT_0 \cup TT_1)$
        \State \Return {$V$}
        \EndFunction
        \end{algorithmic}


Algorithm :ref:`SolveRecursive <ziel-expl>` is as follows:

.. _ziel-expl:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \Function {SolveRecursive}{$V$}
    \State \textbf{if} {$ V = \emptyset$} \textbf{then} \Return {($\emptyset$,\ $\emptyset$)}
    \State $m := \min(\{\rnk(v) \mid v \in V\})$
    \State $\alpha := m \mod 2$
    \State $U := \{ v \in V \mid \rnk(v) = m\}$
    \State $A :=$ \textsc{AttrDefault}{$(U, \alpha)$}
    \State $W_{0}', W_{1}' := {\textsc{SolveRecursive}}{(V \setminus A)}$
    \State $\mathbf{if}\ W_{1-\alpha}' = \emptyset\ \mathbf{then}$ 
        \State $\qquad W_\alpha,W_{1 - \alpha} := A \cup W_{\alpha}', \emptyset$
    \State $\mathbf{else}$
        \State $\qquad B := $\textsc{AttrDefault}{$(W_{1 - \alpha}', 1 - \alpha)$}
        \State $\qquad W_0, W_1 := {\textsc{SolveRecursive}}{(V \setminus B)}$
        \State $\qquad W_{1 - \alpha} := W_{1 - \alpha} \cup B$
    \State \Return {($W_0, W_1$)}
    \EndFunction
    \end{algorithmic}

In Algorithm :ref:`SolveRecursive <ziel-expl>` the routine starts by checking whether the set of nodes `V` is empty and if so it returns a tuple with empty sets. Otherwise, `m` gets the value of the minimal rank of the nodes in `V`, `\alpha` (the player) is computed such that the result is either 0 or 1 depending on `m \mod 2`, and the set `U` is updated to be the set of all nodes in `V` that have rank `m`. Then, by executing routine `\textsc{AttrDefault}` and `\textsc{SolveRecursive}` a set of vertices `W_0` that represent equations evaluating to true, and a set of vertices `W_1` that represent equations evaluating to false are computed.

Note that this algorithm does not explicitly include how to extract strategies from the procedure of solving a structure graph.

Before defining Algorithm :ref:`SolveRecursiveStrategies <ziel-expl-tau>` which solves a structure graph and computes strategies `\tau_0, \tau_1`, let us define strategies function and priority function.

.. _strategy:

.. admonition:: Definition (:math:`\tau(U)`) 

    Let `U \subseteq V`, we define the strategy for nodes in set `U`, denoted as `\tau(U)`, such that:

    .. math::
        :nowrap:

        \[\tau(U)(v) = 
        \begin{cases}
            \bot & \text{if $v \notin U$ or $\scc(v) \neq \emptyset$,} \\
            w & \text{for some $w \in \scc(v)$.}
        \end{cases}\]

.. _priority:

.. admonition:: Definition (:math:`f_1 \lceil f_2`)

    Let `f_1, f_2: S \rightarrow T` be partial functions and `x \in S`, we define the priority function `f_1 \lceil f_2` as follows:

    .. math::
        :nowrap:

        \[f_1 \lceil f_2(x) = 
        \begin{cases}
            f_1(x) & \text{if $f_1(x) \neq \bot$} \\
            f_2(x) & \text{if $f_1(x) = \bot \wedge f_2 \neq \bot$}
        \end{cases}\]

The priority function application is left-associative, thus, given `f_1, \ldots, f_n` partial functions for `1 \leq i \leq n` then `f_1 \lceil f_2 \lceil \ldots f_{n-1}\lceil f_n` is interpreted as `(f_1 \lceil (f_2 \lceil (\ldots (f_{n-1}\lceil f_n)\cdots)))`.

.. _ziel-expl-tau:

.. math::
    :nowrap:
    :class: math-left

    \begin{algorithmic}[1]
    \Function {SolveRecursiveStrategies}{$V$}
    \State \textbf{if} {$ V = \emptyset$} \textbf{then} \Return {($\emptyset$,\ $\emptyset$,\ $\emptyset$, $\emptyset$)}
    \State $m := \min(\{\rnk(v) \mid v \in V\})$
    \State $\alpha := m \mod 2$
    \State $U := \{ v \in V \mid \rnk(v) = m\}$
    \State {\colorbox{lightgray}{$\tau_U := \tau(U)$}}
    \State {\colorbox{lightgray}{$A, \tau :=$ \textsc{AttrDefaultWithTau}{$(U, \alpha, \tau_U)$}}}
    \State {\colorbox{lightgray}{$W_{0}', W_{1}', \tau_{0}', \tau_{1}' := {\textsc{SolveRecursiveStrategies}}{(V \setminus A)}$}}
    \State {$\mathbf{if}\ W_{1-\alpha}' = \emptyset\ \mathbf{then}$} 
        \State $\qquad W_\alpha,W_{1 - \alpha} := A \cup W_{\alpha}', \emptyset$
        \State {\colorbox{lightgray}{$\qquad \tau_\alpha := \tau_{\alpha}' \lceil \tau \lceil \tau_U$}}
        \State {\colorbox{lightgray}{$\qquad \tau_{1 - \alpha} := \tau_{1 - \alpha}'$}}
    \State $\mathbf{else}$
        \State {\colorbox{lightgray}{$\qquad B,\tau_B := $\textsc{AttrDefaultWithTau}{$(W_{1 - \alpha}', 1 - \alpha, \tau_{1 - \alpha}')$}}}
        \State {\colorbox{lightgray}{$\qquad W_0, W_1, \tau_0, \tau_1 := {\textsc{SolveRecursiveStrategies}}{(V \setminus B)}$}}
        \State $\qquad W_{1 - \alpha} := W_{1 - \alpha} \cup B$
        \State {\colorbox{lightgray}{$\qquad \tau_{1 - \alpha} := \tau_{1 - \alpha} \lceil \tau_B \lceil \tau_{1 - \alpha}'$}}
        \State {\colorbox{lightgray}{$\qquad \tau_\alpha := \tau_\alpha \lceil \tau_{\alpha}'$}}
    \State \Return {($W_0, W_1, \tau_0, \tau_1$)}
    \EndFunction
    \end{algorithmic}

In line 11 the strategy for player `\alpha` is updated, in particular `\tau_U` is the strategy computed in set `U`, `\tau` is the strategy that results from the computation of the attractor set of `U` and `\tau_{\alpha}'` is the strategy resulting from the recursive call to `\textsc{SolveRecursive}(V\setminus A)`. In line 12 the strategy for player `1- \alpha` is empty. In line 17 the strategy for player `1- \alpha` is updated, in particular `\tau_{1-\alpha}'` is the strategy resulting from the recursive call to `\textsc{SolveRecursive}(V\setminus A)`, `\tau_B` is the strategy that results from the computation of the attractor set of `W_{1 - \alpha}'` and `\tau_{1-\alpha}` is the strategy resulting from the recursive call to `\textsc{SolveRecursive}(V\setminus B)`.


References
----------

.. [KRW12] J.J.A. Keiren, M.A. Reniers and T.A.C. Willemse. Structural Analysis
            of Boolean Equation Systems. In ACM Transactions on Computational Logic 13(1): 8-1/35, 2012. `(DOI) <http://doi.acm.org/10.1145/2071368.2071376>`__

.. [HKP13] M. Huth, J.HP. Kuo, N. Piterman, (2013). Fatal Attractors in Parity 
            Games. In: Pfenning, F. (eds) Foundations of Software Science and Computation Structures. FoSSaCS 2013. Lecture Notes in Computer Science, vol 7794. Springer, Berlin, Heidelberg. `(DOI) <https://doi.org/10.1007/978-3-642-37075-5_3>`__


.. [Z98] W. Zielonka, Infinite games on finitely coloured graphs with 
        applications to automata on infinite trees. Theor. Comput. Sci. 200, 1-2 (1998), 135–183, `(DOI) <https://doi.org/10.1016/S0304-3975(98)00009-7>`__