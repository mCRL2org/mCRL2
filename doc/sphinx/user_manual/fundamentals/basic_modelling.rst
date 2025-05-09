:Date: 13-02-2012
:Authors: - Sjoerd Cranen
          - Tim Willemse
:Institute: Eindhoven University of Technology

.. default-role:: math
.. highlight:: mcrl2

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
`(s,a,s') \in\,\rightarrow`.


.. _example1:
   
.. admonition:: Example

   Consider a simple coffee machine that accepts a single coin and then
   dispenses coffee. This system could be modelled by the LTS with 
   `S=\{s_0, s_1, s_2\}`, `\act=\{\a{coin}, \a{coffee}\}`, `\rightarrow =
   \{(s_0, \a{coin}, s_1), (s_1, \a{coffee}, s_2)\}`, `i=s_0` and `f=s_2`. Its
   graphical representation is as follows:

   .. math::
      :nowrap:

      \begin{tikzpicture}
         \tikzstyle{every state}=[
            draw,
            shape=circle,
            inner sep=1pt,
            minimum size=5pt,
            node distance=1cm,
            final/.style={double,minimum size=6pt},
            initial text=]

         [auto,->]
         \node[state, initial] (n0) {}; 
         \node[state, right=2cm of n0] (n1) {}; 
         \node[state, accepting, right=2cm of n1] (n2) {};
         \path[->] (n0) edge node[above]{\a{coin}} (n1) 
                   (n1) edge node[above]{\a{coffee}} (n2);
      \end{tikzpicture}

Sequences and choices
---------------------

The coffee machine in the :ref:`example <example1>` above is not very exciting. It performs
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

   * - .. math::
         :nowrap:

         \begin{tikzpicture}
            \tikzstyle{every state}=[
               draw,
               shape=circle,
               inner sep=1pt,
               minimum size=5pt,
               node distance=1cm,
               final/.style={double,minimum size=6pt},
               initial text=]

            [auto,->]
            \node[state, initial] (n0) {}; 
            \node[state, right=2cm of n0] (n1) {}; 
            \node[state, accepting, right=2cm of n1] (n2) {};
            \path[->] (n0) edge node[above]{\a{coin}} (n1) 
                     (n1) edge node[above]{\a{coffee}} (n2);
         \end{tikzpicture}

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

.. _coffee2:
.. list-table:: Another coffee machine.

   * - .. math::
         :nowrap:

         \begin{tikzpicture}
            \tikzstyle{every state}=[
               draw,
               shape=circle,
               inner sep=1pt,
               minimum size=5pt,
               final/.style={double,minimum size=6pt},
               initial text=]

            [auto,->]
            \renewcommand{\a}[1]{\textit{#1}}
            \node[state,initial] (n0) {}; 
            \node[state, right=2cm of n0] (n1) {}; 
            \node[state, accepting, right=2cm of n1] (n2) {};
            \node[state, accepting, below right=2cm of n1] (n3) {};
            \path[->] 
                  (n0) edge node[above]{\a{coin}} (n1) 
                  (n1) edge node[above]{\a{bad}} (n2)
                  (n1) edge node[below left]{\a{coin}} (n3)
                  (n3) edge node[right]{\a{good}} (n2)
                  ;
         \end{tikzpicture}

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
   accepting one coin. This formula does not hold for the machine in figure with
   :ref:`another coffee machine <coffee2>`, because when you insert one coin, the machine will only
   provide you with bad coffee. 

.. admonition:: Example

   The formula `\mcall{\a{coin}}\mccan{\a{bad} \cup \a{coin}}\true` does hold
   for the machine in :ref:`another coffee machine <coffee2>`; it says that always after
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

.. _exercise-bisim:
.. admonition:: Example

   In the following diagram, the dotted lines indicate the pairs of nodes that
   are related by a relation `R`.

   .. math::
      :nowrap:
      
      \begin{tikzpicture}
         \tikzstyle{every state}=[
         draw,
         shape=circle,
         inner sep=1pt,
         minimum size=5pt,
         final/.style={double,minimum size=6pt},
         initial text=]
         [auto,->]
         \renewcommand{\a}[1]{\textit{#1}}
         \begin{scope}
         \node[state,initial] (l1) {};
         \node[state,below=2cm of l1] (l2) {};
         \node[state,below=2cm of l2] (l3) {};
         \path[->] (l1) edge node[left]{\a{coin}} (l2)
                   (l2) edge node[left]{\a{coffee}} (l3);
         \end{scope}
         \begin{scope}[xshift=4cm]
         \node[state,initial] (r1) {};
         \node[state,below left=2cm and 2cm of r1] (r2) {};
         \node[state,below right=2cm and 2cm of r1] (r3) {};
         \node[state,below left=2cm and 1cm of r2] (r4) {};
         \node[state,right=2cm of r4] (r5) {};
         \node[state,right=2cm of r5] (r6) {};
         \node[state,right=2cm of r6] (r7) {};
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
      \end{tikzpicture}

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

   In the following diagram, the dotted lines indicate the pairs of states that
   are related by a relation `R`.

   .. math::
      :nowrap:

      \begin{tikzpicture}
         \tikzstyle{every state}=[
            draw,
            shape=circle,
            inner sep=1pt,
            minimum size=5pt,
            final/.style={double,minimum size=6pt},
            initial text=]
         [auto,->]
         \renewcommand{\a}[1]{\textit{#1}}
         \node[state,initial] (r1) {};
         \node[state,below left=2cm of r1] (r2) {};
         \node[state,below right=2cm of r1] (r3) {};
         \node[state,below left=2cm and 1cm of r2] (r4) {};
         \node[state,right=2cm of r4] (r5) {};
         \node[state,right=1cm of r5] (r6) {};
         \node[state,right=2cm of r6] (r7) {};
         \path[->] (r1) edge node[left] {\a{coin}} (r2) 
                        edge node[right]{\a{coin}} (r3)
                     (r2) edge node[above,rotate=60]{\a{coffee}} (r4) 
                        edge node[above,rotate=-60]{\a{coffee}} (r5)
                     (r3) edge node[above,rotate=60]{\a{coffee}} (r6) 
                        edge node[above,rotate=-60]{\a{coffee}} (r7);

         \path[dotted,bend right]
            (r2) edge (r3)
            (r4) edge (r5) edge (r6) edge (r7) 
            (r5) edge (r6) edge (r7)
            (r6) edge (r7); 
      \end{tikzpicture}  

   `R` is a bisimulation relation, so merging all related states will yield a
   smaller, bisimilar transition system (namely the left transition system of
   the :ref:`previous bisimulation example <exercise-bisim>`).

.. admonition:: Exercise

   Are the following two process definitions bisimilar?
   ::
        
      act coin, good, bad;
      init coin . (bad + coin . good);

      act coin, good, bad;
      init coin . bad + coin . coin . good;