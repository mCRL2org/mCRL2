.. _atermpp-term-library:

mCRL2 Term Library
==================

| *Author*: Maurice Laveaux

.. rubric:: Abstract

This document contains a description of the term library as implemented in the mCRL2 toolset.

.. _atermpp-preliminaries:

Preliminaries
-------------

Let :math:`\mathcal{F} = \biguplus_{i \in \mathbb{N}} \mathcal{F}_i` be a *ranked* alphabet of
*function symbols*. A function symbol :math:`f \in \mathcal{F}_i` is said to have a rank or
*arity*, given by :math:`\alpha(f)`, equal to :math:`i`. The set of terms :math:`\mathcal{T}` is
the smallest set such that for :math:`t_0, \ldots, t_n \in \mathcal{T}` and
:math:`f \in \mathcal{F}_n` it holds that :math:`f(t_0, \ldots, t_n) \in \mathcal{T}`, as such
:math:`\mathcal{F}_0 \subseteq \mathcal{T}`. For each term :math:`f(t_0, \ldots, t_n)` we say
that :math:`f` is its *head*, denoted by :math:`\mathsf{head}(f(t_0, \ldots, t_n))`, and we
define a function :math:`\textsf{args}(f(t_0, \ldots, t_n))` to be equal to a sequence of terms
:math:`[t_0, \ldots, t_n]`.

There are several useful classes of terms that can be identified. One example is the class of
*list* terms. Let :math:`{+\!\!+} \in \mathcal{F}_2` and :math:`[\,] \in \mathcal{F}_0` be
function symbols to define list concatenation and the empty list. The set of *list* terms
:math:`\mathcal{L} \subseteq \mathcal{T}` is the smallest set such that
:math:`[\,] \in \mathcal{L}` and for terms :math:`t \in \mathcal{T}` and
:math:`l \in \mathcal{L}` the *concatenation* :math:`{+\!\!+}(t, l) \in \mathcal{L}`. There are
also other classes, such as *term string* where a string is stored as part of the function symbol
name and *binary tree* where every left and right sub-tree is stored as arguments. These can all
be represented in our term library.

.. _atermpp-term-library-section:

Term Library
------------

The term library provides the storage (in memory and on disk) of terms and function symbols.
Formally, the term library stores a finite subset of terms
:math:`\mathcal{T}' \subseteq \mathcal{T}`. The library has been designed with the following
goals in mind:

1. Minimize the amount of memory used to store the set of terms.
2. Fast creation (and deletion) of terms.
3. Fast comparison, in particular equality, between terms.

.. _atermpp-api:

Application Programming Interface
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The library is implemented in the C++11 compliant subset of the C++ language. We assume some
familiarity with the concepts of C++, for now the distinction between classes and objects is
important and the role of constructors and member functions. A constructor is a language construct
that instantiates an object of a specific class, and member functions operate on that instance.

The application programming interface (API) consists of several constructors that allow the user
to create function symbols and terms. Function symbols are constructed from a sequence of
characters to represent their name and a natural number for its arity. The relation between the
mathematical elements and these constructors are listed in the following table.

.. list-table:: The relation between the definition and the API
   :header-rows: 1

   * - Definition
     - API
   * - :math:`\texttt{name} \in \mathcal{F}_n`
     - :math:`\text{function\_symbol}(\texttt{name}, n)`
   * - :math:`f(t_0, \ldots, t_n) \in \mathcal{T}`
     - :math:`\text{term\_appl}(f, t_0, \ldots, t_n)`

For the already identified classes of terms there are specific constructors defined as shown in
the following table.

.. list-table:: The relation between the list class and the API
   :header-rows: 1

   * - Definition
     - API
   * - :math:`[\,] \in \mathcal{F}_0`
     - ``function_symbol(<empty_list>,0)``
   * - :math:`{+\!\!+} \in \mathcal{F}_2`
     - ``function_symbol(<list_constructor>,2)``
   * - :math:`{+\!\!+}(t, l) \in \mathcal{L}`
     - ``term_list(t, l)``

There are also additional member functions to check equality (or inequality) between terms. Note
that there is not really a semantic inequality between terms defined, but from an implementation
viewpoint this is useful to have.

There are also several member functions to access the information carried by a term. There is a
``function`` function that maps a term to its head function symbol. The ``arg`` function takes a
term and an index as input and returns the argument term at the specified index.

Finally, we also provide the ability to write a term to a *stream* and subsequently retrieve the
term from this stream. In particular, the ability to write multiple individual terms to a stream is
provided. For this we provide the function :math:`\textsf{write}(s, t)` to write term :math:`t`
to a stream :math:`s`, which is a sequence of bits or characters, and symmetrically read a term
from a stream by means of :math:`\textsf{read}(s)`, which returns this term.

.. _atermpp-architecture:

Architecture
~~~~~~~~~~~~

The main architectural choice for this library is that terms and function symbols are maximally
shared, by a technique called *hash consing*. Here, every function symbol and term has a unique
identifier and these identifiers are used by a term as references to its head function symbol and
arguments. This sharing facilitates the goal to minimize memory usage by the stored terms and also
allows for constant time comparison, because the identifiers can be compared in constant time. The
inequality between terms is determined by the natural order of these identifiers, which are natural
numbers.

This functionality is implemented by an underlying function symbol *pool* class that stores the
finite subset of function symbols :math:`\mathcal{F}'` and a term pool to store the finite subset
of terms :math:`\mathcal{T}'`. To facilitate ease of programming on a high-level this sharing is
completely transparent to the programmer, which instantiates an object that internally stores an
identifier to look up the information about the underlying function symbol (or term).

