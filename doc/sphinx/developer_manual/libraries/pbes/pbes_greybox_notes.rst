PBES Greybox Implementation Notes
===================================

*Gijs Kant*

Instantiation from PBES to Parity Game
-----------------------------------------

PBES
~~~~~

*Predicate formulae* :math:`\phi` are defined by the following grammar:

.. math::

   \phi \mathrel{::=} b \mid \mathsf{X}(\vec{e}) \mid \neg\phi \mid \phi \oplus \phi
       \mid \mathsf{Q}\,d {:} D \mathrel{.} \phi

where :math:`\oplus \in \{\wedge, \vee, \Rightarrow\}`, :math:`\mathsf{Q} \in \{\forall, \exists\}`,
:math:`b` is a data term of sort :math:`\mathsf{Bool}`, :math:`\mathsf{X} \in \mathcal{X}` is a
predicate variable, :math:`d` is a data variable of sort :math:`D`, and :math:`\vec{e}` is a vector
of data terms. Any predicate formula without predicate variables is called a *simple formula*. The
class of predicate formulae is denoted :math:`\mathcal{F}`.

.. definition::

   A *First-Order Boolean Equation* is an equation of the form:

   .. math::

      \sigma\,\mathsf{X}(d {:} D) = \phi

   where :math:`\sigma \in \{\mu, \nu\}` is a minimum (:math:`\mu`) or maximum (:math:`\nu`) fixed
   point operator, :math:`d` is a data variable of sort :math:`D`, and :math:`\phi` is a predicate
   formula.

.. definition::

   A *Parameterised Boolean Equation System (PBES)* is a sequence of First-Order Boolean Equations:

   .. math::

      \mathcal{E} = (\sigma_1\,\mathsf{X}_1(d_1 {:} D_1) = \phi_1)
                  \quad\ldots\quad
                  (\sigma_n\,\mathsf{X}_n(d_n {:} D_n) = \phi_n)

We adopt the standard limitations: expressions are in positive form (negation occurs only in data
expressions) and every variable occurs only once as the left hand side of an equation. A PBES that
contains no quantifiers and parameters is called a *Boolean Equation System* (BES). A PBES can be
*instantiated* to a BES by expanding the quantifiers to finite conjunctions or disjunctions and
substituting concrete values for the data parameters.

A one-to-one mapping can be made from a BES to an equivalent *parity game* if the BES has only
expressions that are either conjunctive or disjunctive. The parity game is then represented by a
game graph with nodes that represent propositional variables with concrete parameters and edges that
represent dependencies. To make instantiation of a PBES to a parity game more direct, we preprocess
the PBES to a format that only allows expressions to be either conjunctive or disjunctive. This
format is called the *Parameterised Parity Game*.

.. definition::

   A PBES is a *Parameterised Parity Game (PPG)* if every right hand side of an equation is a
   formula of the form:

   .. math::

      \mathsf{PPG} \mathrel{::=}
          \bigwedge_{i \in I} f_i \wedge \bigwedge_{j \in J} \forall_{\vec{v} \in D_j} \mathrel{.}
          \bigl(g_j \Rightarrow \mathsf{X}_j(e_j)\bigr)
      \;\bigg|\;
          \bigvee_{i \in I} f_i \vee \bigvee_{j \in J} \exists_{\vec{v} \in D_j} \mathrel{.}
          \bigl(g_j \wedge \mathsf{X}_j(e_j)\bigr)

   where :math:`f_i` and :math:`g_j` are simple boolean formulae and :math:`e_j` is a data
   expression. :math:`I` and :math:`J` are finite (possibly empty) index sets.

The expressions range over two index sets :math:`I` and :math:`J`. The left part is a conjunction
(or disjunction) of simple expressions :math:`f_i` that can be seen as conditions that must hold in
the current state. The right part is a conjunction (or disjunction) of quantifiers over a (possibly
empty) vector of variables for next states :math:`\mathsf{X}_j` with parameters :math:`e_j`,
guarded by simple expression :math:`g_j`.

