.. _atermpp_library:

Introduction to the atermpp library
===================================

The atermpp library is a C++ implementation inspired by the ATerm library
[M.G.T. van den Brand , H.A. de Jong, P. Klint, P.A. Olivier. Efficient annotated terms, Software - Practice & Experience, 30(3):259-291, 2000].
It allows to construct terms, where a term is a function symbol applied to a number of
arguments. Terms can also be natural numbers or lists.

Important features of the
atermpp library, inherited from the ATerm library, are maximal subterm sharing, automatic garbage collection, and easy
ways to write terms to and from disk. The name aterm comes from the possibility in the original ATerm library
to annotate a term with extra information. This has been dropped, along with some other hardly used features
such as reals, blobs and placeholders. The description below restricts itself to the atermpp library as it is found as part of the
mCRL2 toolset.


What is an aterm?
=================

ATerms are hierarchical terms composed of three building blocks: number, lists and
function applications. A function consists of a name, which is an arbitrary string, and
an arity.
Simple examples of aterms are ``[0,1,2]`` and ``f(x,y)``.
Terms can be nested arbitrarily, e.g. ``[0,[f(1),2]]`` and
``f(g(x,[a,b]),h(z))``.

ATermpp types
-------------

In the atermpp library there is a class for function symbols and 4 predefined core types for terms, as shown in the following table.
The class :mcrl2:`aterm <atermpp::aterm>` is a base class for all others.

.. table:: atermpp function symbols and terms

   =======================================================  ==========================================
   type                                                     description
   =======================================================  ==========================================
   :mcrl2:`function_symbol <atermpp::function_symbol>`      a function symbol
   :mcrl2:`aterm <atermpp::aterm>`                          a generic aterm, i.e. an applicative term
   :mcrl2:`term_list\<T\> <atermpp::term_list>`             a list of terms
   :mcrl2:`aterm_int <atermpp::aterm_int>`                  a term containing a 64 bit positive number
   =======================================================  ==========================================


:mcrl2:`term_list\<T\> <atermpp::term_list>` is a template class. A typedef exists for the commonly
used variant:

.. code-block:: c++

  typedef term_list<aterm> aterm_list;

A :mcrl2:`term_list\<T\> <atermpp::term_list>` models a read-only singly
linked list with elements of type ``T``. The element type should be
:mcrl2:`aterm <atermpp::aterm>` or one of its derivatives, or a user defined
aterm (see :ref:`atermpp_programming_user_defined`).

