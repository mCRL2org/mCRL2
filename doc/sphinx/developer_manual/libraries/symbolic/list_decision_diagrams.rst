List Decision Diagrams
----------------------

It can be compactly represented using an LDD, see the figure below. For our
applications we use the LDD implementation that is part of the Sylvan multi-core
framework for decision diagrams, see [DijkP17]_.

.. figure:: latex/ldd_if_then.png
	:width: 15cm

For an LDD :math:`A` we use :math:`\mathit{down}(A)` to denote
:math:`A[x_i = v]` and :math:`\mathit{right}(A)` to denote :math:`A[x_i > v]`.
We use :math:`|A|` to denote the size of the LDD, determined by the number of
nodes. Given a vector :math:`v = x_0\,x_1 \cdots x_n` we define its length,
denoted by :math:`|v|`, as :math:`n + 1`.
Note that an LDD can represent (some) sets where two vectors have different
lengths. For example the set :math:`\{1\,1, 0\}` can be represented by an
LDD. However, the set :math:`\{1\,1, 1\}` cannot be represented since the root
node can either have :math:`\textsf{true}` or
:math:`\mathit{node}(1, \textsf{true}, \textsf{false})` as the down node and
:math:`\textsf{true}` has no down or right nodes.
In general, we cannot represent a set with vectors that are strict prefixes (any
vector :math:`x_0 \cdots x_m` with :math:`m < n` is a strict prefix of
:math:`v`) of other vectors in the set.
In practice, this means that we only consider LDDs where every vector in the
represented set has the same length. We refer to this length as the height of
the LDD.
We often require that the input LDDs have equal height since not every output
can be represented. For example the union of :math:`\{1\,1\}` and
:math:`\{1\}` cannot be represented as previously shown.

Union
~~~~~

We define the union operator on two equal height LDDs :math:`A` and :math:`B`.
This computes the union of the represented sets of vectors.

.. rst-class:: mcrl2_pseudocode

   .. math::
	  :nowrap:
	  :label: ldd_union_algorithm

	  \begin{algorithmic}[1]
	  \Function{union}{$A, B$}
	  \If{$A = B$}
		  \State \Return $a$
	  \ElsIf{$A = \textsf{false}$}
		  \State \Return $b$
	  \ElsIf{$B = \textsf{false}$}
		  \State \Return $a$
	  \EndIf

	  \If{$val(A) < val(B)$}
		  \State \Return $node(val(A), down(A), \textsc{union}(right(A), B))$
	  \ElsIf{$val(A) = val(B)$}
		  \State \Return $node(val(A), \textsc{union}(down(A), down(B)), \textsc{union}(\mathit{right}(A), right(B)))$
	  \ElsIf{$val(A) > val(B)$}
		  \State \Return $node(val(B), down(B), \textsc{union}(A, right(B)))$
	  \EndIf
	  \EndFunction
	  \end{algorithmic}

**Lemma.**
For all LDDs :math:`A` and :math:`B` it holds that
:math:`\sem{ \textsc{union}(A, B) } = \sem{ A } \cup \sem{ B }`.

**Proof.**
Pick arbitrary LDDs :math:`A` and :math:`B`. Proof by induction on the
structure of :math:`A` and :math:`B`.
For all LDDs :math:`A'` and :math:`B'` we assume that
:math:`\sem{ \textsc{union}(A', right(B')) } = \sem{ A' } \cup \sem{ right(B') }`
and
:math:`\sem{ \textsc{union}(A', down(B')) } = \sem{ A' } \cup \sem{ down(B') }`.
Furthermore,
:math:`\sem{ \textsc{union}(\mathit{right}(A'), B') } = \sem{ \mathit{right}(A') } \cup \sem{ B' }`
and
:math:`\sem{ \textsc{union}(down(A'), B') } = \sem{ \mathit{down}(A') } \cup \sem{ B' }`.

