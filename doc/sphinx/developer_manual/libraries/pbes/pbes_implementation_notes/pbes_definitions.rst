Definitions
===========

Parameterised Boolean Equation Systems (PBESs) are empty (denoted
:math:`\epsilon`) or finite sequences of fixed point equations, where each
equation is of the form :math:`(\mu X(d{:}D) = \phi)` or
:math:`(\nu X(d{:}D) = \phi)`. The left-hand side of each equation consists
of a *fixed point symbol*, where :math:`\mu` indicates a least and :math:`\nu`
a greatest fixed point, and a sorted predicate variable :math:`X` of sort
:math:`D \rightarrow \mathbb{B}`, taken from some countable domain of sorted
predicate variables :math:`\mathcal{X}`. The right-hand side of each equation
is a predicate formula as defined below.

.. definition::

   *Predicate formulae* :math:`\phi` are defined by the following grammar:

   .. math::

      \phi ::= b \mid X(e) \mid \lnot\phi \mid \phi \oplus \phi \mid \mathsf{Q}\,d{:}D.\,\phi

   where :math:`\oplus \in \{\wedge, \vee, \Rightarrow\}`,
   :math:`\mathsf{Q} \in \{\forall, \exists\}`, :math:`b` is a data term of
   sort :math:`\mathsf{B}`, :math:`X` is a predicate variable, :math:`d` is a
   data variable of sort :math:`D`, and :math:`e` is a vector of data terms.

The set of predicate variables that occur in a predicate formula :math:`\phi`,
denoted by :math:`\mathsf{occ}`, is defined recursively as follows, for any
formulae :math:`\phi_1, \phi_2`:

.. math::

   \begin{array}{llll}
   \mathsf{occ}(b) &=_{\mathit{def}} \emptyset &
   \mathsf{occ}(X(e)) &=_{\mathit{def}} \{X\} \\
   \mathsf{occ}(\phi_1 \oplus \phi_2) &=_{\mathit{def}}
     \mathsf{occ}(\phi_1) \cup \mathsf{occ}(\phi_2) \qquad &
   \mathsf{occ}(\mathsf{Q}\,d{:}D{.\,}\phi_1) &=_{\mathit{def}} \mathsf{occ}(\phi_1)
   \end{array}

Extended to equation systems, :math:`\mathsf{occ}(\mathcal{E})` is the union
of all variables occurring at the right-hand side of equations in
:math:`\mathcal{E}`. Likewise, the set of predicate variable instantiations
that occur in a predicate formula :math:`\phi` is denoted by
:math:`\mathsf{iocc}`, and is defined recursively as follows:

.. math::

   \begin{array}{llll}
   \mathsf{iocc}(b) &=_{\mathit{def}} \emptyset &
   \mathsf{iocc}(X(e)) &=_{\mathit{def}} \{X(e)\} \\
   \mathsf{iocc}(\phi_1 \oplus \phi_2) &=_{\mathit{def}}
     \mathsf{iocc}(\phi_1) \cup \mathsf{iocc}(\phi_2) \qquad &
   \mathsf{iocc}(\mathsf{Q}\,d{:}D{.\,}\phi_1) &=_{\mathit{def}} \mathsf{iocc}(\phi_1)
   \end{array}

For any equation system :math:`\mathcal{E}`, the set of *binding predicate
variables*, :math:`\mathsf{bnd}(\mathcal{E})`, is the set of variables
occurring at the left-hand side of some equation in :math:`\mathcal{E}`.
Formally:

.. math::

   \begin{array}{llll}
   \mathsf{bnd}(\epsilon) &=_{\mathit{def}} \emptyset \qquad &
   \mathsf{bnd}((\sigma X(d{:}D)=\phi)\,\mathcal{E}) &=_{\mathit{def}}
     \mathsf{bnd}(\mathcal{E}) \cup \{X\} \\
   \mathsf{occ}(\epsilon) &=_{\mathit{def}} \emptyset \qquad &
   \mathsf{occ}((\sigma X(d{:}D)=\phi)\,\mathcal{E}) &=_{\mathit{def}}
     \mathsf{occ}(\mathcal{E}) \cup \mathsf{occ}(\phi)
   \end{array}

