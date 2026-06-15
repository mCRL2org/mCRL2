ReachableVariables
==================

Let :math:`\mathcal{E} = (\sigma_1 X_1(d_{X_1}{:}D_{X_1})=\varphi_{X_1}) \cdots (\sigma_n X_n(d_{X_n}{:}D_{X_n})=\varphi_{X_n})`
be a PBES, and let :math:`X_{init}(e_{init})` be the initial state. The
algorithm :math:`\operatorname{ReachableVariables}` computes the reachable
predicate variables.

.. math::

   \begin{array}{l}
   \operatorname{ReachableVariables}(\mathcal{E},\; X_{init}) \\
   \mathit{visited} := \{X_{init}\};\quad \mathit{explored} := \emptyset \\
   \mathbf{while}\ \mathit{visited} \neq \emptyset \\
   \quad \mathbf{choose}\ X_i \in \mathit{visited} \\
   \quad \mathit{visited} := \mathit{visited} \setminus \{X_i\} \\
   \quad \mathit{explored} := \mathit{explored} \cup \{X_i\} \\
   \quad \mathbf{for\ each}\ X_j(e) \in \mathsf{iocc}(\varphi_{X_i}) \\
   \quad\quad \mathbf{if}\ X_j \notin \mathit{explored} \\
   \quad\quad\quad \mathit{visited} := \mathit{visited} \cup \{X_j\} \\
   \mathbf{return}\ \mathit{explored}
   \end{array}

Appendix
========

ATerm format
------------

.. math::

   \begin{array}{ll}
   \mathtt{<DataExpr>} & c \\
   \mathtt{StateTrue} & \mathit{true} \\
   \mathtt{StateFalse} & \mathit{false} \\
   \mathtt{StateNot(<StateFrm>)} & \lnot\varphi \\
   \mathtt{StateAnd(<StateFrm>,<StateFrm>)} & \varphi \wedge \varphi \\
   \mathtt{StateOr(<StateFrm>,<StateFrm>)} & \varphi \vee \varphi \\
   \mathtt{StateImp(<StateFrm>,<StateFrm>)} & \varphi \Rightarrow \varphi \\
   \mathtt{StateForall(<DataVarId>+,<StateFrm>)} & \forall x{:}D.\,\varphi \\
   \mathtt{StateExists(<DataVarId>+,<StateFrm>)} & \exists x{:}D.\,\varphi \\
   \mathtt{StateMust(<RegFrm>,<StateFrm>)} & \langle\alpha\rangle\varphi \\
   \mathtt{StateMay(<RegFrm>,<StateFrm>)} & [\alpha]\varphi \\
   \mathtt{StateYaled} & \nabla \\
   \mathtt{StateYaledTimed(<DataExpr>)} & \nabla(t) \\
   \mathtt{StateDelay} & \Delta \\
   \mathtt{StateDelayTimed(<DataExpr>)} & \Delta(t) \\
   \mathtt{StateVar(<String>,<DataExpr>*)} & X(d) \\
   \mathtt{StateNu(<String>,<DataVarIdInit>*,<StateFrm>)} & \nu X(x{:}D{:=}d).\,\varphi \\
   \mathtt{StateMu(<String>,<DataVarIdInit>*,<StateFrm>)} & \mu X(x{:}D{:=}d).\,\varphi
   \end{array}

Naming conventions
------------------

.. math::

   \begin{array}{lcl}
   \mathsf{left}(\varphi \otimes \psi) &=& \varphi \\
   \mathsf{right}(\varphi \otimes \psi) &=& \psi \\
   \arg(\lnot\varphi) &=& \varphi \\
   \arg(\forall d{:}D.\,\varphi) = \arg(\exists d{:}D.\,\varphi) &=& \varphi \\
   \mathsf{var}(\forall d{:}D.\,\varphi) = \mathsf{var}(\exists d{:}D.\,\varphi) &=& d{:}D \\
   \arg(\langle\alpha\rangle\varphi) = \arg([\alpha]\varphi) &=& \varphi \\
   \mathsf{act}(\langle\alpha\rangle\varphi) = \mathsf{act}([\alpha]\varphi) &=& \alpha \\
   \mathsf{time}(\nabla(t)) = \mathsf{time}(\Delta(t)) &=& t \\
   \mathsf{var}(X(d{:}D)) &=& d{:}D \\
   \arg(\sigma X(d{:}D{:=}e).\,\varphi) &=& \varphi \\
   \mathsf{name}(\sigma X(d{:}D{:=}e).\,\varphi) &=& X \\
   \mathsf{var}(\sigma X(d{:}D{:=}e).\,\varphi) &=& d{:}D \\
   \mathsf{val}(\sigma X(d{:}D{:=}e).\,\varphi) &=& e
   \end{array}

where :math:`\sigma` is either :math:`\mu` or :math:`\nu`, and :math:`\otimes`
is either :math:`\wedge`, :math:`\vee`, or :math:`\Rightarrow`.
