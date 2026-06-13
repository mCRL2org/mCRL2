Data library
============

The mCRL2 language describes processes with data. The Data Library contains
everything that has to do with the data part of the language. The main concepts
are sorts and functions working upon these sorts. The meaning of these
functions can be described by means of equational axioms.
In the :doc:`language reference </user_manual/language_reference/data>` these
concepts are explained in more detail.

The library provides:

- :doc:`Data specifications <data_library>` and expressions (this page)
- :doc:`Sort aliases and sort normalisation <data_sort_normalisation>` — making sort representations unique across a specification
- :doc:`Data rewriters <data_rewriters>` — evaluating data expressions using equational axioms
- :doc:`Rewriter implementation notes <data_rewriter_implementation>` — mathematical foundations (HRS theory, match trees)
- :doc:`Capture-avoiding substitutions <data_substitutions>` — formal substitution over data expressions
- :doc:`Data enumerator <data_enumerator>` — enumerating values satisfying a predicate

.. toctree::
   :hidden:
   :maxdepth: 1

   data_sort_normalisation
   data_rewriters
   data_rewriter_implementation
   data_substitutions
   data_enumerator

Data specifications
-------------------

Data specifications contains the declaration of data types.
It contains sorts, sort aliases, constructors, mappings and conditional equations.

A specification can straightforwardly be constructed by declaring
the required objects and adding them to a specification. The elements
that are added to the specification are not checked.


.. code-block:: c++

   #include "mcrl2/data/data_specification.h"
   #include "mcrl2/data/basic_sort.h"
   #include "mcrl2/data/function_symbol.h"
   #include "mcrl2/data/variable.h"
   #include "mcrl2/data/data_equation.h"

   using namespace mcrl2::data;

   data_specification spec;
   basic_sort D("D");                                          // sort D
   spec.add_sort(D);
   function_symbol m("m",D);                                   // map  m:D;
   function_symbol c1("c1",D);                                 // cons c1:D;
   function_symbol c2("c2",function_sort(D,D));                //      c2:D->D;
   spec.add_mapping(m);
   spec.add_constructor(c1);
   spec.add_constructor(c2);
   variable x("x", D);                                         // var  x:D;
   data_application e1(c2,x);
   data_equation e(variable_list({x}), sort_bool::true_(), e1, m); // eqn  true -> c1(x)=m;
   spec.add_equation(e);
   basic_sort E("E");                                          // sort E=D; (add a sort alias)
   spec.add_alias(E,D);

For any specification it is possible to retrieve the elements that have been added to the
specification as follows:

.. code-block:: c++

   sorts_const_range s=spec.user_defined_sorts();
   constructors_const_range c=spec.user_defined_constructors();
   mappings_const_range m=spec.user_defined_mappings();
   equations_const_range e=spec.user_defined_equations();
   ltr_aliases_mapping a=spec.user_defined_aliases();