Let :math:`\mathsf{dvar}(d)` be the set of *free data variables* occurring in
a data term :math:`d`. The function :math:`\mathsf{dvar}` is extended to
predicate formulae using:

.. math::

   \begin{array}{llll}
   \mathsf{dvar}(X(e)) &=_{\mathit{def}} \mathsf{dvar}(e) &
   \mathsf{dvar}(\mathsf{Q}\,d{:}D{.\,}\phi_1) &=_{\mathit{def}}
     \mathsf{dvar}(\phi_1) \setminus \mathsf{dvar}(d) \\
   \mathsf{dvar}(\phi_1 \oplus \phi_2) &=_{\mathit{def}}
     \mathsf{dvar}(\phi_1) \cup \mathsf{iocc}(\phi_2) \qquad &  &
   \end{array}

The set of freely occurring predicate variables in :math:`\mathcal{E}`,
denoted :math:`\mathsf{pvar}(\mathcal{E})`, is defined as
:math:`\mathsf{occ}(\mathcal{E}) \setminus \mathsf{bnd}(\mathcal{E})`. An
equation system :math:`\mathcal{E}` is said to be *well-formed* iff every
binding predicate variable occurs at the left-hand side of precisely one
equation of :math:`\mathcal{E}`. We only consider well-formed equation systems
in this document.

An equation system :math:`\mathcal{E}` is called *closed* if
:math:`\mathsf{pvar}(\mathcal{E}) = \emptyset` and *open* otherwise. An
equation :math:`(\sigma X(d{:}D)=\phi)`, where :math:`\sigma` denotes either
the fixed point sign :math:`\mu` or :math:`\nu`, is called *data-closed* if
the set of data variables that occur freely in :math:`\phi` is contained in
the set of variables induced by the vector of variables :math:`d`. An equation
system is called *data-closed* iff each of its equations is data-closed.

.. definition::

   *Action formulae* :math:`\alpha` are defined by the following grammar:

   .. math::

      \alpha ::= b \mid \lnot\alpha \mid \alpha \oplus \alpha \mid
      \mathsf{Q}\,d{:}D.\,\alpha \mid a(d) \mid \alpha \mathbin{@} t

   where :math:`\oplus \in \{\wedge, \vee, \Rightarrow\}`,
   :math:`\mathsf{Q} \in \{\forall, \exists\}`, :math:`b` is a data term of
   sort :math:`\mathsf{B}`, :math:`X` is a predicate variable, :math:`d` is a
   data variable of sort :math:`D`, and :math:`a` is an action label.

.. definition::

   *State formulae* :math:`\phi` are defined by the following grammar:

   .. math::

      \phi ::= b \mid X(e) \mid \lnot\phi \mid \phi \oplus \phi \mid
      \mathsf{Q}\,d{:}D.\,\phi \mid \langle\alpha\rangle\phi \mid [\alpha]\phi
      \mid \Delta \mid \Delta(t) \mid \nabla \mid \nabla(t) \mid
      \sigma X(d{:}D{:=}e)

   where :math:`\oplus \in \{\wedge, \vee, \Rightarrow\}`,
   :math:`\mathsf{Q} \in \{\forall, \exists\}`,
   :math:`\sigma \in \{\mu, \nu\}`, :math:`b` is a data term of sort
   :math:`\mathsf{B}`, :math:`X` is a predicate variable, :math:`d` is a data
   variable of sort :math:`D`, :math:`e` is a vector of data terms, and
   :math:`\alpha` is an action formula.

Well typedness constraints
--------------------------

Well typedness constraints for PBES equations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- The binding variable parameters have unique names.
- The names of the quantifier variables in the equation are disjoint with the
  binding variable parameter names.
- Within the scope of a quantifier variable in the formula, no other quantifier
  variables with the same name may occur.

Well typedness constraints for PBESs
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- The sorts occurring in the global variables of the equations are declared in
  the data specification.
- The sorts occurring in the binding variable parameters are declared in the
  data specification.
- The sorts occurring in the quantifier variables of the equations are declared
  in the data specification.