Besides this the library contains a few more data types in which elements derived from aterms can
be stored. Indexed sets (:mcrl2:`indexed_set\<T\> <atermpp::indexed_set>` are essentially unordered sets providing a unique index for each inserted
element. They are more memory efficient than unordered_maps mapping terms to a number. Balanced trees
(:mcrl2:`term_balanced_tree\<T\> <atermpp::term_balanced_tree>`) are used to store trees of terms.
They provide an alternative
way to store lists. As the atermpp library uses maximal sharing, balanced trees provided a memory
efficient way to store such lists, in case there are many subtrees are the same. This is for instance
the case when storing the state vectors in a state space. Contrary to an indexed set, a balanced
tree is itself an aterm, and can therefore be used in other terms and lists.

.. table:: additional data types

   ==================================================================  ===================================================================================
   type                                                                description
   ==================================================================  ===================================================================================
   :mcrl2:`indexed_set\<T\> <atermpp::indexed_set>`                    an unordered set to store aterms providing a unique number (index) for each element
   :mcrl2:`term_balanced_tree\<T\> <atermpp::term_balanced_tree>`      a sort containing balanced trees of terms
   ==================================================================  ===================================================================================

ATerm properties
----------------
The aterms in the atermpp library have some properties that need to be
understood to use the library effectively. Function symbols and aterms are essentially
pointers to data structures stored internally. Copying an aterm is thus a
very cheap operation.

ATerm sharing and garbage collection
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
The most important feature of the atermpp library is that function symbols and terms are shared in
memory. Consider the terms ``f(x,g(y))`` and ``h(f(x,g(y)),g(y))``. The atermpp
library makes sure that the subterm ``f(x,g(y))`` will only appear once in
memory. Even the term ``g(y)`` exists only once. In the case of lists, the atermpp
library shares the tail parts. For example, from the lists ``[0,1,2,3]``
and ``[0,1,2,4,1,2,3]`` the tail part ``[1,2,3]`` will appear only once in
memory. The front parts ``[0,1,2]`` will *not* be shared.

Note that the argument lists of function applications
are not shared. So for the terms ``f(a,b,c)`` and ``g(a,b,c)`` the sequences of
arguments ``a,b,c`` are stored in separate locations, but the individual subterms
``a``, ``b`` and ``c`` are shared.

All terms are automatically garbage collected when not in use anymore. This
is implemented using a simple reference counting mechanism. Every function symbol
and term that is created or copied leads to an increase of the reference count
and this reference count decreased when the object is destroyed. When the
reference count becomes zero functions and terms symbols are ultimately removed from
the underlying data structures.

Read-only terms
^^^^^^^^^^^^^^^

As a result of the maximal sharing, aterms have the property that they are
read-only. All member functions of the ``atermpp`` classes (except the assignment
operator) are constant.

.. note::

   All aterm objects are constant. Whenever you want to
   modify an attribute of an aterm, a new object has to be created.

Needless to say that this has a significant effect on the way aterms are used.

Type conversions
^^^^^^^^^^^^^^^^

Data types that employ the atermpp library typically derive from
:mcrl2:`aterm <atermpp::aterm>`, and sometimes
from :mcrl2:`term_list\<T\> <atermpp::term_list>` and
:mcrl2:`aterm_int <atermpp::aterm_int>`. This means that there is
an automatic conversion from such classes towards :mcrl2:`aterm <atermpp::aterm>`'s.

To convert aterm based types to derived classes explicit
constructors can be used. There is a disadvantage because constructors
will generally copy an aterm which will also lead to an increase
and ultimately a decrease of a reference count.

It is more efficient to use a :mcrl2:`down_cast\<T\> <atermpp::down_cast>`
which allows to cast a term of a type derived from an aterm to a term
of a derived type. The derived type must not have additional fields beyond the
aterm from which it derived. To transform an aterm based term multiple
steps through the inheritance hierarchy :mcrl2:`vertical_cast\<T\> <atermpp::vertical_cast>`
can be used. When the toolset is compiled in debug mode, it is carefully checked
that terms are well defined and of proper type.

.. code-block:: c++

    using atermpp;
    aterm_int x(10);
    aterm y = x;   /                                // No type cast needed; aterm_int inherits from aterm.
    aterm_int& z1 = down_cast<aterm_int>(x)y;       // The down_cast prevents increasing a reference count.
    aterm_int z2(y);                                // The use of an explicit constructor.
    assert(z1.value() == 10 and z2.value == 10);
    aterm f = read_term_from_string("f(x,y)");      // reads an applicative aterm.

String representations
^^^^^^^^^^^^^^^^^^^^^^

ATerms and derived terms can be transformed to strings using the pretty print function
:mcrl2:`pp <atermpp::pp>` function. ATerms can also be used in output streams.

.. code-block:: c++

    aterm_int x;
    std::string s = pp(x);
    std::out << "This is how a default aterm looks like: " << x << "\n";

In most cases this string can be converted back to an aterm using the
:mcrl2:`read_term_from_string <atermpp::read_term_from_string>` function. However, when using some
control characters the resul will not be the same.

Comparing aterms
^^^^^^^^^^^^^^^^
For the aterms all standard comparison operators are defined (``==``, ``!=``, ``<=``, ``<``, ``>`` and ``>=``).
These operations are very efficient which is made possible as aterms are essentially
pointers. If two terms are equal they both consist of the same pointer. As it is nondeterministic where aterms are stored,
the comparison operators can yield different outcomes if terms are destructed and constructed again. The comparison operators
yield consistent outcomes when the reference count of the terms never reaches 0.

Recognizing basic aterms
^^^^^^^^^^^^^^^^^^^^^^^^
Using the aterm member function :mcrl2:`type_is_list <atermpp::unprotected_aterm::type_is_list>`,
:mcrl2:`type_is_int <atermpp::unprotected_aterm::type_is_int>`
to figure out whether an aterm is a list, a function application or an aterm_int.
The aterm member function :mcrl2:`defined <atermpp::unprotected_aterm::defined>` can be used to
find out whether an aterm is equal to the default constructed aterm.


Programming with aterms
=======================

ATerm creation
--------------

All aterm types have their own appropriate constructors or factory functions for creating them.

For ``aterm_int`` and ``term_list``, constructors are available:

.. code-block:: c++

    #include "mcrl2/atermpp/aterm_int.h"
    #include "mcrl2/atermpp/aterm_list.h"
    using namespace atermpp;
    aterm_int i(10);             // an aterm_int can be constructed from a value.
    aterm x, y;
    aterm_list empty_list;       // the default constructor yields the empty list.
    aterm_list l(x, empty_list); // this is the list [x].

For function applications (applicative aterms), the ``make_term_appl`` function is used,
which writes the result into a target variable:

.. code-block:: c++

    #include "mcrl2/atermpp/aterm.h"
    using namespace atermpp;
    aterm x, y, t;
    function_symbol f("f", 2);        // the function symbol f of arity 2.
    make_term_appl(t, f, x, y);       // t now represents f(x,y).

Using iterators, applicative aterms and lists with more arguments can be constructed:

.. code-block:: c++

    #include <vector>
    #include "mcrl2/atermpp/aterm.h"
    #include "mcrl2/atermpp/aterm_list.h"
    using namespace atermpp;
    std::vector<aterm> v;
    aterm x, y, t;
    v.push_back(x);
    v.push_back(y);
    function_symbol g("g", 2);                     // a function symbol with two arguments.
    make_term_appl(t, g, v.begin(), v.end());       // t represents g(x,y). Vector v must have length 2.
    aterm_list l(v.begin(), v.end());               // construct the list [x,y].

Applicative aterms can also be constructed from iterators while applying a conversion to each element.
The converter is a lambda or function object that is applied to each element before it becomes an argument:

.. code-block:: c++

    #include <vector>
    #include "mcrl2/atermpp/aterm.h"
    #include "mcrl2/atermpp/aterm_list.h"
    using namespace atermpp;
    std::vector<Term> v;              // Assume Term is derived from aterm; v = [x1,...,xn].
    aterm t;
    function_symbol g("g", 2);
    make_term_appl(t, g, v.begin(), v.end(),
                   [](const Term& x){ return convertor(x); });
                                      // t represents g(convertor(x1),...,convertor(xn)).
    aterm_list l(v.begin(), v.end(), conversion_class);
                                      // [conversion(x1),...,conversion(xn)]

For lists it is possible to apply a filter on the elements in the input. A filter is a class containing a function
bool operator()(const Term& t). Only if this function provides true on an element it is added to the list.
In the code fragment below the list of aterm_lists l is constructed by taking each aterm_list m in vector v that has a length larger
than 4 with the aterm y put in front of it.

.. code-block:: c++

    #include <vector>
    #include "mcrl2/atermpp/aterm_list.h"
    using namespace atermpp;
    std::vector<aterm_list> v;          // Assume v is a vector containing aterm_lists.
    aterm y;
    term_list<aterm_list> l(v.begin(),
                            v.end(),
                            [&y](const aterm_list& m){ return aterm_list(y, m); },
                            [](const aterm_list& m){ return m.size() > 4; });

ATerm manipulation
------------------

All elements of aterm and derived types can be constructed, assigned and destructed. Standard swap and hash functions are
defined for aterms. Swapping aterms is more efficient than assigning aterms as it prevents adapting reference counts.
All comparision operators are defined as already mentioned above.


For a function_symbol the function :mcrl2:`name() <atermpp::function_symbol::name>` provides the name of the function
as a string and the function :mcrl2:`arity() <atermpp::function_symbol::arity>` gives its arity.

.. code-block:: c++

     #include <atermpp::function_symbol>
     using atermpp;
     function_symbol f("FUNCTION",5);
     std::string s=f.name();              // s becomes the string "FUNCTION".
     size_t n=f.arity();                  // n becomes 5.

The value in an aterm_int can be obtained using the function :mcrl2:`value() <atermpp::aterm_int::value>`.

.. code-block:: c++

     #include <atermpp::aterm_int>
     using atermpp;
     aterm_int n(12);
     size_t x=n.value();    // x gets the value 12.

The function symbol of an aterm is obtained using :mcrl2:`function() <atermpp::aterm::function>`.
The number of arguments of a term is obtained using :mcrl2:`size() <atermpp::aterm::size>`. A convenience function
:mcrl2:`empty() <atermpp::aterm::empty>` can be used to check whether the term application is a constant, i.e.,
has no arguments. An argument can be obtained using the subscript operator :mcrl2:`operator[] <atermpp::aterm::operator[]>`.
The first argument has number 0. Using const iterators it is possible to iterate over the arguments of a term.
For this purpose the functions :mcrl2:`begin() <atermpp::aterm::begin>` and :mcrl2:`end() <atermpp::aterm::end>` are defined.

.. code-block:: c++

     #include "mcrl2/atermpp/aterm.h"
     using namespace atermpp;
     function_symbol f("f", 3);
     aterm x, y, z, t;
     make_term_appl(t, f, x, y, z);
     function_symbol g = t.function();  // g becomes equal to function symbol f.
     aterm u = t[1];                    // u becomes equal to y.
     std::size_t n = t.size();          // n becomes 3.
     bool b = t.empty();                // t has three arguments, therefore b is false.
     for (aterm::const_iterator i = t.begin(); i != t.end(); ++i)
     {
       ...                              // iterate over the arguments of t
     }

There are a number of functions to manipulate with term_lists.
The function :mcrl2:`front() <atermpp::term_list::front>` provides the first element of a list and
the function :mcrl2:`tail() <atermpp::term_list::tail>` give the tail of the list.
The function :mcrl2:`pop_front() <atermpp::term_list::pop_front>` removes the first element from
a list and :mcrl2:`push_front() <atermpp::term_list::push_front>` is used to add an element to the front
of the list. The length of a list is obtained by :mcrl2:`size() <atermpp::term_list::size>` and the
convenience function :mcrl2:`empty() <atermpp::term_list::empty>` can be used to check whether
a list is empty. It is possible to iterate over the elements of a list using
:mcrl2:`end() <atermpp::term_list::end>`.

.. code-block:: c++

     #include <atermpp::aterm_list.h>
     #include <atermpp::aterm_io.h>
     using atermpp;
     aterm_list l=read_term_from_string("[1,2,3,17,5]");
     aterm_list m=l.tail();             // m is [2,3,17,5].
     aterm_int n=l.front();             // n is the aterm_int with value 1.
     l.pop_front();                     // now l is [2,3,17,5].
     l.push_front(aterm_int(29));       // now l is [29,2,3,17,5].
     size_t n=l.size();                 // n becomes 5.
     bool b=l.empty();                  // b becomes false.
     for(aterm_list::const_iterator i=l.begin(); i!=l.end(); ++i)
     {
       // iterate over the five elements of l.
     }


ATerms and the C++ Standard Library
-----------------------------------

Standard containers
^^^^^^^^^^^^^^^^^^^^
ATerms work seamlessly with the standard containers of C++.
For example:

.. code-block:: c++

   #include <vector>
   #include <atermpp::aterm_io.h>

   std::vector<atermpp::aterm> v;
   v.push_back(atermpp::read_term_from_string("f(x)");
   v.push_back(atermpp::read_term_from_string("g(y)");


Iterator interfaces
^^^^^^^^^^^^^^^^^^^
The classes :mcrl2:`term_list\<T\> <atermpp::term_list>` and
:mcrl2:`aterm <atermpp::aterm>` have C++ standard conforming iterator interfaces.
The iterator of a :mcrl2:`term_list` iterates over the elements in the list. The iterator
of an :mcrl2:`aterm <atermpp::aterm>` iterates over the arguments of the term.
They operate well with the C++ Standard Library, as illustrated by the following example:

.. code-block:: c++

    #include <algorithm>
    #include <iostream>
    #include "atermpp/atermpp.h"

    using namespace std;
    using namespace atermpp;

    struct counter
    {
      int& m_sum;

      counter(int& sum)
        : m_sum(sum)
      {}

      void operator()(const aterm_int& t)
      {
        m_sum += t.value();
      }
    };

    int main(int argc, char* argv[])
    {
      term_list<aterm_int> q = read_term_from_string("[1,2,3,4]");
      int sum = 0;
      for_each(q.begin(), q.end(), counter(sum));
      assert(sum == 10);

      for (term_list<aterm_int>::iterator i = q.begin(); i != q.end(); ++i)
      {
        cout << i->value() << " ";
      }
    }

.. _atermpp_programming_user_defined:

User defined terms
^^^^^^^^^^^^^^^^^^

The aterm library provides an excellent basis on top of which user defined terms
can be constructed. Suppose one wants to create terms with zero, one and addition
where only addition is defined explicitly below.
This can be done by creating a class :mcrl2:`Expression` inheriting from :mcrl2:`aterm <atermpp::aterm>`.

.. code-block:: c++

   using namespace atermpp;
   class Expression: public aterm
   {
   public:
     explicit Expression(const function_symbol& f)
     {
       make_term_appl(*this, f);
     }

     Expression(const function_symbol& f, const Expression& e1, const Expression& e2)
     {
       make_term_appl(*this, f, e1, e2);
     }

     // Check whether the expression is zero.
     bool is_zero() const
     {
       return function() == function_symbol("zero", 0);
     }

     // Check whether the expression is one.
     bool is_one() const
     {
       return function() == function_symbol("one", 0);
     }

     // Check whether the expression is an addition.
     bool is_addition() const
     {
       return function() == function_symbol("add", 2);
     }
   };

   class Zero: public Expression
   {
   public:
     Zero()
       : Expression(function_symbol("zero", 0))
     {}
   };

   class One: public Expression
   {
   public:
     One()
       : Expression(function_symbol("one", 0))
     {}
   };

   class Addition: public Expression
   {
   public:
     Addition(const Expression& e1, const Expression& e2)
       : Expression(function_symbol("add", 2), e1, e2)
     {}

     // Get left argument.
     const Expression& left() const
     {
       return down_cast<Expression>((*this)[0]);
     }

     // Get right argument.
     const Expression& right() const
     {
       return down_cast<Expression>((*this)[1]);
     }
   };

Now that we have defined :mcrl2:`Expression`, we can use it in standard containers.

.. code-block:: c++

   #include <vector>

   std::vector<Expression> v;
   Zero x;
   One y;
   v.push_back(Addition(x,y));
   v.push_back(x);
   v.push_back(y);

.. _atermpp_programming_algorithms:

ATerm algorithms
----------------

For the ``atermpp`` library a couple of algorithms are defined. Most
of these algorithms have template parameters for the terms that they
operate on. These algorithms work on every class for which an :mcrl2:`aterm_traits`
specialization exists.

Find algorithms
^^^^^^^^^^^^^^^
There are a couple of find algorithms, including :mcrl2:`find_if <atermpp::find_if>`
for searching a subterm that matches a given predicate, and
:mcrl2:`find_all_if <atermpp::find_all_if>` for finding all subterms that match a
predicate. The program fragment below illustrates this:

.. code-block:: c++

  #include "mcrl2/atermpp/algorithm.h"

  // function object to test if it is an aterm with function symbol "f"
  struct is_f
  {
    bool operator()(aterm t) const
    {
      return t.type_is_appl() && t.function().name() == "f";
    }
  };

  aterm a = read_term_from_string("h(g(x),f(y),p(a(x,y),q(f(z))))");
  aterm t = find_if(a, is_f());
  assert(t == read_term_from_string("f(y)"));

  find_all_if(a, is_f(), std::back_inserter(v));
  assert(v.size() == 2);
  assert(v.front() == read_term_from_string("f(y)"));
  assert(v.back() == read_term_from_string("f(z)"));

The find algorithms also work on user defined types. So if ``t`` is of type :mcrl2:`Expression`,
then it is possible to call :mcrl2:`find_if(t, is_f())`
as well.

Replace algorithms
^^^^^^^^^^^^^^^^^^
There are several algorithms for replacing subterms. The ``replace`` algorithm replaces
a subterm with another term, ``bottom_up_replace`` does the same but with a different traversal
order. The latter function also contains a version that maintains a cache of replaced terms,
which may improve the performance if the same subterms occur often.
The algorithm :mcrl2:`~atermpp::partial_replace` has the option to abort further replacements
based on a predicate.

.. code-block:: c++

  #include "atermpp/algorithm.h"

  // function object to test if it is an aterm with function symbol "a" or "b"
  struct is_a_or_b
  {
    bool operator()(aterm t) const
    {
      return t.type_is_appl() &&
        (t.function().name() == "a" || t.function().name() == "b");
    }
  };

  aterm a = read_term_from_string("f(f(x))");
  aterm b = replace(a, read_term_from_string("f(x)"), read_term_from_string("x"));
  assert(b == read_term_from_string("f(x)"));
  aterm c = replace(a, read_term_from_string("f(x)"), read_term_from_string("x"), true);
  assert(c == read_term_from_string("x"));

Miscellaneous algorithms
^^^^^^^^^^^^^^^^^^^^^^^^

The algorithm :mcrl2:`~atermpp::apply` applies an operation to the elements
of a list, and returns the result. The :mcrl2:`~atermpp::for_each` algorithm applies
an operation to each subterm of a term.

.. code-block:: c++

   #include "atermpp/algorithm.h"

   // Applies a function f to the given argument t.
   struct apply_f
   {
     aterm operator()(const aterm& t) const
     {
       aterm result;
       make_term_appl(result, function_symbol("f", 1), t);
       return result;
     }
   };

   bool print(const aterm& t)  // The return value true indicates that for_each
                               // should recurse into the children of t.
   {
     std::cout << t.function().name() << " ";
     return true;
   }

   aterm t = read_term_from_string("h(g(x),f(y))");
   atermpp::for_each(t, print);             // prints "h g x f y"

   aterm_list l = read_term_from_string("[0,1,2,3]");
   l = atermpp::apply(l, apply_f());        // results in [f(0),f(1),f(2),f(3)]

Visitors
^^^^^^^^
The classes ``atermpp::traverser`` and ``atermpp::builder`` are visitors that can be used
as building blocks of other algorithms. They have the following interface:

.. code-block:: c++

  template <typename Derived>
  struct traverser
  {
    void apply(const aterm_int& x);
    void apply(const aterm_list& x);
    void apply(const aterm& x);
  };

  template <typename Derived>
  struct builder
  {
    aterm apply(const aterm_int& x);
    aterm apply(const aterm_list& x);
    aterm apply(const aterm& x);
  };

The function ``traverser::apply`` by default visits all subterms of a term, and the
function ``builder::apply`` rebuilds a term by reassembling it from the bottom up. By overriding the
``apply`` member functions, the default behaviour can be changed. For example, the following is
enough to modify all subterms ``x`` by ``y``.

.. code-block:: c++

  struct xy_replacer: public builder<xy_replacer>
  {
    typedef builder<xy_replacer> super;
    using super::apply;

    aterm apply(const aterm& x)
    {
      if (x == atermpp::read_term_from_string("x"))
      {
        return atermpp::read_term_from_string("y");
      }
      return super::apply(x);
    }
  };

  atermpp::aterm t = atermpp::read_term_from_string("h(g(f(x),x))");
  xy_replacer f;
  atermpp::aterm t1 = f.apply(t);
  std::cout << "t1 == " << t1 << std::endl;
  assert(t1 == atermpp::read_term_from_string("h(g(f(y),y))"));

Note that static polymorphism using the `Curiously recurring template pattern <http://en.wikipedia.org/wiki/Curiously_recurring_template_pattern>`_
is applied in the visitor classes. The call to ``super::apply`` triggers the default behaviour of ``apply(aterm)``, which is to recurse into the subterms.