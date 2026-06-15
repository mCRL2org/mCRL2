.. _lps-communication-open-terms:

Calculation of Communication with Open Terms
============================================

| *Author*: Muck van Weerdenburg

We recall the definition of the communication function :math:`\gamma` of [vW04]_.

Notation
--------

We use the following notation throughout:

- :math:`\boldsymbol{A}` — the set of all parameterised actions :math:`a(\vec{d})`
- :math:`\mathcal{N}_A` — the set of action names
- :math:`\mathbb{B}(X)` — the set of finite bags (multisets) over a set :math:`X`
- :math:`[a_1(\vec{d}_1), \ldots, a_k(\vec{d}_k)]` — a multiaction (bag of parameterised actions); the empty bag is written :math:`[]`
- :math:`\oplus`, :math:`\ominus` — multiset union and difference
- :math:`\mu(m)` — the bag of action names in multiaction :math:`m`; e.g. :math:`\mu([a(\vec{d}), b(\vec{e})]) = [a, b]`
- :math:`|m|` — the cardinality of bag :math:`m`
- :math:`N_{\mathbb{B}} = \{ n \mid n \in \mathbb{B}(\mathcal{N}_A) \wedge 1 < |n| \}` — bags of action names with at least two elements
- :math:`T_{\mathbb{B}}` — the set of open boolean terms
- :math:`\boldsymbol{A'}` — the set of parameterised actions with open (symbolic) data parameters
- :math:`C : N_{\mathbb{B}} \to (\mathcal{N}_A \cup \{\tau\})` — a communication function, satisfying :math:`\forall_{\langle b,a\rangle, \langle c,a\rangle \in C}\,(\forall_{n \in b}\,(n \notin c))`

The χ Function
--------------

**Definition** (χ). Let :math:`m \in \mathbb{B}(\boldsymbol{A})` and :math:`a \in \mathcal{N}_A`.
Also, let :math:`\vec{d}, \vec{e} \in \vec{D}`.
The function :math:`\chi : \mathbb{B}(\boldsymbol{A}) \times \vec{D} \to \mathbb{B}` is true if,
and only if, all actions of the multiaction parameter have the given data vector as parameter, i.e.
:math:`\chi` is defined as follows:

.. math::

   \begin{array}{lll}
   \chi([], \vec{d}) &=& \mathbf{t} \\[2pt]
   \chi([a(\vec{e})] \oplus m, \vec{d}) &=& \chi(m, \vec{d}) \qquad \text{if } \vec{d} = \vec{e} \\[2pt]
   \chi([a(\vec{e})] \oplus m, \vec{d}) &=& \mathbf{f} \qquad\quad \text{if } \vec{d} \neq \vec{e}
   \end{array}

The Communication Function γ
-----------------------------

**Definition** (γ). Let :math:`a(\vec{d}) \in \boldsymbol{A}`, :math:`b \in N_{\mathbb{B}}` and
:math:`m, n, o \in \mathbb{B}(\boldsymbol{A})`.
Also let :math:`C : N_{\mathbb{B}} \to (\mathcal{N}_A \cup \{\tau\})` with
:math:`\forall_{\langle b,a\rangle, \langle c,a\rangle \in C}\,(\forall_{n \in b}\,(n \notin c))`.
The *communication* function :math:`\gamma : \mathbb{B}(\boldsymbol{A}) \times (N_{\mathbb{B}} \to (\mathcal{N}_A \cup \{\tau\})) \to \mathbb{B}(\boldsymbol{A})` is defined by the following definition:

.. math::

   \begin{array}{llll}
   \gamma(m \oplus n, C) &= [a(\vec{d})] \oplus \gamma(n, C)
     && \text{if } \exists_{\langle b,a\rangle \in C}\,(b = \mu(m) \wedge \chi(m, \vec{d})) \\[4pt]
   \gamma(m \oplus n, C) &= \gamma(n, C)
     && \text{if } \exists_{\langle b,\tau\rangle \in C}\,(b = \mu(m) \wedge \chi(m, \vec{d})) \\[4pt]
   \gamma(m, C) &= m
     && \text{if } \neg\exists_{n,o}\bigl(m = n \oplus o \;\wedge\; \exists_{c \in C}\bigl((c = \langle b,a\rangle \vee c = \langle b,\tau\rangle) \;\wedge\; b = \mu(n) \;\wedge\; \exists_{\vec{d} \in \vec{D}}\,\chi(n, \vec{d})\bigr)\bigr)
   \end{array}