- The binding variables of the equations have unique names (well formedness).
- The global variables occurring in the equations are declared in the global
  variable specification.
- The global variables occurring in the equations with the same name are
  identical.
- The declared global variables and the quantifier variables occurring in the
  equations have different names.
- The predicate variable instantiations occurring in the equations match with
  their declarations.
- The predicate variable instantiation occurring in the initial state matches
  with the declaration.
- The data specification is well typed.

Monotonicity
------------

.. definition::

   A state formula is called *monotonous* if it can be rewritten such that
   propositional variables are not inside the scope of a negation or an
   implication. More formally, a state formula :math:`\varphi` is monotonous
   if :math:`m(\varphi) = \mathsf{true}`, where :math:`m` is defined as
   follows. This definition applies to predicate formulae as well.

.. math::

   \begin{array}{lll}
   m(\lnot b) &=_{\mathit{def}}& \mathsf{true} \\
   m(\lnot\lnot\varphi) &=_{\mathit{def}}& m(\varphi) \\
   m(\lnot(\varphi \wedge \psi)) &=_{\mathit{def}}& m(\lnot\varphi) \wedge m(\lnot\psi) \\
   m(\lnot(\varphi \vee \psi)) &=_{\mathit{def}}& m(\lnot\varphi) \wedge m(\lnot\psi) \\
   m(\lnot(\varphi \Rightarrow \psi)) &=_{\mathit{def}}& m(\varphi) \wedge m(\lnot\psi) \\
   m(\lnot\forall d{:}D.\,\varphi) &=_{\mathit{def}}& m(\lnot\varphi) \\
   m(\lnot\exists d{:}D.\,\varphi) &=_{\mathit{def}}& m(\lnot\varphi) \\
   m(\lnot[\alpha]\varphi) &=_{\mathit{def}}& m(\lnot\varphi) \\
   m(\lnot\langle\alpha\rangle\varphi) &=_{\mathit{def}}& m(\lnot\varphi) \\
   m(\lnot\nabla) &=_{\mathit{def}}& \mathsf{true} \\
   m(\lnot\nabla(t)) &=_{\mathit{def}}& \mathsf{true} \\
   m(\lnot\Delta) &=_{\mathit{def}}& \mathsf{true} \\
   m(\lnot\Delta(t)) &=_{\mathit{def}}& \mathsf{true} \\
   m(\lnot X(e)) &=_{\mathit{def}}& \mathsf{false} \\
   m(\lnot\mu X(d{:}D{:=}e).\,\varphi) &=_{\mathit{def}}&
     m(\lnot\varphi[X{:=}\lnot X]) \\
   m(\lnot\nu X(d{:}D{:=}e).\,\varphi) &=_{\mathit{def}}&
     m(\lnot\varphi[X{:=}\lnot X]) \\[4pt]
   m(b) &=_{\mathit{def}}& \mathsf{true} \\
   m(\varphi \wedge \psi) &=_{\mathit{def}}& m(\varphi) \wedge m(\psi) \\
   m(\varphi \vee \psi) &=_{\mathit{def}}& m(\varphi) \wedge m(\psi) \\
   m(\varphi \Rightarrow \psi) &=_{\mathit{def}}& m(\lnot\varphi) \wedge m(\psi) \\
   m(\forall d{:}D.\,\varphi) &=_{\mathit{def}}& m(\varphi) \\
   m(\exists d{:}D.\,\varphi) &=_{\mathit{def}}& m(\varphi) \\
   m([\alpha]\varphi) &=_{\mathit{def}}& m(\varphi) \\
   m(\langle\alpha\rangle\varphi) &=_{\mathit{def}}& m(\varphi) \\
   m(\nabla) &=_{\mathit{def}}& \mathsf{true} \\
   m(\nabla(t)) &=_{\mathit{def}}& \mathsf{true} \\
   m(\Delta) &=_{\mathit{def}}& \mathsf{true} \\
   m(\Delta(t)) &=_{\mathit{def}}& \mathsf{true} \\
   m(X(e)) &=_{\mathit{def}}& \mathsf{true} \\
   m(\mu X(d{:}D{:=}e).\,\varphi) &=_{\mathit{def}}& m(\varphi) \\
   m(\nu X(d{:}D{:=}e).\,\varphi) &=_{\mathit{def}}& m(\varphi)
   \end{array}

