.. default-role:: math
.. highlight:: mcrl2

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
                \node[state,initial] (a0) {}; 
            \end{tikzpicture}

       - ::

            init delta;

**Actions**

  A single action is a process. More precisely, it represents the transition
  system that can perform that action and then terminate.

  .. list-table:: Actions are processes.

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
            
                [->,auto,node distance=1.5cm]
                \renewcommand{\a}[1]{\textit{#1}}
                \node[state,initial] (a0) {}; 
                \node[state,final,right of=a0] (a1) {};
                \path (a0) edge node[above] {\a{coffee}} (a1);
            \end{tikzpicture}

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
            \end{tikzpicture}

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
            \end{tikzpicture}

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
            \end{tikzpicture}

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
            \end{tikzpicture}

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
  operation is best understood by looking at the example 
  :ref:`about the communication operator <comm-operator>`.

  .. _comm-operator:
  .. list-table:: Communication operator.

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
            \end{tikzpicture}

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
          \end{tikzpicture}

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
     \end{tikzpicture}

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