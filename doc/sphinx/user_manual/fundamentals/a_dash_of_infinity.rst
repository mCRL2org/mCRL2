.. default-role:: math
.. highlight:: mcrl2
   
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

.. _coffee-inf1:
.. list-table:: An ever-lasting coffee machine.

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
            \scriptsize
            \node[state,initial] (n0) {P}; 
            \node[state, right of=n0] (n1) {}; 
            \node[state, right of=n1] (n2) {};
            \path (n0) edge[bend left] node[above]{\a{coin}} (n1) 
                  (n1) edge[bend left] node[below]{\a{bad}} (n0)
                  (n1) edge node[above]{\a{coin}} (n2) 
                  (n2) edge[bend left=90] node[below]{\a{good}} (n0);
         \end{tikzpicture}

     - ::

          act coin, good, bad;
          proc P = coin . 
                     (bad . P +
                      coin . good . P);
          init P;

In the specification, we see that the ``proc`` operator accepts equations that
associate processes with process names. In this case, the process ``P`` is
defined. By using it in the right-hand side of its own definition, we introduced
infinitely repeating behaviour. To illustrate this, we could *unfold* this
repetition once and obtain a bisimilar system, as shown in 
:ref:`the figure of the unfolded ever-lasting coffee machine <coffee-inf2>`.

.. _coffee-inf2:
.. list-table:: The ever-lasting coffee machine, unfolded once.

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
            \scriptsize
            
            \node[state,initial,initial where=right] (n3) {};
            \node[state, left of=n3] (n4) {};
            \node[state, left of=n4] (n5) {};
            \node[state, below of=n5,yshift=-0.5cm] (n0) {P}; 
            \node[state, right of=n0] (n1) {}; 
            \node[state, right of=n1] (n2) {};
            \path (n3) edge node[above]{\a{coin}} (n4) 
                  (n4) edge node[above]{\a{coin}} (n5)
                  (n4) edge[bend right] node[right]{\a{bad}} (n0) 
                  (n5) edge node[left]{\a{good}} (n0)
                  (n0) edge[bend left] node[above]{\a{coin}} (n1) 
                  (n1) edge[bend left] node[below]{\a{bad}} (n0)
                  (n1) edge node[below]{\a{coin}} (n2) 
                  (n2) edge[bend left=90] node[below]{\a{good}} (n0);
         \end{tikzpicture}
   
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
  
   Show that the statespaces from figures :ref:`the ever-lasting coffee machine <coffee-inf1>` and
   :ref:`the unfolded ever-lasting coffee machine <coffee-inf2>` are bisimilar.

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

   \mccan{\alpha \cdot \beta}\varphi &= \mccan{\alpha}\mccan{\beta}\varphi \\
   \mccan{\alpha + \beta}\varphi &= (\mccan{\alpha}\varphi) \lor (\mccan{\beta}\varphi) \\
   \mccan{\alpha^*}\varphi &= \varphi \lor (\mccan{\alpha}\varphi) \lor (\mccan{\alpha\cdot\alpha}\varphi) \lor \ldots

.. admonition:: Exercise

   Rewrite the regular HML formula `[a+b]\false` to a pure HML formula.

The modal µ-calculus
--------------------

Although regular HML is a powerful tool to specify properties over infinite
systems, it is still not expressive enough to formulate so-called 
fairness properties*. These are properties that say things like *if the system is offered
the possibility to perform an action infinitely often, then it will eventually
perform this action*.

Another way of extending HML to deal with infinite behaviour is to add a
*least fixpoint operator*. This extension is called the *modal µ-calculus*, named
after the least fixpoint operator `\mu`. The µ-calculus (we often leave out the
*modal*, as no confusion can arise) is famous for its expressivity, and infamous
for its unintelligability. We will therefore first give the definition and the
formal semantics, and then elaborate more informally on its use.

.. admonition:: Definition (µ-calculus)

   A *µ-calculus* formula interpreted over an LTS with action labels `\act` 
   adheres to the following syntax in pseudo-BNF, where `A` is an action formula
   over `\act`, and `X` is chosen from some set of variable names `\mathcal{X}`.

   .. math::
      
      \varphi,\chi ::= \true ~|~ X ~|~ \mu X\,.\, \varphi ~|~ \mccan{A}\varphi 
      ~|~ \varphi \land \chi ~|~ \neg \varphi

We allow the same abbreviations as for HML, and we add the *greatest fixpoint
operator*, which is the dual of the least fixpoint operator:

.. math::

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
variables, we also need a *predicate environment* `\rho: \mathcal{X} \to 2^S`,
which maps variable names to their semantics (*i.e.*, sets of states from
`T`). We use `\rho[X\mapsto V]` to denote the environment that maps `X` to the
set `V`, and that maps all other variable names in the same way `\rho` does.

The semantics of a formula is now given as the set of states
`\sem{\varphi}_T^\rho \subseteq S`, defined as follows.

