Notes on the bisimulation partitioner
======================================

*Authors: Jan Friso Groote and Jan Martens*

Introduction
------------

This document describes the algorithm for branching bisimulation reduction,
including the algorithm that is used to generate counterexamples.

A partitioner for branching bisimulation
-----------------------------------------

The partitioner for branching bisimulation calculates whether states are
bisimilar, branching bisimilar and stuttering preserving branching bisimilar. It
gets a state space and divides it into a number of non-intersecting subsets of
states called *blocks*. All states in a block are bisimilar and have the same
block index. Using this index it is straightforward to calculate whether two
states are equivalent (they have the same index) or to construct the state space
modulo this equivalence.

The algorithm works exactly as described in [GV90]_. As a preprocessing step for
(divergence preserving) branching bisimulation, all states that are strongly
connected via internal transitions are replaced by a single state. In case of
divergence preserving branching bisimulation this state gets a tau loop. In case
of ordinary branching bisimulation, there will not be a tau loop.

Then the algorithm for branching bisimulation is started. The state space is
partitioned into blocks. Initially, all states are put in one block. Repeatedly,
a block is split in two blocks until the partitioning has become stable. For
details see [GV90]_.

Furthermore, there is an option to obtain counter formulas for two non bisimilar
states. The algorithm for this is inspired by [C90]_ and [K91]_.

Given two non bisimilar states :math:`s, t \in S`, where :math:`S` is the set
of all states, a *distinguishing formula* is a formula :math:`\phi` in
Hennessy-Milner logic such that :math:`s \models \phi` and
:math:`t \not\models \phi`. For branching bisimulation there is always a
distinguishing formula in Hennessy-Milner logic extended with the regular
:math:`\langle \tau^* \rangle`, and using the abbreviation
:math:`\langle \tau + \mathit{nil} \rangle \phi \;:=\; \langle\tau\rangle \phi \vee \phi`.

Following [M23]_ and [M24]_ we implement the computation of minimal depth
distinguishing formulas. Two aspects of the implementation differ from the
referenced theoretical background.

Filtering
^^^^^^^^^

There is one post-processing step we call *backwards filtering*. It deals with
the following scenario.

Consider the transition systems depicted below.

.. math::
   :nowrap:
   :class: math-tikz

   \begin{tikzpicture}[node distance=2.5cm]
       \node[state] (s)                    {$s$};
       \node[state] (s1) [below of=s]      {$s_1$};
       \node[state] (t)  [right=5cm of s]  {$t$};
       \node[state] (t1) [below of=t]      {$t_1$};
       \node[state] (t2) [right of=t1]     {$t_2$};
       \path[->]
         (s)  edge                node[left]  {$a$}   (s1)
         (t)  edge                node[left]  {$a$}   (t1)
         (t)  edge                node[right] {$a$}   (t2)
         (s1) edge[loop below]    node[below] {$a,b$} (s1)
         (t2) edge[loop below]    node[below] {$a$}   (t2);
   \end{tikzpicture}

*Two LTSs with initial states* :math:`s` *and* :math:`t` *respectively.*

We consider the scenario of computing a distinguishing formula :math:`\phi(s,t)`
for states :math:`s` and :math:`t`. By the partitioning algorithm we know that
the transition :math:`s \xrightarrow{a} s_1` is a distinguishing observation. The
algorithm has to recursively find a distinguishing formula :math:`\phi'` such
that :math:`s_1 \in \llbracket \phi' \rrbracket` and
:math:`t_1, t_2 \not\in \llbracket \phi' \rrbracket`. This way
:math:`s \in \llbracket \langle a \rangle \phi' \rrbracket` and
:math:`t \not\in \llbracket \langle a \rangle \phi' \rrbracket`.

The algorithm starts by recursively computing a distinguishing formula for
:math:`s_1` and :math:`t_1`. It might compute
:math:`\phi(s_1, t_1) = \langle a \rangle \mathrm{true}`. Since
:math:`t_2 \in \llbracket \langle a \rangle \mathrm{true} \rrbracket`, it also
computes the distinguishing formula
:math:`\phi(s_1, t_2) = \langle b \rangle \mathrm{true}`. This results in the
formula