But first we reformulate :math:`\gamma` to :math:`\gamma'` as follows, because the definition
of :math:`\gamma` above is not really suitable from an implementation point of view. Note that we
somewhat ignore the possibility of right hand sides that are :math:`\tau`, but this is not directly
relevant for the algorithms. If one desires, one can consider :math:`[\tau(\vec{d})]` to be equal
to :math:`[]` to make things fit.

**Definition** (γ'). Let :math:`a(\vec{d}) \in \boldsymbol{A}`, :math:`b \in N_{\mathbb{B}}` and
:math:`m, n, o \in \mathbb{B}(\boldsymbol{A})`. Also let :math:`C : N_{\mathbb{B}} \to (\mathcal{N}_A \cup \{\tau\})` with
:math:`\forall_{\langle b,a\rangle, \langle c,a\rangle \in C}\,(\forall_{n \in b}\,(n \notin c))`.
The *communication* function :math:`\gamma : \mathbb{B}(\boldsymbol{A}) \times (N_{\mathbb{B}} \to (\mathcal{N}_A \cup \{\tau\})) \to \mathbb{B}(\boldsymbol{A})` is defined by the following definition:

.. math::

   \begin{array}{llll}
   \gamma'([], C) &= [] \\[4pt]
   \gamma'([a(\vec{d})] \oplus m, C) &= [a(\vec{d})] \oplus \gamma'(m, C)
     && \text{if } \neg\exists_{n,o,\langle b,c\rangle \in C}\bigl(m = n \oplus o
        \;\wedge\; b = \mu([a(\vec{d})] \oplus n) \;\wedge\; \chi(n, \vec{d})\bigr) \\[4pt]
   \gamma'([a(\vec{d})] \oplus m, C) &= [c(\vec{d})] \oplus \gamma'(o, C)
     && \text{if } \exists_{n,\langle b,c\rangle \in C}\bigl(m = n \oplus o
        \;\wedge\; b = \mu([a(\vec{d})] \oplus n) \;\wedge\; \chi(n, \vec{d})\bigr)
   \end{array}

**Lemma**. Definitions γ and γ' define equivalent functions. That is, :math:`\gamma(m, C) = \gamma'(m, C)`, for all :math:`m` and :math:`C`.

**Proof**. The defining equations of :math:`\gamma'` are complete, so we only need to show that :math:`\gamma'` is sound (with respect to :math:`\gamma`). We do this by induction on :math:`m`.

*Case* :math:`m = []`:

.. math::

   \gamma'([], C) = [] = \gamma([], C)

*Case* :math:`[a(\vec{d})] \oplus m`. We do case distinction on the possibility of :math:`a(\vec{d})` to participate in a communication.

Case :math:`\exists_{n,\langle b,a\rangle \in C}\bigl(m = n \oplus o \;\wedge\; b = \mu([a(\vec{d})] \oplus n) \;\wedge\; \chi(n, \vec{d})\bigr)`:

.. math::

   \begin{array}{ll}
   & \gamma'([a(\vec{d})] \oplus m, C) \\
   = & [a(\vec{d})] \oplus \gamma'(o, C) \\
   = & [c(\vec{d})] \oplus \gamma(o, C) \\
   = & \gamma(([a(\vec{d})] \oplus n) \oplus o, C) \\
   = & \gamma([a(\vec{d})] \oplus (n \oplus o), C) \\
   = & \gamma([a(\vec{d})] \oplus m, C)
   \end{array}

Case :math:`\neg\exists_{n,o,\langle b,a\rangle \in C}\bigl(m = n \oplus o \;\wedge\; b = \mu([a(\vec{d})] \oplus n) \;\wedge\; \chi(n, \vec{d})\bigr)`,
with :math:`m'` and :math:`m''` such that :math:`\gamma(m,C) = m'' \oplus \gamma(m',C)` and :math:`\gamma(m',C) = m'`:

.. math::

   \begin{array}{ll}
   & \gamma'([a(\vec{d})] \oplus m, C) \\
   = & [a(\vec{d})] \oplus \gamma'(m, C) \\
   = & [a(\vec{d})] \oplus \gamma(m, C) \\
   = & [a(\vec{d})] \oplus m'' \oplus \gamma(m', C) \\
   = & [a(\vec{d})] \oplus m'' \oplus m' \\
   = & m'' \oplus [a(\vec{d})] \oplus m' \\
   = & m'' \oplus \gamma([a(\vec{d})] \oplus m', C) \\
   = & \gamma([a(\vec{d})] \oplus m, C) \qquad \square
   \end{array}

Extension to Open Terms
-----------------------

Taking as basis the new definition, we now define the function we are really interested in.
That is, the communication function on open terms. We use the set :math:`T_{\mathbb{B}}` of
(open) boolean terms and assume that expressions depending on action arguments :math:`\vec{d}`
are such terms.

Let :math:`\mathbb{B}(\boldsymbol{A'})` be the set of bags of actions with open data parameters.

**Definition** (:math:`\overline{\gamma}`). The extension of the communication operator over open data terms,

.. math::

   \overline{\gamma} : \mathbb{B}(\boldsymbol{A'}) \times (N_{\mathbb{B}} \to (\mathcal{N}_A \cup \{\tau\}))
     \to \mathcal{P}(\mathbb{B}(\boldsymbol{A'}) \times T_{\mathbb{B}}),

is defined as:

.. math::
   :nowrap:
   :class: math-left

   \begin{align*}
   \overline{\gamma}([], C) &= \{ \langle [], \mathbf{true} \rangle \} \\[6pt]
   \overline{\gamma}([a(\vec{d})] \oplus m, C) &=
     \bigl\{\, \langle r, e \rangle \;\bigm|\;
       \exists_{n,\, o,\, \langle b,c\rangle \in C,\, \langle r',e'\rangle \in \overline{\gamma}(o,C)}\bigl(
         m = n \oplus o
         \;\wedge\; b = \mu([a(\vec{d})] \oplus n) \\
   &\hspace{45mm} \wedge\; e = \chi(n, \vec{d}) \wedge e'
         \;\wedge\; r = [c(\vec{d})] \oplus r' \bigr)
     \,\bigr\} \\
   &\quad\cup\; \bigl\{\, \langle [a(\vec{d})] \oplus r,\; e \wedge
       \neg\exists_{n,o,\langle b,c\rangle \in C}\bigl(m = n \oplus o
         \;\wedge\; b = \mu([a(\vec{d})] \oplus n) \;\wedge\; \chi(n, \vec{d})\bigr)
       \rangle \\
   &\hspace{20mm} \bigm|\; \langle r,e\rangle \in \overline{\gamma}(m,C) \,\bigr\}
   \end{align*}

**Theorem**. Let :math:`m \in \mathbb{B}(\boldsymbol{A'})` and :math:`\sigma` an assignment
of variables to closed terms. Then:

.. math::

   \forall_{\langle r,e\rangle \in \overline{\gamma}(m,C)}\,(e\sigma \;\Rightarrow\; r\sigma = \gamma(m\sigma, C))

Simplification
--------------

Note that we can rewrite :math:`\neg\exists_{n,o,\langle b,c\rangle \in C}(m = n \oplus o \wedge b = \mu([a(\vec{d})] \oplus n) \wedge \chi(n,\vec{d}))` as follows:

.. math::

   \begin{array}{ll}
   & \neg\exists_{n,o,\langle b,c\rangle \in C}\bigl(m = n \oplus o \;\wedge\; b = \mu([a(\vec{d})] \oplus n) \;\wedge\; \chi(n,\vec{d})\bigr) \\[4pt]
   \equiv & \forall_{n,o,\langle b,c\rangle \in C}\,\neg\bigl(m = n \oplus o \;\wedge\; b = \mu([a(\vec{d})] \oplus n) \;\wedge\; \chi(n,\vec{d})\bigr) \\[4pt]
   \equiv & \forall_{n,o,\langle b,c\rangle \in C}\,\bigl(\neg(m = n \oplus o \;\wedge\; b = \mu([a(\vec{d})] \oplus n)) \;\vee\; \neg\chi(n,\vec{d})\bigr) \\[4pt]
   \equiv & \forall_{n,o,\langle b,c\rangle \in C}\,\bigl((m = n \oplus o \;\wedge\; b = \mu([a(\vec{d})] \oplus n)) \;\Rightarrow\; \neg\chi(n,\vec{d})\bigr)
   \end{array}

**Definition** (:math:`\overline{\gamma}'`).

.. math::
   :nowrap:
   :class: math-left

   \begin{align*}
   \overline{\gamma}'([], C) &= \{ \langle [], \mathbf{true} \rangle \} \\[6pt]
   \overline{\gamma}'([a(\vec{d})] \oplus m, C) &=
     \bigl\{\, \langle r, e \rangle \;\bigm|\;
       \exists_{n,\, o,\, \langle b,c\rangle \in C,\, \langle r',e'\rangle \in \overline{\gamma}(o,C)}\bigl(
         m = n \oplus o
         \;\wedge\; b = \mu([a(\vec{d})] \oplus n) \\
   &\hspace{45mm} \wedge\; e = \chi(n, \vec{d}) \wedge e'
         \;\wedge\; r = [c(\vec{d})] \oplus r' \bigr)
     \,\bigr\}
   \end{align*}

**Lemma**.

.. math::
   :nowrap:
   :class: math-left

   \begin{align*}
   \overline{\gamma}([a(\vec{d})] \oplus m, C)
     &= \overline{\gamma}'([a(\vec{d})] \oplus m, C) \\
     &\quad\cup\; \bigl\{\, \langle [a(\vec{d})] \oplus r,\;
         e \wedge \forall_{\langle r',e'\rangle \in \overline{\gamma}'([a(\vec{d})] \oplus m, C)}(\neg e') \rangle
       \;\bigm|\; \langle r,e\rangle \in \overline{\gamma}(m,C) \,\bigr\}
   \end{align*}

The ϕ Helper Function
---------------------

We now concentrate on :math:`\overline{\gamma}'`.

**Definition** (φ).

.. math::
   :nowrap:
   :class: math-left

   \begin{align*}
   \phi(m, \vec{d}, w, n, C) = \bigl\{\, \langle r, e \rangle \;\bigm|\;
     &\;\exists_{o,\, o',\, \langle b,c\rangle \in C,\, \langle r',e'\rangle \in \overline{\gamma}(o' \oplus w, C)}
     \bigl( n = o \oplus o'
       \;\wedge\; b = \mu(m \oplus o) \\
     &\hspace{8mm} \wedge\; e = \chi(o, \vec{d}) \wedge e'
       \;\wedge\; r = [c(\vec{d})] \oplus r' \bigr)
   \,\bigr\}
   \end{align*}

**Lemma**. :math:`\overline{\gamma}'([a(\vec{d})] \oplus m, C) = \phi([a(\vec{d})], \vec{d}, [], m, C)`.

And finally with :math:`\phi`:

.. math::

   \begin{array}{ll}
   & \phi(m, \vec{d}, w, [], C) \\[4pt]
   = & \bigl\{\, \langle r, e \rangle \;\bigm|\; \exists_{o,\, o',\, \langle b,c\rangle \in C,\, \langle r',e'\rangle \in \overline{\gamma}(o' \oplus w, C)}\bigl( [] = o \oplus o' \;\wedge\; b = \mu(m \oplus o) \;\wedge\; e = \chi(o,\vec{d}) \wedge e' \;\wedge\; r = [c(\vec{d})] \oplus r' \bigr) \,\bigr\} \\[4pt]
   = & \bigl\{\, \langle r, e \rangle \;\bigm|\; \exists_{\langle b,c\rangle \in C,\, \langle r',e'\rangle \in \overline{\gamma}([] \oplus w, C)}\bigl( b = \mu(m \oplus []) \;\wedge\; e = \chi([],\vec{d}) \wedge e' \;\wedge\; r = [c(\vec{d})] \oplus r' \bigr) \,\bigr\} \\[4pt]
   = & \bigl\{\, \langle r, e \rangle \;\bigm|\; \exists_{\langle b,c\rangle \in C,\, \langle r',e'\rangle \in \overline{\gamma}(w, C)}\bigl( b = \mu(m) \;\wedge\; e = e' \;\wedge\; r = [c(\vec{d})] \oplus r' \bigr) \,\bigr\} \\[4pt]
   = & \bigl\{\, \langle [c(\vec{d})] \oplus r', e' \rangle \;\bigm|\; \exists_{\langle b,c\rangle \in C}\,(b = \mu(m)) \;\wedge\; \langle r',e'\rangle \in \overline{\gamma}(w,C) \,\bigr\}
   \end{array}

.. math::

   \begin{array}{ll}
   & \phi(m, \vec{d}, w, [a(\vec{f})] \oplus n, C) \\[4pt]
   = & \bigl\{\, \langle r, e \rangle \;\bigm|\; \exists_{o,\, o',\, \langle b,c\rangle \in C,\, \langle r',e'\rangle \in \overline{\gamma}(o', C)}\bigl( [a(\vec{f})] \oplus n = o \oplus o' \;\wedge\; b = \mu(m \oplus o) \;\wedge\; e = \chi(o,\vec{d}) \wedge e' \;\wedge\; r = [c(\vec{d})] \oplus r' \bigr) \,\bigr\}
   \end{array}

Here :math:`a(\vec{f})` can be in :math:`o` or in :math:`o'`. Assume it is in :math:`o`.

.. math::

   \begin{array}{ll}
   & \bigl\{\, \langle r, e \rangle \;\bigm|\; \exists_{o,\, o',\, \langle b,c\rangle \in C,\, \langle r',e'\rangle \in \overline{\gamma}(o' \oplus w, C)}\bigl( [a(\vec{f})] \oplus n = o \oplus o' \;\wedge\; b = \mu(m \oplus o) \;\wedge\; e = \chi(o,\vec{d}) \wedge e' \;\wedge\; r = [c(\vec{d})] \oplus r' \bigr) \,\bigr\} \\[4pt]
   = & \bigl\{\, \langle r, e \rangle \;\bigm|\; \exists_{o'',\, o',\, \langle b,c\rangle \in C,\, \langle r',e'\rangle \in \overline{\gamma}(o' \oplus w, C)}\bigl( n = o'' \oplus o' \;\wedge\; b = \mu(m \oplus [a(\vec{f})] \oplus o'') \\
   & \hspace{45mm} \wedge\; e = (\vec{f} = \vec{d}) \wedge \chi(o'',\vec{d}) \wedge e' \;\wedge\; r = [c(\vec{d})] \oplus r' \bigr) \,\bigr\} \\[4pt]
   = & \bigl\{\, \langle r, e \wedge (\vec{f} = \vec{d}) \rangle \;\bigm|\; \exists_{o'',\, o',\, \langle b,c\rangle \in C,\, \langle r',e'\rangle \in \overline{\gamma}(o' \oplus w, C)}\bigl( n = o'' \oplus o' \;\wedge\; b = \mu(m \oplus [a(\vec{f})] \oplus o'') \\
   & \hspace{45mm} \wedge\; e = \chi(o'',\vec{d}) \wedge e' \;\wedge\; r = [c(\vec{d})] \oplus r' \bigr) \,\bigr\} \\[4pt]
   = & \bigl\{\, \langle r, e \wedge (\vec{f} = \vec{d}) \rangle \;\bigm|\; \langle r,e\rangle \in \phi(m \oplus [a(\vec{f})], \vec{d}, w, n, C) \,\bigr\}
   \end{array}

Now assume it is in :math:`o'`.

.. math::

   \begin{array}{ll}
   & \bigl\{\, \langle r, e \rangle \;\bigm|\; \exists_{o,\, o',\, \langle b,c\rangle \in C,\, \langle r',e'\rangle \in \overline{\gamma}(o' \oplus w, C)}\bigl( [a(\vec{f})] \oplus n = o \oplus o' \;\wedge\; b = \mu(m \oplus o) \;\wedge\; e = \chi(o,\vec{d}) \wedge e' \;\wedge\; r = [c(\vec{d})] \oplus r' \bigr) \,\bigr\} \\[4pt]
   = & \bigl\{\, \langle r, e \rangle \;\bigm|\; \exists_{o,\, o',\, \langle b,c\rangle \in C,\, \langle r',e'\rangle \in \overline{\gamma}([a(\vec{f})] \oplus o' \oplus w, C)}\bigl( n = o \oplus o' \;\wedge\; b = \mu(m \oplus o) \;\wedge\; e = \chi(o,\vec{d}) \wedge e' \;\wedge\; r = [c(\vec{d})] \oplus r' \bigr) \,\bigr\} \\[4pt]
   = & \phi(m, \vec{d}, w \oplus [a(\vec{f})], n, C)
   \end{array}

To conclude:

.. math::
   :nowrap:
   :class: math-left

   \begin{align*}
   \phi(m, \vec{d}, w, [a(\vec{f})] \oplus n, C) =\;
     &\bigl\{\, \langle r, e \wedge (\vec{f} = \vec{d}) \rangle \;\bigm|\; \langle r,e\rangle \in \phi(m \oplus [a(\vec{f})], \vec{d}, w, n, C) \,\bigr\} \\
     &\cup\; \phi(m, \vec{d}, w \oplus [a(\vec{f})], n, C)
   \end{align*}

Algorithms
----------

To conclude, we write an algorithm that uses what we have proven.

**Algorithm**: :math:`\overline{\gamma}(m, C)`

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Function{$\overline{\gamma}$}{$m, C$}
     \If{$m = []$}
       \State \Return $\{ \langle [], \mathbf{true} \rangle \}$
     \Else \Comment{$m = [a(\vec{d})] \oplus n$}
       \State $S \gets \phi([a(\vec{d})], \vec{d}, [], n, C)$
       \State $T \gets \overline{\gamma}(n, C)$
       \State $b \gets \forall_{\langle r,e\rangle \in S}(\neg e)$
       \State $S \gets S \cup \{ \langle [a(\vec{d})] \oplus r,\; e \wedge b \rangle \mid \langle r,e\rangle \in T \}$
       \State \Return $S$
     \EndIf
   \EndFunction
   \end{algorithmic}

**Algorithm**: :math:`\phi(m, \vec{d}, w, n, C)`

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Function{$\phi$}{$m, \vec{d}, w, n, C$}
     \If{$n = []$}
       \If{$\exists_{\langle b,c\rangle \in C}\,(b = \mu(m))$}
         \State $T \gets \overline{\gamma}(w, C)$
         \State \Return $\{ \langle [c(\vec{d})] \oplus t,\; e \rangle \mid \langle t,e\rangle \in T \}$
       \Else
         \State \Return $\emptyset$
       \EndIf
     \Else \Comment{$n = [a(\vec{f})] \oplus o$}
       \State $T \gets \phi(m \oplus [a(\vec{f})], \vec{d}, w, o, C)$
       \State $T \gets \{ \langle r,\; e \wedge (\vec{f} = \vec{d}) \rangle \mid \langle r,e\rangle \in T \}$
       \State \Return $T \cup \phi(m, \vec{d}, w \oplus [a(\vec{f})], o, C)$
     \EndIf
   \EndFunction
   \end{algorithmic}

Complexity and Optimisation
---------------------------

If we analyse this algorithm focussing on the length of input :math:`m`, we see that it is
:math:`O(2^{|m|})`. It basically takes the first action in :math:`m` and computes the result
given that this action participates in a communication and the result given that it does not.

However, looking at :math:`\phi`, we can see that the algorithm needlessly tries to find a part
in :math:`n`, such that :math:`m` with this part can communicate, if :math:`m` is not even a
subbag of a left hand side of a communication in :math:`C`. So, we propose to add an extra
check to :math:`\phi` to prevent this behaviour and making the algorithm more (or precisely) in
the order of :math:`O(2^{|m_1|} + |m_2|)`, with :math:`m = m_1 \oplus m_2` and :math:`m_1`
contains actions that occur in a left hand side of a communication in :math:`C` and :math:`m_2`
actions that do not.

**Algorithm**: :math:`\phi(m, \vec{d}, w, n, C)` (optimised)

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Function{$\phi$}{$m, \vec{d}, w, n, C$}
     \If{$\neg\exists_{o,c}\,(\langle \mu(m) \oplus o, c \rangle \in C)$}
       \State \Return $\emptyset$
     \ElsIf{$n = []$}
       \If{$\exists_{\langle b,c\rangle \in C}\,(b = \mu(m))$}
         \State $T \gets \overline{\gamma}(w, C)$
         \State \Return $\{ \langle [c(\vec{d})] \oplus t,\; e \rangle \mid \langle t,e\rangle \in T \}$
       \Else
         \State \Return $\emptyset$
       \EndIf
     \Else \Comment{$n = [a(\vec{f})] \oplus o$}
       \State $T \gets \phi(m \oplus [a(\vec{f})], \vec{d}, w, o, C)$
       \State $T \gets \{ \langle r,\; e \wedge (\vec{f} = \vec{d}) \rangle \mid \langle r,e\rangle \in T \}$
       \State \Return $T \cup \phi(m, \vec{d}, w \oplus [a(\vec{f})], o, C)$
     \EndIf
   \EndFunction
   \end{algorithmic}

Extended Algorithms with Accumulator
-------------------------------------

Another problem with the above code is that it can generate a lot of negative conditions to
indicate that certain communications do not happen. This appears to be at least exponential.

We solve this by removing the problematic :math:`\forall` in :math:`\overline{\gamma}`. Instead
we add an extra parameter to :math:`\overline{\gamma}` and :math:`\phi` indicating which actions
will not communicate. Then, in the final case of :math:`\overline{\gamma}`, where :math:`m = []`,
we use a new function :math:`\psi` to calculate a more reasonable condition indicating that the
remaining actions do not communicate.

Note that the following algorithm deviates in a significant way from the previous version, which
means that its validity is not guaranteed and additional proofs will be needed.

**Algorithm**: :math:`\overline{\gamma}(m, C, r)`

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Function{$\overline{\gamma}$}{$m, C, r$}
     \If{$m = []$}
       \State \Return $\{ \langle r,\; \psi(r, C) \rangle \}$
     \Else \Comment{$m = [a(\vec{d})] \oplus n$}
       \State $S \gets \phi([a(\vec{d})], \vec{d}, [], n, C, r)$
       \State $T \gets \overline{\gamma}(n, C, [a(\vec{d})] \oplus r)$
       \State \Return $S \cup T$
     \EndIf
   \EndFunction
   \end{algorithmic}

**Algorithm**: :math:`\phi(m, \vec{d}, w, n, C, r)`

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Function{$\phi$}{$m, \vec{d}, w, n, C, r$}
     \If{$\neg\exists_{o,c}\,(\langle \mu(m) \oplus o, c \rangle \in C)$}
       \State \Return $\emptyset$
     \ElsIf{$n = []$}
       \If{$\exists_{\langle b,c\rangle \in C}\,(b = \mu(m))$}
         \State $T \gets \overline{\gamma}(w, C, r)$
         \State \Return $\{ \langle [c(\vec{d})] \oplus t,\; e \rangle \mid \langle t,e\rangle \in T \}$
       \Else
         \State \Return $\emptyset$
       \EndIf
     \Else \Comment{$n = [a(\vec{f})] \oplus o$}
       \State $T \gets \phi(m \oplus [a(\vec{f})], \vec{d}, w, o, C, r)$
       \State $T \gets \{ \langle s,\; e \wedge (\vec{f} = \vec{d}) \rangle \mid \langle s,e\rangle \in T \}$
       \State \Return $T \cup \phi(m, \vec{d}, w \oplus [a(\vec{f})], o, C, r)$
     \EndIf
   \EndFunction
   \end{algorithmic}

The ψ Functions
---------------

**Algorithm**: :math:`\psi(m, C)` (recursive)

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Function{$\psi$}{$m, C$}
     \If{$m = []$}
       \State \Return $\mathbf{true}$
     \Else \Comment{$m = [a(\vec{d})] \oplus n$}
       \State \Return $\psi'(a(\vec{d}), n, C) \wedge \psi(n, C)$
     \EndIf
   \EndFunction
   \end{algorithmic}

**Algorithm**: :math:`\psi'(a(\vec{d}), m, C)` (recursive)

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Function{$\psi'$}{$a(\vec{d}), m, C$}
     \If{$m = []$}
       \State \Return $\mathbf{true}$
     \Else \Comment{$m = [b(\vec{e})] \oplus n$}
       \State $c \gets \exists_{o,k}\,(\langle [a, b] \oplus o,\; k \rangle \in C)$
       \If{$c \wedge \xi([a(\vec{d}), b(\vec{e})], n, C)$}
         \State \Return $\psi'(a(\vec{d}), n, C) \wedge (\vec{d} \neq \vec{e})$
       \Else
         \State \Return $\psi'(a(\vec{d}), n, C)$
       \EndIf
     \EndIf
   \EndFunction
   \end{algorithmic}

**Algorithm**: :math:`\xi(m, n, C)`

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Function{$\xi$}{$m, n, C$}
     \If{$n = []$}
       \State \Return $\exists_k\,(\langle m,\; k \rangle \in C)$
     \Else \Comment{$n = [a(\vec{d})] \oplus o$}
       \If{$\exists_k\,(\langle [a] \oplus m,\; k \rangle \in C)$}
         \State \Return $\mathbf{true}$
       \ElsIf{$\exists_{b,o',k}\,(\langle [a,b] \oplus m \oplus o',\; k \rangle \in C)$}
         \State \Return $\xi([a] \oplus m, o, C) \vee \xi(m, o, C)$
       \Else
         \State \Return $\xi(m, o, C)$
       \EndIf
     \EndIf
   \EndFunction
   \end{algorithmic}

Non-recursive Implementations
------------------------------

Naturally, functions :math:`\psi` and :math:`\psi'` can easily be transformed to the following
non-recursive implementation.

**Algorithm**: :math:`\psi(m, C)` (iterative)

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Function{$\psi$}{$m, C$}
     \State $b \gets \mathbf{true}$
     \While{$m = [a(\vec{d})] \oplus n$}
       \State $b \gets b \wedge \psi'(a(\vec{d}), n, C)$
       \State $m \gets n$
     \EndWhile
     \State \Return $b$
   \EndFunction
   \end{algorithmic}

**Algorithm**: :math:`\psi'(a(\vec{d}), m, C)` (iterative)

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Function{$\psi'$}{$a(\vec{d}), m, C$}
     \State $b \gets \mathbf{true}$
     \While{$m = [b(\vec{e})] \oplus n$}
       \State $c \gets \exists_{o,k}\,(\langle [a,b] \oplus o,\; k \rangle \in C)$
       \If{$c \wedge \xi([a(\vec{d}), b(\vec{e})], n, C)$}
         \State $b \gets b \wedge (\vec{d} \neq \vec{e})$
       \EndIf
       \State $m \gets n$
     \EndWhile
     \State \Return $b$
   \EndFunction
   \end{algorithmic}

Correctness
-----------

**Theorem**.

.. math::

   \overline{\gamma}(m, C, r) = \{ \langle r \oplus r',\; e \wedge \psi(r, C) \rangle \mid \langle r',e\rangle \in \overline{\gamma}(m, C) \}

**Proof**. By induction on :math:`m`.

*Case* :math:`m = []`:

.. math::

   \begin{array}{ll}
   & \{ \langle r \oplus r',\; e \wedge \psi(r,C) \rangle \mid \langle r',e\rangle \in \overline{\gamma}([],C) \} \\
   = & \{ \langle r \oplus [],\; \mathbf{true} \wedge \psi(r,C) \rangle \} \\
   = & \{ \langle r,\; \psi(r,C) \rangle \} \\
   = & \overline{\gamma}([],C,r)
   \end{array}

*Case* :math:`m = [a(\vec{d})] \oplus m'`:

.. math::
   :nowrap:
   :class: math-left

   \begin{align*}
   &\{ \langle r \oplus r',\; e \wedge \psi(r,C) \rangle \mid \langle r',e\rangle \in \overline{\gamma}([a(\vec{d})] \oplus m', C) \} \\
   &= \{ \langle r \oplus r',\; e \wedge \psi(r,C) \rangle \mid \langle r',e\rangle \in \overline{\gamma}'([a(\vec{d})] \oplus m',C) \cup \{ \langle [a(\vec{d})] \oplus s,\; e \wedge \forall_{\langle s',e'\rangle \in \overline{\gamma}'([a(\vec{d})] \oplus m',C)}(\neg e') \rangle \mid \langle s,e\rangle \in \overline{\gamma}(m',C) \} \} \\
   &= \{ \langle r \oplus r',\; e \wedge \psi(r,C) \rangle \mid \langle r',e\rangle \in \phi([a(\vec{d})],\vec{d},[],m',C) \cup \{ \langle [a(\vec{d})] \oplus s,\; e \wedge \forall_{\langle s',e'\rangle \in \phi([a(\vec{d})],\vec{d},[],m',C)}(\neg e') \rangle \mid \langle s,e\rangle \in \overline{\gamma}(m',C) \} \} \\
   &= \phi([a(\vec{d})],\vec{d},[],m',C,r) \cup \{ \langle r \oplus [a(\vec{d})] \oplus s,\; e \wedge \forall_{\langle s',e'\rangle \in \phi([a(\vec{d})],\vec{d},[],m',C)}(\neg e') \wedge \psi(r,C) \rangle \mid \langle s,e\rangle \in \overline{\gamma}(m',C) \} \\
   &= \phi([a(\vec{d})],\vec{d},[],m',C,r) \cup \{ \langle [a(\vec{d})] \oplus r \oplus s,\; e \wedge \psi([a(\vec{d})] \oplus r,C) \rangle \mid \langle s,e\rangle \in \overline{\gamma}(m',C) \} \\
   &= \phi([a(\vec{d})], \vec{d}, [], m', C, r) \;\cup\; \overline{\gamma}(m', C, [a(\vec{d})] \oplus r) \\
   &= \overline{\gamma}([a(\vec{d})] \oplus m', C, r) \qquad \square
   \end{align*}

**Corollary**. :math:`\overline{\gamma}(m, C) = \overline{\gamma}(m, C, [])`.

**Proof**.

.. math::

   \begin{array}{ll}
   & \overline{\gamma}(m, C, []) \\
   = & \{ \langle [] \oplus r',\; e \wedge \psi([],C) \rangle \mid \langle r',e\rangle \in \overline{\gamma}(m,C) \} \\
   = & \{ \langle r',\; e \wedge \mathbf{true} \rangle \mid \langle r',e\rangle \in \overline{\gamma}(m,C) \} \\
   = & \overline{\gamma}(m, C) \qquad \square
   \end{array}

References
----------

.. [vW04] M.J. van Weerdenburg. *GenSpect Process Algebra*. Master's thesis,
   Eindhoven University of Technology, 2004.
