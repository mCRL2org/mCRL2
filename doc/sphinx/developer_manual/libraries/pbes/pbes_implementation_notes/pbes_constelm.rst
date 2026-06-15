Constant parameter detection and elimination
============================================

Let :math:`\mathcal{E} = (\sigma_1 X_1(d_{X_1}{:}D_{X_1})=\varphi_{X_1}) \cdots (\sigma_n X_n(d_{X_n}{:}D_{X_n})=\varphi_{X_n})`
be a PBES. Here, every :math:`d_{X_i}` represents a vector of parameters.
Furthermore, let :math:`\hat{X}(\hat{e})` be an initial state and let
:math:`\mathsf{eval}` be an evaluator function on data expressions. We denote
the :math:`i`-th element of a vector :math:`x` as :math:`x[i]`. We also use
mappings: for a mapping :math:`c`, the image of :math:`i` is denoted
:math:`c[i]`. The empty mapping is denoted with :math:`\emptyset` and the
image of an element not present in a mapping is :math:`\bot`. Note that
:math:`\emptyset[i] = \bot` for all :math:`i`. Then we define the algorithm
:math:`\operatorname{PbesConstelm}` as follows:

.. math::

   \begin{array}{l}
   \operatorname{PbesConstelm}(\mathcal{E},\; \hat{X}(\hat{e}),\; \mathsf{eval}) \\
   \mathbf{for}\ X \in \mathsf{bnd}(\mathcal{E})\ \mathbf{do}\quad c_X := \emptyset \\
   c_{\hat{X}} := update(c_{\hat{X}},\, \mathsf{eval}(\hat{e})) \\
   \mathit{todo} := \{\hat{X}\} \\
   \mathbf{while}\ \mathit{todo} \neq \emptyset\ \mathbf{do} \\
   \quad \mathbf{choose}\ X \in \mathit{todo} \\
   \quad \mathit{todo} := \mathit{todo} \setminus \{X\} \\
   \quad \mathbf{for}\ Y(e) \in \mathsf{iocc}(\varphi_X)\ \mathbf{do} \\
   \quad\quad \mathbf{if}\ \mathsf{eval}(\mathit{needs\_update}(Y(e),\varphi_X)[d_X{:=}c_X]) \neq \mathit{false}\ \mathbf{then} \\
   \quad\quad\quad c_Y' := update(c_Y,\, \mathsf{eval}(e[d_X{:=}c_X])) \\
   \quad\quad\quad \mathbf{if}\ c_Y' \neq c_Y\ \mathbf{then} \\
   \quad\quad\quad\quad c_Y := c_Y' \\
   \quad\quad\quad\quad \mathit{todo} := \mathit{todo} \cup \{Y\} \\
   \mathit{constant\_parameters} := \{(X,i) \mid c_X[i] \neq d_X[i]\} \\
   \mathbf{for}\ i := 1 \cdots n\ \mathbf{do}\quad
     \varphi_{X_i} := \varphi_{X_i}[d_{X_i}{:=}c_{X_i}] \\
   \mathbf{return}\ \mathit{constant\_parameters}
   \end{array}

where :math:`update_X` is defined as follows:

.. math::

   update(c, e) =_{\mathit{def}} c',\quad\text{with}\quad c'[i] = \left\{
   \begin{array}{ll}
   \bot & \text{if } c = \emptyset \text{ and } e = [] \\
   e[i] & \text{if } c = \emptyset \text{ and } e[i] \text{ is constant} \\
   c[i] & \text{if } e[i] = c[i] \\
   d_X[i] & \text{otherwise}
   \end{array}\right.

and where :math:`\mathit{needs\_update}` is a boolean function that determines
whether an update should be performed. A safe choice for this function is the
constant function :math:`\mathit{true}`. [Simon Janssen, 2008] originally
proposed an alternative based on a syntactical analysis of predicate formulae.
The following is an improved version of his definitions.

Let :math:`c` be defined as

.. math::

   \begin{array}{llllll}
   c_T(c) &=& c & c_F(c) &=& \lnot c \\
   c_T(\lnot\varphi) &=& c_F(\varphi) & c_F(\lnot\varphi) &=& c_T(\varphi) \\
   c_T(X(e)) &=& \mathit{true} & c_F(X(e)) &=& \mathit{true} \\
   c_T(\mathsf{Q}\,d{:}D.\,\varphi) &=& \mathsf{Q}\,d{:}D.\,c_T(\varphi) &
   c_F(\mathsf{Q}\,d{:}D.\,\varphi) &=& \mathsf{Q}\,d{:}D.\,c_F(\varphi) \\
   c_T(\varphi \wedge \psi) &=& c_T(\varphi) \wedge c_T(\psi) &
   c_F(\varphi \wedge \psi) &=& c_F(\varphi) \vee c_F(\psi) \\
   c_T(\varphi \vee \psi) &=& c_T(\varphi) \vee c_T(\psi) &
   c_F(\varphi \vee \psi) &=& c_F(\varphi) \wedge c_F(\psi) \\
   c_T(\varphi \Rightarrow \psi) &=& c_F(\varphi) \vee c_T(\psi) &
   c_F(\varphi \Rightarrow \psi) &=& c_T(\varphi) \wedge c_F(\psi)
   \end{array}

and let the set :math:`\mathit{cond}` be defined as

.. math::

   \begin{array}{lll}
   cond(X(e), c) &=& \emptyset \\
   cond(X(e), Y(f)) &=& \emptyset \\
   cond(X(e), \lnot\varphi) &=& cond(X(e), \varphi) \\[4pt]
   cond(X(e), \varphi \wedge \psi) &=& \left\{\begin{array}{ll}
     \{c_T(\psi)\} \cup cond(X(e), \varphi) & \text{if } X(e) \in \mathsf{iocc}(\varphi) \\
     \{c_T(\varphi)\} \cup cond(X(e), \psi) & \text{if } X(e) \in \mathsf{iocc}(\psi) \\
     \emptyset & \text{otherwise}
   \end{array}\right. \\[4pt]
   cond(X(e), \varphi \vee \psi) &=& \left\{\begin{array}{ll}
     \{c_F(\psi)\} \cup cond(X(e), \varphi) & \text{if } X(e) \in \mathsf{iocc}(\varphi) \\
     \{c_F(\varphi)\} \cup cond(X(e), \psi) & \text{if } X(e) \in \mathsf{iocc}(\psi) \\
     \emptyset & \text{otherwise}
   \end{array}\right. \\[4pt]
   cond(X(e), \varphi \Rightarrow \psi) &=& \left\{\begin{array}{ll}
     \{c_F(\psi)\} \cup cond(X(e), \varphi) & \text{if } X(e) \in \mathsf{iocc}(\varphi) \\
     \{c_T(\varphi)\} \cup cond(X(e), \psi) & \text{if } X(e) \in \mathsf{iocc}(\psi) \\
     \emptyset & \text{otherwise}
   \end{array}\right. \\[4pt]
   cond(X(e), \mathsf{Q}\,d{:}D.\,\varphi) &=& \left\{\begin{array}{ll}
     \{c_\Gamma(\forall d{:}D.\,\varphi)\} \cup
       \{\exists d{:}D.\,\theta \mid \theta \in cond(X(e),\varphi)\}
       & \text{if } X(e) \in \mathsf{iocc}(\varphi) \\
     \emptyset & \text{otherwise}
   \end{array}\right.
   \end{array}

with :math:`\mathsf{Q} \in \{\forall, \exists\}`. Then we define

.. math::

   \mathit{needs\_update}(X(e), \varphi) = \bigwedge_{c \in cond(X(e),\varphi)} c

The implementation of these three functions is integrated into one recursive
traverser. The resulting condition is quadratic in the number of quantifier
alternations in which scope :math:`X(e)` occurs and linear in the other
operators. Most PBESs stemming from model checking do not yield conditions
larger than those contained in the LPS. Furthermore, this traverser is only
executed once, a priori. This means that computing the conditions is relatively
cheap.
