PBES Rewriters
==============

*Wieger Wesselink, Tim Willemse, Thomas Neele*

This document describes several rewriters on PBES expressions. We assume that a data
rewriter :math:`r` is given that rewrites data expressions.

Simplifying rewriter
--------------------

We define a simplifying PBES rewriter :math:`R` recursively as follows. We assume
that :math:`D` is a non-empty data type, and we denote the free variables
appearing in :math:`\varphi` as :math:`\mathsf{free}(\varphi)`. We assume that a data
rewriter :math:`r` is given that rewrites data terms.

.. math::

   \begin{array}{lcl}
   b & \rightarrow & r(b) \\
   \lnot \lnot \varphi & \rightarrow & \varphi \\
   \varphi \wedge \mathit{true} & \rightarrow & \varphi \\
   \mathit{true} \wedge \varphi & \rightarrow & \varphi \\
   \varphi \wedge \mathit{false} & \rightarrow & \mathit{false} \\
   \mathit{false} \wedge \varphi & \rightarrow & \mathit{false} \\
   \varphi \wedge \varphi & \rightarrow & \varphi \\
   \varphi \vee \mathit{true} & \rightarrow & \mathit{true} \\
   \mathit{true} \vee \varphi & \rightarrow & \mathit{true} \\
   \varphi \vee \mathit{false} & \rightarrow & \varphi \\
   \mathit{false} \vee \varphi & \rightarrow & \varphi \\
   \varphi \vee \varphi & \rightarrow & \varphi \\
   \varphi \Rightarrow \psi & \rightarrow & \lnot \varphi \vee \psi \\
   \forall_{d:D}.\varphi & \rightarrow & \varphi \text{ if } d \notin \mathsf{free}(\varphi) \\
   \forall_{d:D}.\lnot \varphi & \rightarrow & \lnot \exists_{d:D}.\varphi \\
   \forall_{d:D}.\varphi \wedge \psi & \rightarrow & \forall_{d:D}.\varphi \wedge \forall_{d:D}.\psi \\
   \forall_{d:D}.\varphi \vee \psi & \rightarrow & (\forall_{d:D}.\varphi) \vee \psi \text{ if } d \notin \mathsf{free}(\psi) \\
   \forall_{d:D}.\varphi \vee \psi & \rightarrow & \varphi \vee (\forall_{d:D}.\psi) \text{ if } d \notin \mathsf{free}(\varphi) \\
   \exists_{d:D}.\varphi & \rightarrow & \varphi \text{ if } d \notin \mathsf{free}(\varphi) \\
   \exists_{d:D}.\lnot \varphi & \rightarrow & \lnot \forall_{d:D}.\varphi \\
   \exists_{d:D}.\varphi \vee \psi & \rightarrow & \exists_{d:D}.\varphi \vee \exists_{d:D}.\psi \\
   \exists_{d:D}.\varphi \wedge \psi & \rightarrow & (\exists_{d:D}.\varphi) \wedge \psi \text{ if } d \notin \mathsf{free}(\psi) \\
   \exists_{d:D}.\varphi \wedge \psi & \rightarrow & \varphi \wedge (\exists_{d:D}.\psi) \text{ if } d \notin \mathsf{free}(\varphi) \\
   X(e) & \rightarrow & X(r(e))
   \end{array}

where :math:`\varphi` and :math:`\psi` are arbitrary PBES expressions, :math:`b` is a data
term of data sort :math:`\mathbb{B}`, :math:`\mathit{true}` and :math:`\mathit{false}` are elements of data sort
:math:`\mathbb{B}`, :math:`X` is a predicate variable, :math:`e` consists of zero or more data
sorts and :math:`d` is a data variable of sort :math:`D`.

**Simplify**