.. math::
  :nowrap:

    \begin{align*}
      \sem{\true}_T^\rho &= S       \\
      \sem{X}_T^\rho     &= \rho(X) \\
      \sem{\neg\varphi}_T^\rho        &= S \setminus \sem{\varphi}_T^\rho            \\
      \sem{\varphi \land \chi}_T^\rho &= \sem{\varphi}_T^\rho \cap \sem{\chi}_T^\rho \\
      \sem{\mccan{a}\varphi}_T^\rho   &= \{ s \in S ~|~ \exists_{s'\in S}~ s \stackrel{a}{\longrightarrow} s' \land s' \in \sem{\varphi}_T^\rho \} \\
      \sem{\mu X\,.\,\varphi}_T^\rho  &= \bigcap \{V \subseteq S ~|~ \sem{\varphi}_T^{\rho[X \mapsto V]} \subseteq V \}
    \end{align*}

We say that `T` *satisfies* `\varphi`, denoted `T \models \varphi`, if and only
if `i \in \sem{\varphi}_T^\rho` for any `\rho`.

Using the µ-calculus
""""""""""""""""""""

To understand how the µ-calculus can be used to express properties of systems, it
is instructive to see that regular HML can be encoded into the modal µ-calculus by
using the following equalities.

.. math::

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

   .. math:: \mu X\,.\, \mccan{\true}\true \land \mcall{\overline{\a{coffee}}} X

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
            \node[state,initial] (n)  {};
            \node[state] (e) [below of=n, node distance=2cm] {};
            \path
               (n) edge[bend left=60] node[above,rotate=-90]{\scriptsize{\a{coin(c2)}}} (e)
                     edge node[above,rotate=-90]{\scriptsize{\a{coin(c5)}}} (e)
                     edge[bend right=60] node[below,rotate=-90]{\scriptsize{\a{coin(c10)}}} (e);
         \end{tikzpicture}

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
a state may become infinite, as illustrated by figure :ref:`nats`.
Since each mCRL2 expression is finite, we can no longer give an
equivalent expression using only the plus operator.

.. _nats:
.. list-table:: Transition system with an infinite number of transitions. 

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
         \end{tikzpicture}

     - ::

          act num: Nat;
          init sum v: Nat . num(2 * v);

The sum operator is quite powerful, especially when combined with the *if-then*
construct ``b -> p`` and the *if-then-else* construct ``b -> p <> q``, which
behaves as process ``p`` if ``b`` evaluates to ``true``, and, in case of the
if-then-else construct, as process ``q`` otherwise. Using such constructs, and a
Boolean function ``even``, we can give an alternative description of the 
:ref:`infinite transition system above <nats>`::

  map even: Nat -> Bool;
  var n: Nat;
  eqn even(n) = n mod 2 == 0;

  act num: Nat;
  init sum v: Nat . even(v) -> num(v); 

The Boolean condition ``even(v)`` evaluates to ``true`` or ``false``, dependent
on the value of ``v``. If, the expression ``even(v)`` evaluates to ``true``,
action ``num(v)`` is possible. 

.. admonition:: Exercise

   Give a µ-calculus  expression that states that this 
   :ref:`process <nats>` cannot execute actions ``num`` with an odd natural
   number as its parameter. 

Data variables that are bound by the ``sum`` operator can affect
the entire process that is within the scope of such operators. This way,
we are able to make the system behaviour data-dependent. Suppose, for instance,
that our coffee machine only accepts coins of 10 cents, and
rejects the 2 and 5 cent coins. The significant states modelling this behaviour,
including parts of the mCRL2 description,
are as follows:

.. _picky:
.. list-table:: A picky coffee machine.

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
         \end{tikzpicture}
          
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

Or we could have written the :ref:`picky coffee machine <picky>` as follows::

  proc P(v: Val) = 
    coin(v) . (
      (v != c10) -> rej(v) . P
    + (v == c10) -> coffee . P
    );

  init sum v: Val . P(v);

It may be clear that most data-dependent processes describe transition systems
that can no longer be visualised on a sheet of paper. However, the interaction
between the data and process language is quite powerful. 

.. _exercise_data:
.. admonition:: Exercises

   #. Is there a labelled transition system with a finite number of states that 
      is bisimilar to the incrementer? If so, give this transition system and
      the witnessing bisimulation relation. If not, explain why such a
      transition system does not exist.

   #. Consider the mCRL2 specification depicted :ref:`below <quirky>`,
      defining a rather quirky coffee machine. List some odd things about the
      behaviour of this coffee machine and give an alternative specification
      that fixes these.

.. _quirky:
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
:ref:`this exercise <exercise_data>`, the logics defined in the previous sections are
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

   .. math:: A,B ::= b ~|~ a(e) ~|~ \overline{A} ~|~ A \cup B ~|~ \exists d{:}D. A

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
the following process:

.. _trees:
.. list-table:: A transition system with nested trees.

   * - :: 

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
paths of the LTS described by :ref:`this transition system <trees>`, the parameters
of the action ``num`` are decreasing:

.. math:: \mcall{\true^*} \forall_{v,v'\oftype\nat} \mcall{\a{num}(v) . \a{num}(v')} v' < v

In a similar vein, the µ-calculus can be extended with first-order constructs,
allowing for *parameterised recursion*. This allows one to pass on
data values and use these to record events that have been observed in
the past.

.. rubric:: Footnotes

.. [#f1] If a formula is not in positive normal form, then its least
   and greatest fixpoint are not guaranteed to exist, hence the requirement. 