For the purpose of deleting shared elements there is a ``shared_reference`` class that internally
counts the number of references to each term or function symbol. Here, a reference means that
another term has that identifier as head symbol or argument. The ``term`` and ``function_symbol``
classes internally use this class to keep track of the number of references.

.. _atermpp-function-symbol-pool:

Function Symbol Pool
--------------------

Now, we are going to dive into specific implementation details and need to be a little more
concrete. An important concept of C++ are its (simplified) memory model, where there is a *heap*
that maps positions, also called *pointers* or *references*, to an array of bits; in general data
are now important. We use a :math:`(\textit{name}, \textit{arity})` pair to indicate the
information stored on the heap about the function symbol. Two function symbols with the same name
and arity are equivalent. The function symbol pool has a member function named ``create`` to
obtain the identifier to the function symbol with the given name and arity. This identifier is the
reference to the data stored on the heap.

- :math:`\textsf{create}(\textit{name} : \mathrm{String},\; \textit{arity} : \mathrm{Nat}) : \mathbb{N}`

The implementation of the ``create`` function is given in Algorithm :ref:`atermpp-alg-create-symbol`.
The set of identifiers :math:`\mathcal{F}'`, and later :math:`\mathcal{T}'`, is a set with
polymorphic lookup where the identifier can be found using the
:math:`(\textit{name}, \textit{arity})`. We introduce the notation
:math:`\mathcal{F}'[(\textit{name}, \textit{arity})]` to indicate the partial function mapping the
arguments to the identifier, which yields :math:`\bot` when the element is not in the set. Note
that although :math:`\mathcal{F}'` is a set of identifiers on to elements on the heap it also
directly stores the finite set of function symbols. The :math:`\textsf{allocate}` function reserves
space on the heap and stores the :math:`\textit{name}` and :math:`\textit{arity}` pair in the
reserved space. In the actual implementation this is a separate constructor for a pair-like object.

.. _atermpp-alg-create-symbol:

