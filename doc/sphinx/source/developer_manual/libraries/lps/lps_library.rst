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
* :math:`E_{i}` is a sort, and the elements :math:`e` of :math:`E_{i}` are called *summation variables*.
* :math:`c_{i}` is a boolean term, called the *condition*
* :math:`a_{i}` is a term called the *action*
* :math:`t_{i}` is a real valued term, called the *time*
* :math:`g_{i}` is an *assignment* function to data variables, with the following interpretation. The expression :math:`g_{i}(d,e)` is a state that can be reached from state :math:`d` by performing the action :math:`a_{i}` at time :math:`t_{i}`.

A linear process usually has an accompanying initial value :math:`d_0:D`.

.. note::

   The time :math:`t_{i}` and the condition :math:`c_{i}` are optional.
   The action :math:`a_{i}(d,e)` can have the value :math:`\delta`, corresponding to deadlock.

Specification
-------------
A process can be defined using a (textual) mCRL2 specification. A typical
process specification is the following:

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

This process can be stored in an instance of the class ``lps::specification`` after linearizing it
using

.. code-block:: c++

    std::string text = ... ;
    specification spec = mcrl22lps(text);

The class ``lps::specification`` represents a linearized process specification. It consists of

* a linear process
* a data specification, i.e. a specification of the sorts, constructors, mappings and equations
* an action specification, i.e. a sequence of actions that may occur during execution of the process
* an initial state
* global variables

Once we have such a specification, it can be stored in and read from a file,
either in binary or ascii format. This is illustrated by the following example.

.. code-block:: c++

    specification spec;
    try {
      spec.load("abp.lps");
    }
    catch(mcrl2::runtime_error e)
    {
      std::cerr << "load failed!" << std::endl;
    }
    try {
      spec.save("abp.lps");
    }
    catch(mcrl2::runtime_error e)
    {
      std::cerr << "save failed!" << std::endl;
    }

Linear processes
----------------
An instance of ``lps::specification`` contains a linear process of type ``lps::linear_process``. This linear
process in turn contains a sequence of summands, and an initial state.
Note that the implementation distinguishes between `action summands` and `deadlock summands` (i.e. those
summands containing a :math:`\delta`).

.. code-block:: c++

    const linear_process& proc = spec.process();
    const atermpp::vector<action_summand>& as = proc.action_summands();
    const atermpp::vector<deadlock_summand>& ds = proc.deadlock_summands();
    const process_initializer& init = proc.init();

Both classes ``action_summand`` and ``deadlock_summand`` have an optional attribute time.
It is necessary to check if the time is available, before using it:

.. code-block:: c++

    action_summand s = proc.action_summands().front();
    if (s.has_time())
    {
      std::cout << "time = " << data::pp(s.time()) << std::endl;
    }

Action summands have an associated multi action, which consists of a sequence of actions.
An illustration of it's usage is

.. code-block:: c++

    action_summand s;
    action_list al = s.actions();
    for (action_list::const_iterator i = al.begin(); i != al.end(); ++i)
    {
      action a = *i;
      core::identifier_string name = a.label().name();
      data::data_expression_list arguments = a.arguments();
    }

Here ``name`` corresponds with :math:`a_{i}`, and ``arguments`` with :math:`f_i(d,e)` in
the earlier given formulas.

.. note::

   There is a convention that a linear process without any summands represents the process ``delta @ 0``.

.. warning::

   There is still a class ``lps::summand`` in use, but it has been deprecated.

Classes in the LPS library
==========================
Several classes in the LPS library are just thin wrappers around an ATerm pointer (see also the :ref:`atermpp_library`).
This means that instances of these classes are immutable, and instances with the same
value are shared in memory. The following table gives an overview of the ATerm based classes:

============================== =
   ATerm based classes
============================== =
 ``lps::action_label``
 ``lps::action_label_list``
 ``lps::action``
 ``lps::action_list``
 ``lps::process_initializer``
============================== =

Correctness checks
------------------
The ATerms that are internally used in the classes of the LPS library have to
adhere to a grammar that can be found in the file ``doc/specs/mcrl2.internal.txt``.
In debug mode, all constructors of the classes will automatically check if the
terms are in the correct format. If not, an assertion failure is triggered.
In release mode, these checks are switched off, for efficiency reasons.

For many classes there are additional restrictions to what terms are considered
valid, the so called well typedness constraints. These constraints are implemented
in the class ``lps/detail/lps_well_typed_checker.h``. For example, the following
checks are done for linear processes:

  * process parameters have unique names
  * process parameters and summation variables have different names
  * the left hand sides of the assignments of summands are contained in the process parameters

Such constraints are only checked in debug mode in the `load` and `save` functions of
``lps::specification``. The descriptions of the well typedness constraints are found in the
reference documentation.

Algorithms
==========
In the mCRL2 tool set there are many algorithms available for manipulating
linear process specifications. Some of them are available as an algorithm.
For example:

.. code-block:: c++

    specification spec;
    spec.load("abp.lps");
    spec = parelm(spec);    // elimination of insignificant process parameters
    spec = constelm(spec);  // elimination of constant process parameters
    spec = sumelm(spec);    // apply sum elimination theorem

Similar to the Data Library there is a whole range of search and replace functions
available. An example of their usage is:

.. code-block:: c++

  specification spec = parse_linear_process_specification(
    "glob m: Nat;                 \n"
    "act a: Nat;                  \n"
    "proc P(n:Nat) = a(m).P(n+1); \n"
    "init P(0);                   \n"
  );

  data::variable m(core::identifier_string("m"), data::sort_nat::nat());
  data::variable n(core::identifier_string("n"), data::sort_nat::nat());
  data::variable p(core::identifier_string("p"), data::sort_nat::nat());
  data::variable q(core::identifier_string("q"), data::sort_nat::nat());

  std::set<data::variable> v;
  v = lps::find_variables(spec.process());            // v = { m: Nat, n: Nat }
  v = lps::find_free_variables(spec.process());       // v = { m: Nat }

  data::mutable_map_substitution<> sigma;
  sigma[m] = p;
  sigma[n] = q;
  lps::replace_free_variables(spec.process(), sigma); // spec.process() =  "P(n: Nat) = a(p).P(n+1)"