Before transforming arbitrary PBESs to PPG we first define another normal form on PBESs to make
the transformation easier. This normal form allows an arbitrary sequence of bounded quantifiers as
outermost operators with a conjunctive normal form at the core, called the *Bounded Quantifier Normal
Form (BQNF)*. This is similar to *Predicate Formula Normal Form* (PFNF), implemented in the
``pbesrewr`` tool, in that quantification is outermost and the core is a conjunctive normal form.
Unlike PFNF, BQNF allows bounds on the quantified variables (hence *bounded* quantifiers), and
universal quantification is allowed within the conjunctive part while existential quantification is
allowed within the disjunctive parts.

.. definition::

   A First-Order Boolean formula is in *Bounded Quantifier Normal Form (BQNF)* if it has the form:

   .. math::

      \begin{array}{rcl}
      \mathsf{BQNF} & \mathrel{::=} &
          \forall{\vec{d} \in D} \mathrel{.} b \Rightarrow \mathsf{BQNF}
          \;\mid\;
          \exists{\vec{d} \in D} \mathrel{.} b \wedge \mathsf{BQNF}
          \;\mid\;
          \mathsf{CONJ} \\[6pt]
      \mathsf{CONJ} & \mathrel{::=} &
          \displaystyle\bigwedge_{k \in K} f_k \wedge
          \bigwedge_{i \in I} \forall_{\vec{v} \in D_I} \mathrel{.}
          \bigl(g_i \Rightarrow \mathsf{DISJ}^i\bigr) \\[6pt]
      \mathsf{DISJ}^i & \mathrel{::=} &
          \displaystyle\bigvee_{\ell \in L_i} f_{i\ell} \vee
          \bigvee_{j \in J_i} \exists_{\vec{w} \in D_{ij}} \mathrel{.}
          \bigl(g_{ij} \wedge \mathsf{X}_{ij}(e_{ij})\bigr)
      \end{array}

   where :math:`b`, :math:`f_k`, :math:`f_{i\ell}`, :math:`g_i`, and :math:`g_{ij}` are simple
   boolean formulae, and :math:`e_{ij}` is a data expression. :math:`K`, :math:`I`, :math:`L_i`,
   and :math:`J_i` are finite (possibly empty) index sets.


Translation from BQNF to Parameterised Parity Game
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In order to automatically transform a PBES to a PPG, we define a transformation function from BQNF
to PPG. For brevity, types of parameters are omitted. For equation system
:math:`\mathcal{E} = (\sigma\,\mathsf{X}_1(\vec{d}_1) = \xi_1) \quad \ldots \quad
(\sigma\,\mathsf{X}_n(\vec{d}_n) = \xi_n)` with each :math:`\xi_i` in BQNF, the translation to
PPG is defined as follows. Here :math:`I = \{1,\ldots,m\}`,
:math:`\vec{v} \cap \vec{d} = \varnothing` (variables in :math:`\vec{v}` do not occur in
:math:`\vec{d}`), :math:`b`, :math:`f`, :math:`f_k`, :math:`g_i` are simple formulae, and
:math:`\phi`, :math:`\phi_i` are formulae that may contain predicate variables.

For a full equation system:

.. math::

   s(\mathcal{E}) =_{\mathit{def}}
       s(\sigma\,\mathsf{X}_1(\vec{d}_1) = \xi_1) \quad \ldots \quad
       s(\sigma\,\mathsf{X}_n(\vec{d}_n) = \xi_n)

For a simple equation body :math:`f` (no quantifiers):

.. math::

   s(\sigma\,\mathsf{X}(\vec{d}) = f) =_{\mathit{def}} \sigma\,\mathsf{X}(\vec{d}) = f

For a universally quantified body:

.. math::

   s\bigl(\sigma\,\mathsf{X}(\vec{d}) = \forall\vec{v} \mathrel{.} b \Rightarrow \phi\bigr)
   =_{\mathit{def}}
   \begin{array}[t]{l}
   \Bigl(\sigma\,\mathsf{X}(\vec{d}) = \forall\vec{v} \mathrel{.} b \Rightarrow
       t(\widetilde{\mathsf{X}},\, \vec{d}+\vec{v},\, \phi)\Bigr) \\
   \quad t'(\sigma,\, \widetilde{\mathsf{X}},\, \vec{d}+\vec{v},\, \phi)
   \end{array}

For an existentially quantified body:

