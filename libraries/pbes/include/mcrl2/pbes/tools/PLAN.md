# PBESCEGAR

Just like in pbesstategraph, we first want to create a control flow graph. Assume a global control flow graph.

We want to partially instantiate this graph: the values of the control flow parameters should be filled into the equation. I first of all want to know the following data for each node in the graph:
- Which outgoing edges hold after underapproximation (should be a subgraph)

Underapproximation means: replace all non-control flow parameters with a sort that only contains one option, an enum with one value. Then, update the equation using the "Definitions" below.

<!--First, we check if we can reach an "unsafe" node in the graph. One which is trivially false: is the value of that node after partial instantiation is immediately false. If no such node exists-->

Then, for the underapproximation, we can calculate the guard of all original outgoing edges. Is the guard true or false? If false, remove that for its graph. Does solving this mean the solution is true? Then we are done.

## Iterating

Say we do not have a solution yet. Then, we want to add some other data parameter and consider this as well in our graph. We can use the information from the "target" function for this purpose. This is then a control flow parameter. It is possible that sometimes this value is unknown for any particular node.

We again do the underapproximation here for all other data parameters, and try again. Until all data paramaters are not abstracted anymore. Then, we should always have a solution, since the underapproximation is equal to the overapproximation. 

## Make

Use `cd ~/Documents/mcrl2-build && make pbescegar -j8` to build.

## Definitions from Cranen et al.

6.1. Existential and Universal Abstractions

Consider a data sort $D$ that represents a complex or infinite semantic set (e.g., the natural numbers). Using a homomorphism, all elements, operators, and relations of the domain $D$ can be mapped to a corresponding simpler data structure $\widehat{D}$. Formally, an abstract interpretation of the concrete elements in a domain $D$ is given by a surjective mapping $h_D: D \rightarrow \widehat{D}$, the so-called abstraction function. In addition, every operator and relation on the concrete data domain $D$ is assumed to have a corresponding abstract operation. Note that the result of an abstract operation is no longer unique, as two different concrete elements may map to the same abstract element.

A standard solution to the nonunicity problem is to lift the codomain of a concrete operation or relation $f: D_1 \times \cdots \times D_n \rightarrow D$ to a set when defining its abstract counterpart $\widehat{f}: \widehat{D}_1 \times \cdots \times \widehat{D}_n \rightarrow 2^{\widehat{D}}$. For reasons of consistency, also the domains of the abstract functions and relations $\widehat{f}$ are lifted to sets for all $A_j \subseteq \widehat{D}_j, 1 \leq j \leq n$ :

$$
\widehat{f}\left(A_1, \ldots, A_n\right)=\bigcup_{a_1 \in A_1, \ldots, a_n \in A_n} \widehat{f}\left(a_1, \ldots, a_n\right) .
$$


It is then possible to define an abstraction operator that converts a term $t$ of sort $D$ to its corresponding sort $\widehat{D}$. Assuming the below grammar for our data terms:

$$
t::=d|c| f\left(t_1, \ldots, t_n\right),
$$

where $d$ is a data variable, $c$ is a closed term, and $f$ is an $n$-ary operator or relation. If we have an abstraction function $h$, we can define the operator as follows:

$$
\widehat{d}=\{\bar{d}\}, \quad \widehat{c}=\{h(c)\}, \quad f\left(\widehat{t_1, \ldots,} t_n\right)=\widehat{f}\left(\widehat{t_1}, \ldots, \widehat{t_n}\right) .
$$


Here, we introduce a fresh variable $\bar{d}$ for every variable $d$ of some concrete sort $D$. Computations in the abstract domain using the abstract operators are sensible as long as the safety condition holds for all terms $t$ with free variables $d_1, \ldots, d_n$, and all closed terms $\left.c_1, \ldots, c_n: \llbracket h\left(t\left[c_1 / d_1, \ldots, c_n / d_n\right]\right) \rrbracket \in \llbracket \widehat{t} \mid h\left(c_1\right) / \overline{d_1}, \ldots, h\left(c_n\right) / \overline{d_n}\right] \rrbracket$. Essentially, this condition ensures that $\hat{t}$ always represents a set of concrete values that includes $t$. This condition is met whenever for all $f: D_1 \times \cdots \times D_n \rightarrow E$ :

$$
\forall d_1 \in D_1, \ldots, d_n \in D_n \cdot h_E\left(f\left(d_1, \ldots, d_n\right)\right) \in \widehat{f}\left(\left\{h_{D_1}\left(d_1\right)\right\}, \ldots,\left\{h_{D_n}\left(d_n\right)\right\}\right) .
$$


Given a set of abstraction functions for a set of concrete domains, we can use these, together with the trivial abstraction (identity) functions for those domains not equipped with a user-defined abstraction function, to strengthen or weaken the predicate formulae we encounter in a given equation system. For simplicity, and without loss of generality, we assume that all concrete sorts in an equation system, with the exception of the Boolean sort, have a homomorphism $h$ mapping these sorts to some abstract domain.

Definition 6.1. Let $\phi$ be an arbitrary predicate formula. We inductively define the underapproximation $\mathcal{A}_{\cap}(\phi)$ (the overapproximation $\mathcal{A}_{\sqcup}(\phi)$, respectively) as follows:

$$
\begin{aligned}
\mathcal{A}_m(b) & = \begin{cases}\exists v: B . v \in \widehat{b} \wedge v & \text { if } m=\sqcup \\
\forall v: B . v \in \widehat{b} \Rightarrow v & \text { otherwise, }\end{cases} \\
\mathcal{A}_m(X(e)) & = \begin{cases}\exists v: \widehat{D_X} \cdot v \in \widehat{e} \wedge \widehat{X}_{\cup}(v) & \text { if } m=\sqcup \\
\forall v: \widehat{D_X} \cdot v \in \widehat{e} \Rightarrow \widehat{X}_{\sqcap}(v) & \text { otherwise, }\end{cases} \\
\mathcal{A}_m(\phi \oplus \psi) & =\mathcal{A}_m(\phi) \oplus \mathcal{A}_m(\psi) \quad \text { for } \oplus \in\{\wedge, \vee\}, \\
\mathcal{A}_m\left(Q d_1: D \cdot \phi\right) & =Q \bar{d}_1: \widehat{D} \cdot \mathcal{A}_m(\phi) \quad \text { for } Q \in\{\forall, \exists\}
\end{aligned}
$$


Note that, by definition of $\widehat{\text { - }}$, the predicate formula $\mathcal{A}_m(\phi)$ contains data variables $\bar{d}$ for the variables $d$ in $\phi$, and predicate variables $\widehat{X}_m$ in $\mathcal{A}_m(\phi)$ for $X$ in $\phi$. We extend the preceding operator $\mathcal{A}_m(-)$ to equation systems in the natural way.
