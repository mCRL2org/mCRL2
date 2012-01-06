Programming with ATerms
-----------------------

ATerm Library initialization
^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The :cpp:func:`main` function of each program that uses the ATerm Library
must start with the following initialization code:

.. code-block:: c++

   #include "mcrl2/atermpp/atermpp.h"
   #include "mcrl2/atermpp/aterm_init.h"

   int main(int argc, char* argv[])
   {
     MCRL2_ATERMPP_INIT(argc, argv)
   }

The macro `MCRL2_ATERMPP_INIT` must be the first statement of the
main function. The garbage collector uses `argc` and `argv` to guess
the bottom of the program stack, that is used to determine which
terms are candidates to be destroyed.

ATerm creation
^^^^^^^^^^^^^^

All aterm types have their own appropriate constructors for creating them:

.. code-block:: c++

    aterm_int i(10);
    aterm_real r(2.5);
    aterm_appl f(function_symbol("f", 2), aterm("x"), aterm("y")); // represents f(x,y)

There is also a convenience function `make_term` for easily creating aterms from
strings: `make_term(const std::string& format, ...)`. The `format` argument is
a string that may contain several patterns as given in the table below. For each
occurrence of a pattern, one or more additional arguments need to be supplied to
the function `make_term`.

.. table:: ATerm++ patterns

  ===========   ============= =============================
  type          pattern       argument                    
  ===========   ============= =============================
  Application   <appl>        ``string pattern, arguments`` 
  Blob          <blob>        ``int length, void* data`` 
  Integer       <int>         ``int value`` 
  List          <list>        ``aterm`` 
  Placeholder   <placeholder> ``string type`` 
  Real          <real>        ``double value`` 
  String        <str>         ``string pattern, arguments`` 
  Term          <term>        ``aterm`` 
  ===========   ============= =============================

The following program illustrates the usage of `make_term`.

.. code-block:: c++

  #include <iostream>
  #include "atermpp/atermpp.h"
  #include "mcrl2/atermpp/aterm_init.h"

  using namespace atermpp;

  void foo()
  {
    const int i       = 42;
    const char* s     = "example";
    const char* blob  = "12345678";
    const double r    = 3.14;
    const char *func  = "f";

    aterm_int  t0 = make_term("<int>" , i);         // integer value: 42
    aterm      t1 = make_term("<str>" , func);      // quoted application: "f", no args
    aterm_real t2 = make_term("<real>", r);         // real value: 3.14
    aterm_blob t3 = make_term("<blob>", 8, blob);   // blob of size 8, data: 12345678

    aterm_list l0 = make_term("[]");
    aterm_list l1 = make_term("[1,<int>,<real>]", i, r);
    aterm_list l2 = make_term("[<int>,<list>]", i+1, l1);

    aterm_appl a0 = make_term("<appl>", func);
    aterm_appl a1 = make_term("<appl(<int>)>", func, i);
    aterm_appl a2 = make_term("<appl(<int>, <term>, <list>)>", func, 42, t3, l2);
  }

  int main(int argc, char* argv[])
  {
    MCRL2_ATERMPP_INIT(argc, argv)

    foo();
    return 0;
  }

The function `match` can be used to extract pieces of aterms, as illustrated by the
following program fragment:

.. code-block:: c++

  aterm t = make_term("and(a,not(b))");
  aterm t1;
  aterm t2;
  if (match(t, "and(<term>,<term>)", t1, t2))
  {
    assert(t1 == make_term("a"));
    assert(t2 == make_term("not(b)"));
  }

The garbage collector
^^^^^^^^^^^^^^^^^^^^^
The ATerm Library uses a very agressive garbage collection scheme.
When the garbage collector is triggered, only the following terms will be
retained:

  * Terms that are located on the program stack
  * Terms that have been explicitly protected against garbage collection

.. warning::
   
   All other terms will be destroyed during garbage collection!

The garbage collector of the ATerm Library assumes that all aterms that are
not on the program stack can be safely destroyed
[footnote There is no standard way to determine if an object is located
on the stack, which makes the garbage collection potentially unreliable.].
The terms in the `atermpp`
library all have a `protect` member function that can be called to prevent
this behavior.

The following program illustrates this.