Base case.
The LDD :math:`A` is either :math:`\textsf{true}` or :math:`\textsf{false}`.
Then :math:`B` is either :math:`\textsf{true}` or :math:`\textsf{false}` due to
the equal height assumption. In both cases t he terminal conditions ensure
correctness. For example
:math:`\sem{ \textsc{union}(\textsf{true}, \textsf{false}) } = \sem{ \textsf{true} }`
and :math:`\{[]\} \cup \emptyset = \{[]\}`. Similarly, for the case where
:math:`B` is either :math:`\textsf{true}` or :math:`\textsf{false}`.

Inductive step.

* Case :math:`val(A) < val(B)`.

  Since :math:`A` is an LDD we know that
  :math:`\mathit{val}(A) < \mathit{val}(\mathit{right}(A))`. Therefore, we know
  that
  :math:`\sem{ node(val(A), down(A), \textsc{union}(right(A), B)) }`
  is equal to
  :math:`\{val(A)\,x \mid x \in \sem{ down(A) }\} \cup \sem{ \textsc{union}(right(A), B) }`.
  It follows that
  :math:`\sem{ \textsc{union}(right(A), B) }` is equal to
  :math:`\sem{ right(A) } \cup \sem{ B }`.
  From which we can derive
  :math:`\sem{ A } \cup \sem{ B }`.

* Case :math:`val(A) = val(B)`.

  Since :math:`A` is an LDD we know that
  :math:`\mathit{val}(A) < \mathit{val}(\mathit{right}(A))` and similarly
  because :math:`B` is an LDD we know that
  :math:`\mathit{val}(A) < \mathit{val}(\mathit{right}(B))`.
  Therefore, the following node is valid and
  :math:`\sem{ node(val(A), \textsc{union}(down(A), down(B)), \textsc{union}(right(A), \mathit{right}(B))) }`
  is equal to
  :math:`\{val(A)\,x \mid x \in \sem{ \textsc{union}(down(A), down(B)) } \}`:math:`\cup \sem{ \textsc{union}(right(A), right(B)) }` .
  It follows that the interpretation
  :math:`\{val(A)\,x \mid x \in \sem{ \textsc{union}(down(A), down(B)) }\}`
  is equal to
  :math:`\{val(A)\,x \mid x \in \sem{ down(A) }\} \cup \{val(A)\,x \mid x \in \sem{ down(B) }\}`
  and
  :math:`\sem{ \textsc{union}(right(A), right(B)) }` is equal to
  :math:`\sem{ (right(A) } \cup \sem{ right(B)) }`.

* Case :math:`val(A) > val(B)`.

  Similar to the :math:`val(A) < val(B)` case.

We can show that :math:`|\textsc{union}(A, B)|` for any LDDs :math:`A` and
:math:`B` is at most :math:`|A| + |B|`.
The time complexity of :math:`\textsc{union}(A, B)` is also of order
:math:`\mathcal{O}(|A| + |B|)`.

Project
~~~~~~~

Given a vector :math:`x_0\,\cdots\,x_n` and a subset
:math:`I \subseteq \mathbb{N}`, we define the *projection*, denoted by
:math:`\mathit{project}(x_0\,\cdots\,x_n, I)`, as the vector
:math:`x_{i_0}, \ldots, x_{i_l}` for the largest :math:`l \in \mathbb{N}` such
that :math:`i_0 < i_1 < \ldots < i_l \leq n` and :math:`i_k \in I` for
:math:`0 \leq k \leq l`.
We define the projection operator for an LDD where every vector in the set is
projected. For the LDD operator it is more convenient to specify the indices
:math:`I \subseteq \mathbb{N}` by a vector :math:`x_0\,\cdots\,x_n` such that
for :math:`0 \leq i \leq n` variable :math:`x_i` is one iff :math:`i \in I`.
The operator takes an LDD :math:`A` of height :math:`n` and a sequence of
numbers :math:`x_0\,x_1,\cdots\,x_n`.