Normalization
-------------

The normalization function :math:`h` eliminates implications from a state
formula :math:`\varphi` and pushes negations inwards to the level of data
expressions. A precondition of :math:`h` is that :math:`\varphi` is
monotonous. If this is not the case, during the computation a term
:math:`\lnot X(e)` will be encountered.

.. math::

   \begin{array}{lll}
   h(\lnot b) &=_{\mathit{def}}& \lnot b \\
   h(\lnot\lnot\varphi) &=_{\mathit{def}}& h(\varphi) \\
   h(\lnot(\varphi \wedge \psi)) &=_{\mathit{def}}& h(\lnot\varphi) \vee h(\lnot\psi) \\
   h(\lnot(\varphi \vee \psi)) &=_{\mathit{def}}& h(\lnot\varphi) \wedge h(\lnot\psi) \\
   h(\lnot(\varphi \Rightarrow \psi)) &=_{\mathit{def}}& h(\varphi) \wedge h(\lnot\psi) \\
   h(\lnot\forall d{:}D.\,\varphi) &=_{\mathit{def}}& \exists d{:}D.\,h(\lnot\varphi) \\
   h(\lnot\exists d{:}D.\,\varphi) &=_{\mathit{def}}& \forall d{:}D.\,h(\lnot\varphi) \\
   h(\lnot[\alpha]\varphi) &=_{\mathit{def}}& \langle\alpha\rangle h(\lnot\varphi) \\
   h(\lnot\langle\alpha\rangle\varphi) &=_{\mathit{def}}& [\alpha] h(\lnot\varphi) \\
   h(\lnot\nabla) &=_{\mathit{def}}& \Delta \\
   h(\lnot\nabla(t)) &=_{\mathit{def}}& \Delta(t) \\
   h(\lnot\Delta) &=_{\mathit{def}}& \nabla \\
   h(\lnot\Delta(t)) &=_{\mathit{def}}& \nabla(t) \\
   h(\lnot X(e)) &=_{\mathit{def}}& \text{undefined} \\
   h(\lnot\mu X(d{:}D{:=}e).\,\varphi) &=_{\mathit{def}}&
     \nu X(d{:}D{:=}e).\,h(\lnot\varphi[X{:=}\lnot X]) \\
   h(\lnot\nu X(d{:}D{:=}e).\,\varphi) &=_{\mathit{def}}&
     \mu X(d{:}D{:=}e).\,h(\lnot\varphi[X{:=}\lnot X]) \\[4pt]
   h(b) &=_{\mathit{def}}& b \\
   h(\varphi \wedge \psi) &=_{\mathit{def}}& h(\varphi) \wedge h(\psi) \\
   h(\varphi \vee \psi) &=_{\mathit{def}}& h(\varphi) \vee h(\psi) \\
   h(\varphi \Rightarrow \psi) &=_{\mathit{def}}& h(\lnot\varphi) \vee h(\psi) \\
   h(\mathsf{Q}\,d{:}D.\,\varphi) &=_{\mathit{def}}& \mathsf{Q}\,d{:}D.\,h(\varphi) \\
   h([\alpha]\varphi) &=_{\mathit{def}}& [\alpha] h(\varphi) \\
   h(\langle\alpha\rangle\varphi) &=_{\mathit{def}}& \langle\alpha\rangle h(\varphi) \\
   h(\nabla) &=_{\mathit{def}}& \nabla \\
   h(\nabla(t)) &=_{\mathit{def}}& \nabla(t) \\
   h(\Delta) &=_{\mathit{def}}& \Delta \\
   h(\Delta(t)) &=_{\mathit{def}}& \Delta(t) \\
   h(X(d)) &=_{\mathit{def}}& X(d) \\
   h(\sigma X(d{:}D{:=}e).\,\varphi) &=_{\mathit{def}}&
     \sigma X(d{:}D{:=}e).\,h(\varphi)
   \end{array}
