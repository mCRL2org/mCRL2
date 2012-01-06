What is an ATerm?
=================

ATerms are hierarchical terms composed of two building blocks: lists and function applications.
Simple examples of these are `[0,1,2]` and `f(x,y)`. Terms can be nested arbitrarily, take for
example `[0,[f(1),2]]` or `f(g(x,[a,b]),h(z))`.

ATerm types
-----------

There are 6 predefined ATerm types, as shown in the following table.
The class :cpp:class:`aterm <atermpp::aterm>` is a base class for all others.

.. table:: atermpp terms

   ===========================================================  =============================
   type                                                         description
   ===========================================================  =============================
   :cpp:class:`aterm <atermpp::aterm>`                          a generic aterm
   :cpp:class:`term_list <atermpp::term_list>`                  a list of terms
   :cpp:class:`term_appl <atermpp::term_appl>`                  a function application
   :cpp:class:`aterm_int <atermpp::aterm_int>`                  a term containing an int
   :cpp:class:`aterm_real <atermpp::aterm_real>`                a term containing a real
   :cpp:class:`aterm_blob <atermpp::aterm_blob>`                a term containing binary data
   :cpp:class:`aterm_placeholder <atermpp::aterm_placeholder>`  a place holder for aterms
   ===========================================================  =============================


The types :cpp:class:`term_list <atermpp::term_list>` and :cpp:class:`term_appl <atermpp::term_appl>` are template classes. Typedefs exist
for the most commonly used variants:

.. code-block:: c++

  typedef term_list<aterm> aterm_list;
  typedef term_appl<aterm> aterm_appl;

A :cpp:class:`term_list\<T\> <atermpp::term_list>` models a read-only singly linked list with elements of type
:cpp:type:`T`. The element type should be :cpp:class:`aterm <atermpp::aterm>` or one of its derivatives, or a user
defined aterm (see section [link aterm___library.programming.user_defined User defined terms]).
A :cpp:class:`term_appl\<T\> <atermpp::term_appl>` is a function application with leafs of type :cpp:type:`T`.

Besides this the library contains a few more data types that use
ATerms internally.

.. table:: additional data types
   
   ===========  ==============================
   type         description
   ===========  ==============================
   dictionary   a dictionary based on aterms
   table        a table based on aterms
   indexed_set  an indexed set based on aterms
   ===========  ==============================

ATerm properties
----------------
The aterms in the ATerm++ Library have some properties that need to be understood
to use the library effectively. The aterm classes all wrap pointers to ATerm objects
of the underlying ATerm Library. Copying an aterm is thus a very cheap operation.

ATerm sharing
^^^^^^^^^^^^^
The most important feature of the ATerm Library is that subterms are shared in memory. Consider
the terms `f(x,g(y))` and `h(f(x,g(y)),g(y))`. The ATerm Library makes sure that the subterm
`f(x,g(y))` will only appear once in memory. Even the term `g(y)` exists only once. In the
case of lists, the ATerm Library shares only the tail parts. For example, from the lists
`[0,1,2,3]` and `[0,1,2,4,1,2,3]` the tail part `[1,2,3]` will appear only once in memory.
The front parts `[0,1,2]` will *not* be shared.

Contrary to what one might expect, the argument lists of function applications are not
shared. So for the terms `f(a,b,c)` and `g(a,b,c)` the sequences of arguments `[a,b,c]`
are stored in separate locations.

Read-only terms
^^^^^^^^^^^^^^^

As a result of the maximal sharing, ATerms have the property that they are read-only.
All member functions of the `atermpp` classes (except the assignment operator) are constant.

.. note::
   
   All aterm objects are constant. Whenever you want to
   modify an attribute of an aterm, a new object has to be created.

Needless to say that this has a significant effect on the way ATerms are used.

Type conversions
^^^^^^^^^^^^^^^^

In the underlying ATerm Library, terms are usually represented using generic
`ATerm` pointers, and casted back and forth to specific ATerm types using
unsafe C-style type casts. To make the `atermpp` layer play well with the
ATerm Library, this behavior has partially been copied. The class `aterm` is
a base class for all other aterm types. Using the assignment operator, terms
may be converted into each other:

.. code-block:: c++

    aterm_int x(10);
    aterm y = x;
    aterm_int z = y;                    // will be type-checked in debug mode
    assert(z.value() == 10);
    aterm_appl f = make_term("f(x,y)"); // will be type-checked in debug mode

In the third and fifth statement the user is responsible for assuring
that the right hand side of the assignment has the right type. If the types
don't match, in debug mode an assertion failure will be generated. In release
mode, the behavior is undefined.

There is a lot of C++-code written in terms of the ATerm Library, containing
:cpp:type:`ATerm <aterm::ATerm>`, :cpp:type:`ATermAppl <aterm::ATermAppl>` and :cpp:type:`ATermList <aterm::ATermList>` pointers in the interface.
The `atermpp` library integrates well with this code, since the types :cpp:class:`aterm <atermpp::aterm>`,
:cpp:type:`aterm_appl <atermpp::aterm_appl>` and
:cpp:type:`aterm_list <atermpp::aterm_list>` have conversion operators for these types. For example, if
we take a function with signature :cpp:func:`void f(ATermList l, ATerm x)`, it can be
used like this:

.. code-block:: c++

    aterm_list l;
    aterm x;
    f(l,x);

String representations
^^^^^^^^^^^^^^^^^^^^^^

The predefined ATerm types have a `to_string` member function that can be used
to obtain a string representation of a term:

.. code-block:: c++

    aterm_int x;
    std::string s = x.to_string();

In most cases this string can be converted back to an ATerm using the :cpp:func:`make_term <atermpp::make_term>`
function. However, in some subtle cases the result will not be the same.

Comparing ATerms
^^^^^^^^^^^^^^^^
Comparing ATerms can be done with the `==` operator. Due to the maximal sharing property,
comparing ATerms is a cheap operation. It boils down to a pointer comparison.