The PBES expression rewrite system *Simplify* [Luc Engelen, 2007]
consists of the following rules [#simplify-note]_:

.. math::

   \begin{array}{lcl}
   \mathit{false} \wedge x & \rightarrow & \mathit{false} \\
   x \wedge \mathit{false} & \rightarrow & \mathit{false} \\
   \mathit{true} \wedge x & \rightarrow & x \\
   x \wedge \mathit{true} & \rightarrow & x \\
   \lnot\,\mathit{true} & \rightarrow & \mathit{false} \\
   \lnot\,\mathit{false} & \rightarrow & \mathit{true} \\
   \mathit{ITE}(\mathit{true},x,y) & \rightarrow & x \\
   \mathit{ITE}(\mathit{false},x,y) & \rightarrow & y \\
   x = x & \rightarrow & \mathit{true} \\
   y = x & \rightarrow & x = y, \text{ provided } y \succ x
   \end{array}

.. [#simplify-note] Todo: reformulate this rewrite system.

PFNF rewriter
-------------

.. definition::

   A predicate formula is said to be in *Predicate Formula Normal Form* (PFNF)
   if it has the following form:

   .. math::

      \mathsf{Q}_{1} v_{1}{:}V_{1}.\cdots\, \mathsf{Q}_{n} v_{n}{:}V_{n}.\;
      h \wedge \bigwedge_{i \in I} \left( g_{i} \implies \bigvee_{j \in J_{i}} X^{j}(e^{j}) \right)

   where :math:`X^{j} \in \chi` (:math:`\chi` is a countable set of sorted predicate
   variables), :math:`\mathsf{Q}_{i} \in \{\forall, \exists\}`, :math:`I` is a (possibly
   empty) finite index set, each :math:`J_{i}` is a non-empty finite index set, and
   :math:`h` and every :math:`g_{i}` are simple formulae.

Note that here :math:`J_{i}` is used to index a set of occurrences of not
necessarily different variables. For instance,
:math:`(n > 0 \implies X(3) \vee X(5) \vee Y(6))` is a formula complying with the
definition of PFNF. So long as it does not lead to confusion, we stick to the
convention to drop the typing of the quantified variables :math:`v_{i}`. An
algorithm to compute a PFNF is:

.. math::

   \begin{array}{lll}
   p(c) &=_{\mathit{def}}& c \\
   p(X(d)) &=_{\mathit{def}}& X(d) \\
   p(\forall x{:}D.\,\varphi) &=_{\mathit{def}}& \forall x{:}D.\,p(\varphi) \\
   p(\exists x{:}D.\,\varphi) &=_{\mathit{def}}& \exists x{:}D.\,p(\varphi) \\[6pt]
   p(\varphi \wedge \psi) &=_{\mathit{def}}&
     \mathsf{Q}_{1}^{\varphi} \cdots \mathsf{Q}_{n^{\varphi}}^{\varphi}
     \mathsf{Q}_{1}^{\psi} \cdots \mathsf{Q}_{n^{\psi}}^{\psi}.\;\;
     (h^{\varphi} \wedge h^{\psi}) \\
   & & \quad\wedge \bigwedge_{i \in I^{\varphi} \cup I^{\psi}}
       \left( g_{i} \implies \bigvee_{j \in J_{i}} X^{j}(e^{j}) \right) \\[6pt]
   p(\varphi \vee \psi) &=_{\mathit{def}}&
     \mathsf{Q}_{1}^{\varphi} \cdots \mathsf{Q}_{n^{\varphi}}^{\varphi}
     \mathsf{Q}_{1}^{\psi} \cdots \mathsf{Q}_{n^{\psi}}^{\psi}.\;
     (h^{\varphi} \vee h^{\psi}) \\
   & & \quad\wedge \bigwedge_{i \in I^{\varphi}}
       \left( (\lnot h^{\psi} \wedge g_{i}) \implies \bigvee_{j \in J_{i}} X^{j}(e^{j}) \right) \\
   & & \quad\wedge \bigwedge_{i \in I^{\psi}}
       \left( (\lnot h^{\varphi} \wedge g_{i}) \implies \bigvee_{j \in J_{i}} X^{j}(e^{j}) \right) \\
   & & \quad\wedge \bigwedge_{i \in I^{\varphi},\, k \in I^{\psi}}
       \left( (g_{i} \wedge g_{k}) \implies
         \bigvee_{j \in J_{i},\, m \in J_{k}} X^{j}(e^{j}) \vee X^{m}(e^{m}) \right)
   \end{array}

where

.. math::

   \begin{array}{lll}
   p(\varphi) &=& \mathsf{Q}_{1}^{\varphi} \cdots \mathsf{Q}_{n^{\varphi}}^{\varphi}.\;
     h^{\varphi} \wedge \bigwedge_{i \in I^{\varphi}}
     \left( g_{i} \implies \bigvee_{j \in J_{i}} X^{j}(e^{j}) \right) \\[4pt]
   p(\psi) &=& \mathsf{Q}_{1}^{\psi} \cdots \mathsf{Q}_{n^{\psi}}^{\psi}.\;
     h^{\psi} \wedge \bigwedge_{i \in I^{\psi}}
     \left( g_{i} \implies \bigvee_{j \in J_{i}} X^{j}(e^{j}) \right)
   \end{array}

under the assumption that :math:`I^{\varphi}` and :math:`I^{\psi}` are disjoint, and
:math:`v_{i}^{\varphi} \neq v_{j}^{\psi}` for all :math:`i, j`.

OnePointRule quantifier elimination rewriter
--------------------------------------------

The function :math:`Eq` computes a set of equalities and inequalities for an
expression :math:`\varphi`, such that the following holds:

.. math::

   \begin{array}{lcl}
   Eq(\varphi) = \left(\left\{ b_{1}=e_{1},\ldots,b_{n}=e_{n} \right\},W\right)
   &\Rightarrow& \varphi \equiv \psi \wedge \bigwedge_{i=1}^{n}(b_{i}=e_{i}) \\[4pt]
   Eq(\varphi) = \left(V,\left\{ b_{1}\neq e_{1},\ldots,b_{n}\neq e_{n} \right\}\right)
   &\Rightarrow& \varphi \equiv \psi \vee \bigvee_{i=1}^{n}(b_{i}\neq e_{i})
   \end{array}

for some expression :math:`\psi`.

The function :math:`Eq` is inductively defined as follows:

.. math::

   \begin{array}{lll}
   Eq(\mathit{true}) &=& (\{\emptyset,\top\}) \\
   Eq(\mathit{false}) &=& (\{\top,\emptyset\}) \\
   Eq(b) &=& \left(\left\{ b=\mathit{true} \right\},\left\{ b \neq \mathit{false} \right\}\right) \\[4pt]
   Eq(d=e) &=& \left\{\begin{array}{ll}
     \left(\left\{ d=e \right\},\emptyset\right) & \text{if } d \notin FV(e) \\
     \left(\emptyset,\emptyset\right) & \text{otherwise}
   \end{array}\right. \\[4pt]
   Eq(e=d) &=& Eq(d=e) \\[4pt]
   Eq(d \neq e) &=& \left\{\begin{array}{ll}
     \left(\emptyset,\left\{ d \neq e \right\}\right) & \text{if } d \notin FV(e) \\
     \left(\emptyset,\emptyset\right) & \text{otherwise}
   \end{array}\right. \\[4pt]
   Eq(e \neq d) &=& Eq(d \neq e) \\[4pt]
   Eq(\lnot \varphi) &=& swap(Eq(\varphi)) \\
   Eq(\varphi \wedge \psi) &=& join\_and(Eq(\varphi),\,Eq(\psi)) \\
   Eq(\varphi \vee \psi) &=& join\_or(Eq(\varphi),\,Eq(\psi)) \\
   Eq(\varphi \Rightarrow \psi) &=& join\_or(swap(Eq(\varphi)),\,Eq(\psi)) \\[4pt]
   Eq(\mathit{if}(\varphi,\psi,\chi)) &=& \big((V_\varphi \cup V_\psi) \cap (W_\varphi \cup V_\chi),\;
     (V_\varphi \cup W_\psi) \cap (W_\varphi \cup W_\chi)\big) \\
   & & \quad\text{where }
     \begin{array}[t]{rl}
       (V_\varphi,W_\varphi) &:= Eq(\varphi)\\
       (V_\psi,W_\psi) &:= Eq(\psi)\\
       (V_\chi,W_\chi) &:= Eq(\chi)
     \end{array} \\[4pt]
   Eq(\forall x.\,\varphi) &=& delete(x,\,Eq(\varphi)) \\
   Eq(\exists x.\,\varphi) &=& delete(x,\,Eq(\varphi)) \\
   Eq(\varphi) &=& (\emptyset,\emptyset) \text{ otherwise}
   \end{array}

where :math:`b` is a boolean data variable, :math:`d` is an arbitrary data variable,
:math:`e` is a boolean data expression, :math:`\top` is the set of all equalities and
inequalities (so it is the idempotent element for :math:`\cap`) and

.. math::

   \begin{array}{lll}
   swap((V,W)) &=& (W,V) \\[4pt]
   join\_and((V_1,W_1),(V_2,W_2)) &=& (V_1 \cup V_2,\; W_1 \cap W_2) \\[4pt]
   join\_or((V_1,W_1),(V_2,W_2)) &=& (V_1 \cap V_2,\; W_1 \cup W_2) \\[4pt]
   delete(x,(V,W)) &=& (V_1,W_1) \text{ where} \\
   & & \left\{\begin{array}{l}
     V_1 = \left\{ d=e \in V \mid d \neq x \wedge x \notin FV(e) \right\} \\
     W_1 = \left\{ d \neq e \in W \mid d \neq x \wedge x \notin FV(e) \right\}
   \end{array}\right.
   \end{array}

We define the OnePointRule rewriter :math:`R` inductively as follows:

.. math::

   \begin{array}{lll}
   R(\lnot \varphi) &=& \lnot R(\varphi) \\
   R(\varphi \wedge \psi) &=& R(\varphi) \wedge R(\psi) \\
   R(\varphi \vee \psi) &=& R(\varphi) \vee R(\psi) \\
   R(\varphi \Rightarrow \psi) &=& R(\varphi) \Rightarrow R(\psi) \\[4pt]
   R(\forall x.\,\varphi) &=& \left\{\begin{array}{ll}
     R(\varphi)[x := e] & \text{if } x \neq e \in W,\text{ where } (V,W) = Eq(\varphi) \\
     \forall x.\,R(\varphi) & \text{otherwise}
   \end{array}\right. \\[4pt]
   R(\exists x.\,\varphi) &=& \left\{\begin{array}{ll}
     R(\varphi)[x := e] & \text{if } x = e \in V,\text{ where } (V,W) = Eq(\varphi) \\
     \exists x.\,R(\varphi) & \text{otherwise}
   \end{array}\right. \\[4pt]
   R(\varphi) &=& \varphi \text{ otherwise}
   \end{array}

Quantifier inside rewriter
--------------------------

This rewriter was originally specified by Jan Friso Groote. We define a
rewriter that pushes universal and existential quantifiers into an expression.
A typical example is

.. math::

   \exists x.\,\forall y.\,(f(y,y) \wedge (f(x,y) \vee f(x,x)))

which is rewritten to

.. math::

   (\forall y.\,f(y,y)) \wedge ((\exists x.\,\forall y.\,f(x,y)) \vee \exists x.\,f(x,x)).

If quantifiers are pushed inside formulas as much as possible, quantifier
elimination leads to smaller expressions and the one-point rewriter is more
often applicable. There might be cases where the application of this
push-quantifiers-inside rewriter can also have adverse effects.

Below the definition of the rewrite rules are given. It is assumed that it is
cheap to obtain the free variables in each term, which are denoted by
:math:`\mathit{vars}(\phi)` for a term :math:`\phi`. If not, care needs to be taken as in the
formulation below — calculating the variables of each term recursively on a
by-need basis can be very expensive.

Let :math:`\phi` be a PBES expression. We define :math:`R(\phi)` using the functions
:math:`R_{\forall}(V,\phi)` and :math:`R_{\exists}(V,\phi)` where :math:`V` is a set of typed
variables. The definition of :math:`R` employs the structure of a formula:

.. math::

   \begin{array}{lll}
   R(\neg \phi) &=& \neg R(\phi) \\
   R(\phi \wedge \psi) &=& R(\phi) \wedge R(\psi) \\
   R(\phi \vee \psi) &=& R(\phi) \vee R(\psi) \\
   R(\phi \Rightarrow \psi) &=& R(\phi) \Rightarrow R(\psi) \\
   R(\forall W.\,\phi) &=& R_{\forall}(W,\,R(\phi)) \\
   R(\exists W.\,\phi) &=& R_{\exists}(W,\,R(\phi)) \\
   R(\phi) &=& \phi \quad \text{otherwise}
   \end{array}

Here :math:`W` is a set of typed variables.

The function :math:`R_{\forall}` is defined as

.. math::

   \begin{array}{lll}
   R_{\forall}(V, \neg \phi) &=& \neg R_{\exists}(V, \phi) \\[4pt]
   R_{\forall}(V, \phi \wedge \psi) &=& R_{\forall}(V,\phi) \wedge R_{\forall}(V,\psi) \\[4pt]
   R_{\forall}\!\left(V,\, \bigvee_i \phi_i\right) &=&
     \left\{\begin{array}{ll}
       \forall\, V \cap \mathit{vars}\!\left(\bigvee_i \phi_i\right).\; \bigvee_i \phi_i
         & \text{if } \Psi = \mathit{false} \\[4pt]
       \forall\, V \cap \mathit{vars}(\Phi) \cap \mathit{vars}(\Psi). \\
       \quad \big(R_\forall(V \cap \mathit{vars}(\Phi) \setminus \mathit{vars}(\Psi),\,\Phi)
             \lor R_\forall(V \cap \mathit{vars}(\Psi) \setminus \mathit{vars}(\Phi),\,\Psi)\big)
         & \text{otherwise}
     \end{array}\right. \\[4pt]
   R_{\forall}(V, \phi \Rightarrow \psi) &=& R_{\forall}(V,\, \neg\phi \lor \psi) \\[4pt]
   R_{\forall}(V, \forall W.\,\phi) &=& R_{\forall}(V \cup W,\, \phi) \\[4pt]
   R_{\forall}(V, \phi) &=& \forall\, V \cap \mathit{vars}(\phi).\;\phi \quad \text{otherwise}
   \end{array}

where

.. math::

   \left\{\begin{array}{l}
     Z = \mathit{vars}(\phi_j) \cap V \text{ for some } j \text{ such that } |Z| \text{ is minimal} \\
     \Phi = \bigvee\!\left\{ \phi_i \mid (\mathit{vars}(\phi_i) \cap V) \subseteq Z \right\} \\
     \Psi = \bigvee\!\left\{ \phi_i \mid (\mathit{vars}(\phi_i) \cap V) \nsubseteq Z \right\}
   \end{array}\right.

The function :math:`R_{\exists}` is defined as

.. math::

   \begin{array}{lll}
   R_{\exists}(V, \neg \phi) &=& \neg R_{\forall}(V, \phi) \\[4pt]
   R_{\exists}\!\left(V,\, \bigwedge_i \phi_i\right) &=&
     \left\{\begin{array}{ll}
       \exists\, V \cap \mathit{vars}\!\left(\bigwedge_i \phi_i\right).\; \bigwedge_i \phi_i
         & \text{if } \Psi = \mathit{true} \\[4pt]
       \exists\, V \cap \mathit{vars}(\Phi) \cap \mathit{vars}(\Psi). \\
       \quad \big(R_\exists(V \cap \mathit{vars}(\Phi) \setminus \mathit{vars}(\Psi),\,\Phi)
             \land R_\exists(V \cap \mathit{vars}(\Psi) \setminus \mathit{vars}(\Phi),\,\Psi)\big)
         & \text{otherwise}
     \end{array}\right. \\[4pt]
   R_{\exists}(V, \phi \vee \psi) &=& R_{\exists}(V,\phi) \vee R_{\exists}(V,\psi) \\[4pt]
   R_{\exists}(V, \phi \Rightarrow \psi) &=& R_{\exists}(V,\, \neg\phi \lor \psi) \\[4pt]
   R_{\exists}(V, \exists W.\,\phi) &=& R_{\exists}(V \cup W,\, \phi) \\[4pt]
   R_{\exists}(V, \phi) &=& \exists\, V \cap \mathit{vars}(\phi).\;\phi \quad \text{otherwise}
   \end{array}

where

.. math::

   \left\{\begin{array}{l}
     Z = \mathit{vars}(\phi_j) \cap V \text{ for some } j \text{ such that } |Z| \text{ is minimal,} \\
     \Phi = \bigwedge\!\left\{ \phi_i \mid (\mathit{vars}(\phi_i) \cap V) \subseteq Z \right\} \\
     \Psi = \bigwedge\!\left\{ \phi_i \mid (\mathit{vars}(\phi_i) \cap V) \nsubseteq Z \right\}
   \end{array}\right.

Quantifier expansion rewriter
------------------------------

Conjunctions and disjunctions
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The function :math:`CD` computes a sequence of conjunctions and disjunctions for
an expression :math:`\varphi`, such that the following holds:

.. math::

   \begin{array}{lcl}
   CD(\varphi) = \left([\varphi_1,\ldots,\varphi_n],\, W\right)
     &\Rightarrow& \varphi = \bigwedge_{i=1}^{n} \varphi_i \\[4pt]
   CD(\varphi) = \left(V,\, [\varphi_1,\ldots,\varphi_n]\right)
     &\Rightarrow& \varphi = \bigvee_{i=1}^{n} \varphi_i
   \end{array}

The function :math:`CD` is inductively defined as follows:

.. math::

   \begin{array}{lll}
   CD(\lnot \varphi) &=& negate(CD(\varphi)) \\[4pt]
   CD(\varphi \wedge \psi) &=& \left(
     conjunctions(CD(\varphi)) {+\!+}\, conjunctions(CD(\psi)),\; \emptyset
   \right) \\[4pt]
   CD(\varphi \vee \psi) &=& \left(
     \emptyset,\; disjunctions(CD(\varphi)) {+\!+}\, disjunctions(CD(\psi))
   \right) \\[4pt]
   CD(\varphi \Rightarrow \psi) &=& \left(
     \emptyset,\; disjunctions(negate(CD(\varphi))) {+\!+}\, disjunctions(CD(\psi))
   \right) \\[4pt]
   CD(\varphi) &=& (\varphi,\, \varphi) \quad \text{otherwise}
   \end{array}

with

.. math::

   \begin{array}{lll}
   negate\big(([\varphi_1,\ldots,\varphi_m],\,[\psi_1,\ldots,\psi_n])\big)
     &=& \big([\lnot\psi_1,\ldots,\lnot\psi_n],\,[\lnot\varphi_1,\ldots,\lnot\varphi_m]\big) \\[4pt]
   conjunctions((V,W)) &=& \left\{\begin{array}{ll}
     V & \text{if } V \neq \emptyset \\
     \bigvee_{w \in W} w & \text{otherwise}
   \end{array}\right. \\[4pt]
   disjunctions((V,W)) &=& \left\{\begin{array}{ll}
     W & \text{if } W \neq \emptyset \\
     \bigwedge_{v \in V} v & \text{otherwise}
   \end{array}\right.
   \end{array}

.. note::

   This rewriter has not been implemented yet.
