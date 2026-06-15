Gauß elimination
================

A predicate formula :math:`\varphi` is defined by the following grammar:

.. math::

   \varphi ::= b \mid X(e) \mid \lnot\varphi \mid \varphi \wedge \varphi \mid
   \varphi \vee \varphi \mid \varphi \rightarrow \varphi \mid
   \forall d{:}D.\,\varphi \mid \exists d{:}D.\,\varphi \mid
   \mathit{true} \mid \mathit{false}

where :math:`b` is a data term of sort :math:`\mathbb{B}`, :math:`X` is a
predicate variable, :math:`d` is a data variable of sort :math:`D`, :math:`e`
is a data term, :math:`\mathit{true}` represents true, and
:math:`\mathit{false}` represents false.

.. definition:: Predicate Variable Substitution

   Let :math:`\varphi, \psi` be predicate formulae and :math:`X` a predicate
   variable. Then we define :math:`\psi[\varphi/X]` as the result of applying
   the substitution :math:`X := \varphi` to the formula :math:`\psi`. To make
   this more precise: suppose :math:`X` is declared as :math:`X(d{:}D)`, then
   any occurrence :math:`X(\bar{d})` in :math:`\psi` is replaced by
   :math:`\varphi[d{:=}\bar{d}]`.

.. lemma:: Substitution

   Let :math:`\mathcal{E}` be an equation system for which
   :math:`X, Y \notin \mathit{bnd}(\mathcal{E})`, then:

   .. math::

      (\sigma X(d{:}D)=\varphi)\,\mathcal{E}\,(\sigma' Y(e{:}E)=\psi)
      \equiv
      (\sigma X(d{:}D)=\varphi)[\psi/Y]\,\mathcal{E}\,(\sigma' Y(e{:}E)=\psi)

.. definition:: Approximation

   Let :math:`\varphi, \psi` be predicate formulae and :math:`X` a predicate
   variable. We inductively define :math:`\psi[\varphi/X]^k` as follows:

   .. math::

      \begin{array}{l}
      \psi[\varphi/X]^0 \overset{\mathrm{def}}{=} \varphi \\
      \psi[\varphi/X]^{k+1} \overset{\mathrm{def}}{=} \psi[\varphi/X]^k
      \end{array}

Thus, :math:`\psi[\varphi/X]^k` represents the result of recursively
substituting :math:`\varphi` for :math:`X` in :math:`\psi`.

.. lemma:: Approximants as Solutions

   Let :math:`\varphi` be a predicate formula and :math:`k \in \mathbb{N}`.
   Then

   .. math::

      \begin{array}{l}
      (\mu X(d{:}D) = \varphi[\mathit{false}/X]^k) \Rrightarrow (\mu X(d{:}D) = \varphi) \\
      (\nu X(d{:}D) = \varphi) \Rrightarrow (\nu X(d{:}D) = \varphi[\mathit{true}/X]^k)
      \end{array}

.. lemma:: Stable Approximants as Solutions

   Let :math:`\varphi` be a predicate formula and :math:`k \in \mathbb{N}`.
   Then

   .. math::

      \begin{array}{l}
      \text{if } \varphi[\mathit{false}/X]^k \longleftrightarrow \varphi[\mathit{false}/X]^{k+1}
        \text{ then } (\mu X(d{:}D) = \varphi[\mathit{false}/X]^k) \equiv (\mu X(d{:}D) = \varphi) \\
      \text{if } \varphi[\mathit{true}/X]^k \longleftrightarrow \varphi[\mathit{true}/X]^{k+1}
        \text{ then } (\nu X(d{:}D) = \varphi[\mathit{true}/X]^k) \equiv (\nu X(d{:}D) = \varphi)
      \end{array}

Gauß elimination algorithm
--------------------------

Let :math:`\mathcal{E}` be an equation system of the form

.. math::

   \mathcal{E} = (\sigma_1 X_1(d_1{:}D_1) = \varphi_1) \cdots
   (\sigma_n X_n(d_n{:}D_n) = \varphi_n),

and let :math:`r` be a rewrite function that maps a PBES expression
:math:`\varphi` to an equivalent expression :math:`\varphi'`. Then we define:

.. math::

   \begin{array}{l}
   \operatorname{Gau\text{ß}\ Elimination}(\mathcal{E},\, r,\, \operatorname{solve}) \\
   \mathcal{E}' := \varepsilon \\
   i := n \\
   \mathbf{while\ not}\ i = 0\ \mathbf{do} \\
   \quad \varphi_i := \operatorname{solve}(\sigma_i X_i = \varphi_i) \\
   \quad \mathcal{E}' := \mathcal{E}'(\sigma_i X_i = \varphi_i) \\
   \quad \mathbf{for}\ k = 1\ \mathbf{to}\ i-1\ \mathbf{do}\quad
     \varphi_k := r(\varphi_k[\varphi_i/X_i])\ \mathbf{od} \\
   \quad i := i - 1 \\
   \mathbf{od} \\
   \mathbf{return}\ \mathcal{E}'
   \end{array}

Here :math:`\operatorname{SolveEquation}` is an algorithm that solves a PBES
equation such that the resulting equation has no reference to the predicate
variable in its right hand side. An example of such a solve equation algorithm
is :math:`\operatorname{Approximate}`.

.. math::

   \begin{array}{l}
   \operatorname{Approximate}(\sigma X = \varphi,\, \operatorname{compare}) \\
   j := 0 \\
   \mathbf{if}\ \sigma = \nu\ \mathbf{then}\ \psi_0 := \mathit{true}\ \mathbf{else}\ \psi_0 := \mathit{false} \\
   \mathbf{repeat} \\
   \quad \psi_{j+1} := \varphi[\psi_j/X] \\
   \quad j := j + 1 \\
   \mathbf{until}\ \operatorname{compare}(\psi_j, \psi_{j+1}) \\
   \mathbf{return}\ \sigma X = \psi_j
   \end{array}

Also pattern matching algorithms exist for this. The Gauß Elimination
algorithm solves the equation system :math:`\mathcal{E}` for the predicate
variable :math:`X_1`. To solve the system :math:`\mathcal{E}` for all
variables, the algorithm has to be applied repeatedly.

Solving a BES
-------------

If the equation system :math:`\mathcal{E}` is a BES (i.e. the predicate
variables have no parameters), then the following simple approximate function
can be used to solve it:

.. math::

   \begin{array}{l}
   \operatorname{Approximate\text{-}BES}(\sigma X = \varphi) \\
   \mathbf{if}\ \sigma = \nu\ \mathbf{then}\ \psi_0 := \mathit{true}\ \mathbf{else}\ \psi_0 := \mathit{false} \\
   \mathbf{return}\ \operatorname{Simplify}(\sigma X = \varphi[\psi_0/X])
   \end{array}