.. rubric:: Algorithm: Creation of function symbols

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Procedure{create}{$\textit{name},\, \textit{arity}$}
   \If {$\mathcal{F}'[(\textit{name}, \textit{arity})] \neq \bot$}
     \State {\textbf{return} $\mathcal{F}'[(\textit{name}, \textit{arity})]$}
   \EndIf
   \State {$f \leftarrow \textsf{allocate}(\textit{name}, \textit{arity})$}
   \State {$\mathcal{F}' \leftarrow \mathcal{F}' \cup \{f\}$}
   \State {\textbf{return} $\textsf{ref}(f)$}
   \EndProcedure
   \end{algorithmic}

The resulting identifier is then used by ``function_symbol`` to look up the underlying information
about the function symbol.

To clean up, *i.e.*, removing the underlying data from the heap, objects with a reference count of
zero, there are two different approaches. The first method immediately cleans up the function
symbol from :math:`\mathcal{F}'` and the heap whenever its reference count becomes zero in its
destructor. This approach will be referred to as *direct-destruction*. Another approach is to
periodically remove all function symbols with a reference count of zero. This approach will be
referred to as *garbage collection*.

For function symbols the *direct-destruction* method is used as the number of function symbols
that is destroyed is typically very low. This destructor that is called when :math:`f` goes out of
scope is implemented by the ``destroy`` function defined below.

.. _atermpp-alg-destroy-symbol:

.. rubric:: Algorithm: Destruction of function symbols

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Procedure{destroy}{$f$}
   \State {$\textsf{deallocate}(f)$}
   \State {$\mathcal{F}' \leftarrow \mathcal{F}' \setminus \{f\}$}
   \EndProcedure
   \end{algorithmic}

The :math:`\textsf{deallocate}` function frees the identifier to be used again by a different
function symbol.

.. _atermpp-term-pool:

Term Pool
---------

The term pool is the class that stores the set of terms :math:`\mathcal{T}'` and is very similar
to the function symbol pool. The *create_appl* can be used to create new terms; we refer to terms
with arguments as *function applications*.

The constructor of a function application calls the following function to obtain a reference to an
existing or new term.

- :math:`\text{create\_appl}(f : \mathcal{F},\; t_0, \ldots, t_n : \mathcal{T}) : \mathbb{N}`

The implementation of this function can be seen in Algorithm :ref:`atermpp-alg-create-appl`. The
function symbol :math:`f` and terms :math:`t_0` to :math:`t_n` should be elements of the current
subset of stored function symbols :math:`\mathcal{F}'` and terms :math:`\mathcal{T}'`
respectively, which means that they are identifiers. Note that on the heap a tuple
:math:`(f, c, t_0, \ldots, t_n)` is stored that encodes the actual term with
:math:`c \in \mathbb{N}` a reference counter.

For terms we use the garbage collection approach, where garbage collection is triggered based on
some heuristics. The advantage of garbage collection is that terms with a reference count of zero
can be reused when they are recreated before being destroyed. This means that only its reference
counter must be increased. In practice, this occurs quite often as the volume of terms being
created and destroyed during term rewriting is quite large.

.. _atermpp-alg-create-appl:

.. rubric:: Algorithm: Creation of term applications

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Procedure{create\_appl}{$f, t_0, \ldots, t_n$}
   \If {$\mathcal{T}'[f, t_0, \ldots, t_n] \neq \bot$}
     \State {\textbf{return} $\mathcal{T}'[f, t_0, \ldots, t_n]$}
   \EndIf
   \State {$t \leftarrow \text{construct}(f, t_0, \ldots, t_n)$}
   \If {should-collect-garbage()}
     \State {collect()}
   \EndIf
   \State {$\mathcal{T}' \leftarrow \mathcal{T}' \cup \{t\}$}
   \State {\textbf{return} $t$}
   \EndProcedure
   \end{algorithmic}

The boolean function *should-garbage-collect* is used to decide when garbage collection should be
performed. The garbage collection itself is implemented by the *collect* function. Currently the
*should-garbage-collect* function is implemented by using a counter that is decremented on every
call to *should-garbage-collect*. Whenever this counter reaches zero the function returns true.
During garbage collection this counter is then set to the number of terms in :math:`\mathcal{T}'`,
which is equal to :math:`|\mathcal{T}'|`.

.. _atermpp-garbage-collection:

Garbage Collection
~~~~~~~~~~~~~~~~~~

The garbage collection is implemented by calling destroy on every term with a reference count of
zero, as shown in the following pseudocode.

.. _atermpp-alg-collect:

.. rubric:: Algorithm: Garbage collection of terms

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Procedure{Collect}{}
   \For {$t \in \mathcal{T}'$}
     \If {$\text{reference-count}(t) = 0$}
       \State{Destroy($t$)}
     \EndIf
   \EndFor
   \State{$\text{collect-countdown} \leftarrow |\mathcal{T}'|$}
   \EndProcedure
   \end{algorithmic}

The *Destroy* method recursively destroys all arguments which have a single reference, because
that reference is the current function application.

.. _atermpp-alg-destroy-term:

.. rubric:: Algorithm: Destroying individual terms

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Procedure{Destroy}{$t \in \mathcal{T}'$}
   \For {$p \in \textsf{args}(t)$}
     \If {$\textit{reference-count}(p) = 1$}
       \State {Destroy($p$)}
     \EndIf
   \EndFor
   \State {$\mathcal{T}' \leftarrow \mathcal{T}' \setminus \{t\}$}
   \State {deallocate($t$)}
   \EndProcedure
   \end{algorithmic}

The *deallocate* function frees the heap memory used by this term. Note that this function
prematurely destroys arguments with a single reference count. The reason for this is that the
function ``arg`` is no longer defined for :math:`t` after it has been deallocated.

.. _atermpp-binary-io:

Binary Input/Output
-------------------

We provide two different streaming procedures to share terms between tools of the toolset. The
first method uses the concrete syntax and operators on streams of characters. A function symbol
:math:`f(t_0, \ldots, t_n)` is simply written to the character stream as the name of :math:`f`
written as ``"f"``, followed by recursively printing all its arguments :math:`t_0` to :math:`t_n`
separated by commas and a closing bracket at end. For convenience of reading, list terms are
printed as a sequence of their elements surrounded by square brackets. The reading procedure
simply parses the characters of the input stream and reconstructs the corresponding terms.

Although this is useful for debugging it is not a very efficient exchange format as sharing of
function symbols of terms cannot be employed and characters themselves are very verbose. Therefore,
we have developed the streamable binary (a)term format (SBAF). A single term can be written to
and read from a *binary* stream in SBAF one at a time and terms in the same stream share their
arguments. We focus on writing terms first and then show how the resulting stream can be used to
reconstruct the written terms during reading.

Similar to the in-memory storage, a (different) unique identifier is assigned to each term and
function symbol that is used to identify arguments during writing. The output is going to consist
of *packets* that either represent a function symbol, a term or a subterm indicated by two bits in
the header of a packet. We use one, two and three respectively to represent these headers. The
difference between a term and a subterm is that a term is returned from :math:`\textsf{write}(s)`,
where :math:`s` is a stream, and the subterms are (as their name suggests) subterms of the
retrieved term.

The unique identifier for each written element is stored by a mapping from function symbols and
terms to a natural number. This is done in a data structure called an *indexed set* that increments
a consecutive index whenever an element is inserted and assigns it to the new element. Note that
we are never going to remove elements from this set so the indices are always compact.

Binary streams are not directly facilitated by the C++ library so we have implemented a
``bitstream`` class that is able to write (and read) individual bits from and to a stream. It also
provides several convenience functions to write character sequences and integers to this stream,
where integers are stored using a *variable width* encoding, where we use the overloads
:math:`\textsf{write}(s, \text{``string''})` and :math:`\textsf{write}(s, n)`, with
:math:`n \in \mathbb{N}`. Furthermore, we use the notation :math:`n_i` for some natural number
:math:`n` to indicate that only :math:`i` bits of the binary encoding of this number are
considered when writing it, instead of relying on the variable width encoding.

Let :math:`I` be an indexed set of function symbols. Algorithm
:ref:`atermpp-alg-write-symbol` shows how an individual function symbol is written to a stream
:math:`s`. First, we check whether the function symbol was already written, which means that it
has an index :math:`n`. Otherwise, there are only three distinct packets so only two bits are
needed to encode it. After which the name and arity of the function symbol are written and a new
index :math:`n` is created for this function symbol.

.. _atermpp-alg-write-symbol:

.. rubric:: Algorithm: Writing a function symbol to an output stream :math:`s`

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Procedure{write-function-symbol}{$s,\, f \in \mathcal{F}'$}
   \If {$(f, n) \in I$}
     \Return $n$
   \Else
     \State {$\textsf{write}(s, 0_2)$}
     \State {$\textsf{write}(s, \textit{name}(f))$}
     \State {$\textsf{write}(s, \textit{arity}(f))$}
     \State {$I \leftarrow I \cup \{f\}$}
     \Return $n$ \text{ such that } $(f, n) \in I$
   \EndIf
   \EndProcedure
   \end{algorithmic}

Let :math:`J` be an indexed set of terms. Algorithm :ref:`atermpp-alg-write-term` shows the
implementation to write a term to the stream :math:`s`. We write terms such that the arguments
(and head symbol) of a term are written to the stream before the term itself by traversing the
term bottom up. This can be achieved by maintaining a stack of terms where each term has a value
:math:`\{\top, \bot\}` associated to it to indicate whether its arguments have already been
processed. The bottom up traversal is implemented by first changing the flag for :math:`t'` from
:math:`\bot` to :math:`\top` and then inserting all arguments that are not already in :math:`J`
(as these have already been written) to the stack. We must also maintain that the term taken from
the stack has not been inserted to :math:`J` in the meantime, which can happen whenever the same
subterm occurs multiple times.

The case where the term is actually written to the stream, indicated by :math:`b = \top`, starts
by writing obtaining the function symbol index and possibly writing a function symbol packet to the
stream. Then, the packet identifier (again using 2 bits) is written, which is an output term
whenever the current term is equal to :math:`t` and a subterm otherwise. Then the indices of the
function symbol and each argument (which should already be included in :math:`J`) are written to
the stream. Note that we only need :math:`\textsf{ceil}(\log(|I|) + 1)` bits to uniquely identify
previously written subterms and function symbols. During reading we also know the number of terms
that have been written and as such the number of bits to read can be inferred. Finally, the written
term is stored in the indexed set :math:`J`.

.. _atermpp-alg-write-term:

.. rubric:: Algorithm: Writing a term to an output stream :math:`s`

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Procedure{Write}{$s \in \mathbb{B}^*,\, t \in \mathcal{T}'$}
   \State {$J \leftarrow \emptyset$}
   \State {$Q \leftarrow \{ (s, \bot) \}$}
   \While {$\neg\,\textsf{empty}(Q)$}
     \State {$(t', b) \leftarrow \textsf{pop}(Q)$}
     \If {$\neg \exists\, n \in \mathbb{N} : (t', n) \in J$}
       \If {$b = \top$}
         \State {$n \leftarrow \text{write-function-symbol}(\textsf{head}(t'))$}
         \If {$t' = t$}
           \State {$\textsf{write}(s, 1_2)$}
         \Else
           \State {$\textsf{write}(s, 2_2)$}
         \EndIf
         \State {$\textsf{write}(s,\, n_{\textsf{ceil}(\log(|I|)+1)})$}
         \For {$p \in \textsf{args}(t')$}
           \State {$i$ such that $(i, i) \in J$}
           \State {$\textsf{write}(s,\, i_{\textsf{ceil}(\log(|J|)+1)})$}
         \EndFor
         \State {$J \leftarrow J \cup \{t'\}$}
       \Else
         \State {$\textsf{push}(Q, (t', \top))$}
         \For {$p \in \textsf{args}(t')$}
           \If {$\neg \exists\, n \in \mathbb{N} : (p, n) \in J$}
             \State {$\textsf{push}(Q, (p, \bot))$}
           \EndIf
         \EndFor
       \EndIf
     \EndIf
   \EndWhile
   \Return {$s$}
   \EndProcedure
   \end{algorithmic}

We present a small example to illustrate this algorithm.

.. admonition:: Example

   Let us consider writing the term ``mult(s(s(z)), s(z))`` to a stream, which is the example
   presented in [BJKO00]_. Again, we use the subscript to indicate the number of bits of that
   number written to the stream. To write a string we first write its length, followed by
   :math:`n` characters using 8 bits per character. For numbers smaller than 128 we need eight
   bits in our variable-width encoding. The resulting writes and the values for :math:`I` and
   :math:`J` are presented in the following table.

   .. list-table:: Steps to write ``mult(s(s(z)), s(z))`` to a stream
      :header-rows: 1

      * - Term
        - Function Symbol
        - Output
        - Size (b)
        - :math:`I`
        - :math:`J`
      * -
        - z
        - :math:`0_2\; 1_8 \text{``z''} \; 0_8`
        - 26
        - :math:`\{(z, 0)\}`
        -
      * - z
        -
        - :math:`1_2\; 0_1`
        - 3
        - :math:`\{(z, 0)\}`
        - :math:`\{(z, 0)\}`
      * -
        - s
        - :math:`0_2\; 1_8 \text{``s''} \; 1_8`
        - 26
        - :math:`\{(z, 0),(s, 1)\}`
        - :math:`\{(z, 0)\}`
      * - s(z)
        -
        - :math:`1_2\; 1_1\; 0_1`
        - 4
        - :math:`\{(z, 0),(s, 1)\}`
        - :math:`\{(z, 0),(s(z), 1)\}`
      * - s(s(z))
        -
        - :math:`1_2\; 1_1\; 1_1`
        - 4
        - :math:`\{(z, 0),(s, 1)\}`
        - :math:`\{(z, 0),(s(z), 1),(s(s(z)), 2)\}`
      * -
        - mult
        - :math:`0_2\; 4_8 \text{``mult''} \; 2_8`
        - 50
        - :math:`\{(z, 0),(s, 1),(\text{mult}, 2)\}`
        - :math:`\{(z, 0),(s(z), 1),(s(s(z)), 2)\}`
      * - mult(s(s(z)), s(z))
        -
        - :math:`2_2\; 2_2\; 2_2\; 1_2`
        - 8
        -
        -

   So writing this term requires 121 bits in total. Typically the ratio of terms to function
   symbols is quite high, so the small number of bits needed to encode the terms is the most
   important.

There are two separate indexed sets for function symbols and terms such that the resulting indices
are kept smaller. Furthermore, in practice we also allow a function over terms to be used during
writing (and reading) that is applied to each subterm before the term is written to a stream. This
can be useful to remove some implementation details from the terms that should not be shared
between tools.

The symmetric reading function is implemented in Algorithm :ref:`atermpp-alg-read-term`. Upon
initialization of the stream :math:`I` and :math:`J` are set to :math:`\emptyset` and these
indexed sets should be maintained in between calls to reading terms. Reading from a stream has
additional auxiliary functions :math:`\textsf{read}_{\textit{bits}}(s, n)`,
:math:`\textsf{read}_{\mathbb{N}}(s)` and :math:`\textsf{read}_{\textit{string}}` to read
:math:`n` bits, numbers and characters from the stream :math:`s` respectively. Again, we use an
indexed set :math:`I`. However, in the implementation we can use an array to efficiently map
indices to elements in this set as the indices grow strictly increasing and the mapping is
injective.

.. _atermpp-alg-read-term:

.. rubric:: Algorithm: Reading a term from an input stream :math:`s`

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Procedure{Read}{$s \in \mathbb{B}^*,\, t \in \mathcal{T}',\, I,\, J$}
   \State {$p \leftarrow \textsf{read}_{\textit{bits}}(s, 2)$}
   \If {$p = 0$}
     \State {$\textit{name} \leftarrow \textsf{read}_{\textit{string}}(s)$}
     \State {$\textit{arity} \leftarrow \textsf{read}_{\mathbb{N}}(s)$}
     \State {$I \leftarrow I \cup \{(\textit{name}, \textit{arity})\}$}
   \Else
     \State {$f$ such that $(f,\; \textsf{read}_{\textit{bits}}(s,\; \textsf{ceil}(\log(|J|)+1))) \in I$}
     \State {$Q \leftarrow [t_0, \ldots, t_{\textit{arity}(f)}]$}
     \For {$i \in Q$}
       \State {$t_i$ assigned such that $(t_i,\; \textsf{read}_{\textit{bits}}(s,\; \textsf{ceil}(\log(|J|)+1))) \in J$}
     \EndFor
     \State {$t \leftarrow f(t_0, \ldots, t_{\textit{arity}(f)})$}
     \If {$p = 2$}
       \Return {$t$}
     \Else
       \State {$J \leftarrow J \cup \{t\}$}
     \EndIf
   \EndIf
   \Return {$s$}
   \EndProcedure
   \end{algorithmic}

In the implementation a special function symbol exists to mark the end of the stream.

Previously an alternative format, called binary aterm format, described in [BJKO00]_ was used to
write terms into streams. This previous format allowed better compression by counting the
occurrences of all terms at all argument positions beforehand and minimizing the amount of bits
needed to encode this information. Although this method allowed presumably optimal compression it
could only write a single term (with sharing) to the stream and required a lot of book keeping
while traversing the term twice. However, for labelled transition systems, which are typical output
terms, the bulk of information is the list of transitions. To encode this list of transitions as a
single term the previously mentioned class of list terms was used. This meant that first this
enormous term had to be constructed, then traversed depth-first twice (incurring quite some memory
overhead), followed by writing this term to the stream, where all transitions could occur at the
first position of the list concatenation term.

The new method avoids the construction of this list term as transitions can be written to the
stream individually. Furthermore, the width of indices assigned to transition terms simply scale
logarithmically in the number of transitions, which yielded a better (both in space and time) term
format. Writing the example term ``mult(s(s(z)), s(z))`` in the old format required a table of
140 bits and then 5 bits to write the term itself. Even though the new format uses more bits to
write this term it is much more straightforward to process and in practice it performs (much)
better.

.. _atermpp-data-structures:

Data Structures
---------------

In this section the data structures in the underlying implementation are described in more detail.

.. _atermpp-reference-counting:

Reference Counting
~~~~~~~~~~~~~~~~~~

The shared reference is implemented by a class named ``shared_reference`` which consists of a
reference and a reference counter. The invariant states that the reference counter is always equal
to the number of instances that point to the same referred term. In C++ there are a number of
operators to construct, move and copy classes. A move is an operator where an object is constructed
from another object, but the other object can be left in an undefined state. The reference count
invariant is satisfied by implementing these operations in the following way:

1. When a ``shared_reference`` instance is constructed from a reference its reference count is
   incremented by one.

2. When a ``shared_reference`` instance is copy-constructed its reference count is incremented by
   one.

3. When a ``shared_reference`` instance is move-constructed the reference count is kept the same,
   but the ``shared_reference`` instance that was moved from will become a null reference.

4. When a ``shared_reference`` instance is assigned to, its current reference count is decremented
   by one. The reference count of the assigned reference is incremented by one.

5. When a ``shared_reference`` instance is move-assigned its current reference count is decremented
   by one. The ``shared_reference`` instance that was moved from will become a null reference.

6. When a ``shared_reference`` is destructed the reference count will be decremented by one.

For terms and function symbols all shared references are constructed with a default term or
function symbol respectively. Whenever the reference count for a reference is equal to zero the
referred to term can be cleaned up, except for the default that always maintained a reference
count of at least one. In the case of function symbols this immediately triggers the destroy
function.

.. _atermpp-hash-table:

Hash Table
~~~~~~~~~~

The references to terms and function symbols are both stored in a set which provides constant time
insertion :math:`\cup`, deletion :math:`\setminus` and contains :math:`\in` functions. These sets
store pointers to the elements allocated on the heap. This can be efficiently implemented by using
a hash table. As typical for a hash table it requires a hash function and an equivalence check for
its elements to implement its operations.

Our hash function, which is an operator to natural numbers, for terms is determined by a suitable
combination of the underlying references as shown in Algorithm :ref:`atermpp-alg-hash`. For terms
the hash function is defined by combining the values of all the references. For performance reasons
it is furthermore desirable to be able to compute the hash function and equivalence check without
(temporarily) instantiating elements of that type.

.. _atermpp-alg-hash:

.. rubric:: Algorithm: Hashing terms

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Procedure{hash}{$f, t_0, \ldots, t_n$}
   \State {$\textit{hnr} \leftarrow \textsc{hash}(f)$}
   \For {$t \in \{t_0, \ldots, t_n\}$}
     \State {$\textit{hnr} \leftarrow \textsc{combine}(\textit{hnr}, t)$}
   \EndFor
   \State {\textbf{return} $\textit{hnr}$}
   \EndProcedure
   \end{algorithmic}

In practice it is quite subtle what a good (and fast) :math:`\textsc{combine}` function is that
ensures a fairly random distribution of hash values. For function symbols the hash is obtained
from hashing the name string and its arity and returning its combination. Note that :math:`f` is a
reference to a function symbol and as such it does not require this expensive computation, but the
hash is just derived from the given reference (or pointer). It is straightforward to also derive
functions that can be applied to terms and function symbols directly, which is required for
rehashing. The equivalence between terms can be determined by comparing the references their head
symbols and pairwise comparing the references for its arguments. For function symbols this is done
by checking name and arity equivalence.

As the equivalence check for terms is quite costly it is essential that the number of equivalence
checks upon searching is minimized. For this purpose we have chosen for an open hash table with
single linked-list buckets as the probing used by a closed hash table resulted in a performance
reduction of ten to fifteen percent. On the other hand, closed hash tables have a reduced memory
footprint so there is a trade-off.

.. _atermpp-optimizations:

Optimizations
-------------

There are several optimizations that have been done on the previously described implementation.

.. _atermpp-pool-per-arity:

Term Pool per Arity
~~~~~~~~~~~~~~~~~~~

Most terms only have a small number of arguments. Let :math:`k` be a constant such that a term is
*small* whenever its head symbol has an arity that is less than or equal to :math:`k`. We will use
:math:`\mathcal{T}^i`, for any natural number :math:`i`, to denote a subset of :math:`\mathcal{T}`
with function symbols that have an arity equal to :math:`i`.

The decision procedure has constant complexity as well. Now, instead of having one pool to store
the finite subset :math:`\mathcal{T}' \subseteq \mathcal{T}`, there will be a number of term
pools:

- For small terms, :math:`k` different pools to store
  :math:`\mathcal{T}^0 \cup \cdots \cup \mathcal{T}^k \subseteq \mathcal{T}'`.
- A pool to store the set of terms :math:`\mathcal{T}'' \subset \mathcal{T}'` that do not occur
  in the other pools, *i.e.*,
  :math:`\mathcal{T}'' = \mathcal{T}' \setminus (\mathcal{T}^0 \cup \cdots \cup \mathcal{T}^k)`.

Note that all pools are disjoint, *i.e.*,
:math:`\mathcal{T}^0 \cap \cdots \cap \mathcal{T}^k \cap \mathcal{T}'' = \emptyset` and all pools
combined store the whole subset, as such
:math:`\mathcal{T}^0 \cup \cdots \cup \mathcal{T}^k \cup \mathcal{T}'' = \mathcal{T}'`. The
interface of the term pool remains unchanged. Internally this term pool uses the arity of a
function symbol to decide which underlying pool will be used to create the term. For example calls
to ``create_appl(f)`` will always go to the pool storing :math:`\mathcal{T}^0`.

The advantage is that the complexity for all loops over the arguments of function symbols with an
arity up to and including :math:`k` will become constant. This enables compiler optimizations such
as loop unrolling in practice, which can have quite an effect on run time performance. This
optimization reduces the number of branch misses which increases the effective run-time performance
as well.

.. _atermpp-weak-reference-counting:

Weak Reference Counting
~~~~~~~~~~~~~~~~~~~~~~~

Here, we introduce a way to relax the reference counting for arguments of terms to reduce the
number of reference count changes. For this purpose we introduce the notion of a *weak reference*.
A weak reference is a reference that does not change the reference counter of the referred to term,
but might still refer to other objects. We are going to change the references to arguments of a
term to be weak references, thus reducing the number of reference count changes. However, this
means that garbage collection has to be adapted, because a reference count of zero does not
necessarily mean that the term is no longer referred to.

The garbage collection that is described here is often referred to as a *tracing garbage
collection*. The basic algorithm is a two-phase algorithm where first the reachable terms are
*marked*, followed by a *sweep* that cleans up the unmarked terms, as these are unreachable.

The marking procedure is implemented as follows. Consider the terms as a graph where the set of
vertices are given by :math:`\mathcal{T}'` and the edges are given by the weak references between
terms and their arguments. Every term with a reference count above zero is reachable by definition;
these terms form the *root* set. This has been implemented as indicated in Algorithm
:ref:`atermpp-alg-root-mark`.

.. _atermpp-alg-root-mark:

.. rubric:: Algorithm: Marking the root set

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Procedure{RootMark}{}
   \For {$t \in S$}
     \If {$\text{reference-count}(t) > 0$}
       \State{Mark($t$)}
     \EndIf
   \EndFor
   \EndProcedure
   \end{algorithmic}

Starting from elements in this root set, we perform a search to mark the elements that can be
reached by the edges in this graph. This step is implemented by the *Mark* function described in
Algorithm :ref:`atermpp-alg-mark`. A term can be marked by the function *set-mark* and the mark
can be removed by using *remove-mark*. The function *is-marked* returns true if and only if that
term has been marked. Note that marking stops whenever it finds that the term has already been
marked. This is correct, because marked terms either belong to the root set or the mark function
has been previously applied to them, ensuring that its arguments have already been marked. This
ensures that the subterms of shared terms are not explored again.

.. _atermpp-alg-mark:

.. rubric:: Algorithm: Marking reachable terms

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Procedure{Mark}{$t : \mathcal{T}'$}
   \If{$\neg\,\text{is-marked}(t)$}
     \State{set-mark($t$)}
     \For {$p \in \textsf{args}(t)$}
       \State{Mark($p$)}
     \EndFor
   \EndIf
   \EndProcedure
   \end{algorithmic}

In the implementation this exploration is implemented depth-first and with an explicit stack to
avoid excessive memory usage and stack overflow issues.

The marking of terms itself can be implemented efficiently by observing that all terms outside of
the root set have a reference count of zero. This reference count can be set to a special value
(in this case max value) to indicate that it has been marked and removing the mark resets it back
to zero. Furthermore, in

After these steps we can conclude that all terms that have been marked are not reachable by the
root set and can be deallocated and removed from the set :math:`\mathcal{T}'`. We should also
remove the mark of terms such that the next garbage collection can be performed again. This is
implemented by the *Sweep* function that is described in Algorithm :ref:`atermpp-alg-sweep`.

.. _atermpp-alg-sweep:

.. rubric:: Algorithm: Sweeping terms that are not reachable

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Procedure{Sweep}{}
   \For {$t \in S$}
     \If {$\neg\,\text{is-marked}(t)$}
       \State{$S \leftarrow S \setminus \{t\}$}
       \State{deallocate($t$)}
     \Else
       \State{remove-mark($t$)}
     \EndIf
   \EndFor
   \EndProcedure
   \end{algorithmic}

After this the heap memory used by terms that are not reachable has been freed.

.. _atermpp-null-term:

Null Term
~~~~~~~~~

Instead of introducing a default term that is used whenever the ``shared_reference`` is default
constructed we can introduce an actual *null* reference, as in the literal defined in the language.
This breaks the invariant that any term always has a valid (possibly default) shared reference.
Therefore, the ``shared_reference`` must first check whether it points to a valid term before
trying to adapt its reference counter. For this purpose a function named *defined* is introduced
that returns true iff the shared reference is not equal to *null*.

The advantage of this alternative is a removal of reference count adaptations to all instances of
the default term. In the 1394 protocol state space generation with the option ``--cached`` this
optimization reduces the number of reference count adaptations by ten percent. The run-time
performance was almost unaffected. However, in the case of atomic reference counters this had
about 11 percent run time reduction for the state space exploration.

.. _atermpp-block-allocator:

Block Allocator
~~~~~~~~~~~~~~~

The :math:`\textsf{allocate}` and :math:`\textsf{deallocate}` functions are part of the allocator
interface as defined in the STL. Note that we have combined the :math:`\textsf{allocate}` and
construct function where :math:`\textsf{allocate}` also directly assigns the data on the heap,
which is normally performed by the constructor. Although no specific implementation is required for
these functions they typically call the system interface to obtain memory from the operating system
for each call. In the Linux kernel a slab allocator is used that has no internal fragmentation for
powers of two and a page size is typically 4KB. Compared to that, a single term of arity three
only requires 40 bytes. This is a very small amount and also not a power of two, which is
typically not optimal.

As these terms are fundamental to the operation of the tool set the number of allocated terms is
typically very high. Therefore, a useful approach to reduce the number of system calls and memory
overhead is to allocate larger blocks of memory and return references into these blocks. This is
done by our *block allocator*.

The block allocator has a single linked list of memory blocks called :math:`\textsf{blocks}`. Each
block can store :math:`\textsf{ElementsPerBlock}` number of elements. We refer to these elements
as *slots* in the block that can either be free or contain an element. The allocator stores a
:math:`\textsf{currentIndex}` of the first slot in the block that has never been used. To keep
track of slots that have been freed in the mean time a *free list* is used.

The ``allocate`` function is described in Algorithm :ref:`atermpp-alg-allocate`. First, if the
free list is not empty then we return the next element in that list and remove it from the list;
this is done by ``pop_front``. If we have used all indices at least once, and the freelist is
empty, then a new block is allocated and added to the :math:`\textsf{blocks}` list. Otherwise, we
take the first index in the first block that has not been used yet and update the
``currentIndex`` accordingly.

.. _atermpp-alg-allocate:

.. rubric:: Algorithm: Allocate

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Procedure{Allocate}{}
   \If {$\neg\,\textsf{freeList}.\text{empty}()$}
     \State {\textbf{return} $\textsf{freeList}.\text{pop\_front}()$}
   \EndIf
   \If {$\textsf{currentIndex} \geq \textsf{ElementsPerBlock}$}
     \State {$\textsf{blocks} \leftarrow \textsf{blocks}.\text{push\_front}()$}
     \State {$\textsf{currentIndex} \leftarrow 0$}
   \EndIf
   \State {$\text{firstBlock} \leftarrow \textsf{blocks}.\text{front}()$}
   \State {$\text{slot} \leftarrow \text{firstBlock}[\textsf{currentIndex}]$}
   \State {$\textsf{currentIndex} \leftarrow \textsf{currentIndex} + 1$}
   \State {\textbf{return} $\text{slot}$}
   \EndProcedure
   \end{algorithmic}

Deallocating a term is straightforward as it just has to be added to the freelist, as shown in
Algorithm :ref:`atermpp-alg-deallocate`.

.. _atermpp-alg-deallocate:

.. rubric:: Algorithm: Deallocate

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Procedure{Deallocate}{Reference $r$}
   \State {$\textsf{freeList}.\text{push\_front}(r)$}
   \EndProcedure
   \end{algorithmic}

One thing to note here is that the freelist can be efficiently implemented by storing the reference
to the next element in the list in place of the free slots of the blocks. The first slot in this
free list is pointed to by some variable named *firstFreeSlot*. The *freeList* can then be stored
by storing the *next* reference, which contains the reference to the next element in the list, in
place of the slots. We define the invariant that all slots that are reachable by following next
references after ``firstFreeSlot`` are part of the freelist. This means that the *freeList* is
empty when the *firstFreeSlot* points to null. The *push_front* operation can be achieved by
letting the *firstFreeSlot* point to the reference that was pushed into the *freeList* and setting
the next reference to the head of the *freeList*. Iteration over the *freeList* can be achieved by
following the *next* reference until it is null. This means that the freelist can be updated
without performing any allocations by itself.

Finally, in case that many elements are deallocated it is useful to erase blocks that do not store
any elements, equivalently where all slots are free. For this purpose we introduce the *consolidate*
function as shown in Algorithm :ref:`atermpp-alg-consolidate`. In the first part of the algorithm
all elements of the free list are marked by a special value :math:`\top`, which is a value that
should not occur in any slot before consolidate is called, in lines 2 to 6. For any block that
only contains these special values it can be removed. The next part is to reconstruct the
*freeList* from the free elements in the block. In practice these two loops are combined into one,
maintaining whether the block contains elements that are not :math:`\top` and the *freeList* during
iteration over a block. At the end of this iteration, the block is removed when the first condition
holds and all entries of this block are removed from the *freeList*, which can be done in constant
time by remembering the starting element when entering this loop.

.. _atermpp-alg-consolidate:

.. rubric:: Algorithm: Consolidate

.. math::
   :nowrap:
   :class: math-left

   \begin{algorithmic}[1]
   \Procedure{Consolidate}{}
   \For {$\text{slot} \in \textsf{freeList}$}
     \State {$\text{slot} \leftarrow \top$}
   \EndFor
   \For {$\text{block} \in \textsf{blocks}$}
     \If {$\forall\, \text{slot} \in \text{block} : \text{slot} = \top$}
       \State {$\textsf{blocks}.\text{erase}(\text{block})$}
     \Else
       \For {$\text{slot} \in \text{block}$}
         \If {$\text{slot} = \top$}
           \State{$\textsf{freeList}.\text{push\_back}(\text{slot})$}
         \EndIf
       \EndFor
     \EndIf
   \EndFor
   \EndProcedure
   \end{algorithmic}

Although it might seem that it is very unlikely that blocks become completely empty this
optimization had quite a large effect on the larger examples (in the order of ten to twenty
percent).

.. _atermpp-alignment:

Alignment
~~~~~~~~~

In a typical processor the accesses to main memory are cached through a number of increasingly
larger, but slower caches. A *cache line* is a block of consecutive memory that a processor
fetches from main memory and stores in the cache at once. Whenever the processor loads or stores
an address from main memory it actually fetches the whole block that contains this address.
Furthermore, these blocks are disjoint and so-called *aligned* to multiples of the cache line
*width*.

In a modern processor the typical cache line has a width of 64 bytes. This means that every
memory access will fetch 64 consecutive bytes if it is not already in the cache. One optimization
idea was to store the terms in the memory such that fetching its arguments does not cross cache
line boundaries (and thus only require one fetch). However, benchmarks indicated that this did not
have a good impact on performance and it does carry a potential memory increase, which is also
undesirable.

.. _atermpp-number-terms:

Number Terms
~~~~~~~~~~~~

In some cases it is useful to store a numerical value as an argument to a term directly. For
example to gain better performance it might be useful to index terms in some data structure and
store this index as an argument in the term. Therefore, we extend the definition of terms such that
:math:`\mathbb{N} \subseteq \mathcal{T}`. The API is extended in the following way:

.. table:: Extension to the API with number terms

   =================================  ============================================================
   Definition                         API
   =================================  ============================================================
   :math:`n \in \mathbb{N}`           :math:`\text{create\_int}(n : \mathbb{N})`
   =================================  ============================================================

The ``value`` function is defined that maps constant terms to the natural number value of which it
was constructed. In the implementation this numerical value is stored where normally the reference
to the first argument is stored and a new function symbol
:math:`\texttt{aterm\_int} \in \mathcal{F}_0` is defined that is used to indicate such a special
term.

.. rubric:: References

.. [BJKO00] M.G.J. van den Brand, H.A. de Jong, P. Klint, and P.A. Olivier.
   Efficient Annotated Terms. *Software — Practice & Experience*, 30:259–291, 2000.