.. math::

   s\bigl(\sigma\,\mathsf{X}(\vec{d}) = \exists\vec{v} \mathrel{.} b \wedge \phi\bigr)
   =_{\mathit{def}}
   \begin{array}[t]{l}
   \Bigl(\sigma\,\mathsf{X}(\vec{d}) = \exists\vec{v} \mathrel{.} b \wedge
       t(\widetilde{\mathsf{X}},\, \vec{d}+\vec{v},\, \phi)\Bigr) \\
   \quad t'(\sigma,\, \widetilde{\mathsf{X}},\, \vec{d}+\vec{v},\, \phi)
   \end{array}

For a conjunctive CONJ body:

.. math::

   s\!\left(\sigma\,\mathsf{X}(\vec{d}) = \bigwedge_{k \in K} f_k \wedge
       \bigwedge_{i \in I} \bigl(\forall_{\vec{v}_i} \mathrel{.} g_i \Rightarrow \phi_i\bigr)\right)
   =_{\mathit{def}}
   \begin{array}[t]{l}
   \Bigl(\sigma\,\mathsf{X}(\vec{d}) = \bigwedge_{k \in K} f_k \wedge
       \bigwedge_{i \in I} \bigl(\forall_{\vec{v}_i} \mathrel{.} g_i \Rightarrow
           t(\widetilde{\mathsf{X}}_i,\, \vec{d}+\vec{v}_i,\, \phi_i)\bigr)\Bigr) \\
   \quad t'(\sigma,\, \widetilde{\mathsf{X}}_1,\, \vec{d}+\vec{v}_1,\, \phi_1)
       \quad \ldots \quad
       t'(\sigma,\, \widetilde{\mathsf{X}}_m,\, \vec{d}+\vec{v}_m,\, \phi_m)
   \end{array}

For a disjunctive CONJ body:

.. math::

   s\!\left(\sigma\,\mathsf{X}(\vec{d}) = \bigvee_{k \in K} f_k \vee
       \bigvee_{i \in I} \bigl(\exists_{\vec{v}_i} \mathrel{.} g_i \wedge \phi_i\bigr)\right)
   =_{\mathit{def}}
   \begin{array}[t]{l}
   \Bigl(\sigma\,\mathsf{X}(\vec{d}) = \bigvee_{k \in K} f_k \vee
       \bigvee_{i \in I} \bigl(\exists_{\vec{v}_i} \mathrel{.} g_i \wedge
           t(\widetilde{\mathsf{X}}_i,\, \vec{d}+\vec{v}_i,\, \phi_i)\bigr)\Bigr) \\
   \quad t'(\sigma,\, \widetilde{\mathsf{X}}_1,\, \vec{d}+\vec{v}_1,\, \phi_1)
       \quad \ldots \quad
       t'(\sigma,\, \widetilde{\mathsf{X}}_m,\, \vec{d}+\vec{v}_m,\, \phi_m)
   \end{array}

The helper functions :math:`t` and :math:`t'` are:

.. math::

   t(\mathsf{X},\, \vec{d},\, \phi) =_{\mathit{def}}
   \begin{cases}
   \phi & \text{if } \phi = \mathsf{X}'(e), \\
   \mathsf{X}(\vec{d}) & \text{otherwise;}
   \end{cases}
   \qquad
   t'(\sigma,\, \mathsf{X},\, \vec{d},\, \phi) =_{\mathit{def}}
   \begin{cases}
   \varnothing & \text{if } \phi = \mathsf{X}'(e), \\
   s(\sigma\,\mathsf{X}(\vec{d}) = \phi) & \text{otherwise.}
   \end{cases}


Move Quantifiers Inward
~~~~~~~~~~~~~~~~~~~~~~~~

Note the following equality:

.. math::

   \forall_{\vec{d} \in D} \mathrel{.} \bigwedge_{i \in I} \phi_i
   = \bigwedge_{i \in I} \Bigl(\forall_{\vec{d} \in D} \mathrel{.} \phi_i\Bigr)

Since the PPG form requires conjuncts of quantifiers rather than quantifiers over conjuncts, it is
useful to rewrite expressions so that conjunctions appear more on the outside and universal
quantifiers more to the inside. In the rewriting, not all parameters of the quantifier have to be
moved inward (see the example below). For this we introduce the *quantifier inward rewriter*
:math:`s_{QI}`:

