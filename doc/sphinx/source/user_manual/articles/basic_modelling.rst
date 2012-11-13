:Date: 13-02-2012
:Authors: - Sjoerd Cranen
          - Tim Willemse
:Institute: Eindhoven University of Technology

.. default-role:: math
.. highlight:: mcrl2
.. math::
   :nowrap: 
 
    \newcommand{\seq}{\cdot}
    \newcommand{\alt}{+}
    \newcommand{\nat}{\mathbb{N}}
    \newcommand{\bool}{\mathbb{B}}
    \newcommand{\true}{\mathbf{true}}
    \newcommand{\false}{\mathbf{false}}
    \renewcommand{\a}[1]{\textit{#1}}
    \renewcommand{\implies}{\mathop{\Rightarrow}}
    \newcommand{\act}{\mathit{Act}}
    \newcommand{\mccan}[1]{\langle #1 \rangle}
    \newcommand{\mcall}[1]{[ #1 ]}
    \newcommand{\sem}[1]{[\![ #1 ]\!]}
    \newcommand{\R}{\mathrel{R}}
    \newcommand{\bisim}{\stackrel{\leftrightarrow}{-}}
    \newcommand{\oftype}{\smash{\,:\,}}
    %

Basic modelling with mCRL2
==========================

Behaviour and transition systems
--------------------------------

In process algebras such as mCRL2, the basic units of computation are called
*actions*. The philosophy is that any system can be described in terms of
the observations that you can do about that system. The observations one can
make with respect to a system are dictated only by its behaviour. By extension,
every two systems that one might wish to distinguish must behave differently, or
one would not be able to distinguish them by observing them.

Actions correspond to observable events in the behaviour of a system. As a
running example, we will be introducing various coffee machines, the behaviour
of which will differ, but of which the functionality will usually be expressed
in terms of inserting coins into the machines, and retrieving coffee from it.
Obvious choices for actions that a coffee machine might perform are 'accepting a
coin', and 'dispensing coffee'.

As we already mentioned, coffee machines will usually both accept coins and
provide coffee. In fact, most coffee machines will only provide coffee
*after* accepting coins. Before we continue, we formalise this notion of
behaviour by saying that the behaviour of a system can be described by a
*labelled transition system*, or LTS for short. These are relational
structures that have a convenient graphical representation, and we will use them
to give a semantics to the mCRL2 processes we present.

.. admonition:: Definition (LTS)

   A *labelled transition system* (LTS) is a tuple `\langle S, \act,
   \rightarrow, i \rangle`, where

   - `S` is a set of states,
   - `\act` is a set of action labels,
   - `{\rightarrow} \subseteq S\times \act \times S` is a transition relation and
   - `i\in S` is the initial state.

In graphical depictions of LTSs, states are shown as circles. The initial state
is marked by an incoming arrow that has no source state. For `s,s'\in S` and
`a\in\act`, we will write `s \stackrel{a}{\longrightarrow} s'` instead of
`(s,a,s') \in \rightarrow`.


.. _example1:
.. admonition:: Example

   Consider a simple coffee machine that accepts a single coin and then
   dispenses coffee. This system could be modelled by the LTS with 
   `S=\{s_0, s_1, s_2\}`, `\act=\{\a{coin}, \a{coffee}\}`, `\rightarrow =
   \{(s_0, \a{coin}, s_1), (s_1, \a{coffee}, s_2)\}`, `i=s_0` and `f=s_2`. Its
   graphical representation is as follows:

   .. graphviz:: 

      digraph coffee1 {
        graph [rankdir=LR];
        node [shape=circle, label="", width=0.1];
        edge [arrowhead=vee,arrowsize=0.4];
        ns[style=invisible];
        ns -> n0;
        n0 -> n1 [label=coin,labelangle=90];
        n1 -> n2 [label=coffee,labelangle=90];
      }
   
   .. \begin{tikzpicture}[->,auto,node distance=1.5cm]
      \node[state,initial] (n0) {}; \node[state, right of=n0] (n1) {}; \node[state,final,right of=n1] (n2) {};
      \path (n0) edge node{`\a{coin}`} (n1) (n1) edge node{`\a{coffee}`} (n2);
      \end{tikzpicture}

Sequences and choices
---------------------

The coffee machine in :ref:`example1` is not very exciting. It performs
only two actions, and it performs them *sequentially*. More interesting
behaviour usually involves some form of *choice*. Choices in mCRL2 are
*nondeterministic*, that is to say, if a system can choose between two
actions to perform, then we don't know anything about the probability with which
it will choose either.

Let us consider a slightly more advanced coffee machine. It provides two kinds
of coffee. The first type costs only one coin, and is of the undrinkable,
asphalty kind. The event that the coffee machine dispenses this bad coffee is
modelled by the action `\a{bad}`. The machine may also dispense nice coffee,
modelled by the action `\a{good}`, but this will cost you an extra coin. We will
create a model of this machine in the mCRL2 language.

We start by giving an mCRL2 specification of our simple coffee machine, with on
the left the semantics of the specification.

.. list-table:: A simple coffee machine.

   * - .. tikz::
          :libs: automata

          [auto,->]
          \renewcommand{\a}[1]{\textit{#1}}
          \node[state,initial] (n0) {}; 
          \node[state, right of=n0] (n1) {}; 
          \node[state,final,right of=n1] (n2) {};
          \path (n0) edge node[above]{\a{coin}} (n1) (n1) edge node[above]{\a{coffee}} (n2);
     - ::

          act coin, coffee;
          init coin . coffee;

Note that the ``act`` statement explicitly defines the set `\act` of the LTS on
the left. The ``init`` statement says that the initial state `i` is a state that
can first perform a `\a{coin}` action, followed by a `\a{coffee}` action. This
sequential behaviour is expressed by the ``.`` operator.

We now wish to express that after inserting a single coin, we can either
retrieve bad coffee, or we can insert another coin and get good coffee. This can
be expressed as follows:


.. list-table:: Another coffee machine.

   * - .. tikz::
          :libs: automata

          [auto,->]
          \renewcommand{\a}[1]{\textit{#1}}
          \node[state,initial] (n0) {}; 
          \node[state, right of=n0] (n1) {}; 
          \node[state, right of=n1, below of=n1] (n2) {};
          \node[state,final,right of=n2,above of=n2] (n3) {};
          \path (n0) edge node{\a{coin}} (n1) (n1) edge node{\a{bad}} (n3)
                (n1) edge node[left]{\a{coin}} (n2) (n2) edge node[right]{\a{good}} (n3);
     - ::

          act coin, good, bad;
          init coin . (bad + coin . good);

Again the ``.`` operator is used to indicate sequential execution (after
inserting a coin, the machine can perform ``bad + coin . good``). Now is
a good time to note that ``.`` binds stronger than ``+``, so
``bad + coin . good`` is equal to ``bad + (coin . good)``. The
``+`` operator expresses the choice between either dispensing bad coffee
(``bad``), or accepting another coin and then dispensing good coffee
(``coin . good``). In the corresponding LTS, this choice is visualised
as a state that has two outgoing arrows.

.. admonition:: Exercise

   Give a specification for a machine that sells tea for 10¢ and coffee for 20¢.

Specifying systems
------------------

We mentioned in the introduction that we are interested in that part of the
behaviour of systems which we can observe. In the realm of modelling, we
therefore want to have a means of describing properties of systems such as 'this
coffee machine will always dispense good coffee after inserting two coins'. To
this end, we introduce Hennessy-Milner logic, an extension of Boolean logic that
introduces a modality `\mcall{a} \varphi` that expresses that if a system
performs an `\a{a}` action, then directly afterwards, the property `\varphi`
always holds. Note that in particular this is true if the system cannot do an
`\a{a}` action. Its dual is the modality `\mccan{a} \varphi`, which says that a
system is able to (as opposed to *must*) do an `\a{a}` action, after which
`\varphi` holds.

.. admonition:: Definition (Action formula)

   An action formula over a set of actions `\act` is an expression that
   adheres to the following syntax in pseudo-BNF, where `a\in \act`.

     `A,B ::= \false ~|~ a ~|~ \overline{A} ~|~ A \cup B`

The following abbreviations may also be used.

.. math::

   \true &= \overline{\false} \\
   A \cap B &= \overline{\overline{A} \cup \overline{B}}

An action formula `A` over `\act` is associated with a set `\sem{A} \subseteq
\act` in the following manner. 

.. math::

   \sem{\false} &= \emptyset \\
   \sem{a} &= \{ a \} \\
   \sem{\overline{A}} &= \act \setminus \sem{A} \\
   \sem{A \cup B} &= \sem{A} \cup \sem{B}

.. admonition:: Example

   Let `\act=\{a, b, c\}`. Then the formula `\overline{a}\cap\overline{b}`
   corresponds to `\{ c \}`.

.. admonition:: Definition (HML)

   A *Hennessy-Milner logic* formula interpreted over an LTS with action labels
   `\act` adheres to the following syntax in pseudo-BNF, where `A` is an action
   formula over `\act`.

   .. math::

      \varphi,\chi ::= \mccan{A}\varphi ~|~ \varphi \land \chi ~|~ \neg \varphi ~|~ \true \\


The following common abbreviations are allowed:

.. math::
   :nowrap:

   \begin{align*}
   \false &= \neg \true & \varphi \lor \chi &= \neg(\neg \varphi \land \neg \chi) \\
   \mcall{A}\varphi &= \neg \mccan{A} \neg \varphi & \varphi \implies \chi &= \neg \varphi \lor \chi
   \end{align*}

An HML formula `\varphi` is interpreted over an LTS `T = \langle S, \act,
\rightarrow, i \rangle`. Its semantics is given as the set of states
`\sem{\varphi}_T \subseteq S` of the LTS in which the formula holds. It is
defined as follows.

.. math::
   :nowrap:

    \begin{align*}
    \sem{\true}_T &= S \\
    \sem{\neg\varphi}_T &= S \setminus \sem{\varphi}_T \\
    \sem{\varphi \land \chi}_T &= \sem{\varphi}_T \cap \sem{\chi}_T \\
    \sem{\mccan{A}\varphi}_T &= \{ s \in S ~|~ \exists_{s'\in S, a \in \sem{A}}~ s \stackrel{a}{\longrightarrow} s' \land s' \in \sem{\varphi}_T \}
    \end{align*}

We say that `T` *satisfies* `\varphi`, denoted `T \models \varphi`, if and only
if `i \in \sem{\varphi}_T`.

.. admonition:: Example

   The formula `\mccan{\a{coin}}\mccan{\a{good}}\true` can be used to express
   that there is a possibility that a system dispenses good coffee after
   accepting one coin. This formula does not hold for the machine in Figure
   \ref{lst:coffee2}, because when you insert one coin, the machine will only
   provide you with bad coffee. 

.. admonition:: Example

   The formula `\mcall{\a{coin}}\mccan{\a{bad} \cup \a{coin}}\true` does hold
   for the machine in Figure \ref{lst:coffee2}; it says that always after
   accepting a coin, the machine might (is able to) dispense bad coffee or
   accept another coin.

.. admonition:: Exercises

   #. Show that an arbitrary LTS can never satisfy `\mccan{a}\false`, and that
      it will always satisfy `\mcall{a}\true`.
   #. Describe in English what the formula `\mcall{a}\false` means.
   #. Give HML formulae expressing the following properties:

      a) The coffee machine can dispense good coffee after two coins have been inserted.
      b) The coffee machine will not dispense bad coffee after two coins have been inserted.

Comparing systems
-----------------

Given two system models, one might wonder whether they are in some sense
interchangeable. This calls for a natural notion of *behavioral equivalence*
that relates systems that cannot be distinguished by observing their behaviour.
As we shall see, such an equivalence can be quite straightforward. Not quite
coincidentally, HML was originally designed as an alternative way to distinguish
systems. In fact, it was shown that two systems are related by the equivalence
described below if and only if there is no HML formula that is true for one and
false for the other.

.. admonition:: Definition (Strong bisimulation)

   Let `\langle S_1, \act, \rightarrow_1, i_1 \rangle` and `\langle S_2, \act, \rightarrow_2, i_2 \rangle` be labelled transition systems. A relation `{\R} \subseteq S_1\times S_2` is a strong bisimulation relation if and only if for `(s, s') \in {\R}` (also written `s \R s'`) we have for all `a \in \act`:

   - if `s \stackrel{a}{\longrightarrow}_1 t`, then there is a `t'\in S_2` such that `s' \stackrel{a}{\longrightarrow}_2 t'` and `t \R t'`.
   - if `s' \stackrel{a}{\longrightarrow}_2 t'`, then there is a `t\in S_1` such that `s \stackrel{a}{\longrightarrow}_1 t` and `t \R t'`.

Two states `s, s'` are said to be *strongly bisimilar*, denoted `s \bisim
s'`, if there is a strong bisimulation relation `\R` such that `s \R s'`. Two
LTSs are strongly bisimilar iff their initial states are bisimilar.

.. admonition:: Example
   :class: collapse

   In the following diagram, the dotted lines indicate the pairs of nodes that
   are related by a relation `R`.

   .. tikz::
      :libs: automata

        [auto,->]
        \renewcommand{\a}[1]{\textit{#1}}
        \begin{scope}
        \node[state,initial] (l1) {};
        \node[state,below of=l1] (l2) {};
        \node[state,below of=l2] (l3) {};
        \path[->] (l1) edge node[left]{\a{coin}} (l2)
                  (l2) edge node[left]{\a{coffee}} (l3);
        \end{scope}
        \begin{scope}[xshift=3cm]
        \node[state,initial] (r1) {};
        \node[state,below of=r1,left of=r1] (r2) {};
        \node[state,below of=r2,right of=r1] (r3) {};
        \node[state,below of=r2,left of=r2,xshift=0.5cm] (r4) {};
        \node[state,right of=r4] (r5) {};
        \node[state,right of=r5] (r6) {};
        \node[state,right of=r6] (r7) {};
        \path[->] (r1) edge node[left] {\a{coin}} (r2) 
                       edge node[right]{\a{coin}} (r3)
                  (r2) edge node[above,rotate=60]{\a{coffee}} (r4) 
                       edge node[above,rotate=-60]{\a{coffee}} (r5)
                  (r3) edge node[above,rotate=60]{\a{coffee}} (r6) 
                       edge node[above,rotate=-60]{\a{coffee}} (r7);
        \end{scope}
        \path[dotted,bend right]
          (l1) edge (r1)
          (l2) edge (r2) edge (r3)
          (l3) edge (r4) edge (r5) edge (r6) edge (r7);

   `R` is a bisimulation relation that relates the initial states of the two
   transition systems, hence they are bisimilar.

Note that the definition also allows you to compare states within a single
transition system (*i.e.*, `\langle S_1, \act, \rightarrow_1, i_1 \rangle =
\langle S_2, \act, \rightarrow_2, i_2 \rangle`). If two states are found to be
bisimilar, then for all intents and purposes it is reasonable to see them as
only one state, thus giving rise to a natural *statespace reduction*: if in
an LTS `T` we merge all states that are bisimilar, the resulting LTS `T'` is
bisimilar to `T`.

.. admonition:: Example
   :class: collapse

   In the following diagram, the dotted lines indicate the pairs of states that
   are related by a relation `R`.

   .. tikz::
      :libs: automata

      [auto,->]
      \renewcommand{\a}[1]{\textit{#1}}
      \node[state,initial] (r1) {};
      \node[state,below of=r1,left of=r1] (r2) {};
      \node[state,below of=r2,right of=r1] (r3) {};
      \node[state,below of=r2,left of=r2,xshift=0.5cm] (r4) {};
      \node[state,right of=r4] (r5) {};
      \node[state,right of=r5] (r6) {};
      \node[state,right of=r6] (r7) {};
      \path[->] (r1) edge node[left] {`\a{coin}`} (r2) 
                     edge node[right]{`\a{coin}`} (r3)
                (r2) edge node[above,rotate=60]{`\a{coffee}`} (r4) 
                     edge node[above,rotate=-60]{`\a{coffee}`} (r5)
                (r3) edge node[above,rotate=60]{`\a{coffee}`} (r6) 
                     edge node[above,rotate=-60]{`\a{coffee}`} (r7);

      \path[dotted,bend right]
        (r2) edge (r3)
        (r4) edge (r5) edge (r6) edge (r7) 
        (r5) edge (r6) edge (r7)
        (r6) edge (r7);
 
   `R` is a bisimulation relation, so merging all related states will yield a
   smaller, bisimilar transition system (namely the left transition system of
   Example \ref{ex:bisim}).

.. admonition:: Exercise

   Are the following two process definitions bisimilar?
   ::
        
      act coin, good, bad;
      init coin . (bad + coin . good);

      act coin, good, bad;
      init coin . bad + coin . coin . good;
  

A dash of infinity
==================

Recursion
---------

So far, our models of coffee machines only modelled a single transaction--after
dispensing a single cup of coffee, the machine terminated. In many situations in
real life, however, we wish to model systems that repeatedly perform the same
procedures.

The recipe for this is simple. We give a process a name, say `P`, and then say 
that `P` may exhibit some finite behaviour, after which it will once more behave
like `P`. Let us create an optimistic model of a coffee machine, that lets us
operate the machine forever.

.. list-table:: An ever-lasting coffee machine.

   * - .. tikz::
           :libs: automata

           [auto,->]
           \renewcommand{\a}[1]{\textit{#1}}
           \node[state,initial] (n0) {P}; 
           \node[state, right of=n0] (n1) {}; 
           \node[state, right of=n1] (n2) {};
           \path (n0) edge[bend left] node[above]{\a{coin}} (n1) (n1) edge[bend left] node[below]{\a{bad}} (n0)
                 (n1) edge node{\a{coin}} (n2) (n2) edge[bend left=90] node{\a{good}} (n0);

     - ::

          act coin, good, bad;
          proc P = coin . 
                     (bad . P +
                      coin . good . P);
          init P;

In the specification, we see that the ``proc`` operator accepts equations that
associate processes with process names. In this case, the process ``P`` is
defined. By using it in the right-hand side of its own definition, we introduced
infinitely repeating behaviour. To illustrate this, we could `unfold' this
repetition once and obtain a bisimilar system, as shown in figure
\ref{lst:coffee-inf2}.

.. list-table:: The ever-lasting coffee machine, unfolded once.

   * - .. tikz::
           :libs: automata

           [auto,->]
           \renewcommand{\a}[1]{\textit{#1}}
           \node[state,initial,initial where=right] (n3) {};
           \node[state, left of=n3] (n4) {};
           \node[state, left of=n4] (n5) {};
           \node[state, below of=n5,yshift=-0.5cm] (n0) {P}; 
           \node[state, right of=n0] (n1) {}; 
           \node[state, right of=n1] (n2) {};
           \path (n3) edge node[above]{\a{coin}} (n4) (n4) edge node[above]{\a{coin}} (n5)
                 (n4) edge[bend right] node[right]{\a{bad}} (n0) (n5) edge node[left]{\a{good}} (n0)
                 (n0) edge[bend left] node[above]{\a{coin}} (n1) (n1) edge[bend left] node[below]{\a{bad}} (n0)
                 (n1) edge node{\a{coin}} (n2) (n2) edge[bend left=90] node{\a{good}} (n0);
     - ::

          act coin, good, bad;
          proc P = coin . 
                     (bad . P +
                      coin . good . P);
          init coin .
                 (bad . P +
                  coin . good . P);

Note that the definition of bisimilarity does not have to be changed to deal
with recursive systems; the co-inductive definition guarantees that the future
behaviour stays the same.

.. admonition:: Exercise
  
   Show that the statespaces from figures \ref{lst:coffee-inf1} and
   \ref{lst:coffee-inf2} are bisimilar.

Regular HML
-----------

We saw that recursion does not require the definition of bisimilarity to be
changed. Similarly, HML is still adequate to distinguish recursive systems: if
two finite state systems are not bisimilar, then there is a finite HML
expression that distinguishes the two. However, when we are dealing with
infinitary systems, we often want to express properties of a system that say
that it will always keep doing something, or that it will eventually do
something. Such properties cannot be expressed by HML expressions of finite
length.

To remedy this shortcoming, HML can be extended to allow regular expressions
over action formulas inside the `\mccan{\cdot}` and `\mcall{\cdot}` modalities.
In particular, the Kleene star is a powerful operator that effectively
abbreviates certain HML formulas of infinite size.

.. admonition:: Definition (Regular HML)

   A *regular HML* formula interpreted over an LTS with action labels `\act`
   adheres to the following syntax in pseudo-BNF, where `A` is an action formula
   over `\act`.

   .. math::
    
      \varphi,\chi &::= \mccan{\alpha}\varphi ~|~ \varphi \land \chi ~|~ \neg \varphi ~|~ \true \\
      \alpha,\beta &::= A ~|~ \alpha ^* ~|~ \alpha \cdot \beta ~|~ \alpha + \beta

The usual abbreviations are allowed, as well as writing `\alpha^+` for
`\alpha\cdot\alpha^*`. We will informally give the semantics by rewriting
regular HML formulas to infinitary HML formulas:

.. math::

   \mccan{\alpha \cdot \beta}\varphi &= \mccan{\alpha}\mccan{\beta}\varphi & 
   \mccan{\alpha + \beta}\varphi &= (\mccan{\alpha}\varphi) \lor (\mccan{\beta}\varphi) \\
   \mccan{\alpha^*}\varphi &= \varphi \lor (\mccan{\alpha}\varphi) \lor (\mccan{\alpha\cdot\alpha}\varphi) \lor \ldots

.. admonition:: Exercise

   Rewrite the regular HML formula `[a+b]\false` to a pure HML formula.

The modal µ-calculus
--------------------

Although regular HML is a powerful tool to specify properties over infinite
systems, it is still not expressive enough to formulate so-called 
fairness properties*. These are properties that say things like `if the system is offered
the possibility to perform an action infinitely often, then it will eventually
perform this action'.

Another way of extending HML to deal with infinite behaviour is to add a
*least fixpoint operator*. This extension is called the *modal µ-calculus*, named
after the least fixpoint operator `\mu`. The µ-calculus (we often leave out the
`modal', as no confusion can arise) is famous for its expressivity, and infamous
for its unintelligability. We will therefore first give the definition and the
formal semantics, and then elaborate more informally on its use.

.. admonition:: Definition (µ-calculus)

   A *µ-calculus* formula interpreted over an LTS with action labels `\act` 
   adheres to the following syntax in pseudo-BNF, where `A` is an action formula
   over `\act`, and `X` is chosen from some set of variable names `\mathcal{X}`.

   .. math::
      
      \varphi,\chi &::= \true ~|~ X ~|~ \mu X\,.\, \varphi ~|~ \mccan{A}\varphi 
      ~|~ \varphi \land \chi ~|~ \neg \varphi

We allow the same abbreviations as for HML, and we add the *greatest fixpoint
operator*, which is the dual of the least fixpoint operator:

.. math::
   :nowrap:

    \begin{align*}
    \nu X \,.\, \varphi &= \neg \mu X\,.\, \neg\varphi[\neg X / X]
    \end{align*}

In the above, we use `\varphi[\neg X/X]` to denote `\varphi` with all
occurrences of `X` replaced by `\neg X`.

For technical reasons, we impose an important restriction on the syntax of 
µ-calculus formulas: only formulas in which every use of a fixpoint variable 
from `\mathcal{X}` is preceded by an even number of negations are allowed. The
formula is then in *positive normal form*, allowing us to give it a proper
semantics [#f1]_.

A µ-calculus formula `\varphi` is interpreted over an LTS 
`T = \langle S, \act, \rightarrow, i, f \rangle`. To accomodate the fixpoint
variables, we also need a *predicate environment} `\rho: \mathcal{X} \to 2^S`,
which maps variable names to their semantics (*i.e.*, sets of states from
`T`). We use `\rho[X\mapsto V]` to denote the environment that maps `X` to the
set `V`, and that maps all other variable names in the same way `\rho` does.

The semantics of a formula is now given as the set of states
`\sem{\varphi}_T^\rho \subseteq S`, defined as follows.

.. math::

    \begin{align*}
    \sem{\true}_T^\rho &= S \\
    \sem{X}_T^\rho &= \rho(X) \\
    \sem{\neg\varphi}_T^\rho &= S \setminus \sem{\varphi}_T^\rho \\
    \sem{\varphi \land \chi}_T^\rho &= \sem{\varphi}_T^\rho \cap \sem{\chi}_T^\rho \\
    \sem{\mccan{a}\varphi}_T^\rho &= \{ s \in S ~|~ \exists_{s'\in S}~ s \stackrel{a}{\longrightarrow} s' \land s' \in \sem{\varphi}_T^\rho \} \\
    \sem{\mu X\,.\,\varphi}_T^\rho &= \bigcap \{V \subseteq S ~|~ \sem{\varphi}_T^{\rho[X \mapsto V]} \subseteq V \}
    \end{align*}

We say that `T` *satisfies* `\varphi`, denoted `T \models \varphi`, if and only
if `i \in \sem{\varphi}_T^\rho` for any `\rho`.

Using the µ-calculus
""""""""""""""""""""

To understand how the µ-calculus can be used to express properties of systems, it
is instructive to see that regular HML can be encoded into the modal µ-calculus by
using the following equalities.

.. math::
   :nowrap:

    \begin{align*}
    \mccan{\alpha^*}\varphi &= \mu X\,.\, \varphi \lor \mccan{\alpha}X &
    \mcall{\alpha^*}\varphi &= \nu X\,.\, \varphi \land \mcall{\alpha}X 
    \end{align*}

Intuitively, the least fixpoint operator `\mu` corresponds to an eventuality,
where the greatest fixpoint operator says something about properties that
continue to hold forever.

We can read `\mu X\,.\, \varphi \lor \mccan{\alpha}X` as `X` is the smallest
set of states such that a state is in `X` if and only if `\varphi` holds in that
state, or there is an `\alpha`-successor that is in `X`'. Conversely, `\nu
X\,.\, \varphi \land \mcall{\alpha}X` is the largest set of states such that a
state is in `X` if and only if `\varphi` holds in that state and all of its
`\alpha`-successors are in `X`.


A good way to learn how the µ-calculus works is by understanding how the 
semantics of a formula can be computed. To do so, we use *approximations*. For
each fixpoint we encounter, we start with an initial approximation, and then
keep refining the approximation until the last two refinements are the same. The
current approximation is then a *fixpoint* of the formula, which is what we
were after. The first approximation `\hat{X}^0` for a fixpoint `\mu X \,.\,
\varphi` is given by `\varphi[\false / X]`. For a greatest fixpoint `\nu X \,.\,
\varphi`, it is given by `\varphi[\true / X]`. In other words, for a least
fixpoint operator the initial approximation represents the empty set of states,
and for a greatest fixpoint operator we initially assume the formula holds for
all states. Each next approximation `\hat{X}^{i+1}` is given by
`\varphi[\hat{X}^i / X]`. If `\hat{X}^{i+1} = \hat{X}^i`, then we have reached
our fixpoint.

.. admonition:: Example

   Consider the following formula, which states that a coffee machine will
   always give coffee after a finite number of steps.

   .. math:: \mu X\,.\, \mccan{\true}\true \land \mcall{\overline{`\a{coffee}`}} X

   Note that this formula cannot be expressed using regular expressions. To see
   how the formula works, consider `\hat{X}^0 =\mccan{\true}\true \land
   \mcall{\overline{\a{coffee}}}\false`. The first conjunct of this first
   approximation says that an action can be performed, and the second conjunct
   says that any action that can be performed must be a `\a{coffee}` action. The
   first approximation hence represents the set of states that can--and can
   only--do `\a{coffee}` actions. 

   The next approximation is `\hat{X}^1 = \mccan{\true}\true \land
   \mcall{\overline{\a{coffee}}} \hat{X}^0`. The first conjunct again selects
   all states that may perform an action, and the second conjunct now selects
   those that can additionally do only `\a{coffee}` actions, *or* that can do
   another action and then always end up in the set of states where `\hat{X}^0`
   holds. Continuing this reasoning, it is easy to see that `\hat{X}^i`
   represents the set of states that must reach a state that must do a
   `\a{coffee}` action in `i` or less steps. Hence, when we find a fixpoint, this
   fixpoint represents those states that must eventually reach a state from
   which a `\a{coffee}` action must be performed.

More complicated properties can be expressed by nesting fixpoint operators.

.. admonition:: Exercise

   What does the formula `\nu X \,.\, \varphi \land \mccan{a}X` express? Can it
   be expressed in regular HML?

Data
----

\label{sec:data}

Recursion is one way to introduce infinity in system models. It neatly
enables us to model systems that continuously interact with their
environment. The infinity obtained by recursion is an infinity in the
*depth* of the system. There is another form of infinity that we have
not yet explored: infinity in the width of the system. This type of
infinity can be obtained by combining processes and data.

We first illustrate the idea of combining processes and data with a simple
example. Let us reconsider the ``coin`` action of the coffee machine.
Rather than assuming that there is only one flavour
of coins, there are in fact various types of coins: 2, 5 and 10 cents;
these values can be thought of as elements of the structured
sort ``Val``, defined as::

  sort Val = struct c2 | c5 | c10;

The action ``coin`` can be thought of as inserting
a particular type of coin, the value of which is dictated by a parameter
of the action. Thus, ``coin(c2)`` represents the insertion of a
2 cent coin, whereas ``coin(c10)`` represents the insertion of a
10 cents coin. Below, we have a state that accepts all possible
coins, with on the right the required mCRL2 notation.

.. list-table:: A state accepting coins of all flavour.

   * - .. tikz::
          :libs: automata

          [auto,->]
          \renewcommand{\a}[1]{\textit{#1}}
          \node[state,initial] (n)  {};
          \node[state] (e) [below of=n, node distance=2cm] {};
          \path
             (n) edge[bend left=60] node[above,rotate=-90]{\scriptsize{\a{coin(c2)}}} (e)
                 edge node[above,rotate=-90]{\scriptsize{\a{coin(c5)}}} (e)
                 edge[bend right=60] node[below,rotate=-90]{\scriptsize{\a{coin(c10)}}} (e);
     - ::

          sort Val = struct c2 | c5 | c10;
          act coin: Val;
          init sum v: Val . coin(v);

The statement ``sum v: Val . coin(v)`` actually binds a local variable
``v`` of sort ``Val``, and, for every of its possible values,
specifies a ``coin`` action with that value as a parameter.
An alternative description of the same process is
::

   init coin(c2) + coin(c5) + coin(c10);

This suggests that the summation is like the plus.

As soon as the sort that is used in combination with the ``sum``
operator has infinitely many basic elements, the branching degree of
a state may become infinite, as illustrated by Figure~\ref{fig:nats}.
Since each mCRL2 expression is finite, we can no longer give an
equivalent expression using only the plus operator.

.. list-table:: Transitions ``num`` with all possible even natural number
                parameters. 

   * - .. tikz::
          :libs: automata

          [auto,->]
          \renewcommand{\a}[1]{\textit{#1}}
          \node[state,initial] (n)  {};
          \node[state] (e) [below of=n, node distance=4cm] {};
          \foreach \n/\l in {0/0,1/2,2/4,3/6,4/8}
          \foreach \x in {-1.75cm+\n*0.5cm}
          \path[draw,->] (n) .. controls (\x,-2cm)..  (e) 
             node[sloped,below,pos=0.5]{\scriptsize\a{num(\l)}};
          \foreach \n in {5,...,11}
          \foreach \x in {-1.75cm+\n*0.5cm}
          \path[draw,->,dotted] (n) .. controls (\x,-2cm)..  (e) 
             node[sloped,below,pos=0.5]{\scriptsize$\cdots$};
     - ::

          act num: Nat;
          init sum v: Nat . num(2 * v);

The sum operator is quite powerful, especially when combined with the *if-then*
construct ``b -> p`` and the *if-then-else* construct ``b -> p <> q``, which
behaves as process ``p`` if ``b`` evaluates to ``true``, and, in case of the
if-then-else construct, as process ``q`` otherwise. Using such constructs, and a
Boolean function ``even``, we can give an alternative description of the system
in Figure~\ref{fig:nats}::

  map even: Nat -> Bool;
  var n: Nat;
  eqn even(n) = n mod 2 == 0;

  act num: Nat;
  init sum v: Nat . even(v) -> num(v); 

The Boolean condition ``even(v)`` evaluates to ``true`` or ``false``, dependent
on the value of ``v``. If, the expression ``even(v)`` evaluates to ``true``,
action ``num(v)`` is possible. 

.. admonition:: Exercise

   Give, if possible, a µ-calculus  expression that states that the process of 
   Figure~\ref{fig:nats} cannot execute ``num`` actions carrying an odd natural
   number as its parameter. If impossible, explain why.

Data variables that are bound by the ``sum`` operator can affect
the entire process that is within the scope of such operators. This way,
we are able to make the system behaviour data-dependent. Suppose, for instance,
that our coffee machine only accepts coins of 10 cents, and
rejects the 2 and 5 cent coins. The significant states modelling this behaviour,
including parts of the mCRL2 description,
are as follows:

.. list-table:: A picky coffee machine.

   * - .. tikz::
          :libs: automata

          [auto,->]
          \renewcommand{\a}[1]{\textit{#1}}
          \node[state,initial] (n0) at (0,0) {};
          \node[state] (n2) at (-2,0) {};
          \node[state] (n10) at (0,-2) {};
          \node[state] (n5) at (2,0) {};

          \path[->]
          (n0) edge[bend right] node[above] {\scriptsize{\a{coin(c2)}}} (n2)
          (n2) edge[bend right] node[below] {\scriptsize{\a{rej(c2)}}} (n0)
          (n0) edge[bend left] node[above] {\scriptsize{\a{coin(c5)}}} (n5)
          (n5) edge[bend left] node[below] {\scriptsize{\a{rej(c5)}}} (n0)
          (n0) edge[bend left] node[right] {\scriptsize{\a{coin(c10)}}} (n10)
          (n10) edge[bend left] node[left] {\scriptsize{\a{coffee}}} (n0);
     - ::

          sort Val = struct c2 | c5 | c10;

          act coffee; 
              coin, rej: Val;

          proc P = 
            sum v: Val . coin(v) . ( 
              (v != c10) -> rej(v) . P
            + (v == c10) -> coffee . P
            );

          init P;

Data may also be used to parameterise recursion. A typical example of a process
employing such mechanisms is an incrementer::

  act num:Nat;
  proc P(n:Nat) = num(n).P(n+1);
  init P(0);

Or we could have written the process from figure \ref{fig:picky} as follows::

  proc P(v: Val) = 
    coin(v) . (
      (v != c10) -> rej(v) . P
    + (v == c10) -> coffee . P
    );

  init sum v: Val . P(v);

It may be clear that most data-dependent processes describe transition systems
that can no longer be visualised on a sheet of paper. However, the interaction
between the data and process language is quite powerful. 

.. admonition:: Exercises

   #. Is there a labelled transition system with a finite number of states that 
      is bisimilar to the incrementer? If so, give this transition system and
      the witnessing bisimulation relation. If not, explain why such a
      transition system does not exist not.

   #. Consider the mCRL2 specification depicted in Figure~\ref{fig:quirky},
      defining a rather quirky coffee machine. List some odd things about the
      behaviour of this coffee machine and give an alternative specification
      that fixes these.\label{ex:data}

.. list-table:: An mCRL2 specification of a quirky coffee machine.

   * - :: 

          sort Val = struct c2 | c5 | c10;

          map w: Val -> Nat;
          eqn w(c2)  = 2;  
              w(c5)  = 5; 
              w(c10) = 10;

          act insert_coin, return_coin: Val;
              cancel, bad, good;

          proc Loading(t: Int) =
                 sum v: Val . 
                   insert_coin(v) . Loading(t + w(v))
               + (exists v: Val. t >= w(v)) -> cancel . Flushing(t)
               + (t >= 10) -> bad . Loading(t - 10)
               + (t >= 20) -> good . Loading(t - 20);

               Flushing(t: Int) =     
                 sum v: Val . sum t': Nat . 
                   (t == t' + w(v)) -> return_coin(v) . Flushing(t')
               + (forall v: Val . w(v) > t) -> Loading(t);

          init Loading(0);


The first-order µ-calculus
--------------------------

With the introduction of data-dependent behaviour and, in particular,
with the sum operator, we have moved beyond labelled transition
systems that are finitely branching. As you may have found out in
Exercise~\ref{sec:data}.\ref{ex:data}, the logics defined in the previous sections are
no longer adequate to reason about the systems we can now describe.
This is due to the fact that our grammar does not permit us to construct
infinite sized formulae. We mend this by introducing data in the µ-calculus.
This is done gently: first, we extend Hennessy-Milner logic to deal with
the infinite branching.

Consider the action formulae of Hennessy-Milner logic. It allows one
to describe a set of actions. The actions in our LTSs are of a particular
shape: they start with an action name ``a``, taken from a finite
domain of action names, and they carry parameters of a particular sort,
which can possibly be an infinite sized sort. What we shall do is
extend the Hennessy-Milner action formulae with the facilities to
reason about the possible values these expressions can have. This is
most naturally done using quantifiers.

.. admonition:: Definition (Action formulae) 

   An action formula over a set of action names `\act` is an expression that
   adheres to the following syntax in pseudo-BNF, where `a \in \act`, `d` is a
   data variable, `b` is a Boolean expression, `e` is a data expression and `D`
   is a data sort.

   .. math:: A,B &::= b ~|~ a(e) ~|~ \overline{A} ~|~ A \cup B ~|~ \exists d{:}D. A

The following abbreviations may also be used:

.. math::
   :nowrap:
    
   \begin{align*}
   A \cap B &= \overline{\overline{A} \cup \overline{B}} &
   \forall d{:}D. A &= \exists d{:}D. \overline{A}
   \end{align*}

Since our action formulae may now refer to *data variables*, the meaning
of a formula necessarily depends on the value this variable has. The
assignment of values to variables is recorded in a mapping `\varepsilon`.
An action formula `A` over `\act` is associated with a set 
`\sem{A}{\varepsilon} \subseteq \{a(v) ~|~ a \in \act \}` 
in the following manner. 

.. math::
   :nowrap:

    \begin{align*}
    \sem{b}{\varepsilon} &= 
    \{a(v) ~|~ a \in \act \wedge \varepsilon(b)\} \\
    \sem{a(e)}{\varepsilon} &= \{ a(v) ~|~ v = \varepsilon(e) \} \\
    \sem{\overline{A}}{\varepsilon} &= 
    \{ a(v) ~|~ a \in \act \} \setminus \sem{A}{\varepsilon} \\
    \sem{A \cup B}{\varepsilon} &= \sem{A}{\varepsilon} \cup \sem{B}{\varepsilon} \\
    \sem{\exists d{:}D. A}{\varepsilon} &= 
    \bigcup\limits_{v \in D} \sem{A}{\varepsilon[d := v]} 
    \end{align*}

.. admonition:: Remark

   Note that the function `\varepsilon` is used to assign concrete values to
   variables and extends easily to expressions. Consider, for instance, the
   Boolean expression `b \wedge c`, where `b` and `c` are Boolean variables.
   Suppose that function `\varepsilon` states that `\varepsilon(b) =
   \varepsilon(c) = \true`. Then `\sem{b \wedge c}{\varepsilon} = \varepsilon(b
   \wedge c) = \varepsilon(b) \wedge \varepsilon(c) = \true \wedge \true =
   \true`.

The extension of our action formulae with data is sufficiently powerful to
reason about the infinite branching introduced by the sum operator over
infinite data sorts. However, it still does not permit us to reason about
data-dependent behaviour. Consider, for instance, the LTS described by
the following process::

  act num: Nat;
  proc P(n: Nat) = sum m: Nat . (m < n) -> num(m) . P(m);
  init sum m: Nat . P(m);

Each ``num(v)`` action leads to a state with branching degree `v`, in which the
only actions ``num(w)`` possible are those with ``w < v``. Using Hennessy-Milner
logic combined with our new action formulae fails to allow us to express that
from the initial state, no action ``num(v)`` can be followed by an action
``num(v')`` for which ``v <= v'``. We can mend this by also extending the
grammar for Hennessy-Milner logic.

.. admonition:: Definition (First-order HML)

   A *First-order Hennessy-Milner logic* formula interpreted over an LTS with
   action labels `\act` adheres to the following syntax in pseudo-BNF, where `A`
   is an action formula over `\act`, `b` is a Boolean expression, `d` is a data
   variable and `D` is a data sort.

   .. math:: \varphi,\chi ::= \mccan{A}\varphi ~|~ \exists d{:}D.~\varphi ~|~ 
             \varphi \land \chi ~|~ \neg \varphi  ~|~ b ~|~ 

The following common abbreviations are allowed:

.. math::
   :nowrap:

    \begin{align*}
    \exists d{:}D. \varphi&= \neg \forall d{:}D. \neg \varphi & \varphi \lor \chi &= \neg(\neg \varphi \land \neg \chi) \\
    \mcall{`\a{A}`}\varphi &= \neg \mccan{`\a{A}`} \neg \varphi & \varphi \implies \chi &= \neg \varphi \lor \chi
    \end{align*}

An HML formula `\varphi` is interpreted over an LTS `T = \langle S,
\act, \rightarrow, i, f \rangle`, and in the context of a data variable
valuation function `\varepsilon`. Its semantics is given as the set of
states `\sem{\varphi}_T^\varepsilon \subseteq S` of the LTS in which
the formula holds. It is defined as follows.

.. math::
   :nowrap:

    \begin{align*}
    \sem{b}_T^\varepsilon &= \{s \in S ~|~ \varepsilon(b) \}\\
    \sem{\neg\varphi}_T^\varepsilon &= S \setminus \sem{\varphi}_T^\varepsilon \\
    \sem{\varphi \land \chi}_T^\varepsilon &= \sem{\varphi}_T^\varepsilon \cap \sem{\chi}_T^\varepsilon \\
    \sem{\exists d{:}D. \varphi}_T^\varepsilon &= 
    \bigcup\limits_{v \in D} \sem{\varphi}_T^{\varepsilon[d := v]} \\
    \sem{\mccan{A}\varphi}_T^\varepsilon &= \{ s \in S ~|~ \exists_{s'\in S, a \in \sem{A}{\varepsilon}}~ s \stackrel{a}{\longrightarrow} s' \land s' \in \sem{\varphi}_T^\varepsilon \}
    \end{align*}

We say that `T` *satisfies* `\varphi`, denoted `T \models \varphi`, if and only
if for all `\varepsilon`, `i \in \sem{\varphi}_T^\varepsilon`.

.. admonition:: Example

   The property that from the initial state the `\a{num}(v)` action cannot be
   followed by a `\a{num}(v')` action with `v' \geq v` can now be written in a
   number of ways, one of them being
   `\forall_{v,v'\oftype\nat} \mcall{\a{num}(v)}\mcall{\a{num}(v')} v' < v`.

The regular first-order Hennessy-Milner logic extends the first-order 
Hennessy-Milner logic in the same way as regular Hennessy Milner logic
extends Hennessy-Milner logic. This allows us, for instance, to express that along all
paths of the LTS described by Figure~\ref{fig:trees}, the parameters
of the ``num`` action are decreasing:

.. math:: \mcall{\true^*} \forall_{v,v'\oftype\nat} \mcall{`\a{num}`(v) . `\a{num}`(v')} v' < v

In a similar vein, the µ-calculus can be extended with first-order constructs,
allowing for *parameterised recursion*. This allows one to pass on
data values and use these to record events that have been observed in
the past.

Compositionality
================

.. Explain the algebraic aspects of mCRL2

We have seen that systems can be described by means of a labelled transition 
systems. In this section, we will take a closer look at how to describe
labelled transition systems using the *process algebra* mCRL2. To this
end, we need to extend our definition of a labelled transition system a bit
by adding a final state.

.. admonition:: Definition (LTS)

   A *labelled transition system* (LTS) is a tuple `\langle S, \act,
   \rightarrow, i, f \rangle`, where

   - `S` is a set of states,
   - `\act` is a set of action labels,
   - `{\rightarrow} \subseteq S\times \act \times S` is a transition relation,
   - `i\in S` is the initial state and
   - `f\in S` is the final state.

In graphical depictions of transition systems, final states will be marked by
a double circle. In mCRL2, final states are marked by a `\a{Terminate}` action.

We will now discuss how transition systems can be built up from basic building
blocks: the deadlock process, actions and operators on processes. Let us start
with the two building blocks that themselves represent processes.

The **deadlock process** (``delta``)

  is the process that cannot do anything. In particular, it cannot terminate.

  .. list-table:: The deadlock process.

     * - .. tikz::
            :libs: automata

            [auto,->]
            \renewcommand{\a}[1]{\textit{#1}}
            \node[state,initial] (a0) {}; 
       - ::

            init delta;

**Actions**

  A single action is a process. More precisely, it represents the transition
  system that can perform that action and then terminate.

  .. list-table:: Actions are processes.

     * - .. tikz::
            :libs: automata

            [->,auto,node distance=1.5cm]
            \renewcommand{\a}[1]{\textit{#1}}
            \node[state,initial] (a0) {}; 
            \node[state,final,right of=a0] (a1) {};
            \path (a0) edge node {\a{coffee}} (a1);
       - ::

            act coffee;
            init coffee;

All other processes in mCRL2 are created using these two basic building blocks.
To arrive at more complicated processes, they are combined using *operators*
that create new processes by applying a transformation to one or more given
processes.

Operators
---------

We continue by giving a short description of each of the most important operators
on processes in mCRL2.


**Sequential composition** (``.``)

  identifies the final state of its first argument with the initial state of its
  second argument. If the first argument does not have a final state (*e.g.*,
  because the first argument is the deadlock process), then the sequential
  composition is equal to its first argument.

  .. list-table:: Sequential composition.

     * - .. tikz::
            :libs: automata

            [->,auto,node distance=1.5cm]
            \renewcommand{\a}[1]{\textit{#1}}
            \node[state,initial] (a0) {}; 
            \node[state, final, right of=a0] (a1) {};
            \node[state,initial,right of=a1] (b0) {}; 
            \node[state, final, right of=b0] (b1) {};
            \node[state,initial,below of=a0] (c0) {}; 
            \node[state,        right of=c0] (c1) {};
            \node[state,initial,below of=c0] (n0) {}; 
            \node[state, right of=n0] (n1) {}; 
            \node[state,final,right of=n1] (n2) {};
            \node[state,initial,below of=n0] (m0) {}; 
            \node[state, right of=m0] (m1) {}; 
            \node[state,       right of=m1] (m2) {};
            \path (n0) edge node{\a{coin}} (n1) (n1) edge node{\a{coffee}} (n2)
                  (m0) edge node{\a{coin}} (m1) (m1) edge node{\a{coin}} (m2)
                  (a0) node[below]{\ttfamily P} (a0) edge node{\a{coin}} (a1)
                  (b0) node[below]{\ttfamily Q} (b0) edge node{\a{coffee}} (b1)
                  (c0) node[below]{\ttfamily R} (c0) edge node{\a{coin}} (c1);
       - ::

            act coin, coffee;
            proc P = coin;
                 Q = coffee;
                 R = P . delta;
            init P . Q;
            init P . R . Q;

**Alternative composition** (``+``)

  chooses an initial action from the initial actions of its arguments, and then
  continues to behave like the argument it chose its first action from. Note
  that the deadlock process is the neutral element for ``+``; it has no initial
  actions, so ``P + delta`` can only choose to behave like ``P``. Another useful
  property is that any process ``P`` is bisimilar to ``P + P``.

  .. list-table:: Alternative composition.

     * - .. tikz::
            :libs: automata

            [->,auto,node distance=1.5cm]
            \renewcommand{\a}[1]{\textit{#1}}
            \node[state,initial] (a0) {}; 
            \node[state, final, right of=a0] (a1) {};
            \node[state,initial,right of=a1] (b0) {}; 
            \node[state,initial,below of=a0,yshift=-1cm] (n0) {}; 
            \node[state, right of=n0,yshift=0.5cm] (n1) {}; 
            \node[state,final,right of=n0,yshift=-0.5cm] (n2) {};
            \path (n0) edge node[above]{\a{coffee}} (n1) edge node[below]{\a{coffee}} (n2)
                  (n1) edge[loop right] node{\a{coffee}} (n1)
                  (a0) node[below]{\ttfamily P} (a0) edge node{\a{coffee}} (a1)
                  (b0) node[below]{\ttfamily Q} (b0) edge[loop right] node{\a{coffee}} (b0);
       - ::

            act coin, coffee;
            proc P = coin;
                 Q = coffee . Q;

            init P + Q;

**Conditional choice** (``C -> P <> Q``)} 

  behaves like ``P`` if the boolean expression ``C`` evaluates to ``true``, and
  behaves like ``Q`` otherwise. It is allowed to write ``C -> P`` for ``C -> P
  <> delta``. 

  .. admonition:: Example

     The process ``true -> coin <> coffee`` is bisimilar to ``coin``, and ``false
     -> coin <> coffee`` is bisimilar to ``coffee``.

**Summation** (``sum v: T . P``)} 

  is the (possibly infinite) alternative composition of all those processes
  ``P'`` that can be obtained by replacing ``v`` in ``P`` by a value of type
  ``T``. Similar to alternative composition, if ``P`` is the same, regardless of
  the value of ``v``, then ``sum v: T . P`` is bisimilar to ``P``.

  .. list-table:: Summation.

     * - .. tikz::
            :libs: automata

            [->,auto,node distance=1.5cm]
            \renewcommand{\a}[1]{\textit{#1}}
            \node[state,initial] (a0) {}; 
            \node[state, final, right of=a0] (a1) {};
            \node[state,initial,below of=a0] (b0) {}; 
            \node[state, final, right of=b0] (b1) {};
            \node[state,initial,right of=a1] (c0) {}; 
            \node[state, final, below of=c0] (c1) {};
            \path (a0) node[below]{\ttfamily P} (a0) edge[bend left] node{\a{coffee}} (a1)
                                                     edge[bend right] node[below]{\a{coffee}} (a1)
                  (b0) node[below]{\ttfamily Q} (b0) edge[bend left] node{\a{good}} (b1)
                                                     edge[bend right] node[below]{\a{bad}} (b1)
                  (c0) node[above]{\ttfamily R} (c0) edge node[above,rotate=-90]{\a{good}} (c1);

       - ::

            proc P = sum b: Bool . coffee;
                 Q = sum b: Bool . 
                       b -> good <> bad;
                 R = sum b: Bool .
                       !b -> good;

  Rather than writing ``sum x: T1 . sum y: T1 . sum z: T2 . P``, it is also allowed 
  to write the shorter ``sum x, y: T1, z: T2 . P``.

**Parallel composition** (``P || Q``)} 

  is the denotation for the combined state space (transition system) of
  independently running processes ``P`` and ``Q``. It represents the process
  that can behave like ``P`` and ``Q`` simultaneously, and therefore we need a
  device to represent simultaneous execution of actions. This device is the
  *multi-action* operator; if ``a`` and ``b`` are actions, then ``a|b``
  represents the simultaneous execution of ``a`` and ``b``.

  .. list-table:: Parallel composition.

     * - .. tikz::
            :libs: automata

            [->,auto,node distance=1.5cm]
            \renewcommand{\a}[1]{\textit{#1}}
            \node[state] [label=above:M] (n0) {};
            \node[state] [below of=n0,left of=n0] (n1) {};
            \node[state] [below of=n0,right of=n0] (n2) {};
            \node[state, final] [below of=n1,right of=n1] (n3) {};
            \path (n0) edge node[left]{\a{a}} (n1)
                  (n0) edge node[right]{\a{b}} (n2)
                  (n0) edge node[right]{$\a{a}|\a{b}$} (n3)
                  (n1) edge node[left]{\a{b}} (n3)
                  (n2) edge node[right]{\a{a}} (n3);

       - ::

            act a, b;
            proc M = a || b;
            init M;

  If the state space of ``P`` counts `n` states, and that of ``Q`` counts `m`
  states, then the state space of ``P || Q`` will have `n \cdot m` states. It is
  important to realise this, because it implies that the parallel composition of
  `N` processes will yield a statespace of a size that is exponential in `N`.

**Communication** (``comm(C, P)``)

  is an operator that performs a renaming of multi-actions in which every action
  has identical parameters. The set ``C`` specifies multi-action names that
  should be renamed using the following syntax for each renaming:
  ``a1|...|aN->b``, where ``b`` and ``a1`` through ``aN`` are action names. The
  operation is best understood by looking at the example in Figure
  \ref{fig:comm-operator}.

  .. list-table:: Communication operator.

     * - .. tikz::
            :libs: automata

            [->,auto,node distance=1.5cm]
            \renewcommand{\a}[1]{\textit{#1}}
            \node[state] [initial] (n0) {};
            \node[state] [below of=n0] (n1) {};
            \node[state] [right of=n0] (n2) {};
            \node[state, final] [below of=n2] (n3) {};
            \path (n0) edge node[left]{\a{a}(1)} (n1)
                  (n0) edge node{\a{b}(1)} (n2)
                  (n0) edge node[above=0mm,sloped]{\a{c}(1)} (n3)
                  (n1) edge node[below]{\a{b}(1)} (n3)
                  (n2) edge node{\a{a}(1)} (n3);

            \begin{scope}[yshift=-3cm]
            \node[state] [initial] (n0) {};
            \node[state] [below of=n0] (n1) {};
            \node[state] [right of=n0] (n2) {};
            \node[state, final] [below of=n2] (n3) {};
            \path (n0) edge node[left]{\a{a}(1)} (n1)
                  (n0) edge node{\a{b}(2)} (n2)
                  (n0) edge node[above=0mm,sloped]{$\a{a}(1)|\a{b}(2)$} (n3)
                  (n1) edge node[below]{\a{b}(2)} (n3)
                  (n2) edge node{\a{a}(1)} (n3);
            \end{scope}
       - ::

            act a, b, c: Nat;
            proc P = a(1) || b(1);
            init comm({a|b->c}, P);







            act a, b, c: Nat;
            proc Q = a(1) || b(2);
            init comm({a|b->c}, Q);

**Rename** (``rename(R, P)``)} 

  works exactly like the communication operator, except that only single action
  names can be renamed (not multi-actions).

**Allow** (``allow(A, P)``)

  removes all multi-actions from the transition system that do not occur in
  ``A``. Any states that have become unreachable will also be removed by mCRL2,
  as the resulting system is smaller and bisimilar.

  .. list-table:: Allow operator.

     * - .. tikz::
            :libs: automata

            [->,auto]
            \renewcommand{\a}[1]{\textit{#1}}
            \node[state] [initial] (n0) {};
            \node[state] [below of=n0] (n1) {};
            \node[state, final] [right of=n1] (n3) {};
            \path (n0) edge node[left]{\a{a}} (n1)
                  (n0) edge node[above=0mm,sloped]{\a{c}} (n3);

            \begin{scope}[yshift=-2cm]
            \node[state] [initial] (n0) {};
            \node[state] [below of=n0] (n1) {};
            \node[state, final] [right of=n1] (n3) {};
            \path (n0) edge node[left]{\a{a}} (n1)
                  (n0) edge node[above=0mm,sloped]{$\a{a}|\a{b}$} (n3);
            \end{scope}
       - ::

            act a, b, c;
            proc P = a || b;
            init allow({a,c}, 
                   comm({a|b->c}, P));




            init allow({b|a,a}, P);

  Note that the multi-action operator is commutative, so the order in which the 
  actions appear does not matter.

**Hide** (``hide(H, P)``)

  performs a renaming of the actions in the set ``H`` to the special action name
  ``tau`` (which takes no parameters). This special action represents an event
  that is invisible to an outside observer, and therefore has some special
  properties. For instance, ``allow(A, tau)`` is always equal to ``tau`` per
  definition, regardless of the contents of ``A`` (so effectively, internal
  cannot be blocked).

  Hiding can be used to abstract away from events and gives rise to coarser
  notions of behavioural equality, such as *branching bisimulation*. Such
  equalities again compare systems based on what can be observed of a system,
  but this time taking into account that the ``tau`` action cannot be observed
  directly.

Communicating systems
---------------------

To conclude, we give an example of how the operators from the previous section
can be used to model interacting processes. 

Communication is modelled by assigning a special meaning to actions that occur
simultaneously. To say that an action `\a{a}` communicates with an action
`\a{b}` is to say that `\a{a}|\a{b}` may occur, but `\a{a}` and `\a{b}` cannot
occur separately. Usually this scenario will correspond to `\a{a}` and `\a{b}`
being a send/receive action pair. This standard way of communicating is
*synchronous*, i.e. a sender may be prevented from sending because there is no
receiver to receive the communication.

Going back to the coffee machine, we now show how we can model a user that is
interacting with such a machine. The user is rather stingy, and is not prepared
to pay two coins. In mCRL2, it looks like this::

  act coin, good, bad,
      pay, yay, boo;
  proc M = coin . (coin . good + bad) . M;
  proc U = coin . (good + bad) . U;
  init allow({pay, yay, boo},
         comm({coin|coin->pay,good|good->yay,bad|bad->boo},
           U || M
       ));

The corresponding statespaces are shown below. On the right hand side, the
statespace of the parallel composition is shown. The two black transitions are
all that remain when communication and blocking are applied.

.. tikz::
   :libs: automata

    [->,auto]
    \renewcommand{\a}[1]{\textit{#1}}
    \begin{scope}[yshift=1cm]
    \node[state] [label=above:M] (n0) {};
    \node[state] [below of=n0] (n1) {};
    \node[state] [below of=n1] (n2) {};
    \path (n0) edge[left] node{\a{coin}} (n1)
          (n1) edge[left] node{\a{coin}} (n2)
          (n1) edge[bend right=90] node[right]{\a{bad}} (n0)
          (n2) edge[bend right=90] node[right]{\a{good}} (n0);
    \end{scope}
    \begin{scope}[xshift=3cm]
    \node[state] [label=left:U] (n0) {};
    \node[left of=n0] {\color{red}$\|$};
    \node[state] [right of=n0] (n1) {};
    \path (n0) edge node{\a{coin}} (n1)
          (n1) edge[bend left=90] node{\a{good}} (n0)
          (n1) edge[bend right=90] node[above]{\a{bad}} (n0);
    \end{scope}
    \begin{scope}[hide/.style={draw=gray},node distance=2cm,xshift=6cm,yshift=2cm]
    \node[state] [label=left:UM] (n00) {};  \node[state] [right of=n00,hide] (n10) {};
    \node[state,hide] [below of=n00]    (n01) {};  \node[state] [right of=n01] (n11) {};
    \node[state,hide] [below of=n01]    (n02) {};  \node[state] [right of=n02,hide] (n12) {};
    \node[left of=n01,node distance=1cm] {\color{red}$=$};
    \path (n00) edge[hide] (n10)
          (n10) edge[hide,bend left=20] (n00)
          (n10) edge[hide,bend right=20] (n00)
          (n01) edge[hide] (n11)
          (n11) edge[hide,bend left=20] (n01)
          (n11) edge[hide,bend right=20] (n01)
          (n02) edge[hide] (n12)
          (n12) edge[hide,bend left=20] (n02)
          (n12) edge[hide,bend right=20] (n02)
          (n00) edge[hide] (n01)
          (n01) edge[hide,bend left=20] (n00)
          (n01) edge[hide] (n02)
          (n02) edge[hide,bend left=20] (n00)
          (n10) edge[hide] (n11)
          (n11) edge[hide,bend right=20] (n10)
          (n11) edge[hide] (n12)
          (n12) edge[hide,bend right=20] (n10)
          (n01) edge[hide, bend left=20] (n12)
          (n12) edge[hide, bend left=20] (n01)
          (n10) edge[hide, bend left=20] (n01)
          (n10) edge[hide, bend right=20] (n01)
          (n01) edge[hide] (n10)
          (n11) edge[hide, bend left=20] (n02)
          (n11) edge[hide, bend right=20] (n02)
          (n12) edge[hide, bend left=5] (n00)
          (n12) edge[hide, bend right=5] (n00)
          (n02) edge[hide] (n10)
          (n00) edge[hide, bend right=20] (n11)
          (n00) edge[bend left=20] node[right]{\a{pay}} (n11)
          (n11) edge node[left]{\a{boo}} (n00);
    \end{scope}

It is obvious from this picture that you get what you pay for: good coffee is
not achievable for this user. The picture also illustrates that synchronicity of
systems helps reduce the state space tremendously. If the actions of the machine
and the user had been completely independent, then the resulting state space
would have had 3 times more states, and 15 times more transitions.

.. note:: 

   In our example, we only have two communicating parties, so that no confusion
   can arise as to who was supposed to be communicating with who. For larger
   systems, more elaborate naming schemes for actions are often used in order to
   avoid mistakes, so for instance the sender of a message will perform a
   ``s_msg`` action, and the receiver a ``r_msg`` action, rather than both using
   an action called ``msg``.

   It should also be noted that for instance `n`-way communication is also
   possible, which can be useful to model, *e.g.*, barrier synchronisation or
   clock ticks.

.. rubric:: Footnotes

.. [#f1] If a formula is not in positive normal form, then its least
   and greatest fixpoint are not guaranteed to exist, hence the requirement. 
   See also \cite{?}
