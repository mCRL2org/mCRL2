Introduction
============
The mCRL2 language describes processes with data. The Data Library contains
everything that has to do with the data part of the language. The main concepts
are sorts and functions working upon these sorts. The meaning of these
functions can be described by means of equational axioms.
In the __mcrl2_language_reference__ these concepts are explained in more detail.

Data specifications
===================

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

Expressions
===========
In this section we first introduce the basic structures of sort expressions
and data expressions. We then continue to defining the sort expressions
with operations that are predefined in the Data Library.
The code in the Data Library is inside the namespace ``mcrl2::data``.

Sort expressions
----------------

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

These sort expressions correspond to the grammar

     S ::= Sb | Sc | S x ... x S -> S | Sstruct,

where ``Sb`` is a given set of basic sorts, always including the booleans
(sort ``Bool``). S x ... x S -> S denotes the function sorts, where -> is right
associative. ``Sc`` is the set of container sorts, and Sstruct is the set of
structured sorts.

The set of container sorts ``Sc`` is defined as follows.

     Sc ::= List(S) | Set(S) | FSet(S) | Bag(S) | FBag(S)

Where ``FSet(S)`` and ``FBag(S)`` represent finite sets and finite bags
respectively.

The syntax of structured sorts Sstruct is defined as follows
(where p is a string):

     Sstruct ::= p(proj*)?p

in which proj has the following syntax:

     proj ::= S | p:S

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
----------------
The class ``data_expression`` represents expressions like ``true``,
[^x > 3] and [^forall n:Nat. f(n) < 5]. Each data expression ``d`` has a type or
sort ``d.sort()`` of type ``sort_expression``.
Let's look at a simple example
that constructs the numbers two and three, and builds the expression 2 + 3:

.. code-block:: c++

   #include "mcrl2/data/data.h"
   #include "mcrl2/atermpp/aterm_init.h"
   #include <cassert>

   using namespace mcrl2::data;

   int main(int argc, char* argv[])
   {
     MCRL2_ATERMPP_INIT(argc, argv)

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

  e ::= x | n | e(e, ..., e) | lambda x:S, ..., x:S . e |
        forall x:S, ..., x:S. e | exists x:S, ..., x:S. e |
        e whr x = e, ..., x = e end

 Here ``e(e,...,e)`` denotes application of data expressions, ``lambda x:S, ..., x:S . e``
 denotes lambda abstraction, ``forall x:S, ..., x:S . e`` and ``exists x:S, ..., x:S . e``
 denote universal and existential quantification.

Predefined sorts
----------------

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
------------------------------

Default operations
^^^^^^^^^^^^^^^^^^
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

Booleans
""""""""
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

Positive numbers
""""""""""""""""
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

Natural numbers
"""""""""""""""
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

Integers
""""""""
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

Real numbers
""""""""""""
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

Lists
"""""
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
   element_at(S,l,n)  l.n        The element at position ``n`` in ``l``
   head(S,l)          head(l)    The first element of ``l``
   tail(S,l)          tail(l)    ``l`` from which the first element has been removed
   rhead(S,l)         rhead(l)   The last element of ``l``
   rtail(S,l)         rtail(l)   ``l`` from which the last element has been removed
   =================  =========  ==================================================

Finite sets
"""""""""""
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


Finite bags
"""""""""""
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

.. table:: Functions for sort FSet(S)

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

Sets
""""
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

Bags
""""
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
-------------------------

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

   int main(int argc, char* argv[])
   {
     // two ways to create the expression m + n
     std::string var_decl = "m, n: Pos;\n";
     data_expression d1 = parse_data_expression("m+n", var_decl);

     variable m = parse_data_expression("m", var_decl);
     variable n = parse_data_expression("n", var_decl);
     data_expression d2 = sort_pos::plus(m, n);

     return 0;
   }


Sort aliases and sort normalisation
===================================

In mCRL2 it is possible to define sort aliases, which have the form :math:`A = B`. This means
that sort :math:`A` and sort :math:`B` are considered the same, and are completely exchangeable.

Typical examples of sort aliases are

.. code-block:: mcrl2

   sort Time    = Nat;
        L       = List(List(Bool));
        F       = Nat->List(Nat);
        Complex = Bag(A -> Set(B));

It is also possible to define structured sorts that can be recursive (contrary to function sorts,
lists, sets, and bags above, which cannot be recursive).

A structured sort has the shape:

.. math::

   \begin{array}{lll}
   \mathbf{sort} & A = \mathbf{struct}
     & c_1(\mathit{pr}_{1,1} {:} A_{1,1},\ \ldots,\ \mathit{pr}_{1,k_1} {:} A_{1,k_1})
       \;?\mathit{isC_1} \\
   & \mid
     & c_2(\mathit{pr}_{2,1} {:} A_{2,1},\ \ldots,\ \mathit{pr}_{2,k_2} {:} A_{2,k_2})
       \;?\mathit{isC_2} \\
   & & \vdots \\
   & \mid
     & c_n(\mathit{pr}_{n,1} {:} A_{n,1},\ \ldots,\ \mathit{pr}_{n,k_n} {:} A_{n,k_n})
       \;?\mathit{isC_n}
   \end{array}

This declares sort :math:`A` to consist of :math:`n` constructors :math:`c_i`, projection
functions :math:`\mathit{pr}_{i,j}` and recognisers :math:`\mathit{isC_i}`. All the
:math:`A_{i,j}` are sorts. The :math:`A_{i,j}` can be equal to :math:`A`, in which case
:math:`A` is a recursive structured sort.

A very well known example is that of a tree data structure in which natural numbers can be stored.

.. code-block:: mcrl2

   sort Tree = struct node(left:Tree, right:Tree) | leave(Nat)?is_leave;

Note that at the left of a sort alias there is a ``basic_sort``, and at the
right there is a ``sort_expression``.

An alias is declared as follows:

.. code-block:: c++

   #include "mcrl2/data/alias.h"
   #include "mcrl2/data/container_sort.h"

   using namespace mcrl2::data;

   void alias_demo()
   {
     basic_sort b("sort_id");
     container_sort c(bag,sort_nat::sort_nat());

     alias a(b,c);

     std::cout << "Alias name: " << a.name() << " Alias rhs: " << pp(a.reference()) << "\n";
   }

An important consequence of the use of aliases is that different sort
expressions can denote the same sort. For instance, ``Time`` and ``Nat``
denote the same sort, so the variables ``x:Time`` and ``x:Nat`` are the
same object. More generally, by combining aliases and structured sorts it
is possible to have very different looking sort expressions that denote the
same sort: two such expressions are equal if, by folding and unfolding the
definitions in sort aliases and structured sorts, they can be rewritten to
each other. It is time consuming to continuously calculate whether sorts are
the same, which is undesirable if it comes to verification. Therefore, we
require that all sorts in expressions that are equal modulo sort aliases
are represented by a unique sort expression. This process is called sort
normalisation. Note that sort normalisation is dependent on a particular
specification; the sorts in one expression can be normalised differently
for two different data specifications. In particular, adding one sort alias
or one structured sort can change the outcome of the normalisation procedure.

More concretely, for a sort alias

.. code-block:: mcrl2

   sort A=B;

sort A and B are equal. Sort normalisation will rewrite each sort B to A, except
if B is a Bool, Pos, Nat, Int or Real.
In case there are more aliases referring to the same sort, as in the
example below there are more options for the unique sort.

.. code-block:: mcrl2

   sort A1=List(B);
        A2=List(B);

In this case either A1, or A2 is chosen as the representation for List(B).

Sort normalisation takes place automatically inside a data specification.
Functions, sorts, equations, etc. that are added using for instance
add_equation are automatically normalised. Aliases that
are added are also automatically applied to all elements in the data type.
If the elements of a data type are requested, e.g. the sorts, constructors,
mappings and equations of a data type, then these are provided with
normalised sorts. The functions user_defined_aliases, user_defined_sorts, etc.
are provided to extract the aliases, sorts, mappings, constructors and equations
in exactly the form they were added to the specification using the add\_.... functions.

However, objects outside the data specification are not automatically normalised.
These must be normalized explicitly. Normalisation functions, normalise_sort, exist for all types
that derive from terms, such as data_expressions, assignments, sort_expressions, data_equations, etc.,
as well as for lists of these types.

.. code-block:: c++

   #include mcrl2/data/data_specification.h
   #include "mcrl2/data/alias.h"
   #include "mcrl2/data/function_symbol.h"

   using namespace mcrl2::data;

   void normalise_sort_demo()
   {
     data_specification spec;
     const basic_sort a("A");
     const basic_sort b("B");
     spec.add_alias(alias(a,b));

     const function_symbol f("f",b));

     // An example of normalising a function symbol explicitly.
     std::cout << "Not normalised: " << pp(f) << " has sort " << pp(f.sort()) << "\n";
     const function_symbol normalised_f=spec.normalise(f);
     std::cout << "Normalised: " << pp(normalised_f) << " has sort " << pp(normalised_f.sort()) << "\n";

     spec.add_mapping(function_symbol);

     // Get the mapping and the sorts, which are normalised. So, f:A is replaced by f:B.
     mappings_const_range m=mappings();
     for(function_symbol::const_iterator i=m.begin(); i!=m.end(); ++i)
     { std::cout << "Function symbol " << pp(*i) << " has sort " << pp(i->sort()) << "\n";
     }
   }

Given a particular sort, it is sometimes useful to find the sort it represents. E.g.
suppose that sort F is defined by F=A->B. Then from sort F alone it cannot be seen
that F is actually a function sort. The function unalias yields the structure of an
alias. So, in this particular case data_spec.unalias(F) yields A->B. As aliases may be
recursive, as in sort E=struct nil | insert(Nat,E), unalias will only unfold an alias
until it cannot be unfolded further, or until a type with a type constructing operator
occurs.

Normalisation algorithm
-----------------------

Below we give an algorithm to perform normalisation which is used in the mCRL2 tool suite. The
essential idea is that all the definitions of structured sorts are interpreted from right to left,
whereas all other rules are interpreted as rewrite rules from left to right. So, in the example
above, :math:`\mathit{Time}` is rewritten to :math:`\mathbb{N}`, :math:`F` is rewritten to
:math:`C \rightarrow D`, etc. Because ordinary sort aliases rules cannot be recursive, and
structured sorts shrink with every rewrite step, this rewrite system is terminating.

But as the rewrite system is not confluent, unique normal forms are not guaranteed. The following
example shows the problem.

.. code-block:: mcrl2

   sort
     A = struct f(Nat);
     C = struct f(Nat);

A sort of the shape :math:`\mathbf{struct}\ f(\mathbb{N})` can be normalised to sort :math:`A`
and sort :math:`B`. In order to deal with this problem, we apply Knuth-Bendix completion, to
guarantee that all normal forms are unique.

The algorithm is performed in three steps. First, the set of aliases is checked for recursive
definitions in all sorts except the structured sorts. If such a loop in the sort aliases is
detected, an exception is thrown. The algorithm consists of a simple depth first search.

Second, we have two sets of rewrite rules. We have two auxiliary multimaps that map types to their
respective right hand side. The multimap
:math:`\mathit{resulting\_normalized\_sort\_aliases}` contains all definitive type rewrite rules,
except that in case of multiple entries, with the same lhs only one will end up in the definitive
set of type rewrite rules. The multimap
:math:`\mathit{sort\_aliases\_to\_be\_investigated}` contains those type rewrite rules that must
be investigated for critical pairs to determine whether they lead to extra type rewrite rules.

All aliases :math:`B = type\_expression` are directly added to
:math:`\mathit{resulting\_normalized\_sort\_aliases}` as a rewrite rule
:math:`B \rightarrow type\_expression` if :math:`type\_expression` is not a structured type.
Otherwise they are added as a rewrite rule of the shape
:math:`type\_expression \rightarrow B` to :math:`\mathit{sort\_aliases\_to\_be\_investigated}`.

As a third step the sort aliases are taken as rewrite rules, and a form of Knuth-Bendix completion
is applied to them, to transform them into a confluent term rewriting system, guaranteeing unique
representations. Only the rules in :math:`\mathit{sort\_aliases\_to\_be\_investigated}` need to be
investigated as those in :math:`\mathit{resulting\_normalized\_sort\_aliases}` cannot give rise to
critical pairs.

So, if there are two overlapping left hand sides in the rewrite system, this means that one term is
a subterm of the other. So, we have a rule :math:`C(g(t)) \rightarrow u_1` and a rule
:math:`g(t) \rightarrow u_2` where :math:`C` represents a possibly empty context. So, the term
:math:`C(g(t))` can rewrite to both :math:`u_1` and :math:`C(u_2)`. In this case we add a rewrite
rule :math:`t \rightarrow u_1` where :math:`t` is the normal form of :math:`C(u_2)` for the
rewrite rules in :math:`\mathit{resulting\_normalized\_sort\_aliases}`.

An important observation is that the rules always have one of the following shapes:

.. math::

   \begin{array}{rcl}
   \mathbf{struct}\ldots & \rightarrow & A, \\
   B                     & \rightarrow & \mathit{Exp}
   \end{array}

where :math:`A` and :math:`B` are basic sorts and :math:`\mathit{Exp}` is a sort expression
which can be a basic sort, but can also contain all other type forming constructs. There are the
following invariants on the rules. For each basic sort :math:`B` there is at most one rule of the
form :math:`B \rightarrow \cdots`. Furthermore, a basic sort :math:`A` occurring at the right of a
struct rule can never occur as the left hand side of a rewrite rule also.

So, when one left hand side of a rule overlaps with another left hand side, one of the rules must
have the shape :math:`\mathbf{struct}\ldots \rightarrow \ldots`, whereas the other can contain a
struct or a basic term at the left hand side. As the rule with a struct rewrites to a basic sort
:math:`A`, the newly added rewrite rule has :math:`A` at its right hand side.

The number of newly added rules in this way is bounded. When both left hand sides contain structs,
the newly added rule has a strictly smaller number of structs in its right hand side than one of its
originals. Moreover, no new basic sort is introduced that can act as the lhs of a new rule. When a
rule of the shape :math:`A \rightarrow \mathit{Exp}` contains overlap, a rule is obtained where an
occurrence of :math:`A` is replaced by an occurrence of :math:`\mathit{Exp}`. But as these rules
are acyclic, this can only be performed a finite number of times.

In more detail, we have two sets of rewrite rules. One that is definitive
:math:`\mathit{m\_normalised\_sort\_aliases}` and
:math:`\mathit{sort\_aliases\_to\_be\_investigated}` that contains sort rewrite rules still to be
investigated. Initially, all rules are in :math:`\mathit{sort\_aliases\_to\_be\_investigated}`.
Each rewrite rule :math:`t_1 \rightarrow u_1` in
:math:`\mathit{sort\_aliases\_to\_be\_investigated}` is checked with each rule
:math:`t_2 \rightarrow u_2` in :math:`\mathit{m\_normalised\_sort\_aliases}`. If :math:`t_1` is a
subterm of :math:`t_2` (i.e. :math:`t_2 = C(t_1)`) and :math:`u_2` and :math:`C(u_1)` do not
have the same normal forms, then a rule :math:`C(u_1) \rightarrow u_2` is added to
:math:`\mathit{sort\_aliases\_to\_be\_investigated}`. If :math:`t_2` is a subterm of :math:`t_1` a
symmetric sequence of steps is done. After all rewrite rules :math:`t_1 \rightarrow u_1` in
:math:`\mathit{m\_normalised\_sort\_aliases}` have been investigated,
:math:`t_2 \rightarrow u_2` is added to :math:`\mathit{m\_normalised\_sort\_aliases}`.

The resulting rewrite system is terminating, provided that the original rewrite system was
terminating. Each new rule that is added has the shape :math:`C = a`, where :math:`C` is a basic
or complex type, and :math:`a` is a basic sort, which is a normal form in the rewrite system. The
only way that there is non termination, is when there is an infinite sequence of basic sorts
:math:`a_1`, :math:`a_2, \ldots`, such that :math:`a_i` rewrites to :math:`a_{i+1}`. This loop
came into existence by adding some rewrite rule :math:`a = a'` at some moment in time, where
:math:`a'` was not a normal form. But this cannot happen, because by construction :math:`a'` is a
normal form.

After constructing the normal forms, the content of
:math:`\mathit{m\_normalised\_sort\_aliases}` is copied into
:math:`\mathit{m\_normalised\_aliases}`, where every right hand side is normalised, to speed up
rewriting when applied to concrete sorts.

Normalisation of concrete sorts is now very simple. Every sort which equals a left hand side of a
sort alias is replaced by the right hand side. This is repeated until no such substitution can be
applied. This can be done using a simple innermost rewriting procedure. This rewriter has been
implemented in ``normalize_sorts_function``.

.. rubric:: Acknowledgements

Thanks go to Aleksi Peltonen for identifying that in the algorithm up to spring 2018 the newly
added type rewrite rules were not normalised, leading to the addition of an exponential type
rewrite rules, slowing type rewriting down. His example was

.. math::

   \begin{array}{ll}
   \mathbf{sort} & A_t = \mathit{Nat};\; B_t = \mathit{Nat};\; C_t = \mathit{Nat};\;
                   D_t = \mathit{Nat};\; E_t = \mathit{Nat};\; F_t = \mathit{Nat};\;
                   G_t = \mathit{Nat}; \\
                 & S_t = \mathbf{struct}\ s(A{:}A_t,\, B{:}B_t,\, C{:}C_t,\,
                   D{:}D_t,\, E{:}E_t,\, F{:}F_t,\, G{:}G_t); \\
   \mathbf{init} & \delta;
   \end{array}

This would lead to :math:`2^n` type rewrite rules for all :math:`n` arguments of the function
:math:`s`.

Data rewriters
==============

A rewriter is a function that rewrites terms using a number of rewrite rules.
In the mCRL2 toolset a class ``data::rewriter`` is available that operates on data expressions,
and that is initialized using a data specification. The equations of the data specification
are interpreted as rewrite rules from left to right. An example is given below.

.. code-block:: c++

   // rewrite two data expressions, and check if they are the same
   rewriter r;
   data_expression d1 = parse_data_expression("2+7");
   data_expression d2 = parse_data_expression("4+5");
   assert(d1 != d2);
   assert(r(d1) == r(d2));

.. tip::

   Rewriters can be used to determine equivalence between data expressions. In general this
   problem is undecidable. Only if [^r(d1) == r(d2)] one can conclude that the expressions ``d1``
   and ``d2`` are equivalent, otherwise the answer is unknown.

For efficiency reasons a rewriter can be invoked with an optional substitution function
__sigma as a second argument, where __sigma maps data variables to data expressions. The
function __sigma must satisfy the property that for all data variables ``v``

   __sigma(v) == r(__sigma(v)).

Under this condition the following property holds:

   r(d,__sigma) == r(__sigma(d)).

Note that in general the computation of [^r(d,__sigma)] can be done more efficiently than the
computation of [^r(__sigma(d))]. In the mCRL2 toolset substition functions are used that take constant time.

An example of rewriting with a substitution function is given below.

.. code-block:: c++

   rewriter r;

   // Create a substitution sequence sigma with two substitutions: [m:=3, n:=4]
   std::string var_decl = "m, n: Pos;\n";
   mutable_map_substitution sigma;
   sigma[parse_data_expression("m", var_decl)] = r(parse_data_expression("3"));
   sigma[parse_data_expression("n", var_decl)] = r(parse_data_expression("4"));

   data::data_expression d1 = parse_data_expression("m+n", var_decl);
   data::data_expression d2 = parse_data_expression("7");
   assert(r(d1, sigma) == r(d2));

.. caution::

   The current implementation of rewriting with substitutions to data variables is
   inefficient. The interface of the underlying ``Rewriter`` class needs to be adapted
   to get rid of these inefficiencies.

Rewriter Concept
----------------

In the mCRL2 toolset a ``Rewriter`` is a concept with the following requirements:

.. table:: Associated types

   =======================  ====================================================
   Expression               Meaning
   =======================  ====================================================
   Rewriter::term_type      the type of the terms on which the rewriter operates
   Rewriter::variable_type  the type of the variables
   =======================  ====================================================

A ``SubstitutionFunction`` is a function that maps variables to terms. Let ``sigma`` be a
substitution function, and let ``v`` be an expression of type ``Rewriter::variable_type``.

.. table:: Expression semantics for SubstitutionFunction

   ==========  ===================================================
   Expression  Result
   ==========  ===================================================
   sigma(v)    Returns an expression of type ``Rewriter::term_type``
   ==========  ===================================================

Let ``r`` be a ``Rewriter``, let ``d`` be an expression of type ``Rewriter::term_type``
and let ``sigma`` be a ``SubstitutionFunction``.

.. table:: Expression semantics for Rewriter

   +-------------+-----------------------------------------------------------------------------------------------------------+
   | Expression  | Result                                                                                                    |
   +=============+===========================================================================================================+
   | r(d)        | Returns an expression of type Rewriter::term_type that is the result of applying the rewriter r to term d |
   +-------------+-----------------------------------------------------------------------------------------------------------+
   | r(d, sigma) | Returns an expression of type Rewriter::term_type that is the result of applying the rewriter r to term , |
   |             | while on the fly applying the substitution function sigma to all data variables in d.                     |
   +-------------+-----------------------------------------------------------------------------------------------------------+

Algorithms using a rewriter
---------------------------

Algorithms that use a rewriter are typically implemented with a template parameter
for the rewriter. An example of this is the constelm algorithm of the LPS library:

.. code-block:: c++

    template <typename Rewriter>
    specification constelm(const specification& spec, Rewriter r, bool verbose = false)
    {
      ...
    }

The algorithm may only assume that the requirements of the Rewriter Concept hold,
with proper choices for the nested variable and term types.

Data enumerator
===============

Documentation not yet available
