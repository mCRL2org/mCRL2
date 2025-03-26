.. index:: lpsreach

.. _tool-lpsreach:

lpsreach
==========

This is a symbolic state space exploration tool for linear process
specifications (.lps extension) largely based on the work presented in [1] and
[2]. Similarly to :ref:`tool-lps2lts` it can take any lps as input and produce a finite
state space output. The main difference when compared to lps2lts is that
lpsreach uses so-called symbolic representations internally to represent the set
of states and transitions of the labelled transition system. Note that the
symbolic representations in this tool are limited to finite sets, unlike the
symbolic representations used in :ref:`tool-pbessymbolicbisim`.

A typically use for `lpsreach` is as follows::
  
  lpsreach --verbose infile.lps outfile.sym

This prints the number of states in the state space and saves the resulting
symbolic state space in an internal file format for symbolic representations.
The `--verbose` causes progress information to be printed as well. Currently,
the resulting `outfile.sym` is not very useful, but in the future other tools
will be introduced to convert it into a concrete LTS and reduce it modulo the
standard equivalence relations such as bisimulation.

There are a number of tool options that can typically lead to large speedups.
These can be tried in combination with `--max-iterations` to limit exploration
depth to quickly benchmark their impact. First of all, `--cached` uses
additional memory to reduce the amount of rewriting required. Next,
`--groups=simple`` is a simple, but often effective, heuristic to merge
so-called transition groups, which correspond to the summands of the linear
process and are explained in more detail below. Generally it is advisable by
linearise the mCRL2 specification using the `--no-cluster` (shorthand `-n`)
option of :ref:`tool-mcrl22lps`.

There are two orthogonal exploration strategies implemented under options
`--chaining` and `--saturation` that also often have a large impact on
exploration performance.

To further guide the effectiveness of the exploration we need some additional
background information.

Symbolic Exploration
-------------------------

The effectiveness of the symbolic representation is mostly determined by the
size (in the number of nodes shown by `--print-nodesize`) of the symbolic data
structures during intermediate computations. This size relies heavily on the
order of the process parameters in the linear process and the dependencies of
its summands on these process parameters. Intuitively, each summand is a
condition-action-effect statement, where syntactic occurrences of process
parameters in conditions, action and effects induce `read` dependencies and
non-trivial effect statements induce `write` dependencies. Trivial being of the
shape :math:`x := x`.

Currently, there are no heuristics implemented to compute the parameter order
and the order provided by the linear process is used, but the `--reorder` option
can be used to manually change the order. Ideally, parameters that depend (in
some sense) on eachother, because for example one parameter occurs in the effect
of another parameter, should be located close to eachother. To inspect these
dependencies use the option `--info`. This will show a matrix under `read/write
patterns compacted` where every column corresponds a parameter (in order) and
every row corresponds to a transition group (in order), which are the summands
by default.

Furthermore, the symbolic representation uses the process parameters as they are
provided, which means that for complex data types (such as lists) the structure
of the list is not used and it is represented as a set of values. Here, we can
use :ref:`tool-lpsparunfold` to introduce new parameters that allow a more
structured symbolic representation. 

Besides the order of the process parameters we also have to take into account
the order of transition groups. By default there is one transition group for
each summand in order of the summands of the linear process (as stored on disk).
Use options `-d --info` to print the summands for every transition group for
inspection. The order in which transition groups are applied can affect the size
(in particular for the additional exploration strategies), where transition
groups that write parameters that are read dependencies of other transition
groups should generally be ordered first. Again, there are no heuristics for
this and an order can be manually specified using `--groups`.

Transition groups can also be merged to reduce the amount of symbolic operations
required, but doing so will over-approximate their dependencies (except for the
`--groups=simple`) and this can be disadvantageous. Finally, to understand the
consequences of the symbolic representation we explain it in more detail.

Symbolic Representation
-------------------------

For the state space of a linear process the set of states is typically a set of
vectors, where every element in the vector corresponds to a process parameter.
In :ref:`tool-lps2lts` the states are represented as a set of (maximally shared) balanced
binary trees. However, in `lpsreach` these set of vectors are represented by a
so-called decision diagram based on function decomposition. This representation
allows for maximally sharing equivalent sets and sharing prefixes. Furthermore,
it allows for efficient implementation of various set and relational operations.

A set of vectors can be represented by the following function similar to how
:math:`\mathbb{N} \rightarrow \mathbb{B}` represents a set of natural numbers.

.. math::
  :nowrap:

  \begin{equation*}
    f: \mathbb{N} \times \cdots \times \mathbb{N} \rightarrow \mathbb{B}
  \end{equation*}

Note that we assume that all process parameters are bounded natural numbers
since we can always bidirectionally map from natural numbers to abstract data.
Now, the we can decompose :math:`f`` on the first argument as follows.

.. math::
  :nowrap:

  \begin{equation*}
    f(x_0, \ldots, x_n) = 
    \begin{cases}
      f'_0(x_1, \ldots, x_n) &\textsf{if } x_0 = 0 \\
      f'_1(x_1, \ldots, x_n) &\textsf{if } x_0 = 1 \\
      \cdots \\
      f'_{\|x_0\|}(x_1, \ldots, x_n) &\textsf{if } x_0 = \|x_0\| \\
    \end{cases}
  \end{equation*}

Such that :math:`f(x_0, x_1, \ldots, x_n) = f'_{x_0}(x_1, \ldots, x_n)` for all
:math:`0 \leq x_0 \leq \|x_0\|, \ldots, 0 \leq x_n \leq \|x_n\|`, where
:math:`\|x_0\|` indicates the maximum value of :math:`x_0`.

The decision diagram for this decomposed function consists of vertices and edges
where vertices represent the functions, such as :math:`f` and :math:`f'_0`, and
edges the decisions, for example :math:`x_0 = 0`. The vertices are maximally
shared such that if :math:`f'_0 = f'_1` then there is a unique vertex in memory
representing this function. Furthermore, we always decompose on the first
argument, so the decision diagram is a tree of height :math:`n+1`.

The resulting decision diagram is called a quasi-reduced multi-valued decision
diagrams, which is quasi-reduced since every path from the root to a leaf is
exactly :math:`n+1` long, because it never skips levels. For the implementation
we use Sylvan [3], which implements list decision diagrams. These are unfolded
multi-valued decision diagrams where every vertex has exactly two edges, one
being the decision and the other being the next element in the list.

Limitations
-----------

This tool is only available on macOS and Linux since the Sylvan dependency
cannot be compiled by Visual Studio.

References 
----------------------

  - [1] Stefan Blom and Jan Cornelis van de Pol. Symbolic reachability for process algebras with recursive data types. In J.S. Fitzgerald, A.E. Haxthausen, and H. Yenigun, editors, Theoretical Aspects of Computing \- ICTAC 2008, number Supplement in Lecture Notes in Computer Science, pages 81–95. Springer, August 2008.
  - [2] Jeroen Meijer. Efficient learning and analysis of system behavior. PhD thesis, University of Twente, Netherlands, September 2019.
  - [3] Tom van Dijk, Jaco van de Pol. Sylvan: multi-core framework for decision diagrams. Int. J. Softw. Tools Technol. Transf. 19(6): 675-696 (2017)

.. mcrl2_manual:: lpsreach