If sorts are added to a data specification, automatically certain functions,
mappings and in some cases even derived sorts are added to the specification.
For every sort D, there functions if then else (if:Bool#D#D->D), equality (==:D#D->Bool),
inequality (!=:D#D->Bool) and inequalities (<,<=,>=,>:D#D->Bool) are added.
For structured sorts (e.g. sort Tree=struct leaf(Nat) | node(Nat,Nat)) the constructors,
projection functions and recognizers are added to the specification. For container sorts
(List(D), Set(D) and Bag(D) for arbitrary sort D) all standard functions for these
sorts are also added automatically to the specification. The same holds for standard
sorts Bool (booleans),
Pos (Positive numbers), Nat (Natural numbers), Int (Integers) and Real (Real numbers).

Standards sorts cannot explicitly
be added to a specification. In order to indicate that certain sorts must be present
in a data specification, these must be added explicitly to the context sorts.
The sort Bool is always present in a specification. Sorts that occur
in other sorts, constructors and mappings are automatically defined.
For instance, if the sort Real and its operations should be made available in a specification, it
is sufficient to add Real to the context sorts as follows:

.. code-block:: c++

   add_context_sort(sort_real::real());

The function context_sorts gives a list of sorts added to the context.

In order to retrieve all sorts, constructors, mappings or equations in
a specification (including those that are automatically generated),
there are functions listed below. As generally these functions are of
interest, instead of their counterparts which only define the user_defined
elements, they have the shorter and more natural names.

.. code-block:: c++

   sorts_const_range s=spec.sorts();
   constructors_const_range c=spec.constructors();
   mappings_const_range m=spec.mappings();
   equations_const_range e=spec.equations();
   ltr_aliases_mapping a=spec.aliases();

When adding sort aliases to a specification, the names of sorts are not
unique anymore. When declaring in mCRL2

.. code-block:: mcrl2

   sort Time=Nat;
        D=List(Nat);
        E=struct f(E)|g;

the sorts Time and Nat, as well as D and List(Nat) and even
E and struct f(E)|g are pairwise equal. In a specification it is not very
inefficient to have different names for equal sorts. Therefore the sorts
in a specification are made unique. The algorithm that is used maps every
structured and container sort for which an alias is introduced to the
sort alias at the left hand side. Every sort alias between basic sorts
is mapped to the right hand side. In the example above every occurrence
of Time is replaced by Nat, and occurrences of List(Nat) and struct f(E)|g
are replaced by D and E respectively.

The method sort_alias_map() delivers a mapping from sorts to sorts giving
for each sort the unique name. Using the function template <class T> T normalise_sorts(T t)
the sorts in each object t can be renamed to their unique representation.
If this is not done, objects can be equal except for their types, and this
will not be recognized. This is particularly problematic when using the
rewriter. Eg. in the following process specification

.. code-block:: mcrl2

   sort Time=Nat;
   map too_late:Time->Bool;
   var t:Time;
   eqn too_late(t) = t>10;
   proc P(u:Time)=too_late(u) -> a.delta;
   init P(9);

the data specification will normalise the equation too_late(t)= t>10 to
such that t has sort Nat as all occurrences of the sort Time are replaced
by Nat. When this is not done in process P, the parameter u still has sort
Time and too_late(u) will not be rewritten as the sorts do not match.
Therefore, it is necessary to apply normalise_sorts to any object used
in the context of a specification. If sort aliases are added to a
data specification, all sorts in the context of this specification
must be renormalised.

There are a few utility functions that help to determine the nature
of sorts. The function bool is_certainly_finite(const sort_expression) indicates
that a sort has a finite number of elements. This is in general an undecidable
property, but in certain cases it can be determined that there are at most
a finite number of elements in a sort.

The function bool is_constructor_sort(const sort_expression s) indicates
whether there is a constructor with target sort s. If so, the sort is
called a constructor sort.

.. _data-expressions:

Expressions
-----------

In this section we first introduce the basic structures of sort expressions
and data expressions. We then continue to defining the sort expressions
with operations that are predefined in the Data Library.
The code in the Data Library is inside the namespace ``mcrl2::data``.

Sort expressions
^^^^^^^^^^^^^^^^

Except for the untyped identifiers, all expressions in the Data Library
are typed. There are many different kinds of sorts in the mCRL2 language, all
of which can be represented in the data library.

.. table:: Sort expressions

   =========================  =========================================
   Type                       Meaning
   =========================  =========================================
   basic_sort                 basic sort
   function_sort              function sort
   structured_sort            structured sort
   container_sort             container sort
   multiple_possible_sorts    expression matching any of multiple sorts
   unknown_sort               unknown sort expression
   =========================  =========================================

.. warning::
   The types ``multiple_possible_sorts`` and ``unknown_sort`` should not occur
   after type checking.

These sort expressions correspond to the grammar:

.. productionlist::
   S: `Sb` | `Sc` | `S` x ... x `S` -> `S` | `Sstruct`
   Sc: List(`S`) | Set(`S`) | FSet(`S`) | Bag(`S`) | FBag(`S`)
   Sstruct: p ( proj* )? p
   proj: `S` | p : `S`

where ``Sb`` is a given set of basic sorts, always including the booleans
(sort ``Bool``). ``S x ... x S -> S`` denotes the function sorts, where ``->`` is right
associative. ``Sc`` is the set of container sorts, and ``Sstruct`` is the set of
structured sorts. ``FSet(S)`` and ``FBag(S)`` represent finite sets and finite bags
respectively.

In general, structured sorts have the following form (with ``n`` a positive number,
``ki`` a natural number with ``1 <= i <= n``):

.. code-block:: mcrl2

   struct c1(pr1,1:S1,1, ..., pr1,k1:S1,k1)?is_c1 |
          c2(pr2,1:S2,1, ..., pr2,k2:S2,k2)?is_c2 |
          ...
          cn(prn,1:Sn,1, ..., prn,kn:Sn,kn)?is_cn;

We refer to ``ci`` as the constructors of the structured sort. ``Si,j`` are the
sorts of the arguments of the constructors. ``pri,j`` are names for optional
projection functions, retrieving the corresponding argument for a constructor.
``is_ci`` are the names of optional recognizer functions, returning a boolean
value.

As an example of some of the introduced concepts, consider the following code
snippet that constructs a structured sort

.. code-block:: mcrl2

   struct c1(p0:S0, S1)?is_c1 |
          c2(p0:S0);

The construction of this structured sort is as follows, assuming that also all
of the subexpressions still need to be defined:

.. code-block:: c++

   basic_sort s0("S0");                               /* Name for the sort S0 */
   basic_sort s1("S1");                               /* Name for the sort S1 */
   structured_sort_constructor_argument p0(s0, "p0"); /* Constructor argument p0: S0 */
   structured_sort_constructor_argument p1(s1);       /* Constructor argument S1 */

   structured_sort_constructor_argument_vector a1;    /* p0: S0, S1 */
   a1.push_back(p0);
   a1.push_back(p1);
   structured_sort_constructor_argument_vector a2;    /* p0 */
   a2.push_back(p0);

   structured_sort_constructor c1("c1", a1, "is_c1"); /* c1(p0:S0, S1)?is_c1 */
   structured_sort_constructor c2("c2", a2);          /* c2(p0:S0) */

   structured_sort_constructor_vector cs;             /* c1(p0:S0, S1)?is_c1 | c2(p0:S0) */
   cs.push_back(c1);
   cs.push_back(c2);

   structured_sort s(cs);                             /* struct c1(p0:S0, S1)?is_c1 | c2(p0:S0) */

Data expressions
^^^^^^^^^^^^^^^^

The class ``data_expression`` represents expressions like ``true``,
:math:`x > 3` and :math:`\forall n {:} \mathit{Nat}.\ f(n) < 5`. Each data
expression ``d`` has a type or sort ``d.sort()`` of type ``sort_expression``.
Let's look at a simple example that constructs the numbers two and three, and
builds the expression 2 + 3:

.. code-block:: c++

   #include "mcrl2/data/data.h"
   #include <cassert>

   using namespace mcrl2::data;

   int main()
   {
     data_expression two   = sort_nat::nat(2);
     data_expression three = sort_nat::nat(3);
     data_expression five  = sort_nat::plus(two, three);

     assert(five.sort() == sort_nat::nat());
     return 0;
   }

.. table:: Data Expressions

   =================  =======================================================
   Expression         Meaning
   =================  =======================================================
   data_expression    any data expression
   function_symbol    function symbol
   variable           variable
   abstraction        expression with variable binding
   lambda             lambda abstraction
   forall             universal quantification
   exists             existential quantification
   where_clause       where clause
   application        function application
   identifier         untyped identifier (not to be used after type checking)
   =================  =======================================================

.. warning::

   The expression ``identifier`` should not occur after type checking, as it entails an
   untyped sort expression, whereas all libraries and tools in the toolset in
   general assume fully typed expressions.

An overview of all data expressions in the Data Library is given in the table
above. More detailed, data expressions are divided into function symbols, represented
by the class ``function_symbol``, variables, represented by ``variable``,
abstractions, represented by the class ``abstraction``, where clauses,
represented by ``where_clause``, and applications of expressions to expressions,
represented by ``application``. Furthermore, when used in the initial phases
of parsing and type checking, the use of untyped identifiers, represented
by ``identifier`` is allowed.

Abstractions provide a mechanism for variable binding. As such, they are
further subdivided into lambda abstraction, represented by ``lambda``,
and universal and existential quantifications, represented by
``forall`` and ``exists`` respectively.

More formally, data expressions ``e``, with sort expression ``S`` and variable names
``x`` correspond to the following grammar:

.. productionlist::
   e: x | n | e(e, ..., e)
    : | lambda x:S, ..., x:S . e
    : | forall x:S, ..., x:S. e
    : | exists x:S, ..., x:S. e
    : | e whr x = e, ..., x = e end
    : | {x:S | e}

Here ``e(e,...,e)`` denotes application of data expressions, ``lambda x:S, ..., x:S . e``
denotes lambda abstraction, ``forall x:S, ..., x:S . e`` and ``exists x:S, ..., x:S . e``
denote universal and existential quantification. The form ``{x:S | e}`` is set or bag
comprehension: a set when ``e`` has sort ``Bool``, a bag when ``e`` has sort ``Nat``.

Predefined sorts
^^^^^^^^^^^^^^^^

The mCRL2 language has a number of predefined sorts, given in the table below:

.. table:: Predefined sorts

   ==================== ================
   Expression           Sort
   ==================== ================
   sort_bool::bool_()   booleans
   sort_pos::pos()      positive numbers
   sort_nat::nat()      natural numbers
   sort_int::int_()     integers
   sort_real::real()    real numbers
   ==================== ================

Furthermore, a number of container sorts is predefined. Assuming that `s` is
a sort expression, all container sorts are given in the table below:

.. table:: Container sorts

   ==================== ===========
   Expression           Type
   ==================== ===========
   sort_list::list(s)   lists
   sort_set::set_(s)    sets
   sort_fset::fset(s)   finite sets
   sort_bag::bag(s)     bags
   sort_fbag::fbag(s)   finite bags
   ==================== ===========

Note that the source code for all predefined sorts is generated from
specification files.

Operations on data expressions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Default operations
""""""""""""""""""
For all sorts, a number of operations is available by default. The corresponding
functions can be found in `standard.h`.

Let ``b`` be a data expressions of sort ``Bool``, and let ``x``
and ``y`` be two data expressions with the same sort. Then the following
operations are supported:

.. table:: Operations on all data types

   ====================  =========  =========================
   Expression            Syntax     Meaning
   ====================  =========  =========================
   equal_to(x, y)        x == y     equality
   not_equal_to(x, y)    x != y     inequality
   if_(b, x, y)          if(b,x,y)  conditional expression
   less(x,y)             x < y      less than
   less_equal(x,y)       x <= y     less than or equal to
   greater(x,y)          x > y      greater than
   greater_equal(x,y)    x >= y     greater than or equal to
   ====================  =========  =========================

For the predefined sorts, the most important operations are also available
by default.

.. note::

   In all definitions of operations on predefined sorts, elements of which
   the syntax starts with @ cannot directly be entered by the user when writing
   an mCRL2 specification. The @ means that the specified operation is
   implementation specific. Printing such an expression as feedback to the user
   should be prevented at all times.

**Booleans**

All standard operations for the Booleans are available in `bool.h`, and can be
found in the namespace ``data::sort_bool``. First of all
the two constants ``true`` and ``false`` can be constructed.

.. table:: Constructors for sort Bool

   ==========  ======  =======
   Expression  Syntax  Meaning
   ==========  ======  =======
   true()      true    true
   false()     false   false
   ==========  ======  =======

Furthermore the following functions are available on Booleans (for details
about the allowed types also see bool.spec). Let ``b`` and ``c`` be Boolean expressions.

.. table:: Functions for sort Bool

   ===============  =======  ===========
   Expression       Syntax   Meaning
   ===============  =======  ===========
   not_(b,c)        !b       negation
   and_(b,c)        b && c   conjunction
   or_(b,c)         b || c   disjunction
   implies(b,c)     b => c   implication
   ===============  =======  ===========

**Positive numbers**

All standard operations for positive numbers are available in `pos.h`, and can
be found in the namespace ``data::sort_pos``. The positive numbers have two
constructors, facilitating an encoding with size logarithmic in the number
that is represented.
Let ``b`` be a Boolean expression, and ``p`` be a positive expression.

.. table:: Constructors for sort Pos

   ===========  ==========  =======
   Expression   Syntax      Meaning
   ===========  ==========  =======
   c1()         @1          1
   cdub(b,p)    @cDub(b,p)  2*p + b
   ===========  ==========  =======

Furthermore the standard operations are available on Positive numbers.
Let ``b`` and ``c`` be Boolean expressions, and ``p``, ``q``, and ``r`` be positive
numbers.

.. table:: Functions for sort Pos

   ======================= ============  ===============================
   Expression              Syntax        Meaning
   ======================= ============  ===============================
   `max(p,q)`              max(p,q)      maximum
   `min(p,q)`              min(p,q)      minimum
   `abs(p)`                abs(p)        absolute value
   `succ(p)`               succ(p)       successor
   `plus(p,q)`             p+q           addition
   `add_with_carry(b,p,q)` @addc(b,p,q)  addition with carry (p + q + b)
   ======================= ============  ===============================

**Natural numbers**

All standard operations for natural numbers are available in `nat.h`, and can
be found in the namespace ``data::sort_nat``. The natural numbers have two
constructors, representing ``0`` and a positive number interpreted as a
natural number.

Let ``p`` be a positive expression.

.. table:: Constructors for sort Nat

   ==========  ========  ===============================
   Expression  Syntax    Meaning
   ==========  ========  ===============================
   `c0()`      @0        0
   `cnat(p)`   @cNat(p)  p interpreted as natural number
   ==========  ========  ===============================

Furthermore the standard operations are available on Natural numbers.
Let ``b`` and ``c`` be Boolean expressions, ``p``, ``q`` be positive numbers,
and ``n``, ``m``, ``u``, ``v`` be natural numbers.

.. note::

   Operations marked ``???`` in the following tables are implementation-internal
   operations whose precise semantics are not yet documented here.

.. table::  Functions for sort Nat

   ========================  =========================  =======================================
   Expression                Syntax                     Meaning
   ========================  =========================  =======================================
   Pos2Nat(p)                Pos2Nat(p)                 explicit conversion of ``p`` to sort Nat
   Nat2Pos(n)                Nat2Pos(n)                 explicit conversion of ``n`` to sort Pos
   max(p,n)                  max(p,n)                   maximum
   max(n,p)                  max(n,p)                   maximum
   max(m,n)                  max(m,n)                   maximum
   min(m,n)                  min(m,n)                   minimum
   abs(n)                    abs(n)                     absolute value
   succ(n)                   succ(n)                    successor
   pred(n)                   pred(n)                    predecessor
   dub(b,n)                  @dub(b,n)                  ???
   plus(p,n)                 p+n                        addition
   plus(n,p)                 n+p                        addition
   plus(m,n)                 m+n                        addition
   gtesubtb(b,p,q)           @gtesubtb(b,p,q)           substraction with borrow
   times(m,n)                m*n                        multiplication
   div(m,p)                  m div p                    integer division
   mod(m,p)                  m mod p                    modulus
   exp(p,n)                  p^n                        exponentiation
   exp(m,n)                  m^n                        exponentiation
   even(n)                   @even(n)                   predicate to indicate ``n`` is even
   monus(m,n)                @monus(m,n)                ``(m-n) max 0``
   swap_zero(m,n)            @swap_zero(m,n)            ???
   swap_zero_add(m,n,u,v)    @swap_zero_add(m,n,u,v)    ???
   swap_zero_min(m,n,u,v)    @swap_zero_min(m,n,u,v)    ???
   swap_zero_monus(m,n,u,v)  @swap_zero_monus(m,n,u,v)  ???
   swap_zero_lte(m,n,u,v)    @swap_zero_lte(m,n,u,v)    ???
   ========================  =========================  =======================================

To facilitate efficient rewriting, also a sort ``@NatPair`` is available. Code
for this is also present in ``nat.h``, in namespace ``data::sort_nat``.

Let ``m``, ``n`` be expressions of sort Nat.

.. table:: Constructors for sort @NatPair

   ==========  ===========  ===========
   Expression  Syntax       Meaning
   ==========  ===========  ===========
   cpair(m,n)  @cPair(m,n)  tuple (m,n)
   ==========  ===========  ===========

Also functions for these pairs are available.
Let ``b`` be a Boolean expression, ``p``, ``q`` be positive numbers,
and ``n``, ``m``, ``u``, ``v`` be natural numbers.

.. table:: Functions for sort @NatPair

   ========================  ==========================  =====================================================
   Expression                Syntax                      Meaning
   ========================  ==========================  =====================================================
   first(cpair(m,n))         @first(@cPair(m,n))         projection of first argument
   second(cpair(m,n))        @second(@cPair(m,n))        projection of second argument
   divmod(p,q)               @divmod(p,q)                simultaneous division and modulus
   gdivmod(pair(m,n), b, p)  @gdivmod(@pair(m,n), b, p)  generalised simultaneous division and modulus
   ggdivmod(m, n, p)         @ggdivmod(m ,n, p)          doubly generalised simultaneous division and modulus
   ========================  ==========================  =====================================================

**Integers**

All standard operations for integers are available in `int.h`, and can
be found in the namespace ``data::sort_int``. The integers have two
constructors, one interpreting a natural number as integer, and one
interpreting a positive number as a negative integer.

Let ``p`` be a positive expression, and ``n`` be a natural number.

.. table:: Constructors for sort Int

   ==========  ========  =====================================
   Expression  Syntax    Meaning
   ==========  ========  =====================================
   cint(n)     @cInt(n)   ``n`` interpreted as an integer
   cneg(p)     @cNeg(p)   ``p`` interpreted as the integer ``-p``
   ==========  ========  =====================================

Furthermore the standard operations are available on Natural numbers.
Let ``b`` be a Boolean expression, ``p``, ``q`` be positive numbers,
``n``, ``m`` be natural numbers, and ``x``, ``y`` be integers.

.. table:: Functions for sort Int

   ==========  ==========     =======================================
   Expression  Syntax         Meaning
   ==========  ==========     =======================================
   Nat2Int(n)  Nat2Int(n)     explicit conversion of ``n`` to sort Int
   Int2Nat(x)  Int2Nat(x)     explicit conversion of ``x`` to sort Nat
   Pos2Int(p)  Pos2Int(p)     explicit conversion of ``p`` to sort Int
   Int2Pos(x)  Int2Pos(x)     explicit conversion of ``x`` to sort Pos
   max(p,x)    max(p,x)       maximum
   max(x,p)    max(x,p)       maximum
   max(n,x)    max(n,x)       maximum
   max(x,n)    max(x,n)       maximum
   max(x,y)    max(x,y)       maximum
   min(x,y)    min(x,y)       minimum
   abs(x)      abs(x)         absolute value
   negate(p)   -p             unary minus
   negate(n)   -n             unary minus
   negate(x)   -x             unary minus
   succ(x)     succ(x)        successor
   pred(n)     pred(n)        predecessor
   pred(x)     pred(x)        predecessor
   dub(b,x)    @dub(b,x)      ???
   plus(x,y)   x+y            addition
   minus(p,q)  p-q            subtraction
   minus(n,m)  n-m            subtraction
   minus(x,y)  x-y            subtraction
   times(x,y)  x*y            multiplication
   div(x,p)    x div p        integer division
   mod(x,p)    x mod p        modulus
   exp(x,n)    x^n            exponentiation
   ==========  ==========     =======================================

**Real numbers**

All standard operations for real numbers are available in `real.h`, and can
be found in the namespace ``data::sort_real``. The real numbers do not have
any constructors, because they cannot be finitely enumerated.

Standard functions for real are available however.
Let ``p``, ``q`` be positive numbers,``n``, ``m`` be natural numbers, ``x``, ``y`` be integers,
and ``r``, ``s`` be real numbers.

.. table:: Functions for sort Real

   =================  ==================  ===================================================
   Expression         Syntax              Meaning
   =================  ==================  ===================================================
   Pos2Real(p)        Pos2Real(p)         explicit conversion of ``p`` to sort Real
   Nat2Real(n)        Nat2Real(n)         explicit conversion of ``n`` to sort Real
   Int2Real(x)        Int2Real(x)         explicit conversion of ``x`` to sort Real
   Real2Pos(r)        Real2Pos(r)         explicit conversion of ``r`` to sort Pos
   Real2Nat(r)        Real2Nat(r)         explicit conversion of ``r`` to sort Nat
   Real2Int(r)        Real2Int(r)         explicit conversion of ``r`` to sort Int
   max(r,s)           max(r,s)            maximum
   min(r,s)           min(r,s)            minimum
   abs(r)             abs(r)              absolute value
   negate(r)          -r                  unary minus
   succ(r)            succ(r)             successor
   pred(r)            pred(r)             predecessor
   plus(r,s)          r+s                 addition
   minus(r,s)         r-s                 subtraction
   times(r,s)         r*s                 multiplication
   divides(p,q)       p / q               division
   divides(m,n)       m / n               division
   divides(x,y)       x / y               division
   divides(r,s)       r / s               division
   floor(r)           floor(r)            floor
   ceil(r)            ceil(r)             ceil
   round(r)           round(r)            round
   redfrac(x,y)       @redfrac(x,y)       reduce fraction x/y w.r.t. lowest common multiple
   redfracwhr(p,x,n)  @redfracwhr(p,x,n)  ???
   redfrachlp(r,x)    @redfrachlp(r,x)    ???
   =================  ==================  ===================================================

.. important::

   The sorts that are allowed as arguments to the functions for numeric sorts
   are exactly the ones that correspond to the sorts of the variables in the tables
   with functions. Note that e.g. `sort_real::max(p,q)` is also allowed, and the
   correct result sort of ``Pos`` will automatically be inferred.

**Lists**

All standard operations for lists are available in `list.h`, and can
be found in the namespace data::sort_list. The lists have two
constructors, the empty list ([]), and inserting an element into a list (\|>).

Let x be an element of sort S, and l of sort List(S).

.. table:: Constructors for sort List(S)

   ===========  =======  ==============================
   Expression   Syntax   Meaning
   ===========  =======  ==============================
   nil(S)       []       The empty list of sort S
   cons(S,x,l)  x \|> l  The list l prefixed with x
   ===========  =======  ==============================

Also, the following functions operating on lists are available. Again,
let x be an element of sort S, l of sort List(S), and n of sort Nat.

.. table:: Functions for sort List(S)

   =================  =========  ==================================================
   Expression         Syntax     Meaning
   =================  =========  ==================================================
   in(S,x,l)          x in l     Test whether ``x`` is an element of ``l``
   count(S,l)         #l         The size of ``l``
   snoc(S,l,x)        l <| x     The list ``l`` suffixed with ``x``
   concat(S,l,l')     l ++ l'    The concatenation of ``l`` and ``l'``
   element_at(S,l,n)  l.n        The element at position ``n`` in ``l``
   head(S,l)          head(l)    The first element of ``l``
   tail(S,l)          tail(l)    ``l`` from which the first element has been removed
   rhead(S,l)         rhead(l)   The last element of ``l``
   rtail(S,l)         rtail(l)   ``l`` from which the last element has been removed
   =================  =========  ==================================================

**Finite sets**

The finite sets quite closely resemble lists. For sort ``FSet(S)`` the following
constructors are available, assuming a sort ``S``, an element ``x`` of sort ``S``, and
``t`` being of sort ``FSet(S)``.

.. table:: Constructors for sort FSet(S)

   ================  =============  ====================================
   Expression        Syntax         Meaning
   ================  =============  ====================================
   fset_empty(S)     @fset_empty()  The empty finite set of sort ``s``
   fset_cons(S,x,t)  @fset_cons     The finite set ``t`` extended with ``x``
   ================  =============  ====================================


Let ``b`` be a Boolean, ``x`` an element of sort ``S``, ``f``, ``g`` be functions of sort
``S -> Bool``, and ``s``, ``t`` be of sort ``FSet(S)``. The operations of finite
sets are defined as follows.

.. table:: Functions for sort FSet(S)

   ===========================  ===================  ===========================
   Expression                   Syntax               Meaning
   ===========================  ===================  ===========================
   insert(S,x,s)                @fsetinsert(x,s)     Insert ``x`` into ``s``
   cinsert(S,x,b,s)             @fsetinsert(x,b,s)   ???
   in(S,x,s)                    @fsetin(x,s)         Test whether ``x`` is in ``s``
   lte(S,f,s,t)                 @fsetlte(f,s,t)      ``s`` is a subset of ``t``
   union(S,f,g,s,t)             @fsetunion(f,g,s,t)  Union of ``s`` and ``t``
   intersection(S,f,g,s,t)      @fsetinter(f,g,s,t)  Intersection of ``s`` and ``t``
   ===========================  ===================  ===========================


**Finite bags**

Finite bags are defined in a similar vein as finite sets.
For sort ``FBag(S)`` the following
constructors are available, assuming a sort ``S``, an element ``x`` of sort ``S``,
``p`` being a positive number, and
``b`` being of sort ``FBag(S)``.

.. table:: Constructors for sort FSet(S)

   ==================  ================  =========================================================
   Expression          Syntax            Meaning
   ==================  ================  =========================================================
   fbag_empty(S)       @fbag_empty()     The empty finite bag of sort ``s``
   fbag_cons(S,x,p,b)  @bag_cons(x,p,b)  The finite bag ``b``, extended with ``p`` occurrences of ``x``
   ==================  ================  =========================================================

Let ``x`` an element of sort ``S``, ``f``,``g`` be functions of sort
``S -> Nat``, ``t`` of sort ``FSet(S)``, and ``b``,``c`` be elements of sort ``FBag(S)``.
The operations on finite bags are defined as follows.

.. table:: Functions for sort FBag(S)

   =========================  ====================  ===================================================
   Expression                 Syntax                Meaning
   =========================  ====================  ===================================================
   fbaginsert(S,x,p,b)        @fbag_insert(x,p,b)   Insert ``p`` occurrences of ``x`` into ``b``
   fsetcinsert(S,x,n,b)       @fbag_cinsert(x,n,b)  ???
   fbagcount(S,x,b)           @fbag_count(x,b)      Test count the number of occurrences of ``x`` in ``b``
   fbagin(S,x,b)              @fbag_in(x,b)         Test whether ``x`` is in ``b``
   fbaglte(S,f,b,c)           @fbag_lte(f,b,c)      ``b`` is a subset of ``c``
   fbagjoin(S,f,g,b,c)        @fbag_join(f,g,b,c)   Join of ``b`` and ``c``
   fbagintersect(S,f,g,b,c)   @fbag_inter(f,g,b,c)  Intersection of ``b`` and ``c``
   fbagdifference(S,f,g,b,c)  @fbag_diff(f,g,b,c)   Difference of ``b`` and ``c``
   fset2fbag(S,t)             @fset2fbag(t)         Convert ``t`` to a finite bag
   =========================  ====================  ===================================================

**Sets**

Like the Real numbers, sets and bags do not have
constructors. This means that elements of these sorts are built using functions,
as well as their more simple counterparts, the finite sets and bags.

For sets the following functions are available. Let ``d``, ``e`` be of sort Set(S),
``x`` be of sort ``S``, ``s`` be of sort ``FSet(S)``, and ``f`` and ``g`` be function of
sort ``S -> Bool``.

.. table:: Functions for sort Set(S)

   =======================  ===========  ===================================================================
   Expression               Syntax       Meaning
   =======================  ===========  ===================================================================
   setconstructor(S, f, s)  @set(f,s)    Construct a set from a function and a finite set
   emptyset(S)              {}           Empty set of sort S
   setfset(S, s)            @setfset(s)  Interpret finite set s as a set
   setcomprehension(S, f)   @setcomp(f)  The set of all elements of sort ``S`` satisfying ``f``
   in(S,x,d)                x in d       Test whether ``x`` is an element of ``d``
   setcomplement(S,d)       !d           Set complement of ``d``
   setunion_(S,d,e)         d + e        Union of ``d`` and ``e``
   setintersection(S,d,e)   d * e        Intersection of ``d`` and ``e``
   setdifference(S,d,e)     d - e        Difference of ``d`` and ``e``
   false_function(S)        @false_      The constant function returning false
   true_function(S)         @true_       The constant function returning true
   false_function(S)        @false_      The constant function returning false
   not_function(S,f)        @not_(f)     The function returning ``!f(x)`` for all elements ``x`` in ``S``
   and_function(S,f,g)      @and_(f,g)   The function returning ``f(x) && g(x)`` for all elements ``x`` in ``S``
   or_function(S,f,g)       @or_(f,g)    The function returning ``f(x) || g(x)`` for all elements ``x`` in ``S``
   =======================  ===========  ===================================================================

Note that the \*_function operations are used as implementation details
for representing sets.

**Bags**

For bags the following functions are available. Let ``b``, ``c`` be of sort ``FBag(S)``,
``e`` of sort ``S``, ``f``,``g``, of sort ``S -> Nat``, ``h`` of sort ``S -> Bool``, ``s`` of sort
``FSet(S)``, and ``x``,``y`` of sort ``Bag(S)``.

.. table:: Functions for sort Bag(S)

   =======================  =============  ========================================================================
   Expression               Syntax         Meaning
   =======================  =============  ========================================================================
   bagconstructor(S, f, b)  @bag(f,b)      Construct a bag from a function and a finite bag
   emptybag(S)              {}             Empty bag of sort S
   bagfbag(S, b)            @bagfset(b)    Interpret finite bag ``b`` as a bag
   bagcomprehension(S, f)   @bagcomp(f)    The bag of all elements of sort ``S`` given by ``f``
   bagcount(S,e,x)          count(e,x)     The number of occurrences of ``e`` in ``x``
   bagin(S,e,x)             in(e,x)        Determine whether ``e`` occurs in ``x``
   bagjoin(S,x,y)           x + y          Join of ``x`` and ``y``
   bagintersect(S,x,y)      x * y          Intersection of ``x`` and ``y``
   bagdifference(S,x,y)     x - y          Difference of ``x`` and ``y``
   bag2set(S,x)             Bag2Set(x)     Convert bag ``x`` to a set
   set2bag(S,t)             Set2Bag(t)     Convert set ``t`` to a bag
   zero_function(S)         @zero_         The constant function returning ``0``
   one_function(S)          @one_          The constant function returning ``1``
   add_function(S,f,g)      @add_(f,g)     The function returning ``f(x) + g(x)`` for all elements ``x`` in ``S``
   min_function(S,f,g)      @min_(f,g)     The function returning ``min(f(x),g(x))`` for all elements ``x`` in ``S``
   monus_function(S,f,g)    @monus_(f,g)   The function returning ``monus(f(x),g(x))`` for all elements ``x`` in ``S``
   nat2bool_function(S,f)   @Nat2Bool_(f)  The function returning false if ``f(x)``0``, and true otherwise
   bool2nat_function(S,h)   @Bool2Nat_(h)  The function returning ``1`` if ``f(x)``, ``0`` otherwise
   =======================  =============  ========================================================================

Note that, like for sets, the \*_function operations are used as implementation
details for representing bags.

Creating data expressions
^^^^^^^^^^^^^^^^^^^^^^^^^

Data expressions can be created in two ways: directly using constructors,
or using a parser.

Constructing data expressions directly can be quite tedious:

.. code-block:: c++

   basic_sort X("X");
   basic_sort Y("Y");
   basic_sort Z("Z");
   sort_expression XYZ = function_sort(function_sort(X, Y), Z);
   variable x("x", XYZ);

   variable three("3", sort_pos::pos());
   variable zero("0", sort_nat::nat());

For convenience a function `parse_data_expression` is available. This function
takes a variable declaration as optional second argument, that can be used to
specify unbound variables that appear in the expression. An example of this
is:

.. code-block:: c++

   #include "mcrl2/data/parse.h"
   #include "mcrl2/data/pos.h"
   #include "mcrl2/data/nat.h"

   int main()
   {
     // two ways to create the expression m + n
     std::string var_decl = "m, n: Pos;\n";
     data_expression d1 = parse_data_expression("m+n", var_decl);

     variable m = parse_data_expression("m", var_decl);
     variable n = parse_data_expression("n", var_decl);
     data_expression d2 = sort_pos::plus(m, n);

     return 0;
   }

.. seealso::

   :doc:`Sort aliases and sort normalisation <data_sort_normalisation>` — required
   reading before using the rewriter; sorts must be normalised so that expressions
   with aliased sorts are recognised as equal.

   :doc:`Data rewriters <data_rewriters>` — evaluating and simplifying data expressions.

Formal foundations
------------------

This section gives the mathematical definitions underlying the mCRL2 data
language. Practical usage is described in the sections above; the material here
provides the formal reference for the concepts implemented in the library.

Data specification
^^^^^^^^^^^^^^^^^^

.. admonition:: Definition (Data specification)

   A *data specification* is a triple

   .. math:: D = (S,\, \Omega,\, E)

   where :math:`S` is a set of sorts, :math:`\Omega` is a set of operations, and
   :math:`E` is a set of equations. In an mCRL2 specification, sorts are
   declared with the ``sort`` keyword, constructors with ``cons``, mappings with
   ``map``, and equations with ``eqn``.

Sort expressions
^^^^^^^^^^^^^^^^

We assume a fixed set of *basic sorts* :math:`S_\mathit{Basic}`, always
containing the booleans :math:`\mathbb{B}`, positive naturals
:math:`\mathbb{N}^+`, naturals :math:`\mathbb{N}`, integers :math:`\mathbb{Z}`,
and reals :math:`\mathbb{R}`.

.. admonition:: Definition (Sort expressions)

   Sort expressions :math:`S` are defined as follows (:math:`\rightarrow`
   right-associative):

   .. math::

      S\ ::=\ S_\mathit{Basic}\ \mid\ S_\mathit{Container}\
              \mid\ S \times \cdots \times S \rightarrow S\
              \mid\ S_\mathit{Struct}

   with container sorts

   .. math::

      S_\mathit{Container}\ ::=\ \mathit{List}(S)\ \mid\ \mathit{Set}(S)\ \mid\ \mathit{Bag}(S)

   In :math:`S_0 \times \cdots \times S_n \rightarrow S`, the sorts
   :math:`S_0, \ldots, S_n` are the *domain* and :math:`S` is the *codomain*.
   Sorts outside :math:`S_\mathit{Basic} \cup S_\mathit{Container} \cup
   S_\mathit{Struct}` are *function sorts*.

The language also supports *sort aliases* :math:`S_0 = S_1`; only one of the
two is treated as the canonical sort.

.. admonition:: Example (Sort aliases)

   Given alias :math:`\mathit{LNat} = \mathit{List}(\mathit{Nat})`,
   data expressions of sort :math:`\mathit{LNat}` and of sort
   :math:`\mathit{List}(\mathit{Nat})` are interchangeable.

.. admonition:: Definition (Variables)

   We assume a set :math:`V` of variable names with associated sorts. We write
   :math:`V_s` for the variables of sort :math:`s`.

Operations
^^^^^^^^^^

.. admonition:: Definition (Operations)

   The set of operations :math:`\Omega` consists of *constructors*
   :math:`\Omega_C` and *mappings* :math:`\Omega_M`:

   .. math:: \Omega = \Omega_C \cup \Omega_M

   Every element is a typed symbol :math:`n : S`. Constructors are restricted
   to basic-sort codomains:

   .. math::

      \Omega_C\ ::=\ n : S_B\ \mid\ n : S \times \cdots \times S \rightarrow S_B

   We write :math:`\Omega_{C,s}` for the constructors whose codomain is :math:`s`.

.. admonition:: Definition (Signature)

   A *signature* :math:`\Sigma = (S_\mathit{Basic},\, \Omega)` pairs a set of
   basic sorts with a set of operations.  The signature always contains at least
   :math:`\mathbb{B},\, \mathbb{N}^+,\, \mathbb{N},\, \mathbb{Z},\, \mathbb{R}`.

Data expressions
^^^^^^^^^^^^^^^^

.. admonition:: Definition (Data expressions)

   Data expressions :math:`e`, with sort expressions :math:`S` and variables
   :math:`x`, are defined inductively:

   .. math::

      e\ ::=\ x\ \mid\ n\ \mid\ e(e,\ldots,e)
             \ \mid\ \lambda\,x{:}S,\ldots,x{:}S.\,e
             \ \mid\ \forall\,x{:}S,\ldots,x{:}S.\,e
             \ \mid\ \exists\,x{:}S,\ldots,x{:}S.\,e
             \ \mid\ e\ \mathbf{whr}\ x=e,\ldots,x=e\ \mathbf{end}
             \ \mid\ \{x{:}S\mid e\}

   Here :math:`e(e,\ldots,e)` is application, :math:`\lambda\ldots` is
   abstraction, and :math:`\{x:S\mid e\}` is set or bag comprehension (a set
   when :math:`e:\mathbb{B}`, a bag when :math:`e:\mathbb{N}`).

.. admonition:: Convention (Binding operators)

   We write :math:`\Lambda` to denote any binding operator
   (:math:`\lambda`, :math:`\forall`, :math:`\exists`, :math:`\{\}`) when
   stating rules that apply to all of them uniformly.

.. admonition:: Convention (System-defined operators)

   System-defined operators are written infix; for example :math:`b_1 \land b_2`
   for :math:`\mathit{and}(b_1,b_2)`. Standard operator precedence applies.

Valid data expressions
^^^^^^^^^^^^^^^^^^^^^^

Type validity is defined relative to a *context* :math:`\Gamma`—a set of
typing statements for variables and operations. We write
:math:`\Gamma, x : s` for :math:`\Gamma \cup \{x : s\}` and
:math:`\exists^1_s` to mean *exactly one* such sort :math:`s` exists.

.. admonition:: Definition (Valid data expressions)

   .. math::

      \dfrac{x : s \in \Gamma}{\Gamma \vdash x : s}\ (\mathit{Var})
      \qquad\qquad
      \dfrac{n : s \in \Gamma}{\Gamma \vdash n : s}\ (\mathit{Op})

   .. math::

      \dfrac{
        \Gamma,\,x_0{:}s_0,\ldots,x_n{:}s_n \vdash e : s
      }{
        \Gamma \vdash (\lambda\,x_0{:}s_0,\ldots,x_n{:}s_n.\,e)
          : s_0 \times \cdots \times s_n \rightarrow s
      }\ (\mathit{Abs})

   .. math::

      \dfrac{
        \exists^1_{s_0,\ldots,s_n}\!\Bigl(
          \Gamma \vdash t : s_0 \times \cdots \times s_n \rightarrow s
          \quad \Gamma \vdash t_0 : s_0 \quad \cdots \quad
          \Gamma \vdash t_n : s_n
        \Bigr)
      }{
        \Gamma \vdash t(t_0,\ldots,t_n) : s
      }\ (\mathit{Appl})

   .. math::

      \dfrac{
        \exists^1_{s_0,\ldots,s_n}\!\Bigl(
          \Gamma \vdash x_i : s_i,\;
          \Gamma \vdash e_i : s_i\; (0 \le i \le n),\;
          \Gamma,\,x_0{:}s_0,\ldots,x_n{:}s_n \vdash e : s
        \Bigr)
      }{
        \Gamma \vdash (e\ \mathbf{whr}\ x_0=e_0,\ldots,x_n=e_n\ \mathbf{end}) : s
      }\ (\mathit{Where})

   .. math::

      \dfrac{
        \Gamma,\,x_0{:}s_0,\ldots,x_n{:}s_n \vdash e : \mathbb{B}
      }{
        \Gamma \vdash (\forall\,x_0{:}s_0,\ldots,x_n{:}s_n.\,e) : \mathbb{B}
      }\ (\mathit{Forall})
      \qquad
      \dfrac{
        \Gamma,\,x_0{:}s_0,\ldots,x_n{:}s_n \vdash e : \mathbb{B}
      }{
        \Gamma \vdash (\exists\,x_0{:}s_0,\ldots,x_n{:}s_n.\,e) : \mathbb{B}
      }\ (\mathit{Exists})

   .. math::

      \dfrac{
        \Gamma,\,x{:}s \vdash e : \mathbb{B}
      }{
        \Gamma \vdash \{x{:}s\mid e\} : \mathit{Set}(s)
      }\ (\mathit{SetComp})
      \qquad
      \dfrac{
        \Gamma,\,x{:}s \vdash e : \mathbb{N}
      }{
        \Gamma \vdash \{x{:}s\mid e\} : \mathit{Bag}(s)
      }\ (\mathit{BagComp})

Equations
^^^^^^^^^

.. admonition:: Definition (Equations)

   The syntax of equations is:

   .. math:: E\ ::=\ e = e\ \mid\ e \rightarrow e = e

   An unconditional equation has the form :math:`d = e`; a *conditional*
   equation :math:`c \rightarrow d = e` requires :math:`c` to be true.

   Validity under context :math:`\Gamma`:

   .. math::

      \dfrac{
        \exists^1_s\!\bigl(\Gamma \vdash d : s \;\; \Gamma \vdash e : s\bigr)
      }{
        \Gamma \vdash d = e
      }\ (\mathit{Eq})
      \qquad
      \dfrac{
        \Gamma \vdash c : \mathbb{B} \quad
        \exists^1_s\!\bigl(\Gamma \vdash d : s \;\; \Gamma \vdash e : s\bigr)
      }{
        \Gamma \vdash c \rightarrow d = e
      }\ (\mathit{CondEq})

Semantics
^^^^^^^^^

.. admonition:: Definition (:math:`\Sigma`-algebra)

   A :math:`\Sigma`-algebra :math:`A` for
   :math:`\Sigma = (S_\mathit{Basic},\,\Omega)` assigns:

   - a *carrier set* :math:`A(s)` to each sort :math:`s`, containing all
     elements of that sort;
   - a total function :math:`A(n:s)` to each operation :math:`n:s \in \Omega`.

   All elements of :math:`A(s)` are obtainable by applying the constructors
   :math:`\Omega_{C,s}`.

.. admonition:: Example (:math:`\Sigma`-algebra for natural numbers)

   With :math:`\Omega_C = \{\mathit{zero}:\mathit{Nat},\;
   \mathit{succ}:\mathit{Nat}\rightarrow\mathit{Nat}\}` and
   :math:`\Omega_M = \{\mathit{add}:\mathit{Nat}\times\mathit{Nat}
   \rightarrow\mathit{Nat}\}`, one :math:`\Sigma`-algebra :math:`A` sets
   :math:`A(\mathit{Nat})=\mathbb{N}`, :math:`A(\mathit{zero})=0`,
   :math:`A(\mathit{succ})=\lambda n.\,n+1`, and
   :math:`A(\mathit{add})=\lambda m,n.\,m+n`.

An *assignment* :math:`\alpha : V \rightarrow A` is a family of functions
:math:`\alpha_s : V_s \rightarrow A(s)`. The *value* of expression :math:`e`
under :math:`A` and :math:`\alpha` is written :math:`A(\alpha)(e)`.

.. admonition:: Definition (Value of a data expression)

   .. math::

      \begin{aligned}
      A(\alpha)(x)
        &= \alpha_s(x) && x \in V_s\\
      A(\alpha)(n)
        &= A(n)\\
      A(\alpha)\bigl(e(u_0,\ldots,u_n)\bigr)
        &= A(\alpha)(e)\bigl(A(\alpha)(u_0),\ldots,A(\alpha)(u_n)\bigr)\\
      A(\alpha)(\Lambda\,x_0{:}s_0,\ldots,x_n{:}s_n.\,e)
        &= \widehat{\Lambda}\,d_0{\in}A(s_0),\ldots,d_n{\in}A(s_n).\,
           A\!\left(\alpha[x_i:=d_i]_{0\le i\le n}\right)(e)\\
      A(\alpha)(e\ \mathbf{whr}\ x_0{=}e_0,\ldots,x_n{=}e_n\ \mathbf{end})
        &= A\!\left(\alpha[x_i:=d_i]_{0\le i\le n}\right)(e),\quad
           d_i = A(\alpha)(e_i)
      \end{aligned}

   Here :math:`\widehat{\Lambda}` denotes abstraction in the semantic domain.

.. admonition:: Remark (Substitution vs. assignment)

   There is a close relation between the syntactic notion of substitution and
   the semantic notion of assignment: for all substitutions
   :math:`\sigma : V \rightarrow T_{\Sigma(W)}`, :math:`\Sigma`-algebras
   :math:`A`, assignments :math:`\beta : W \rightarrow A`, and data expressions
   :math:`e \in T_{\Sigma(V)}`,

   .. math:: A(\beta)(\sigma(e)) = A(\alpha)(e)

   where :math:`\alpha : V \rightarrow A` is defined by
   :math:`\alpha(x) = A(\beta)(\sigma(x))`.

   See the :doc:`Capture-avoiding substitutions <data_substitutions>` page for
   the full definition of syntactic substitution.

Equational logic
""""""""""""""""

.. admonition:: Definition (Satisfaction)

   For a :math:`\Sigma`-algebra :math:`A`, condition :math:`c`, and expressions
   :math:`d,e` of the same sort:

   .. math::

      A \vDash_{EL} c \rightarrow d = e
      \iff
      A(\alpha)(d) = A(\alpha)(e)\ \wedge\ A(\alpha)(c) = \mathit{true},
      \quad \text{for all } \alpha : V \rightarrow A

   If :math:`c` is omitted it is treated as :math:`\mathit{true}`.

.. admonition:: Definition (Model and logical consequence)

   A :math:`\Sigma`-algebra :math:`A` is a *model* of :math:`E` if
   :math:`A \vDash_{EL} eq` for all :math:`eq \in E`; we denote this
   :math:`A \vDash_{EL} E`. The class of all models is
   :math:`\mathit{Mod}_{EL}(E)`.

   An equation :math:`eq` is a *logical consequence* of :math:`E`, written
   :math:`E \vDash_{EL} eq`, if :math:`A \vDash_{EL} eq` for all
   :math:`A \in \mathit{Mod}_{EL}(E)`.

Finiteness of sorts
^^^^^^^^^^^^^^^^^^^

Determining whether a sort is finite underlies the ``is_certainly_finite``
function described under `Data specifications`_.

Let :math:`\mathit{DependentSorts} : \Omega_C \rightarrow 2^S` and
:math:`\mathit{Sorts} : S \rightarrow 2^S` be defined as follows:

.. math::

   \mathit{DependentSorts}(n : s) =
   \begin{cases}
     \emptyset
       & \text{if } s \in S_\mathit{Basic}\\[4pt]
     \displaystyle\bigcup_{0 \le i \le m}
       \bigl(\{s_i\} \cup \mathit{Sorts}(s_i)\bigr)
       & \text{if } s = s_0 \times \cdots \times s_m \rightarrow s'
   \end{cases}

.. math::

   \mathit{Sorts}(s) =
   \begin{cases}
     \displaystyle\bigcup_{n \in \Omega_{C,s}} \mathit{DependentSorts}(n)
       & \text{if } s \in S_\mathit{Basic}\\[8pt]
     \mathit{Sorts}(s')
       & \text{if } s \in S_\mathit{Container}\\[4pt]
     \displaystyle\bigcup_{0 \le i \le m} \mathit{Sorts}(s_i) \cup \{s'\}
       & \text{if } s = s_0 \times \cdots \times s_m \rightarrow s'\\[8pt]
     \displaystyle\bigcup_{i,j} \mathit{Sorts}(s_{i,j})
       & \text{if } s = \mathbf{struct}\ c_i(\ldots pr_{i,j}{:}s_{i,j}\ldots)
   \end{cases}

The predicate :math:`\mathit{Finite} : S \rightarrow \mathbb{B}` is:

.. math::

   \mathit{Finite}(s) =
   \begin{cases}
     \Omega_{C,s} \neq \emptyset
       \;\wedge\; s \notin \mathit{Sorts}(s)
       \;\wedge\; \forall n \in \Omega_{C,s},\,
         s' \in \mathit{DependentSorts}(n).\;
         \mathit{Finite}(s')
       & \text{if } s \in S_\mathit{Basic}\\[4pt]
     \mathit{Finite}(s')
       & \text{if } s = \mathit{Set}(s')\\[4pt]
     \mathit{false}
       & \text{if } s \in S_\mathit{Container},\;
         s \neq \mathit{Set}(s')\\[4pt]
     (\forall i.\;\mathit{Finite}(s_i))
       \;\wedge\; \mathit{Finite}(s')
       & \text{if } s = s_0 \times \cdots \times s_m \rightarrow s'\\[4pt]
     s \notin \mathit{Sorts}(s)
       \;\wedge\; \forall s'' \in \mathit{Sorts}(s).\;
         \mathit{Finite}(s'')
       & \text{if } s = \mathbf{struct}\ \ldots
   \end{cases}

Free variables and closed expressions
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. admonition:: Definition (Free variables)

   The set of *free variables* :math:`\mathit{FV}(e)` is defined inductively:

   .. math::

      \begin{aligned}
      \mathit{FV}(x) &= \{x\}\\
      \mathit{FV}(n) &= \emptyset\\
      \mathit{FV}\bigl(e(e_0,\ldots,e_n)\bigr)
        &= \mathit{FV}(e) \cup \bigcup_{0 \le i \le n} \mathit{FV}(e_i)\\
      \mathit{FV}(\lambda\,x_0{:}s_0,\ldots,x_n{:}s_n.\,e)
        &= \mathit{FV}(e) \setminus \{x_i \mid 0 \le i \le n\}
      \end{aligned}

.. admonition:: Definition (Closed expression)

   A data expression :math:`e` is *closed* iff :math:`\mathit{FV}(e) = \emptyset`.

Equality checking
^^^^^^^^^^^^^^^^^

Equality of data expressions can be checked by a rewriter or a prover; see
:doc:`Data rewriters <data_rewriters>`. An equality checker
:math:`\mathit{Eq}` must satisfy:

.. math::

   \begin{aligned}
   \mathit{Eq}(\mathit{true},\,\mathit{false}) &\equiv \mathit{false}\\
   \mathit{Eq}(e,\,e') &\implies e = e'
   \end{aligned}

That is, :math:`\mathit{true}` and :math:`\mathit{false}` are distinct, and
:math:`\mathit{Eq}` is sound: it only reports equality when it holds.

Historical notes
----------------

Design decisions
^^^^^^^^^^^^^^^^

The mCRL2 data language was designed with the following explicit constraints:

- **Layout-neutral semantics.** Whitespace and indentation have no effect on
  the semantics of a specification. This means that declarations must be
  terminated by a semicolon; without it, the grammar would be ambiguous.  For
  example, without semicolons the two lines::

     X = f(g)
     (k) = Y

  could be parsed either as ``X = f(g)`` and ``(k) = Y``, or as ``X = f`` and
  ``Y = (g)(k)``, because layout is not significant.

Comparison with related languages
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The data part of mCRL2 can be compared with related languages from the
functional programming world. The table below shows a number of aspects for
mCRL (the predecessor of mCRL2), mCRL2, Haskell/Clean, and MetaOCaml.

.. list-table:: Comparison of data languages
   :header-rows: 1
   :widths: 30 10 10 15 10

   * - Aspect
     - mCRL
     - mCRL2
     - Haskell/Clean
     - MetaOCaml
   * - Purely functional
     - yes
     - yes
     - yes
     - no
   * - Expressiveness
     - first-order
     - higher-order
     - higher-order
     - higher-order
   * - Strict
     - no
     - no
     - no
     - yes
   * - Evaluation
     - somewhat lazy
     - somewhat lazy
     - lazy
     - eager
   * - Control of evaluation order
     - no
     - yes
     - yes
     - yes
   * - Partial evaluation
     - yes
     - yes
     - no
     - yes
   * - Polymorphism
     - no
     - no
     - yes
     - yes
   * - Modules
     - no
     - no
     - yes
     - yes
   * - Object orientation
     - no
     - no
     - no
     - yes
   * - Concrete data types
     - no
     - yes
     - yes
     - yes

Originally the evaluation in mCRL and mCRL2 was eager, but the addition of
just-in-time rewriting strategies moved evaluation toward laziness. Because
both languages have a non-strict semantics yet evaluation is not fully lazy,
evaluation only *approximates* the semantics in degenerate cases. In practice
this rarely causes problems, and when it does, the evaluation order can usually
be controlled explicitly through conditional rewrite rules.