.. math::

   \phi(s, t) = \langle a \rangle \bigl(\langle a \rangle \mathrm{true}
                \wedge \langle b \rangle \mathrm{true}\bigr).

We see that adding the conjunct
:math:`\phi(s_1, t_2) = \langle b \rangle \mathrm{true}` made the first conjunct
:math:`\phi(s_1, t_1)` obsolete (since
:math:`t_1 \not\in \llbracket \langle b \rangle \mathrm{true} \rrbracket`). This
scenario is very hard to prevent a priori.

To avoid a distinguishing formula with unnecessary conjuncts, each conjunct is
reconsidered in FIFO order. We compute the semantics of all other conjuncts and
check whether they already achieve the goal. If so, the conjunct under
consideration can be safely removed.

Minimal depth partitioning for branching bisimulation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

According to [M24]_ we compute the partitions conforming to the correct
:math:`k`-depth relations. Given the partition :math:`\pi_i` on level :math:`i`
we want to construct :math:`\pi_{i+1}` such that it is stable with respect to
the following signatures:

.. math::

   \mathit{sig}(s,\, \pi_i) \;:=\;
   \bigl\{\,(B, a, B') \;\big|\;
     s \xrightarrow{\tau^*} s' \xrightarrow{a} s'',\;
     s' \in B \in \pi_i,\; s'' \in B' \in \pi_i,\;
     B \neq B' \text{ or } a \neq \tau
   \,\bigr\}

Performing this partitioning step efficiently is not obvious. We implemented the
following algorithm. It relies on the preprocessing step having removed all
strongly connected :math:`\tau` components, so no :math:`\tau` cycles exist.

.. code-block:: none

   frontier := { s | s has no outgoing τ-transition }
   done     := ∅
   while frontier ≠ ∅ do
     s := frontier.pop_front()
     sig := { (B, a, B') | s →ᵃ s', s∈B, s'∈B', B,B'∈πᵢ, a≠τ or B≠B' }
     for all s →ᵗ s' do
       sig := sig ∪ sigs[s']
     sigs[s] := sig
     done    := done ∪ {s}
     for all sₚ →ᵗ s do
       if { s' | sₚ →ᵗ s' } \ done = ∅ then
         frontier.push_back(sₚ)

References
----------

.. [C90] R. Cleaveland. On automatically explaining bisimulation inequivalence.
   In E. M. Clarke and R. P. Kurshan, editors, *Computer Aided Verification
   (CAV'90)*, volume 531 of *Lecture Notes in Computer Science*,
   Springer-Verlag, pages 364–372, 1990.

.. [GV90] J. F. Groote and F. W. Vaandrager. An efficient algorithm for
   branching bisimulation and stuttering equivalence.
   In M. S. Paterson, editor, *Proceedings 17th ICALP, Warwick*, volume 443 of
   *Lecture Notes in Computer Science*, pages 626–638. Springer-Verlag, 1990.

.. [K91] H. Korver. Computing distinguishing formulas for branching bisimulation.
   In K. G. Larsen and A. Skou, editors, *Computer Aided Verification
   (CAV'91)*, volume 575 of *Lecture Notes in Computer Science*,
   Springer-Verlag, pages 13–23, 1991.

.. [M23] J. J. M. Martens and J. F. Groote. Computing Minimal Distinguishing
   Hennessy-Milner Formulas is NP-Hard, but Variants are Tractable.
   In G.-A. Pérez and J.-F. Raskin, editors, *CONCUR '23*, Volume 279 of
   *LIPIcs*, Schloss Dagstuhl – Leibniz-Zentrum für Informatik,
   pages 32:1–32:17, 2023.

.. [M24] J. J. M. Martens and J. F. Groote. Minimal Depth Distinguishing
   Formulas without Until for Branching Bisimulation.
   To appear in LNCS, 2024.
