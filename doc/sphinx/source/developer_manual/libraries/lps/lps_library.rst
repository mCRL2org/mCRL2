Linear process specifications
=============================

The LPS library contains classes and algorithms for linear processes. The code
in the LPS library is contained in the namespace =lps=. LPS is shorthand for
linear process specification.

Definitions
-----------
A *linear process* is defined as

.. math::  P(d:D)=\sum\limits_{i\in I}s_{i}(d)

with

.. math:: s_{i}(d)=\sum_{e:E_{i}}c_{i}(d,e)\rightarrow a_{i}(d,e)^@t_{i}(d,e)\cdot P(g_{i}(d,e))

and

:math:`a_{i}(d,e) = a_{i}^1(f_{i}^1(d,e)) \mid \ldots \mid a_{i}^n(f_{i}^n(d,e))` or :math:`\delta`

where

* :math:`d` is a vector of data variables, called the *process parameters*. The corresponding vector of sorts :math:`D` models the states of the process.
* :math:`s_{i}` is a linear process term, called a *summand*
* :math:`E_{i}` is a sort, and the elements :math:`e` of :math:`E_{i}` are called *summation variables*. [footnote The tool set requires that there are only finitely many values /e/ for which __ci has the value true. ]
* :math:`c_{i} is a boolean term, called the *condition*
* :math:`a_{i}` is a term called the *action*
* :math:`t_{i}` is a real valued term, called the *time*
* :math:`g_{i}` is an *assignment* function to data variables, with the following interpretation. The expression :math:`g_{i}(d,e)` is a state that can be reached from state :math:`d` by performing the action :math:`a_{i}` at time :math:`t_{i}`.

A linear process usually has an accompanying initial value :math:`d0:D`.

.. note::

   The time :math:`t_{i}` and the condition :math:`c_{i}` are optional.
   The action :math:`a_{i}` can assume the value :math:`\delta`, corresponding to deadlock.

.. note::

   Both the summands of a linear process and the initial state can have so called
   *free variables*. Free variables do not appear in the process parameters :math:`d` or
   the summation variables :math:`e:E_{i}`. Moreover, for each value of a free variable, the
   resulting systems are by definition bisimulation equivalent.

For more information about linear processes, see the __mcrl2_primer__ .

Specification
-------------
A process can be defined using a (textual) mCRL2 specification. A typical
specification is the following:

.. code-block:: mcrl2

   sort
     D     = struct d1 | d2;
     Error = struct e;
   
   act
     r1,s4: D;
     s2,r2,c2: D # Bool;
     s3,r3,c3: D # Bool;
     s3,r3,c3: Error;
     s5,r5,c5: Bool;
     s6,r6,c6: Bool;
     s6,r6,c6: Error;
     i;
   
   proc
     S(b:Bool)     = sum d:D. r1(d).T(d,b);
     T(d:D,b:Bool) = s2(d,b).(r6(b).S(!b)+(r6(!b)+r6(e)).T(d,b));
   
     R(b:Bool)     = sum d:D. r3(d,b).s4(d).s5(b).R(!b)+
                     (sum d:D.r3(d,!b)+r3(e)).s5(!b).R(b);
   
     K             = sum d:D,b:Bool. r2(d,b).(i.s3(d,b)+i.s3(e)).K;
   
     L             = sum b:Bool. r5(b).(i.s6(b)+i.s6(e)).L;
   
   init
     allow({r1,s4,c2,c3,c5,c6,i},
       comm({r2|s2->c2, r3|s3->c3, r5|s5->c5, r6|s6->c6},
           S(true) || K || L || R(true)
       )
     );

There is no class in the LPS library available that can represent
this specification directly. It has to be linearized first, and the result
of that can be stored in an `lps::specification`. The class
`lps::specification` represents a linearized process specification.
It consists of

* a linear process
* a data specification, i.e. a specification of the sorts, constructors, mappings and equations
* an action specification, i.e. a sequence of actions that may occur during execution of the process
* an initial state

If we assume that the textual specification above is stored in the string =text=,
then it can be converted into an `lps::specification` as follows:

.. code-block:: c++

    lps::specification spec = lps::mcrl22lps(text);

Once we have such a specification, it can be stored in and read from a file,
either in binary or ascii format. This is illustrated by the following example.

.. code-block:: c++

    lps::specification spec;
    try {
      spec.load("abp.lps");
    }
    catch(std::runtime_error e)
    {
      std::cerr << "load failed!" << std::endl;
    }
    try {
      bool binary = true;
      spec.save("abp.lps", binary);
    }
    catch(std::runtime_error e)
    {
      std::cerr << "save failed!" << std::endl;
    }

Linear processes
----------------
A linear process is represented by the class `lps::linear_process`, whereas
the initial state of a process is represented by `lps::process_initializer`.
Both have their own declaration of free variables:

.. code-block:: c++

    linear_process p = spec.process();
    data_variable_list pvars = p.free_variables();

    process_initializer init = spec.initial_state();
    data_variable_list initvars = init.free_variables();

.. note::

   In an `lps::specification`, the sequences of free variables
   of the linear process and of the initial state may be different.

summands
^^^^^^^^
A summand of a linear process is represented by the class `lps::summand`.

.. note::

   There are a few restrictions on the usage of summands.
   The condition of a summand may be nil, in which case it must be ignored.
   The time of a summand is optional, so before using it one must check that it exists.

actions
^^^^^^^
An action is represented by the class `lps::action`. An action has two
attributes. It has a name (:math:`a_{i}`), which is a label that may be shared with other
actions. And it has a parameter, which is the vector of data expressions :math:`f_i(d,e)`.
In a summand, a multi-action is stored as a sequence of actions.

The following example illustrates the concepts of an `lps::summand`.

.. code-block:: c++

    linear_process p;
    summand s = p.summands().front(); // the first summand

    if (s.has_time())
    {
      std::cout << "time = " << pp(s.time()) << std::endl;
    }

    if (data_expr::is_nil(s.condition()))
    {
      std::cout << "the summand has no condition!" << std::endl;
    }
    else
    {
      std::cout << "condition = " << pp(s.condition()) << std::endl;
    }

    if (s.is_delta())
    {
      std::cout << "the summand is a delta summand!" << std::endl;
    }
    else
    {
      std::cout << "the multi-action has " << s.actions().size() << " actions." << std::endl;
    }

Classes in the LPS library
==========================
The classes in the LPS library are internally represented by ATerms (see the
:ref:`atermpp_library`). Such terms are read-only, and as a result most of the classes
in the LPS library are read-only too.
Sequences in the classes support a C++ iterator interface, with the restriction
that they are read-only singly linked lists. Classes can be pretty printed
using the `pp` function. An example of this is:

.. code-block:: c++

    lps::specification spec;
    lps::linear_process p = spec.process();
    for (lps::linear_process::summand_iterator i = p.summands().begin(); i != p.summands().end(); ++i)
    {
      std::cout << pp(*i) << std::endl; // pp is the pretty print function
    }

Correctness checks
------------------
The ATerms that are internally used in the classes of the LPS library have to
adhere to a grammar that can be found in the file =doc/specs/mcrl2.internal.txt=.
In debug mode, all constructors of the classes will automatically check if the
terms are in the correct format. If not, an assertion failure is triggered.
In release mode, these checks are switched off, for efficiency reasons.

For many classes there are restrictions to what terms are considered valid,
the so called well typedness constraints. These constraints can be checked
using the `is_well_typed` member functions. For example, the function
`linear_process::is_well_typed` checks the following constraints:

  * the free variables occurring in the process are declared in `free_variables()`
  * the process parameters have unique names
  * the free variables have unique names
  * process parameters and summation variables have different names
  * the left hand sides of the assignments of summands are contained in the process parameters

Such constraints are only checked in the `load` and `save` functions of
`lps::specification`. The descriptions of the well typedness constraints
are found in the reference documentation.

Search and replace
------------------
Since most classes in the LPS library are internally represented by ATerms,
the search and replace algorithms of the :ref:`atermpp_library` can be applied to
them. An example of this is the following:

.. code-block:: c++

    specification spec1;
    data_variable v("v", sort_expr::nat());
    data_variable w("w", sort_expr::nat());

    // replace all occurrences of `v:Nat` with `w:Nat` in `spec1`
    specification spec2 = atermpp::replace(spec1, v, w);

Algorithms
==========
In the mCRL2 tool set there are many algorithms available for manipulating
linear process specifications. Some of them are available as an algorithm.
For example:

.. code-block:: c++

    lps::specification spec;
    spec.load("abp.lps");
    spec = parelm(spec);    // elimination of insignificant process parameters
    spec = constelm(spec);  // elimination of constant process parameters
    spec = sumelm(spec);    // apply sum elimination theorem