.. rst-class:: mcrl2_pseudocode

   .. math::
	  :nowrap:
	  :label: ldd_project_algorithm

	  \begin{algorithmic}[1]
	  \Function{project}{$A, x_0\,x_1\,\cdots\,x_n$}
		  \If{$A = \textsf{true}$}
			  \State \Return $\textsf{true}$
		  \ElsIf{$A = \textsf{false}$}
			  \State \Return $\textsf{false}$
		  \EndIf

		  \If{$x_0 = 1$}
			  \State \Return{$\mathit{node}(\mathit{val}(A), \textsc{project}(\mathit{down}(A), x_1\,\cdots\,x_n), \textsc{project}(\mathit{right}(A), x_0\,\cdots\,x_n))$}
		  \ElsIf{$x_0 = 0$}
			  \State $a \gets A$
			  \State $R \gets \textsf{false}$
			  \While {$a \neq \textsf{false}$}
				  \State $R \gets \textsc{union}(R, \textsc{project}(\mathit{down}(a), x_1\,\cdots\,x_n))$
				  \State $a \gets \mathit{right}(a)$
			  \EndWhile
			  \State \Return {$R$}
		  \EndIf
	  \EndFunction
	  \end{algorithmic}

**Lemma.**
For all LDDs :math:`A` and sequences :math:`x_0\,x_1\,\cdots\,x_n` it holds
that :math:`\sem{ \textsc{project}(A, x_0\,\cdots\,x_n) }` is
equal to
:math:`\{\mathit{project}(v, x_0\,\cdots\,x_n) \in \sem{ A }\}`.

Note that for a sequence of :math:`|A|` zeroes
:math:`\textsc{project}(A, 0\,\cdots\,0)` is equal to :math:`\textsf{true}` and
for a sequence of :math:`|A|` ones
:math:`\textsc{project}(A, 1\,\cdots\,1)` is equal to :math:`A`.

Caching
~~~~~~~

We can speed up LDD operations at the cost of memory by using an operation
cache. For every operation there will be a separate *global* cache,
represented by a set, that stores a tuple of the inputs and the output.
We use *global* to emphasize that every recursion sees the latest values stored
in the cache.
For example the :math:`\textsc{union}(A, B)` operation has a cache
:math:`C_\textsc{union}` and the pseudocode of :math:`\textsc{union}` is
changed such that after the terminal cases first we check whether
:math:`\exists R : (A, B, R) \in C_\textsc{union}` and return the result
:math:`R` if that is the case. Otherwise, we perform the computation as before
but store the result in :math:`C_\textsc{union}` instead before returning it.
Thus we obtain the following algorithm:

.. rst-class:: mcrl2_pseudocode

   .. math::
	  :nowrap:
	  :label: ldd_union_cache_algorithm

	  \begin{algorithmic}[1]
	  \Function{union}{$A, B,  C_\textsc{union}$}
	  \If{$A = B$}
		  \State \Return $a$
	  \ElsIf{$A = \textsf{false}$}
		  \State \Return $b$
	  \ElsIf{$B = \textsf{false}$}
		  \State \Return $a$
	  \EndIf

	  \If{$\exists R: (A, B, R) \in C_\textsc{union}$}
		  \Return $R$
	  \EndIf

	  \If{$val(A) < val(B)$}
		  \State $R \gets node(val(A), down(A), \textsc{union}(right(A), B, C_\textsc{union}))$
	  \ElsIf{$val(A) = val(B)$}
		  \State $R \gets node(val(A), union(down(A), down(B), union(right(A), right(B), C_\textsc{union}))$
	  \ElsIf{$val(A) > val(B)$}
		  \State $R \gets node(val(B), down(B), \textsc{union}(A, right(B), C_\textsc{union}))$
	  \EndIf

	  \State $C_\textsc{union} \gets C_\textsc{union} \cup \{(A, B, R)\}$
	  \State \Return $R$
	  \EndFunction
	  \end{algorithmic}

Parallelism
~~~~~~~~~~~

In some cases we can improve the performance further by computing several
results in parallel during the operation.
For example in the union of two LDDs :math:`A` and :math:`B` in the case
:math:`val(A) = val(B)` we could determine the result of both unions in
parallel and only merge them after they have finished.

References
~~~~~~~~~~

.. [DijkP17] Tom van Dijk and Jaco van de Pol.
	*Sylvan: multi-core framework for decision diagrams*.
	Int. J. Softw. Tools Technol. Transf. 19(6):675-696, 2017.
	DOI: 10.1007/s10009-016-0433-2.
