Parity game generator
=====================

Let :math:`\mathcal{E} = (\sigma_1 X_1(d_1{:}D_1)=\varphi_1) \ldots (\sigma_n X_n(d_n{:}D_n)=\varphi_n)`
be a PBES with initial state :math:`X_{init}(e_{init})`, and let
:math:`R : \mathit{PbesTerm} \rightarrow \mathit{PbesTerm}` be a rewriter. The
PBES must be in normal form, i.e. it may not contain negations or implications.
The following algorithm computes a BES. The generated equations are in a
restricted format, such that the BES can be taken as input for a parity game
solver.

.. math::

   \begin{array}{l}
   \operatorname{GenerateBES}(\mathcal{E},\; X_{init}(e_{init}),\; R) \\
   \mathit{result} := \{(\nu Y_\top = Y_\top),\; (\mu Y_\bot = Y_\bot)\} \\
   \mathit{visited} := \{R(X_{init}(e_{init}))\} \\
   \mathit{explored} := \{\top, \bot\} \\
   \mathbf{while}\ \mathit{visited} \neq \emptyset\ \mathbf{do} \\
   \quad \mathbf{choose}\ \psi \in \mathit{visited} \setminus \mathit{explored} \\
   \quad \mathit{visited} := \mathit{visited} \setminus \{\psi\} \\
   \quad \mathit{explored} := \mathit{explored} \cup \{\psi\} \\
   \quad \mathbf{if}\ \{\psi = X_k(e)\}\ \mathbf{then} \\
   \quad\quad \xi := R(\varphi_k[d_k{:=}e]) \\
   \quad \mathbf{else} \\
   \quad\quad \xi := \psi \\
   \quad \mathbf{if}\ \{\xi = X_k(e)\}\ \mathbf{then} \\
   \quad\quad \mathit{result} := \mathit{result} \cup (\sigma_\psi Y_\psi = Y_\xi) \\
   \quad\quad \sigma_\xi := \sigma_k \\
   \quad\quad \mathit{visited} := \mathit{visited} \cup \{\xi\} \\
   \quad \mathbf{else\ if}\ \{\xi = \bigwedge_{j \in J} \phi_j\}\ \mathbf{then} \\
   \quad\quad \mathit{result} := \mathit{result} \cup (\sigma_\psi Y_\psi = \bigwedge_{j \in J} Y_{\phi_j}) \\
   \quad\quad \mathbf{for}\ j \in J\ \mathbf{do} \\
   \quad\quad\quad \mathbf{if}\ \{\xi = X_k(e)\}\ \mathbf{then}\ \sigma_{\phi_j} := \sigma_k\ \mathbf{else}\ \sigma_{\phi_j} := \sigma_\psi \\
   \quad\quad \mathit{visited} := \mathit{visited} \cup \{\phi_j\}_{j \in J} \\
   \quad \mathbf{else\ if}\ \{\xi = \bigvee_{j \in J} \phi_j\}\ \mathbf{then} \\
   \quad\quad \mathit{result} := \mathit{result} \cup (\sigma_\psi Y_\psi = \bigvee_{j \in J} Y_{\phi_j}) \\
   \quad\quad \mathbf{for}\ j \in J\ \mathbf{do} \\
   \quad\quad\quad \mathbf{if}\ \{\xi = X_k(e)\}\ \mathbf{then}\ \sigma_{\phi_j} := \sigma_k\ \mathbf{else}\ \sigma_{\phi_j} := \sigma_\psi \\
   \quad\quad \mathit{visited} := \mathit{visited} \cup \{\phi_j\}_{j \in J} \\
   \quad \mathbf{else\ if}\ \{\xi = \top\}\ \mathbf{then} \\
   \quad\quad \mathit{result} := \mathit{result} \cup (\sigma_\psi Y_\psi = Y_\top) \\
   \quad \mathbf{else\ if}\ \{\xi = \bot\}\ \mathbf{then} \\
   \quad\quad \mathit{result} := \mathit{result} \cup (\sigma_\psi Y_\psi = Y_\bot) \\
   \mathbf{return}\ \mathit{result}
   \end{array}

In every step of the while loop the equation for :math:`Y_\psi` is computed.
If the right hand side of the equation for :math:`Y_\psi` is a propositional
variable instantiation, it is expanded into the right hand side of the
corresponding PBES equation. Otherwise it is converted into a disjunction or
conjunction by introducing new BES variables. The rewriter :math:`R` is
expected to eliminate all quantifiers, so the while loop does not contain
cases for handling them. The order of the equations in the BES is significant.
Therefore in the implementation instead of fixpoint symbols :math:`\sigma_\psi`
priority values are used. The BES variables :math:`Y_\psi` are represented by
integers.

An alternative for inserting the equations :math:`(\nu Y_\top = Y_\top)` and
:math:`(\mu Y_\bot = Y_\bot)` at the beginning of the resulting BES is to
replace :math:`\mu Y_\psi = \top` and :math:`\nu Y_\psi = \top` by
:math:`\nu Y_\psi = Y_\psi`, and to replace :math:`\mu Y_\psi = \bot` and
:math:`\nu Y_\psi = \bot` by :math:`\mu Y_\psi = Y_\psi`. This eliminates the
need to introduce special equations for true and false.

This algorithm is implemented in the class ``parity_game_generator``. The
choice for :math:`\psi` in the while loop is left to the user of the class.