.. math::

   \begin{array}{l@{\;=_{\mathit{def}}\;}l}
   s_{QI}(b) & b \\[4pt]
   s_{QI}(\mathsf{X}(e)) & \mathsf{X}(e) \\[4pt]
   s_{QI}\bigl(\exists_{\vec{d} \in D} \mathrel{.} \phi\bigr) &
       \exists_{\vec{d} \in D} \mathrel{.} s_{QI}(\phi) \\[4pt]
   s_{QI}\Bigl(\displaystyle\bigvee_{i \in I} \phi_i\Bigr) &
       \displaystyle\bigvee_{i \in I} s_{QI}(\phi_i) \\[4pt]
   s_{QI}\Bigl(\displaystyle\bigwedge_{i \in I} \phi_i\Bigr) &
       \displaystyle\bigwedge_{i \in I} s_{QI}(\phi_i) \\[4pt]
   s_{QI}\Bigl(\forall_{\vec{d} \in D} \mathrel{.} g \Rightarrow
       \displaystyle\bigwedge_{i \in I} \phi_i\Bigr) &
       \displaystyle\bigwedge_{i \in I} \Bigl(
           \forall_{\vec{d} \cap \mathrm{free}(\phi_i)} \mathrel{.}
           g_i \Rightarrow s_{QI}(\phi_i)\Bigr)
   \end{array}

where :math:`\phi` is an arbitrary expression in BQNF and :math:`b` is a data term of sort
:math:`\mathsf{Bool}`. The guarded condition :math:`g_i` is defined as:

.. math::

   g_i = \Bigl(\exists_{\vec{d} \cap (\mathrm{free}(g) \setminus \mathrm{free}(\phi_i))} \mathrel{.}
           \mathsf{filter}(g,\, \vec{d} \setminus \mathrm{free}(\phi_i))\Bigr)
         \wedge \mathsf{filter}(g,\, \vec{d} \cap \mathrm{free}(\phi_i))

The function :math:`\mathsf{filter}` is defined recursively as follows, where
:math:`\phi_i' = \mathsf{filter}(\phi_i, \vec{d})` and :math:`\oplus \in \{\wedge, \vee\}`:

.. math::

   \mathsf{filter}(b, \vec{d}) =_{\mathit{def}}
   \begin{cases}
   b & \text{if } \bigl(\mathrm{free}(b) \cap \vec{d}\bigr) = \varnothing, \\
   \varnothing & \text{otherwise.}
   \end{cases}

.. math::

   \mathsf{filter}(\phi_1 \oplus \phi_2, \vec{d}) =_{\mathit{def}}
   \begin{cases}
   \varnothing           & \text{if } \phi_1' = \varnothing \wedge \phi_2' = \varnothing, \\
   \phi_1'               & \text{if } \phi_1' \neq \varnothing \wedge \phi_2' = \varnothing, \\
   \phi_2'               & \text{if } \phi_1' = \varnothing \wedge \phi_2' \neq \varnothing, \\
   \phi_1' \oplus \phi_2' & \text{otherwise.}
   \end{cases}

.. admonition:: Example

   The formula ``forall x,y . (x < 5) => ((x==a) /\ (y==b))`` should translate to:

   .. code-block:: none

      (forall x . (x < 5) => (x==a)) /\ (forall y . (exists x . x < 5) => (y==b))


Partitioned State Vector, Transition Groups, and Dependency Matrix
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

We regard the instantiation of PBESs to Parity Games as generating a transition system, where
states are propositional variables with concrete parameters and transitions are dependencies,
specified by the right hand side of the corresponding equation in the PBES. We use the tool LTSmin
to generate a Parity Game given a PBES.


Partitioned State Vector
^^^^^^^^^^^^^^^^^^^^^^^^^

The state is represented as a vector :math:`\langle x_1, x_2, \ldots, x_m \rangle` for a fixed
:math:`m`. For PBES instantiation, the state vector is partitioned as:

.. math::

   \langle \mathsf{X},\, x_1,\, x_2,\, \ldots,\, x_k \rangle

where :math:`\mathsf{X}` is a propositional variable and for :math:`i \in \{1,\ldots,k\}` each
:math:`x_i` is the value of parameter :math:`i`. :math:`k` is the total number of parameter
signatures in the system, ordered alphabetically; the signature consists of the name and type of the
parameter. From the propositional variable :math:`\mathsf{X}`, the type
:math:`\in \{\wedge, \vee\}`, priority (an integer value), and fixpoint operator
:math:`\sigma \in \{\mu, \nu\}` can be derived.