.. code-block:: c++

    #include <string>
    #include "atermpp/atermpp.h"
    #include "mcrl2/atermpp/aterm_init.h"
    using namespace atermpp;

    aterm a; // using global aterm variables is in general not a good idea
    aterm b;

    aterm f(const std::string& s)
    {
      aterm x = make_term("f(" + s + ")"); // no need to protect x, since it will be created on stack
      return x;
    }

    int main(int argc, char* argv[])
    {
      MCRL2_ATERMPP_INIT(argc, argv)

      a.protect();          // Unsafe! Protecting a term should be done after assignment.
      a = aterm_int(4);

      b = aterm_real(1.1);
      b.protect();          // OK, now it is safe to use b

      b = aterm_real(2.5);  // Oops, b has not been unprotected, so the term aterm_real(1.1)
                            // will never be freed.

      aterm_appl* c = new aterm_appl(make_term("f(x)"));
      c->protect();         // Term c is on the heap, so it must be protected.
      // use c
      c->unprotect();
      delete c;             // After calling unprotect the term can be safely deleted.
    }

ATerms and the C++ Standard Library
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Protected containers
""""""""""""""""""""
Most of the container classes of the C++ Standard Library put their data on the heap.
As a result, putting ATerms in a container without taking precautions is unsafe.
For example:

.. code-block:: c++

   #include <vector>

   std::vector<atermpp::aterm> v;             // This is unsafe!
   v.push_back(atermpp::make_term("f(x)");
   v.push_back(atermpp::make_term("g(y)");

Once the garbage collector is triggered, all terms inside `v` will be destroyed. For
this reason, protected variants of most common containers exist. By default, only
the predefined aterm types are protected inside such a container. For user defined
terms, extra precautions are needed.

.. code-block:: c++

   #include "atermpp/vector.h"

   atermpp::vector<atermpp::aterm> v;         // This is OK!
   v.push_back(atermpp::make_term("f(x)");
   v.push_back(atermpp::make_term("g(y)");

Iterator interfaces
"""""""""""""""""""
The classes `term_list` and `term_appl` have C++ standard conforming iterator interfaces.
Thus they operate well with the C++ Standard Library, as illustrated by the following
example:

.. code-block:: c++

    #include <algorithm>
    #include <iostream>
    #include "atermpp/atermpp.h"
    #include "mcrl2/atermpp/aterm_init.h"

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
      MCRL2_ATERMPP_INIT()

      term_list<aterm_int> q = make_term("[1,2,3,4]");
      int sum = 0;
      for_each(q.begin(), q.end(), counter(sum));
      assert(sum == 10);

      for (term_list<aterm_int>::iterator i = q.begin(); i != q.end(); ++i)
      {
        cout << i->value() << " ";
      }
    }

User defined terms
""""""""""""""""""

Suppose we want to create a class `MyTerm` that has an ATerm as attribute:

.. code-block:: c++

   struct MyTerm
   {
     atermpp::aterm x_;

     MyTerm(std::string x)
       : x_(atermpp::make_term(x))
     { }
   };

To make this class usable within the `atermpp` library, we must specify how
to protect it from garbage collection, and how to obtain an ATerm representation
of the term. For this the `aterm_traits` class must be used.

ATerm traits
^^^^^^^^^^^^
In the `atermpp` a class is considered a term if a specialization of the class
`aterm_traits` exists for it. This is a traits class that describes how the
specialized type can be protected from garbage collection and how an ATerm
can be obtained from it. For the class `MyTerm`, the specialization looks like
this:

.. code-block:: c++

   namespace atermpp
   {
     template<>
     struct aterm_traits<MyTerm>
     {
       typedef ATermAppl aterm_type;
       static void protect(MyTerm t)   { t.x_.protect(); }   // protect the term against garbage collection
       static void unprotect(MyTerm t) { t.x_.unprotect(); } // undo the protection against garbage collection
       static void mark(MyTerm t)      { t.x_.mark(); }      // mark the term for not being garbage collected
                                                             // when it is inside a protected container
       static ATerm term(MyTerm t)     { return t.term(); }  // return the ATerm corresponding to t
       static ATerm* ptr(MyTerm& t)    { return &t.term(); } // return the address of the ATerm corresponding to t
     };
   } // namespace atermpp

Now that we have defined `aterm_traits<MyTerm>`, it is safe to use `MyTerm` in a protected container:

.. code-block:: c++

   #include "atermpp/vector.h"

   atermpp::vector<MyTerm> v;
   v.push_back(MyTerm("f(x)");
   v.push_back(MyTerm("g(y)");

Also the search and replace algorithms of section
[link aterm___library.programming.algorithms ATerm algorithms]
can be applied to `MyTerm`.

ATerm algorithms
^^^^^^^^^^^^^^^^

For the `atermpp` library has a couple of algorithms are defined. Most
of these algorithms have template parameters for the terms that they
operate on. These algorithms work on every class for which an `aterm_traits`
specialization exists.

Find algorithms
"""""""""""""""
There are two find algorithms, `find_if` for searching a subterm that matches a
given predicate, and `find_all_if` for finding all subterms that match a
predicate. The program fragment below illustrates this:

.. code-block:: c++

  #include "mcrl2/atermpp/algorithm.h"

  // function object to test if it is an aterm_appl with function symbol "f"
  struct is_f
  {
    bool operator()(aterm t) const
    {
      return (t.type() == AT_APPL) && aterm_appl(t).function().name() == "f";
    }
  };

  aterm_appl a = make_term("h(g(x),f(y),p(a(x,y),q(f(z))))");
  aterm t = find_if(a, is_f());
  assert(t == make_term("f(y)"));

  find_all_if(a, is_f(), std::back_inserter(v));
  assert(v.size() == 2);
  assert(v.front() == make_term("f(y)"));
  assert(v.back() == make_term("f(z)"));

The find algorithms also work on user defined types. So if `t` is of type `MyTerm`
and `aterm_traits<MyTerm>` is defined, then it is possible to call `find_if(t, is_f())`
as well.

Replace algorithms
""""""""""""""""""
There are several algorithms for replacing subterms. The `replace` algorithm replaces
a subterm with another term, `bottom_up_replace` does the same but with a different traversal
order. The algorithm `replace_if` makes replacements based on a predicate. There is also
`partial_replace`, that has the option to prevent further recursion based on a predicate.

.. code-block:: c++

  #include "atermpp/algorithm.h"

  // function object to test if it is an aterm_appl with function symbol "a" or "b"
  struct is_a_or_b
  {
    bool operator()(aterm t) const
    {
      return (t.type() == AT_APPL) &&
      (aterm_appl(t).function().name() == "a" || aterm_appl(t).function().name() == "b");
    }
  };

  aterm_appl a = make_term("f(f(x))");
  aterm_appl b = replace(a, make_term("f(x)"), make_term("x"));
  assert(b == make_term("f(x)"));
  aterm_appl c = replace(a, make_term("f(x)"), make_term("x"), true);
  assert(c == make_term("x"));

  aterm d = make_term("h(g(b),f(a),p(a(x,y),q(a(a))))");
  aterm_appl e = replace_if(d, is_a_or_b(), make_term("u"));
  assert(e == make_term("h(g(u),f(u),p(u,q(u)))"));

Miscellaneous algorithms
""""""""""""""""""""""""

The algorithm `apply` applies an operation to the elements
of a list, and returns the result. The `for_each` algorithm applies
an operation to each subterm of a term.

.. code-block:: c++

   #include "atermpp/algorithm.h"

   // Applies a function f to the given argument t.
   struct apply_f
   {
     aterm_appl operator()(aterm_appl t) const
     {
       return aterm_appl(function_symbol("f", 1), t);
     }
   };

   bool print(aterm_appl t) // The return value true indicates that for_each
                              // should recurse into the children of t.
   {
     std::cout << t.function().name() << " ";
     return true;
   }

   aterm_appl t = make_term("h(g(x),f(y))");
   atermpp::for_each(t, print);             // prints "h g x f y"

   aterm_list l = make_term("[0,1,2,3]");
   l = atermpp::apply(l, apply_f());        // results in [f(0),f(1),f(2),f(3)]

Compiler optimizations
^^^^^^^^^^^^^^^^^^^^^^

One should be very careful with choosing optimization flags when compiling
code using the ATerm Library. It is reported that the -O3 flag of the g++ compiler
may result in optimizations that make the garbage collector fail, resulting in
unexpected garbage collection of terms.

.. warning::
   
   Using the g++ compiler, the -O3 flag should not be used.