Transition Groups
^^^^^^^^^^^^^^^^^^

The equations in the PBES specify the transitions between states. These transitions can be
partitioned by the part of the equation system they originate from — in this case, the parts of the
right hand sides of the equations.

For a PBES of the form:

.. math::

   \sigma\,\mathsf{X}(d {:} D) = \bigwedge_{i \in I}
       \forall_{\ell {:} D_i} \mathrel{.} g_i(d,\ell) \Rightarrow \mathsf{X}_i(h_i(d,\ell))

for each :math:`i \in I` there is a *transition group* :math:`\mathsf{X}_i` with an associated
transition relation :math:`\to_{\mathsf{X}_i}`, defined as:

.. math::

   \mathsf{X}_i(d {:} D) \to_{\mathsf{X}_i} \mathsf{X}_i(h_i(d,\ell))

for all :math:`\ell {:} D_i` such that :math:`g_i(d, \ell)`.

.. admonition:: Example

   A specification of two sequential buffers (``buffer.2``):

   .. math::

      \begin{array}{l@{\hspace{6pt}}l}
      \mathbf{eqn} & N = 2; \\[6pt]
      \mathbf{proc} & \mathsf{In}(i {:} \mathrm{Pos},\; q {:} \mathrm{List}(D)) = \\
        & \quad \displaystyle\sum_{d{:}D}\; (\#q < N) \rightarrow \mathrm{r}_1(d) \mathbin{.}
              \mathsf{In}(i,\; q \mathbin{\triangleleft} d) \\
        & \quad +\; (q \neq []) \rightarrow \mathrm{w}(i{+}1,\, \mathit{head}(q)) \mathbin{.}
              \mathsf{In}(i,\, \mathit{tail}(q)); \\[6pt]
      \mathbf{proc} & \mathsf{Out}(i {:} \mathrm{Pos},\; q {:} \mathrm{List}(D)) = \\
        & \quad \displaystyle\sum_{d{:}D}\; (\#q < N) \rightarrow \mathrm{r}(i,d) \mathbin{.}
              \mathsf{Out}(i,\; q \mathbin{\triangleleft} d) \\
        & \quad +\; (q \neq []) \rightarrow \mathrm{s}_4(\mathit{head}(q)) \mathbin{.}
              \mathsf{Out}(i,\, \mathit{tail}(q)); \\[6pt]
      \mathbf{init} & \mathbf{allow}(\{\mathrm{r}_1, \mathrm{c}, \mathrm{s}_4\},\;
              \mathbf{comm}(\{\mathrm{w} \mid \mathrm{r} \rightarrow \mathrm{c}\},\;
              \mathsf{In}(1,[]) \parallel \mathsf{Out}(2,[])));
      \end{array}

   with the property that if a message is read through :math:`\mathrm{r}_1`, it will eventually be
   sent through :math:`\mathrm{s}_4`:

   .. math::

      [\mathbf{true}^*]\Bigl(\forall d {:} D \mathrel{.}
      [\mathrm{r}_1(d)]\bigl(\nu\,\mathsf{X} \mathrel{.} \mu\,\mathsf{Y} \mathrel{.}
      \bigl([\mathrm{s}_4(d)]\,\mathsf{X} \wedge [\neg\mathrm{s}_4(d)]\,\mathsf{Y}\bigr)\bigr)\Bigr)

   The resulting PBES is (transition groups numbered (1)–(9)):

   .. math::

      \begin{aligned}
      \mathbf{pbes}\quad & \nu\,\mathsf{Z}(q_{\mathit{in}},\, q_{\mathit{out}} {:} \mathrm{List}(D)) = \\
      & \quad (\forall_{d{:}D} \mathrel{.} (\#q_{\mathit{in}} < 2) \Rightarrow
          \mathsf{X}(q_{\mathit{in}} \mathbin{\triangleleft} d,\; q_{\mathit{out}},\; d))
          \quad (1) \\
      & \quad \wedge\; (\forall_{d_0{:}D} \mathrel{.} (\#q_{\mathit{in}} < 2) \Rightarrow
          \mathsf{Z}(q_{\mathit{in}} \mathbin{\triangleleft} d_0,\; q_{\mathit{out}}))
          \quad (2) \\
      & \quad \wedge\; ((q_{\mathit{out}} \neq []) \Rightarrow
          \mathsf{Z}(q_{\mathit{in}},\; \mathit{tail}(q_{\mathit{out}})))
          \quad (3) \\
      & \quad \wedge\; ((q_{\mathit{in}} \neq [] \wedge \#q_{\mathit{out}} < 2) \Rightarrow
          \mathsf{Z}(\mathit{tail}(q_{\mathit{in}}),\;
          q_{\mathit{out}} \mathbin{\triangleleft} \mathit{head}(q_{\mathit{in}})))
          \quad (4) \\[6pt]
      & \nu\,\mathsf{X}(q_{\mathit{in}},\, q_{\mathit{out}} {:} \mathrm{List}(D),\; d{:}D) = \\
      & \quad \mathsf{Y}(q_{\mathit{in}},\; q_{\mathit{out}},\; d)
          \quad (5) \\[6pt]
      & \mu\,\mathsf{Y}(q_{\mathit{in}},\, q_{\mathit{out}} {:} \mathrm{List}(D),\; d{:}D) = \\
      & \quad ((\mathit{head}(q_{\mathit{out}}) \neq d) \vee (q_{\mathit{out}} = []) \vee
          \mathsf{X}(q_{\mathit{in}},\; \mathit{tail}(q_{\mathit{out}}),\; d))
          \quad (6) \\
      & \quad \wedge\; (\forall_{d_0{:}D} \mathrel{.} (\#q_{\mathit{in}} < 2) \Rightarrow
          \mathsf{Y}(q_{\mathit{in}} \mathbin{\triangleleft} d_0,\; q_{\mathit{out}},\; d))
          \quad (7) \\
      & \quad \wedge\; ((\mathit{head}(q_{\mathit{out}}) = d) \vee (q_{\mathit{out}} = []) \vee
          \mathsf{Y}(q_{\mathit{in}},\; \mathit{tail}(q_{\mathit{out}}),\; d))
          \quad (8) \\
      & \quad \wedge\; ((q_{\mathit{in}} \neq [] \wedge \#q_{\mathit{out}} < 2) \Rightarrow
          \mathsf{Y}(\mathit{tail}(q_{\mathit{in}}),\;
          q_{\mathit{out}} \mathbin{\triangleleft} \mathit{head}(q_{\mathit{in}}),\; d))
          \quad (9) \\[6pt]
      \mathbf{init} & \quad \mathsf{Z}([],[])
      \end{aligned}

   For this equation system, the structure of the state vector is
   :math:`\langle \mathsf{X},\; q_{\mathit{in}},\; q_{\mathit{out}},\; d \rangle`. The initial
   state is encoded as :math:`\langle \mathsf{Z},\; [],\; [],\; 0 \rangle`; since the initial
   state has no parameter :math:`d`, a default value is chosen. The numbers (1)–(9) denote the
   different transition groups: each conjunct of a conjunctive expression forms a group. For example:

   - :math:`\mathsf{var}(\text{(3)}) = \mathsf{Z}`
   - :math:`\mathsf{params}(\mathsf{var}(\text{(3)})) = \langle q_{\mathit{in}},\; q_{\mathit{out}} \rangle`
   - :math:`\mathsf{expr}(\text{(3)}) = (q_{\mathit{out}} \neq []) \Rightarrow \mathsf{Z}(q_{\mathit{in}},\; \mathit{tail}(q_{\mathit{out}}))`

   :math:`\text{Group-Next}(\mathsf{Z}([],[]),\; \text{(3)})` yields the empty set because
   :math:`q_{\mathit{out}} = []`. :math:`\text{Group-Next}(\mathsf{Z}([],[]),\; \text{(2)})` results
   in :math:`\{\mathsf{Z}([d_1],[]),\; \mathsf{Z}([d_2],[])\}`.


Dependency Matrix
^^^^^^^^^^^^^^^^^^

For an equation :math:`\sigma\,\mathsf{X}(d {:} D) = \phi`, the list of parameters is
:math:`\mathsf{params}(\mathsf{X}) =_{\mathit{def}} d {:} D`. Let :math:`\mathrm{free}(d)` be
the set of *free data variables* occurring in a data term :math:`d`. Let
:math:`\mathsf{used}(\phi)` be the set of free data variables occurring in an expression
:math:`\phi` such that the variables are not merely passed on to the next state. For example,
with :math:`\mathsf{X}(a, b) = \xi`, for the expression :math:`\phi = a \wedge \mathsf{X}(c, b)`,
:math:`\mathsf{used}(\phi) = \{a, c\}`. The variable :math:`b` is not in the set because it does
not influence the computation but is only passed on to the next state.

For a formula :math:`\phi`, the function :math:`\mathsf{changed}(\phi)` computes the variable
parameters changed in the formula:

.. math::

   \mathsf{changed}(\mathsf{X}(d_1,\ldots,d_m)) =_{\mathit{def}}
   \bigl\{p_i \mid i \in \{1,\ldots,m\} \wedge
       p_i = \mathsf{params}(\mathsf{X})_i \wedge d_i \neq p_i\bigr\}

The function :math:`\mathsf{tf}(\phi)` determines if :math:`\phi` contains a branch that directly
results in :math:`\mathbf{true}` or :math:`\mathbf{false}` (not a variable). For group :math:`g`
and state vector part :math:`i`, we define read dependence :math:`d_R` and write dependence
:math:`d_W`:

.. math::

   d_R(g, i) =_{\mathit{def}}
   \begin{cases}
   \mathbf{true} & \text{if } i = 1, \\
   p_i \in \bigl(\mathsf{params}(\mathsf{var}(g)) \cap
       \mathsf{used}(\mathsf{expr}(g))\bigr) & \text{otherwise.}
   \end{cases}

.. math::

   d_W(g, i) =_{\mathit{def}}
   \begin{cases}
   \bigl(\mathrm{occ}(\mathsf{expr}(g)) \setminus \{\mathsf{var}(g)\} \neq \varnothing\bigr)
       \vee \mathsf{tf}(\mathsf{expr}(g))
   & \text{if } i = 1, \\[4pt]
   p_i \in \mathsf{changed}(\mathsf{expr}(g), \varnothing)
   & \text{otherwise.}
   \end{cases}

.. definition::

   For a PPG :math:`P`, the *PPG dependency matrix* :math:`DM(P)` is a :math:`K \times M` matrix
   defined for :math:`1 \leq g \leq K` and :math:`1 \leq i \leq M` as:

   .. math::

      DM(P)_{g,i} =
      \begin{cases}
      + & \text{if } d_R(g,i) \wedge d_W(g,i), \\
      r & \text{if } d_R(g,i) \wedge \neg d_W(g,i), \\
      w & \text{if } \neg d_R(g,i) \wedge d_W(g,i), \\
      - & \text{otherwise.}
      \end{cases}

.. admonition:: Example

   For the PBES in the previous example, the dependency matrix is:

   +---+---+-------+--------+---+
   | g | X | q_in  | q_out  | d |
   +===+===+=======+========+===+
   | 1 | + | +     | -      | w |
   +---+---+-------+--------+---+
   | 2 | + | +     | -      | - |
   +---+---+-------+--------+---+
   | 3 | + | -     | +      | - |
   +---+---+-------+--------+---+
   | 4 | + | +     | +      | - |
   +---+---+-------+--------+---+
   | 5 | + | -     | -      | - |
   +---+---+-------+--------+---+
   | 6 | + | -     | +      | r |
   +---+---+-------+--------+---+
   | 7 | + | +     | -      | - |
   +---+---+-------+--------+---+
   | 8 | + | -     | +      | r |
   +---+---+-------+--------+---+
   | 9 | + | +     | +      | - |
   +---+---+-------+--------+---+

   ``+`` denotes both read and write dependency, ``w`` write dependency only, ``r`` read dependency
   only, and ``-`` no dependency between the group and the state vector part. The effect of caching
   is illustrated by transition group (5): it only moves states from :math:`\mathsf{X}` to
   :math:`\mathsf{Y}` without affecting parameters. Once such a transition has been computed (by
   Group-Next) it can be applied to any :math:`\mathsf{X}`-state simply by replacing
   :math:`\mathsf{X}` with :math:`\mathsf{Y}`.


Helpful Functions
^^^^^^^^^^^^^^^^^^

For an equation :math:`\sigma\,\mathsf{X}(d {:} D) = \phi`:

.. math::

   \mathsf{params}(\mathsf{X}) =_{\mathit{def}} d {:} D

Let :math:`\mathrm{free}(d)` be the set of *free data variables* occurring in a data term
:math:`d`. The function :math:`\mathsf{used}` is defined as:

.. math::

   \begin{array}{l@{\;=_{\mathit{def}}\;}l}
   \mathsf{used}(d) & \mathrm{free}(d) \\[4pt]
   \mathsf{used}(\mathsf{X}(e)) & \ldots\; \text{(parameters that are read, not only passed on)} \\[4pt]
   \mathsf{used}(\phi_1 \oplus \phi_2) & \mathsf{used}(\phi_1) \cup \mathsf{used}(\phi_2) \\[4pt]
   \mathsf{used}(\mathsf{Q}\,d {:} D \mathrel{.} \phi) & \mathsf{used}(\phi) \setminus \mathrm{free}(d)
   \end{array}

For a formula :math:`\phi`, the function :math:`\mathsf{changed}(\phi, \varnothing)` computes the
variable parameters changed in the formula:

.. math::

   \begin{array}{l@{\;=_{\mathit{def}}\;}l}
   \mathsf{changed}(b, L) & \varnothing \\[4pt]
   \mathsf{changed}(\neg\phi, L) & \mathsf{changed}(\phi, L) \\[4pt]
   \mathsf{changed}(\phi_1 \oplus \phi_2, L) &
       \mathsf{changed}(\phi_1, L) \cup \mathsf{changed}(\phi_2, L) \\[4pt]
   \mathsf{changed}(\mathsf{Q}\,d {:} D \mathrel{.} \phi, L) &
       \mathsf{changed}(\phi, L \cup \{d\}) \\[4pt]
   \mathsf{changed}(\mathsf{X}(d_1,\ldots,d_m), L) &
       \{p_i \mid i \in \{1,\ldots,m\} \wedge p_i = \mathsf{params}(\mathsf{X})_i
       \wedge (d_i \neq p_i \vee d_i \in L)\}
   \end{array}

For a formula :math:`\phi`, the function :math:`\mathsf{reset}(\phi, \vec{d})` computes the
variable parameters in :math:`\vec{d}` that are reset in the formula (meaning that in a successor
state those parameter values will not be used):

.. math::

   \begin{array}{l@{\;=_{\mathit{def}}\;}l}
   \mathsf{reset}(b, \vec{d}) & \varnothing \\[4pt]
   \mathsf{reset}(\neg\phi, \vec{d}) & \mathsf{reset}(\phi, \vec{d}) \\[4pt]
   \mathsf{reset}(\phi_1 \oplus \phi_2, \vec{d}) &
       \mathsf{reset}(\phi_1, \vec{d}) \cup \mathsf{reset}(\phi_2, \vec{d}) \\[4pt]
   \mathsf{reset}(\mathsf{Q}\,v {:} V \mathrel{.} \phi, \vec{d}) & \mathsf{reset}(\phi, \vec{d}) \\[4pt]
   \mathsf{reset}(\mathsf{X}(e), \vec{d}) & \vec{d} \setminus \mathsf{params}(\mathsf{X})
   \end{array}

For a formula :math:`\phi`, the function :math:`\mathsf{tf}(\phi)` determines if it contains a
branch that directly results in a boolean value (not a variable):

.. math::

   \begin{array}{l@{\;=_{\mathit{def}}\;}l}
   \mathsf{tf}(b) & \mathbf{true} \\[4pt]
   \mathsf{tf}(\neg\phi) & \mathsf{tf}(\phi) \\[4pt]
   \mathsf{tf}(\phi_1 \oplus \phi_2) & \mathsf{tf}(\phi_1) \vee \mathsf{tf}(\phi_2) \\[4pt]
   \mathsf{tf}(\mathsf{Q}\,d {:} D \mathrel{.} \phi) & \mathsf{tf}(\phi) \\[4pt]
   \mathsf{tf}(\mathsf{X}(e)) & \mathbf{false}
   \end{array}
